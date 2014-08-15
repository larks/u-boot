/*
 * board/UiB/rcu-2.0/board.c
 *
 * Board specific code for the SmartFusion2 SoC on the ALICE Time Projection Chamber RCU2 
 *
 ** This file is property of and copyright by the Experimental Nuclear 
 ** Physics Group, Dep. of Physics and Technology
 ** University of Bergen, Norway, 2014
 ** This file has been written by Lars Bratrud,
 ** Lars.Bratrud@cern.ch
 **
 ** Permission to use, copy, modify and distribute this software and its  
 ** documentation strictly for non-commercial purposes is hereby granted  
 ** without fee, provided that the above copyright notice appears in all  
 ** copies and that both the copyright notice and this permission notice  
 ** appear in the supporting documentation. The authors make no claims    
 ** about the suitability of this software for any purpose. It is         
 ** provided "as is" without express or implied warranty.   
 */

#include <common.h>
#include <netdev.h>
#include <asm/arch/ddr.h>

const uint8_t frame_size = 16u;

/*
 * Generate DDR timings depending on the following DDR clock
 */
#define M2S_DDR_MHZ		(CONFIG_SYS_M2S_SYSREF / (1000 * 1000))

/*
 * Common conversion macros used for DDR cfg
 */

#define M2S_CLK_VAL(ns, div)	((((ns) * M2S_DDR_MHZ) / div))
#define M2S_CLK_MOD(ns, div)	((((ns) * M2S_DDR_MHZ) % div))

#define M2S_CLK_MIN(ns)		(M2S_CLK_MOD(ns,1000) ?			\
				 M2S_CLK_VAL(ns,1000) + 1 :		\
				 M2S_CLK_VAL(ns,1000))
#define M2S_CLK32_MIN(ns)	(M2S_CLK_MOD(ns,32000) ?		\
				 M2S_CLK_VAL(ns,32000) + 1 :		\
				 M2S_CLK_VAL(ns,32000))
#define M2S_CLK1024_MIN(ns)	(M2S_CLK_MOD(ns,1024000) ?		\
				 M2S_CLK_VAL(ns,1024000) + 1 :		\
				 M2S_CLK_VAL(ns,1024000))
#define M2S_CLK_MAX(ns)		(M2S_CLK_VAL(ns,1000))
#define M2S_CLK32_MAX(ns)	(M2S_CLK_VAL(ns,32000))
#define M2S_CLK1024_MAX(ns)	(M2S_CLK_VAL(ns,1024000))

/*
 * MT41J256M8 params & timings
 */
//#define DDR_BL			8	/* Burst length (value)		*/
//#define DDR_MR_BL		3	/* Burst length (power of 2)	*/
//#define DDR_BT			0	/* Burst type int(1)/seq(0)	*/

//#define DDR_CL			9	/* CAS (read) latency		*/
//#define DDR_WL			1	/* Write latency		*/
/*#define DDR_tMRD		2*/
/*#define DDR_tWTR		2*/
/*#define DDR_tXP			1*/
/*#define DDR_tCKE		1*/

/*#define DDR_tRFC		M2S_CLK_MIN(72)*/
/*#define DDR_tREFI		M2S_CLK32_MAX(7800)*/
/*#define DDR_tCKE_pre		M2S_CLK1024_MIN(200000)*/
/*#define DDR_tCKE_post		M2S_CLK1024_MIN(400)*/
/*#define DDR_tRCD		M2S_CLK_MIN(18)*/
/*#define DDR_tRRD		M2S_CLK_MIN(12)*/
/*#define DDR_tRP			M2S_CLK_MIN(18)*/
/*#define DDR_tRC			M2S_CLK_MIN(60)*/
/*#define DDR_tRAS_max		M2S_CLK1024_MAX(70000)*/
/*#define DDR_tRAS_min		M2S_CLK_MIN(42)*/
/*#define DDR_tWR			M2S_CLK_MIN(15)*/

/*
 * There are no these timings exactly in spec, so take smth
 */
#define DDR_tCCD		2	/* 2-reads/writes (bank A to B)	*/

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	return 0;
}

int checkboard(void)
{
	printf("Board: RCU-2.0 Rev %s\n",
		CONFIG_SYS_BOARD_REV_STR);
	return 0;
}

/*
 * Initialize DRAM
 */
int dram_init (void)
{
#if ( CONFIG_NR_DRAM_BANKS > 0 )
	volatile struct ddr_regs	*ddr = (void *)0x40020000;
	u16				val;

	/*
	 * Enable access to MDDR regs
	 */
	M2S_SYSREG->mddr_cr = (1 << 0);

	/*
	 * No non-bufferable regions
	 */
	M2S_SYSREG->ddrb_nb_size_cr = 0;

	ddr->ddrc.DYN_POWERDOWN_CR = (0 << REG_DDRC_POWERDOWN_EN);

	/*
	 * Apply DDR settings from Microsemi
	 */
	ddr->ddrc.DYN_SOFT_RESET_CR =			0;
	ddr->ddrc.DYN_REFRESH_1_CR =			0x27de;
	ddr->ddrc.DYN_REFRESH_2_CR =			0x30f;
	/* ddr->ddrc.DYN_POWERDOWN_CR =			0x02; */
	ddr->ddrc.DYN_DEBUG_CR =			0x00;
	ddr->ddrc.MODE_CR =				0x101;
	ddr->ddrc.ADDR_MAP_BANK_CR =			0x999;
	ddr->ddrc.ECC_DATA_MASK_CR =			0x0000;
	ddr->ddrc.ADDR_MAP_COL_1_CR =			0x3333;
	ddr->ddrc.ADDR_MAP_COL_2_CR =			0xffff;
	ddr->ddrc.ADDR_MAP_ROW_1_CR =			0x8888;
	ddr->ddrc.ADDR_MAP_ROW_2_CR =			0x8ff;
	ddr->ddrc.INIT_1_CR =				0x0001;
	ddr->ddrc.CKE_RSTN_CYCLES_1_CR =		0x4242;
	ddr->ddrc.CKE_RSTN_CYCLES_2_CR =		0x8;
	ddr->ddrc.INIT_MR_CR =				0x520;
	ddr->ddrc.INIT_EMR_CR =				0x44;
	ddr->ddrc.INIT_EMR2_CR =			0x0000;
	ddr->ddrc.INIT_EMR3_CR =			0x0000;
	ddr->ddrc.DRAM_BANK_TIMING_PARAM_CR =		0xce0;
	ddr->ddrc.DRAM_RD_WR_LATENCY_CR =		0x86;
	ddr->ddrc.DRAM_RD_WR_PRE_CR =			0x235;
	ddr->ddrc.DRAM_MR_TIMING_PARAM_CR =		0x5c;
	ddr->ddrc.DRAM_RAS_TIMING_CR =			0x10f;
	ddr->ddrc.DRAM_RD_WR_TRNARND_TIME_CR =		0x178;
	ddr->ddrc.DRAM_T_PD_CR =			0x33;
	ddr->ddrc.DRAM_BANK_ACT_TIMING_CR =		0x1947;
	ddr->ddrc.ODT_PARAM_1_CR =			0x10;
	ddr->ddrc.ODT_PARAM_2_CR =			0x0000;
	ddr->ddrc.ADDR_MAP_COL_3_CR =			0x3300;
	/* ddr->ddrc.DEBUG_CR =				0x3300; */
	ddr->ddrc.MODE_REG_RD_WR_CR =			0x0000;
	ddr->ddrc.MODE_REG_DATA_CR =			0x0000;
	ddr->ddrc.PWR_SAVE_1_CR =			0x506;
	ddr->ddrc.PWR_SAVE_2_CR =			0x0000;
	ddr->ddrc.ZQ_LONG_TIME_CR =			0x200;
	ddr->ddrc.ZQ_SHORT_TIME_CR =			0x40;
	ddr->ddrc.ZQ_SHORT_INT_REFRESH_MARGIN_1_CR =	0x12;
	ddr->ddrc.ZQ_SHORT_INT_REFRESH_MARGIN_2_CR =	0x2;
	ddr->ddrc.PERF_PARAM_1_CR =			0x4000;
	ddr->ddrc.HPR_QUEUE_PARAM_1_CR =		0x80f8;
	ddr->ddrc.HPR_QUEUE_PARAM_2_CR =		0x7;
	ddr->ddrc.LPR_QUEUE_PARAM_1_CR =		0x80f8;
	ddr->ddrc.LPR_QUEUE_PARAM_2_CR =		0x7;
	ddr->ddrc.WR_QUEUE_PARAM_CR =			0x200;
	ddr->ddrc.PERF_PARAM_2_CR =			0x400;
	ddr->ddrc.PERF_PARAM_3_CR =			0x0000;
	ddr->ddrc.DFI_RDDATA_EN_CR =			0x5;
	ddr->ddrc.DFI_MIN_CTRLUPD_TIMING_CR =		0x0003;
	ddr->ddrc.DFI_MAX_CTRLUPD_TIMING_CR =		0x0040;
	ddr->ddrc.DFI_WR_LVL_CONTROL_1_CR =		0x0000;
	ddr->ddrc.DFI_WR_LVL_CONTROL_2_CR =		0x0000;
	ddr->ddrc.DFI_RD_LVL_CONTROL_1_CR =		0x0000;
	ddr->ddrc.DFI_RD_LVL_CONTROL_2_CR =		0x0000;
	ddr->ddrc.DFI_CTRLUPD_TIME_INTERVAL_CR =	0x309;
	/* ddr->ddrc.DYN_SOFT_RESET_CR2 =		0x4; */
	ddr->ddrc.AXI_FABRIC_PRI_ID_CR =		0x0000;
	ddr->ddrc.ECC_INT_CLR_REG =			0x0000;

	ddr->phy.DYN_BIST_TEST_CR =			0x0;
	ddr->phy.DYN_BIST_TEST_ERRCLR_1_CR =		0x0;
	ddr->phy.DYN_BIST_TEST_ERRCLR_2_CR =		0x0;
	ddr->phy.DYN_BIST_TEST_ERRCLR_3_CR =		0x0;
	ddr->phy.BIST_TEST_SHIFT_PATTERN_1_CR =		0x0;
	ddr->phy.BIST_TEST_SHIFT_PATTERN_2_CR =		0x0;
	ddr->phy.BIST_TEST_SHIFT_PATTERN_3_CR =		0x0;
	ddr->phy.DYN_LOOPBACK_TEST_CR =			0x0000;
	ddr->phy.BOARD_LOOPBACK_CR =			0x0;
	ddr->phy.CTRL_SLAVE_RATIO_CR =			0x80;
	ddr->phy.CTRL_SLAVE_FORCE_CR =			0x0;
	ddr->phy.CTRL_SLAVE_DELAY_CR =			0x0;
	ddr->phy.DATA_SLICE_IN_USE_CR =			0xf;
	ddr->phy.LVL_NUM_OF_DQ0_CR =			0x0;
	ddr->phy.DQ_OFFSET_1_CR =			0x0;
	ddr->phy.DQ_OFFSET_2_CR =			0x0;
	ddr->phy.DQ_OFFSET_3_CR =			0x0;
	ddr->phy.DIS_CALIB_RST_CR =			0x0;
	ddr->phy.DLL_LOCK_DIFF_CR =			0xb;
	ddr->phy.FIFO_WE_IN_DELAY_1_CR =		0x0;
	ddr->phy.FIFO_WE_IN_DELAY_2_CR =		0x0;
	ddr->phy.FIFO_WE_IN_DELAY_3_CR =		0x0;
	ddr->phy.FIFO_WE_IN_FORCE_CR =			0x0;
	ddr->phy.FIFO_WE_SLAVE_RATIO_1_CR =		0x80;
	ddr->phy.FIFO_WE_SLAVE_RATIO_2_CR =		0x2004;
	ddr->phy.FIFO_WE_SLAVE_RATIO_3_CR =		0x100;
	ddr->phy.FIFO_WE_SLAVE_RATIO_4_CR =		0x8;
	ddr->phy.GATELVL_INIT_MODE_CR =			0x0;
	ddr->phy.GATELVL_INIT_RATIO_1_CR =		0x0;
	ddr->phy.GATELVL_INIT_RATIO_2_CR =		0x0;
	ddr->phy.GATELVL_INIT_RATIO_3_CR =		0x0;
	ddr->phy.GATELVL_INIT_RATIO_4_CR =		0x0;
	ddr->phy.LOCAL_ODT_CR =				0x1;
	ddr->phy.INVERT_CLKOUT_CR =			0x0;
	ddr->phy.RD_DQS_SLAVE_DELAY_1_CR =		0x0;
	ddr->phy.RD_DQS_SLAVE_DELAY_2_CR =		0x0;
	ddr->phy.RD_DQS_SLAVE_DELAY_3_CR =		0x0;
	ddr->phy.RD_DQS_SLAVE_FORCE_CR =		0x0;
	ddr->phy.RD_DQS_SLAVE_RATIO_1_CR =		0x4050;
	ddr->phy.RD_DQS_SLAVE_RATIO_2_CR =		0x501;
	ddr->phy.RD_DQS_SLAVE_RATIO_3_CR =		0x5014;
	ddr->phy.RD_DQS_SLAVE_RATIO_4_CR =		0x0;
	ddr->phy.WR_DQS_SLAVE_DELAY_1_CR =		0x0;
	ddr->phy.WR_DQS_SLAVE_DELAY_2_CR =		0x0;
	ddr->phy.WR_DQS_SLAVE_DELAY_3_CR =		0x0;
	ddr->phy.WR_DQS_SLAVE_FORCE_CR =		0x0;
	ddr->phy.WR_DQS_SLAVE_RATIO_1_CR =		0x0;
	ddr->phy.WR_DQS_SLAVE_RATIO_2_CR =		0x0;
	ddr->phy.WR_DQS_SLAVE_RATIO_3_CR =		0x0;
	ddr->phy.WR_DQS_SLAVE_RATIO_4_CR =		0x0;
	ddr->phy.WR_DATA_SLAVE_DELAY_1_CR =		0x0;
	ddr->phy.WR_DATA_SLAVE_DELAY_2_CR =		0x0;
	ddr->phy.WR_DATA_SLAVE_DELAY_3_CR =		0x0;
	ddr->phy.WR_DATA_SLAVE_FORCE_CR =		0x0;
	ddr->phy.WR_DATA_SLAVE_RATIO_1_CR =		0x50;
	ddr->phy.WR_DATA_SLAVE_RATIO_2_CR =		0x501;
	ddr->phy.WR_DATA_SLAVE_RATIO_3_CR =		0x5010;
	ddr->phy.WR_DATA_SLAVE_RATIO_4_CR =		0x0;
	ddr->phy.WRLVL_INIT_MODE_CR =			0x0;
	ddr->phy.WRLVL_INIT_RATIO_1_CR =		0x0;
	ddr->phy.WRLVL_INIT_RATIO_2_CR =		0x0;
	ddr->phy.WRLVL_INIT_RATIO_3_CR =		0x0;
	ddr->phy.WRLVL_INIT_RATIO_4_CR =		0x0;
	ddr->phy.WR_RD_RL_CR =				0x43;
	ddr->phy.RDC_FIFO_RST_ERRCNTCLR_CR =		0x0;
	ddr->phy.RDC_WE_TO_RE_DELAY_CR =		0x3;
	ddr->phy.USE_FIXED_RE_CR =			0x1;
	ddr->phy.USE_RANK0_DELAYS_CR =			0x1;
	ddr->phy.USE_LVL_TRNG_LEVEL_CR =		0x0;
	ddr->phy.DYN_CONFIG_CR =			0x0000;
	ddr->phy.RD_WR_GATE_LVL_CR =			0x0;

	/* ddr->fic.NB_ADDR_CR =			0x0; */
	/* ddr->fic.NBRWB_SIZE_CR =			0x0; */
	/* ddr->fic.WB_TIMEOUT_CR =			0x0; */
	/* ddr->fic.HPD_SW_RW_EN_CR =			0x0; */
	/* ddr->fic.HPD_SW_RW_INVAL_CR =		0x0; */
	/* ddr->fic.SW_WR_ERCLR_CR =			0x0; */
	/* ddr->fic.ERR_INT_ENABLE_CR =			0x0; */
	/* ddr->fic.NUM_AHB_MASTERS_CR =		0x0; */
	/* ddr->fic.LOCK_TIMEOUTVAL_1_CR =		0x0; */
	/* ddr->fic.LOCK_TIMEOUTVAL_2_CR =		0x0; */
	/* ddr->fic.LOCK_TIMEOUT_EN_CR =		0x0; */

	ddr->phy.DYN_RESET_CR =				0x1;
	ddr->ddrc.DYN_SOFT_RESET_CR =			0x0001;

	/*
	 * Set up U-Boot global data
	 */
	gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
	gd->bd->bi_dram[0].size = CONFIG_SYS_RAM_SIZE;

#endif
	return 0;
}

int misc_init_r(void)
{
	/* Try to force SPI */
/*	MSS_SPI_init(&g_mss_spi0);
	MSS_SPI_configure_master_mode(&g_mss_spi0,
                                  MSS_SPI_SLAVE_0,
                                  MSS_SPI_MODE0,
                                  MSS_SPI_PCLK_DIV_256,
                                  frame_size);

    This did nothing
  */        
	return 0;
}

#ifdef CONFIG_M2S_ETH
int board_eth_init(bd_t *bis)
{
	return 0;
	//return m2s_eth_driver_init(bis);
}
#endif

