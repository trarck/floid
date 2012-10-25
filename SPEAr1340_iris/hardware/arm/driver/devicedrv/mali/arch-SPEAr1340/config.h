/*
 * Copyright (C) 2010-2012 STMicroelectronics. All rights reserved.
 * 
 * This program is free software and is provided to you
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation, and any use by you of this program is subject
 * to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __ARCH_CONFIG_H__
#define __ARCH_CONFIG_H__

/* Note: IRQ auto detection (setting irq to -1) only works if the IRQ is not shared with any other hardware resource */
/* Modified for SPEAr1340 */
static _mali_osk_resource_t arch_configuration [] =
{
	{
		.type = MALIGP2,
		.description = "MALI GP2",
		.base = 0xD0902000,
		.irq = -1, /*109*/
		.mmu_id = 1
	},
#if USING_MMU
	{
		.type = MMU,
		.base = 0xD0903000,
		.irq = -1, /*108*/
		.description = "Mali MMU",
		.mmu_id = 1
	},
#endif /* USING_MMU */
	{
		.type = MALI200,
		.base = 0xD0900000,
		.irq = -1 /*110*/,
		.description = "Mali 200 (GX525)",
		.mmu_id = 1
	},
	{
		.type = OS_MEMORY,
		.description = "OS Memory",
		.alloc_order = 10, /* Lowest preference for this memory */
		.size = 393 * 1024 * 1024,
		.flags = _MALI_CPU_WRITEABLE | _MALI_CPU_READABLE | _MALI_PP_READABLE | _MALI_PP_WRITEABLE |_MALI_GP_READABLE | _MALI_GP_WRITEABLE
	},

#if ! ONLY_ZBT
#if USING_OS_MEMORY
#else /* USING_OS_MEMORY */
	{	/* SPEAr Memory */
		.type = MEMORY,
		.description = "Mali SDRAM remapped to baseboard",
		.cpu_usage_adjust = 0x0,
		.alloc_order = 0, /* Highest preference for this memory */
#if MALI_USE_UNIFIED_MEMORY_PROVIDER != 0
		.base = 0x1F000000, /* Start from 1008MB */
		.size = 0x01000000,
#endif /* MALI_USE_UNIFIED_MEMORY_PROVIDER != 0 */
		.flags = _MALI_CPU_WRITEABLE | _MALI_CPU_READABLE | _MALI_PP_READABLE | _MALI_PP_WRITEABLE |_MALI_GP_READABLE | _MALI_GP_WRITEABLE
	},
#endif /** USING_OS_MEMORY */
#endif /* ! ONLY_ZBT */

	{
		.type = MEM_VALIDATION,
		.description = "Framebuffer",
		.base = 0x00000000,
		.size = 0x20000000, /* 1GB */
		.flags = _MALI_CPU_WRITEABLE | _MALI_CPU_READABLE | _MALI_PP_WRITEABLE | _MALI_PP_READABLE
	},
};

#endif /* __ARCH_CONFIG_H__ */
