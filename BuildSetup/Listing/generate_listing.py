# -*- coding: utf-8 -*-

import sys
import os
import datetime
import time
import ConfigParser

from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, PageBreak
    
def read_config(configFile):
    config = ConfigParser.ConfigParser()
    config.read(configFile)
    
    properties = {}
    
    def makeAbsPaths(pathList):
        absPathList = []
        for path in pathList:
            absPathList.append(os.path.abspath(path))
        return absPathList
        
    # sources
    if config.has_option("sources", "exclude_paths"):
        properties["exclude_paths"] = config.get("sources", "exclude_paths").split(";")
        properties["exclude_paths"] = makeAbsPaths(properties["exclude_paths"])
    else:
        properties["exclude_paths"] = []
    
    if config.has_option("sources", "source_paths"):
        properties["source_paths"] = config.get("sources", "source_paths").split(";")
        properties["source_paths"] = makeAbsPaths(properties["source_paths"])
    else:
        properties["source_paths"] = []
        
    if config.has_option("sources", "extensions"):
        properties["extensions"] = config.get("sources", "extensions").split(";")
    else:
        properties["extensions"] = [".h", ".cpp", ".hpp"]
        
    if config.has_option("sources", "encodings"):
        properties["encodings"] = config.get("sources", "encodings").split(";")
    else:
        properties["encodings"] = ["Windows-1251", "KOI8-R", "utf-8"]
    
    # appearance
    if config.has_option("appearance", "font"):
        properties["font"] = config.get("appearance", "font")
        
    if config.has_option("appearance", "font_size"):
        properties["font_size"] = int(config.get("appearance", "font_size"))
    else:
        properties["font_size"] = 10
        
    if config.has_option("appearance", "left_margin"):
        properties["left_margin"] = float(config.get("appearance", "left_margin"))
    else:
        properties["left_margin"] = 0
                
    if config.has_option("appearance", "right_margin"):
        properties["right_margin"] = float(config.get("appearance", "right_margin"))
    else:
        properties["right_margin"] = 0
                
    if config.has_option("appearance", "top_margin"):
        properties["top_margin"] = float(config.get("appearance", "top_margin"))
    else:
        properties["top_margin"] = 0
                
    if config.has_option("appearance", "bottom_margin"):
        properties["bottom_margin"] = float(config.get("appearance", "bottom_margin"))
    else:
        properties["bottom_margin"] = 0
    
    return properties

properties = read_config("config.ini")

def make_styles():
    from reportlab.lib.enums import TA_LEFT
    from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
    from reportlab.pdfbase import pdfmetrics
    from reportlab.pdfbase.ttfonts import TTFont

    reload(sys)
    sys.setdefaultencoding("latin1")
    styles = getSampleStyleSheet()
    styles.add(ParagraphStyle(name="Heading", alignment=TA_LEFT, fontSize=12))
    if "font" in properties:
        pdfmetrics.registerFont(TTFont("CustomFont", properties["font"]))
        styles.add(ParagraphStyle(name="CodeWithCyrillic", alignment=TA_LEFT, fontSize=properties["font_size"], fontName="CustomFont"))
    else:
        # may not work because leak of support cyrillic symbols
        styles.add(ParagraphStyle(name="CodeWithCyrillic", alignment=TA_LEFT, fontSize=properties["font_size"], fontName="Courier"))
    return styles

styles = make_styles()

def to_utf8(text):
    for enc in properties["encodings"]:
        try:
            data = text.decode(enc)
            break
        except Exception:
            continue
            
    if data is None:
        print "can't convert file content to utf-8"
        return text
    else:
        return data.encode('utf-8')
        
def format_text(text):
    formatted = ""
    specials = {'&' : "&amp;", '<' : "&lt;", '>' : "&gt;"}
    newline = 0
    for char in text:
        if char == '\n':
            newline = 1
            formatted += "<br/>"
        elif char in specials:
            formatted += specials[char]
        elif newline:
            if char == " ":
                formatted += "&nbsp;"
            elif char == "\t":
                formatted += "&nbsp;&nbsp;&nbsp;&nbsp;"
            else:
                formatted += char
                newline = 0
        else:
            formatted += char
    formatted.encode("utf-8")
    return formatted

def print_heading(content):
    content.append(Paragraph("Powered by IZ3D Inc. %s" % time.strftime("%a, %d %b %Y %H:%M:%S", time.gmtime()), styles["Normal"]))
    return
    
def extension_listed(filename):
    for ext in properties["extensions"]:
        if filename.endswith(ext):
            return 1
    return 0
  
def print_files(content, baseDir, name):
    path = os.path.abspath(name)
    if path in properties["exclude_paths"]:
        return
   
    if os.path.isdir(name):
        # recursively print files
        for node in os.listdir(path):
            print_files(content, baseDir + "\\" + node, path + "\\" + node)
    elif extension_listed(path):
        # print file into pdf
        baseDir = baseDir[0:-len(os.path.basename(path))]
        pdfFileName = baseDir + os.path.basename(path)
        print "parsing '%s'" % pdfFileName
        
        code = to_utf8( file(path).read() )
        content.append(PageBreak())
        content.append(Paragraph("<b>File '%s'</b>" % pdfFileName, styles["Heading"]))
        content.append(Paragraph("<b>Number of lines: %d</b>" % len(code.split('\n')), styles["Heading"]))
        content.append(Spacer(1, 12))
        content.append(Paragraph(format_text(code), styles["CodeWithCyrillic"]))
    return

if len(sys.argv) < 2:
    print "Usage: <script> <listing file> [config file]"
    sys.exit()
else:
    if len(sys.argv) == 3:
        properties = read_config(sys.argv[2])
        
    startTime = time.time()

    # generate pdf content
    content = []
    print_heading(content)
    for path in properties["source_paths"]:
        print_files(content, "", path)

    # construct pdf
    print "generating pdf"
    pdf = SimpleDocTemplate(sys.argv[1], 
                            bottomMargin = properties["bottom_margin"], 
                            topMargin = properties["top_margin"], 
                            rightMargin = properties["right_margin"], 
                            leftMargin = properties["left_margin"])
    pdf.build(content)

    deltaTime = int(time.time() - startTime)
    print "working time: %s" % str(datetime.timedelta(seconds = deltaTime))
