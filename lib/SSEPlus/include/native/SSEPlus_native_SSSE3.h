//
// Copyright (c) 2006-2008 Advanced Micro Devices, Inc. All Rights Reserved.
// This software is subject to the Apache v2.0 License.
//
#ifndef __SSEPLUS_NATIVE_SSSE3_H__
#define __SSEPLUS_NATIVE_SSSE3_H__

#include "../SSEPlus_base.h"
#include <tmmintrin.h> // SSSE3


/** @addtogroup native_SSSE3   
 *  @{ 
 *  @name Native SSSE3 Operations
 */

/** \SSSE3{Native,_mm_abs_epi16} */
SSP_FORCEINLINE __m128i ssp_abs_epi16_SSSE3 (__m128i a)
{
    return _mm_abs_epi16( a );
}
/** \SSSE3{Native,_mm_abs_epi32} */
SSP_FORCEINLINE __m128i ssp_abs_epi32_SSSE3 (__m128i a)
{
    return _mm_abs_epi32(a);
}
/** \SSSE3{Native,_mm_abs_epi8} */
SSP_FORCEINLINE __m128i ssp_abs_epi8_SSSE3 (__m128i a)
{
    return _mm_abs_epi8(a);
}
/** \SSSE3{Native,_mm_abs_pi16} */
SSP_FORCEINLINE __m64 ssp_abs_pi16_SSSE3 (__m64 a)
{
    return _mm_abs_pi16(a);
}
/** \SSSE3{Native,_mm_abs_pi32} */
SSP_FORCEINLINE __m64 ssp_abs_pi32_SSSE3 (__m64 a)
{
    return _mm_abs_pi32(a);
}
/** \SSSE3{Native,_mm_abs_pi8} */
SSP_FORCEINLINE __m64 ssp_abs_pi8_SSSE3 (__m64 a)
{
    return _mm_abs_pi8(a);
}
/** \SSSE3{Native,_mm_alignr_epi8} */
SSP_FORCEINLINE __m128i ssp_alignr_epi8_SSSE3 (__m128i a, __m128i b, int n)
{
    n = (n>=32) ?  32 : n;
    switch( n )
    {
        CASE_32( _mm_alignr_epi8, a, b );
    }
}
/** \SSSE3{Native,_mm_alignr_pi8} */
SSP_FORCEINLINE __m64 ssp_alignr_pi8_SSSE3 (__m64 a, __m64 b, int n)
{
    n = (n>=16) ?  16 : n;
    switch( n )
    {
        CASE_16( _mm_alignr_pi8, a, b );
    }
}
/** \SSSE3{Native,_mm_hadd_epi16} */
SSP_FORCEINLINE __m128i ssp_hadd_epi16_SSSE3 (__m128i a, __m128i b)
{
    return _mm_hadd_epi16(a, b);
}
/** \SSSE3{Native,_mm_hadd_epi32} */
SSP_FORCEINLINE __m128i ssp_hadd_epi32_SSSE3 (__m128i a, __m128i b)
{
    return _mm_hadd_epi32(a, b);
}
/** \SSSE3{Native,_mm_hadd_pi16} */
SSP_FORCEINLINE __m64 ssp_hadd_pi16_SSSE3 (__m64 a, __m64 b)
{
    return _mm_hadd_pi16(a, b);
}
/** \SSSE3{Native,_mm_hadd_pi32} */
SSP_FORCEINLINE __m64 ssp_hadd_pi32_SSSE3 (__m64 a, __m64 b)
{
    return _mm_hadd_pi32(a, b);
}
/** \SSSE3{Native,_mm_hadds_epi16} */
SSP_FORCEINLINE __m128i ssp_hadds_epi16_SSSE3 (__m128i a, __m128i b)
{
    return _mm_hadds_epi16(a, b);
}
/** \SSSE3{Native,_mm_hadds_pi16} */
SSP_FORCEINLINE __m64 ssp_hadds_pi16_SSSE3 (__m64 a, __m64 b)
{
    return _mm_hadds_pi16(a, b);
}
/** \SSSE3{Native,_mm_hsub_epi16} */
SSP_FORCEINLINE __m128i ssp_hsub_epi16_SSSE3 (__m128i a, __m128i b)
{
    return _mm_hsub_epi16(a, b);
}
/** \SSSE3{Native,_mm_hsub_epi32} */
SSP_FORCEINLINE __m128i ssp_hsub_epi32_SSSE3 (__m128i a, __m128i b)
{
    return _mm_hsub_epi32(a, b);
}
/** \SSSE3{Native,_mm_hsub_pi16} */
SSP_FORCEINLINE __m64 ssp_hsub_pi16_SSSE3 (__m64 a, __m64 b)
{
    return _mm_hsub_pi16(a, b);
}
/** \SSSE3{Native,_mm_hsub_pi32} */
SSP_FORCEINLINE __m64 ssp_hsub_pi32_SSSE3 (__m64 a, __m64 b)
{
    return _mm_hsub_pi32(a, b);
}
/** \SSSE3{Native,_mm_hsubs_epi16} */
SSP_FORCEINLINE __m128i ssp_hsubs_epi16_SSSE3 (__m128i a, __m128i b)
{
    return _mm_hsubs_epi16(a, b);
}
/** \SSSE3{Native,_mm_hsubs_pi16} */
SSP_FORCEINLINE __m64 ssp_hsubs_pi16_SSSE3 (__m64 a, __m64 b)
{
    return _mm_hsubs_pi16(a, b);
}
/** \SSSE3{Native,_mm_maddubs_epi16} */
SSP_FORCEINLINE __m128i ssp_maddubs_epi16_SSSE3 (__m128i a, __m128i b)
{
    return _mm_maddubs_epi16(a, b);
}
/** \SSSE3{Native,_mm_maddubs_pi16} */
SSP_FORCEINLINE __m64 ssp_maddubs_pi16_SSSE3 (__m64 a, __m64 b)
{
    return _mm_maddubs_pi16(a, b);
}
/** \SSSE3{Native,_mm_mulhrs_epi16} */
SSP_FORCEINLINE __m128i ssp_mulhrs_epi16_SSSE3 (__m128i a, __m128i b)
{
    return _mm_mulhrs_epi16(a, b);
}
/** \SSSE3{Native,_mm_mulhrs_pi16} */
SSP_FORCEINLINE __m64 ssp_mulhrs_pi16_SSSE3 (__m64 a, __m64 b)
{
    return _mm_mulhrs_pi16(a, b);
}
/** \SSSE3{Native,_mm_shuffle_epi8} */
SSP_FORCEINLINE __m128i ssp_shuffle_epi8_SSSE3 (__m128i a, __m128i b)
{
    return _mm_shuffle_epi8(a, b);
}
/** \SSSE3{Native,_mm_shuffle_pi8} */
SSP_FORCEINLINE __m64 ssp_shuffle_pi8_SSSE3 (__m64 a, __m64 b)
{
    return _mm_shuffle_pi8(a, b);
}
/** \SSSE3{Native,_mm_sign_epi16} */
SSP_FORCEINLINE __m128i ssp_sign_epi16_SSSE3 (__m128i a, __m128i b)
{
    return _mm_sign_epi16(a, b);
}
/** \SSSE3{Native,_mm_sign_epi32} */
SSP_FORCEINLINE __m128i ssp_sign_epi32_SSSE3 (__m128i a, __m128i b)
{
    return _mm_sign_epi32(a, b);
}
/** \SSSE3{Native,_mm_sign_epi8} */
SSP_FORCEINLINE __m128i ssp_sign_epi8_SSSE3 (__m128i a, __m128i b)
{
    return _mm_sign_epi8(a, b);
}
/** \SSSE3{Native,_mm_sign_pi16} */
SSP_FORCEINLINE __m64 ssp_sign_pi16_SSSE3 (__m64 a, __m64 b)
{
    return _mm_sign_pi16(a, b);
}
/** \SSSE3{Native,_mm_sign_pi32} */
SSP_FORCEINLINE __m64 ssp_sign_pi32_SSSE3 (__m64 a, __m64 b)
{
    return _mm_sign_pi32(a, b);
}
/** \SSSE3{Native,_mm_sign_pi8} */
SSP_FORCEINLINE __m64 ssp_sign_pi8_SSSE3 (__m64 a, __m64 b)
{
    return _mm_sign_pi8(a, b);
}

/** @} 
 *  @}
 */

#endif // __SSEPLUS_NATIVE_SSSE3_H__
