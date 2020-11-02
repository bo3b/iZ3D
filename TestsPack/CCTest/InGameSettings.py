import re
import sys
import testing
import xml.etree.ElementTree as etree

def do_test():
    tree = etree.parse( open(testing.USER_PROFILE_PATH, "r") )
    ns   = testing.USER_PROFILE_NS
    root = tree.getroot()

    # key : (required value, default value)
    values =\
    {\
        "ShowFPS" : (1, 0),\
        "ShowWizardAtStartup" : (0, 0),\
        "FrustumAdjustMode" : (1, 0),\
        "DontLockMouse" : (1, 0),\
        "ShowOSD" : (0, 1),\
        "ForceVSyncOff" : (1, 0),\
        "UseSimpleStereoProjectionMethod" : (1, 0),\
        "SwapEyes" : (1, 0),\
        "BlackAreaWidth" : (0.25, 0.03),\
        "SeparationScale" : (2.0, 1.0),\
        "DeviceMode" : (0, 3),\
        "SeparationMode" : (2, 0)
    }

    for key,(value,default) in values.iteritems():
        elems = root.findall( "{%s}Profile[@Name='DTest']/{%s}%s" % (ns, ns, key) )
        if default == value: 
            # default value should not be listed in config
            if not len(elems) == 0:
                return 1
            else:
                continue
        elif len(elems) != 1:
            return 1
        
        elem = elems[0]
        try:
            if float(elem.attrib["Value"]) != float(value):
                return 1
        except ValueError, KeyError:
            return 1

    return 0

if __name__ == "__main__":
    sys.exit(do_test())
