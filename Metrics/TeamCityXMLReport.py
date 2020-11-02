'''
Created on 06.10.2010

@author: Alex
'''
class TeamCityXMLReport:
    keys = {}
    def __init__(self):
        pass
    
    def __del__(self):
        f = open( 'teamcity-info.xml', 'w' )
        # write root tag
        f.write( '<build>' )
        for key in self.keys.keys():
            f.write( '<statisticValue key="' )
            f.write( key )
            f.write( '" value="' )
            f.write( self.keys[key] )
            f.write( '"/>')
            print( key )
            print( '|||')
            print( self.keys[key] )
        # write root tag end
        f.write( '</build>' )
        f.close()
        pass
    
    def addKeyValue( self, strKey, strValue ):
        self.keys[strKey] = strValue
        