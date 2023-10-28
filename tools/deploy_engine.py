import globals

import subprocess , sys , os
import shutil

args = globals.ProcessArguments(sys.argv)
print(args)
CONFIG = globals.GetArgValue(args , "c" , "debug")

ret = 0

if globals.IsWindows():
    APP_DATA = os.environ["APPDATA"][1:-1]
    ENGINEY_PATH = os.path.join(APP_DATA , "EngineY")
    if os.path.exists(os.path.join(ENGINEY_PATH , "engine")):
        print("Removing old enginey directory...")
        shutil.rmtree(os.path.join(ENGINEY_PATH , "engine"))
    
    print("Copying engine directory... [{}]".format(os.path.join(ENGINEY_PATH , "engine"))) 
    shutil.copytree("engine" , os.path.join(ENGINEY_PATH , "engine")) 
    subprocess.call(["cmd.exe" , "/c" , "premake\\premake5" , "vs2022"] , cwd=ENGINEY_PATH)
    
    VS_BUILD_PATH = os.environ["VS_BUILD_PATH"][8:-1]
    VS_BUILD_PATH = "C:\\\\" + VS_BUILD_PATH
    ret = subprocess.call(["cmd.exe" , "/c" , VS_BUILD_PATH , "{}.sln".format(globals.MAIN_NAME) , "/property:Configuration={}".format(CONFIG)] , cwd=ENGINEY_PATH)
    if (ret != 0):
        sys.exit(ret)
    ret = subprocess.call(["cmd.exe" , "/c" , VS_BUILD_PATH , "{}.sln".format(globals.MAIN_NAME) , "/property:Configuration={}".format(CONFIG)] , cwd=ENGINEY_PATH)
    
sys.exit(ret)