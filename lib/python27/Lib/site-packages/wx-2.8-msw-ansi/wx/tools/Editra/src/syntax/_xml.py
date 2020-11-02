###############################################################################
# Name: xml.py                                                                #
# Purpose: Define XML syntax for highlighting and other features              #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2007 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
FILE: xml.py
AUTHOR: Cody Precord
@summary: Lexer configuration module for XML Files.

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: _xml.py 62364 2009-10-11 01:02:12Z CJP $"
__revision__ = "$Revision: 62364 $"

#-----------------------------------------------------------------------------#
# Imports
import wx.stc as stc

# Local Imports
import synglob
import syndata
import _html

#-----------------------------------------------------------------------------#

#---- Keyword Specifications ----#

# Xml Keywords
XML_KEYWORDS = ("rss atom pubDate channel version title link description "
                "language generator item")

#-----------------------------------------------------------------------------#

class SyntaxData(syndata.SyntaxDataBase):
    """SyntaxData object for XML""" 
    def __init__(self, langid):
        syndata.SyntaxDataBase.__init__(self, langid)

        # Setup
        self.SetLexer(stc.STC_LEX_XML)

    def GetKeywords(self):
        """Returns Specified Keywords List """
        sgml = _html.KeywordString(synglob.ID_LANG_SGML)
        return [(5, XML_KEYWORDS + u" " + sgml)]

    def GetSyntaxSpec(self):
        """Syntax Specifications """
        return _html.SYNTAX_ITEMS

    def GetProperties(self):
        """Returns a list of Extra Properties to set """
        return [_html.FOLD, _html.FLD_HTML]

    def GetCommentPattern(self):
        """Returns a list of characters used to comment a block of code """
        return [u'<!--', u'-->']
