###############################################################################
# Name: style_editor.py                                                       #
# Purpose: Syntax Highlighting configuration dialog                           #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2008 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
Provides an editor dialog for graphically editing how the text is presented in 
the editor when syntax highlighting is turned on. It does this by taking the 
data from the controls and formating it into an Editra Style Sheet that the 
editor can load to configure the styles of the text.

@summary: Gui for creating custom Editra Style Sheets

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: style_editor.py 63520 2010-02-19 03:27:26Z CJP $"
__revision__ = "$Revision: 63520 $"

#--------------------------------------------------------------------------#
# Imports
import os
import glob
import wx

# Editra Imports
import ed_glob
from profiler import Profile_Get, Profile_Set
import ed_basestc
from ed_style import StyleItem
import util
import syntax.syntax as syntax
import eclib

# Function Aliases
_ = wx.GetTranslation

# Global Values
ID_STYLES = wx.NewId()
ID_FORE_COLOR = wx.NewId()
ID_BACK_COLOR = wx.NewId()
ID_BOLD = wx.NewId()
ID_ITALIC = wx.NewId()
ID_EOL = wx.NewId()
ID_ULINE = wx.NewId()
ID_FONT = wx.NewId()
ID_FONT_SIZE = wx.NewId()

SETTINGS_IDS = [ ID_FORE_COLOR, ID_BACK_COLOR, ID_BOLD, ID_ITALIC,
                 ID_EOL, ID_ULINE, ID_FONT, ID_FONT_SIZE ]
#--------------------------------------------------------------------------#

class StyleEditor(wx.Dialog):
    """This class creates the window that contains the controls
    for editing/configuring the syntax highlighting styles it acts
    as a graphical way to interact with the L{ed_style.StyleMgr}.

    @see: ed_style.StyleMgr
    """
    def __init__(self, parent, id_=wx.ID_ANY, title=_("Style Editor"),
                 style=wx.DEFAULT_DIALOG_STYLE | wx.RAISED_BORDER):
        """Initializes the Dialog
        @todo: rework the layout

        """
        wx.Dialog.__init__(self, parent, id_, title, style=style)

        # Attributes
        self.LOG = wx.GetApp().GetLog()
        self.preview = ed_basestc.EditraBaseStc(self, wx.ID_ANY, size=(-1, 200),
                                                style=wx.SUNKEN_BORDER)
        self.styles_orig = self.preview.GetStyleSet()
        self.preview.SetCaretLineVisible(True)
        self.styles_new = DuplicateStyleDict(self.styles_orig)
        self.preview.SetStyles('preview', self.styles_new, True)
        self.OpenPreviewFile('cpp')
        # XXX On Windows the settings pane must be made before the
        #     sizer it is to be put in or it becomes unable to recieve
        #     focus. But is the exact opposite on mac/gtk. This is really
        #     a pain or possibly a bug?
        if wx.Platform == '__WXMSW__':
            self._settings = SettingsPanel(self)

        # Main Sizer
        sizer = wx.BoxSizer(wx.VERTICAL)

        # Control Panel
        self.ctrl_pane = wx.Panel(self, wx.ID_ANY)
        ctrl_sizer = wx.BoxSizer(wx.HORIZONTAL)  # Main Control Sizer
        left_colum = wx.BoxSizer(wx.VERTICAL)    # Left Column
        right_colum = wx.BoxSizer(wx.VERTICAL)   # Right Column

        # XXX On Mac/GTK if panel is created before sizer all controls in
        #     it become unable to recieve focus from clicks, but it is the
        #     exact opposite on windows!
        if wx.Platform != '__WXMSW__':
            self._settings = SettingsPanel(self)

        # Control Panel Left Column
        left_colum.AddMany([((10, 10), 0),
                            (self.__StyleSheets(), 0, wx.ALIGN_LEFT),
                            ((10, 10), 0),
                            (self.__LexerChoice(), 0, wx.ALIGN_LEFT),
                            ((10, 10), 0),
                            (self.__StyleTags(), 1, wx.ALIGN_LEFT|wx.EXPAND),
                            ((10, 10), 0)])
        ctrl_sizer.Add(left_colum, 0, wx.ALIGN_LEFT)

        # Divider
        ctrl_sizer.Add(wx.StaticLine(self.ctrl_pane, size=(-1, 2),
                                     style=wx.LI_VERTICAL),
                       0, wx.ALIGN_CENTER_HORIZONTAL | wx.EXPAND)
        ctrl_sizer.Add((5, 5), 0)

        # Control Panel Right Column
        right_colum.Add(self._settings, 1, wx.ALIGN_LEFT | wx.EXPAND)
        ctrl_sizer.AddMany([(right_colum, 1, wx.ALIGN_RIGHT | wx.EXPAND),
                            ((5, 5), 0)])

        # Finish Control Panel Setup
        self.ctrl_pane.SetSizer(ctrl_sizer)
        sizer.AddMany([((10, 10)), (self.ctrl_pane, 0, wx.ALIGN_CENTER)])

        # Preview Area
        pre_sizer = wx.BoxSizer(wx.HORIZONTAL)
        pre_sizer.AddMany([((10, 10), 0),
                           (wx.StaticText(self, label=_("Preview") + u": "),
                            0, wx.ALIGN_LEFT)])
        sizer.AddMany([((10, 10), 0), (pre_sizer, 0, wx.ALIGN_LEFT),
                       (self.preview, 0, wx.EXPAND | wx.BOTTOM)])

        # Create Buttons
        b_sizer = wx.BoxSizer(wx.HORIZONTAL)
        ok_b = wx.Button(self, wx.ID_OK, _("Ok"))
        ok_b.SetDefault()
        b_sizer.AddMany([(wx.Button(self, wx.ID_CANCEL, _("Cancel")), 0),
                         ((5, 5), 0),
                         (wx.Button(self, wx.ID_SAVE, _("Export")), 0),
                         ((5, 5), 0), (ok_b, 0)])
        sizer.Add(b_sizer, 0, wx.ALIGN_RIGHT |
                  wx.ALIGN_CENTER_VERTICAL | wx.ALL, 5)

        # Finish the Layout
        self.SetSizer(sizer)
        self.SetAutoLayout(True)
        sizer.Fit(self)
        self.EnableSettings(False)

        # Event Handlers
        self.Bind(wx.EVT_BUTTON, self.OnCancel, id=wx.ID_CANCEL)
        self.Bind(wx.EVT_BUTTON, self.OnOk, id=wx.ID_OK)
        self.Bind(wx.EVT_BUTTON, self.OnExport, id=wx.ID_SAVE)
        self.Bind(wx.EVT_CHOICE, self.OnChoice)
        self.Bind(wx.EVT_CHECKBOX, self.OnCheck)
        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.Bind(wx.EVT_LISTBOX, self.OnListBox)
        self.Bind(eclib.EVT_COLORSETTER, self.OnColor)
        self.preview.Bind(wx.EVT_LEFT_UP, self.OnTextRegion)
        self.preview.Bind(wx.EVT_KEY_UP, self.OnTextRegion)
    #--- End Init ---#

    def __LexerChoice(self):
        """Returns a sizer object containing a choice control with all
        available lexers listed in it.
        @return: sizer item containing a choice control with all available
                 syntax test files available

        """
        lex_sizer = wx.BoxSizer(wx.HORIZONTAL)
        lexer_lbl = wx.StaticText(self.ctrl_pane, wx.ID_ANY,
                                  _("Syntax Files") + u": ")
        lexer_lst = wx.Choice(self.ctrl_pane, ed_glob.ID_LEXER,
                              choices=syntax.GetLexerList())
        lexer_lst.SetToolTip(wx.ToolTip(_("Set the preview file type")))
        lexer_lst.SetStringSelection(u"CPP")
        lex_sizer.AddMany([((10, 10)), (lexer_lbl, 0, wx.ALIGN_CENTER_VERTICAL),
                           ((5, 0)), (lexer_lst, 1, wx.ALIGN_CENTER_VERTICAL),
                           ((10, 10))])
        return lex_sizer

    def __StyleSheets(self):
        """Returns a sizer item that contains a choice control with
        all the available style sheets listed in it.
        @return: sizer item holding all installed style sheets

        """
        ss_sizer = wx.BoxSizer(wx.HORIZONTAL)
        ss_lbl = wx.StaticText(self.ctrl_pane, wx.ID_ANY,
                               _("Style Theme") + u": ")
        ss_lst = util.GetResourceFiles(u'styles', get_all=True)
        ss_choice = wx.Choice(self.ctrl_pane, ed_glob.ID_PREF_SYNTHEME,
                              choices=sorted(ss_lst))
        ss_choice.SetToolTip(wx.ToolTip(_("Base new theme on existing one")))
        ss_choice.SetStringSelection(Profile_Get('SYNTHEME', 'str'))
        ss_new = wx.CheckBox(self.ctrl_pane, wx.ID_NEW, _("New"))
        ss_new.SetToolTip(wx.ToolTip(_("Start a blank new style")))
        ss_sizer.AddMany([((10, 10)), (ss_lbl, 0, wx.ALIGN_CENTER_VERTICAL),
                          ((5, 0)),
                          (ss_choice, 0, wx.ALIGN_CENTER_VERTICAL), ((10, 0)),
                          (ss_new, 0, wx.ALIGN_CENTER_VERTICAL), ((10, 10))])
        return ss_sizer

    def __StyleTags(self):
        """Returns a sizer object containing a choice control with all
        current style tags in it.
        @return: sizer item containing list of all available style tags

        """
        style_sizer = wx.BoxSizer(wx.HORIZONTAL)
        style_sizer2 = wx.BoxSizer(wx.VERTICAL)

        style_lbl = wx.StaticText(self.ctrl_pane, wx.ID_ANY,
                                  _("Style Tags") + u": ")

        style_tags = self.styles_orig.keys()
        lsize = (-1, 100)
        if wx.Platform == '__WXMAC__':
            lsize = (-1, 120)

        style_lst = wx.ListBox(self.ctrl_pane, ID_STYLES, size=lsize,
                               choices=sorted(style_tags), style=wx.LB_SINGLE)
        style_sizer2.AddMany([(style_lbl, 0, wx.ALIGN_CENTER_VERTICAL),
                             (style_lst, 1, wx.EXPAND)])
        style_sizer.AddMany([((10, 10), 0),
                             (style_sizer2, 1,
                              wx.ALIGN_CENTER_HORIZONTAL | wx.EXPAND),
                             ((10, 10), 0)])
        return style_sizer

    def DiffStyles(self):
        """Checks if the current style set is different from the
        original set. Used internally to check if a save prompt needs
        to be brought up. Returns True if the style sets are different.
        @return: whether style set has been modified or not
        @rtype: bool

        """
        diff = False
        for key in self.styles_orig:
            if unicode(self.styles_orig[key]) != unicode(self.styles_new[key]):
                diff = True
                break

        result = wx.ID_NO
        if diff:
            dlg = wx.MessageDialog(self,
                                    _("Some styles have been changed would "
                                      "you like to save before exiting?"),
                                   _("Save Styles"),
                                   style=wx.YES_NO | wx.YES_DEFAULT | \
                                         wx.CANCEL | wx.ICON_INFORMATION)
            dlg.CenterOnParent()
            result = dlg.ShowModal()
            dlg.Destroy()
        return result

    def EnableSettings(self, enable=True):
        """Enables/Disables all settings controls
        @keyword enable: whether to enable/disable settings controls

        """
        for child in self._settings.GetChildren():
            child.Enable(enable)

    def ExportStyleSheet(self):
        """Writes the style sheet data out to a style sheet
        @return: whether style sheet was exported properly or not

        """
        if ed_glob.CONFIG['STYLES_DIR'] == ed_glob.CONFIG['SYS_STYLES_DIR']:
            stdpath = wx.StandardPaths_Get()
            user_config = os.path.join(stdpath.GetUserDataDir(), 'styles')
            if not os.path.exists(user_config):
                try:
                    os.mkdir(user_config)
                except (OSError, IOError), msg:
                    self.LOG("[style_editor][err] %s" % msg)
                else:
                    ed_glob.CONFIG['STYLES_DIR'] = user_config

        result = wx.ID_CANCEL
        ss_c = self.FindWindowById(ed_glob.ID_PREF_SYNTHEME)
        new_cb = self.FindWindowById(wx.ID_NEW)
        if new_cb.GetValue():
            name = ''
        else:
            name = ss_c.GetStringSelection()

        fname = wx.GetTextFromUser(_("Enter style sheet name"),
                                   _("Export Style Sheet"),
                                   name, self)

        if len(fname):
            sheet_path = os.path.join(ed_glob.CONFIG['STYLES_DIR'], fname)
            if sheet_path.split(u'.')[-1] != u'ess':
                sheet_path += u".ess"

            try:
                writer = util.GetFileWriter(sheet_path)
                writer.write(self.GenerateStyleSheet())
                writer.close()
            except (AttributeError, IOError), msg:
                self.LOG('[style_editor][err] Failed to export style sheet')
                self.LOG('[style_editor][err] %s' % str(msg))
            else:
                # Update Style Sheet Control
                sheet = ".".join(os.path.basename(sheet_path).split(u'.')[:-1])
                ss_c.SetItems(util.GetResourceFiles(u'styles', get_all=True))
                ss_c.SetStringSelection(sheet)
                ss_c.Enable()
                self.FindWindowById(wx.ID_NEW).SetValue(False)
                self.styles_orig = self.styles_new
                self.styles_new = DuplicateStyleDict(self.styles_orig)
                result = wx.ID_OK

                if sheet_path.startswith(ed_glob.CONFIG['STYLES_DIR']) or \
                   sheet_path.startswith(ed_glob.CONFIG['SYS_STYLES_DIR']):
                    # Update editor windows/buffer to use new style sheet
                    UpdateBufferStyles(sheet)

        return result

    def GenerateStyleSheet(self):
        """Generates a style sheet from the dialogs style data
        @return: The dictionary of L{StyleItem} in self.styles_new transformed
                 into a string that is in Editra Style Sheet format.

        """
        sty_sheet = list()
        ditem = self.styles_new.get('default_style', StyleItem())
        dvals = ';\n\t\t'.join([item.replace(',', ' ')
                                for item in ditem.GetAsList() ]) + ';'
        sty_sheet.append(''.join(['default_style {\n\t\t', dvals, '\n\n}\n\n']))

        tags = sorted(self.styles_new.keys())
        for tag in tags:
            item = self.styles_new[tag]
            if item.IsNull() or tag == 'default_style':
                continue

            stage1 = wx.EmptyString
            for attr in ('fore', 'back', 'face', 'size'):
                ival = item.GetNamedAttr(attr)
                if attr in ('fore', 'back'):
                    ival = ival.upper()

                if ival is None or ival == ditem.GetNamedAttr(attr):
                    continue

                stage1 = ''.join((stage1, attr, u':',
                                  ival.replace(',', ' '), u';'))

            # Add any modifiers to the modifier tag
            modifiers = item.GetModifiers()
            if len(modifiers):
                stage1 += (u"modifiers:" + modifiers + u";").replace(',', ' ')

            # If the StyleItem had any set attributes add it to the stylesheet
            if len(stage1):
                sty_sheet.append(tag + u" {\n")
                stage2 = u"\t\t" + stage1[0:-1].replace(u";", u";\n\t\t") + u";"
                sty_sheet.append(stage2)
                sty_sheet.append(u"\n}\n\n")

        return u"".join(sty_sheet)

    def OnCancel(self, evt):
        """Catches the cancel button clicks and checks if anything
        needs to be done before closing the window.
        @param evt: event that called this handler

        """
        self.LOG('[style_editor][evt] Cancel Clicked Closing Window')
        evt.Skip()

    def OnCheck(self, evt):
        """Handles Checkbox events
        @param evt: event that called this handler

        """
        e_id = evt.GetId()
        e_obj = evt.GetEventObject()
        if e_id == wx.ID_NEW:
            val = e_obj.GetValue()
            choice = self.ctrl_pane.FindWindowById(ed_glob.ID_PREF_SYNTHEME)
            choice.Enable(not val)
            if val:
                self.styles_orig = self.preview.BlankStyleDictionary()
                self.styles_new = DuplicateStyleDict(self.styles_orig)
                self.preview.SetStyles('preview', self.styles_new, nomerge=True)
                self.preview.UpdateAllStyles('preview')

                # For some reason this causes the text display to refresh
                # properly when nothing else would work.
                self.OnTextRegion()
            else:
                scheme = choice.GetStringSelection().lower()
                self.preview.UpdateAllStyles(scheme)
                self.styles_orig = self.preview.GetStyleSet()
                self.styles_new = DuplicateStyleDict(self.styles_orig)
        elif e_id in [ID_BOLD, ID_EOL, ID_ULINE, ID_ITALIC]:
            self.UpdateStyleSet(e_id)
        else:
            evt.Skip()

    def OnChoice(self, evt):
        """Handles the events generated from the choice controls
        @param evt: event that called this handler

        """
        e_id = evt.GetId()
        e_obj = evt.GetEventObject()
        val = e_obj.GetStringSelection()
        if e_id == ed_glob.ID_LEXER:
            self.OpenPreviewFile(val)
        elif e_id == ed_glob.ID_PREF_SYNTHEME:
            # TODO Need to check for style changes before switching this
            self.preview.UpdateAllStyles(val)
            self.styles_new = self.preview.GetStyleSet()
            self.styles_orig = DuplicateStyleDict(self.styles_new)
            ctrl = self.FindWindowById(ID_STYLES)
            tag = ctrl.GetStringSelection()
            if tag != wx.EmptyString:
                self.UpdateSettingsPane(self.styles_new[tag])
        elif e_id in [ID_FONT, ID_FONT_SIZE]:
            self.UpdateStyleSet(e_id)
        else:
            evt.Skip()

    def OnClose(self, evt):
        """Handles the window closer event
        @param evt: event that called this handler

        """
        self.LOG("[style_editor][evt] Dialog closing...")
        self.OnOk(evt)

    def OnColor(self, evt):
        """Handles color selection events
        @param evt: event that called this handler

        """
        # Update The Style data for current tag
        self.UpdateStyleSet(evt.GetId())

    def OnTextRegion(self, evt=None):
        """Processes clicks in the preview control and sets the style
        selection in the style tags list to the style tag of the area
        the cursor has moved into.
        @param evt: event that called this handler

        """
        if evt is not None:
            evt.Skip()

        style_id = self.preview.GetStyleAt(self.preview.GetCurrentPos())
        tag_lst = self.FindWindowById(ID_STYLES)
        data = self.preview.FindTagById(style_id)
        if data != wx.EmptyString and data in self.styles_new:
            tag_lst.SetStringSelection(data)
            if wx.Platform == '__WXGTK__':
                tag_lst.SetFirstItemStr(data)
            self.UpdateSettingsPane(self.styles_new[data])
            self.EnableSettings()

    def OnListBox(self, evt):
        """Catches the selection of a style tag in the listbox
        and updates the style window appropriately.
        @param evt: event that called this handler

        """
        tag = evt.GetEventObject().GetStringSelection()
        if tag != wx.EmptyString and tag in self.styles_new:
            self.UpdateSettingsPane(self.styles_new[tag])
            self.EnableSettings()
        else:
            self.EnableSettings(False)

    def OnOk(self, evt):
        """Catches the OK button click and checks if any changes need to be
        saved before the window closes.
        @param evt: event that called this handler

        """
        self.LOG('[style_editor][evt] Ok Clicked Closing Window')


        result = self.DiffStyles()
        if result == wx.ID_NO:
            # Get Current Selection to update buffers
            csheet = self.FindWindowById(ed_glob.ID_PREF_SYNTHEME).GetStringSelection()
            UpdateBufferStyles(csheet)
            evt.Skip()
        elif result == wx.ID_CANCEL:
            self.LOG('[style_editor][info] canceled closing')
        else:
            result = self.ExportStyleSheet()
            if result != wx.ID_CANCEL:
                evt.Skip()

    def OnExport(self, evt):
        """Catches save button event
        @param evt: event that called this handler

        """
        self.LOG('[style_editor][evt] Export Clicked')
        self.ExportStyleSheet()

    def OpenPreviewFile(self, file_lbl):
        """Opens a file using the names in the Syntax Files choice
        control as a search query.
        @param file_lbl: name of file to open in test data directory

        """
        fname = file_lbl.replace(u" ", u"_").replace(u"/", u"_").lower()
        fname = fname.replace('#', 'sharp')
        try:
            fname = glob.glob(ed_glob.CONFIG['TEST_DIR'] + fname + ".*")[0]
        except IndexError:
            self.LOG('[style_editor][err] File %s Does not exist' % fname)
            return False

        self.preview.SetFileName(fname)
        self.preview.ClearAll()
        self.preview.LoadFile(fname)
        self.preview.FindLexer()
        self.preview.EmptyUndoBuffer()
        return True

    def UpdateSettingsPane(self, syntax_data):
        """Updates all the settings controls to hold the
        values of the selected tag.
        @param syntax_data: syntax data set to configure panel from

        """
        val_str = unicode(syntax_data)
        val_map = { ID_FORE_COLOR : syntax_data.GetFore(),
                    ID_BACK_COLOR : syntax_data.GetBack(),
                    ID_BOLD       : "bold" in val_str,
                    ID_ITALIC     : "italic" in val_str,
                    ID_EOL        : "eol" in val_str,
                    ID_ULINE      : "underline" in val_str,
                    ID_FONT       : syntax_data.GetFace(),
                    ID_FONT_SIZE  : syntax_data.GetSize()
                  }

        # Fall back to defaults for color values
        # that we may not be able to understand
        if u"#" not in val_map[ID_FORE_COLOR]:
            val_map[ID_FORE_COLOR] = self.preview.GetDefaultForeColour(as_hex=True)
        if u"#" not in val_map[ID_BACK_COLOR]:
            val_map[ID_BACK_COLOR] = self.preview.GetDefaultBackColour(as_hex=True)

        for sid in SETTINGS_IDS:
            ctrl = self.FindWindowById(sid)
            c_type = ctrl.GetClassName()
            if c_type == 'wxCheckBox':
                ctrl.SetValue(val_map[sid])
            elif c_type == "wxChoice":
                ctrl.SetStringSelection(val_map[sid])
            elif isinstance(ctrl, eclib.ColorSetter):
                ctrl.SetLabel(val_map[sid][:7])
        return True

    def UpdateStyleSet(self, id_):
        """Updates the value of the style tag to reflect any changes
        made in the settings controls.
        @param id_: identifier of the style tag in the list

        """
        # Get the tag that has been modified
        tag = self.FindWindowById(ID_STYLES)
        tag = tag.GetStringSelection()
        if tag == None or tag == wx.EmptyString:
            return False

        # Get the modified value
        ctrl = self.FindWindowById(id_)
        ctrl_t = ctrl.GetClassName()
        if ctrl_t == 'wxCheckBox':
            val = ctrl.GetValue()
        elif ctrl_t == 'wxChoice':
            val = ctrl.GetStringSelection()
        elif isinstance(ctrl, eclib.ColorSetter):
            val = ctrl.GetLabel()
        else:
            return False

        # Update the value of the modified tag
        val_map = { ID_FONT       : u'face',
                    ID_FONT_SIZE  : u'size',
                    ID_BOLD       : u"bold",
                    ID_EOL        : u"eol",
                    ID_ITALIC     : u"italic",
                    ID_ULINE      : u"underline",
                    ID_FORE_COLOR : u"fore",
                    ID_BACK_COLOR : u"back"
                  }

        if id_ in [ ID_FONT, ID_FONT_SIZE, ID_FORE_COLOR, ID_BACK_COLOR ]:
            self.styles_new[tag].SetNamedAttr(val_map[id_], val)
        elif id_ in [ ID_BOLD, ID_ITALIC, ID_ULINE, ID_EOL ]:
            self.styles_new[tag].SetExAttr(val_map[id_], val)
        else:
            return False

        # Update the Preview Area
        self.preview.SetStyleTag(tag, self.styles_new[tag])
        self.preview.RefreshStyles()

#-----------------------------------------------------------------------------#

class SettingsPanel(wx.Panel):
    """Panel holding all settings controls for changing the font,
    colors, styles, ect.. in the style set.

    """
    def __init__(self, parent):
        """Create the settings panel"""
        wx.Panel.__init__(self, parent)

        # Attributes

        # Layout
        self.__DoLayout()

    def __DoLayout(self):
        """Layout the controls in the panel"""
        setting_sizer = wx.BoxSizer(wx.VERTICAL)
        setting_top = wx.BoxSizer(wx.HORIZONTAL)

        # Settings top
        setting_sizer.Add((10, 10))
        cbox_sizer = wx.StaticBoxSizer(wx.StaticBox(self,
                                          label=_("Color") + u":"), wx.VERTICAL)

        # Foreground
        fground_sizer = wx.BoxSizer(wx.HORIZONTAL)
        fground_lbl = wx.StaticText(self, label=_("Foreground") + u": ")
        fground_sel = eclib.ColorSetter(self, ID_FORE_COLOR, wx.BLACK)
        fground_sizer.AddMany([((5, 5)),
                               (fground_lbl, 0, wx.ALIGN_CENTER_VERTICAL),
                               ((2, 2), 1, wx.EXPAND),
                               (fground_sel, 0, wx.ALIGN_CENTER_VERTICAL),
                               ((5, 5))])
        cbox_sizer.AddMany([(fground_sizer, 0, wx.ALIGN_LEFT | wx.EXPAND),
                            ((10, 10))])

        # Background
        bground_sizer = wx.BoxSizer(wx.HORIZONTAL)
        bground_lbl = wx.StaticText(self, label=_("Background") + u": ")
        bground_sel = eclib.ColorSetter(self, ID_BACK_COLOR, wx.WHITE)
        bground_sizer.AddMany([((5, 5)),
                               (bground_lbl, 0, wx.ALIGN_CENTER_VERTICAL),
                               ((2, 2), 1, wx.EXPAND),
                               (bground_sel, 0, wx.ALIGN_CENTER_VERTICAL),
                               ((5, 5))])
        cbox_sizer.Add(bground_sizer, 0, wx.ALIGN_LEFT | wx.EXPAND)
        setting_top.AddMany([(cbox_sizer, 0, wx.ALIGN_TOP), ((10, 10))])

        # Attrib Box
        attrib_box = wx.StaticBox(self, label=_("Attributes") + u":")
        abox_sizer = wx.StaticBoxSizer(attrib_box, wx.VERTICAL)

        # Attributes
        bold_cb = wx.CheckBox(self, ID_BOLD, _("bold"))
        eol_cb = wx.CheckBox(self, ID_EOL, _("eol"))
        ital_cb = wx.CheckBox(self, ID_ITALIC, _("italic"))
        uline_cb = wx.CheckBox(self, ID_ULINE, _("underline"))
        abox_sizer.AddMany([(bold_cb, 0, wx.ALIGN_CENTER_VERTICAL),
                            (eol_cb, 0, wx.ALIGN_CENTER_VERTICAL),
                            (ital_cb, 0, wx.ALIGN_CENTER_VERTICAL),
                            (uline_cb, 0, wx.ALIGN_CENTER_VERTICAL)])
        setting_top.Add(abox_sizer, 0, wx.ALIGN_TOP)

        # Font
        fh_sizer = wx.BoxSizer(wx.HORIZONTAL)
        font_box = wx.StaticBox(self, label=_("Font Settings") + u":")
        fbox_sizer = wx.StaticBoxSizer(font_box, wx.VERTICAL)

        # Font Face Name
        fsizer = wx.BoxSizer(wx.HORIZONTAL)
        flbl = wx.StaticText(self, label=_("Font") + u": ")
        fontenum = wx.FontEnumerator()
        if wx.Platform == '__WXMAC__':
            # FixedWidthOnly Asserts on wxMac
            fontenum.EnumerateFacenames(fixedWidthOnly=False)
        else:
            fontenum.EnumerateFacenames(fixedWidthOnly=True)
        font_lst = ["%(primary)s", "%(secondary)s"]
        font_lst.extend(sorted(fontenum.GetFacenames()))
        fchoice = wx.Choice(self, ID_FONT, choices=font_lst)
        fsizer.AddMany([((5, 5), 0), (flbl, 0, wx.ALIGN_CENTER_VERTICAL),
                        (fchoice, 0, wx.ALIGN_CENTER_VERTICAL), ((5, 5))])
        fbox_sizer.Add(fsizer, 0, wx.ALIGN_LEFT)

        # Font Size
        fsize_sizer = wx.BoxSizer(wx.HORIZONTAL)
        fsize_lbl = wx.StaticText(self, label=_("Size") + u": ")
        fsizes = ['%(size)d', '%(size2)d']
        fsizes.extend([ str(x) for x in xrange(4, 21) ])
        fs_choice = wx.Choice(self, ID_FONT_SIZE, choices=fsizes)
        fsize_sizer.AddMany([((5, 5), 0),
                             (fsize_lbl, 0, wx.ALIGN_CENTER_VERTICAL),
                             (fs_choice, 1, wx.EXPAND | wx.ALIGN_RIGHT),
                             ((5, 5), 0)])
        fbox_sizer.AddMany([((5, 5)),
                            (fsize_sizer, 0, wx.ALIGN_LEFT | wx.EXPAND)])
        fh_sizer.AddMany([(fbox_sizer, 0, wx.ALIGN_CENTER_HORIZONTAL),
                          ((10, 10))])

        # Build Section
        setting_sizer.AddMany([(setting_top, 0, wx.ALIGN_CENTER_HORIZONTAL),
                               ((10, 10), 1, wx.EXPAND),
                               (fh_sizer, 0, wx.ALIGN_CENTER_HORIZONTAL)])
        self.SetSizer(setting_sizer)
        self.SetAutoLayout(True)

#-----------------------------------------------------------------------------#
# Utility funtcions
def DuplicateStyleDict(style_dict):
    """Duplicates the style dictionary to make a true copy of
    it, as simply assigning the dictionary to two different variables
    only copies a reference leaving both variables pointing to the
    same object.
    @param style_dict: dictionary of tags->StyleItems
    @return: a copy of the given styleitem dictionary

    """
    new_dict = dict()
    for tag in style_dict:
        new_dict[tag] = StyleItem()
        is_ok = new_dict[tag].SetAttrFromStr(unicode(style_dict[tag]))
        if not is_ok:
            new_dict[tag].null = True
    return new_dict

def UpdateBufferStyles(sheet):
    """Update the style used in all buffers
    @param sheet: Style sheet to use

    """
    # Only update if the sheet has changed
    if sheet is None or sheet == Profile_Get('SYNTHEME'):
        return
    Profile_Set('SYNTHEME', sheet)
