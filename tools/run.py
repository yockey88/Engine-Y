import globals
import os , subprocess , sys

args = globals.ProcessArguments(sys.argv)
config = globals.GetArgValue(args , "c" , "debug")
project = globals.GetArgValue(args , "p" , globals.EXE_NAME)

if (config == "debug"):
    config = "Debug"
    
if (config == "release"):
    config = "Release"

exepath = "{}/bin/{}/{}".format(os.getcwd() , config , globals.EXE_NAME)
ret = 0

if globals.IsWindows():
    print("Running: {}\\run.bat {} {}".format(globals.TOOLS_DIR , config , project))
    ret = subprocess.call(
        [
            "cmd.exe" , "/c" , "{}\\run.bat".format(globals.TOOLS_DIR) , 
            config , project ,
            "--project-name" , "editor" ,
            "--project-path" , "./editor" ,
            "--modules-path" , "bin/Debug/editor/editor_modules.dll" ,
            "--project-file" , "editor/editor.yproj"
        ] , 
        cwd=os.getcwd()
    )
else:
    ret = subprocess.call(["{}".format(exepath)] , cwd=exepath)

sys.exit(ret)
