###############################################################################
# Name: ed_shelf.py                                                           #
# Purpose: Editra Shelf container                                             #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2009 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
Shelf plugin and control implementation

@summary: Shelf Implementation

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: ed_shelf.py 64251 2010-05-08 23:42:10Z CJP $"
__revision__ = "$Revision: 64251 $"

#-----------------------------------------------------------------------------#
# Imports
import re
import wx

# Editra Libraries
import ed_menu
import ed_glob
from profiler import Profile_Get
import plugin
import iface
import extern.aui as aui

#--------------------------------------------------------------------------#
# Globals

PGNUM_PAT = re.compile(' - [0-9]+')
_ = wx.GetTranslation

#-----------------------------------------------------------------------------#

def mainwinonly(func):
    """Decorator method to add guards for methods that require the parent
    to be a MainWindow instance

    """
    def WrapMainWin(*args, **kwargs):
        self = args[0]
        if not hasattr(self._parent, 'GetFrameManager'):
            return
        else:
            func(*args, **kwargs)

    WrapMainWin.__name__ = func.__name__
    WrapMainWin.__doc__ = func.__doc__
    return WrapMainWin

#-----------------------------------------------------------------------------#

class Shelf(plugin.Plugin):
    """Plugin that creates a notebook for holding the various Shelf items
    implemented by L{ShelfI}.

    """
    SHELF_NAME = u'Shelf'
    observers = plugin.ExtensionPoint(iface.ShelfI)
    delegate = None

    def GetUiHandlers(self):
        """Gets the update ui handlers for the shelf's menu
        @return: [(ID, handler),]

        """
        handlers = [ (item.GetId(), Shelf.delegate.UpdateShelfMenuUI)
                     for item in self.observers ]
        return handlers

    def Init(self, parent):
        """Mixes the shelf into the parent window
        @param parent: Reference to MainWindow

        """
        # First check if the parent has an instance already
        parent = parent
        mgr = parent.GetFrameManager()
        if mgr.GetPane(Shelf.SHELF_NAME).IsOk():
            return

        shelf = EdShelfBook(parent)
        mgr.AddPane(shelf,
                    wx.aui.AuiPaneInfo().Name(Shelf.SHELF_NAME).\
                            Caption("Shelf").Bottom().Layer(0).\
                            CloseButton(True).MaximizeButton(False).\
                            BestSize(wx.Size(500,250)))

        # Hide the pane and let the perspective manager take care of it
        mgr.GetPane(Shelf.SHELF_NAME).Hide()
        mgr.Update()

        # Create the delegate
        # Parent MUST take ownership and clear the class variable before
        # another call to Init is made.
        delegate = EdShelfDelegate(shelf, self)
        assert Shelf.delegate is None, "Delegate not cleared!"
        Shelf.delegate = delegate

        # Install Shelf menu under View and bind event handlers
        view = parent.GetMenuBar().GetMenuByName("view")
        menu = delegate.GetMenu()
        pos = 0
        for pos in xrange(view.GetMenuItemCount()):
            mitem = view.FindItemByPosition(pos)
            if mitem.GetId() == ed_glob.ID_PERSPECTIVES:
                break

        view.InsertMenu(pos + 1, ed_glob.ID_SHELF, _("Shelf"), 
                        menu, _("Put an item on the Shelf"))

        for item in menu.GetMenuItems():
            if item.IsSeparator():
                continue
            parent.Bind(wx.EVT_MENU, delegate.OnGetShelfItem, item)

        if menu.GetMenuItemCount() < 3:
            view.Enable(ed_glob.ID_SHELF, False)

        # Check for any other plugin specific install needs
        for observer in self.observers:
            if not observer.IsInstalled() and \
               hasattr(observer, 'InstallComponents'):
                observer.InstallComponents(parent)

        delegate.StockShelf(Profile_Get('SHELF_ITEMS', 'list', []))

#--------------------------------------------------------------------------#

class EdShelfBook(aui.AuiNotebook):
    """Shelf notebook control"""
    def __init__(self, parent):
        style = aui.AUI_NB_BOTTOM | \
                aui.AUI_NB_TAB_SPLIT | \
                aui.AUI_NB_SCROLL_BUTTONS | \
                aui.AUI_NB_CLOSE_ON_ACTIVE_TAB | \
                aui.AUI_NB_TAB_MOVE | \
                aui.AUI_NB_DRAW_DND_TAB
        if wx.Platform == '__WXMAC__':
            style |= aui.AUI_NB_CLOSE_ON_TAB_LEFT
        aui.AuiNotebook.__init__(self, parent, agwStyle=style)

        # Attributes
        self._parent = parent
        self._open = dict()
        self._imgidx = dict()
        self._imglst = wx.ImageList(16, 16)

        # Setup
        self.SetImageList(self._imglst)
        self.SetSashDClickUnsplit(True)

    @property
    def ImgIdx(self):
        return self._imgidx

    @property
    def ImageList(self):
        return self._imglst

    def AddItem(self, item, name, imgid=-1):
        """Add an item to the shelf's notebook. This is useful for interacting
        with the Shelf from outside its interface. It may be necessary to
        call L{EnsureShelfVisible} before or after adding an item if you wish
        the shelf to be shown when the item is added.
        @param item: A panel like instance to add to the shelf's notebook
        @param name: Items name used for page text in notebook

        """
        self.AddPage(item, 
                     u"%s - %d" % (name, self._open.get(name, 0)),
                     select=True)

        # Set the tab icon
        if imgid >= 0 and Profile_Get('TABICONS', default=True):
            self.SetPageImage(self.GetPageCount()-1, imgid)
        self._open[name] = self._open.get(name, 0) + 1

    @mainwinonly
    def EnsureShelfVisible(self):
        """Make sure the Shelf is visible
        @precondition: Shelf.Init has been called
        @postcondition: Shelf is shown

        """
        mgr = self._parent.GetFrameManager()
        pane = mgr.GetPane(Shelf.SHELF_NAME)
        if not pane.IsShown():
            pane.Show()
            mgr.Update()

    def GetCount(self, item_name):
        """Get the number of open instances of a given Shelf Item
        @param item_name: Name of the Shelf item
        @return: number of instances on the Shelf

        """
        count = 0
        for page in xrange(self.GetPageCount()):
            if self.GetPageText(page).startswith(item_name):
                count = count + 1
        return count

    def GetMainWindow(self):
        """Get the main window that this shelf instance was created for
        @return: ed_main.MainWindow

        """
        return self._parent

    def GetOpen(self):
        """Get the list of open shelf items
        @return: list

        """
        return self._open

    @mainwinonly
    def Hide(self):
        """Hide the shelf
        @postcondition: Shelf is hidden by aui manager

        """
        mgr = self._parent.GetFrameManager()
        pane = mgr.GetPane(Shelf.SHELF_NAME)
        if pane.IsOk():
            pane.Hide()
            mgr.Update()

    def ItemIsOnShelf(self, item_name):
        """Check if at least one instance of a given item
        is currently on the Shelf.
        @param item_name: name of Item to look for

        """
        for page in xrange(self.GetPageCount()):
            if self.GetPageText(page).startswith(item_name):
                return True
        return False

    @mainwinonly
    def IsShown(self):
        """Is the shelf visible?
        @return: bool

        """
        mgr = self._parent.GetFrameManager()
        pane = mgr.GetPane(Shelf.SHELF_NAME)
        if pane.IsOk():
            return pane.IsShown()
        else:
            return False

#--------------------------------------------------------------------------#

class EdShelfDelegate(object):
    """Delegate class to mediate between the plugin singleton object and the
    UI implementation.

    """
    def __init__(self, shelf, pobject):
        """Create the delegate object
        @param shelf: Ui component instance
        @param pobject: Reference to the plugin object

        """
        object.__init__(self)

        # Attributes
        self._log = wx.GetApp().GetLog()
        self._shelf = shelf
        self._pin = pobject

    @property
    def observers(self):
        return self._pin.observers

    def AddItem(self, item, name, imgid=-1):
        """Add an item to the shelf"""
        self._shelf.AddItem(item, name, imgid)

    def CanStockItem(self, item_name):
        """See if a named item can be stocked or not, meaning if it
        can be saved and opened in the next session or not.
        @param item_name: name of item to check
        @return: bool whether item can be stocked or not

        """
        for item in self.observers:
            if item_name == item.GetName():
                if hasattr(item, 'IsStockable'):
                    return item.IsStockable()
                else:
                    break
        return False

    def EnsureShelfVisible(self):
        """Ensure the shelf is visible"""
        self._shelf.EnsureShelfVisible()

    def GetItemById(self, itemid):
        """Get the shelf item by its id
        @param itemid: Shelf item id
        @return: reference to a ShelfI object

        """
        for item in self.observers:
            if item.GetId() == itemid:
                return item
        return None

    def GetItemId(self, item_name):
        """Get the id that identifies a given item
        @param item_name: name of item to get ID for
        @return: integer id or None if not found

        """
        for item in self.observers:
            if item_name == item.GetName():
                return item.GetId()
        return None

    def GetItemStack(self):
        """Returns a list of ordered named items that are open in the shelf
        @return: list of strings

        """
        rval = list()
        if self._shelf is None:
            return rval

        for page in xrange(self._shelf.GetPageCount()):
            rval.append(re.sub(PGNUM_PAT, u'', 
                        self._shelf.GetPageText(page), 1))
        return rval

    def GetMenu(self):
        """Return the menu of this object
        @return: ed_menu.EdMenu()

        """
        menu = ed_menu.EdMenu()
        menu.Append(ed_glob.ID_SHOW_SHELF, _("Show Shelf") + \
                    ed_menu.EdMenuBar.keybinder.GetBinding(ed_glob.ID_SHOW_SHELF), 
                    _("Show the Shelf"))
        menu.AppendSeparator()
        menu_items = list()
        open_items = self._shelf.GetOpen()
        for observer in self.observers:
            # Register Observers
            open_items[observer.GetName()] = 0
            try:
                menu_i = observer.GetMenuEntry(menu)
                if menu_i is not None:
                    menu_items.append((menu_i.GetItemLabel(), menu_i))
            except Exception, msg:
                self._log("[shelf][err] %s" % str(msg))
        menu_items.sort()

        combo = 0
        for item in menu_items:
            combo += 1
            item[1].SetText(item[1].GetText() + "\tCtrl+Alt+" + str(combo))
            menu.AppendItem(item[1])
        return menu

    def GetOwnerWindow(self):
        """Return the L{ed_main.MainWindow} instance that owns/created
        this Shelf.
        @return: reference to ed_main.MainWindow or None

        """
        return self._shelf.GetMainWindow()

    def GetWindow(self):
        """Return reference to the Shelfs window component
        @return: FlatnoteBook

        """
        return self._shelf

    def OnGetShelfItem(self, evt):
        """Handles menu events that have been registered
        by the Shelf Items on the Shelf.
        @param evt: Event that called this handler

        """
        e_id = evt.GetId()
        if e_id == ed_glob.ID_SHOW_SHELF:
            parent = self.GetOwnerWindow()
            if self._shelf.IsShown():
                self._shelf.Hide()
                nb = parent.GetNotebook()
                nb.GetCurrentCtrl().SetFocus()
            else:
                self._shelf.EnsureShelfVisible()
                mgr = parent.GetFrameManager()
                pane = mgr.GetPane(Shelf.SHELF_NAME)
                if pane is not None:
                    page = pane.window.GetCurrentPage()
                    if hasattr(page, 'SetFocus'):
                        page.SetFocus()
        else:
            self.PutItemOnShelf(evt.GetId())

    def OnPutShelfItemAway(self, evt):
        """Handles when an item is closed
        @param evt: event that called this handler
        @todo: is this needed?

        """
        raise NotImplementedError

    def PutItemOnShelf(self, shelfid):
        """Put an item on the shelf by using its unique shelf id.
        This is only for use with loading items implementing the
        L{ShelfI} interface. See L{AddItem} if you wish to pass
        a panel to the shelf to add.
        @param shelfid: id of the ShelfItem to open

        """
        item = None
        for shelfi in self.observers:
            if shelfi.GetId() == shelfid:
                item = shelfi
                break

        if item is None:
            return

        name = item.GetName()
        if self._shelf.ItemIsOnShelf(name) and \
            not item.AllowMultiple() or self._shelf is None:
            return
        else:
            self.EnsureShelfVisible()
            item_id = item.GetId()
            index = -1
            if hasattr(item, 'GetBitmap'):
                if item_id in self._shelf.ImgIdx:
                    index = self._shelf.ImgIdx[item_id]
                else:
                    bmp = item.GetBitmap()
                    if bmp.IsOk():
                        index = self._shelf.ImageList.Add(bmp)
                        self._shelf.ImgIdx[item_id] = index

            self.AddItem(item.CreateItem(self._shelf), name, index)

    def RaiseItem(self, item_name):
        """Set the selection in the notebook to be the that of the first
        instance of item_name that is found in the shelf.
        @param item_name: ShelfI name
        @return: reference to the selected page or None if no instance is

        """
        for page in xrange(self._shelf.GetPageCount()):
            if self._shelf.GetPageText(page).startswith(item_name):
                self._shelf.SetSelection(page)
                return self._shelf.GetPage(page)
        else:
            return None

    def RaiseWindow(self, window):
        """Set the selection in the notebook to be the that of the given
        window. Mostly used internally by items implementing L{ShelfI}.
        @param window: Window object
        @return: reference to the selected page or None if no instance is

        """
        for page in xrange(self._shelf.GetPageCount()):
            ctrl = self._shelf.GetPage(page)
            if window == ctrl:
                self._shelf.SetSelection(page)
                return ctrl
        else:
            return None

    def StockShelf(self, i_list):
        """Fill the shelf by opening an ordered list of items
        @param i_list: List of named L{ShelfI} instances
        @type i_list: list of strings

        """
        for item in i_list:
            if self.CanStockItem(item):
                itemid = self.GetItemId(item)
                if itemid:
                    self.PutItemOnShelf(itemid)

    def UpdateShelfMenuUI(self, evt):
        """Enable/Disable shelf items based on whether they support
        muliple instances or not.
        @param evt: wxEVT_UPDATEUI

        """
        item = self.GetItemById(evt.GetId())
        if item is None:
            evt.Skip()
            return

        count = self._shelf.GetCount(item.GetName())
        if count and not item.AllowMultiple():
            evt.Enable(False)
        else:
            evt.Enable(True)
