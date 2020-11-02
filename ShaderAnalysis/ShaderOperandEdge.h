#pragma once

#include "ShaderOperand.h"
namespace shader_analyzer
{
	class ShaderOperandEdge
	{
		const ShaderOperand* operand_;
		unsigned volatile componentState_;
	public:
		inline ShaderOperandEdge();
		explicit inline ShaderOperandEdge( const shader_analyzer::ShaderOperand* pOperand );
		inline ShaderOperandEdge( const ShaderOperandEdge& opEnd);
		inline unsigned getComponentState() const;
		inline void setComponentState( const unsigned state );
		inline bool isContains( shader_analyzer::ShaderOperand::EComponentState state ) const;
		inline std::string toString() const;
		inline const ShaderOperand* getOperand() const;
		inline ShaderOperandEdge& operator = ( const ShaderOperandEdge& b);
		friend bool operator < ( const ShaderOperandEdge& a, const ShaderOperandEdge& b )
		{
			return a.operand_ < b.operand_;
		}
		friend bool operator == ( const ShaderOperandEdge& a, const ShaderOperandEdge& b )
		{
			return a.operand_ == b.operand_ && a.componentState_ == b.componentState_;
		}
	};
}

#include "ShaderOperandEdge.inl"