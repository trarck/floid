/*
 * Copyright (C) 2010-2012 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file mali_platform.c
 * Platform specific Mali driver functions for a default platform
 */
#include "mali_kernel_common.h"
#include "mali_osk.h"
#include "mali_platform.h"
#include <asm/io.h>			/* SPEAr specific code, ioremap */

/* SPEAR specific : misc regs */
#include <asm/io.h>			/* ioremap, readl etc */
#include <mach/misc_regs.h> 
#include <linux/module.h>   /* kernel module definitions */
//BARBA: added support for user space configuration

/* The base address of the memory block for the dedicated memory backend */
extern int mali_memory_address;
module_param(mali_memory_address, int, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH); /* rw-rw-r-- */
MODULE_PARM_DESC(mali_memory_address, "The physical address to map for the dedicated memory backend");

/* The size of the memory block for the dedicated memory backend */
extern int mali_memory_size;
module_param(mali_memory_size, int, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH); /* rw-rw-r-- */
MODULE_PARM_DESC(mali_memory_size, "The size of fixed memory to map in the dedicated memory backend");


/* The base address of the memory block for the dedicated memory backend */
extern int mem_validation_base;
module_param(mem_validation_base, int, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH); /* rw-rw-r-- */
MODULE_PARM_DESC(mem_validation_base, "The base physical address for memory validation");

/* The size of the memory block for the dedicated memory backend */
extern int mem_validation_size;
module_param(mem_validation_size, int, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH); /* rw-rw-r-- */
MODULE_PARM_DESC(mem_validation_size, "The memory size for memory validation");

//BARBA end

/* SPEAr specific function - initializes clocks, enable MALI IP */
#define SPEAr1340
#define KERNEL37

void spear_mali_init(void)
{
#ifdef SPEAr1340
	//unsigned long val;
	unsigned int* reg;
	
	MALI_PRINT(("Mali SPEAr1340, configuring clock synth\n"));
	/* clock synth configuration, SSCG4corresponding register in MISC is GEN_CLK_SSCG3 (sic) */
	reg = ioremap(0xE0700304, 0x4);/**/
	/*asserting reset*/
	*reg = 0x20000;
	/*configuring freq divisor*/
	/*in PLL_CFG select Fin=FVCO1/4*/
	/*configuring To (3 MSB are integer part ), output is Fout=Fin/(2*To) */
	*reg = 0x00002800; /* binary 0.101 decimal 0.625 */
	/*releasing reset*/
	*reg &= ~(0x20000);
	iounmap(reg);
	
	MALI_PRINT(("Mali SPEAr1340, enabling clock\n"));
	reg = ioremap(0xE0700314, 0x4);/*PERIP3_CLK_ENB*/
	/*Enabling clock*/
	//val = readl(0xe0700314);
	//reg |= (0x1 << 6);
	//MALI_PRINT(("Mali testing SPEAr1340, CLOCKVALUE before: %08X\n", *reg));
	*reg = *reg | 0x40;
	//MALI_PRINT(("Mali testing SPEAr1340, CLOCKVALUE before: %08X\n", *reg));
	//writel(val, 0xe0700314);
	iounmap(reg);
	/*Reset is 0 by default, trying to set anyway*/
	reg = ioremap(0xE0700320, 0x4);/*PERIP3_SW_RST*/
	*reg &= ~(0x1 << 6);
	
	
	
	MALI_PRINT(("Mali SPEAr1340, clock enabled\n"));
#else
	/* Misc registers : PLL_CFG */
	unsigned long val;
#ifdef KERNEL37
	val = readl(VA_PLL_CFG);
	val &= (~(0x3 << RAS_SYNT0_1_CLK_SHIFT));	/* VA_PLL_CFG.rasSynt01 = 00 VCO1div4 */
	writel(val, VA_PLL_CFG);
#else
	val = readl(PLL_CFG);
	val &= (~(0x3 << RAS_SYNT0_1_CLK_SHIFT));	/* PLL_CFG.rasSynt01 = 00 VCO1div4 */
	writel(val, PLL_CFG);
#endif
	/* RAS_SYNTH0.To = (fin/(2*fout))*2^14 = (500MHz(VCO1div4)/(2*204MHz))*2^14 */
	/* 204MHz = 0x4e6e */
	/* 250MHz = 0x4000 */
#ifdef MALI250
	val = 0x4000;	/* RAS_SYNTH0.To */
#else
	//val = 0x4e6e;	/* RAS_SYNTH0.To */
	val = 0x2737;	/* RAS_SYNTH0.To */ /* Ukmar: set MALI core frequency to 204MHz with VCOfreq=1GHz */
#endif

#ifdef KERNEL37
	writel(val, VA_RAS_CLK_SYNT0);

	val = readl(VA_RAS_CLK_ENB);
	val = val | (1 | (1 << SYNT0_CLK_ENB));	/* RAS_CLK_ENB.synt0_clken clock enable and RAS_ACLK enable */
	writel(val, VA_RAS_CLK_ENB);

	val = readl(VA_RAS_SW_RST);
	val = val & ~(1 | (1 << SYNT0_CLK_ENB));	/* RAS_SW_RST.synt0_swrst disable sw reset */
	writel(val, VA_RAS_SW_RST);
#else
	writel(val, RAS_CLK_SYNT0);

	val = readl(RAS_CLK_ENB);
	val = val | (1 | (1 << SYNT0_CLK_ENB));	/* RAS_CLK_ENB.synt0_clken clock enable and RAS_ACLK enable */
	writel(val, RAS_CLK_ENB);

	val = readl(RAS_SW_RST);
	val = val & ~(1 | (1 << SYNT0_CLK_ENB));	/* RAS_SW_RST.synt0_swrst disable sw reset */
	writel(val, RAS_SW_RST);
#endif /*LCAD_KERNEL*/

	
#ifdef MALI250
	MALI_PRINT(("Mali 200 Frequency 250MHz, no dedicated cache\n"));
#else
	MALI_PRINT(("Mali 200 Frequency 204MHz, no dedicated cache\n"));
#endif 
#endif /*SPEAr1340*/
}

/* SPEAr specific function - disable MALI and program clocks */
void spear_mali_exit()
{
	unsigned long val;
#ifdef KERNEL37
	val = readl(VA_RAS_CLK_ENB);
	val = val & ~(1 << SYNT0_CLK_ENB);	/* RAS_CLK_ENB.synt0_clken clock gated */
	writel(val, VA_RAS_CLK_ENB);
	val = readl(VA_RAS_SW_RST);
	val = val | (1 << SYNT0_CLK_ENB);	/* RAS_SW_RST.synt0_swrst enable sw reset */
	writel(val, VA_RAS_SW_RST);
#else
	val = readl(RAS_CLK_ENB);
	val = val & ~(1 << SYNT0_CLK_ENB);	/* RAS_CLK_ENB.synt0_clken clock gated */
	writel(val, RAS_CLK_ENB);
	val = readl(RAS_SW_RST);
	val = val | (1 << SYNT0_CLK_ENB);	/* RAS_SW_RST.synt0_swrst enable sw reset */
	writel(val, RAS_SW_RST);
#endif
}

_mali_osk_errcode_t mali_platform_init(void)
{
	spear_mali_init();
    MALI_SUCCESS;
}

_mali_osk_errcode_t mali_platform_deinit(void)
{
	spear_mali_exit();
    MALI_SUCCESS;
}

_mali_osk_errcode_t mali_platform_power_mode_change(mali_power_mode power_mode)
{
    MALI_SUCCESS;
}

void mali_gpu_utilization_handler(u32 utilization)
{
}

void set_mali_parent_power_domain(void* dev)
{
}


