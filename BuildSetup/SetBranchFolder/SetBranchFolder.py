# -*- coding: utf-8 -*-

import sys
import os
import datetime
import time
    
def getSVNURL(configFile):
    header = file(configFile).readlines()
    for line in header:
        words = line.split()
        if (len(words) >= 3) and (words[0] == "#define") and (words[1] == "BRANCH_URL"):
            if words[2].startswith('"') and words[2].endswith('"'):
                return words[2][1:-1]
            else:
                return words[2]
    return ""

url = getSVNURL("bin\\temp_version.h")
#url = "svn://svn.neurok.ru/dev/driver/branch/dffff/fff"
driverurl = "svn://svn.neurok.ru/dev/driver/"
foldername = ""
if url.startswith(driverurl):
    foldername = url[len(driverurl):]
    if foldername == "trunk":
        foldername = ""
    elif foldername.startswith("branch/") or foldername.startswith("branches/"):
        foldername = foldername[foldername.find("/") + 1:]
        foldername = foldername[:foldername.find("/")] + "\\"
    if foldername.startswith("tags/"):
        foldername = foldername[foldername.find("/") + 1:]
        foldername = "tag_" + foldername[:foldername.find("/")] + "\\"
os.putenv('BRANCH_NAME', foldername)
command = 'set %s=%s\n' % ("BRANCH_NAME", foldername)
open('setbranchname.bat', 'w').write(command)