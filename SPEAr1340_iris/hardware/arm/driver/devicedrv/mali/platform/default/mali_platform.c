/*
 * Copyright (C) 2010-2012 ARM Limited. All rights reserved.
 * Copyright (C) 2012 STMicroelectronics
 * 
 * Giuseppe Barba <giuseppe.barba@st.com>
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file mali_platform.c
 * SPEAr specific function - initializes clocks, enable MALI IP
 */
#include "mali_kernel_common.h"
#include "mali_osk.h"
#include "mali_platform.h"
#include <asm/io.h>			/* SPEAr specific code, ioremap */

/* SPEAR specific : misc regs */
#include <asm/io.h>			/* ioremap, readl etc */
#include <mach/misc_regs.h> 
#include <linux/module.h>   /* kernel module definitions */

void spear_mali_init(void)
{
	//unsigned long val;
	unsigned int* reg;
	
	MALI_PRINT(("Mali SPEAr1340, configuring clock synth\n"));

	/* clock synth configuration, SSCG4corresponding register in MISC is GEN_CLK_SSCG3 (sic) */
	reg = ioremap(0xE0700304, 0x4);

	/*asserting reset*/
	*reg = 0x20000;

	/* configuring freq divisor */
	/* in PLL_CFG select Fin=FVCO1/4 */
	/* configuring To (3 MSB are integer part ), output is Fout=Fin/(2*To) */
	*reg = 0x00002800; /* binary 0.101 decimal 0.625 */

	/*releasing reset*/
	*reg &= ~(0x20000);
	iounmap(reg);

	MALI_PRINT(("Mali SPEAr1340, enabling clock\n"));
	reg = ioremap(0xE0700314, 0x4); /* PERIP3_CLK_ENB */

	/*Enabling clock*/
	*reg = *reg | 0x40;
	iounmap(reg);

	/*Reset is 0 by default, trying to set anyway*/
	reg = ioremap(0xE0700320, 0x4);/*PERIP3_SW_RST*/
	*reg &= ~(0x1 << 6);

	MALI_PRINT(("Mali SPEAr1340, clock enabled\n"));
}

/* SPEAr specific function - disable MALI and program clocks */
void spear_mali_exit()
{
	unsigned long val;

	val = readl(VA_RAS_CLK_ENB);
	val = val & ~(1 << SYNT0_CLK_ENB);	/* RAS_CLK_ENB.synt0_clken clock gated */
	writel(val, VA_RAS_CLK_ENB);
	val = readl(VA_RAS_SW_RST);
	val = val | (1 << SYNT0_CLK_ENB);	/* RAS_SW_RST.synt0_swrst enable sw reset */
	writel(val, VA_RAS_SW_RST);
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


