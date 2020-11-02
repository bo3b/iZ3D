/*++
 *
 * Copyright (c) 2004-2006 Intel Corporation - All Rights Reserved
 *
 * This software program is licensed subject to the BSD License, 
 * available at http://www.opensource.org/licenses/bsd-license.html
 *
 * Abstract: The main routine
 * 
 --*/

#include <stdafx.h>
#include <Windows.h>
#include <stdio.h>
#include <intrin.h>
#include <crtdbg.h>
#include "crc.h"

#pragma warning( disable: 4267 )
//to avoid unnecessary warnings when using "touch" variables

/* 
 * the following variables are used for counting cycles and bytes 
 */

extern uint32_t crc_tableil8_o32[256];
extern uint32_t crc_tableil8_o40[256];
extern uint32_t crc_tableil8_o48[256];
extern uint32_t crc_tableil8_o56[256];
extern uint32_t crc_tableil8_o64[256];
extern uint32_t crc_tableil8_o72[256];
extern uint32_t crc_tableil8_o80[256];
extern uint32_t crc_tableil8_o88[256];

#define XOROT 0xFFFFFFFF

/**
 *
 * Routine Description:
 *
 * Computes the CRC32c checksum for the specified buffer.                      
 *
 * Arguments:
 *
 *		p_running_crc - pointer to the initial or final remainder value 
 *						used in CRC computations. It should be set to 
 *						non-NULL if the mode argument is equal to CONT or END
 *		p_buf - the packet buffer where crc computations are being performed
 *		length - the length of p_buf in bytes
 *		init_bytes - the number of initial bytes that need to be processed before
 *					 aligning p_buf to multiples of 4 bytes
 *		mode - can be any of the following: BEGIN, CONT, END, BODY 
 *
 * Return value:
 *		
 *		The computed CRC32c value
 */
uint32_t
crc32c(
	uint32_t*		p_running_crc,
	const uint8_t*	p_buf,
	const uint32_t	length,
	uint8_t			mode) 
{
	uint32_t crc;
	const uint8_t* p_end = p_buf + length;
	if((mode == MODE_CONT) || (mode == MODE_END))
		crc = *p_running_crc;
	else	
		crc = CRC32C_INIT_REFLECTED;
	while(p_buf < p_end )
		crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
	if((mode == MODE_BEGIN) || (mode == MODE_CONT))
		return crc;		
	return crc ^ XOROT;
}



/**
 *
 * Routine Description:
 *
 * Computes the CRC32c checksum for the specified buffer using the slicing by 8 
 * algorithm over 64 bit quantities.                      
 *
 * Arguments:
 *
 *		p_running_crc - pointer to the initial or final remainder value 
 *						used in CRC computations. It should be set to 
 *						non-NULL if the mode argument is equal to CONT or END
 *		p_buf - the packet buffer where crc computations are being performed
 *		length - the length of p_buf in bytes
 *		init_bytes - the number of initial bytes that need to be processed before
 *					 aligning p_buf to multiples of 4 bytes
 *		mode - can be any of the following: BEGIN, CONT, END, BODY 
 *
 * Return value:
 *		
 *		The computed CRC32c value
 */

uint32_t
crc32c_sb8_64_bit(
	uint32_t* p_running_crc,
	const uint8_t*	p_buf,
	const uint32_t length,
	const uint32_t init_bytes,
	uint8_t			mode)
{
	uint32_t li;
	uint32_t crc, term1, term2;
	uint32_t running_length;
	uint32_t end_bytes;
	if((mode == MODE_CONT) || (mode == MODE_END))
		crc = *p_running_crc;
	else	
		crc = CRC32C_INIT_REFLECTED;
	running_length = ((length - init_bytes)/8)*8;
	end_bytes = length - init_bytes - running_length; 

	for(li=0; li < init_bytes; li++) 
		crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);	
	for(li=0; li < running_length/8; li++) 
	{
		crc ^= *(uint32_t *)p_buf;
		p_buf += 4;
		term1 = crc_tableil8_o88[crc & 0x000000FF] ^
				crc_tableil8_o80[(crc >> 8) & 0x000000FF];
		term2 = crc >> 16;
		crc = term1 ^
			  crc_tableil8_o72[term2 & 0x000000FF] ^ 
			  crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
		term1 = crc_tableil8_o56[(*(uint32_t *)p_buf) & 0x000000FF] ^
				crc_tableil8_o48[((*(uint32_t *)p_buf) >> 8) & 0x000000FF];
		
		term2 = (*(uint32_t *)p_buf) >> 16;
		crc =	crc ^ 
				term1 ^		
				crc_tableil8_o40[term2  & 0x000000FF] ^	
				crc_tableil8_o32[(term2 >> 8) & 0x000000FF];	
		p_buf += 4;
	}
	for(li=0; li < end_bytes; li++) 
		crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
	if((mode == MODE_BEGIN) || (mode ==  MODE_CONT))
		return crc;
	return crc ^ XOROT;	
}



/**
 *
 * Routine Description:
 *
 * Computes the CRC32c checksum for the specified buffer.                      
 *
 * Arguments:
 *
 *		p_running_crc - pointer to the initial or final remainder value 
 *						used in CRC computations. It should be set to 
 *						non-NULL if the mode argument is equal to CONT or END
 *		p_buf - the packet buffer where crc computations are being performed
 *		length - the length of p_buf in bytes
 *		mode - can be any of the following: BEGIN, CONT, END, BODY 
 *
 * Return value:
 *		
 *		The computed CRC32c value
 */

uint32_t
	crc32c_sse4(
	uint32_t*		p_running_crc,
	const uint8_t*	p_buf,
	uint32_t		length,
	uint8_t			mode) 
{
	uint32_t crc;
	size_t align;
	size_t ndwords;
	if((mode == MODE_CONT) || (mode == MODE_END))
		crc = *p_running_crc;
	else	
		crc = CRC32C_INIT_REFLECTED;

	// Align to DWORD boundary
	align = (sizeof(DWORD) - ((INT_PTR)p_buf)) & (sizeof(DWORD) - 1);
	align = min(align, length);
	_ASSERT(align == 0);

#ifdef WIN64
	ndwords = length / sizeof(DWORD64);
	__int64 crc64 = crc;
	for (; ndwords; ndwords--) {
		crc64 = _mm_crc32_u64(crc64, *(DWORD64*)p_buf);
		p_buf += sizeof(DWORD64);
	}
	length &= sizeof(DWORD64) - 1;
	crc = (uint32_t)crc64;
#else
	ndwords = length / sizeof(DWORD);
	for (; ndwords; ndwords--) {
		crc = _mm_crc32_u32(crc, *(DWORD*)p_buf);
		p_buf += sizeof(DWORD);
	}
	length &= sizeof(DWORD) - 1;
#endif
	for (; length; length--)
		crc = _mm_crc32_u8(crc, *p_buf++);

	if((mode == MODE_BEGIN) || (mode == MODE_CONT))
		return crc;
	return crc ^ XOROT;
}



/**
 *
 * Routine Description:
 *
 * Best realization auto detection.                      
 *
 * Arguments:
 *
 *		p_running_crc - pointer to the initial or final remainder value 
 *						used in CRC computations. It should be set to 
 *						non-NULL if the mode argument is equal to CONT or END
 *		p_buf - the packet buffer where crc computations are being performed
 *		length - the length of p_buf in bytes
 *		mode - can be any of the following: BEGIN, CONT, END, BODY 
 *
 * Return value:
 *		
 *		The computed CRC32c value
 */

uint32_t
	crc32c_auto(
	uint32_t*		p_running_crc,
	const uint8_t*	p_buf,
	uint32_t		length,
	uint8_t			mode) 
{
	static int SSE42Supported = 0;
	if (SSE42Supported == 0)
	{
		int CPUInfo[4] = {-1};
		__cpuid(CPUInfo, 1);
		SSE42Supported = ((CPUInfo[2] & 0x100000) || false) ? 1 : 2;
	}
	if (SSE42Supported == 1)
		return crc32c_sse4(p_running_crc, p_buf, length, mode);
	else
		return crc32c_sb8_64_bit(p_running_crc, p_buf, length, 0, mode);
}

#define MPA_FRAME_LENGTH		48
#define MPA_FRAME_INDEX1		5
#define MPA_FRAME_VALUE1		0x2a
#define MPA_FRAME_INDEX2		6
#define MPA_FRAME_VALUE2		0x40
#define MPA_FRAME_INDEX3		7
#define MPA_FRAME_VALUE3		0x03
#define MPA_FRAME_INDEX4		19
#define MPA_FRAME_VALUE4		0x01
#define MPA_FRAME_CRC			0x84B3864C

/**
 *
 * Routine Description:
 *
 * performs the mpa sample frame test                    
 *
 * Arguments:
 *
 *		non
 *
 * Return value:
 *		
 *		none
 */
void
	mpa_sample_frame_test(
	void)
{
	uint8_t	mpa[MPA_FRAME_LENGTH];
	uint32_t result;
	int i;
	for(i = 0; i < MPA_FRAME_LENGTH; i++ )
		mpa[i] = 0;
	mpa[MPA_FRAME_INDEX1] = MPA_FRAME_VALUE1;
	mpa[MPA_FRAME_INDEX2] = MPA_FRAME_VALUE2;
	mpa[MPA_FRAME_INDEX3] = MPA_FRAME_VALUE3;
	mpa[MPA_FRAME_INDEX4] = MPA_FRAME_VALUE4;
	printf("\nVerifying algorithms against MPA sample frame\n\n"); 

	printf("testing the Sarwate algorithm..............................");
	result = crc32c(NULL, mpa, MPA_FRAME_LENGTH, MODE_BODY);
	if( result != MPA_FRAME_CRC)
	{
		printf( "error\n" );
		exit(0);
	}
	printf( "passed\n" );
	printf("testing the slicing by 8 over 64 bit algorithm.............");
	result = crc32c_sb8_64_bit(NULL, mpa, MPA_FRAME_LENGTH, 0, MODE_BODY);
	if( result != MPA_FRAME_CRC)
	{
		printf( "error\n" );
		exit(0);
	}
	printf( "passed\n" );
	printf("testing sse 4 algorithm.............");
	result = crc32c_sse4(NULL, (const BYTE *)mpa, MPA_FRAME_LENGTH, MODE_BODY);
	if( result != MPA_FRAME_CRC)
	{
		printf( "error\n" );
		exit(0);
	}
	printf( "passed\n" );
	return;
}
