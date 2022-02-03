/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

//  setup the compile environment since we are using part of the project
#include "multiprotocols_diy_inc.h"

/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Multiprotocol is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Multiprotocol.  If not, see <http://www.gnu.org/licenses/>.
 */

#define MODE_SERIAL 0

//******************
// Protocols
//******************
enum PROTOCOLS
{
	PROTO_PROTOLIST	= 0,	// NO RF
	PROTO_FLYSKY 	= 1,	// =>A7105
	PROTO_HUBSAN	= 2,	// =>A7105
	PROTO_FRSKYD	= 3,	// =>CC2500
	PROTO_HISKY		= 4,	// =>NRF24L01
	PROTO_V2X2		= 5,	// =>NRF24L01
	PROTO_DSM		= 6,	// =>CYRF6936
	PROTO_DEVO		= 7,	// =>CYRF6936
	PROTO_YD717		= 8,	// =>NRF24L01
	PROTO_KN		= 9,	// =>NRF24L01
	PROTO_SYMAX		= 10,	// =>NRF24L01
	PROTO_SLT		= 11,	// =>NRF24L01
	PROTO_CX10		= 12,	// =>NRF24L01
	PROTO_CG023		= 13,	// =>NRF24L01
	PROTO_BAYANG	= 14,	// =>NRF24L01
	PROTO_FRSKYX	= 15,	// =>CC2500
	PROTO_ESKY		= 16,	// =>NRF24L01
	PROTO_MT99XX	= 17,	// =>NRF24L01
	PROTO_MJXQ		= 18,	// =>NRF24L01
	PROTO_SHENQI	= 19,	// =>NRF24L01
	PROTO_FY326		= 20,	// =>NRF24L01
	PROTO_FUTABA	= 21,	// =>CC2500
	PROTO_J6PRO		= 22,	// =>CYRF6936
	PROTO_FQ777		= 23,	// =>NRF24L01
	PROTO_ASSAN		= 24,	// =>NRF24L01
	PROTO_FRSKYV	= 25,	// =>CC2500
	PROTO_HONTAI	= 26,	// =>NRF24L01
	PROTO_OPENLRS	= 27,	// =>OpenLRS hardware
	PROTO_AFHDS2A	= 28,	// =>A7105
	PROTO_Q2X2		= 29,	// =>NRF24L01, extension of CX-10 protocol
	PROTO_WK2x01	= 30,	// =>CYRF6936
	PROTO_Q303		= 31,	// =>NRF24L01
	PROTO_GW008		= 32,	// =>NRF24L01
	PROTO_DM002		= 33,	// =>NRF24L01
	PROTO_CABELL	= 34,	// =>NRF24L01
	PROTO_ESKY150	= 35,	// =>NRF24L01
	PROTO_H8_3D		= 36,	// =>NRF24L01
	PROTO_CORONA	= 37,	// =>CC2500
	PROTO_CFLIE     = 38,   // =>NRF24L01
	PROTO_HITEC     = 39,   // =>CC2500
	PROTO_WFLY		= 40,	// =>CYRF6936
	PROTO_BUGS		= 41,	// =>A7105
	PROTO_BUGSMINI	= 42,	// =>NRF24L01
	PROTO_TRAXXAS	= 43,	// =>CYRF6936
	PROTO_NCC1701	= 44,	// =>NRF24L01
	PROTO_E01X		= 45,	// =>NRF24L01
	PROTO_V911S		= 46,	// =>NRF24L01
	PROTO_GD00X		= 47,	// =>NRF24L01
	PROTO_V761		= 48,	// =>NRF24L01
	PROTO_KF606		= 49,	// =>NRF24L01
	PROTO_REDPINE	= 50,	// =>CC2500
	PROTO_POTENSIC	= 51,	// =>NRF24L01
	PROTO_ZSX		= 52,	// =>NRF24L01
	PROTO_HEIGHT	= 53,	// =>A7105
	PROTO_SCANNER	= 54,	// =>CC2500
	PROTO_FRSKY_RX	= 55,	// =>CC2500
	PROTO_AFHDS2A_RX= 56,	// =>A7105
	PROTO_HOTT		= 57,	// =>CC2500
	PROTO_FX816		= 58,	// =>NRF24L01
	PROTO_BAYANG_RX	= 59,	// =>NRF24L01
	PROTO_PELIKAN	= 60,	// =>A7105
	PROTO_TIGER		= 61,	// =>NRF24L01
	PROTO_XK		= 62,	// =>NRF24L01
	PROTO_XN297DUMP	= 63,	// =>NRF24L01
	PROTO_FRSKYX2	= 64,	// =>CC2500
	PROTO_FRSKY_R9	= 65,	// =>SX1276
	PROTO_PROPEL	= 66,	// =>NRF24L01
	PROTO_FRSKYL	= 67,	// =>CC2500
	PROTO_SKYARTEC	= 68,	// =>CC2500
	PROTO_ESKY150V2	= 69,	// =>CC2500+NRF24L01
	PROTO_DSM_RX	= 70,	// =>CYRF6936
	PROTO_JJRC345	= 71,	// =>NRF24L01
	PROTO_Q90C		= 72,	// =>NRF24L01 or CC2500
	PROTO_KYOSHO	= 73,	// =>A7105
	PROTO_RLINK		= 74,	// =>CC2500
	PROTO_REALACC	= 76,	// =>NRF24L01
	PROTO_OMP		= 77,	// =>CC2500 & NRF24L01
	PROTO_MLINK		= 78,	// =>CYRF6936
	PROTO_WFLY2		= 79,	// =>A7105
	PROTO_E016HV2	= 80,	// =>CC2500 & NRF24L01
	PROTO_E010R5	= 81,	// =>CYRF6936
	PROTO_LOLI		= 82,	// =>NRF24L01
	PROTO_E129		= 83,	// =>CYRF6936
	PROTO_JOYSWAY	= 84,	// =>A7105
	PROTO_E016H		= 85,	// =>NRF24L01
	PROTO_CONFIG	= 86,	// Module config
	PROTO_IKEAANSLUTA = 87, // =>CC2500
	PROTO_WILLIFM	= 88,	// 27/35ab/40/41/72 MHz module external project
	PROTO_LOSI		= 89,	// =>CYRF6936
	PROTO_MOULDKG	= 90,	// =>NRF24L01
	PROTO_XERALL	= 91,	// =>NRF24L01

	PROTO_NANORF	= 126,	// =>NRF24L01
	PROTO_TEST		= 127,	// =>CC2500
};

//PPM protocols
struct PPM_Parameters
{
	uint8_t protocol;
	uint8_t sub_proto	: 3;
	uint8_t rx_num		: 6;
	uint8_t power		: 1;
	uint8_t autobind	: 1;
	int8_t option;
	uint32_t chan_order;
};

//Callback
typedef uint16_t (*uint16_function_t) (void);	//pointer to a function with no parameters which return an uint16_t integer
typedef void     (*void_function_t  ) (void);	//pointer to a function with no parameters which returns nothing

struct __attribute__((__packed__)) mm_protocol_definition {
	uint8_t protocol;
	const char *ProtoString;
	const char *SubProtoString;
	uint8_t nbrSubProto        : 4;
	uint8_t optionType         : 4;
	uint8_t failSafe           : 1;
	uint8_t chMap              : 1;
	uint8_t rfSwitch           : 2;
	void_function_t		Init;
	uint16_function_t	CallBack;
};

extern const mm_protocol_definition multi_protocols[];

enum RF_SWITCH
{
	SW_A7105	= 0,	//antenna RF1
	SW_CC2500	= 1,	//antenna RF2
	SW_NRF		= 2,	//antenna RF3
	SW_CYRF		= 3,	//antenna RF4
};
