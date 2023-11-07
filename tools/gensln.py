import globals
import subprocess , sys

ret = 0;

if globals.IsWindows():
    ret = subprocess.call(
        ["cmd.exe" , "/c" , "premake\\premake5" , "--debug" , "vs2022"]
    )
    if (ret != 0):
        sys.exit(ret)
        
    ret = subprocess.call(
        ["cmd.exe" , "/c" , "premake\\premake5" , "--file=launcher\\premake5.lua" , "--debug" , "vs2022"]
    )
    if (ret != 0):
        sys.exit(ret)

        
    ret = subprocess.call(
        ["cmd.exe" , "/c" , "premake\\premake5" , "--file=editor\\premake5.lua" , "--debug" , "vs2022"]
    )
    if (ret != 0):
        sys.exit(ret)

if globals.IsLinux():
    ret = subprocess.call(["premake/premake5.linux" , "gmake2"])

if globals.IsMac():
    ret = subprocess.call(["premake/premake5" , "gmake2"])
    if ret == 0:
        subprocess.call(["premake/premake5" , "xcode4"])

sys.exit(ret)