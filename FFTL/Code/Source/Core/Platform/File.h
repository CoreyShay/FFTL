#pragma once

#include <stdio.h>

namespace FFTL
{


class File
{
public:

	enum OpenMode
	{
		OpenRead,// = 1 << 0,
		OpenWrite,// = 1 << 1,
//		OpenReadWrite = OpenRead | OpenWrite,
	};

	File();
#if defined(FFTL_WCHAR)
#endif
	~File();

	File(const char* pszFileName, OpenMode mode);
	bool Open(const char* pszFileName, OpenMode mode);
#if defined(FFTL_WCHAR)
	File(const wchar_t* pszFileName, OpenMode mode);
	bool Open(const wchar_t* pszFileName, OpenMode mode);
#endif
	void Close();
	int SeekAbs(int nPos);
	int SeekRel(int nPos);
	size_t Read(void* pBuffer, size_t byteCount);
	size_t Write(const void* pBuffer, size_t byteCount);
	
	template <typename T>
	size_t WriteObj(const T* pBuffer);

	size_t GetSize() const { return m_Size; }
	bool GetIsOpen() const { return m_pFile != nullptr; }
	bool GetIsEnd() const { return m_Pos >= m_Size; }

protected:
	FILE*	m_pFile = nullptr;
	size_t	m_Size = 0;
	size_t	m_Pos = 0;
};


}

#include "File.inl"