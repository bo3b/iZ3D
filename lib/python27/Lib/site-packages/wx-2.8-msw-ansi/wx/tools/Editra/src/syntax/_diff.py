###############################################################################
# Name: diff.py                                                               #
# Purpose: Define Diff/Patch file syntax for highlighting and other features  #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2007 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
FILE: diff.py
AUTHOR: Cody Precord
@summary: Lexer configuration module for Diff/Patch files
@todo:

"""

__author__ = "Cody Precord <cprecord@editra.org"
__svnid__ = "$Id: _diff.py 63834 2010-04-03 06:04:33Z CJP $"
__revision__ = "$Revision: 63834 $"

#-----------------------------------------------------------------------------#
# Imports
import wx.stc as stc

# Local Imports
import synglob
import syndata

#-----------------------------------------------------------------------------#

#---- Keyword Definitions ----#
# None
#---- End Keyword Definitions ----#

#---- Syntax Style Specs ----#
SYNTAX_ITEMS = [(stc.STC_DIFF_ADDED,    'global_style'),
                (stc.STC_DIFF_COMMAND,  'pre_style'),
                (stc.STC_DIFF_COMMENT,  'comment_style'),
                (stc.STC_DIFF_DEFAULT,  'default_style'),
                (stc.STC_DIFF_DELETED,  'error_style'),
                (stc.STC_DIFF_HEADER,   'comment_style'),
                (stc.STC_DIFF_POSITION, 'pre_style')]

#---- Extra Properties ----#
FOLD = ('fold', '1')
FOLD_COMPACT = ('fold.compact', '1')

#-----------------------------------------------------------------------------#

class SyntaxData(syndata.SyntaxDataBase):
    """SyntaxData object for Diff files""" 
    def __init__(self, langid):
        syndata.SyntaxDataBase.__init__(self, langid)

        # Setup
        self.SetLexer(stc.STC_LEX_DIFF)

    def GetSyntaxSpec(self):
        """Syntax Specifications """
        return SYNTAX_ITEMS

    def GetProperties(self):
        """Returns a list of Extra Properties to set """
        return [FOLD, FOLD_COMPACT]

    def GetCommentPattern(self):
        """Returns a list of characters used to comment a block of code """
        return ['--- ']
