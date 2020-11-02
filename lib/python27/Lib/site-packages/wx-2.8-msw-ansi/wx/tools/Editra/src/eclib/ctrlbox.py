###############################################################################
# Name: ctrlbox.py                                                            #
# Purpose: Container Window helper class                                      #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2008 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
Editra Control Library: ControlBox

Sizer managed panel class with support for a toolbar like control that can be
placed on the top/bottom of the main window area, multiple control bars are also
possible.

Class ControlBar:

Toolbar like control with automatic item spacing and layout.

Styles:
  - CTRLBAR_STYLE_DEFAULT: Plain background
  - CTRLBAR_STYLE_GRADIENT: Draw the bar with a vertical gradient.
  - CTRLBAR_STYLE_BORDER_BOTTOM: add a border to the bottom
  - CTRLBAR_STYLE_BORDER: add a border to the top

Class ControlBox:

The ControlBox is a sizer managed panel that supports easy creation of windows
that require a sandwich like layout.

+---------------------------------------+
| ControlBar                            |
+---------------------------------------+
|                                       |
|                                       |
|          MainWindow Area              |
|                                       |
|                                       |
|                                       |
+---------------------------------------+
| ControlBar                            |
+---------------------------------------+

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: ctrlbox.py 61856 2009-09-08 03:42:32Z CJP $"
__revision__ = "$Revision: 61856 $"

__all__ = ["ControlBox", "CTRLBOX_NAME_STR",

           "ControlBar", "ControlBarEvent",
           "CTRLBAR_STYLE_DEFAULT", "CTRLBAR_STYLE_GRADIENT",
           "CTRLBAR_STYLE_BORDER_TOP", "CTRLBAR_STYLE_BORDER_BOTTOM",
           "EVT_CTRLBAR", "edEVT_CTRLBAR", "CTRLBAR_NAME_STR",

           "SegmentBar", "SegmentBarEvent",
           "EVT_SEGMENT_SELECTED", "edEVT_SEGMENT_SELECTED",
           "EVT_SEGMENT_CLOSE", "edEVT_SEGMENT_CLOSE",
           "CTRLBAR_STYLE_LABELS", "CTRLBAR_STYLE_NO_DIVIDERS",
           "SEGBTN_OPT_CLOSEBTNL", "SEGBTN_OPT_CLOSEBTNR",
           "SEGBAR_NAME_STR", "SEGMENT_HT_NOWHERE",
           "SEGMENT_HT_SEG", "SEGMENT_HT_X_BTN"
]

#--------------------------------------------------------------------------#
# Dependancies
import math
import wx

# Local Imports
from eclutil import AdjustColour, DrawCircleCloseBmp

#--------------------------------------------------------------------------#
# Globals

#-- Control Name Strings --#
CTRLBAR_NAME_STR = u'EditraControlBar'
CTRLBOX_NAME_STR = u'EditraControlBox'
SEGBAR_NAME_STR = u'EditraSegmentBar'

#-- Control Style Flags --#

# ControlBar / SegmentBar Style Flags
CTRLBAR_STYLE_DEFAULT       = 0
CTRLBAR_STYLE_GRADIENT      = 1     # Paint the bar with a gradient
CTRLBAR_STYLE_BORDER_BOTTOM = 2     # Add a border to the bottom
CTRLBAR_STYLE_BORDER_TOP    = 4     # Add a border to the top
CTRLBAR_STYLE_LABELS        = 8     # Draw labels under the icons (SegmentBar)
CTRLBAR_STYLE_NO_DIVIDERS   = 16    # Don't draw dividers between segments

# Segment Button Options
SEGBTN_OPT_NONE          = 1     # No options set.
SEGBTN_OPT_CLOSEBTNL     = 2     # Close button on the segments left side.
SEGBTN_OPT_CLOSEBTNR     = 4     # Close button on the segment right side.

# Hit test locations
SEGMENT_HT_NOWHERE = 0
SEGMENT_HT_SEG     = 1
SEGMENT_HT_X_BTN   = 2

# Segment States
SEGMENT_STATE_NONE = 0  # Hover no where
SEGMENT_STATE_SEG  = 1  # Hover on segment
SEGMENT_STATE_X    = 2  # Hover on segment x button

# ControlBar event for items added by AddTool
edEVT_CTRLBAR = wx.NewEventType()
EVT_CTRLBAR = wx.PyEventBinder(edEVT_CTRLBAR, 1)
class ControlBarEvent(wx.PyCommandEvent):
    """ControlBar Button Event"""

edEVT_SEGMENT_SELECTED = wx.NewEventType()
EVT_SEGMENT_SELECTED = wx.PyEventBinder(edEVT_SEGMENT_SELECTED, 1)
edEVT_SEGMENT_CLOSE = wx.NewEventType()
EVT_SEGMENT_CLOSE = wx.PyEventBinder(edEVT_SEGMENT_CLOSE, 1)
class SegmentBarEvent(wx.PyCommandEvent):
    """SegmentBar Button Event"""
    def __init__(self, etype, id=0):
        wx.PyCommandEvent.__init__(self, etype, id)

        # Attributes
        self.notify = wx.NotifyEvent(etype, id)
        self._pre = -1
        self._cur = -1

    def GetPreviousSelection(self):
        """Get the previously selected segment
        @return: int

        """
        return self._pre

    def GetCurrentSelection(self):
        """Get the currently selected segment
        @return: int

        """
        return self._cur

    def IsAllowed(self):
        """Is the event allowed to propagate
        @return: bool

        """
        return self.notify.IsAllowed()

    def SetSelections(self, previous=-1, current=-1):
        """Set the events selection
        @keyword previous: previously selected button index (int)
        @keyword previous: currently selected button index (int)

        """
        self._pre = previous
        self._cur = current

    def Veto(self):
        """Veto the event"""
        self.notify.Veto()

#--------------------------------------------------------------------------#

class ControlBox(wx.PyPanel):
    """Simple managed panel helper class that allows for adding and
    managing the position of a small toolbar like panel.
    @see: L{ControlBar}

    """
    def __init__(self, parent, id=wx.ID_ANY,
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=wx.TAB_TRAVERSAL|wx.NO_BORDER,
                 name=CTRLBOX_NAME_STR):
        wx.PyPanel.__init__(self, parent, id, pos, size, style, name)

        # Attributes
        self._sizer = wx.BoxSizer(wx.VERTICAL)
        self._topb = None
        self._main = None
        self._botb = None

        # Layout
        self.SetSizer(self._sizer)
        self.SetAutoLayout(True)

    def ChangeWindow(self, window):
        """Change the main window area, and return the current window
        @param window: Any window/panel like object
        @return: the old window or None

        """
        rwindow = None
        if self.GetWindow() is None or not isinstance(self._main, wx.Window):
            del self._main
            if self._topb is None:
                self._sizer.Add(window, 1, wx.EXPAND)
            else:
                self._sizer.Insert(1, window, 1, wx.EXPAND)
        else:
            self._sizer.Replace(self._main, window)
            rwindow = self._main

        self._main = window
        return rwindow

    def CreateControlBar(self, pos=wx.TOP):
        """Create a ControlBar at the given position if one does not
        already exist.
        @keyword pos: wx.TOP (default) or wx.BOTTOM
        @postcondition: A top aligned L{ControlBar} is created.
        @return: ControlBar

        """
        cbar = self.GetControlBar(pos)
        if cbar is None:
            cbar = ControlBar(self, size=(-1, 24),
                              style=CTRLBAR_STYLE_GRADIENT)

            self.SetControlBar(cbar, pos)
        return cbar

    def GetControlBar(self, pos=wx.TOP):
        """Get the L{ControlBar} used by this window
        @param pos: wx.TOP or wx.BOTTOM
        @return: ControlBar or None

        """
        if pos == wx.TOP:
            cbar = self._topb
        else:
            cbar = self._botb

        return cbar

    def GetWindow(self):
        """Get the main display window
        @return: Window or None

        """
        return self._main

    def ReplaceControlBar(self, ctrlbar, pos=wx.TOP):
        """Replace the L{ControlBar} at the given position
        with the given ctrlbar and return the bar that was
        replaced or None.
        @param ctrlbar: L{ControlBar}
        @keyword pos: Postion
        @return: L{ControlBar} or None

        """
        tbar = self.GetControlBar(pos)
        rbar = None
        if pos == wx.TOP:
            if tbar is None:
                self._sizer.Insert(0, ctrlbar, 0, wx.EXPAND)
            else:
                self._sizer.Replace(self._topb, ctrlbar)
                rbar = self._topb

            self._topb = ctrlbar
        else:
            if tbar is None:
                self._sizer.Add(ctrlbar, 0, wx.EXPAND)
            else:
                self._sizer.Replace(self._botb, ctrlbar)
                rbar = self._botb

            self._botb = ctrlbar

        return rbar

    def SetControlBar(self, ctrlbar, pos=wx.TOP):
        """Set the ControlBar used by this ControlBox
        @param ctrlbar: L{ControlBar}
        @keyword pos: wx.TOP/wx.BOTTOM

        """
        tbar = self.GetControlBar(pos)
        if pos == wx.TOP:
            if tbar is None:
                self._sizer.Insert(0, ctrlbar, 0, wx.EXPAND)
            else:
                self._sizer.Replace(self._topb, ctrlbar)

                try:
                    self._topb.Destroy()
                except wx.PyDeadObjectError:
                    pass

            self._topb = ctrlbar
        else:
            if tbar is None:
                self._sizer.Add(ctrlbar, 0, wx.EXPAND)
            else:
                self._sizer.Replace(self._botb, ctrlbar)

                try:
                    self._botb.Destroy()
                except wx.PyDeadObjectError:
                    pass

            self._botb = ctrlbar

    def SetWindow(self, window):
        """Set the main window control portion of the box. This will be the
        main central item shown in the box
        @param window: Any window/panel like object

        """
        if self.GetWindow() is None:
            if (self._topb and self._botb is None) or \
               (self._topb is None and self._botb is None):
                self._sizer.Add(window, 1, wx.EXPAND)
            elif self._botb and self._topb is None:
                self._sizer.Insert(0, window, 1, wx.EXPAND)
            else:
                self._sizer.Insert(1, window, 1, wx.EXPAND)
        else:
            self._sizer.Replace(self._main, window)

            try:
                self._main.Destroy()
            except wx.PyDeadObjectError:
                pass

        self._main = window

#--------------------------------------------------------------------------#

class ControlBar(wx.PyPanel):
    """Toolbar like control container for use with a L{ControlBox}. It
    uses a panel with a managed sizer as a convenient way to add a small
    bar with various controls in it to any window.

    """
    def __init__(self, parent, id=wx.ID_ANY,
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=CTRLBAR_STYLE_DEFAULT,
                 name=CTRLBAR_NAME_STR):
        wx.PyPanel.__init__(self, parent, id, pos, size,
                            wx.TAB_TRAVERSAL|wx.NO_BORDER, name)

        # Attributes
        self._style = style
        self._sizer = wx.BoxSizer(wx.HORIZONTAL)
        self._tools = dict(simple=list())
        self._spacing = (5, 5)

        # Drawing related
        color = wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE)
        if wx.Platform != '__WXMAC__':
            self._color2 = AdjustColour(color, 15)
            self._color = AdjustColour(color, -10)
        else:
            self._color2 = AdjustColour(color, 15)
            self._color = AdjustColour(color, -20)

        pcolor = tuple([min(190, x) for x in AdjustColour(self._color, -25)])
        self._pen = wx.Pen(pcolor, 1)

        # Setup
        msizer = wx.BoxSizer(wx.VERTICAL)
        spacer = (0, 0)
        msizer.Add(spacer, 0)
        msizer.Add(self._sizer, 1, wx.EXPAND)
        msizer.Add(spacer, 0)
        self.SetSizer(msizer)
        self.SetAutoLayout(True)

        # Event Handlers
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_BUTTON, self._DispatchEvent)

    def _DispatchEvent(self, evt):
        """Translate the button events generated by the controls added by
        L{AddTool} to L{ControlBarEvent}'s.

        """
        e_id = evt.GetId()
        if e_id in self._tools['simple']:
            cb_evt = ControlBarEvent(edEVT_CTRLBAR, e_id)
            wx.PostEvent(self.GetParent(), cb_evt)
        else:
            evt.Skip()

    def AddControl(self, control, align=wx.ALIGN_LEFT, stretch=0):
        """Add a control to the bar
        @param control: The control to add to the bar
        @keyword align: wx.ALIGN_LEFT or wx.ALIGN_RIGHT
        @keyword stretch: The controls proportions 0 for normal, 1 for expand

        """
        if wx.Platform == '__WXMAC__':
            if hasattr(control, 'SetWindowVariant'):
                control.SetWindowVariant(wx.WINDOW_VARIANT_SMALL)

        if align == wx.ALIGN_LEFT:
            self._sizer.Add(self._spacing, 0)
            self._sizer.Add(control, stretch, align|wx.ALIGN_CENTER_VERTICAL)
        else:
            self._sizer.Add(control, stretch, align|wx.ALIGN_CENTER_VERTICAL)
            self._sizer.Add(self._spacing, 0)

        self.Layout()

    def AddSpacer(self, width, height):
        """Add a fixed size spacer to the control bar
        @param width: width of the spacer
        @param height: height of the spacer

        """
        self._sizer.Add((width, height), 0)

    def AddStretchSpacer(self):
        """Add an expanding spacer to the bar that will stretch and
        contract when the window changes size.

        """
        self._sizer.AddStretchSpacer(2)

    def AddTool(self, tid, bmp, help='', align=wx.ALIGN_LEFT):
        """Add a simple bitmap button tool to the control bar
        @param tid: Tool Id
        @param bmp: Tool bitmap
        @keyword help: Short help string
        @keyword align: wx.ALIGN_LEFT or wx.ALIGN_RIGHT

        """
        tool = wx.BitmapButton(self, tid, bmp, style=wx.NO_BORDER)
        if wx.Platform == '__WXGTK__':
            tool.SetMargins(0, 0)
            spacer = (0, 0)
        else:
            spacer = self._spacing
        tool.SetToolTipString(help)

        self._tools['simple'].append(tool.GetId())
        if align == wx.ALIGN_LEFT:
            self._sizer.Add(spacer, 0)
            self._sizer.Add(tool, 0, align|wx.ALIGN_CENTER_VERTICAL)
        else:
            self._sizer.Add(spacer, 0)
            self._sizer.Add(tool, 0, align|wx.ALIGN_CENTER_VERTICAL)

    def GetControlSizer(self):
        """Get the sizer that is used to layout the contols (horizontal sizer)
        @return: wx.BoxSizer

        """
        return self._sizer

    def GetControlSpacing(self):
        """Get the spacing used between controls
        @return: size tuple

        """
        return self._spacing

    def DoPaintBackground(self, dc, rect, color, color2):
        """Paint the background of the given rect based on the style of
        the control bar.
        @param dc: DC to draw on
        @param rect: wx.Rect
        @param color: Pen/Base gradient color
        @param color2: Gradient end color

        """
        # Paint the gradient
        if self._style & CTRLBAR_STYLE_GRADIENT:
            if isinstance(dc, wx.GCDC):
                gc = dc.GetGraphicsContext()
            else:
                gc = wx.GraphicsContext.Create(dc)
            grad = gc.CreateLinearGradientBrush(rect.x, .5, rect.x, rect.GetHeight(),
                                                color2, color)

            gc.SetPen(gc.CreatePen(self._pen))
            gc.SetBrush(grad)
            gc.DrawRectangle(rect.x, 0, rect.GetWidth() - 0.5, rect.GetHeight() - 0.5)

        dc.SetPen(wx.Pen(color, 1))
        # Add a border to the bottom
        if self._style & CTRLBAR_STYLE_BORDER_BOTTOM:
            dc.DrawLine(rect.x, rect.GetHeight() - 1, rect.GetWidth(), rect.GetHeight() - 1)

        # Add a border to the top
        if self._style & CTRLBAR_STYLE_BORDER_TOP:
            dc.DrawLine(rect.x, 1, rect.GetWidth(), 1)

    def OnPaint(self, evt):
        """Paint the background to match the current style
        @param evt: wx.PaintEvent

        """
        dc = wx.AutoBufferedPaintDCFactory(self)
        gc = wx.GCDC(dc)
        rect = self.GetClientRect()

        self.DoPaintBackground(gc, rect, self._color, self._color2)

        evt.Skip()

    def SetToolSpacing(self, px):
        """Set the spacing to use between tools/controls.
        @param px: int (number of pixels)
        @todo: dynamically update existing layouts

        """
        self._spacing = (px, px)

    def SetVMargin(self, top, bottom):
        """Set the Vertical margin used for spacing controls from the
        top and bottom of the bars edges.
        @param top: Top margin in pixels
        @param bottom: Bottom maring in pixels

        """
        sizer = self.GetSizer()
        sizer.GetItem(0).SetSpacer((top, top))
        sizer.GetItem(2).SetSpacer((bottom, bottom))
        sizer.Layout()

    def SetWindowStyle(self, style):
        """Set the style flags of this window
        @param style: long

        """
        self._style = style
        self.Refresh()

#--------------------------------------------------------------------------#

class SegmentBar(ControlBar):
    """Simple toolbar like control that displays bitmaps and optionaly
    labels below each bitmap. The bitmaps are turned into a toggle button
    where only one segment in the bar can be selected at one time.

    """
    HPAD = 5
    VPAD = 3
    def __init__(self, parent, id=wx.ID_ANY,
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=CTRLBAR_STYLE_DEFAULT,
                 name=SEGBAR_NAME_STR):
        ControlBar.__init__(self, parent, id, pos, size, style, name)

        # Attributes
        self._buttons = list()
        self._segsize = (0, 0)
        self._selected = -1
        self._scolor1 = AdjustColour(self._color, -20)
        self._scolor2 = AdjustColour(self._color2, -20)
        self._spen = wx.Pen(AdjustColour(self._pen.GetColour(), -25))
        self._x_clicked_before = False

        if wx.Platform == '__WXMAC__':
            self.SetWindowVariant(wx.WINDOW_VARIANT_SMALL)

        # Event Handlers
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.Bind(wx.EVT_MOTION, self.OnMouseMove)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)

    def AddSegment(self, id, bmp, label=u''):
        """Add a segment to the bar
        @param id: button id
        @param bmp: wx.Bitmap
        @param label: string

        """
        assert bmp.IsOk()
        lsize = self.GetTextExtent(label)
        # TODO: Refactor to a Segment Class to manage these data members
        self._buttons.append(dict(id=id, bmp=bmp, label=label,
                                  lsize=lsize, bsize=bmp.GetSize(),
                                  bx1=0, bx2=0, opts=0,
                                  selected=False,
                                  x_state=SEGMENT_STATE_NONE))
        self.InvalidateBestSize()
        self.Refresh()

    def DoDrawButton(self, dc, xpos, bidx, selected=False, draw_label=False):
        """Draw a button
        @param dc: DC to draw on
        @param xpos: X coordinate
        @param bidx: button dict
        @keyword selected: is this the selected button (bool)
        @keyword draw_label: draw the label (bool)
        return: int (next xpos)

        """
        button = self._buttons[bidx]
        rect = self.GetRect()
        height = rect.GetHeight()
        bsize = button['bsize']

        bxpos = ((self._segsize[0] / 2) - (bsize.GetWidth() / 2)) + xpos
        bpos = (bxpos, SegmentBar.VPAD)
        rside = xpos + self._segsize[0]
        brect = wx.Rect(xpos, 0, rside - xpos, height)
        if selected:
            self.DoPaintBackground(dc, brect, self._scolor1, self._scolor2)

        bmp = button['bmp']
        dc.DrawBitmap(bmp, bpos[0], bpos[1], bmp.GetMask() != None)

        if draw_label:
            lcolor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_WINDOWTEXT)
            dc.SetTextForeground(lcolor)
            twidth, theight = button['lsize']
            typos = height - theight - 2
            trect = wx.Rect(xpos, typos, self._segsize[0], theight + 3)
            dc.DrawLabel(button['label'], trect, wx.ALIGN_CENTER)

        if not selected:
            if not (self._style & CTRLBAR_STYLE_NO_DIVIDERS):
                dc.SetPen(self._pen)
                dc.DrawLine(xpos, 0, xpos, height)
                dc.DrawLine(rside, 0, rside, height)
        else:
            dc.SetPen(self._spen)
            tmpx = xpos + 1
            trside = rside - 1
            dc.DrawLine(tmpx, 0, tmpx, height)
            dc.DrawLine(trside, 0, trside, height)

            tpen = wx.Pen(self._spen.GetColour())
            tpen.SetJoin(wx.JOIN_BEVEL)
            mpoint = height / 2
            mlen = mpoint / 2
            dc.DrawLine(tmpx + 1, mpoint, tmpx, 0)
            dc.DrawLine(tmpx + 1, mpoint, tmpx, height)
            dc.DrawLine(trside - 1, mpoint, trside, 0)
            dc.DrawLine(trside - 1, mpoint, trside, height)

        button['bx1'] = xpos + 1
        button['bx2'] = rside - 1
        button['selected'] = selected

        if self.SegmentHasCloseButton(bidx):
            brect = wx.Rect(button['bx1'], 0, button['bx2'] - (xpos - 1), height)
            self.DoDrawCloseBtn(dc, button, brect)

        return rside

    def DoDrawCloseBtn(self, gcdc, button, rect):
        """Draw the close button on the segment
        @param gcdc: Device Context
        @param button: Segment Dict
        @param rect: Segment Rect

        """
        if button['opts'] & SEGBTN_OPT_CLOSEBTNL:
            x = rect.x + 8
            y = rect.y + 6
        else:
            x = (rect.x + rect.GetWidth()) - 8
            y = rect.y + 6

        color = self._scolor2
        if button['selected']:
            color = AdjustColour(color, -25)

        if button['x_state'] == SEGMENT_STATE_X:
            color = AdjustColour(color, -20)

        gcdc.SetPen(wx.Pen(AdjustColour(color, -30)))

        brect = wx.Rect(x-3, y-3, 8, 8)
        bmp = DrawCircleCloseBmp(color, wx.WHITE)
        gcdc.DrawBitmap(bmp, brect.x, brect.y)
        button['xbtn'] = brect
        return
    
        # Square style button
#        gcdc.DrawRectangleRect(brect)
#        gcdc.SetPen(wx.BLACK_PEN)
#        gcdc.DrawLine(brect.x+1, brect.y+1,
#                      brect.x + brect.GetWidth() - 1, brect.y + brect.GetHeight() - 1)
#        gcdc.DrawLine(brect.x + brect.GetWidth() - 1, brect.y + 1,
#                      brect.x + 1, brect.y + brect.GetHeight() - 1)
#        gcdc.SetBrush(brush)
#        gcdc.SetPen(pen)
#        button['xbtn'] = brect

    def DoGetBestSize(self):
        """Get the best size for the control"""
        mwidth, mheight = 0, 0
        draw_label = self._style & CTRLBAR_STYLE_LABELS
        for btn in self._buttons:
            bwidth, bheight = btn['bsize']
            twidth = btn['lsize'][0]
            if bheight > mheight:
                mheight = bheight

            if bwidth > mwidth:
                mwidth = bwidth

            if draw_label:
                if twidth > mwidth:
                    mwidth = twidth

        # Adjust for label text
        if draw_label and len(self._buttons):
            mheight += self._buttons[0]['lsize'][1]

        width = (mwidth + (SegmentBar.HPAD * 2)) * len(self._buttons)
        size = wx.Size(width + (SegmentBar.HPAD * 2),
                       mheight + (SegmentBar.VPAD * 2))
        self.CacheBestSize(size)
        self._segsize = (mwidth + (SegmentBar.HPAD * 2),
                         mheight + (SegmentBar.VPAD * 2))
        return size

    def GetIndexFromPosition(self, pos):
        """Get the segment index closest to the given position"""
        cur_x = pos[0]
        for idx, button in enumerate(self._buttons):
            xpos = button['bx1']
            xpos2 = button['bx2']
            if cur_x >= xpos and cur_x <= xpos2 + 1:
                return idx
        else:
            return wx.NOT_FOUND

    def GetSegmentCount(self):
        """Get the number segments in the control
        @return: int

        """
        return len(self._buttons)

    def GetSegmentLabel(self, index):
        """Get the label of the given segment
        @param index: segment index
        @return: string

        """
        return self._buttons[index]['label']

    def GetSelection(self):
        """Get the currently selected index"""
        return self._selected

    def HitTest(self, pos):
        """Find where the position is in the window
        @param pos: (x, y) in client cords
        @return: int

        """
        index = self.GetIndexFromPosition(pos)
        where = SEGMENT_HT_NOWHERE
        if index != wx.NOT_FOUND:
            button = self._buttons[index]
            if self.SegmentHasCloseButton(index):
                brect = button['xbtn']
                trect = wx.Rect(brect.x, brect.y, brect.GetWidth()+4, brect.GetHeight()+4)
                if trect.Contains(pos):
                    where = SEGMENT_HT_X_BTN
                else:
                    where = SEGMENT_HT_SEG
            else:
                where = SEGMENT_HT_SEG

        return where, index

    def OnEraseBackground(self, evt):
        """Handle the erase background event"""
        pass

    def OnLeftDown(self, evt):
        """Handle clicks on the bar
        @param evt: wx.MouseEvent

        """
        epos = evt.GetPosition()
        index = self.GetIndexFromPosition(epos)
        if index != wx.NOT_FOUND:
            button = self._buttons[index]
            pre = self._selected
            self._selected = index

            if self._selected != pre:
                self.Refresh()
                sevt = SegmentBarEvent(edEVT_SEGMENT_SELECTED, button['id'])
                sevt.SetSelections(pre, index)
                sevt.SetEventObject(self)
                wx.PostEvent(self.GetParent(), sevt)

        self._x_clicked_before = False

        # Check for click on close btn
        if self.SegmentHasCloseButton(index):
            if self.HitTest(epos)[0] == SEGMENT_HT_X_BTN:
                self._x_clicked_before = True

        evt.Skip()

    def OnLeftUp(self, evt):
        """Handle clicks on the bar
        @param evt: wx.MouseEvent

        """
        epos = evt.GetPosition()
        where, index = self.HitTest(epos)

        # Check for click on close btn
        if self.SegmentHasCloseButton(index) and self._x_clicked_before:
            if where == SEGMENT_HT_X_BTN:
                event = SegmentBarEvent(edEVT_SEGMENT_CLOSE, self.GetId())
                event.SetSelections(index, index)
                event.SetEventObject(self)
                self.GetEventHandler().ProcessEvent(event)
                if not event.IsAllowed():
                    return False
                removed = self.RemoveSegment(index)

        evt.Skip()

    def OnMouseMove(self, evt):
        """Handle when the mouse moves over the bar"""
        epos = evt.GetPosition()
        where, index = self.HitTest(epos)
        if index == -1 or not self.SegmentHasCloseButton(index):
            return

        button = self._buttons[index]
        x_state = button['x_state']
        button['x_state'] = SEGMENT_STATE_NONE

        if where != SEGMENT_HT_NOWHERE:
            if where == SEGMENT_HT_X_BTN:
                button['x_state'] = SEGMENT_STATE_X
            elif where == SEGMENT_HT_SEG:
                # TODO: add highligh option for hover on segment
                pass
        else:
            evt.Skip()
            return

        # If the hover state over a segments close button
        # has changed redraw the close button to reflect the
        # proper state.
        
        bRedrawX = button['x_state'] != x_state
        if bRedrawX:
            dc = wx.ClientDC(self) # TODO: this has poor results on msw
            crect = self.GetClientRect()
            brect = wx.Rect(button['bx1'], 0,
                            button['bx2'] - (button['bx1'] - 2),
                            crect.GetHeight())
            self.DoDrawCloseBtn(dc, button, brect)

        evt.Skip()

    def OnPaint(self, evt):
        """Paint the control"""
        dc = wx.AutoBufferedPaintDCFactory(self)
        gc = wx.GCDC(dc)

        # Setup
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        gc.SetBrush(wx.TRANSPARENT_BRUSH)
        gc.SetFont(self.GetFont())
        gc.SetBackgroundMode(wx.TRANSPARENT)
        gc.Clear()

        # Paint the background
        rect = self.GetClientRect()
        self.DoPaintBackground(gc, rect, self._color, self._color2)

        # Draw the buttons
        # TODO: would be more efficient to just redraw the buttons that
        #       need redrawing.
        npos = 5
        use_labels = self._style & CTRLBAR_STYLE_LABELS
        for idx, button in enumerate(self._buttons):
            npos = self.DoDrawButton(gc, npos, idx,
                                      self._selected == idx,
                                      use_labels)

    def RemoveSegment(self, index):
        """Remove a segment from the bar
        @param index: int
        @return: bool

        """
        button = self._buttons[index]

        # TODO: wxPython 2.8.9.2 this causes a crash...
#        if button['bmp']:
#            button['bmp'].Destroy()
        del self._buttons[index]

        if self.GetSelection() == index:
            count = self.GetSegmentCount()
            if index >= count:
                self.SetSelection(count-1)

        self.Refresh()
        return True

    def SegmentHasCloseButton(self, index):
        """Does the segment at index have a close button
        @param index: int

        """
        button = self._buttons[index]
        if button['opts'] & SEGBTN_OPT_CLOSEBTNL or \
           button['opts'] & SEGBTN_OPT_CLOSEBTNR:
            return True
        return False

    def SetSegmentImage(self, index, bmp):
        """Set the image to use on the given segment
        @param index: int
        @param bmp: Bitmap

        """
        assert bmp.IsOk()
        segment = self._buttons[index]
        if segment['bmp'].IsOk():
            segment['bmp'].Destroy()
            del segment['bmp']
        segment['bmp'] = bmp
        segment['bsize'] = bmp.GetSize()
        self.InvalidateBestSize()
        self.Refresh()

    def SetSegmentLabel(self, index, label):
        """Set the label for a given segment
        @param index: segment index
        @param label: string

        """
        segment = self._buttons[index]
        lsize = self.GetTextExtent(label)
        segment['label'] = label
        segment['lsize'] = lsize
        self.InvalidateBestSize()
        self.Refresh()

    def SetSegmentOption(self, index, option):
        """Set an option on a given segment
        @param index: segment index
        @param option: option to set

        """
        self._buttons[index]['opts'] |= option
        self.Refresh()

    def SetSelection(self, index):
        """Set the selection
        @param index: int

        """
        self._selected = index
        self.Refresh()

# Cleanup namespace
#del SegmentBar.__dict__['AddControl']
#del SegmentBar.__dict__['AddSpacer']
#del SegmentBar.__dict__['AddTool']
#del SegmentBar.__dict__['SetToolSpacing']
#del SegmentBar.__dict__['SetVMargin']

#--------------------------------------------------------------------------#
