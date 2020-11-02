from xml.dom.minidom import parse

def getValuableString( dataFile ):
    s = ""
    while len( s ) == 0 or s.isspace():
        s = dataFile.readline()
    return s
    
def processTheNode( NodeToProcess, dataFile ):
    if NodeToProcess.nodeType != NodeToProcess.TEXT_NODE:
        for node in NodeToProcess.childNodes:
            processTheNode( node, dataFile )
        return
    if len( NodeToProcess.data ) == 0 or NodeToProcess.data.isspace():
        return
    print("Eng " + NodeToProcess.data + "===" )
    bufStr = getValuableString( dataFile )
    NodeToProcess.data = bufStr.split('\t')[1]
    print("Jap " + NodeToProcess.data + "===" )
    
    

def translate( FromFileName, Sample, ToFileName ):
    domSample = parse( Sample ) # parse the Sample file by name
    f = open( FromFileName, encoding='utf-8' )
    processTheNode( domSample, f )
    f.close()
    toSave = open( ToFileName, mode='w', encoding='utf-8' )
    domSample.writexml( toSave )
    toSave.close()
    return

if __name__ == "__main__":
    translate( "d:/work/driver-m/S3DDriverSetup/Content/language/Japanese.txt",
               "C:/Users/Alex/Downloads/English.xml",
               "d:/work/driver-m/S3DDriverSetup/Content/language/Japanese.xml" )
