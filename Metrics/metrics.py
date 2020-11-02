# The main launch python script

import os
import xml.dom.minidom
import sys
import xmlrpc.client
from TeamCityXMLReport import TeamCityXMLReport

def LaunchCLOC( curDir ):
    f = os.popen( curDir+'\\Cloc\\cloc-1.52.exe . --xml --quiet --force-lang="c++" --exclude-dir=lib --progress-rate=0 --skip-uniqueness' )
    resStr = ''
    for s in f:
        resStr += s.strip()
    dom = xml.dom.minidom.parseString( resStr )
    tag = dom.getElementsByTagName( "total")
    tag = tag[0]
    line = tag.attributes["code"]
    return int( line.value )

# xml-rpc access to the jira
def getInformationFromJira():
    try:
        s = xmlrpc.client.ServerProxy( 'http://jira.neurok.ru:8080/rpc/xmlrpc')
    except (ValueError, NameError):
        print( NameError )
        return
    try:
        auth = s.jira1.login('buildserver', 'jirabuild')
    except ( ValueError, NameError ):
        print( NameError )
        return
    try:
        issues = s.jira1.getIssuesFromFilter( auth, "10265" )
    except ( ValueError, NameError ):
        print( NameError )
    return len( issues )

    
if __name__ == '__main__':
    teamCityReport = TeamCityXMLReport()
    strToWrite = str( LaunchCLOC( os.path.dirname( sys.argv[0] ) ) )
    teamCityReport.addKeyValue( "Lines", strToWrite )
    teamCityReport.addKeyValue( "Bugs", str( getInformationFromJira() ) )
