/*
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2008-2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#ifndef __ARCH_CONFIG_H__
#define __ARCH_CONFIG_H__

#define ARCH_UMP_BACKEND_DEFAULT          1

//Start from 960MB
#define ARCH_UMP_MEMORY_ADDRESS_DEFAULT   0x3C000000 

//Start from 192MB
//#define ARCH_UMP_MEMORY_ADDRESS_DEFAULT   0x0C000000 

#define ARCH_UMP_MEMORY_SIZE_DEFAULT 48UL * 1024UL * 1024UL

#endif /* __ARCH_CONFIG_H__ */
