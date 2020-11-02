###############################################################################
# Name: cobra.py                                                              #
# Purpose: Define Cobra syntax for highlighting and other features            #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2009 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
Define support for Cobra programming language. 
@summary: Lexer configuration module for Cobra.

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: _cobra.py 63834 2010-04-03 06:04:33Z CJP $"
__revision__ = "$Revision: 63834 $"

#-----------------------------------------------------------------------------#
# Imports
import wx.stc as stc

# Local Imports
import synglob
import syndata

#-----------------------------------------------------------------------------#

# Indenter keywords
INDENT_KW = (u"body", u"branch",u"class", u"cue", u"def", u"else", u"except", 
             u"expect", u"finally", u"for", u"if", u"invariant", u"namespace",
             u"on" u"post", u"shared", u"success", u"test", u"try", u"while")

UNINDENT_KW = (u"return", u"raise", u"break", u"continue", u"pass")

# Cobra Keywords
KEYWORDS = ("abstract adds all and any as assert base be body bool branch "
            "break callable catch char class const continue cue decimal def do"
            "dynamic each else end ensure enum event every except expect "
            "extend extern fake false finally float for from get has if ignore "
            "implements implies import in inherits inlined inout int interface "
            "internal invariant is listen mixin must namespace new nil "
            "nonvirtual not number objc of off old on or out override partial "
            "pass passthrough post print private pro protected public raise "
            "ref require return same set shared sig stop struct success test "
            "this throw to to\\? trace true try uint use using var vari "
            "virtual where while yield")
KEYWORDS = (0, KEYWORDS)

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
TIMMY = ("tab.timmy.whinge.level", "1") # Mark Inconsistant indentation

#-----------------------------------------------------------------------------#

class SyntaxData(syndata.SyntaxDataBase):
    """SyntaxData object for Cobra""" 
    def __init__(self, langid):
        syndata.SyntaxDataBase.__init__(self, langid)

        # Setup
        self.SetLexer(stc.STC_LEX_PYTHON)
        self.RegisterFeature(synglob.FEATURE_AUTOINDENT, AutoIndenter)

    def GetKeywords(self):
        """Returns Specified Keywords List """
        return [KEYWORDS,]

    def GetSyntaxSpec(self):
        """Syntax Specifications """
        return SYNTAX_ITEMS

    def GetProperties(self):
        """Returns a list of Extra Properties to set """
        return [FOLD, TIMMY]

    def GetCommentPattern(self):
        """Returns a list of characters used to comment a block of code """
        return [u'#',]

#-----------------------------------------------------------------------------#

def AutoIndenter(stc, pos, ichar):
    """Auto indent cobra code. uses \n the text buffer will
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

    indent = stc.GetLineIndentation(line)
    if ichar == u"\t":
        tabw = stc.GetTabWidth()
    else:
        tabw = stc.GetIndent()

    i_space = indent / tabw
    end_spaces = ((indent - (tabw * i_space)) * u" ")

    tokens = filter(None, text.strip().split())
    if tokens and not inspace:
        if tokens[-1].endswith(u""):
            if tokens[0] in INDENT_KW:
                i_space += 1
            elif tokens[0] in UNINDENT_KW:
                i_space = max(i_space - 1, 0)
        elif tokens[-1].endswith(u"\\"):
            i_space += 1

    rval = u"\n" + (ichar * i_space) + end_spaces
    if inspace and ichar != u"\t":
        rpos = indent - (pos - spos)
        if rpos < len(rval) and rpos > 0:
            rval = rval[:-rpos]
        elif rpos >= len(rval):
            rval = u"\n"

    return rval
