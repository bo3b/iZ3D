#pragma once

#include <xstring>
#include <vector>
namespace iz3d {
	namespace UI{
		class InformTable
		{
			typedef const std::wstring TItem;
			typedef std::vector< TItem > TRow;
			typedef std::vector< TRow > TTable;
			TTable m_Table;
			const wchar_t* GetWizardHotKeyCaution();
		public:
			void ReadTextMessages();
			size_t GetLinesTable();
			size_t GetColumnsTable();
			const wchar_t* GetItem( size_t nLineNum, size_t nColumnTable );
			InformTable();
			~InformTable();
		};
	}
}