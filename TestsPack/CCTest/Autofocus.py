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
        "FrameYCoordinate" : (0.0, 0.495),\
        "SearchFrameSizeX" : (500, 512),\
        "SearchFrameSizeY" : (60, 64),\
        "ShiftSpeed" : (2, 1),\
        "Max_ABS_rZPS" : (0.1, 0.2)
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
