/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "stdafx.h"
#include "d3d9_8.h"
#include "Direct3DShader8.h"

LONG textsz = 0;
LPSTR sstream = NULL;
BOOL put_pnt = TRUE;

static const char * const rastout_names[] = { "oPos", "oFog", "oPts" };

static bool SupportedInstructions[] =
{
	true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,
	true,true,true,true,true,true,true,true,true,false,false,false,false,false,false,
	false,false,false,false,false,false,false,false,false,false,false,false,false,
	false,false,false,false,false,false,false,false,false,false,false,false,false,
	false,false,false,false,false,false,false,true,true,true,true,true,true,true,
	true,true,true,true,true,true,true,true,true,true,true
};


static const char * const CommandsNames[] =
{
	"nop","mov","add","sub","mad","mul","rcp","rsq","dp3","dp4","min","max","slt",
	"sge","exp","log","lit","dst","lrp","frc","m4x4","m4x3","m3x4","m3x3","m3x2",0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	"texcoord","texkill","tex","texbem","texbeml","texreg2ar","texreg2gb","texm3x2pad",
	"texm3x2tex","texm3x3pad","texm3x3tex","texm3x3diff","texm3x3spec","texm3x3vspec", 
	"expp", "logp","cnd","def"
};

static BYTE CommandOperandsCount[] =
{
	0,2,3,3,4,3,2,2,3,3,3,3,3,3,2,2,2,3,4,2,3,3,3,3,3,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,1,1,2,2,2,2,2,2,2,2,3,3,2, 2, 2, 4, 5	
};

static DWORD lineNumber = 0;

DWORD ParseDummyCommand(DWORD* command)
{
    DWORD length = 1;
    command++;
    while (*command & 0x80000000)
    {
        command++;
        length++;
    }
    return length;
}

void ParseRegister(DWORD reg)
{
    DWORD regnum = reg & D3DSP_REGNUM_MASK;

    switch ((reg & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT)
    {
    case D3DSPR_INPUT :
        textsz += sprintf( sstream+textsz,  "v%lu", regnum);
        break;

    case D3DSPR_TEXCRDOUT :
        textsz += sprintf( sstream+textsz,  "oT%lu", regnum);
        break;

    case D3DSPR_CONST :
		textsz += sprintf( sstream+textsz,  "c%lu%s", regnum,
			(reg & D3DVS_ADDRMODE_RELATIVE) ? "[a0.x]" : "");
        break;

    case D3DSPR_RASTOUT :
        textsz += sprintf( sstream+textsz,  "%s", rastout_names[regnum]);
        if(regnum > 0)
            put_pnt = FALSE;
        break;

    case D3DSPR_TEMP :
        textsz += sprintf( sstream+textsz,  "r%lu", regnum);
        break;

    case D3DSPR_ATTROUT :
        textsz += sprintf( sstream+textsz,  "oD%lu", regnum);
        break;

    case D3DSPR_ADDR :
        textsz += sprintf( sstream+textsz,  "a%lu", regnum);
        break;
    }
}

static DWORD opCode;

void ParseOutParam(DWORD param)
{
    ParseRegister(param);
	if(	put_pnt){
		if ((param & D3DSP_WRITEMASK_ALL) == D3DSP_WRITEMASK_ALL)
		{
			DWORD reg = (param & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT;
			switch (reg)
			{
			case D3DSPR_ADDR:
				param = D3DSP_WRITEMASK_0;
				break;
			case D3DSPR_RASTOUT:
				if((reg & D3DSP_REGNUM_MASK) > 0) // Scalar output registers
					param = D3DSP_WRITEMASK_0;
				break;
			}
		}
		else
		{
			if (opCode == D3DSIO_M4x4)
				param = D3DSP_WRITEMASK_ALL; // X5477
		}
        if ((param & D3DSP_WRITEMASK_ALL) != D3DSP_WRITEMASK_ALL)
        {
            textsz += sprintf( sstream+textsz,  ".");
            if (param & D3DSP_WRITEMASK_0)  textsz += sprintf( sstream+textsz,  "x");
            if (param & D3DSP_WRITEMASK_1)  textsz += sprintf( sstream+textsz,  "y");
            if (param & D3DSP_WRITEMASK_2)  textsz += sprintf( sstream+textsz,  "z");
            if (param & D3DSP_WRITEMASK_3)  textsz += sprintf( sstream+textsz,  "w");
        }
        put_pnt = TRUE;
    }
}

#define SWIZZLE0 0xE4
#define SWIZZLE1(x) ((x) & 0x03)
#define SWIZZLE2(x) (((x) & 0x0C) >> 2)
#define SWIZZLE3(x) (((x) & 0x30) >> 4)
#define SWIZZLE4(x) (((x) & 0xC0) >> 6)

void ParseSwizzle(DWORD param)
{
    char components[] = "xyzw";
    DWORD swizzle = (param & D3DVS_SWIZZLE_MASK) >> D3DVS_SWIZZLE_SHIFT;

    if (swizzle == SWIZZLE0) return;

    if (SWIZZLE1(swizzle) == SWIZZLE2(swizzle)
        && SWIZZLE1(swizzle) == SWIZZLE3(swizzle)
        && SWIZZLE1(swizzle) == SWIZZLE4(swizzle))
    {
        textsz += sprintf( sstream+textsz,  ".%c", components[SWIZZLE1(swizzle)]);
    }
    else
    {
        textsz += sprintf( sstream+textsz,  ".%c%c%c%c",
            components[SWIZZLE1(swizzle)],
            components[SWIZZLE2(swizzle)],
            components[SWIZZLE3(swizzle)],
            components[SWIZZLE4(swizzle)]);
    }
}


inline void ParseInParam(DWORD param)
{
    if ((param & D3DSP_SRCMOD_MASK) == D3DSPSM_NEG) textsz += sprintf( sstream+textsz,  "-");
    ParseRegister(param);
    ParseSwizzle(param);
}

int ParseVertexFunction(DWORD* command)
{
	lineNumber = 0;
    DWORD *start = command;
    command++;
    while (*command != D3DVS_END())
	{
		opCode = *command & D3DSI_OPCODE_MASK;
		if (opCode < sizeof(SupportedInstructions)){ 
			if(SupportedInstructions[opCode]){
				DWORD i;
				DWORD num_params = CommandOperandsCount[opCode];

				textsz += sprintf( sstream+textsz,  id);
				textsz += sprintf( sstream+textsz,  CommandsNames[opCode]);
				textsz += sprintf( sstream+textsz,  " ");
				command++;

				if (num_params > 0)
				{
					ParseOutParam(*command++);

					for( i=1; i<num_params; i++ ) 
					{
						textsz += sprintf( sstream+textsz,  ", ");
						ParseInParam(*command++);
					}
				}
				textsz += sprintf( sstream+textsz,  nl);
				lineNumber++;
			} 
			else
			{
				DEBUG_TRACE1("ERROR: Not supported vertex shader command %d\n", *command); //???
				command++; // skip one DWORD
			}
		}
		else if (opCode == D3DSIO_COMMENT) {
			command += ((*command & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT) + 1;
		} else {
			command += ParseDummyCommand(command);
		}
    }
    return (command - start + 1);
}
