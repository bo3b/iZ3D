###############################################################################
# Name: python.py                                                             #
# Purpose: Define Python syntax for highlighting and other features           #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2007 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
FILE: python.py
AUTHOR: Cody Precord
@summary: Lexer configuration module for Python.

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: _python.py 64177 2010-04-30 01:44:27Z CJP $"
__revision__ = "$Revision: 64177 $"

#-----------------------------------------------------------------------------#
# Imports
import wx.stc as stc
import keyword

# Local Imports
import synglob
import syndata

#-----------------------------------------------------------------------------#

#---- Keyword Specifications ----#

# Indenter keywords
INDENT_KW = (u"def", u"if", u"elif", u"else", u"for", u"while",
             u"class", u"try", u"except", u"finally", u"with")
UNINDENT_KW = (u"return", u"raise", u"break", u"continue",
               u"pass", u"exit", u"quit")

# Python Keywords
KEYWORDS = keyword.kwlist
KEYWORDS.extend(['True', 'False', 'None', 'self'])
PY_KW = (0, u" ".join(KEYWORDS))

# Highlighted builtins
try:
    import __builtin__
    BUILTINS = dir(__builtin__)
except:
    BUILTINS = list()
#BUILTINS.append('self')
BUILTINS = list(set(BUILTINS))

PY_BIN = (1, u" ".join(sorted(BUILTINS)))

#---- Syntax Style Specs ----#
SYNTAX_ITEMS = [ (stc.STC_P_DEFAULT, 'default_style'),
                 (stc.STC_P_CHARACTER, 'char_style'),
                 (stc.STC_P_CLASSNAME, 'class_style'),
                 (stc.STC_P_COMMENTBLOCK, 'comment_style'),
                 (stc.STC_P_COMMENTLINE, 'comment_style'),
                 (stc.STC_P_DECORATOR, 'decor_style'),
                 (stc.STC_P_DEFNAME, 'keyword3_style'),
                 (stc.STC_P_IDENTIFIER, 'default_style'),
                 (stc.STC_P_NUMBER, 'number_style'),
                 (stc.STC_P_OPERATOR, 'operator_style'),
                 (stc.STC_P_STRING, 'string_style'),
                 (stc.STC_P_STRINGEOL, 'stringeol_style'),
                 (stc.STC_P_TRIPLE, 'string_style'),
                 (stc.STC_P_TRIPLEDOUBLE, 'string_style'),
                 (stc.STC_P_WORD, 'keyword_style'),
                 (stc.STC_P_WORD2, 'userkw_style')]

#---- Extra Properties ----#
FOLD = ("fold", "1")
TIMMY = ("tab.timmy.whinge.level", "1") # Mark Inconsistent indentation

#-----------------------------------------------------------------------------#

class SyntaxData(syndata.SyntaxDataBase):
    """SyntaxData object for Python""" 
    def __init__(self, langid):
        syndata.SyntaxDataBase.__init__(self, langid)

        # Setup
        self.SetLexer(stc.STC_LEX_PYTHON)
        self.RegisterFeature(synglob.FEATURE_AUTOINDENT, AutoIndenter)

    def GetKeywords(self):
        """Returns Specified Keywords List """
        return [PY_KW, PY_BIN]

    def GetSyntaxSpec(self):
        """Syntax Specifications """
        return SYNTAX_ITEMS

    def GetProperties(self):
        """Returns a list of Extra Properties to set """
        return [FOLD, TIMMY]

    def GetCommentPattern(self):
        """Returns a list of characters used to comment a block of code """
        return [u'#']

#-----------------------------------------------------------------------------#

def AutoIndenter(stc, pos, ichar):
    """Auto indent python code. uses \n the text buffer will
    handle any eol character formatting.
    @param stc: EditraStyledTextCtrl
    @param pos: current carat position
    @param ichar: Indentation character
    @return: string

    """
    rtxt = u''
    line = stc.GetCurrentLine()
    spos = stc.PositionFromLine(line)
    text = stc.GetTextRange(spos, pos)
    epos = stc.GetLineEndPosition(line)
    inspace = text.isspace()

    # Cursor is in the indent area somewhere
    if inspace:
        return u"\n" + text

    # Check if the cursor is in column 0 and just return newline.
    if not len(text):
        return u"\n"

    # Ignore empty lines and backtrace to find the previous line that we can
    # get the indent position from
#    while text.isspace():
#        line -= 1
#        if line < 0:
#            return u''
#        text = stc.GetTextRange(stc.PositionFromLine(line), pos)

    indent = stc.GetLineIndentation(line)
    if ichar == u"\t":
        tabw = stc.GetTabWidth()
    else:
        tabw = stc.GetIndent()

    i_space = indent / tabw
    end_spaces = ((indent - (tabw * i_space)) * u" ")

    tokens = filter(None, text.strip().split())
    if tokens and not inspace:
        if tokens[-1].endswith(u":"):
            if tokens[0].rstrip(u":") in INDENT_KW:
                i_space += 1
        elif tokens[-1].endswith(u"\\"):
            i_space += 1
        elif tokens[0] in UNINDENT_KW:
            i_space = max(i_space - 1, 0)

    rval = u"\n" + (ichar * i_space) + end_spaces
    if inspace and ichar != u"\t":
        rpos = indent - (pos - spos)
        if rpos < len(rval) and rpos > 0:
            rval = rval[:-rpos]
        elif rpos >= len(rval):
            rval = u"\n"

    return rval

#---- End Required Module Functions ----#

#---- Syntax Modules Internal Functions ----#
def KeywordString():
    """Returns the specified Keyword String
    @note: not used by most modules

    """
    return PY_KW[1]

#---- End Syntax Modules Internal Functions ----#
