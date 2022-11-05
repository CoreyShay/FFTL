#pragma once

#include "../Utils/MetaProgramming.h"
#include <cerrno>

namespace FFTL
{



inline File::File(const char* pszFileName, OpenMode mode)
	: File()
{
	Open(pszFileName, mode);
}

#if defined(FFTL_WCHAR)
inline File::File(const wchar_t* pszFileName, OpenMode mode)
	: File()
{
	Open(pszFileName, mode);
}
#endif

inline File::~File()
{
	Close();
}

inline bool File::Open(const char* pszFileName, OpenMode mode)
{
	if (m_pFile != nullptr)
		return false;

#if defined(_MSC_VER)
	const auto result = fopen_s(&m_pFile, pszFileName, mode == OpenRead ? "rb" : "wb");
	if (result != 0)
	{
#if defined(FFTL_ENABLE_ASSERT)
		char szErr[128];
		strerror_s(szErr, errno);
		FFTL_LOG_ERR("File::Open(\"%s\") failed with \"%s\"", pszFileName, szErr);
#endif
		return false;
	}
#else
	m_pFile = fopen(pszFileName, mode == OpenRead ? "rb" : "wb");
	if (m_pFile == nullptr)
	{
		const auto szErr = strerror(errno);
		FFTL_LOG_ERR("File::Open(\"%s\") failed with \"%s\"", pszFileName, szErr);
		return false;
	}
#endif

	if (mode == OpenRead)
	{
		FFTL_VERIFY_EQ(0, fseek(m_pFile, 0, SEEK_END));
		m_Size = safestatic_cast<size_t>(ftell(m_pFile));
		fseek(m_pFile, 0, SEEK_SET);
	}

	return true;
}

#if defined(FFTL_WCHAR)
inline bool File::Open(const wchar_t* pszFileName, OpenMode mode)
{
	if (m_pFile != nullptr)
		return false;

	if (pszFileName == nullptr)
		return false;

#if defined(_MSC_VER)
	const auto result = _wfopen_s(&m_pFile, pszFileName, mode == OpenRead ? L"rb" : L"wb");
	if (result != 0)
		return false;
#else
	size_t len = 0;
	for (const wchar_t* p = pszFileName; *p; ++p)
	{
		++len;
	}

	if (len == 0)
		return false;

	char* pszASCII = static_cast<char*>(alloca(len+1));
	ToASCII(pszASCII, pszFileName, len+1);

	m_pFile = fopen(pszASCII, mode == OpenRead ? "rb" : "wb");
	if (m_pFile == nullptr)
		return false;
#endif

	if (mode == OpenRead)
	{
		FFTL_VERIFY_EQ(0, fseek(m_pFile, 0, SEEK_END));
		m_Size = safestatic_cast<size_t>(ftell(m_pFile));
		fseek(m_pFile, 0, SEEK_SET);
	}

	return true;
}
#endif

inline void File::Close()
{
	if (m_pFile != nullptr)
	{
		fclose(m_pFile);
		m_pFile = nullptr;
		m_Size = 0;
		m_Pos = 0;
	}
}

inline int File::SeekAbs(int nPos)
{
	const int seekCount = fseek(m_pFile, nPos, SEEK_SET);
	m_Pos = safestatic_cast<size_t>(nPos);
	return seekCount;
}

inline int File::SeekRel(int nPos)
{
	const int seekCount = fseek(m_pFile, nPos, SEEK_CUR);
	m_Pos += seekCount;
	return seekCount;
}

inline size_t File::Read(void* pBuffer, size_t byteCount)
{
#if defined(_MSC_VER)
	const size_t readCount = fread_s(pBuffer, byteCount, 1, byteCount, m_pFile);
#else
	const size_t readCount = fread(pBuffer, 1, byteCount, m_pFile);
#endif
	m_Pos += readCount;
	return readCount;
}

inline size_t File::Write(const void* pBuffer, size_t byteCount)
{
	const size_t writeCount = fwrite(pBuffer, 1, byteCount, m_pFile);
	m_Pos += writeCount;
	m_Size = Max(m_Size, m_Pos);
	return writeCount;
}

template <typename T>
inline size_t File::WriteObj(const T* pBuffer)
{
	return Write(pBuffer, sizeof(T));
}


}