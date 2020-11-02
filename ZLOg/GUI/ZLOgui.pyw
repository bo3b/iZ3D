import re
import sys
import time
import wx

class Severity:
    (FATAL_ERROR, ERROR, WARNING, MESSAGE, NOTICE, FLOOD) = range(6)
    
    @staticmethod
    def from_string(str):
        return 0
        
class FormatError(RuntimeError):
    pass

class Message:
    rexp_all       = re.compile("\[(.*)\]<(.*)> (.*)");
    rexp_common    = re.compile("\[(.*)\]<(.*)> (.*)")
    rexp_code_line = re.compile("\[(.*)\]<(.*)> (.*)\((\d*)\): (.*)")
    rexp_function  = re.compile("\[(.*)\]<(.*)> (.*); (.*)")
    
    def __init__(self, str):
        m = re.match(Message.rexp_function, str)
        if m:
            self.module   = m.group(0)
            self.severity = Severity.from_string( m.group(1) )
            self.function = m.group(2)
            return
           
        m = re.match(Message.rexp_code_line, str)
        if m:
            self.module   = m.group(0)
            self.severity = Severity.from_string( m.group(1) )
            self.file     = m.group(2)
            self.line     = int( m.group(3) )
            return
           
        m = re.match(Message.rexp_common, str)
        if m:
            self.module   = m.group(0)
            self.severity = Severity.from_string( m.group(1) )
            self.file     = m.group(2)
            return
        
        raise FormatError()
    
class DecodeError(RuntimeError):
    pass

def to_utf8(text):
    for enc in ['utf-32', 'utf-16',  'utf-8', 'cp1251', 'acsii']:
        try:
            return text.decode(enc)
        except Exception:
            continue
    raise DecodeError()
        
class MessageSet:
    def __init__(self):
        self.messages = []
        
    def parseLine(self, line):
        try:
            print line
            self.messages.append(Message(line))
            print 'Matched'
        except FormatError:
            print 'Not matched'
            #self.messages += Message("[ZLOGui]<ERROR> Parse error: " + line)
            pass
            
    def readLogFile(self, fileName):
        file = open(fileName, 'r')
        text = to_utf8(file.read())
        print text
        for line in text.split('\n'):
            self.parseLine(line)
        file.close()
    
class MainFrame(wx.Frame):
    def __init__(self, parent):
        wx.Frame.__init__(self, parent, -1, 'Example', style = wx.CAPTION | wx.MINIMIZE_BOX | wx.CLOSE_BOX | wx.SYSTEM_MENU)
        self.SetSize((1024, 768))
        self.Centre()

class ZLOgui(wx.App):
    def OnInit(self):
        frame = MainFrame(None)
        frame.Show()
        return True

def main(args):
    application = ZLOgui()
    
    ms = MessageSet()
    for arg in args:
        try:
            ms.readLogFile(arg)
        except IOError:
            dlg = wx.MessageDialog(None,
                                   'Can\'t open \'' + arg + '\' log file. Proceed?',
                                   'Error',
                                   wx.YES_NO | wx.NO_DEFAULT | wx.ICON_WARNING);
            if dlg.ShowModal() == wx.ID_NO:
                return 1
                
    application.MainLoop()

if __name__ == '__main__':
    sys.argv += ['BasicHLSL10.exe.log']
    main(sys.argv[1:])
