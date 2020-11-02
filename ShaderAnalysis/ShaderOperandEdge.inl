shader_analyzer::ShaderOperandEdge::ShaderOperandEdge( const ShaderOperand* pOperand ):
operand_( pOperand ), componentState_( 0 )
{
}

shader_analyzer::ShaderOperandEdge::ShaderOperandEdge():
operand_( 0 ), componentState_( 0 )
{
}

unsigned shader_analyzer::ShaderOperandEdge::getComponentState() const
{
	return componentState_;
}

bool shader_analyzer::ShaderOperandEdge::isContains(
	shader_analyzer::ShaderOperand::EComponentState state ) const
{
	return (componentState_ & state) != 0;
}

void shader_analyzer::ShaderOperandEdge::setComponentState( const unsigned state )
{
	componentState_ = state;
}

std::string shader_analyzer::ShaderOperandEdge::toString() const
{
	std::string retStr;
	if( operand_)
	{
		retStr += operand_->getName();
		retStr += operand_->getIndexString();
		retStr += ".";
	}
	if( isContains( shader_analyzer::ShaderOperand::X_STATE ) )
		retStr += "x";
	if( isContains( shader_analyzer::ShaderOperand::Y_STATE ) )
		retStr += "y";
	if( isContains( shader_analyzer::ShaderOperand::Z_STATE ) )
		retStr += "z";
	if( isContains( shader_analyzer::ShaderOperand::W_STATE ) )
		retStr += "w";
	return retStr;
}

shader_analyzer::ShaderOperandEdge::ShaderOperandEdge( const ShaderOperandEdge& opEnd )
: operand_( opEnd.operand_ ), componentState_( opEnd.componentState_ )
{

}

const shader_analyzer::ShaderOperand* shader_analyzer::ShaderOperandEdge::getOperand() const
{
	return operand_;
}

shader_analyzer::ShaderOperandEdge& 
shader_analyzer::ShaderOperandEdge::operator = ( const shader_analyzer::ShaderOperandEdge& b )
{
	operand_ = b.operand_;
	componentState_ = b.componentState_;
	return *this;
}
