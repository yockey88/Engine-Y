import globals
import sys

print("[[Machine Y]-->v[{}.{}.{}]]".format(globals.VMAJOR , globals.VMINOR , globals.VPATCH))
if len(sys.argv) >= 2:
    print(" , ".join(sys.argv))

sys.exit(0)