###############################################################################
# Name: Cody Precord                                                          #
# Purpose: Log output viewer for the shelf                                    #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2008 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
Editra LogViewer

This module provides classes for managing the log display and filtering of its
messages. The module also exports an implementation of a shelf plugin for
displaying a L{LogViewer} in the Shelf.

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: ed_log.py 62723 2009-11-26 18:43:20Z CJP $"
__revision__ = "$Revision: 62723 $"

#--------------------------------------------------------------------------#
# Imports
import os
import re
import wx

# Editra Libraries
import ed_msg
import eclib
import iface
import plugin
from profiler import Profile_Get
import ed_glob

#-----------------------------------------------------------------------------#
# Globals
_ = wx.GetTranslation

SHOW_ALL_MSG = 'ALL'
#-----------------------------------------------------------------------------#

# Interface Implementation
class EdLogViewer(plugin.Plugin):
    """Shelf interface implementation for the log viewer"""
    plugin.Implements(iface.ShelfI)

    __name__ = u'Editra Log'

    @staticmethod
    def AllowMultiple():
        """EdLogger allows multiple instances"""
        return True

    @staticmethod
    def CreateItem(parent):
        """Returns a log viewr panel"""
        return LogViewer(parent)

    def GetBitmap(self):
        """Get the log viewers tab icon
        @return: wx.Bitmap

        """
        bmp = wx.ArtProvider.GetBitmap(str(self.GetId()), wx.ART_MENU)
        return bmp

    @staticmethod
    def GetId():
        """Plugin menu identifier ID"""
        return ed_glob.ID_LOGGER

    @staticmethod
    def GetMenuEntry(menu):
        """Get the menu entry for the log viewer
        @param menu: the menu items parent menu

        """
        return wx.MenuItem(menu, ed_glob.ID_LOGGER, _("Editra Log"),
                           _("View Editra's console log"))

    def GetName(self):
        """Return the name of this control"""
        return self.__name__

    @staticmethod
    def IsStockable():
        """EdLogViewer can be saved in the shelf preference stack"""
        return True

#-----------------------------------------------------------------------------#

# LogViewer Ui Implementation
class LogViewer(eclib.ControlBox):
    """LogViewer is a control for displaying and working with output
    from Editra's log.

    """
    def __init__(self, parent):
        eclib.ControlBox.__init__(self, parent)

        # Attributes
        self._buffer = LogBuffer(self)
        self.SetWindow(self._buffer)
        self._srcfilter = None

        # Layout
        self.__DoLayout()

        # Event Handlers
        self.Bind(wx.EVT_BUTTON,
                  lambda evt: self._buffer.Clear(), id=wx.ID_CLEAR)
        self.Bind(wx.EVT_CHOICE, self.OnChoice, self._srcfilter)

        # Message Handlers
        ed_msg.Subscribe(self.OnThemeChange, ed_msg.EDMSG_THEME_CHANGED)

    def __del__(self):
        """Cleanup and unsubscribe from messages"""
        ed_msg.Unsubscribe(self.OnThemeChange)

    def __DoLayout(self):
        """Layout the log viewer window"""
        # Setup ControlBar
        ctrlbar = eclib.ControlBar(self, style=eclib.CTRLBAR_STYLE_GRADIENT)
        if wx.Platform == '__WXGTK__':
            ctrlbar.SetWindowStyle(eclib.CTRLBAR_STYLE_DEFAULT)

        # View Choice
        self._srcfilter = wx.Choice(ctrlbar, wx.ID_ANY, choices=[])
        ctrlbar.AddControl(wx.StaticText(ctrlbar,
                                         label=_("Show output from") + ":"))
        ctrlbar.AddControl(self._srcfilter)

        # Clear Button
        ctrlbar.AddStretchSpacer()
        cbmp = wx.ArtProvider.GetBitmap(str(ed_glob.ID_DELETE), wx.ART_MENU)
        if cbmp.IsNull() or not cbmp.IsOk():
            cbmp = None
        clear = eclib.PlateButton(ctrlbar, wx.ID_CLEAR, _("Clear"),
                                     cbmp, style=eclib.PB_STYLE_NOBG)
        ctrlbar.AddControl(clear, wx.ALIGN_RIGHT)
        ctrlbar.SetVMargin(1, 1)
        self.SetControlBar(ctrlbar)
        
    def OnChoice(self, evt):
        """Set the filter based on the choice controls value
        @param evt: wx.CommandEvent

        """
        self._buffer.SetFilter(self._srcfilter.GetStringSelection())

    def OnThemeChange(self, msg):
        """Update the buttons icon when the icon theme changes
        @param msg: Message Object

        """
        clear = self.FindWindowById(wx.ID_CLEAR)
        cbmp = wx.ArtProvider.GetBitmap(str(ed_glob.ID_DELETE), wx.ART_MENU)
        clear.SetBitmap(cbmp)
        clear.Refresh()

    def SetSources(self, srclist):
        """Set the list of available log sources in the choice control
        @param srclist: list of log sources

        """
        choice = self._srcfilter.GetStringSelection()
        lst = sorted(srclist)
        lst.insert(0, _("All"))
        self._srcfilter.SetItems(lst)
        if not self._srcfilter.SetStringSelection(choice):
            self._srcfilter.SetSelection(0)

#-----------------------------------------------------------------------------#
class LogBuffer(eclib.OutputBuffer):
    """Buffer for displaying log messages that are sent on Editra's
    log channel.
    @todo: make line buffering configurable through interface

    """
    RE_WARN_MSG = re.compile(r'\[err\]|\[error\]|\[warn\]')
    ERROR_STYLE = eclib.OPB_STYLE_MAX + 1

    def __init__(self, parent):
        eclib.OutputBuffer.__init__(self, parent)

        # Attributes
        self._filter = SHOW_ALL_MSG
        self._srcs = list()
        self.SetLineBuffering(2000)

        # Setup
        font = Profile_Get('FONT1', 'font', wx.Font(11, wx.FONTFAMILY_MODERN, 
                                                    wx.FONTSTYLE_NORMAL, 
                                                    wx.FONTWEIGHT_NORMAL))
        self.SetFont(font)
        style = (font.GetFaceName(), font.GetPointSize(), "#FF0000")
        self.StyleSetSpec(LogBuffer.ERROR_STYLE,
                          "face:%s,size:%d,fore:#FFFFFF,back:%s" % style)

        # Subscribe to Editra's Log
        ed_msg.Subscribe(self.UpdateLog, ed_msg.EDMSG_LOG_ALL)

    def __del__(self):
        """Unregister from recieving any more log messages"""
        ed_msg.Unsubscribe(self.UpdateLog, ed_msg.EDMSG_LOG_ALL)
        super(LogBuffer, self).__del__()

    def AddFilter(self, src):
        """Add a new filter source
        @param src: filter source string
        @postcondition: if src is new the parent window is updated

        """
        if src not in self._srcs:
            self._srcs.append(src)
            self.GetParent().SetSources(self._srcs)

    def ApplyStyles(self, start, txt):
        """Apply coloring to error and warning messages
        @note: overridden from L{outbuff.OutputBuffer}
        @param start: Start position of text that needs styling in the buffer
        @param txt: The string of text that starts at the start position in the
                    buffer.

        """
        for group in LogBuffer.RE_WARN_MSG.finditer(txt):
            sty_s = start + group.start()
            sty_e = start + group.end()
            self.StartStyling(sty_s, 0xff)

            # Highlight error messages with ERROR_STYLE
            self.SetStyling(sty_e - sty_s, LogBuffer.ERROR_STYLE)

    def SetFilter(self, src):
        """Set the level of what is shown in the display
        @param src: Only show messages from src
        @return: bool

        """
        if src in self._srcs:
            self._filter = src
            return True
        elif src == _("All"):
            self._filter = SHOW_ALL_MSG
            return True
        else:
            return False

    def UpdateLog(self, msg):
        """Add a new log message
        @param msg: Message Object containing a LogMsg

        """
        if not self.IsRunning():
            if wx.Thread_IsMain():
                self.Start(150)

        # Check filters
        logmsg = msg.GetData()
        org = logmsg.Origin
        if org not in self._srcs:
            self.AddFilter(org)

        if self._filter == SHOW_ALL_MSG:
            self.AppendUpdate(unicode(logmsg) + os.linesep)
        elif self._filter == logmsg.Origin:
            msg = u"[%s][%s]%s" % (logmsg.ClockTime, logmsg.Type, logmsg.Value)
            self.AppendUpdate(msg + os.linesep)
        else:
            pass
