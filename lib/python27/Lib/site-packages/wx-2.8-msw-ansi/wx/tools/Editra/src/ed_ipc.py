###############################################################################
# Name: ed_ipc.py                                                             #
# Purpose: Editra IPC client/server                                           #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2008-2009 Cody Precord <staff@editra.org>                    #
# License: wxWindows License                                                  #
###############################################################################

"""
Classes and utilities for handling IPC between running instances of Editra. The
IPC is done through sockets using the TCP protocol. Message packets have a
specified format and authentication method that is described in L{EdIpcServer}.

Remote Control Protocol:

This server and its relationship with the main application object allows for
some limited remote control of Editra. The server's basic message protocol
requirements are as follows.

SESSION_KEY;xml;MSGEND

Where the SESSION_KEY is the unique authentication key created by the app that
started the server. This key is stored in the user profile and only valid for
the current running session of Editra. The MSGEND indicator is the L{MSGEND}
string defined in this file (*EDEND*). If both of these parts of the message
are found and correct the server will forward the messages that are packed in
between to the app.

Message Format:

<edipc>
   <filelist>
      <file name="absolute_filepath"/>
   </filelist>
   <arglist>
      <arg name="g" value="2"/>
   </arglist>
</edipc>

@example: SESSION_KEY;xml;MSGEND
@summary: Editra's IPC Library

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: ed_ipc.py 63657 2010-03-09 01:45:25Z CJP $"
__revision__ = "$Revision: 63657 $"

#-----------------------------------------------------------------------------#
# Imports
import sys
import wx
import threading
import socket
import time
#import select

# Editra Libs
import syntax
import ebmlib

#-----------------------------------------------------------------------------#
# Globals

# Port choosing algorithm ;)
EDPORT = (10 * int('ed', 16) + sum(ord(x) for x in "itr") + int('a', 16)) * 10
MSGEND = u"*EDEND*"

# Xml Implementation
EDXML_IPC       = u"edipc"
EDXML_FILELIST  = u"filelist"
EDXML_FILE      = u"file"
EDXML_ARGLIST   = u"arglist"
EDXML_ARG       = u"arg"

#-----------------------------------------------------------------------------#

edEVT_COMMAND_RECV = wx.NewEventType()
EVT_COMMAND_RECV = wx.PyEventBinder(edEVT_COMMAND_RECV, 1)
class IpcServerEvent(wx.PyCommandEvent):
    """Event to signal the server has recieved some commands"""
    def __init__(self, etype, eid, values=None):
        """Creates the event object"""
        wx.PyCommandEvent.__init__(self, etype, eid)
        self._value = values

    def GetCommands(self):
        """Returns the list of commands sent to the server
        @return: the value of this event

        """
        return self._value

#-----------------------------------------------------------------------------#

class EdIpcServer(threading.Thread):
    """Create an instance of IPC server for Editra. IPC is handled through
    a socket connection to an instance of this server listening on L{EDPORT}.
    The server will recieve commands and dispatch them to the app.
    Messages sent to the server must be in the following format.
    
      AuthenticationKey;Message Data;MSGEND

    The _AuthenticationKey_ is the same as the key that started the server it
    is used to validate that messages are coming from a legitimate source.

    _Message Data_ is a string of data where items are separated by a single
    ';' character. If you use L{SendCommands} to communicate with the server
    then this message separators are handled internally by that method.

    L{MSGEND} is the token to signify that the client is finished sending
    commands to the server. When using L{SendCommands} this is also 
    automatically handled.

    @todo: investigate possible security issues

    """
    def __init__(self, app, key):
        """Create the server thread
        @param app: Application object the server belongs to
        @param key: Unique user authentication key (string)

        """
        threading.Thread.__init__(self)

        # Attributes
        self._exit = False
        self.__key = key
        self.app = app
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Setup
        ## Try new ports till we find one that we can use
        global EDPORT
        while True:
            try:
                self.socket.bind(('127.0.0.1', EDPORT))
                break
            except:
                EDPORT += 1

        self.socket.listen(5)

    def Shutdown(self):
        """Tell the server to exit"""
        self._exit = True
        # Wake up the server in case its waiting
        # TODO: should add a specific exit event message
        SendCommands(IPCCommand(), self.__key)

    def run(self):
        """Start the server. The server runs in blocking mode, this
        shouldn't be an issue as it should rarely need to respond to
        anything.

        """
        while not self._exit:
            try:
                client, addr = self.socket.accept()

                if self._exit:
                    break

                # Block for up to 2 seconds while reading
                start = time.time()
                recieved = u''
                while time.time() < start + 2:
                    recieved += client.recv(4096)
                    if recieved.endswith(MSGEND):
                        break

                # If message key is correct and the message is ended, process
                # the input and dispatch to the app.
                if recieved.startswith(self.__key) and recieved.endswith(MSGEND):
                    recieved = recieved.replace(self.__key, u'', 1)

                    # Parse the xml
                    exml = IPCCommand()
                    try:
                        xmlstr = recieved.rstrip(MSGEND).strip(u";")
                        exml.LoadFromString(xmlstr)
                    except Exception, msg:
                        # ignore parsing errors
                        continue

                    evt = IpcServerEvent(edEVT_COMMAND_RECV, wx.ID_ANY, exml)
                    wx.CallAfter(wx.PostEvent, self.app, evt)
            except socket.error:
                # TODO: Better error handling
                self._exit = True

        # Shutdown Server
        try:
            self.socket.shutdown(socket.SHUT_RDWR)
        except:
            pass

        self.socket.close()

#-----------------------------------------------------------------------------#

def SendCommands(xmlobj, key):
    """Send commands to the running instance of Editra
    @param xmlobj: EditraXml Object
    @param key: Server session authentication key
    @return: bool

    """
    assert isinstance(xmlobj, syntax.EditraXml), "SendCommands expects an xml object"

    # Build the edipc protocol msg
    cmds = list()
    cmds.insert(0, key)
    cmds.append(xmlobj.GetXml())
    cmds.append(MSGEND)

    try:
        # Setup the client socket
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect(('127.0.0.1', EDPORT))

        # Server expects commands delimited by ;
        message = u";".join(cmds)
        client.send(message)
        client.shutdown(socket.SHUT_RDWR)
        client.close()
    except Exception, msg:
        return False
    else:
        return True

#-----------------------------------------------------------------------------#
# Command Serialization

class IPCCommand(syntax.EditraXml):
    """Xml packet used for sending data to remote process through ipc"""
    def __init__(self):
        syntax.EditraXml.__init__(self)

        # Attributes
        self._files = IPCFileList()
        self._args = IPCArgList()

        # Setup
        self.SetName(EDXML_IPC)
        self.RegisterHandler(self._files)
        self.RegisterHandler(self._args)

    #---- Public Api ----#

    def GetArgs(self):
        """Get the list of paths
        @return: list of tuples

        """
        return self._args.GetArgs()

    def SetArgs(self, args):
        """Set the files
        @param flist: list of strings

        """
        self._args.SetArgs(args)

    def GetFiles(self):
        """Get the list of paths
        @return: list of strings

        """
        return self._files.GetFiles()

    def SetFiles(self, flist):
        """Set the files
        @param flist: list of strings

        """
        self._files.SetFiles(flist)

class IPCFileList(syntax.EditraXml):
    """Xml object for holding the list of files

    <filelist>
       <file value="/path/to/file"/>
    </filelist>

    """
    def __init__(self):
        syntax.EditraXml.__init__(self)

        # Attributes
        self._files = list()

        # Setup
        self.SetName(EDXML_FILELIST)

    #---- Xml Implementation ----#

    def startElement(self, name, attrs):
        """Collect all the file elements"""
        if name == EDXML_FILE:
            fname = attrs.get(syntax.EXML_VALUE, None)
            if fname is not None:
                self._files.append(fname)

    def GetSubElements(self):
        """Get the objects subelements"""
        xmlstr = u""
        tmp = u"<%s %s=\"" % (EDXML_FILE, syntax.EXML_VALUE)
        tmp += u"%s\"/>"
        for fname in self._files:
            if not ebmlib.IsUnicode(fname):
                fname = fname.decode(sys.getfilesystemencoding())
            xmlstr += tmp % fname
        return xmlstr

    #--- public api ----#

    def GetFiles(self):
        """Get the list of paths
        @return: list of strings

        """
        return self._files

    def SetFiles(self, flist):
        """Set the list of files
        @param flist: list of strings

        """
        self._files = flist

class IPCArgList(syntax.EditraXml):
    """Xml object for holding the list of args

    <arglist>
       <arg name="test" value="x"/>
    </arglist>

    """
    def __init__(self):
        syntax.EditraXml.__init__(self)

        # Attributes
        self._args = list()

        # Setup
        self.SetName(EDXML_ARGLIST)

    #---- Xml Implementation ----#

    def startElement(self, name, attrs):
        """Collect all the file elements"""
        if name == EDXML_ARG:
            arg = attrs.get(syntax.EXML_NAME, None)
            val = attrs.get(syntax.EXML_VALUE, u'')
            if not val.isdigit():
                val = -1
            else:
                val = int(val)
            if arg is not None:
                self._args.append((arg, val))

    def GetSubElements(self):
        """Get the objects sub-elements"""
        xmlstr = u""
        tmp = u"<%s %s=\"" % (EDXML_ARG, syntax.EXML_NAME)
        tmp += u"%s\" "
        tmp += "%s=\"" % syntax.EXML_VALUE
        tmp += "%s\"/>"
        for argval in self._args:
            xmlstr += tmp % argval
        return xmlstr

    #--- public api ----#

    def GetArgs(self):
        """Get the list of arguments (command line switches)
        @return: list of tuples

        """
        return self._args

    def SetArgs(self, args):
        """Set the list of files
        @param flist: list of tuples

        """
        self._args = args
    