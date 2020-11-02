#pragma once
#include <vector>
#include <WTypes.h>
#include <map>
namespace iz3d
{
	namespace shader
	{
		class ShaderProcessor
		{
		public:
			typedef std::vector<DWORD> TBinData;
			ShaderProcessor();
			~ShaderProcessor();

			void CompileInstruction( const char* pStr,
				TBinData& retData );

		private:
			typedef std::map< std::string, DWORD > TOperationBinaryElement;
			typedef std::map< DWORD, size_t > TOperationNumOperandsElement;
			static TOperationBinaryElement m_sOperationHandle;
			static TOperationNumOperandsElement m_sOperationOperandsNumber;
			static TOperationBinaryElement m_sOperandTypes;
			bool parseOperation( const char* pStr, TBinData& operationCode );
			bool parseOperand( const char* pStr, TBinData& operandCode, bool isFirst );
			DWORD defineOperandType( const char* pStr );
			bool defineOperandIndices( const char* pStr, TBinData& indeces );
			bool defineComponents( const char* pStr, TBinData& components );
			DWORD getBinDataForComponents( const TBinData& components, bool isFirst );
			DWORD getBinDataForIndices( const TBinData& indices );
		};
	}
}