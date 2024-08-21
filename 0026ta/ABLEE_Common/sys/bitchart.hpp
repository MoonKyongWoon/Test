#ifndef BITCHART_HPP
#define BITCHART_HPP

namespace BITCHART
{
    namespace GLOBAL_MODE_REG
    {
        enum
        {
			_READBACK                = 0x1000000,
			_VER_YMD                 = 0x1000004,
			_VER_FW                  = 0x1000008,
			_ALPG_CLEAR              = 0x1000010,
			_FPGA_PLL_RESET          = 0x1000014,
			_DIAG_REG_CLEAR          = 0x1000018,
			_DBM_ADDR_CLEAR          = 0x100001C,
			_PE_CLEAR		         = 0x1000020,	//2022.12.23 - kwmoon(replace ioa/iob/uclk chip reset)
			_DIAG_COUNT              = 0x1000024,
			_DRV_CLAER               = 0x1000028,
			_CMP_CLAER               = 0x100002C,
			_CONFIG_REQ              = 0x1000034,
			_CONFIG_BUSY             = 0x1000038,
			_DFM_CLEAR               = 0x100003C,
			_DMA_WR_RD_SEL           = 0x1000040,
			_EDQ_SEL                 = 0x1000044,
			_DBM_DFM_SEL             = 0x1000048,
//			_DRAM_CTRL_RESET         = 0x100004C,	//2022.12.01 - kwmoon(removed)
			_TPG_IDELAY_CTRL_RESET   = 0x100004C,	//2022.12.01 - kwmoon(removed)
			_TG_MEM_CLEAR            = 0x1000050,
			_IO_DELAY_RESET          = 0x1000054,
			_CPU_RESET               = 0x1000058,
			_DFM_WRITE_SEL           = 0x100005C,
			_DFM_SAVE_EN             = 0x1000060,
			_BCLK_RESET              = 0x1000064,
			_BIB_CE_SEL              = 0x1000068,	//2022.11.23 - kwmoon
			_STI_LDI_CLEAR           = 0x100006C,	//2023.05.22 - fatric
			_PE_READ_ENABLE_SEL      = 0x1000080,	//2022.11.23 - kwmoon
			_DBM_CTRL_RESET          = 0x1000084,	//2022.11.23 - kwmoon
			_DFM_CTRL_RESET          = 0x1000088,	//2022.11.23 - kwmoon
			_DR_DRE_SEL              = 0x1000090,	//2023.02.02 - fatric
			_TG_START                = 0x10000A0,
			_TG_STOP                 = 0x10000A4,
			_FREQ_REQ                = 0x1000100,
			_BCLK1_FREQ_R            = 0x1000104,
			_BCLK2_FREQ_R            = 0x1000108,
			_BCLK3_FREQ_R            = 0x100010C,
			_BCLK4_FREQ_R            = 0x1000110,
			_BCLK5_FREQ_R            = 0x1000114,
			_BCLK6_FREQ_R            = 0x1000118,
			_BCLK7_FREQ_R            = 0x100011C,
			_BCLK8_FREQ_R            = 0x1000120,
			_BCLK9_FREQ_R            = 0x1000124,
			_BCLK10_FREQ_R           = 0x1000128,
			_STRB1_FREQ_R            = 0x100012C,
			_STRB2_FREQ_R            = 0x1000130,
			_STRB3_FREQ_R            = 0x1000134,
			_STRB4_FREQ_R            = 0x1000138,
			_RSTRB1_FREQ_R           = 0x100013C,
			_RSTRB2_FREQ_R           = 0x1000140,
			_RSTRB3_FREQ_R           = 0x1000144,
			_RSTRB4_FREQ_R           = 0x1000148,
			_DRELT_FREQ_R            = 0x100014C,
			_RATE_FREQ_R             = 0x1000150,
			_TRANS_PLL_FREQ_R        = 0x1000154,
			_CPE_COUNT               = 0x1000158,
			_FC1_FAIL_COUNT          = 0x1000160,
			_FC2_FAIL_COUNT          = 0x1000164,
			_FC12_FAIL_COUNT         = 0x1000168,
			_ALL_SCAN_OR_b31_b0      = 0x1000170,
			_ALL_SCAN_OR_b63_b32     = 0x1000174,
			_ALL_SCAN_AND_b31_b0     = 0x1000178,
			_ALL_SCAN_AND_b63_b32    = 0x100017C,
			_FM_WRITE_MODE           = 0x1000180,	//2022.11.17 - KWMOON
			_FM_LAST_READ            = 0x1000184,	//2022.11.18 - KWMOON
			_FM_FAIL_LIMIT           = 0x100018C,	//2022.12.01 - KWMOON
			_FM_MULTI_IO             = 0x1000190,	//2022.12.01 - KWMOON
			_ALL_SCAN_MATCH_FLAG     = 0x10001A0,	//2023.08.16 - KWMOON
			_MATCH_CPE_DELAY         = 0x10001A4,	//2022.12.01 - KWMOON
		};
    };

    namespace DRV_IOA
    {
        enum
        {
			_READBACK                = 0x1030000,
			_VER_HYEAR               = 0x1030004,
			_VER_LYEAR               = 0x1030008,
			_VER_MONTH               = 0x103000C,
			_VER_DAY                 = 0x1030010,
			_VER_FW                  = 0x1030014,
			_VER_CHIP                = 0x1030018,
			_CHIP_CLEAR              = 0x1030020,
			_DRV_CALIBRATION         = 0x1030024,
			_CPU_RESET               = 0x1030028,
			_CPE_MASK                = 0x103002C,
			_BIB_IF                  = 0x1030030,
			_BIB_IF_READ             = 0x1030034,
			_BIB_SEL                 = 0x1030040,
			_BIB_ID                  = 0x1030044,
			_BOARD_CONTACT           = 0x103004C,

			_BCLK9_IDELAY            = 0x1030050,
			_BCLK10_IDELAY           = 0x1030054,
			_CPE0_IDELAY             = 0x1030058,
			_CPE1_IDELAY             = 0x103005C,
			_STRB1_0_IDELAY          = 0x1030060,
			_STRB1_1_IDELAY          = 0x1030064,
			_STRB2_0_IDELAY          = 0x1030068,
			_STRB2_1_IDELAY          = 0x103006C,
			_IO_DELAY_RESET          = 0x1030070,
			_IO_DELAY_REQ            = 0x1030074,
			_CAL_SEL                 = 0x1030078,
			_FC1_IOA_EN_b47_b40      = 0x1030080,
			_FC1_IOA_EN_b39_b32      = 0x1030084,
			_FC1_IOA_EN_b31_b24      = 0x1030088,
			_FC1_IOA_EN_b23_b16      = 0x103008C,
			_FC1_IOA_EN_b15_b8       = 0x1030090,
			_FC1_IOA_EN_b7_b0        = 0x1030094,
			_FC2_IOA_EN_b47_b40      = 0x1030098,
			_FC2_IOA_EN_b39_b32      = 0x103009C,
			_FC2_IOA_EN_b31_b24      = 0x10300A0,
			_FC2_IOA_EN_b23_b16      = 0x10300A4,
			_FC2_IOA_EN_b15_b8       = 0x10300A8,
			_FC2_IOA_EN_b7_b0        = 0x10300AC,
			_FC1_DRV_EN_b31_b24      = 0x10300B0,
			_FC1_DRV_EN_b23_b16      = 0x10300B4,
			_FC1_DRV_EN_b15_b8       = 0x10300B8,
			_FC1_DRV_EN_b7_b0        = 0x10300BC,

			_FC1_IOA_RDATA_b47_b40   = 0x10300C0,
			_FC1_IOA_RDATA_b39_b32   = 0x10300C4,
			_FC1_IOA_RDATA_b31_b24   = 0x10300C8,
			_FC1_IOA_RDATA_b23_b16   = 0x10300CC,
			_FC1_IOA_RDATA_b15_b8    = 0x10300D0,
			_FC1_IOA_RDATA_b7_b0     = 0x10300D4,
			_FC2_IOA_RDATA_b47_b40   = 0x10300D8,
			_FC2_IOA_RDATA_b39_b32   = 0x10300DC,
			_FC2_IOA_RDATA_b31_b24   = 0x10300E0,
			_FC2_IOA_RDATA_b23_b16   = 0x10300E4,
			_FC2_IOA_RDATA_b15_b8    = 0x10300E8,
			_FC2_IOA_RDATA_b7_b0     = 0x10300EC,
			_FC1_DRV_RDATA_b31_b24   = 0x10300F0,
			_FC1_DRV_RDATA_b23_b16   = 0x10300F4,
			_FC1_DRV_RDATA_b15_b8    = 0x10300F8,
			_FC1_DRV_RDATA_b7_b0     = 0x10300FC,

			_PPS_DGS_CHECK           = 0x1030120,
			_S1PS_DGS_CHECK          = 0x1030124,
			_PPS_SENSE_CHECK         = 0x1030128,
			_S1PS_SENSE_CHECK        = 0x103012C,
			_READ_SIGNAL_ENABLE      = 0x1030130,
			_PPS_SENSEING_CONTROL    = 0x1030134,
			_SYSTEM_MANAGE_ADDR      = 0x103013C,
			_SYSTEM_STATUS_REQ       = 0x1030140,
			_SYSTEM_STATUS_READ_L    = 0x1030144,
			_SYSTEM_STATUS_READ_H    = 0x1030148,
			_SYSTEM_ALARM_FLAG       = 0x103014C,
			//FIXH
			_FC1_IOA_FIXH_b47_b40    = 0x1030150,
			_FC1_IOA_FIXH_b39_b32    = 0x1030154,
			_FC1_IOA_FIXH_b31_b24    = 0x1030158,
			_FC1_IOA_FIXH_b23_b16    = 0x103015C,
			_FC1_IOA_FIXH_b15_b8     = 0x1030160,
			_FC1_IOA_FIXH_b7_b0      = 0x1030164,
			_FC2_IOA_FIXH_b47_b40    = 0x1030168,
			_FC2_IOA_FIXH_b39_b32    = 0x103016C,
			_FC2_IOA_FIXH_b31_b24    = 0x1030170,
			_FC2_IOA_FIXH_b23_b16    = 0x1030174,
			_FC2_IOA_FIXH_b15_b8     = 0x1030178,
			_FC2_IOA_FIXH_b7_b0      = 0x103017C,
			_FC1_DRV_FIXH_b31_b24    = 0x1030180,
			_FC1_DRV_FIXH_b23_b16    = 0x1030184,
			_FC1_DRV_FIXH_b15_b8     = 0x1030188,
			_FC1_DRV_FIXH_b7_b0      = 0x103018C,
			//FIXL
			_FC1_IOA_FIXL_b47_b40    = 0x1030190,
			_FC1_IOA_FIXL_b39_b32    = 0x1030194,
			_FC1_IOA_FIXL_b31_b24    = 0x1030198,
			_FC1_IOA_FIXL_b23_b16    = 0x103019C,
			_FC1_IOA_FIXL_b15_b8     = 0x10301A0,
			_FC1_IOA_FIXL_b7_b0      = 0x10301A4,
			_FC2_IOA_FIXL_b47_b40    = 0x10301A8,
			_FC2_IOA_FIXL_b39_b32    = 0x10301AC,
			_FC2_IOA_FIXL_b31_b24    = 0x10301B0,
			_FC2_IOA_FIXL_b23_b16    = 0x10301B4,
			_FC2_IOA_FIXL_b15_b8     = 0x10301B8,
			_FC2_IOA_FIXL_b7_b0      = 0x10301BC,
			_FC1_DRV_FIXL_b31_b24    = 0x10301C0,
			_FC1_DRV_FIXL_b23_b16    = 0x10301C4,
			_FC1_DRV_FIXL_b15_b8     = 0x10301C8,
			_FC1_DRV_FIXL_b7_b0      = 0x10301CC,

			_FC12_IOA_DRE_FIXH       = 0x10301D0,	//IOA_IO/DRV FIXH CONTROL -> //2023.02.27 - KWMOON(IOA FIXH)
			_FC1_DRV_DRE_FIXH        = 0x10301D4,	//2023.02.27 - KWMOON(DRV FIXH)

			_S1PS_VOLTAGE_LV         = 0x10301F0,	//2023.03.02 - KWMOON
			_DRV_IOA_DCI_LOCK        = 0x10302D4,
			_DRV_IOA_DCI_DATA_WRT    = 0x10302E0,
			_DRV_IOA_DCI_RESULT      = 0x10302E4,
		};
	}

	namespace DRV_IOB
	{
		enum
		{
			_READBACK                = 0x1034000,
			_VER_HYEAR               = 0x1034004,
			_VER_LYEAR               = 0x1034008,
			_VER_MONTH               = 0x103400C,
			_VER_DAY                 = 0x1034010,
			_VER_FW                  = 0x1034014,
			_VER_CHIP                = 0x1034018,
			_CHIP_CLEAR              = 0x1034020,
			_DRV_CALIBRATION         = 0x1034024,
			_CPE_MASK                = 0x103402C,

			_BCLK9_IDELAY            = 0x1034050,
			_BCLK10_IDELAY           = 0x1034054,
			_CPE2_IDELAY             = 0x1034058,
			_CPE3_IDELAY             = 0x103405C,

			_STRB3_0_IDELAY          = 0x1034060,
			_STRB3_1_IDELAY          = 0x1034064,
			_STRB4_0_IDELAY          = 0x1034068,
			_STRB4_1_IDELAY          = 0x103406C,
			_IO_DELAY_RESET          = 0x1034070,
			_IO_DELAY_REQ            = 0x1034074,
			_CAL_SEL                 = 0x1034078,
			_FC1_IOB_EN_b47_b40      = 0x1034080,
			_FC1_IOB_EN_b39_b32      = 0x1034084,
			_FC1_IOB_EN_b31_b24      = 0x1034088,
			_FC1_IOB_EN_b23_b16      = 0x103408C,
			_FC1_IOB_EN_b15_b8       = 0x1034090,
			_FC1_IOB_EN_b7_b0        = 0x1034094,
			_FC2_IOB_EN_b47_b40      = 0x1034098,
			_FC2_IOB_EN_b39_b32      = 0x103409C,
			_FC2_IOB_EN_b31_b24      = 0x10340A0,
			_FC2_IOB_EN_b23_b16      = 0x10340A4,
			_FC2_IOB_EN_b15_b8       = 0x10340A8,
			_FC2_IOB_EN_b7_b0        = 0x10340AC,
			_FC2_DRV_EN_b31_b24      = 0x10340B0,
			_FC2_DRV_EN_b23_b16      = 0x10340B4,
			_FC2_DRV_EN_b15_b8       = 0x10340B8,
			_FC2_DRV_EN_b7_b0        = 0x10340BC,

			_FC1_IOB_RDATA_b47_b40   = 0x10340C0,
			_FC1_IOB_RDATA_b39_b32   = 0x10340C4,
			_FC1_IOB_RDATA_b31_b24   = 0x10340C8,
			_FC1_IOB_RDATA_b23_b16   = 0x10340CC,
			_FC1_IOB_RDATA_b15_b8    = 0x10340D0,
			_FC1_IOB_RDATA_b7_b0     = 0x10340D4,
			_FC2_IOB_RDATA_b47_b40   = 0x10340D8,
			_FC2_IOB_RDATA_b39_b32   = 0x10340DC,
			_FC2_IOB_RDATA_b31_b24   = 0x10340E0,
			_FC2_IOB_RDATA_b23_b16   = 0x10340E4,
			_FC2_IOB_RDATA_b15_b8    = 0x10340E8,
			_FC2_IOB_RDATA_b7_b0     = 0x10340EC,
			_FC2_DRV_RDATA_b31_b24   = 0x10340F0,
			_FC2_DRV_RDATA_b23_b16   = 0x10340F4,
			_FC2_DRV_RDATA_b15_b8    = 0x10340F8,
			_FC2_DRV_RDATA_b7_b0     = 0x10340FC,

			//2023.03.02 - KWMOON(MOVE TO UCLK)
			//_EEPROM_INS              = 0x1034100,
			//_EEPROM_ADDR_H           = 0x1034104,
			//_EEPROM_ADDR_L           = 0x1034108,
			//_EEPROM_DATA             = 0x103410C,
			//_EEPROM_WRITE_REQ        = 0x1034110,
			//_EEPROM_READ             = 0x1034114,

			_READ_SIGNAL_ENABLE      = 0x1034130,
			_SYSTEM_MANAGE_ADDR      = 0x103413C,
			_SYSTEM_STATUS_REQ       = 0x1034140,
			_SYSTEM_STATUS_READ_L    = 0x1034144,
			_SYSTEM_STATUS_READ_H    = 0x1034148,
			_SYSTEM_ALARM_FLAG       = 0x103414C,

			//FIXH
			_FC1_IOB_FIXH_b47_b40    = 0x1034150,
			_FC1_IOB_FIXH_b39_b32    = 0x1034154,
			_FC1_IOB_FIXH_b31_b24    = 0x1034158,
			_FC1_IOB_FIXH_b23_b16    = 0x103415C,
			_FC1_IOB_FIXH_b15_b8     = 0x1034160,
			_FC1_IOB_FIXH_b7_b0      = 0x1034164,
			_FC2_IOB_FIXH_b47_b40    = 0x1034168,
			_FC2_IOB_FIXH_b39_b32    = 0x103416C,
			_FC2_IOB_FIXH_b31_b24    = 0x1034170,
			_FC2_IOB_FIXH_b23_b16    = 0x1034174,
			_FC2_IOB_FIXH_b15_b8     = 0x1034178,
			_FC2_IOB_FIXH_b7_b0      = 0x103417C,
			_FC2_DRV_FIXH_b31_b24    = 0x1034180,
			_FC2_DRV_FIXH_b23_b16    = 0x1034184,
			_FC2_DRV_FIXH_b15_b8     = 0x1034188,
			_FC2_DRV_FIXH_b7_b0      = 0x103418C,
			//FIXL
			_FC1_IOB_FIXL_b47_b40    = 0x1034190,
			_FC1_IOB_FIXL_b39_b32    = 0x1034194,
			_FC1_IOB_FIXL_b31_b24    = 0x1034198,
			_FC1_IOB_FIXL_b23_b16    = 0x103419C,
			_FC1_IOB_FIXL_b15_b8     = 0x10341A0,
			_FC1_IOB_FIXL_b7_b0      = 0x10341A4,
			_FC2_IOB_FIXL_b47_b40    = 0x10341A8,
			_FC2_IOB_FIXL_b39_b32    = 0x10341AC,
			_FC2_IOB_FIXL_b31_b24    = 0x10341B0,
			_FC2_IOB_FIXL_b23_b16    = 0x10341B4,
			_FC2_IOB_FIXL_b15_b8     = 0x10341B8,
			_FC2_IOB_FIXL_b7_b0      = 0x10341BC,
			_FC2_DRV_FIXL_b31_b24    = 0x10341C0,
			_FC2_DRV_FIXL_b23_b16    = 0x10341C4,
			_FC2_DRV_FIXL_b15_b8     = 0x10341C8,
			_FC2_DRV_FIXL_b7_b0      = 0x10341CC,

			//_IOB_DRE_FIXH            = 0x10341D0,	//IOA_IO/DRV FIXH CONTROL
			_FC12_IOB_DRE_FIXH       = 0x10341D0,	//IOB_IO/DRV FIXH CONTROL -> //2023.02.27 - KWMOON(IOA FIXH)
			_FC2_DRV_DRE_FIXH        = 0x10341D4,	//2023.02.27 - KWMOON(DRV FIXH)
			_DRV_IOB_DCI_LOCK        = 0x10342D4,
			_DRV_IOB_DCI_DATA_WRT    = 0x10342E0,
			_DRV_IOB_DCI_RESULT      = 0x10342E4,

		};
	}

    namespace DRV_UCLK
    {
        enum
        {
			_READBACK                = 0x1038000,
			_VER_HYEAR               = 0x1038004,
			_VER_LYEAR               = 0x1038008,
			_VER_MONTH               = 0x103800C,
			_VER_DAY                 = 0x1038010,
			_VER_FW                  = 0x1038014,
			_VER_CHIP                = 0x1038018,
			_CHIP_CLEAR              = 0x1038020,
			_DRV_CALIBRATION         = 0x1038024,
			_CPE_MASK                = 0x103802C,

			_EEPROM_INS              = 0x1038030,
			_EEPROM_ADDR_H           = 0x1038034,
			_EEPROM_ADDR_L           = 0x1038038,
			_EEPROM_DATA             = 0x103803C,
			_EEPROM_WRITE_REQ        = 0x1038040,
			_EEPROM_READ             = 0x1038044,

			_BCLK1_IDELAY            = 0x1038060,
			_BCLK2_IDELAY            = 0x1038064,
			_BCLK3_IDELAY            = 0x1038068,
			_BCLK4_IDELAY            = 0x103806C,
			_BCLK5_IDELAY            = 0x1038070,
			_BCLK6_IDELAY            = 0x1038074,
			_BCLK7_IDELAY            = 0x1038078,
			_BCLK8_IDELAY            = 0x103807C,
			_STRB1_0_IDELAY          = 0x1038080,
			_STRB1_1_IDELAY          = 0x1038084,
			_CPE_IDELAY              = 0x1038088,

			_IO_DELAY_RESET          = 0x1038090,
			_IO_DELAY_REQ            = 0x1038094,
			_CAL_SEL                 = 0x1038098,
			_UCLK_SEL                = 0x103809C,
			_SLOT_ID                 = 0x10380A0,
			_DAC_CLEAR               = 0x10380A4,
			_DAC_LOAD                = 0x10380A8,
			_DAC_CMD_ADDR            = 0x10380AC,
			_DAC_WDATA_H             = 0x10380B0,
			_DAC_WDATA_L             = 0x10380B4,
			_DAC_WRITE_REQ           = 0x10380B8,

			_FC1_UCLK_RDATA_b87_b80  = 0x10380C0,
			_FC1_UCLK_RDATA_b79_b72  = 0x10380C4,
			_FC1_UCLK_RDATA_b71_b64  = 0x10380C8,
			_FC1_UCLK_RDATA_b63_b56  = 0x10380CC,
			_FC1_UCLK_RDATA_b55_b48  = 0x10380D0,
			_FC1_UCLK_RDATA_b47_b40  = 0x10380D4,
			_FC1_UCLK_RDATA_b39_b32  = 0x10380D8,
			_FC1_UCLK_RDATA_b31_b24  = 0x10380DC,
			_FC1_UCLK_RDATA_b23_b16  = 0x10380E0,
			_FC1_UCLK_RDATA_b15_b8   = 0x10380E4,
			_FC1_UCLK_RDATA_b7_b0    = 0x10380E8,
			_FC2_UCLK_RDATA_b87_b80  = 0x10380EC,
			_FC2_UCLK_RDATA_b79_b72  = 0x10380F0,
			_FC2_UCLK_RDATA_b71_b64  = 0x10380F4,
			_FC2_UCLK_RDATA_b63_b56  = 0x10380F8,
			_FC2_UCLK_RDATA_b55_b48  = 0x10380FC,
			_FC2_UCLK_RDATA_b47_b40  = 0x1038100,
			_FC2_UCLK_RDATA_b39_b32  = 0x1038104,
			_FC2_UCLK_RDATA_b31_b24  = 0x1038108,
			_FC2_UCLK_RDATA_b23_b16  = 0x103810C,
			_FC2_UCLK_RDATA_b15_b8   = 0x1038110,
			_FC2_UCLK_RDATA_b7_b0    = 0x1038114,

			_ADC_CONV_START          = 0x1038120,
			_ADC_BYSY                = 0x1038124,
			_ADC_DATA_TRANSFER       = 0x1038128,
			_ADC_RESET               = 0x103812C,
			_ADC_CLEAR               = 0x1038130,
			_ADC_MUX                 = 0x1038134,
			_ADC_CH1_READ_H          = 0x1038140,
			_ADC_CH1_READ_L          = 0x1038144,
			_ADC_CH2_READ_H          = 0x1038148,
			_ADC_CH2_READ_L          = 0x103814C,
			_ADC_CH3_READ_H          = 0x1038150,
			_ADC_CH3_READ_L          = 0x1038154,
			_ADC_CH4_READ_H          = 0x1038158,
			_ADC_CH4_READ_L          = 0x103815C,
			_ADC_CH5_READ_H          = 0x1038160,
			_ADC_CH5_READ_L          = 0x1038164,
			_ADC_CH6_READ_H          = 0x1038168,
			_ADC_CH6_READ_L          = 0x103816C,
			_ADC_CH7_READ_H          = 0x1038170,
			_ADC_CH7_READ_L          = 0x1038174,
			_ADC_CH8_READ_H          = 0x1038178,
			_ADC_CH8_READ_L          = 0x103817C,

			_FC1_UCLK_EN_b87_b80     = 0x1038180,
			_FC1_UCLK_EN_b79_b72     = 0x1038184,
			_FC1_UCLK_EN_b71_b64     = 0x1038188,
			_FC1_UCLK_EN_b63_b56     = 0x103818C,
			_FC1_UCLK_EN_b55_b48     = 0x1038190,
			_FC1_UCLK_EN_b47_b40     = 0x1038194,
			_FC1_UCLK_EN_b39_b32     = 0x1038198,
			_FC1_UCLK_EN_b31_b24     = 0x103819C,
			_FC1_UCLK_EN_b23_b16     = 0x10381A0,
			_FC1_UCLK_EN_b15_b8      = 0x10381A4,
			_FC1_UCLK_EN_b7_b0       = 0x10381A8,
			_FC2_UCLK_EN_b87_b80     = 0x10381AC,
			_FC2_UCLK_EN_b79_b72     = 0x10381B0,
			_FC2_UCLK_EN_b71_b64     = 0x10381B4,
			_FC2_UCLK_EN_b63_b56     = 0x10381B8,
			_FC2_UCLK_EN_b55_b48     = 0x10381BC,
			_FC2_UCLK_EN_b47_b40     = 0x10381C0,
			_FC2_UCLK_EN_b39_b32     = 0x10381C4,
			_FC2_UCLK_EN_b31_b24     = 0x10381C8,
			_FC2_UCLK_EN_b23_b16     = 0x10381CC,
			_FC2_UCLK_EN_b15_b8      = 0x10381D0,
			_FC2_UCLK_EN_b7_b0       = 0x10381D4,

			_READ_SIGNAL_ENABLE      = 0x10381E0,
			_SYSTEM_MANAGE_ADDR      = 0x10381EC,
			_SYSTEM_STATUS_REQ       = 0x10381F0,
			_SYSTEM_STATUS_READ_L    = 0x10381F4,
			_SYSTEM_STATUS_READ_H    = 0x10381F8,
			_SYSTEM_ALARM_FLAG       = 0x10381FC,


			//FIXH
			_FC1_UCLK_FIXH_b87_b80    = 0x1038200,
			_FC1_UCLK_FIXH_b79_b72    = 0x1038204,
			_FC1_UCLK_FIXH_b71_b64    = 0x1038208,
			_FC1_UCLK_FIXH_b63_b56    = 0x103820C,
			_FC1_UCLK_FIXH_b55_b48    = 0x1038210,
			_FC1_UCLK_FIXH_b47_b40    = 0x1038214,
			_FC1_UCLK_FIXH_b39_b32    = 0x1038218,
			_FC1_UCLK_FIXH_b31_b24    = 0x103821C,
			_FC1_UCLK_FIXH_b23_b16    = 0x1038220,
			_FC1_UCLK_FIXH_b15_b8     = 0x1038224,
			_FC1_UCLK_FIXH_b7_b0      = 0x1038228,
			_FC2_UCLK_FIXH_b87_b80    = 0x103822C,
			_FC2_UCLK_FIXH_b79_b72    = 0x1038230,
			_FC2_UCLK_FIXH_b71_b64    = 0x1038234,
			_FC2_UCLK_FIXH_b63_b56    = 0x1038238,
			_FC2_UCLK_FIXH_b55_b48    = 0x103823C,
			_FC2_UCLK_FIXH_b47_b40    = 0x1038240,
			_FC2_UCLK_FIXH_b39_b32    = 0x1038244,
			_FC2_UCLK_FIXH_b31_b24    = 0x1038248,
			_FC2_UCLK_FIXH_b23_b16    = 0x103824C,
			_FC2_UCLK_FIXH_b15_b8     = 0x1038250,
			_FC2_UCLK_FIXH_b7_b0      = 0x1038254,
			//FIXL
			_FC1_UCLK_FIXL_b87_b80    = 0x1038260,
			_FC1_UCLK_FIXL_b79_b72    = 0x1038264,
			_FC1_UCLK_FIXL_b71_b64    = 0x1038268,
			_FC1_UCLK_FIXL_b63_b56    = 0x103826C,
			_FC1_UCLK_FIXL_b55_b48    = 0x1038270,
			_FC1_UCLK_FIXL_b47_b40    = 0x1038274,
			_FC1_UCLK_FIXL_b39_b32    = 0x1038278,
			_FC1_UCLK_FIXL_b31_b24    = 0x103827C,
			_FC1_UCLK_FIXL_b23_b16    = 0x1038280,
			_FC1_UCLK_FIXL_b15_b8     = 0x1038284,
			_FC1_UCLK_FIXL_b7_b0      = 0x1038288,
			_FC2_UCLK_FIXL_b87_b80    = 0x103828C,
			_FC2_UCLK_FIXL_b79_b72    = 0x1038290,
			_FC2_UCLK_FIXL_b71_b64    = 0x1038294,
			_FC2_UCLK_FIXL_b63_b56    = 0x1038298,
			_FC2_UCLK_FIXL_b55_b48    = 0x103829C,
			_FC2_UCLK_FIXL_b47_b40    = 0x10382A0,
			_FC2_UCLK_FIXL_b39_b32    = 0x10382A4,
			_FC2_UCLK_FIXL_b31_b24    = 0x10382A8,
			_FC2_UCLK_FIXL_b23_b16    = 0x10382AC,
			_FC2_UCLK_FIXL_b15_b8     = 0x10382B0,
			_FC2_UCLK_FIXL_b7_b0      = 0x10382B4,
			_UCLK_DRE_FIXH            = 0x10382B8,
			_DRV_UCLK_DCI_LOCK        = 0x10382F4,
			_DRV_UCLK_DCI_DATA_WRT    = 0x1038300,
			_DRV_UCLK_DCI_RESULT      = 0x1038304,
		};
    }

	namespace MIO
	{
		enum MIO_CTRL
		{
			_MIO_ENABLE              = 0x1010000,
			_MIO_DATA                = 0x1010004,
			_MIO_READ                = 0x1010008,
		};
	}

	namespace PPS_S1PS
	{
		enum CTRL
		{
			_PPS_WRITE_PROTOCOL      = 0x1010100,
			_PPS_READ_PROTOCOL       = 0x1010104,
			_PPS_READ                = 0x1010108,
			_PPS_WRITE_PEC           = 0x101010C,	//2022.11.07
			_PPS_SMBALERT_MASK_W     = 0x1010110,
			_PPS_SMBALERT_MASK_R     = 0x1010114,
			_PPS_REQUEST             = 0x101011C,
			_PPS_PROTOCOL_SELECT     = 0x1010120,
			_PPS_ENABLE              = 0x1010130,
			_PPS_ALERT               = 0x1010134,

			_S1PS_WRITE_PROTOCOL     = 0x1010180,
			_S1PS_READ_PROTOCOL      = 0x1010184,
			_S1PS_READ               = 0x1010188,
			_S1PS_WRITE_PEC          = 0x101018C,	//2022.11.07
			_S1PS_SMBALERT_MASK_W    = 0x1010190,
			_S1PS_SMBALERT_MASK_R    = 0x1010194,
			_S1PS_REQUEST            = 0x101019C,
			_S1PS_PROTOCOL_SELECT    = 0x10101A0,
			_S1PS_ENABLE             = 0x10101B0,
			_S1PS_ALERT              = 0x10101B4,
		};
	}

	namespace SYSTEM_MANAGEMENT
	{
		enum CTRL
		{
			_SYSTEM_ADDR             = 0x1010200,
			_STATUS_REQ              = 0x1010204,
			_STATUS_READ             = 0x1010208,
			_STATUS_ALARM_FLAG       = 0x101020C,
			_FC1_IOA_RDATA_b31_b0    = 0x1010210,
			_FC1_IOA_RDATA_b47_b32   = 0x1010214,
			_FC1_IOB_RDATA_b31_b0    = 0x1010218,
			_FC1_IOB_RDATA_b47_b32   = 0x101021C,
			_FC2_IOA_RDATA_b31_b0    = 0x1010220,
			_FC2_IOA_RDATA_b47_b32   = 0x1010224,
			_FC2_IOB_RDATA_b31_b0    = 0x1010228,
			_FC2_IOB_RDATA_b47_b32   = 0x101022C,
			_FC_DRV_RDATA_b31_b0     = 0x1010230,
			_FC_UCLK_RDATA_b31_b0    = 0x1010234,
			_FC_UCLK_RDATA_b63_b32   = 0x1010238,
			_FC_UCLK_RDATA_b87_b64   = 0x101023C,
		};
	}

	//2022.12.22 - KWMOON
	namespace PFM
	{
		enum PFM_CTRL
		{
			PFM_CLEAR                  = 0x1010300,
			PFM_REQUEST                = 0x1010304,
			PFM_BUSY                   = 0x1010308,
			PFM_PPS_FAULT_MASK         = 0x1010400,
			PFM_S1PS_FAULT_MASK        = 0x1010404,
		};
		enum PFM_FAULT_POINTER
		{
			PFM_STATUS                 = 0x101030C,
			PFM_PPS1_FAULT_POINTER     = 0x1010310,
			PFM_PPS2_FAULT_POINTER     = 0x1010314,
			PFM_PPS3_FAULT_POINTER     = 0x1010318,
			PFM_PPS4_FAULT_POINTER     = 0x101031C,
			PFM_PPS5_FAULT_POINTER     = 0x1010320,
			PFM_PPS6_FAULT_POINTER     = 0x1010324,
			PFM_PPS7_FAULT_POINTER     = 0x1010328,
			PFM_PPS8_FAULT_POINTER     = 0x101032C,
			PFM_S1PS1_FAULT_POINTER    = 0x1010330,
			PFM_S1PS2_FAULT_POINTER    = 0x1010334,
			PFM_S1PS3_FAULT_POINTER    = 0x1010338,
			PFM_S1PS4_FAULT_POINTER    = 0x101033C,
			PFM_S1PS5_FAULT_POINTER    = 0x1010340,
			PFM_S1PS6_FAULT_POINTER    = 0x1010344,
			PFM_S1PS7_FAULT_POINTER    = 0x1010348,
			PFM_S1PS8_FAULT_POINTER    = 0x101034C,
		};
		enum PFM_PPS_STAT
		{
			PFM_PPS1_READ_IOUT_VOUT    = 0x1011000,
			PFM_PPS2_READ_IOUT_VOUT    = 0x1011100,
			PFM_PPS3_READ_IOUT_VOUT    = 0x1011200,
			PFM_PPS4_READ_IOUT_VOUT    = 0x1011300,
			PFM_PPS5_READ_IOUT_VOUT    = 0x1011400,
			PFM_PPS6_READ_IOUT_VOUT    = 0x1011500,
			PFM_PPS7_READ_IOUT_VOUT    = 0x1011600,
			PFM_PPS8_READ_IOUT_VOUT    = 0x1011700,
			PFM_PPS1_STAT_SCALE_WORD   = 0x1011800,
			PFM_PPS2_STAT_SCALE_WORD   = 0x1011900,
			PFM_PPS3_STAT_SCALE_WORD   = 0x1011A00,
			PFM_PPS4_STAT_SCALE_WORD   = 0x1011B00,
			PFM_PPS5_STAT_SCALE_WORD   = 0x1011C00,
			PFM_PPS6_STAT_SCALE_WORD   = 0x1011D00,
			PFM_PPS7_STAT_SCALE_WORD   = 0x1011E00,
			PFM_PPS8_STAT_SCALE_WORD   = 0x1011F00,
			PFM_PPS1_STAT_VOUT         = 0x1012000,
			PFM_PPS2_STAT_VOUT         = 0x1012100,
			PFM_PPS3_STAT_VOUT         = 0x1012200,
			PFM_PPS4_STAT_VOUT         = 0x1012300,
			PFM_PPS5_STAT_VOUT         = 0x1012400,
			PFM_PPS6_STAT_VOUT         = 0x1012500,
			PFM_PPS7_STAT_VOUT         = 0x1012600,
			PFM_PPS8_STAT_VOUT         = 0x1012700,
		};
		enum PFM_S1PS_STAT
		{
			PFM_S1PS1_READ_IOUT_VOUT   = 0x1013000,
			PFM_S1PS2_READ_IOUT_VOUT   = 0x1013100,
			PFM_S1PS3_READ_IOUT_VOUT   = 0x1013200,
			PFM_S1PS4_READ_IOUT_VOUT   = 0x1013300,
			PFM_S1PS5_READ_IOUT_VOUT   = 0x1013400,
			PFM_S1PS6_READ_IOUT_VOUT   = 0x1013500,
			PFM_S1PS7_READ_IOUT_VOUT   = 0x1013600,
			PFM_S1PS8_READ_IOUT_VOUT   = 0x1013700,
			PFM_S1PS1_STAT_SCALE_WORD  = 0x1013800,
			PFM_S1PS2_STAT_SCALE_WORD  = 0x1013900,
			PFM_S1PS3_STAT_SCALE_WORD  = 0x1013A00,
			PFM_S1PS4_STAT_SCALE_WORD  = 0x1013B00,
			PFM_S1PS5_STAT_SCALE_WORD  = 0x1013C00,
			PFM_S1PS6_STAT_SCALE_WORD  = 0x1013D00,
			PFM_S1PS7_STAT_SCALE_WORD  = 0x1013E00,
			PFM_S1PS8_STAT_SCALE_WORD  = 0x1013F00,
			PFM_S1PS1_STAT_VOUT        = 0x1014000,
			PFM_S1PS2_STAT_VOUT        = 0x1014100,
			PFM_S1PS3_STAT_VOUT        = 0x1014200,
			PFM_S1PS4_STAT_VOUT        = 0x1014300,
			PFM_S1PS5_STAT_VOUT        = 0x1014400,
			PFM_S1PS6_STAT_VOUT        = 0x1014500,
			PFM_S1PS7_STAT_VOUT        = 0x1014600,
			PFM_S1PS8_STAT_VOUT        = 0x1014700,
		};
	}

	namespace ALPG
	{
		enum SEQUENCE_REG
		{
			_OPCODE                  = 0x2000000,
			_OPERAND                 = 0x2010000,
		};

		enum IDXRL_REG
		{
			_IDX1                    = 0x2020000,
			_IDX2                    = 0x2020004,
			_IDX3                    = 0x2020008,
			_IDX4                    = 0x202000C,
			_IDX5                    = 0x2020010,
			_IDX6                    = 0x2020014,
			_IDX7                    = 0x2020018,
			_IDX8                    = 0x202001C,
			_IDX9                    = 0x2020020,
			_IDX10                   = 0x2020024,
			_IDX11                   = 0x2020028,
			_IDX12                   = 0x202002C,
			_IDX13                   = 0x2020030,
			_IDX14                   = 0x2020034,
			_IDX15                   = 0x2020038,
			_IDX16                   = 0x202003C,
			_PG_START                = 0x2020040,
			_PG_CONTINUE             = 0x2020044,
			_PG_STOP                 = 0x2020048,
			_CFLAG                   = 0x202004C,
		};

		enum REFRESH_TIMER_REG
		{
			_START_ADDRESS           = 0x2020050,
			_ISP_REGISTER            = 0x2020054,
			_BAR_REGISTER            = 0x2020058,
			_TIMER                   = 0x2020060,
			_TREFRESH                = 0x2020064,
			_TPAUSE                  = 0x2020068,
			_TJET                    = 0x202006C,
			_RERESH_PAUSE_MODE       = 0x2020070,
			_MLOOP_REGISTER          = 0x2020074,
		};

		enum SEQUENCE_STATUS_REG
		{
			_SEQUENCE_STATUS         = 0x2030000,
			_BUMP_REGISTER           = 0x2030004,
		};

		enum ALPG_MODE_REG
		{
			_ALPG_MODE               = 0x3000000,
		};

		enum SET_DSET_REG
		{
			_DSET_INSTRUCTION        = 0x3010000,
		};

		enum ADDRESS_REG
		{
			_XBASE                   = 0x3020000,
			_YBASE                   = 0x3030000,
			_ZBASE                   = 0x3040000,
			_XCURRENT                = 0x3050000,
			_YCURRENT                = 0x3060000,
			_NB                      = 0x3070000,
			_D3D4                    = 0x3080000,
			_SOURCE                  = 0x3090000,
		};

		enum GROUP_AB_REG
		{
			_GROUPA_OPR              = 0x30A0000,
			_GROUPB_OPR              = 0x30B0000,
		};

		enum D1_D2_REG
		{
			_D1B                     = 0x3200000,
			_D1C                     = 0x3200004,
			_D1D                     = 0x3200008,
			_D1E                     = 0x320000C,
			_D1F                     = 0x3200010,
			_D1G                     = 0x3200014,
			_D1H                     = 0x3200018,
			_D2B                     = 0x320001C,
			_D2C                     = 0x3200020,
			_D2D                     = 0x3200024,
			_LMAX                    = 0x3200028,
			_HMAX                    = 0x320002C,
			_XMAX                    = 0x3200030,
			_YMAX                    = 0x3200034,
			_ZMAX                    = 0x3200038,
		};

		enum ADDRESS_DEFINE_REG
		{
			_ADDRESS_DEFINE0         = 0x3200080,
			_ADDRESS_DEFINE1         = 0x3200084,
			_ADDRESS_DEFINE2         = 0x3200088,
			_ADDRESS_DEFINE3         = 0x320008C,
			_ADDRESS_DEFINE4         = 0x3200090,
			_ADDRESS_DEFINE5         = 0x3200094,
			_ADDRESS_DEFINE6         = 0x3200098,
			_ADDRESS_DEFINE7         = 0x320009C,
			_ADDRESS_DEFINE8         = 0x32000A0,
			_ADDRESS_DEFINE9         = 0x32000A4,
			_ADDRESS_DEFINE10        = 0x32000A8,
		};

		enum ND_REGISTER
		{
			_ND2                     = 0x3200100,
			_ND3                     = 0x3200104,
			_ND4                     = 0x3200108,
			_ND5                     = 0x320010C,
			_ND6                     = 0x3200110,
			_ND7                     = 0x3200114,
			_ND8                     = 0x3200118,
			_ND9                     = 0x320011C,
			_ND10                    = 0x3200120,
			_ND11                    = 0x3200124,
			_ND12                    = 0x3200128,
			_ND13                    = 0x320012C,
			_ND14                    = 0x3200130,
			_ND15                    = 0x3200134,
			_ND16                    = 0x3200138,
			_ND17                    = 0x320013C,
			_ND18                    = 0x3200140,
			_ND19                    = 0x3200144,
			_ND20                    = 0x3200148,
			_ND21                    = 0x320014C,
			_ND22                    = 0x3200150,
			_ND23                    = 0x3200154,
			_ND24                    = 0x3200158,
			_ND25                    = 0x320015C,
			_ND26                    = 0x3200160,
			_ND27                    = 0x3200164,
			_ND28                    = 0x3200168,
			_ND29                    = 0x320016C,
			_ND30                    = 0x3200170,
			_ND31                    = 0x3200174,
			_ND32                    = 0x3200178,
		};

		enum BD_REGISTER
		{
			_BD2                     = 0x3200180,
			_BD3                     = 0x3200184,
			_BD4                     = 0x3200188,
			_BD5                     = 0x320018C,
			_BD6                     = 0x3200190,
			_BD7                     = 0x3200194,
			_BD8                     = 0x3200198,
			_BD9                     = 0x320019C,
			_BD10                    = 0x32001A0,
			_BD11                    = 0x32001A4,
			_BD12                    = 0x32001A8,
			_BD13                    = 0x32001AC,
			_BD14                    = 0x32001B0,
			_BD15                    = 0x32001B4,
			_BD16                    = 0x32001B8,
			_BD17                    = 0x32001BC,
			_BD18                    = 0x32001C0,
			_BD19                    = 0x32001C4,
			_BD20                    = 0x32001C8,
			_BD21                    = 0x32001CC,
			_BD22                    = 0x32001D0,
			_BD23                    = 0x32001D4,
			_BD24                    = 0x32001D8,
			_BD25                    = 0x32001DC,
			_BD26                    = 0x32001E0,
			_BD27                    = 0x32001E4,
			_BD28                    = 0x32001E8,
			_BD29                    = 0x32001EC,
			_BD30                    = 0x32001F0,
			_BD31                    = 0x32001F4,
			_BD32                    = 0x32001F8,
		};

		enum L1_L2_L3_L4_REG
		{
			_L1A                     = 0x3200200,
			_L1B                     = 0x3200204,
			_L1C                     = 0x3200208,
			_L1D                     = 0x320020C,
			_L1E                     = 0x3200210,
			_L1F                     = 0x3200214,
			_L1G                     = 0x3200218,
			_L1H                     = 0x320021C,
			_L2A                     = 0x3200220,
			_L2B                     = 0x3200224,
			_L2C                     = 0x3200228,
			_L2D                     = 0x320022C,
			_L2E                     = 0x3200230,
			_L2F                     = 0x3200234,
			_L2G                     = 0x3200238,
			_L2H                     = 0x320023C,
			_L3A                     = 0x3200240,
			_L3B                     = 0x3200244,
			_L3C                     = 0x3200248,
			_L3D                     = 0x320024C,
			_L3E                     = 0x3200250,
			_L3F                     = 0x3200254,
			_L3G                     = 0x3200258,
			_L3H                     = 0x320025C,
			_L4A                     = 0x3200260,
			_L4B                     = 0x3200264,
			_L4C                     = 0x3200268,
			_L4D                     = 0x320026C,
			_L4E                     = 0x3200270,
			_L4F                     = 0x3200274,
			_L4G                     = 0x3200278,
			_L4H                     = 0x320027C,
		};

		enum S1A_TEMP_REG
		{
			_S1A                     = 0x3200300,
			_S_TEMP                  = 0x3200304,
			_V1A                     = 0x3200310,
			_V_TEMP                  = 0x3200314,
		};

		enum MUT_REG
		{
			_MUT_CBIT                = 0x3300000,
			_MUT_RW                  = 0x3310000,
		};

		enum SCAN_REG
		{
			_SCAN_ADDRESS            = 0x4800000,
			_SCAN_DATA_b31_b0        = 0x4810000,
			_SCAN_DATA_b63_b32       = 0x4811000,
		};

		enum VECTOR_REG
		{
			_VECTOR_ADDRESS          = 0x4980000,
			_VECTOR_DATA1            = 0x4900000,
			_VECTOR_DATA2            = 0x4910000,
			_VECTOR_DATA3            = 0x4920000,
			_VECTOR_DATA4            = 0x4930000,
		};

		enum TP1_TP2_REG
		{
			_TP1_TP2                 = 0x3410000,
		};

		enum GROUP_CD_REG
		{
			_GROUP_C_OPR_b31_b0      = 0x3420000,
			_GROUP_C_OPR_b35_b32     = 0x3430000,
			_GROUP_D_OPR_b31_b0      = 0x3440000,
			_GROUP_D_OPR_b35_b32     = 0x3450000,
		};

		enum INV_HOLD_SOURCE_REG
		{
			_INV_HOLD_SOURCE_DATA    = 0x3460000,
		};

		enum UBM_REG
		{
			UBM_FC1_IOA_b31_b0       = 0x5000000,
			UBM_FC1_IOA_b47_b32      = 0x5010000,
			UBM_FC1_IOB_b31_b0       = 0x5020000,
			UBM_FC1_IOB_b47_b32      = 0x5030000,
			UBM_FC2_IOA_b31_b0       = 0x5040000,
			UBM_FC2_IOA_b47_b32      = 0x5050000,
			UBM_FC2_IOB_b31_b0       = 0x5060000,
			UBM_FC2_IOB_b47_b32      = 0x5070000,
		};

		enum CYCLE_PALETTE_REG
		{
			_CYP                     = 0x3800000,
		};

		enum PDS_REG
		{
			_PDS_PD_CH               = 0x3810000,
			_PDS_P_CH                = 0x3810100,
			_PDS_UCLK_CH             = 0x3810200,
			_DRE1                    = 0x3810400,
			_DRE2                    = 0x3810404,
			_DRE3                    = 0x3810408,
			_DRE4                    = 0x381040C,
			_DRE5                    = 0x3810410,
			_DRE6                    = 0x3810414,
			_DRE7                    = 0x3810418,
			_DRE8                    = 0x381041C,
			_CPE1                    = 0x3810420,
			_CPE2                    = 0x3810424,
			_CPE3                    = 0x3810428,
			_CPE4                    = 0x381042C,
			_CPE5                    = 0x3810430,
			_CPE6                    = 0x3810434,
			_CPE7                    = 0x3810438,
			_CPE8                    = 0x381043C,
			_RINV                    = 0x3810440,
			_FMA0                    = 0x3810450,
			_MUT0                    = 0x38104B0,
		};
		enum FMT_REG
		{
			_FPDCH3_0                = 0x3820000,
			_FPDCH7_4                = 0x3820004,
			_FPDCH11_8               = 0x3820008,
			_FPDCH15_12              = 0x382000C,
			_FPDCH19_16              = 0x3820010,
			_FPDCH23_20              = 0x3820014,
			_FPDCH27_24              = 0x3820018,
			_FPDCH31_28              = 0x382001C,
			_FPCH3_0                 = 0x3820100,
			_FPCH7_4                 = 0x3820104,
			_FPCH11_8                = 0x3820108,
			_FPCH15_12               = 0x382010C,
			_FPCH19_16               = 0x3820110,
			_FPCH23_20               = 0x3820114,
			_FPCH27_24               = 0x3820118,
			_FPCH31_28               = 0x382011C,
			_FPCH35_32               = 0x3820120,
			_FPCH39_36               = 0x3820124,
			_FPCH43_40               = 0x3820128,
			_FPCH47_44               = 0x382012C,

			_FDRE                    = 0x3820200,

			_FUCLKCH3_0              = 0x3820400,
			_FUCLKCH7_4              = 0x3820404,
			_FUCLKCH11_8             = 0x3820408,
			_FUCLKCH15_12            = 0x382040C,
			_FUCLKCH19_16            = 0x3820410,
			_FUCLKCH23_20            = 0x3820414,
			_FUCLKCH27_24            = 0x3820418,
			_FUCLKCH31_28            = 0x382041C,
			_FUCLKCH35_32            = 0x3820420,
			_FUCLKCH39_36            = 0x3820424,
			_FUCLKCH43_40            = 0x3820428,
			_FUCLKCH47_44            = 0x382042C,
			_FUCLKCH51_48            = 0x3820430,
			_FUCLKCH55_52            = 0x3820434,
			_FUCLKCH59_56            = 0x3820438,
			_FUCLKCH63_60            = 0x382043C,
			_FUCLKCH67_64            = 0x3820440,
			_FUCLKCH71_68            = 0x3820444,
			_FUCLKCH75_72            = 0x3820448,
			_FUCLKCH79_76            = 0x382044C,
			_FUCLKCH83_80            = 0x3820450,
			_FUCLKCH87_84            = 0x3820454,
		};

		enum RINV_REG
		{
			_RINV_PD_CH             = 0x3830200,
			_RINV_P_CH_b31_b0       = 0x3830210,
			_RINV_P_CH_b47_b32      = 0x3830214,
			_RINV_UCLK_CH_b31_b0    = 0x3830220,
			_RINV_UCLK_CH_b63_b32   = 0x3830224,
			_RINV_UCLK_CH_b87_b64   = 0x3830228,
		};

		enum DRE_CPE_SEL_REG
		{
			_DRE_SEL                = 0x3850000,
			_HCPE_SEL               = 0x3850100,
			_LCPE_SEL               = 0x3850200,
		};

		enum PATTERN_DELAY_REG
		{
			_DRDLY                  = 0x3870000,
			_IODLY                  = 0x3870100,
			_DREDLY                 = 0x3870300,
			_CPEDLY                 = 0x3870500,
			_UCLKDLY                = 0x3870600,
		};

		enum DMA
		{
			_DMA_ADDRESS          = 0x6FF0000,
			_DMA_DATA             = 0x7000000,
			//_DMA_DBM_ADDRESS      = 0x100000000LL,
		};

		enum DBMAP_REG
		{
			_DBMAP                = 0x3500000,
		};

		enum RESULT_REG
		{
			_IO_RESULT_R_FC1_IOA_b31_b0    = 0x3890000,
			_IO_RESULT_R_FC1_IOA_b47_b32   = 0x3890004,
			_IO_RESULT_R_FC1_IOB_b31_b0    = 0x3890008,
			_IO_RESULT_R_FC1_IOB_b47_b32   = 0x389000C,
			_IO_RESULT_R_FC2_IOA_b31_b0    = 0x3890010,
			_IO_RESULT_R_FC2_IOA_b47_b32   = 0x3890014,
			_IO_RESULT_R_FC2_IOB_b31_b0    = 0x3890018,
			_IO_RESULT_R_FC2_IOB_b47_b32   = 0x389001C,
			_DRV_RESULT_R                  = 0x3890020,
			_UCLK_RESULT_R_b31_b0          = 0x3890024,
			_UCLK_RESULT_R_b63_b32         = 0x3890028,
			_UCLK_RESULT_R_b87_b64         = 0x389002C,
			_READ_MASK_W_FC1_IOA_b31_b0    = 0x3890030,
			_READ_MASK_W_FC1_IOA_b47_b32   = 0x3890034,
			_READ_MASK_W_FC1_IOB_b31_b0    = 0x3890038,
			_READ_MASK_W_FC1_IOB_b47_b32   = 0x389003C,
			_READ_MASK_W_FC2_IOA_b31_b0    = 0x3890040,
			_READ_MASK_W_FC2_IOA_b47_b32   = 0x3890044,
			_READ_MASK_W_FC2_IOB_b31_b0    = 0x3890048,
			_READ_MASK_W_FC2_IOB_b47_b32   = 0x389004C,
			_READ_MASK_W_FC_DRV_b31_b0     = 0x3890050,
			_READ_MASK_W_FC1_IOA_b32_b0    = 0x3890054,
			_READ_MASK_W_FC1_IOA_b63_b32   = 0x3890058,
			_READ_MASK_W_FC1_IOA_b87_b64   = 0x389005C,
			_IO_CAL_READ_C1_CAL0           = 0x3890060,
			_IO_CAL_READ_C1_CAL1           = 0x3890064,
			_IO_CAL_READ_C2_CAL0           = 0x3890068,
			_IO_CAL_READ_C3_CAL0           = 0x389006C,
			_IO_CAL_READ_C3_CAL1           = 0x3890070,
			_FM_SCAN_MASK_CLEAR            = 0x3890080,
		};

		enum IO_MASK_REG
		{
			_IO_MASK_MEM_0                 = 0x3890500,
			_IO_MASK_MEM_1                 = 0x3890504,
			_IO_MASK_MEM_2                 = 0x3890508,
			_IO_MASK_MEM_3                 = 0x389050C,
			_IO_MASK_MEM_4                 = 0x3890510,
			_IO_MASK_MEM_5                 = 0x3890514,
			_IO_MASK_MEM_6                 = 0x3890518,
			_IO_MASK_MEM_7                 = 0x389051C,
			_IO_MASK_MEM_8                 = 0x3890520,
			_IO_MASK_MEM_9                 = 0x3890524,
			_IO_MASK_MEM_10                = 0x3890528,
			_IO_MASK_MEM_11                = 0x389052C,
			_IO_MASK_MEM_12                = 0x3890530,
			_IO_MASK_MEM_13                = 0x3890534,
			_IO_MASK_MEM_14                = 0x3890538,
			_IO_MASK_MEM_15                = 0x389053C,
		};

		enum LATCH_DATA_MEMORY
		{
			_FC1_REAL_SCAN                 = 0x3890600,
			_FC1_R_IOAB_0_15               = 0x3890604,
			_FC1_R_IOAB_16_31              = 0x3890608,
			_FC1_R_IOAB_32_47              = 0x389060C,
			_FC1_F_IOAB_0_15               = 0x3890610,
			_FC1_F_IOAB_16_31              = 0x3890614,
			_FC1_F_IOAB_32_47              = 0x3890618,
			_FC2_REAL_SCAN                 = 0x3890620,
			_FC2_R_IOAB_0_15               = 0x3890624,
			_FC2_R_IOAB_16_31              = 0x3890628,
			_FC2_R_IOAB_32_47              = 0x389062C,
			_FC2_F_IOAB_0_15               = 0x3890630,
			_FC2_F_IOAB_16_31              = 0x3890634,
			_FC2_F_IOAB_32_47              = 0x3890638,
		};

		enum FM_SCAN_MASK
		{
			_FC1_SCAN0_IOAB_0_15           = 0x38A0000,
			_FC1_SCAN0_IOAB_16_31          = 0x38A0004,
			_FC1_SCAN0_IOAB_32_47          = 0x38A0008,
			//.....
			_FC1_SCAN255_IOAB_0_15         = 0x38A0FF0,
			_FC1_SCAN255_IOAB_16_31        = 0x38A0FF4,
			_FC1_SCAN255_IOAB_32_47        = 0x38A0FF8,

			_FC2_SCAN0_IOAB_0_15           = 0x38A1000,
			_FC2_SCAN0_IOAB_16_31          = 0x38A1004,
			_FC2_SCAN0_IOAB_32_47          = 0x38A1008,
			//.....
			_FC2_SCAN255_IOAB_0_15         = 0x38A1FF0,
			_FC2_SCAN255_IOAB_16_31        = 0x38A1FF4,
			_FC2_SCAN255_IOAB_32_47        = 0x38A1FF8,
		};

		enum DIAGNOSTIC
		{
			_DLE                           = 0x3900000,
			_WCS_ADDR                      = 0x3910000,
			_DIAG_DRE                      = 0x3920000,
			_DIAG_CPE                      = 0x3930000,
			_H_EDGE_PD                     = 0x3940000,
			_L_EDGE_PD                     = 0x3950000,
			_H_EDGE_P_0                    = 0x3960000,
			_H_EDGE_P_1                    = 0x3970000,
			_L_EDGE_P_0                    = 0x3980000,
			_L_EDGE_P_1                    = 0x3990000,
			_H_EDGE_UCLK_0                 = 0x39A0000,
			_H_EDGE_UCLK_1                 = 0x39B0000,
			_H_EDGE_UCLK_2                 = 0x39C0000,
			_L_EDGE_UCLK_0                 = 0x39D0000,
			_L_EDGE_UCLK_1                 = 0x39E0000,
			_L_EDGE_UCLK_2                 = 0x39F0000,
			_DIAG_CYP                      = 0x3A00000,
			_DIAG_TS                       = 0x3A10000,
		};

		enum TIMESET_REG
		{
			_TIMESET              = 0x3F00000,
		};

		enum RATE_DRELT_BCLK_REG
		{
			_RATE                 = 0x6000000,
			_DREL1                = 0x6000100,
			_DRET1                = 0x6000180,
			_BCLK1                = 0x6000200,
			_CCLK1                = 0x6000280,
			_BCLK2                = 0x6000300,
			_CCLK2                = 0x6000380,
			_BCLK3                = 0x6000400,
			_CCLK3                = 0x6000480,
			_BCLK4                = 0x6000500,
			_CCLK4                = 0x6000580,
			_BCLK5                = 0x6000600,
			_CCLK5                = 0x6000680,
			_BCLK6                = 0x6000700,
			_CCLK6                = 0x6000780,
			_BCLK7                = 0x6000800,
			_CCLK7                = 0x6000880,
			_BCLK8                = 0x6000900,
			_CCLK8                = 0x6000980,
			_BCLK9                = 0x6000A00,
			_CCLK9                = 0x6000A80,
			_BCLK10               = 0x6000B00,
			_CCLK10               = 0x6000B80,
			_STRB1                = 0x6000C00,
			_STRB2                = 0x6000C80,
			_STRB3                = 0x6000D00,
			_STRB4                = 0x6000D80,
			_STRB5                = 0x6000E00,
			_STRB6                = 0x6000E80,
			_STRB7                = 0x6000F00,
			_STRB8                = 0x6000F80,
			_RSTRB1               = 0x6001000,
			_RSTRB2               = 0x6001080,
			_RSTRB3               = 0x6001180,
			_RSTRB4               = 0x6001180,
			_RSTRB5               = 0x6001280,
			_RSTRB6               = 0x6001280,
			_RSTRB7               = 0x6001380,
			_RSTRB8               = 0x6001380,
		};

		enum DELAY_OFFSET_REG
		{
			_BCLK1_OFFSET        = 0x6020000,
			_BCLK2_OFFSET        = 0x6020004,
			_BCLK3_OFFSET        = 0x6020008,
			_BCLK4_OFFSET        = 0x602000C,
			_BCLK5_OFFSET        = 0x6020010,
			_BCLK6_OFFSET        = 0x6020014,
			_BCLK7_OFFSET        = 0x6020018,
			_BCLK8_OFFSET        = 0x602001C,
			_BCLK9_OFFSET        = 0x6020020,
			_BCLK10_OFFSET       = 0x6020024,
			_DRELT1_OFFSET       = 0x6020028,
			_STRB1_2_OFFSET      = 0x6020030,
			_STRB3_4_OFFSET      = 0x6020034,
			_STRB5_6_OFFSET      = 0x6020038,
			_STRB7_8_OFFSET      = 0x602003C,
			_RSTRB1_2_OFFSET     = 0x6020040,
			_RSTRB3_4_OFFSET     = 0x6020044,
			_RSTRB5_6_OFFSET     = 0x6020048,
			_RSTRB7_8_OFFSET     = 0x602004C,
			_R1_OFFSET           = 0x6020080,
			_R2_OFFSET           = 0x6020084,
			_R3_OFFSET           = 0x6020088,
			_R4_OFFSET           = 0x602008C,
			_R5_OFFSET           = 0x6020090,
			_R6_OFFSET           = 0x6020094,
			_R7_OFFSET           = 0x6020098,
			_R8_OFFSET           = 0x602009C,
			_R9_OFFSET           = 0x60200A0,
			_R10_OFFSET          = 0x60200A4,
			_R11_OFFSET          = 0x60200A8,
			_R12_OFFSET          = 0x60200AC,
			_R13_OFFSET          = 0x60200B0,
			_R14_OFFSET          = 0x60200B4,
			_R15_OFFSET          = 0x60200B8,
			_R16_OFFSET          = 0x60200BC,
		};
	}
	namespace DMA_MEMORY_ADDRESS
	{
		const unsigned long int DBM_MEM_ADDRESS = 0x100000000LL;
		const unsigned long int DFM_MEM_ADDRESS = 0x200000000LL;
	}
}

#endif
