#include "parsing/shader/shader_parser.hpp"

#include <filesystem>
#include <fstream>

#include "core/log.hpp"
#include "core/filesystem.hpp"

namespace EngineY {

    bool GlslParser::Match(const std::vector<GlslTokenType>& types) {
        for (auto& type : types) {
            if (Check(type)) {
                Advance();
                return true;
            }
        }
        return false;
    }

    bool GlslParser::IsValidReturnType() {
        return Match({ GlslTokenType::TVOID , GlslTokenType::BOOL , GlslTokenType::INT ,
                       GlslTokenType::UINT , GlslTokenType::FLOAT , GlslTokenType::DOUBLE ,  
                       GlslTokenType::VEC2 , GlslTokenType::VEC3 , GlslTokenType::VEC4 , 
                       GlslTokenType::DVEC2 , GlslTokenType::DVEC3 , GlslTokenType::DVEC4 ,
                       GlslTokenType::BVEC2 , GlslTokenType::BVEC3 , GlslTokenType::BVEC4 ,
                       GlslTokenType::IVEC2 , GlslTokenType::IVEC3 , GlslTokenType::IVEC4 ,
                       GlslTokenType::MAT2 , GlslTokenType::MAT3 , GlslTokenType::MAT4  ,
                       GlslTokenType::MAT2X2 , GlslTokenType::MAT2X3 , GlslTokenType::MAT2X4 ,
                       GlslTokenType::MAT3X2 , GlslTokenType::MAT3X3 , GlslTokenType::MAT3X4 ,
                       GlslTokenType::MAT4X2 , GlslTokenType::MAT4X3 , GlslTokenType::MAT4X4 ,
                       GlslTokenType::DMAT2 , GlslTokenType::DMAT3 , GlslTokenType::DMAT4 ,
                       GlslTokenType::DMAT2X2 , GlslTokenType::DMAT2X3 , GlslTokenType::DMAT2X4 ,
                       GlslTokenType::DMAT3X2 , GlslTokenType::DMAT3X3 , GlslTokenType::DMAT3X4 ,
                       GlslTokenType::DMAT4X2 , GlslTokenType::DMAT4X3 , GlslTokenType::DMAT4X4 ,
                       GlslTokenType::SAMPLER1D , GlslTokenType::TEXTURE1D , GlslTokenType::IMAGE1D ,
                       GlslTokenType::SAMPLER1DSHADOW ,
                       GlslTokenType::SAMPLER1DARRAY , GlslTokenType::TEXTURE1DARRAY , GlslTokenType::IMAGE1DARRAY ,
                       GlslTokenType::SAMPLER1DARRAYSHADOW ,
                       GlslTokenType::SAMPLER2D , GlslTokenType::TEXTURE2D , GlslTokenType::IMAGE2D ,
                       GlslTokenType::SAMPLER2DSHADOW ,
                       GlslTokenType::SAMPLER2DARRAY , GlslTokenType::TEXTURE2DARRAY , GlslTokenType::IMAGE2DARRAY ,
                       GlslTokenType::SAMPLER2DARRAYSHADOW ,
                       GlslTokenType::SAMPLER2DMS , GlslTokenType::TEXTURE2DMS , GlslTokenType::IMAGE2DMS ,
                       GlslTokenType::SAMPLER2DMSARRAY , GlslTokenType::TEXTURE2DMSARRAY , GlslTokenType::IMAGE2DMSARRAY ,
                       GlslTokenType::SAMPLER2DRECT , GlslTokenType::TEXTURE2DRECT , GlslTokenType::IMAGE2DRECT ,
                       GlslTokenType::SAMPLER2DRECTSHADOW ,
                       GlslTokenType::SAMPLER3D , GlslTokenType::TEXTURE3D , GlslTokenType::IMAGE3D ,
                       GlslTokenType::SAMPLERCUBE , GlslTokenType::TEXTURECUBE , GlslTokenType::IMAGECUBE ,
                       GlslTokenType::SAMPLERCUBESHADOW ,
                       GlslTokenType::SAMPLERCUBEARRAY , GlslTokenType::TEXTURECUBEARRAY , GlslTokenType::IMAGECUBEARRAY ,
                       GlslTokenType::SAMPLERCUBEARRAYSHADOW ,
                       GlslTokenType::SAMPLERBUFFER , GlslTokenType::TEXTUREBUFFER , GlslTokenType::IMAGEBUFFER ,
                       GlslTokenType::SUBPASSINPUT , GlslTokenType::SUBPASSINPUTMS ,
                       GlslTokenType::ISAMPLER1D , GlslTokenType::ITEXTURE1D , GlslTokenType::IIMAGE1D ,
                       GlslTokenType::ISAMPLER1DARRAY , GlslTokenType::ITEXTURE1DARRAY , GlslTokenType::IIMAGE1DARRAY ,
                       GlslTokenType::ISAMPLER2D , GlslTokenType::ITEXTURE2D , GlslTokenType::IIMAGE2D ,
                       GlslTokenType::ISAMPLER2DARRAY , GlslTokenType::ITEXTURE2DARRAY , GlslTokenType::IIMAGE2DARRAY ,
                       GlslTokenType::ISAMPLER2DMS , GlslTokenType::ITEXTURE2DMS , GlslTokenType::IIMAGE2DMS ,
                       GlslTokenType::ISAMPLER2DMSARRAY , GlslTokenType::ITEXTURE2DMSARRAY , GlslTokenType::IIMAGE2DMSARRAY ,
                       GlslTokenType::ISAMPLER2DRECT , GlslTokenType::ITEXTURE2DRECT , GlslTokenType::IIMAGE2DRECT ,
                       GlslTokenType::ISAMPLER3D , GlslTokenType::ITEXTURE3D , GlslTokenType::IIMAGE3D ,
                       GlslTokenType::ISAMPLERCUBE , GlslTokenType::ITEXTURECUBE , GlslTokenType::IIMAGECUBE ,
                       GlslTokenType::ISAMPLERCUBEARRAY , GlslTokenType::ITEXTURECUBEARRAY , GlslTokenType::IIMAGECUBEARRAY ,
                       GlslTokenType::ISAMPLERBUFFER , GlslTokenType::ITEXTUREBUFFER , GlslTokenType::IIMAGEBUFFER ,
                       GlslTokenType::ISUBPASSINPUT , GlslTokenType::ISUBPASSINPUTMS ,
                       GlslTokenType::USAMPLER1D , GlslTokenType::UTEXTURE1D , GlslTokenType::UIMAGE1D ,
                       GlslTokenType::USAMPLER1DARRAY , GlslTokenType::UTEXTURE1DARRAY , GlslTokenType::UIMAGE1DARRAY ,
                       GlslTokenType::USAMPLER2D , GlslTokenType::UTEXTURE2D , GlslTokenType::UIMAGE2D ,
                       GlslTokenType::USAMPLER2DARRAY , GlslTokenType::UTEXTURE2DARRAY , GlslTokenType::UIMAGE2DARRAY ,
                       GlslTokenType::USAMPLER2DMS , GlslTokenType::UTEXTURE2DMS , GlslTokenType::UIMAGE2DMS ,
                       GlslTokenType::USAMPLER2DMSARRAY , GlslTokenType::UTEXTURE2DMSARRAY , GlslTokenType::UIMAGE2DMSARRAY ,
                       GlslTokenType::USAMPLER2DRECT , GlslTokenType::UTEXTURE2DRECT , GlslTokenType::UIMAGE2DRECT ,
                       GlslTokenType::USAMPLER3D , GlslTokenType::UTEXTURE3D , GlslTokenType::UIMAGE3D ,
                       GlslTokenType::USAMPLERCUBE , GlslTokenType::UTEXTURECUBE , GlslTokenType::UIMAGECUBE ,
                       GlslTokenType::USAMPLERCUBEARRAY , GlslTokenType::UTEXTURECUBEARRAY , GlslTokenType::UIMAGECUBEARRAY ,
                       GlslTokenType::USAMPLERBUFFER , GlslTokenType::UTEXTUREBUFFER , GlslTokenType::UIMAGEBUFFER ,
                       GlslTokenType::ATOMIC_UINT ,
                       GlslTokenType::USUBPASSINPUT , GlslTokenType::USUBPASSINPUTMS ,
                       GlslTokenType::SAMPLER , GlslTokenType::SAMPLERSHADOW});
    }

    /// \todo implement this later when we need more advance parsing
    // void GlslParser::Synchronize() {
    //     Advance();
    //     while(!IsEOF()) {
    //         if (Previous().type == GlslTokenType::SEMICOLON)
    //             return;
    //         switch (Peek().type) {
    //             default: Advance(); break;
    //         }
    //     }
    // }

    std::shared_ptr<GlslStmnt> GlslParser::ParseTU() {
        // 1> Global Decl
        // 2> Func Decl
        return nullptr;
    }

    std::shared_ptr<GlslStmnt> GlslParser::ParseGlobalDecl() {
        return nullptr;
    }
    
    std::pair<std::string , std::vector<std::shared_ptr<GlslStmnt>>> GlslParser::Parse() {
        ENGINE_ASSERT(false , "This function is not implemented yet");

        // parse tokens
        index = 0;

        if (IsEOF()) return { "<n>" , {} };
        if (Peek().type != GlslTokenType::START_OF_FILE)
            throw GetError("Expected start of file");

        // ++index;

        while (!IsEOF() && Peek().type != GlslTokenType::END_OF_FILE) {
            std::shared_ptr<GlslStmnt> stmnt = ParseGlobalDecl();
            if (stmnt != nullptr) statements.push_back(stmnt);
            if (abort) break;
        }
        
        return { compiled_src.str() , statements };
    }

}