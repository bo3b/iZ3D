#pragma once
#include <d3d10umddi.h>
#include <vector>

namespace shader_analyzer{
	class ShaderOperand
	{
	public:
		enum EComponentState
		{
			X_STATE = 1,
			Y_STATE = 2,
			Z_STATE = 4,
			W_STATE = 8,
			ALL_COMPONENTS = X_STATE | Y_STATE | Z_STATE | W_STATE
		};

		class ShaderRegister
		{
		public:
			ShaderRegister() 
				: commandIndex(0), operandIndex(0), mask(ALL_COMPONENTS) {}
			ShaderRegister(unsigned commandIndex_, unsigned operandIndex_, ShaderOperand::EComponentState components_)
				: commandIndex(commandIndex_), operandIndex(operandIndex_), mask(components_) { }
			unsigned commandIndex;
			unsigned operandIndex;
			EComponentState mask;
		};

		union T64Index{
			UINT val[2];
			UINT64 value;
		};

		ShaderOperand(unsigned* data);	//
		ShaderOperand(const shader_analyzer::ShaderOperand& ShaderOperand);
		ShaderOperand();
		inline unsigned getLength() const;	// gets length in dwords
		inline D3D10_SB_OPERAND_NUM_COMPONENTS getOperandNumComponents() const;

		inline D3D10_SB_OPERAND_TYPE getOperandType() const;
		inline void setOperandType( D3D10_SB_OPERAND_TYPE opType );

		inline D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE getOperand4ComponentSelectionMode() const;
		inline D3D10_SB_OPERAND_INDEX_DIMENSION getOperandIndexDimension() const;
		inline bool isOperandExtended() const;
		inline D3D10_SB_EXTENDED_OPERAND_TYPE getExtendedOperandType() const;
		inline D3D10_SB_OPERAND_MODIFIER getOperandModifier() const;
		inline unsigned getOperand4ComponentMask() const;
		inline unsigned getOperand4ComponentSwizzle() const;
		inline D3D10_SB_4_COMPONENT_NAME getOperand4ComponentSwizzleSource( D3D10_SB_4_COMPONENT_NAME destComp ) const;
		inline D3D10_SB_4_COMPONENT_NAME getOperand4ComponentSelect1() const;
		inline D3D10_SB_OPERAND_INDEX_REPRESENTATION
			getOperandIndexRepresentation( D3D10_SB_OPERAND_INDEX_DIMENSION dim ) const;

		inline unsigned getIndex( const unsigned i ) const;
		bool setImmediate32IndexIfItIsTheOne( const unsigned ind );
		
		const unsigned getImmediateValue() const;
		std::string toString() const;
		std::string getName() const;
		std::string getIndexString() const;
		inline unsigned getComponentState(  unsigned outCS = ALL_COMPONENTS ) const;
		inline unsigned getComponentStateCount() const;
		inline unsigned getComponentFormGiven( const unsigned inComponent ) const;
		inline unsigned GetSwizzleComponentState( D3D10_SB_4_COMPONENT_NAME sbName ) const;
	private:

		unsigned* m_pData;
		unsigned m_length;
		std::vector<ShaderOperand> m_indexOperands;
		T64Index m_indexes[3];
		friend std::ostream& operator << ( std::ostream& os,
			const shader_analyzer::ShaderOperand& sdOperand );
		friend bool operator == ( const shader_analyzer::ShaderOperand& rhw,
			const shader_analyzer::ShaderOperand& lhw );
	};
}

#include "ShaderOperand.inl"