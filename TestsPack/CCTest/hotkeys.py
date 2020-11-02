import re
import sys
import testing
import xml.etree.ElementTree as etree

def do_test():
    tree = etree.parse( open(testing.USER_PROFILE_PATH, "r") )
    ns   = testing.USER_PROFILE_NS
    root = tree.getroot()
    
    keys = root.findall( "{%s}Profile[@Name='DTest']/{%s}Keys/*" % (ns, ns) )
    if keys == []:
        return 1
    
    hotkey =\
    {\
        "ToggleStereo" : "65",\
        "ZPSIncrease" : "81",\
        "ZPSDecrease" : "87",\
        "StereoBaseIncrease" : "69",\
        "StereoBaseDecrease" : "82",\
        "SetPreset1" : "83",\
        "SetPreset2" : "68",\
        "SetPreset3" : "70",\
        "SwapEyes" : "89",\
        "ToggleAutoFocus" : "84",\
        "ToggleLaserSight" : "71",\
        "Screenshot" : "72"\
    }
    
    for key in keys:
        tag = re.match("{.*}(.*)", key.tag).group(1)
        if hotkey.has_key(tag) and hotkey[tag] != key.attrib["Key"]:
            return 1
            
    return 0
    
if __name__ == "__main__":
    sys.exit(do_test())
