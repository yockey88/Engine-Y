/**
 * \file win+_crash_handler.cpp
 * \author Y
 * \brief crash handler to show stack trace on Windows
 * \details This is based off of Godot's CrashHandlerException function which in turn is
 *              inspired by https://stackoverflow.com/questions/6205981/windows-c-stack-trace-from-a-running-app
*/

#include "platform/win_crash_handler.hpp"

/**
 * Code below is from Godot Engine
 * https://github.com/godotengine/godot/blob/master/platform/windows/crash_handler_windows.cpp
*/

#ifdef CRASH_HANDLER_EXCEPTION 1

#include <psapi.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// Some versions of imagehlp.dll lack the proper packing directives themselves
// so we need to do it.
#pragma pack( push, before_imagehlp, 8 )
#include <imagehlp.h>
#pragma pack( pop, before_imagehlp )

struct module_data {
	std::string image_name;
	std::string module_name;
	void *base_address = nullptr;
	DWORD load_size;
};

class symbol {
    typedef IMAGEHLP_SYMBOL64 sym_type;
    sym_type* sym;
    static const int max_name_len = 1024;

    public:
        symbol(HANDLE process , DWORD64 address) :
            sym((sym_type*)::operator new(sizeof(*sym) + max_name_len)) {

            memset(sym , '\0' , sizeof(*sym) + max_name_len);
            sym->SizeOfStruct = sizeof(*sym);
            sym->MaxNameLength = max_name_len;
            DWORD64 displacement;

            SymGetSymFromAddr64(process , address , &displacement , sym);
        }

        std::string name() { return std::string(sym->Name); }
        std::string undecorated_name() {
            if (*sym->Name == '\0') {
                return "<couldn't map PC to fn name>";
            }
            std::vector<char> und_name(max_name_len);
            UnDecorateSymbolName(sym->Name , und_name.data() , max_name_len , UNDNAME_COMPLETE);
            return std::string(und_name.data() , strlen(und_name.data()));
        }
};

class get_mod_info {
    HANDLE process;
    
    public:
        get_mod_info(HANDLE h)
            : process(h) {}

        module_data operator()(HMODULE m) {
            module_data ret;
            char temp[4096];
            MODULEINFO mi;

            GetModuleInformation(process , m , &mi , sizeof(mi));
            ret.base_address = mi.lpBaseOfDll;
            ret.load_size = mi.SizeOfImage;

            GetModuleFileNameEx(process , m , (LPWSTR)temp , sizeof(temp));
            ret.image_name = temp;
            GetModuleBaseName(process , m , (LPWSTR)temp , sizeof(temp));
            ret.module_name = temp;

            std::vector<char> img(ret.image_name.begin() , ret.image_name.end());
            std::vector<char> mod(ret.module_name.begin() , ret.module_name.end());

            SymLoadModule64(process , 0 , img.data() , mod.data() , (DWORD64)ret.base_address , ret.load_size);
            return ret;
        }
};

DWORD CrashHandlerException(EXCEPTION_POINTERS* ep) {
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    DWORD offset_from_symbol = 0;
    IMAGEHLP_LINE64 line = { 0 };
    std::vector<module_data> modules;
    DWORD cbNeeded;
    std::vector<HMODULE> module_handles(1);

    /// \todo Check if we are currently checking for crashes right now
    ///         always need this during dev

    // YE_ERR_LOG(__FUNCTION__ crashed);

    if (!SymInitialize(process , nullptr , false)) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    SymSetOptions(SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_EXACT_SYMBOLS);
    EnumProcessModules(process , module_handles.data() , module_handles.size() * sizeof(HMODULE) , &cbNeeded);
    module_handles.resize(cbNeeded / sizeof(HMODULE));
    EnumProcessModules(process , module_handles.data() , module_handles.size() * sizeof(HMODULE) , &cbNeeded);
    std::transform(module_handles.begin() , module_handles.end() , std::back_inserter(modules) , get_mod_info(process));
    void* base = modules[0].base_address;

    CONTEXT* context = ep->ContextRecord;
    STACKFRAME64 frame;
    bool skip_first = false;

	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrStack.Mode = AddrModeFlat;
	frame.AddrFrame.Mode = AddrModeFlat;

#if defined(_M_X64)
	frame.AddrPC.Offset = context->Rip;
	frame.AddrStack.Offset = context->Rsp;
	frame.AddrFrame.Offset = context->Rbp;
#elif defined(_M_ARM64) || defined(_M_ARM64EC)
	frame.AddrPC.Offset = context->Pc;
	frame.AddrStack.Offset = context->Sp;
	frame.AddrFrame.Offset = context->Fp;
#elif defined(_M_ARM)
	frame.AddrPC.Offset = context->Pc;
	frame.AddrStack.Offset = context->Sp;
	frame.AddrFrame.Offset = context->R11;
#else
	frame.AddrPC.Offset = context->Eip;
	frame.AddrStack.Offset = context->Esp;
	frame.AddrFrame.Offset = context->Ebp;

	// Skip the first one to avoid a duplicate on 32-bit mode
	skip_first = true;
#endif

	line.SizeOfStruct = sizeof(line);
	IMAGE_NT_HEADERS *h = ImageNtHeader(base);
	DWORD image_type = h->FileHeader.Machine;

    /// \todo change these printf to Logging functions
    int n = 0;
	do {
		if (skip_first) {
			skip_first = false;
		} else {
			if (frame.AddrPC.Offset != 0) {
				std::string fnName = symbol(process, frame.AddrPC.Offset).undecorated_name();

				if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &offset_from_symbol, &line)) {
					printf("[%d] %s (%s:%d)\n", n, fnName.c_str(), (char *)line.FileName, (int)line.LineNumber);
				} else {
					printf("[%d] %s\n", n, fnName.c_str());
				}
			} else {
				printf("[%d] ???\n", n);
			}

			n++;
		}

		if (!StackWalk64(image_type, process, thread, &frame, context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr)) {
			break;
		}
	} while (frame.AddrReturn.Offset != 0 && n < 256);

    SymCleanup(process);

    return EXCEPTION_CONTINUE_SEARCH;
}

#endif