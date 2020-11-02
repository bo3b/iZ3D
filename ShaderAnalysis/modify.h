#pragma once
#include "disasm.h"

struct ModifiedShaderData
{
	unsigned			CBIndex;
	unsigned			dp4VectorRegister;
	unsigned			ZNearRegister;
	bool				ModifiedShaderAvailable;
	D3D10DDI_HSHADER	ModifiedShaderHandle;	//--- destroyed in Destroy() command, not in this class ---
	float				StereoZNear;
	float				Multiplier;
	float				ConvergenceShift;

	ModifiedShaderData() 
		:	CBIndex(0)
		,	dp4VectorRegister(0)
		,	ZNearRegister(0)
		,	ModifiedShaderAvailable(false)
		,	StereoZNear(1.0f)
		,	Multiplier(1.0f)
		,	ConvergenceShift(0.0f)
	{
		ModifiedShaderHandle.pDrvPrivate = NULL;
	}
};

//--- modified shader data ---

//--- simple finding of free register - use only shader declaration ---
//--- we must modify dcl_temp instruction for this method ---
//--- return -1 if no free registers found ---
//--- dclOpCodeIndex = -1 if no D3D10_SB_OPCODE_DCL_TEMPS found ---
bool FindFreeRegisters( const shader_analyzer::TShaderList& originalShader, unsigned* reg, unsigned regCount, bool& needTmpDCLModification, unsigned& dclTempOpCodeIndex );

//--- shader modifier functions, returns true if shader modified successfully ---
bool ModifyShader( const shader_analyzer::TShaderList& originalShader, unsigned outputRegisterIndex, bool AddZNearCheck, shader_analyzer::TShaderList& modifiedShader, ModifiedShaderData& MData );
