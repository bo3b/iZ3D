/*++
 *
 * Copyright (c) 2004-2006 Intel Corporation - All Rights Reserved
 * 
 * This software program is licensed subject to the BSD License, 
 * available at http://www.opensource.org/licenses/bsd-license.html
 *
 --*/

#ifndef CRC_H_INCLUDED
#define CRC_H_INCLUDED

#include <stdint.h>

#define CRC32C_INIT_REFLECTED 0xFFFFFFFF

#define MODE_BEGIN	0
#define	MODE_CONT	1
#define	MODE_END	2
#define	MODE_BODY	3

#ifdef __cplusplus
extern "C"
{
#endif	/* __cplusplus */

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
crc32c(
	uint32_t*		p_running_crc,
	const uint8_t*	p_buf,
	const uint32_t	length,
	uint8_t			mode
#ifdef __cplusplus
	= MODE_BODY
#endif
	);


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
	uint8_t			mode
#ifdef __cplusplus
	= MODE_BODY
#endif
	);

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
	uint32_t* p_running_crc,
	const uint8_t* p_buf,
	const uint32_t length,
	uint8_t mode
#ifdef __cplusplus
	= MODE_BODY
#endif
	);

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
	uint32_t* p_running_crc,
	const uint8_t* p_buf,
	const uint32_t length,
	uint8_t mode
#ifdef __cplusplus
	= MODE_BODY
#endif
	);

/*
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
					void);
#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	//CRC_H_INCLUDED

