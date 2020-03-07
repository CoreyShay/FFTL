#pragma once


#include "../defs.h"
#include "../Utils/Casts.h"
#include <random>



namespace FFTL
{

class FFTL_NODISCARD RandomNumberGenerator
{
public:
	static void Init();
	static inline u32 Rand();
	static inline u32 Rand(u32 nRange);
	static inline s32 Rand(s32 nMin, s32 nMax);
private:
	static std::minstd_rand m_v;
};

inline u32 RandomNumberGenerator::Rand()
{
	return m_v();
}
inline u32 RandomNumberGenerator::Rand(u32 nRange)
{
	const u32 nRand = nRange != 0 ? m_v() % nRange : 0;
	return nRand;
}
inline s32 RandomNumberGenerator::Rand(s32 nMin, s32 nMax)
{
	const u32 nDiff = safestatic_cast<u32>(nMax - nMin);
	const u32 nRand = nDiff != 0 ? m_v() % nDiff : 0;
	return safestatic_cast<s32>(nRand) - nMin;
}



inline u32 Random32()
{
	return RandomNumberGenerator::Rand();
}
inline u32 Random32(u32 nRange)
{
	return RandomNumberGenerator::Rand(nRange);
}
inline s32 Random32(s32 nMin, s32 nMax)
{
	return RandomNumberGenerator::Rand(nMin, nMax);
}



}



