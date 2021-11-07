//  This file has been generated using multiprotocol_diy.py
//  This file must be #include in multiprotocols_diy.h

#pragma once

//  typedefs
#include <stdint.h>

//  replicate from multiprotocols_diy.cpp with prefix (to avoid ambiguity) so referenceable by multiprotocols.cpp
enum
{
	MM_OPTION_NONE,
	MM_OPTION_OPTION,
	MM_OPTION_RFTUNE,
	MM_OPTION_VIDFREQ,
	MM_OPTION_FIXEDID,
	MM_OPTION_TELEM,
	MM_OPTION_SRVFREQ,
	MM_OPTION_MAXTHR,
	MM_OPTION_RFCHAN,
	MM_OPTION_RFPOWER,
	MM_OPTION_WBUS,
	MM_OPTION_COUNT
};

//  force all subTypes for FRSKY RX protocol. Need to filter when used.
#define SEND_CPPM

//  force all protocol definitions to be loaded
#define MULTI_CONFIG_INO
#define ASSAN_NRF24L01_INO
#define BAYANG_NRF24L01_INO
#define BAYANG_RX_NRF24L01_INO
#define BUGS_A7105_INO
#define BUGSMINI_NRF24L01_INO
#define CABELL_NRF24L01_INO
#define CFLIE_NRF24L01_INO
#define CG023_NRF24L01_INO
#define CORONA_CC2500_INO
#define CX10_NRF24L01_INO
#define DEVO_CYRF6936_INO
#define DM002_NRF24L01_INO
#define DSM_CYRF6936_INO
#define DSM_RX_CYRF6936_INO
#define E010R5_CYRF6936_INO
#define E016H_NRF24L01_INO
#define E016HV2_CC2500_INO
#define E01X_NRF24L01_INO
#define E129_CYRF6936_INO
#define ESKY_NRF24L01_INO
#define ESKY150_NRF24L01_INO
#define ESKY150V2_CC2500_INO
#define FLYSKY_A7105_INO
#define AFHDS2A_A7105_INO
#define AFHDS2A_RX_A7105_INO
#define FQ777_NRF24L01_INO
#define FRSKY_RX_CC2500_INO
#define FRSKYD_CC2500_INO
#define FRSKYV_CC2500_INO
#define FRSKYX_CC2500_INO
#define FRSKYL_CC2500_INO
#define FRSKYR9_SX1276_INO
#define FUTABA_CC2500_INO
#define FX816_NRF24L01_INO
#define FY326_NRF24L01_INO
#define GD00X_CCNRF_INO
#define GW008_NRF24L01_INO
#define H8_3D_NRF24L01_INO
#define HEIGHT_A7105_INO
#define HISKY_NRF24L01_INO
#define HITEC_CC2500_INO
#define HONTAI_NRF24L01_INO
#define HOTT_CC2500_INO
#define HUBSAN_A7105_INO
#define IKEAANSLUTA_CC2500_INO
#define J6PRO_CYRF6936_INO
#define JJRC345_NRF24L01_INO
#define JOYSWAY_A7105_INO
#define KF606_CCNRF_INO
#define KN_NRF24L01_INO
#define KYOSHO_A7105_INO
#define LOLI_NRF24L01_INO
#define LOSI_CYRF6936_INO
#define MJXQ_CCNRF_INO
#define MLINK_CYRF6936_INO
#define MOULDKG_NRF24L01_INO
#define MT99XX_CCNRF_INO
#define NCC1701_NRF24L01_INO
#define OMP_CCNRF_INO
#define PELIKAN_A7105_INO
#define POTENSIC_NRF24L01_INO
#define PROPEL_NRF24L01_INO
#define CX10_NRF24L01_INO
#define Q303_CCNRF_INO
#define Q90C_CCNRF_INO
#define RLINK_CC2500_INO
#define REALACC_NRF24L01_INO
#define REDPINE_CC2500_INO
#define SCANNER_CC2500_INO
#define SHENQI_NRF24L01_INO
#define SKYARTEC_CC2500_INO
#define SLT_CCNRF_INO
#define SYMAX_NRF24L01_INO
#define TIGER_NRF24L01_INO
#define TRAXXAS_CYRF6936_INO
#define V2X2_NRF24L01_INO
#define V761_NRF24L01_INO
#define V911S_CCNRF_INO
#define WK2x01_CYRF6936_INO
#define WFLY_CYRF6936_INO
#define WFLY2_A7105_INO
#define XERALL_NRF24L01_INO
#define XK_CCNRF_INO
#define XN297DUMP_NRF24L01_INO
#define YD717_NRF24L01_INO
#define ZSX_NRF24L01_INO
#define TEST_CC2500_INO
#define NANORF_NRF24L01_INO

//  dummy functions
#define CONFIG_init   nullptr
#define CONFIG_callback   nullptr
#define ASSAN_init   nullptr
#define ASSAN_callback   nullptr
#define BAYANG_init   nullptr
#define BAYANG_callback   nullptr
#define BAYANG_RX_init   nullptr
#define BAYANG_RX_callback   nullptr
#define BUGS_init   nullptr
#define BUGS_callback   nullptr
#define BUGSMINI_init   nullptr
#define BUGSMINI_callback   nullptr
#define CABELL_init   nullptr
#define CABELL_callback   nullptr
#define CFLIE_init   nullptr
#define CFLIE_callback   nullptr
#define CG023_init   nullptr
#define CG023_callback   nullptr
#define CORONA_init   nullptr
#define CORONA_callback   nullptr
#define CX10_init   nullptr
#define CX10_callback   nullptr
#define DEVO_init   nullptr
#define DEVO_callback   nullptr
#define DM002_init   nullptr
#define DM002_callback   nullptr
#define DSM_init   nullptr
#define DSM_callback   nullptr
#define DSM_RX_init   nullptr
#define DSM_RX_callback   nullptr
#define E010R5_init   nullptr
#define E010R5_callback   nullptr
#define E016H_init   nullptr
#define E016H_callback   nullptr
#define E016HV2_init   nullptr
#define E016HV2_callback   nullptr
#define E01X_init   nullptr
#define E01X_callback
#define E129_init   nullptr
#define E129_callback   nullptr
#define ESKY_init   nullptr
#define ESKY_callback   nullptr
#define ESKY150_init   nullptr
#define ESKY150_callback   nullptr
#define ESKY150V2_init   nullptr
#define ESKY150V2_callback   nullptr
#define FLYSKY_init   nullptr
#define FLYSKY_callback   nullptr
#define AFHDS2A_init   nullptr
#define AFHDS2A_callback   nullptr
#define AFHDS2A_RX_init   nullptr
#define AFHDS2A_RX_callback   nullptr
#define FQ777_init   nullptr
#define FQ777_callback   nullptr
#define FRSKY_RX_init   nullptr
#define FRSKY_RX_callback   nullptr
#define FRSKYD_init   nullptr
#define FRSKYD_callback   nullptr
#define FRSKYV_init   nullptr
#define FRSKYV_callback   nullptr
#define FRSKYX_init   nullptr
#define FRSKYX_callback   nullptr
#define FRSKYX_init   nullptr
#define FRSKYX_callback   nullptr
#define FRSKYL_init   nullptr
#define FRSKYL_callback   nullptr
#define FRSKYR9_init   nullptr
#define FRSKYR9_callback   nullptr
#define SFHSS_init   nullptr
#define SFHSS_callback   nullptr
#define FX816_init   nullptr
#define FX816_callback   nullptr
#define FY326_init   nullptr
#define FY326_callback   nullptr
#define GD00X_init   nullptr
#define GD00X_callback   nullptr
#define GW008_init   nullptr
#define GW008_callback   nullptr
#define H8_3D_init   nullptr
#define H8_3D_callback   nullptr
#define HEIGHT_init   nullptr
#define HEIGHT_callback   nullptr
#define HISKY_init   nullptr
#define HISKY_callback   nullptr
#define HITEC_init   nullptr
#define HITEC_callback   nullptr
#define HONTAI_init   nullptr
#define HONTAI_callback   nullptr
#define HOTT_init   nullptr
#define HOTT_callback   nullptr
#define HUBSAN_init   nullptr
#define HUBSAN_callback   nullptr
#define IKEAANSLUTA_init   nullptr
#define IKEAANSLUTA_callback   nullptr
#define J6PRO_init   nullptr
#define J6PRO_callback   nullptr
#define JJRC345_init   nullptr
#define JJRC345_callback   nullptr
#define JOYSWAY_init   nullptr
#define JOYSWAY_callback   nullptr
#define KF606_init   nullptr
#define KF606_callback   nullptr
#define KN_init   nullptr
#define KN_callback   nullptr
#define KYOSHO_init   nullptr
#define KYOSHO_callback   nullptr
#define LOLI_init   nullptr
#define LOLI_callback   nullptr
#define LOSI_init   nullptr
#define LOSI_callback   nullptr
#define MJXQ_init   nullptr
#define MJXQ_callback   nullptr
#define MLINK_init   nullptr
#define MLINK_callback   nullptr
#define MOULDKG_init   nullptr
#define MOULDKG_callback   nullptr
#define MT99XX_init   nullptr
#define MT99XX_callback   nullptr
#define NCC_init   nullptr
#define NCC_callback   nullptr
#define OMP_init   nullptr
#define OMP_callback   nullptr
#define PELIKAN_init   nullptr
#define PELIKAN_callback   nullptr
#define POTENSIC_init   nullptr
#define POTENSIC_callback   nullptr
#define PROPEL_init   nullptr
#define PROPEL_callback   nullptr
#define CX10_init   nullptr
#define CX10_callback   nullptr
#define Q303_init   nullptr
#define Q303_callback   nullptr
#define Q90C_init   nullptr
#define Q90C_callback   nullptr
#define RLINK_init   nullptr
#define RLINK_callback   nullptr
#define REALACC_init   nullptr
#define REALACC_callback   nullptr
#define REDPINE_init   nullptr
#define REDPINE_callback   nullptr
#define SCANNER_init   nullptr
#define SCANNER_callback   nullptr
#define SHENQI_init   nullptr
#define SHENQI_callback   nullptr
#define SKYARTEC_init   nullptr
#define SKYARTEC_callback   nullptr
#define SLT_init   nullptr
#define SLT_callback   nullptr
#define SYMAX_init   nullptr
#define SYMAX_callback   nullptr
#define TIGER_init   nullptr
#define TIGER_callback   nullptr
#define TRAXXAS_init   nullptr
#define TRAXXAS_callback   nullptr
#define V2X2_init   nullptr
#define V2X2_callback   nullptr
#define V761_init   nullptr
#define V761_callback   nullptr
#define V911S_init   nullptr
#define V911S_callback   nullptr
#define WK_init   nullptr
#define WK_callback   nullptr
#define WFLY_init   nullptr
#define WFLY_callback   nullptr
#define WFLY2_init   nullptr
#define WFLY2_callback   nullptr
#define XERALL_init   nullptr
#define XERALL_callback   nullptr
#define XK_init   nullptr
#define XK_callback   nullptr
#define XN297Dump_init   nullptr
#define XN297Dump_callback   nullptr
#define YD717_init   nullptr
#define YD717_callback   nullptr
#define ZSX_init   nullptr
#define ZSX_callback   nullptr
#define TEST_init   nullptr
#define TEST_callback   nullptr
#define NANORF_init   nullptr
#define NANORF_callback   nullptr
