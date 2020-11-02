# --------------------------------------------------------------------------- #
# TOASTERBOX wxPython IMPLEMENTATION
# Ported And Enhanced From wxWidgets Contribution (Aj Bommarito) By:
#
# Andrea Gavana, @ 16 September 2005
# Latest Revision: 01 Dec 2009, 14.00 GMT
#
#
# TODO/Caveats List
#
# 1. Any Idea?
#
#
# For All Kind Of Problems, Requests Of Enhancements And Bug Reports, Please
# Write To Me At:
#
# andrea.gavana@gmail.com
# gavana@kpo.kz
#
# Or, Obviously, To The wxPython Mailing List!!!
#
#
# End Of Comments
# --------------------------------------------------------------------------- #


"""
ToasterBox is a cross-platform widget to make the creation of MSN style "toaster"
popups easier.


Description
===========

ToasterBox is a cross-platform widget to make the creation of MSN style "toaster"
popups easier. The syntax is really easy especially if you are familiar with the
syntax of wxPython.

It has 2 main styles:

- ``TB_SIMPLE``: using this style, you will be able to specify a background image for
  ToasterBox, text properties as text colour, font and label;

- ``TB_COMPLEX``: this style will allow you to put almost any control inside a
  ToasterBox. You can add a panel in which you can put all the controls you like.

Both styles support the setting of ToasterBox position (on screen coordinates),
size, the time after which the ToasterBox is destroyed (linger), and the scroll
speed of ToasterBox.


Supported Platforms
===================

ToasterBox has been tested on the following platforms:

- Windows (verified on Windows XP, 2000)
- Linux
- Mac


Window Styles
=============

This class supports the following window styles:

==================== =========== ==================================================
Window Styles        Hex Value   Description
==================== =========== ==================================================
``TB_SIMPLE``                0x1 A simple ToasterBox, with background image and text customization can be created.
``TB_ONTIME``                0x1 `ToasterBox` will close after a specified amount of time.
``TB_COMPLEX``               0x2 ToasterBoxes with different degree of complexity can be created. You can add as  many controls as you want, provided that you call the AddPanel() method and pass  to it a dummy frame and a wx.Panel. See the demo for details.
``TB_ONCLICK``               0x2 `ToasterBox` can be closed by clicking anywhere on the `ToasterBox` frame.
``TB_DEFAULT_STYLE``   0x2008002 Default window style for `ToasterBox`, with no caption nor close box.
``TB_CAPTION``        0x22009806 `ToasterBox` will have a caption, with the possibility to set a title for the `ToasterBox` frame, and a close box.
==================== =========== ==================================================


Events Processing
=================

`No custom events are available for this class.`


License And Version
===================

ToasterBox is distributed under the wxPython license.

Latest revision: Andrea Gavana @ 01 Dec 2009, 14.00 GMT

Version 0.3

"""

import textwrap
import wx

from wx.lib.statbmp import GenStaticBitmap as StaticBitmap

# Define Window List, We Use It Globally
winlist = []

TB_SIMPLE = 1
""" A simple ToasterBox, with background image and text customization can be created. """
TB_COMPLEX = 2
""" ToasterBoxes with different degree of complexity can be created. You can add as """ \
""" many controls as you want, provided that you call the AddPanel() method and pass """ \
""" to it a dummy frame and a wx.Panel. See the demo for details. """
TB_DEFAULT_STYLE = wx.SIMPLE_BORDER | wx.STAY_ON_TOP | wx.FRAME_NO_TASKBAR
""" Default window style for `ToasterBox`, with no caption nor close box. """
TB_CAPTION = TB_DEFAULT_STYLE | wx.CAPTION | wx.SYSTEM_MENU | wx.CLOSE_BOX | wx.FRAME_TOOL_WINDOW
""" `ToasterBox` will have a caption, with the possibility to set a title """ \
""" for the `ToasterBox` frame, and a close box. """
TB_ONTIME = 1
""" `ToasterBox` will close after a specified amount of time. """
TB_ONCLICK = 2
""" `ToasterBox` can be closed by clicking anywhere on the `ToasterBox` frame. """

# scroll from up to down
TB_SCR_TYPE_UD = 1
# scroll from down to up
TB_SCR_TYPE_DU = 2

# ------------------------------------------------------------------------------ #
# Class ToasterBox
#    Main Class Implementation. It Is Basically A wx.Timer. It Creates And
#    Displays Popups And Handles The "Stacking".
# ------------------------------------------------------------------------------ #

class ToasterBox(wx.Timer):
    """
    ToasterBox is a cross-platform widget to make the creation of MSN style "toaster"
    popups easier.
    """

    def __init__(self, parent, tbstyle=TB_SIMPLE, windowstyle=TB_DEFAULT_STYLE,
                 closingstyle=TB_ONTIME, scrollType=TB_SCR_TYPE_DU):
        """
        Default class constructor.

        :param `parent`: the window parent;
        :param `tbstyle`: the L{ToasterBox} main style. Can be one of the following
         bits:

         ====================== ======= ================================
         `ToasterBox` Style      Value  Description
         ====================== ======= ================================
         ``TB_SIMPLE``              0x1 A simple `ToasterBox`, with background image and text customization can be created
         ``TB_COMPLEX``             0x2 `ToasterBoxes` with different degree of complexity can be created. You can add as many controls as you want, provided that you call the L{AddPanel} method and pass to it a dummy frame and a `wx.Panel`.
         ====================== ======= ================================

        :param `windowstyle`: this parameter influences the visual appearance of
         L{ToasterBox}, and can be one of the following styles:

         ====================== ========== ================================
         Window Style           Hex Value  Description
         ====================== ========== ================================
         ``TB_DEFAULT_STYLE``   0x2008002  Default window style for `ToasterBox`, with no caption nor close box.
         ``TB_CAPTION``         0x22009806 `ToasterBox` will have a caption, with the possibility to set a title for the `ToasterBox` frame, and a close box.
         ====================== ========== ================================
       
        :param `closingstyle`: the closing style for L{ToasterBox}. Can be one of the
         following bits:

         ==================== =========== ==================================================
         Closing Styles       Hex Value   Description
         ==================== =========== ==================================================
         ``TB_ONTIME``                0x1 `ToasterBox` will close after a specified amount of time.
         ``TB_ONCLICK``               0x2 `ToasterBox` can be closed by clicking anywhere on the `ToasterBox` frame.
         ==================== =========== ==================================================

        :param `scrollType`: the scrolling direction for L{ToasterBox}. Can be one of the
         following bits:

         ==================== =========== ==================================================
         Scroll Styles        Hex Value   Description
         ==================== =========== ==================================================
         ``TB_SCR_TYPE_UD``           0x1 `ToasterBox` will scroll from up to down
         ``TB_SCR_TYPE_DU``           0x2 `ToasterBox` will scroll from down to up
         ==================== =========== ==================================================
         
        """

        self._parent = parent
        self._sleeptime = 10
        self._pausetime = 1700
        self._popuptext = "default"
        self._popupposition = wx.Point(100,100)
        self._popuptop = wx.Point(0,0)
        self._popupsize = wx.Size(150, 170)

        self._backgroundcolour = wx.WHITE
        self._foregroundcolour = wx.BLACK
        self._textfont = wx.Font(8, wx.SWISS, wx.NORMAL, wx.NORMAL, False, "Verdana")

        self._bitmap = None

        self._tbstyle = tbstyle
        self._windowstyle = windowstyle
        self._closingstyle = closingstyle

        self._panel = None

        self._bottomright = wx.Point(wx.GetDisplaySize().GetWidth(),
                                    wx.GetDisplaySize().GetHeight())

        if parent is not None:
            parent.Bind(wx.EVT_ICONIZE, lambda evt: [w.Hide() for w in winlist])
 
        self._tb = ToasterBoxWindow(self._parent, self, self._tbstyle, self._windowstyle,
                                    self._closingstyle, scrollType=scrollType)


    def SetPopupPosition(self, pos):
        """
        Sets the L{ToasterBox} position on screen.

        :param `pos`: the widget position, an instance of `wx.Point`.        
        """

        self._popupposition = pos


    def SetPopupPositionByInt(self, pos):
        """
        Sets the L{ToasterBox} position on screen, at one of the screen corners.
 
        :param `pos`: an integer specifying the screen corner, namely:

         ============= ========================================
         Corner Number Position
         ============= ========================================
               0       Top left screen corner
               1       Top right screen corner
               2       Bottom left screen corner
               3       Bottom right screen corner
         ============= ========================================
         
        """

        w, h = wx.GetDisplaySize()
        self._bottomright = wx.Point(w, h)

        # top left
        if pos == 0:
            popupposition = wx.Point(0,0)
        # top right
        elif pos == 1:
            popupposition = wx.Point(w - self._popupsize[0], 0)
        # bottom left
        elif pos == 2:
            popupposition = wx.Point(0, h - self._popupsize[1])
        # bottom right
        elif pos == 3:
            popupposition = wx.Point(self._bottomright.x - self._popupsize[0],
                                     self._bottomright.y - self._popupsize[1])

        self._bottomright = wx.Point(popupposition.x + self._popupsize[0],
                                     popupposition.y + self._popupsize[1])

        self._popupposition = popupposition


    def SetPopupBackgroundColour(self, colour=None):
        """
        Sets the L{ToasterBox} background colour.

        :param `colour`: a valid `wx.Colour` object. If defaulted to ``None``, then
         the background colour will be white.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        if colour is None:
            colour = wx.WHITE

        self._backgroundcolour = colour


    def SetPopupTextColour(self, colour=None):
        """
        Sets the L{ToasterBox} foreground colour.

        :param `colour`: a valid `wx.Colour` object. If defaulted to ``None``, then
         the background colour will be black.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        if colour is None:
            colour = wx.BLACK

        self._foregroundcolour = colour


    def SetPopupTextFont(self, font=None):
        """
        Sets the L{ToasterBox} text font.

        :param `colour`: a valid `wx.Colour` object. If defaulted to ``None``, then
         a simple generic font will be generated.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        if font is None:
            font = wx.Font(8, wx.SWISS, wx.NORMAL, wx.NORMAL, False)

        self._textfont = font


    def SetPopupSize(self, size):
        """
        Sets the L{ToasterBox} size.

        :param `size`: the new control size, an instance of `wx.Size`.        
        """

        self._popupsize = size


    def SetPopupPauseTime(self, pausetime):
        """
        Sets the time after which the L{ToasterBox} is destroyed (linger).

        :param `pausetime`: the delay after which the control is destroyed, in seconds.
        """

        self._pausetime = pausetime


    def SetPopupBitmap(self, bitmap=None):
        """
        Sets the L{ToasterBox} background image.

        :param `bitmap`: a valid `wx.Bitmap` object. If defaulted to ``None``, then
         no background bitmap is used.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        if bitmap is not None:
            bitmap = wx.Bitmap(bitmap, wx.BITMAP_TYPE_BMP)

        self._bitmap = bitmap


    def SetPopupScrollSpeed(self, speed):
        """
        Sets the L{ToasterBox} scroll speed.

        :param `speed`: it is the pause time (in milliseconds) for every step in the
         `ScrollUp` method.
        """

        self._sleeptime = speed


    def SetPopupText(self, text):
        """
        Sets the L{ToasterBox} text label.

        :param `text`: the widget label.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        self._popuptext = text


    def AddPanel(self, panel):
        """
        Adds a panel to the L{ToasterBox}.

        :param `panel`: an instance of `wx.Window`.
        
        :note: Use this method only for a L{ToasterBox} created with the ``TB_COMPLEX`` style.
        """

        if not self._tbstyle & TB_COMPLEX:
            raise Exception("\nERROR: Panel Can Not Be Added When Using TB_SIMPLE ToasterBox Style")

        self._panel = panel


    def Play(self):
        """ Creates the L{ToasterBoxWindow}, that does all the job. """

        # create new window
        self._tb.SetPopupSize((self._popupsize[0], self._popupsize[1]))
        self._tb.SetPopupPosition((self._popupposition[0], self._popupposition[1]))
        self._tb.SetPopupPauseTime(self._pausetime)
        self._tb.SetPopupScrollSpeed(self._sleeptime)

        if self._tbstyle == TB_SIMPLE:
            self._tb.SetPopupTextColour(self._foregroundcolour)
            self._tb.SetPopupBackgroundColour(self._backgroundcolour)
            self._tb.SetPopupTextFont(self._textfont)

            if self._bitmap is not None:
                self._tb.SetPopupBitmap(self._bitmap)

            self._tb.SetPopupText(self._popuptext)

        if self._tbstyle == TB_COMPLEX:
            if self._panel is not None:
                self._tb.AddPanel(self._panel)

        # clean up the list
        self.CleanList()

        # check to see if there is already a window displayed
        # by looking at the linked list
        if len(winlist) > 0:
            # there ARE other windows displayed already
            # reclac where it should display
            self.MoveAbove(self._tb)

        # shift new window on to the list
        winlist.append(self._tb)

        if not self._tb.Play():
            # if we didn't show the window properly, remove it from the list
            winlist.remove(winlist[-1])
            # delete the object too
            self._tb.Destroy()
            return


    def MoveAbove(self, tb):
        """
        If a L{ToasterBox} already exists, move the new one above the existing one.

        :param `tb`: another instance of L{ToasterBox}.
        """

        # recalc where to place this popup
 
        self._tb.SetPopupPosition((self._popupposition[0], self._popupposition[1] -
                                   self._popupsize[1]*len(winlist)))


    def GetToasterBoxWindow(self):
        """ Returns the L{ToasterBox} frame. """

        return self._tb


    def SetTitle(self, title):
        """
        Sets the L{ToasterBox} title if it was created with ``TB_CAPTION`` window style.

        :param `title`: the L{ToasterBox} caption title.        
        """

        self._tb.SetTitle(title)


    def Notify(self):
        """ It's time to hide a L{ToasterBox}. """

        if len(winlist) == 0:
            return

        # clean the window list
        self.CleanList()

        # figure out how many blanks we have
        try:
            node = winlist[0]
        except:
            return

        if not node:
            return

        # move windows to fill in blank space
        for i in xrange(node.GetPosition()[1], self._popupposition[1], 4):
            if i > self._popupposition[1]:
                i = self._popupposition[1]

            # loop through all the windows
            for j in xrange(0, len(winlist)):
                ourNewHeight = i - (j*self._popupsize[1] - 8)
                tmpTb = winlist[j]
                # reset where the object THINKS its supposed to be
                tmpTb.SetPopupPosition((self._popupposition[0], ourNewHeight))
                # actually move it
                tmpTb.SetDimensions(self._popupposition[0], ourNewHeight, tmpTb.GetSize().GetWidth(),
                                    tmpTb.GetSize().GetHeight())

            wx.Usleep(self._sleeptime)


    def CleanList(self):
        """ Cleans the window list, erasing the stack of L{ToasterBox} objects. """

        if len(winlist) == 0:
            return

        node = winlist[0]
        while node:
            if not node.IsShown():
                winlist.remove(node)
                try:
                    node = winlist[0]
                except:
                    node = 0
            else:
                indx = winlist.index(node)
                try:
                    node = winlist[indx+1]
                except:
                    node = 0


# ------------------------------------------------------------------------------ #
# Class ToasterBoxWindow
#    This Class Does All The Job, By Handling Background Images, Text Properties
#    And Panel Adding. Depending On The Style You Choose, ToasterBoxWindow Will
#    Behave Differently In Order To Handle Widgets Inside It.
# ------------------------------------------------------------------------------ #

class ToasterBoxWindow(wx.Frame):
    """
    This class does all the job, by handling background images, text properties
    and panel adding. Depending on the style you choose, L{ToasterBoxWindow} will
    behave differently in order to handle widgets inside it.
    """
    
    def __init__(self, parent, parent2, tbstyle, windowstyle, closingstyle,
                 scrollType=TB_SCR_TYPE_DU):
        """
        Default class constructor.
        Used internally. Do not call directly this class in your application!

        :param `parent`: the window parent;
        :param `parent2`: the L{ToasterBox} calling this window;
        :param `tbstyle`: the L{ToasterBoxWindow} main style. Can be one of the following
         bits:

         ====================== ======= ================================
         `ToasterBox` Style      Value  Description
         ====================== ======= ================================
         ``TB_SIMPLE``              0x1 A simple `ToasterBox`, with background image and text customization can be created
         ``TB_COMPLEX``             0x2 `ToasterBoxes` with different degree of complexity can be created. You can add as many controls as you want, provided that you call the L{AddPanel} method and pass to it a dummy frame and a `wx.Panel`.
         ====================== ======= ================================

        :param `windowstyle`: this parameter influences the visual appearance of
         L{ToasterBoxWindow}, and can be one of the following styles:

         ====================== ========== ================================
         Window Style           Hex Value  Description
         ====================== ========== ================================
         ``TB_DEFAULT_STYLE``   0x2008002  Default window style for `ToasterBox`, with no caption nor close box.
         ``TB_CAPTION``         0x22009806 `ToasterBox` will have a caption, with the possibility to set a title for the `ToasterBox` frame, and a close box.
         ====================== ========== ================================
       
        :param `closingstyle`: the closing style for L{ToasterBoxWindow}. Can be one of the
         following bits:

         ==================== =========== ==================================================
         Closing Styles       Hex Value   Description
         ==================== =========== ==================================================
         ``TB_ONTIME``                0x1 `ToasterBox` will close after a specified amount of time.
         ``TB_ONCLICK``               0x2 `ToasterBox` can be closed by clicking anywhere on the `ToasterBox` frame.
         ==================== =========== ==================================================

        :param `scrollType`: the scrolling direction for L{ToasterBoxWindow}. Can be one of the
         following bits:

         ==================== =========== ==================================================
         Scroll Styles        Hex Value   Description
         ==================== =========== ==================================================
         ``TB_SCR_TYPE_UD``           0x1 `ToasterBox` will scroll from up to down
         ``TB_SCR_TYPE_DU``           0x2 `ToasterBox` will scroll from down to up
         ==================== =========== ==================================================

        """

        wx.Frame.__init__(self, parent, wx.ID_ANY, "window", wx.DefaultPosition,
                         wx.DefaultSize, style=windowstyle | wx.CLIP_CHILDREN)

        self._starttime = wx.GetLocalTime()
        self._parent2 = parent2
        self._parent = parent
        self._sleeptime = 10
        self._step = 4
        self._pausetime = 1700
        self._textcolour = wx.BLACK
        self._popuptext = "Change Me!"
        # the size we want the dialog to be
        framesize = wx.Size(150, 170)
        self._count = 1
        self._tbstyle = tbstyle
        self._windowstyle = windowstyle
        self._closingstyle = closingstyle
        self._scrollType = scrollType

        if tbstyle == TB_COMPLEX:
            self.sizer = wx.BoxSizer(wx.VERTICAL)
        else:
            self._staticbitmap = None

        if self._windowstyle == TB_CAPTION:
            self.Bind(wx.EVT_CLOSE, self.OnClose)
            self.SetTitle("")

        if self._closingstyle & TB_ONCLICK and self._windowstyle != TB_CAPTION:
            self.Bind(wx.EVT_LEFT_DOWN, self.OnMouseDown)

        self._bottomright = wx.Point(wx.GetDisplaySize().GetWidth(),
                                     wx.GetDisplaySize().GetHeight())

        self.SetDimensions(self._bottomright.x, self._bottomright.y,
                           framesize.GetWidth(), framesize.GetHeight())


    def OnClose(self, event):
        """
        Handles the ``wx.EVT_CLOSE`` event for L{ToasterBoxWindow}.

        :param `event`: a `wx.CloseEvent` event to be processed.
        """

        self.NotifyTimer(None)
        event.Skip()


    def OnMouseDown(self, event):
        """
        Handles the ``wx.EVT_LEFT_DOWN`` event for L{ToasterBoxWindow}.

        :param `event`: a `wx.MouseEvent` event to be processed.
        """

        self.NotifyTimer(None)
        event.Skip()


    def SetPopupBitmap(self, bitmap):
        """
        Sets the L{ToasterBox} background image.

        :param `bitmap`: a valid `wx.Bitmap` object. If defaulted to ``None``, then
         no background bitmap is used.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        bitmap = bitmap.ConvertToImage()
        xsize, ysize = self.GetSize()
        bitmap = bitmap.Scale(xsize, ysize)
        bitmap = bitmap.ConvertToBitmap()
        self._staticbitmap = StaticBitmap(self, -1, bitmap, pos=(0,0))

        if self._closingstyle & TB_ONCLICK and self._windowstyle != TB_CAPTION:
            self._staticbitmap.Bind(wx.EVT_LEFT_DOWN, self.OnMouseDown)


    def SetPopupSize(self, size):
        """
        Sets the L{ToasterBox} size.

        :param `size`: the new control size, an instance of `wx.Size`.        
        """

        self.SetDimensions(self._bottomright.x, self._bottomright.y, size[0], size[1])


    def SetPopupPosition(self, pos):
        """
        Sets the L{ToasterBox} position on screen.

        :param `pos`: the widget position, an instance of `wx.Point`.        
        """

        self._bottomright = wx.Point(pos[0] + self.GetSize().GetWidth(),
                                     pos[1] + self.GetSize().GetHeight())
        self._dialogtop = pos


    def SetPopupPositionByInt(self, pos):
        """
        Sets the L{ToasterBox} position on screen, at one of the screen corners.
 
        :param `pos`: an integer specifying the screen corner, namely:

         ============= ========================================
         Corner Number Position
         ============= ========================================
               0       Top left screen corner
               1       Top right screen corner
               2       Bottom left screen corner
               3       Bottom right screen corner
         ============= ========================================
         
        """

        w, h = wx.GetDisplaySize()
        self._bottomright = wx.Point(w, h)

        # top left
        if pos == 0:
            popupposition = wx.Point(0, 0)
        # top right
        elif pos == 1:
            popupposition = wx.Point(w - self._popupsize[0], 0)
        # bottom left
        elif pos == 2:
           popupposition = wx.Point(0, h - self._popupsize[1])
         # bottom right
        elif pos == 3:
            popupposition = wx.Point(self._bottomright.x - self._popupsize[0],
                                     self._bottomright.y - self._popupsize[1])

        self._bottomright = wx.Point(popupposition.x + self._popupsize[0],
                                     popupposition.y + self._popupsize[1])

        self._dialogtop = popupposition


    def SetPopupPauseTime(self, pausetime):
        """
        Sets the time after which the L{ToasterBox} is destroyed (linger).

        :param `pausetime`: the delay after which the control is destroyed, in seconds.
        """

        self._pausetime = pausetime


    def SetPopupScrollSpeed(self, speed):
        """
        Sets the L{ToasterBox} scroll speed.

        :param `speed`: it is the pause time (in milliseconds) for every step in the
         L{ScrollUp} method.
        """

        self._sleeptime = speed


    def AddPanel(self, panel):
        """
        Adds a panel to the L{ToasterBox}.

        :param `panel`: an instance of `wx.Window`.
        
        :note: Use this method only for a L{ToasterBox} created with the ``TB_COMPLEX`` style.
        """

        if not self._tbstyle & TB_COMPLEX:
            raise Exception("\nERROR: Panel Can Not Be Added When Using TB_SIMPLE ToasterBox Style")

        self.sizer.Add(panel, 1, wx.EXPAND)
        self.sizer.Layout()
        self.SetSizer(self.sizer)

        if self._closingstyle & TB_ONCLICK and self._windowstyle != TB_CAPTION:
            panel.Bind(wx.EVT_LEFT_DOWN, self.OnMouseDown)


    def SetPopupText(self, text):
        """
        Sets the L{ToasterBox} text label.

        :param `text`: the widget label.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        self._popuptext = text


    def SetPopupTextFont(self, font):
        """
        Sets the L{ToasterBox} text font.

        :param `colour`: a valid `wx.Colour` object. If defaulted to ``None``, then
         a simple generic font will be generated.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        self._textfont = font


    def GetPopupText(self):
        """
        Returns the L{ToasterBox} text.

        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.       
        """

        return self._popuptext


    def Play(self):
        """ Creates the L{ToasterBoxWindow}, that does all the job. """

        # do some checks to make sure this window is valid
        if self._bottomright.x < 1 or self._bottomright.y < 1:
            return False

        if self.GetSize().GetWidth() < 50 or self.GetSize().GetWidth() < 50:
            # toasterbox launches into a endless loop for some reason
            # when you try to make the window too small.
            return False

        self.ScrollUp()
        timerid = wx.NewId()
        self.showtime = wx.Timer(self, timerid)
        self.showtime.Start(self._pausetime)
        self.Bind(wx.EVT_TIMER, self.NotifyTimer, id=timerid)

        return True


    def NotifyTimer(self, event):
        """ Hides gradually the L{ToasterBoxWindow}. """

        self.showtime.Stop()
        del self.showtime
        self.ScrollDown()


    def SetPopupBackgroundColour(self, colour):
        """
        Sets the L{ToasterBox} background colour.

        :param `colour`: a valid `wx.Colour` object. If defaulted to ``None``, then
         the background colour will be white.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        self.SetBackgroundColour(colour)


    def SetPopupTextColour(self, colour):
        """
        Sets the L{ToasterBox} foreground colour.

        :param `colour`: a valid `wx.Colour` object. If defaulted to ``None``, then
         the background colour will be black.
         
        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.
        """

        self._textcolour = colour


    def ScrollUp(self):
        """ Scrolls the L{ToasterBox} up, which means gradually showing it. """

        self.Show(True)

        # walk the Y value up in a raise motion
        xpos = self.GetPosition().x
        ypos = self._bottomright[1]
        windowsize = 0

        # checking the type of the scroll (from up to down or from down to up)
        if self._scrollType == TB_SCR_TYPE_UD:
            start = self._dialogtop[1]
            stop = ypos
            step = self._step
        elif self._scrollType == TB_SCR_TYPE_DU:
            start = ypos
            stop = self._dialogtop[1]
            step = -self._step
        else:
            errMsg = ("scrollType not supported (in ToasterBoxWindow.ScrollUp): %s" %
                      self._scrollType)
            raise ValueError(errMsg)

        for i in xrange(start, stop, step):
            if i < self._dialogtop[1]:
                i = self._dialogtop[1]

            windowsize = windowsize + self._step

            # checking the type of the scroll (from up to down or from down to up)
            if self._scrollType == TB_SCR_TYPE_UD:
                dimY = self._dialogtop[1]
            elif self._scrollType == TB_SCR_TYPE_DU:
                dimY = i
            else:
                errMsg = ("scrollType not supported (in ToasterBoxWindow.ScrollUp): %s" %
                          self._scrollType)
                raise ValueError(errMsg)

            self.SetDimensions(self._dialogtop[0], dimY, self.GetSize().GetWidth(),
                               windowsize)

            if self._tbstyle == TB_SIMPLE:
                self.DrawText()

            wx.Usleep(self._sleeptime)
            self.Update()
            self.Refresh()

        self.Update()

        if self._tbstyle == TB_SIMPLE:
            self.DrawText()

        self.SetFocus()


    def ScrollDown(self):
        """ Scrolls the L{ToasterBox} down, which means gradually hiding it. """

        # walk down the Y value
        windowsize = self.GetSize().GetHeight()

        # checking the type of the scroll (from up to down or from down to up)
        if self._scrollType == TB_SCR_TYPE_UD:
            start = self._bottomright.y
            stop = self._dialogtop[1]
            step = -self._step
        elif self._scrollType == TB_SCR_TYPE_DU:
            start = self._dialogtop[1]
            stop = self._bottomright.y
            step = self._step
        else:
            errMsg = ("scrollType not supported (in ToasterBoxWindow.ScrollUp): %s" %
                      self._scrollType)
            raise ValueError(errMsg)

        for i in xrange(start, stop, step):
            if i > self._bottomright.y:
                i = self._bottomright.y

            windowsize = windowsize - self._step

            if windowsize <= 0:
               break
            
            # checking the type of the scroll (from up to down or from down to up)
            if self._scrollType == TB_SCR_TYPE_UD:
                dimY = self._dialogtop[1]
            elif self._scrollType == TB_SCR_TYPE_DU:
                dimY = i
            else:
                errMsg = ("scrollType not supported (in ToasterBoxWindow.ScrollUp): %s" %
                          self._scrollType)
                raise ValueError(errMsg)

            self.SetDimensions(self._dialogtop[0], dimY,
                               self.GetSize().GetWidth(), windowsize)

            wx.Usleep(self._sleeptime)
            self.Refresh()

        self.Hide()
        if self._parent2:
            self._parent2.Notify()


    def DrawText(self):
        """ Draws the text label for a L{ToasterBox} with ``TB_SIMPLE`` style set. """
      
        if self._staticbitmap is not None:
            dc = wx.ClientDC(self._staticbitmap)
        else:
            dc = wx.ClientDC(self)
           
        dc.SetFont(self._textfont)

        if not hasattr(self, "text_coords"):
            self._getTextCoords(dc)

        fg = dc.GetTextForeground()
        dc.SetTextForeground(self._textcolour)
        dc.DrawTextList(*self.text_coords)
        dc.SetTextForeground(fg)


    def _getTextCoords(self, dc):
        """
        Draw the user specified text.

        :param `dc`: an instance of `wx.DC`.

        :note: Use this method only for a L{ToasterBox} created with the ``TB_SIMPLE`` style.        
        """

        # border from sides and top to text (in pixels)
        border = 7
        # how much space between text lines
        textPadding = 2

        pText = self.GetPopupText()

        max_len = len(pText)

        tw, th = self._parent2._popupsize

        if self._windowstyle == TB_CAPTION:
            th = th - 20

        while 1:
            lines = textwrap.wrap(pText, max_len)

            for line in lines:
                w, h = dc.GetTextExtent(line)
                if w > tw - border * 2:
                    max_len -= 1
                    break
            else:
                break

        fh = 0
        for line in lines:
            w, h = dc.GetTextExtent(line)
            fh += h + textPadding
        y = (th - fh) / 2; coords = []

        for line in lines:
            w, h = dc.GetTextExtent(line)
            x = (tw - w) / 2
            coords.append((x, y))
            y += h + textPadding

        self.text_coords = (lines, coords)
