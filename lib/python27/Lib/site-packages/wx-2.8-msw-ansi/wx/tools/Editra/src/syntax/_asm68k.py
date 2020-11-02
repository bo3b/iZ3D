###############################################################################
# Name: asm68k.py                                                             #
# Purpose: Define 68k assembly syntax for highlighting and other features     #
# Author: Cody Precord <cprecord@editra.org>                                  #
# Copyright: (c) 2007 Cody Precord <staff@editra.org>                         #
# License: wxWindows License                                                  #
###############################################################################

"""
FILE: asm68k.py
AUTHOR: Cody Precord
@summary: Lexer configuration file 68k Assembly Code
@todo: more color configuration

"""

__author__ = "Cody Precord <cprecord@editra.org>"
__svnid__ = "$Id: _asm68k.py 63834 2010-04-03 06:04:33Z CJP $"
__revision__ = "$Revision: 63834 $"

#-----------------------------------------------------------------------------#
# Imports
import wx.stc as stc

# Local Imports
import synglob
import syndata

#-----------------------------------------------------------------------------#

#---- Keyword Definitions ----#

ASM_CPU_INST = (0, "andi and as b beq bg b bl bne bge bpl bchg bclr bfchg "
                   "bfclr bfexts bfextu bfffo bfins bfset bftst bkpt bra bset "
                   "bsr btst callm cas2 cas chk2 chk clr cmpa cmpi cmpm cmp "
                   "dbcc dbcs dbvc dbvs dbeq dbf dbt dbge dbgt dbhi dbmi dble "
                   "dbls dblt dbne dbpl dbra eori eor exg extb ext illegaljmp "
                   "jsr lea link ls mm movea movec movem movep moveq moves "
                   "move nbcd negx neg nop not ori or pack pea reset ro rox rt "
                   "sbcd seq sne spl swap tas trap tst unlk unpk abcd")

ASM_MATH_INST = (1, "adda addi addq addx add div mul suba subi subq subx sub "
                    "tdiv")

ASM_REGISTER = (2, "a0 a1 a2 a3 a4 a5 a6 a7 d0 d1 d2 d3 d4 d5 d6 d7 pc sr "
                   "ccr sp usp ssp vbr sfc sfcr dfc dfcr msp isp zpc cacr "
                   "caar za0 za1 za2 za3 za4 za5 za6 za7 zd0 zd1 zd2 zd3 "
                   "zd4 zd5 zd6 zd7 crp srp tc ac0 ac1 acusr tt0 tt1 mmusr "
                   "dtt0 dtt1 itt0 itt1 urp cal val scc crp srp drp tc ac psr "
                   "pcsr bac0 bac1 bac2 bac3 bac4 bac5 bac6 bac7 bad0 bad1 "
                   "bad2 bad3 bad4 bad5 bad6 bad7 fp0 fp1 fp2 fp3 fp4 fp5 fp6 "
                   "fp7 control status iaddr fpcr fpsr fpiar ")

ASM_DIRECTIVES = (3, "ALIGN CHIP COMLINE COMMON DC DCB DS END EQU FEQU FAIL "
                     "FOPT IDNT LLEN MASK2 NAME NOOBJ OFFSET OPT ORG PLEN REG "
                     "RESTORE SAVE SECT SECTION SET SPC TTL XCOM XDEF XREF")

#---- Language Styling Specs ----#
SYNTAX_ITEMS = [ (stc.STC_ASM_DEFAULT, 'default_style'),
                 (stc.STC_ASM_CHARACTER, 'char_style'),
                 (stc.STC_ASM_COMMENT, 'comment_style'),
                 (stc.STC_ASM_COMMENTBLOCK, 'comment_style'),
                 (stc.STC_ASM_CPUINSTRUCTION, 'keyword_style'),
                 (stc.STC_ASM_DIRECTIVE, 'keyword3_style'),
                 (stc.STC_ASM_DIRECTIVEOPERAND, 'keyword4_style'),
                 (stc.STC_ASM_EXTINSTRUCTION, 'funct_style'),
                 (stc.STC_ASM_IDENTIFIER, 'default_style'),
                 (stc.STC_ASM_MATHINSTRUCTION, 'keyword_style'),
                 (stc.STC_ASM_NUMBER, 'number_style'),
                 (stc.STC_ASM_OPERATOR, 'operator_style'),
                 (stc.STC_ASM_REGISTER, 'keyword2_style'),
                 (stc.STC_ASM_STRING, 'string_style'),
                 (stc.STC_ASM_STRINGEOL, 'stringeol_style') ]

#-----------------------------------------------------------------------------#

class SyntaxData(syndata.SyntaxDataBase):
    """SyntaxData object for 68k assembly files""" 
    def __init__(self, langid):
        syndata.SyntaxDataBase.__init__(self, langid)

        # Setup
        # synglob.ID_LANG_68K
        self.SetLexer(stc.STC_LEX_ASM)

    def GetKeywords(self):
        """Returns Specified Keywords List"""
        return [ASM_CPU_INST, ASM_MATH_INST, ASM_REGISTER, ASM_DIRECTIVES]

    def GetSyntaxSpec(self):
        """Syntax Specifications"""
        return SYNTAX_ITEMS

    def GetCommentPattern(self):
        """Returns a list of characters used to comment a block of code """
        return [u';']
