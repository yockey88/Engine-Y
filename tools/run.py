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
    ret = subprocess.call(["cmd.exe" , "/c" , "{}\\run.bat".format(globals.TOOLS_DIR) , config , project] , cwd=os.getcwd())
else:
    ret = subprocess.call(["{}".format(exepath)] , cwd=exepath)

sys.exit(ret)