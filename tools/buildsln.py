import globals 
import os , subprocess , sys
import shutil

args = globals.ProcessArguments(sys.argv)
print(args)
CONFIG = globals.GetArgValue(args , "c" , "debug")

print("Building Machine Y in [{}] Configuration".format(CONFIG))
print("\n")

ret = 0

if globals.IsWindows():
    VS_BUILD_PATH = os.environ["VS_BUILD_PATH"][8:-1]
    VS_BUILD_PATH = "C:\\\\" + VS_BUILD_PATH
    ret = subprocess.call(["cmd.exe" , "/c" , VS_BUILD_PATH , "{}.sln".format(globals.MAIN_NAME) , "/property:Configuration={}".format(CONFIG)])
    if (ret != 0):
        sys.exit(ret)

if globals.IsLinux():
    ret = subprocess.call(["make" , "config={}".format(CONFIG)])

if globals.IsMac():
    ret = subprocess.call(["make" , "config={}".format(CONFIG)])
    

    
if os.path.exists("bin/{}/{}".format(CONFIG , globals.EXE_NAME)):
    print("\n")
    print("Build finished | Copying dependencies...")
    print("\n")
    
    exe_directory = "bin/{}/{}".format(CONFIG , globals.EXE_NAME)
    is_debug = CONFIG == "debug"
    mono_lib_dir = "external/mono/bin/{}".format(CONFIG)
    directory = os.fsencode(mono_lib_dir)
        
    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        if filename.endswith(".dll") or filename.endswith(".pdb") and not os.path.exists(os.path.join(exe_directory , filename)):
            shutil.copy(os.path.join(mono_lib_dir , filename) , exe_directory)
            
    assimp_dir = "external/assimp/lib/{}".format(CONFIG)
    if is_debug:
        if not os.path.exists(os.path.join(exe_directory , "assimp-vc143-mtd.dll")):
            shutil.copy(os.path.join(assimp_dir , "assimp-vc143-mtd.dll") , exe_directory)
        if not os.path.exists(os.path.join(exe_directory , "assimp-vc143-mtd.pdb")):
            shutil.copy(os.path.join(assimp_dir , "assimp-vc143-mtd.pdb") , exe_directory)
    else:
        if not os.path.exists(os.path.join(exe_directory , "assimp-vc143-mt.dll")):
            shutil.copy(os.path.join(assimp_dir , "assimp-vc143-mt.dll") , exe_directory)
        if not os.path.exists(os.path.join(exe_directory , "assimp-vc143-mt.pdb")):
            shutil.copy(os.path.join(assimp_dir , "assimp-vc143-mt.pdb") , exe_directory)
else:
    print("Build failed, see output above for details")
    ret = 1

sys.exit(ret)