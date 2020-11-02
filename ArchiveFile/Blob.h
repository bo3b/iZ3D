#pragma once

#include <vector>
class Blob
{
	std::vector<unsigned char> data_;
public:
	Blob();
	~Blob();
	bool Initialize( const TCHAR* pResFile );
	const void* GetData() const;
	const size_t GetSize() const;
};
