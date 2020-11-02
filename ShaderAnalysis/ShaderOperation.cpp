#include "stdafx.h"
#include "ShaderOperation.h"
#include <iomanip>

shader_analyzer::ShaderOperation::ShaderOperation(const unsigned* data): m_pData(data), m_length(0)
{
	if(!m_pData)
		return;
	m_length++;
	if(isOpcodeExtended())
	{
		m_length++;
	}
	if (getType() == D3D10_SB_OPCODE_CUSTOMDATA)
		m_length = m_pData[1];
}

//////////////////////////////////////////////////////////////////////////
shader_analyzer::ShaderOperation::ShaderOperation( const ShaderOperation& ShOp )
{
	m_pData = ShOp.m_pData;
	m_length = ShOp.m_length;
}

shader_analyzer::ShaderOperation::ShaderOperation()
{
	// TODO
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
std::ostream& shader_analyzer::operator << (std::ostream& os, const shader_analyzer::ShaderOperation& sdOperation)
{
	if(sdOperation.isCustomData())
	{	
		D3D10_SB_CUSTOMDATA_CLASS cdClass = sdOperation.getCustomDataClass();
		switch (cdClass)
		{
		case D3D10_SB_CUSTOMDATA_COMMENT:
			os << "// Comment ";
			break;
		case D3D10_SB_CUSTOMDATA_DEBUGINFO:
			os << "// DebugInfo ";
			break;
		case D3D10_SB_CUSTOMDATA_OPAQUE:
			os << "// Opaque ";
			break;
		case D3D10_SB_CUSTOMDATA_DCL_IMMEDIATE_CONSTANT_BUFFER:
			os << "// Immediate CB: ";
			break;
		default:
			_ASSERT(FALSE);
			break;
		}
		// custom data block
		// write custom data as array of chars
		if (cdClass != D3D10_SB_CUSTOMDATA_DCL_IMMEDIATE_CONSTANT_BUFFER)
		{
			for(unsigned i = 2; i < sdOperation.m_length; i++)
			{
				UINT data = sdOperation.m_pData[i];
				os << "0x" << std::setfill('0') << std::setw(8) << std::hex << data << " ";
			}
		}
		else
		{
			unsigned c = (sdOperation.m_length - 2) / 4;
			for(unsigned i = 0; i < c; i++)
			{
				os << "(";
				for(unsigned j = 0; j < 4; j++)
				{
					float data = ((float*)sdOperation.m_pData)[i * 4 + j + 2];
					os << data << " ";
				}
				os << ") ";
			}
		}
	}
	else
	{
		// standard opcode token
		std::string resStr;
		D3D10_SB_OPCODE_TYPE opCode = sdOperation.getType();
		switch(opCode)
		{
		case D3D10_SB_OPCODE_ADD:
			resStr += "add";
			break;
		case D3D10_SB_OPCODE_AND:
			resStr += "and";
			break;
		case D3D10_SB_OPCODE_BREAK:
			resStr += "break";
			break;
		case D3D10_SB_OPCODE_BREAKC:
			resStr += "breakc";
			break;
		case D3D10_SB_OPCODE_CALL:
			resStr += "call";
			break;
		case D3D10_SB_OPCODE_CALLC:
			resStr += "callc";
			break;
		case D3D10_SB_OPCODE_CASE:
			resStr += "case";
			break;
		case D3D10_SB_OPCODE_CONTINUE:
			resStr += "continue";
			break;
		case D3D10_SB_OPCODE_CONTINUEC:
			resStr += "continuec";
			break;
		case D3D10_SB_OPCODE_CUT:
			resStr += "cut";
			break;
		case D3D10_SB_OPCODE_DEFAULT:
			resStr += "default";
			break;
		case D3D10_SB_OPCODE_DERIV_RTX:
			resStr += "deriv_rtx";
			break;
		case D3D10_SB_OPCODE_DERIV_RTY:
			resStr += "deriv_rty";
			break;
		case D3D10_SB_OPCODE_DISCARD:
			resStr += "discard";
			break;
		case D3D10_SB_OPCODE_DIV:
			resStr += "div";
			break;
		case D3D10_SB_OPCODE_DP2:
			resStr += "dp2";
			break;
		case D3D10_SB_OPCODE_DP3:
			resStr += "dp3";
			break;
		case D3D10_SB_OPCODE_DP4:
			resStr += "dp4";
			break;
		case D3D10_SB_OPCODE_ELSE:
			resStr += "else";
			break;
		case D3D10_SB_OPCODE_EMIT:
			resStr += "emit";
			break;
		case D3D10_SB_OPCODE_EMITTHENCUT:
			resStr += "emitthencut";
			break;
		case D3D10_SB_OPCODE_ENDIF:
			resStr += "endif";
			break;
		case D3D10_SB_OPCODE_ENDLOOP:
			resStr += "endloop";
			break;
		case D3D10_SB_OPCODE_ENDSWITCH:
			resStr += "endswitch";
			break;
		case D3D10_SB_OPCODE_EQ:
			resStr += "eq";
			break;
		case D3D10_SB_OPCODE_EXP:
			resStr += "exp";
			break;
		case D3D10_SB_OPCODE_FRC:
			resStr += "frc";
			break;
		case D3D10_SB_OPCODE_FTOI:
			resStr += "ftoi";
			break;
		case D3D10_SB_OPCODE_FTOU:
			resStr += "ftou";
			break;
		case D3D10_SB_OPCODE_GE:
			resStr += "ge";
			break;
		case D3D10_SB_OPCODE_IADD:
			resStr += "iadd";
			break;
		case D3D10_SB_OPCODE_IF:
			resStr += "if";
			break;
		case D3D10_SB_OPCODE_IEQ:
			resStr += "ieq";
			break;
		case D3D10_SB_OPCODE_IGE:
			resStr += "ige";
			break;
		case D3D10_SB_OPCODE_ILT:
			resStr += "ilt";
			break;
		case D3D10_SB_OPCODE_IMAD:
			resStr += "imad";
			break;
		case D3D10_SB_OPCODE_IMAX:
			resStr += "imax";
			break;
		case D3D10_SB_OPCODE_IMIN:
			resStr += "imin";
			break;
		case D3D10_SB_OPCODE_IMUL:
			resStr += "imul";
			break;
		case D3D10_SB_OPCODE_INE:
			resStr += "ine";
			break;
		case D3D10_SB_OPCODE_INEG:
			resStr += "ineg";
			break;
		case D3D10_SB_OPCODE_ISHL:
			resStr += "ishl";
			break;
		case D3D10_SB_OPCODE_ISHR:
			resStr += "ishr";
			break;
		case D3D10_SB_OPCODE_ITOF:
			resStr += "itof";
			break;
		case D3D10_SB_OPCODE_LABEL:
			resStr += "label";
			break;
		case D3D10_SB_OPCODE_LD:
			resStr += "ld";
			break;
		case D3D10_SB_OPCODE_LD_MS:
			resStr += "ld_ms";
			break;
		case D3D10_SB_OPCODE_LOG:
			resStr += "log";
			break;
		case D3D10_SB_OPCODE_LOOP:
			resStr += "loop";
			break;
		case D3D10_SB_OPCODE_LT:
			resStr += "lt";
			break;
		case D3D10_SB_OPCODE_MAD:
			resStr += "mad";
			break;
		case D3D10_SB_OPCODE_MIN:
			resStr += "min";
			break;
		case D3D10_SB_OPCODE_MAX:
			resStr += "max";
			break;
		case D3D10_SB_OPCODE_MOV:
			resStr += "mov";
			break;
		case D3D10_SB_OPCODE_MOVC:
			resStr += "movc";
			break;
		case D3D10_SB_OPCODE_MUL:
			resStr += "mul";
			break;
		case D3D10_SB_OPCODE_NE:
			resStr += "ne";
			break;
		case D3D10_SB_OPCODE_NOP:
			resStr += "nop";
			break;
		case D3D10_SB_OPCODE_NOT:
			resStr += "not";
			break;
		case D3D10_SB_OPCODE_OR:
			resStr += "or";
			break;
		case D3D10_SB_OPCODE_RESINFO:
			resStr += "resinfo";
			break;
		case D3D10_SB_OPCODE_RET:
			resStr += "ret";
			break;
		case D3D10_SB_OPCODE_RETC:
			resStr += "retc";
			break;
		case D3D10_SB_OPCODE_ROUND_NE:
			resStr += "round_ne";
			break;
		case D3D10_SB_OPCODE_ROUND_NI:
			resStr += "round_ni";
			break;
		case D3D10_SB_OPCODE_ROUND_PI:
			resStr += "round_pi";
			break;
		case D3D10_SB_OPCODE_ROUND_Z:
			resStr += "round_z";
			break;
		case D3D10_SB_OPCODE_RSQ:
			resStr += "rsq";
			break;
		case D3D10_SB_OPCODE_SAMPLE:
			resStr += "sample";
			break;
		case D3D10_SB_OPCODE_SAMPLE_C:
			resStr += "sample_c";
			break;
		case D3D10_SB_OPCODE_SAMPLE_C_LZ:
			resStr += "sample_c_lz";
			break;
		case D3D10_SB_OPCODE_SAMPLE_L:
			resStr += "sample_l";
			break;
		case D3D10_SB_OPCODE_SAMPLE_D:
			resStr += "sample_d";
			break;
		case D3D10_SB_OPCODE_SAMPLE_B:
			resStr += "sample_b";
			break;
		case D3D10_SB_OPCODE_SQRT:
			resStr += "sqrt";
			break;
		case D3D10_SB_OPCODE_SWITCH:
			resStr += "switch";
			break;
		case D3D10_SB_OPCODE_SINCOS:
			resStr += "sincos";
			break;
		case D3D10_SB_OPCODE_UDIV:
			resStr += "udiv";
			break;
		case D3D10_SB_OPCODE_ULT:
			resStr += "ult";
			break;
		case D3D10_SB_OPCODE_UGE:
			resStr += "uge";
			break;
		case D3D10_SB_OPCODE_UMUL:
			resStr += "umul";
			break;
		case D3D10_SB_OPCODE_UMAD:
			resStr += "umad";
			break;
		case D3D10_SB_OPCODE_UMAX:
			resStr += "umax";
			break;
		case D3D10_SB_OPCODE_UMIN:
			resStr += "umin";
			break;
		case D3D10_SB_OPCODE_USHR:
			resStr += "ushr";
			break;
		case D3D10_SB_OPCODE_UTOF:
			resStr += "utof";
			break;
		case D3D10_SB_OPCODE_XOR:
			resStr += "xor";
			break;
		case D3D10_SB_OPCODE_DCL_RESOURCE:// DCL* opcodes have
			resStr += "dcl_resource";
			break;
		case D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER:// custom operand formats.
			resStr += "dcl_constantbuffer";
			break;
		case D3D10_SB_OPCODE_DCL_SAMPLER:
			resStr += "dcl_sampler";
			break;
		case D3D10_SB_OPCODE_DCL_INDEX_RANGE:
			resStr += "dcl_index_range";
			break;
		case D3D10_SB_OPCODE_DCL_GS_OUTPUT_PRIMITIVE_TOPOLOGY:
			resStr += "dcl_gs_output_primitive_topology";
			break;
		case D3D10_SB_OPCODE_DCL_GS_INPUT_PRIMITIVE:
			resStr += "dcl_gs_input_primitive";
			break;
		case D3D10_SB_OPCODE_DCL_MAX_OUTPUT_VERTEX_COUNT:
			resStr += "dcl_max_output_vertex_count";
			break;
		case D3D10_SB_OPCODE_DCL_INPUT:
			resStr += "dcl_input";
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_SGV:
			resStr += "dcl_input_sgv";
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_SIV:
			resStr += "dcl_input_siv";
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_PS:
			resStr += "dcl_input_ps";
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_PS_SGV:
			resStr += "dcl_input_ps_sgv";
			break;
		case D3D10_SB_OPCODE_DCL_INPUT_PS_SIV:
			resStr += "dcl_input_ps_siv";
			break;
		case D3D10_SB_OPCODE_DCL_OUTPUT:
			resStr += "dcl_output";
			break;
		case D3D10_SB_OPCODE_DCL_OUTPUT_SGV:
			resStr += "dcl_output_sgv";
			break;
		case D3D10_SB_OPCODE_DCL_OUTPUT_SIV:
			resStr += "dcl_output_siv";
			break;
		case D3D10_SB_OPCODE_DCL_TEMPS:
			resStr += "dcl_temps";
			break;
		case D3D10_SB_OPCODE_DCL_INDEXABLE_TEMP:
			resStr += "dcl_indexable_temp";
			break;
		case D3D10_SB_OPCODE_DCL_GLOBAL_FLAGS:
			resStr += "dcl_global_flags";
			break;
			// ---------- DX 10.1 op codes---------------------
		case D3D10_1_SB_OPCODE_LOD:
			resStr += "lod";
			break;
		case D3D10_1_SB_OPCODE_GATHER4:
			resStr += "gather4";
			break;
		case D3D10_1_SB_OPCODE_SAMPLE_POS:
			resStr += "sample_pos";
			break;
		case D3D10_1_SB_OPCODE_SAMPLE_INFO:
			resStr += "sample_info";
			break;
		case D3D10_1_SB_OPCODE_RESERVED1:
				break;
		case D3D11_SB_OPCODE_HS_DECLS:
			resStr += "hs_decls";
			break;
		case D3D11_SB_OPCODE_HS_CONTROL_POINT_PHASE:
			resStr += "hs_control_point_phase";
			break;
		case D3D11_SB_OPCODE_HS_FORK_PHASE:
			resStr += "hs_fork_phase";
			break;
		case D3D11_SB_OPCODE_HS_JOIN_PHASE:
			resStr += "hs_join_phase";
			break;
		case D3D11_SB_OPCODE_EMIT_STREAM:
			resStr += "emit_stream";
			break;
		case D3D11_SB_OPCODE_CUT_STREAM:
			resStr += "cut_stream";
			break;
		case D3D11_SB_OPCODE_EMITTHENCUT_STREAM:
			resStr += "emitthencut_stream";
			break;
		case D3D11_SB_OPCODE_INTERFACE_CALL:
			resStr += "interface_call";
			break;

		case D3D11_SB_OPCODE_BUFINFO:
			resStr += "bufinfo";
			break;
		case D3D11_SB_OPCODE_DERIV_RTX_COARSE:
			resStr += "deriv_rtx_coarse";
			break;
		case D3D11_SB_OPCODE_DERIV_RTX_FINE:
			resStr += "deriv_rtx_fine";
			break;
		case D3D11_SB_OPCODE_DERIV_RTY_COARSE:
			resStr += "deriv_rty_coarse";
			break;
		case D3D11_SB_OPCODE_DERIV_RTY_FINE:
			resStr += "deriv_rty_fine";
			break;
		case D3D11_SB_OPCODE_GATHER4_C:
			resStr += "gather4_c";
			break;
		case D3D11_SB_OPCODE_GATHER4_PO:
			resStr += "gather4_po";
			break;
		case D3D11_SB_OPCODE_GATHER4_PO_C:
			resStr += "gather4_po_c";
			break;
		case D3D11_SB_OPCODE_RCP:
			resStr += "rcp";
			break;
		case D3D11_SB_OPCODE_F32TOF16:
			resStr += "f32tof16";
			break;
		case D3D11_SB_OPCODE_F16TOF32:
			resStr += "f16tof32";
			break;
		case D3D11_SB_OPCODE_UADDC:
			resStr += "uaddc";
			break;
		case D3D11_SB_OPCODE_USUBB:
			resStr += "usubb";
			break;
		case D3D11_SB_OPCODE_COUNTBITS:
			resStr += "countbits";
			break;
		case D3D11_SB_OPCODE_FIRSTBIT_HI:
			resStr += "firstbit_hi";
			break;
		case D3D11_SB_OPCODE_FIRSTBIT_LO:
			resStr += "firstbit_lo";
			break;
		case D3D11_SB_OPCODE_FIRSTBIT_SHI:
			resStr += "firstbit_shi";
			break;
		case D3D11_SB_OPCODE_UBFE:
			resStr += "ubfe";
			break;
		case D3D11_SB_OPCODE_IBFE:
			resStr += "ibfe";
			break;
		case D3D11_SB_OPCODE_BFI:
			resStr += "bfi";
			break;
		case D3D11_SB_OPCODE_BFREV:
			resStr += "bfrev";
			break;
		case D3D11_SB_OPCODE_SWAPC:
			resStr += "swapc";
			break;
		case D3D11_SB_OPCODE_DCL_STREAM:
			resStr += "dcl_stream";
			break;
		case D3D11_SB_OPCODE_DCL_FUNCTION_BODY:
			resStr += "dcl_function_body";
			break;
		case D3D11_SB_OPCODE_DCL_FUNCTION_TABLE:
			resStr += "dcl_function_table";
			break;
		case D3D11_SB_OPCODE_DCL_INTERFACE:
			resStr += "dcl_interface";
			break;
		case D3D11_SB_OPCODE_DCL_INPUT_CONTROL_POINT_COUNT:
			resStr += "dcl_input_control_point_count";
			break;
		case D3D11_SB_OPCODE_DCL_OUTPUT_CONTROL_POINT_COUNT:
			resStr += "dcl_output_control_point_count";
			break;
		case D3D11_SB_OPCODE_DCL_TESS_DOMAIN:
			resStr += "dcl_tess_domain";
			break;
		case D3D11_SB_OPCODE_DCL_TESS_PARTITIONING:
			resStr += "dcl_tess_partitioning";
			break;
		case D3D11_SB_OPCODE_DCL_TESS_OUTPUT_PRIMITIVE:
			resStr += "dcl_tess_output_primitive";
			break;
		case D3D11_SB_OPCODE_DCL_HS_MAX_TESSFACTOR:
			resStr += "dcl_hs_max_tessfactor";
			break;
		case D3D11_SB_OPCODE_DCL_HS_FORK_PHASE_INSTANCE_COUNT:
			resStr += "dcl_hs_fork_phase_instance_count";
			break;
		case D3D11_SB_OPCODE_DCL_HS_JOIN_PHASE_INSTANCE_COUNT:
			resStr += "dcl_hs_join_phase_instance_count";
			break;
		case D3D11_SB_OPCODE_DCL_THREAD_GROUP:
			resStr += "dcl_thread_group";
			break;
		case D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_TYPED:
			resStr += "dcl_unordered_access_view_typed";
			break;
		case D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_RAW:
			resStr += "dcl_unordered_access_view_raw";
			break;
		case D3D11_SB_OPCODE_DCL_UNORDERED_ACCESS_VIEW_STRUCTURED:
			resStr += "dcl_unordered_access_view_structured";
			break;
		case D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_RAW:
			resStr += "dcl_thread_group_shared_memory_raw";
			break;
		case D3D11_SB_OPCODE_DCL_THREAD_GROUP_SHARED_MEMORY_STRUCTURED:
			resStr += "dcl_thread_group_shared_memory_structured";
			break;
		case D3D11_SB_OPCODE_DCL_RESOURCE_RAW:
			resStr += "dcl_resource_raw";
			break;
		case D3D11_SB_OPCODE_DCL_RESOURCE_STRUCTURED:
			resStr += "dcl_resource_structured";
			break;
		case D3D11_SB_OPCODE_LD_UAV_TYPED:
			resStr += "ld_uav_typed";
			break;
		case D3D11_SB_OPCODE_STORE_UAV_TYPED:
			resStr += "store_uav_typed";
			break;
		case D3D11_SB_OPCODE_LD_RAW:
			resStr += "ld_raw";
			break;
		case D3D11_SB_OPCODE_STORE_RAW:
			resStr += "store_raw";
			break;
		case D3D11_SB_OPCODE_LD_STRUCTURED:
			resStr += "ld_structured";
			break;
		case D3D11_SB_OPCODE_STORE_STRUCTURED:
			resStr += "store_structured";
			break;
		case D3D11_SB_OPCODE_ATOMIC_AND:
			resStr += "atomic_and";
			break;
		case D3D11_SB_OPCODE_ATOMIC_OR:
			resStr += "atomic_or";
			break;
		case D3D11_SB_OPCODE_ATOMIC_XOR:
			resStr += "atomic_xor";
			break;
		case D3D11_SB_OPCODE_ATOMIC_CMP_STORE:
			resStr += "atomic_cmp_store";
			break;
		case D3D11_SB_OPCODE_ATOMIC_IADD:
			resStr += "atomic_iadd";
			break;
		case D3D11_SB_OPCODE_ATOMIC_IMAX:
			resStr += "atomic_imax";
			break;
		case D3D11_SB_OPCODE_ATOMIC_IMIN:
			resStr += "atomic_imin";
			break;
		case D3D11_SB_OPCODE_ATOMIC_UMAX:
			resStr += "atomic_umax";
			break;
		case D3D11_SB_OPCODE_ATOMIC_UMIN:
			resStr += "atomic_umin";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_ALLOC:
			resStr += "imm_atomic_alloc";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_CONSUME:
			resStr += "imm_atomic_consume";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_IADD:
			resStr += "imm_atomic_iadd";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_AND:
			resStr += "imm_atomic_and";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_OR:
			resStr += "imm_atomic_or";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_XOR:
			resStr += "imm_atomic_xor";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_EXCH:
			resStr += "imm_atomic_exch";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_CMP_EXCH:
			resStr += "imm_atomic_cmp_exch";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_IMAX:
			resStr += "imm_atomic_imax";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_IMIN:
			resStr += "imm_atomic_imin";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_UMAX:
			resStr += "imm_atomic_umax";
			break;
		case D3D11_SB_OPCODE_IMM_ATOMIC_UMIN:
			resStr += "imm_atomic_umin";
			break;   
		case D3D11_SB_OPCODE_SYNC:
			resStr += "sync";
			break;
		case D3D11_SB_OPCODE_DADD:
			resStr += "dadd";
			break;
		case D3D11_SB_OPCODE_DMAX:
			resStr += "dmax";
			break;
		case D3D11_SB_OPCODE_DMIN:
			resStr += "dmin";
			break;
		case D3D11_SB_OPCODE_DMUL:
			resStr += "dmul";
			break;
		case D3D11_SB_OPCODE_DEQ:
			resStr += "deq";
			break;
		case D3D11_SB_OPCODE_DGE:
			resStr += "dge";
			break;
		case D3D11_SB_OPCODE_DLT:
			resStr += "dlt";
			break;
		case D3D11_SB_OPCODE_DNE:
			resStr += "dne";
			break;
		case D3D11_SB_OPCODE_DMOV:
			resStr += "dmov";
			break;
		case D3D11_SB_OPCODE_DMOVC:
			resStr += "dmovc";
			break;
		case D3D11_SB_OPCODE_DTOF:
			resStr += "dtof";
			break;
		case D3D11_SB_OPCODE_FTOD:
			resStr += "ftod";
			break;
		case D3D11_SB_OPCODE_EVAL_SNAPPED:
			resStr += "eval_snapped";
			break;
		case D3D11_SB_OPCODE_EVAL_SAMPLE_INDEX:
			resStr += "eval_sample_index";
			break;
		case D3D11_SB_OPCODE_EVAL_CENTROID:
			resStr += "eval_centroid";
			break;
		case D3D11_SB_OPCODE_DCL_GS_INSTANCE_COUNT:
			resStr += "dcl_gs_instance_count";
			break;
		case D3D10_SB_NUM_OPCODES:// Should be the last entry
			{
				break;
			}
		default:
			resStr += "// unsupported coomand";
			break;
		}
		os << resStr.c_str();
	}
	return os;
}
