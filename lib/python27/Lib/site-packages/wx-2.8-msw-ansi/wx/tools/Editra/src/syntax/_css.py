###############################################################################
# Name: css.py                                                                #
# Purpose: Define CSS syntax for highlighting and other features              #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2007 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
FILE: css.py
@author: Cody Precord
@summary: Lexer configuration file for Cascading Style Sheets.

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: _css.py 63834 2010-04-03 06:04:33Z CJP $"
__revision__ = "$Revision: 63834 $"

#-----------------------------------------------------------------------------#
# Imports
import wx.stc as stc

# Local Imports
import synglob
import syndata

#-----------------------------------------------------------------------------#

#---- Keyword Specifications ----#

# CSS1 Keywords (Idenifiers)
CSS1_KEYWORDS = (0, "font-family font-style font-variant font-weight font-size "
                    "font color background-color background-image "
                    "background-repeat background-position background "
                    "word-spacing letter-spacing text-decoration "
                    "vertical-align text-transform text-align text-indent "
                    "line-height margin-top margin-right margin-left margin "
                    "padding-top padding-right padding-bottom padding-left "
                    "padding border-top-width border-right-width "
                    "border-bottom-width border-left-width border-width "
                    "border-color border-style border-top border-right "
                    "border-bottom border-left border width height float clear "
                    "display white-space list-style-type list-style-image "
                    "list-style-position list-style margin-bottom "
                    "text-decoration min-width min-height")

# CSS Psuedo Classes
CSS_PSUEDO_CLASS = (1, "link visited active hover focus before after left "
                       "right lang first-letter first-line first-child")

# CSS2 Keywords (Identifers2)
# This is meant for css2 specific keywords, but in order to get a better
# coloring effect this will contain special css properties as well.
CSS2_KEYWORDS = (2, "src stemv stemh slope ascent descent widths bbox baseline "
                    "centerline mathline topline all aqua black blue fuchsia "
                    "gray green lime maroon navy olive purple red silver teal "
                    "yellow ActiveBorder ActiveCaption AppWorkspace ButtonFace "
                    "ButtonHighlight ButtonShadow ButtonText CaptionText "
                    "GrayText Highlight HighlightText InactiveBorder "
                    "InactiveCaption InactiveCaptionText InfoBackground "
                    "InfoText Menu MenuText Scrollbar ThreeDDarkShadow "
                    "ThreeDFace ThreeDHighlight ThreeDLightShadow ThreeDShadow "
                    "Window WindowFrame WindowText Background auto none "
                    "inherit top bottom medium normal cursive fantasy "
                    "monospace italic oblique bold bolder lighter larger "
                    "smaller icon menu narrower wider color center scroll "
                    "fixed underline overline blink sub super middle "
                    "capitalize uppercase lowercase center justify baseline "
                    "width height float clear overflow clip visibility thin "
                    "thick both dotted dashed solid double groove ridge inset "
                    "outset hidden visible scroll collapse content quotes disc "
                    "circle square hebrew armenian georgian inside outside "
                    "size marks inside orphans widows landscape portrait crop "
                    "cross always avoid cursor default crosshair pointer move "
                    "wait help invert position below level above higher block "
                    "inline compact static relative absolute fixed ltr rtl "
                    "embed bidi-override pre nowrap volume during azimuth "
                    "elevation stress richness silent non mix leftwards "
                    "rightwards behind faster slower male female child code "
                    "digits continuous separate show hide once ")

#---- Syntax Style Specs ----#
SYNTAX_ITEMS = [ (stc.STC_CSS_DEFAULT, 'default_style'),
                 (stc.STC_CSS_ATTRIBUTE, 'funct_style'),
                 (stc.STC_CSS_CLASS, 'global_style'),
                 (stc.STC_CSS_COMMENT, 'comment_style'),
                 (stc.STC_CSS_DIRECTIVE, 'directive_style'),
                 (stc.STC_CSS_DOUBLESTRING, 'string_style'),
                 (stc.STC_CSS_ID, 'scalar_style'),
                 (stc.STC_CSS_IDENTIFIER, 'keyword_style'),
                 (stc.STC_CSS_IDENTIFIER2, 'keyword3_style'),
                 (stc.STC_CSS_IMPORTANT, 'error_style'),
                 (stc.STC_CSS_OPERATOR, 'operator_style'),
                 (stc.STC_CSS_PSEUDOCLASS, 'scalar_style'),
                 (stc.STC_CSS_SINGLESTRING, 'string_style'),
                 (stc.STC_CSS_TAG, 'keyword_style'),
                 (stc.STC_CSS_UNKNOWN_IDENTIFIER, 'unknown_style'),
                 (stc.STC_CSS_UNKNOWN_PSEUDOCLASS, 'unknown_style'),
                 (stc.STC_CSS_VALUE, 'char_style') ]

#---- Extra Properties ----#
FOLD = ("fold", "1")
#------------------------------------------------------------------------------#

class SyntaxData(syndata.SyntaxDataBase):
    """SyntaxData object for CSS""" 
    def __init__(self, langid):
        syndata.SyntaxDataBase.__init__(self, langid)

        # Setup
        self.SetLexer(stc.STC_LEX_CSS)
        self.RegisterFeature(synglob.FEATURE_AUTOINDENT, AutoIndenter)

    def GetKeywords(self):
        """Returns Specified Keywords List """
        return [CSS1_KEYWORDS , CSS_PSUEDO_CLASS, CSS2_KEYWORDS]

    def GetSyntaxSpec(self):
        """Syntax Specifications """
        return SYNTAX_ITEMS

    def GetProperties(self):
        """Returns a list of Extra Properties to set """
        return [FOLD]

    def GetCommentPattern(self):
        """Returns a list of characters used to comment a block of code """
        return [u'/*', u'*/']

#-----------------------------------------------------------------------------#

def AutoIndenter(stc, pos, ichar):
    """Auto indent cpp code. uses \n the text buffer will
    handle any eol character formatting.
    @param stc: EditraStyledTextCtrl
    @param pos: current carat position
    @param ichar: Indentation character
    @return: string

    """
    rtxt = u''
    line = stc.GetCurrentLine()
    text = stc.GetTextRange(stc.PositionFromLine(line), pos)

    indent = stc.GetLineIndentation(line)
    if ichar == u"\t":
        tabw = stc.GetTabWidth()
    else:
        tabw = stc.GetIndent()

    i_space = indent / tabw
    ndent = u"\n" + ichar * i_space
    rtxt = ndent + ((indent - (tabw * i_space)) * u' ')

    if text.endswith('{'):
        rtxt += ichar

    return rtxt
