/*

Original author:
Corey Shay
corey@signalflowtechnologies.com

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#include "../Utils/S24.h"

namespace FFTL
{


void DspConvertPcm(f32* pOutBuffer, const  u8* pInBuffer, size_t totalSampleCount);
void DspConvertPcm(f32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
void DspConvertPcm(f32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
void DspConvertPcm(f32* pOutBuffer, const s32* pInBuffer, size_t totalSampleCount);
void DspConvertPcm(f32* pOutBuffer, const s64* pInBuffer, size_t totalSampleCount);
void DspConvertPcm(f32* pOutBuffer, const f64* pInBuffer, size_t totalSampleCount);

void DspConvertPcm( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
void DspConvertPcm(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);
void DspConvertPcm(s32* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount);

void DspConvertPcm( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
void DspConvertPcm(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);
void DspConvertPcm(s16* pOutBuffer, const  u8* pInBuffer, size_t totalSampleCount);

u32 DspConvertPcmDither( u8* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
u32 DspConvertPcmDither(s16* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
u32 DspConvertPcmDither( u8* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);
u32 DspConvertPcmDither(s16* pOutBuffer, const f32* pInBuffer, size_t totalSampleCount, u32 ditherSeed);

void IntegerConvert(s32* pOutBuffer, const  u8* pInBuffer, size_t totalSampleCount); // Converts from unsigned offset
void IntegerConvert(s32* pOutBuffer, const s16* pInBuffer, size_t totalSampleCount);
void IntegerConvert(s32* pOutBuffer, const s24* pInBuffer, size_t totalSampleCount);

void DspDeInterleave2(f32* pOutBuffer0, f32* pOutBuffer1, const f32* pInBuffer, size_t totalSampleCount);
void DspDeInterleave4(f32* pOutBuffer0, f32* pOutBuffer1, f32* pOutBuffer2, f32* pOutBuffer3, const f32* pInBuffer, size_t totalSampleCount);


}


#include "DspPcmConvert.inl"
