//  Copyright (c) 2014 Jakub Filipowicz <jakubf@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "parser.h"
#include "dh.h"
#include "keywords.h"

struct dh_table *keywords;

#define PRAGMA_ADD(name, tok) if (dh_addv(keywords, name, tok, 0) < 0) return -1;
#define OP_ADD(name, tok, opcode) if (dh_addv(keywords, name, tok, opcode) < 0) return -1;

// -----------------------------------------------------------------------
int kw_init()
{
	keywords = dh_create(1024, 0);

	if (!keywords) return -1;

	PRAGMA_ADD(".cpu", P_CPU);
	PRAGMA_ADD(".file", P_FILE);
	PRAGMA_ADD(".line", P_LINE); // handled in lexer
	PRAGMA_ADD(".include", P_INCLUDE); // handled in lexer
	PRAGMA_ADD(".equ", P_EQU);
	PRAGMA_ADD(".const", P_CONST);
	PRAGMA_ADD(".lbyte", P_LBYTE);
	PRAGMA_ADD(".rbyte", P_RBYTE);
	PRAGMA_ADD(".word", P_WORD);
	PRAGMA_ADD(".dword", P_DWORD);
	PRAGMA_ADD(".float", P_FLOAT);
	PRAGMA_ADD(".ascii", P_ASCII);
	PRAGMA_ADD(".asciiz", P_ASCIIZ);
	PRAGMA_ADD(".res", P_RES);
	PRAGMA_ADD(".org", P_ORG);
	PRAGMA_ADD(".entry", P_ENTRY);
	PRAGMA_ADD(".global", P_GLOBAL);

	OP_ADD("LW", OP_RN, 0b0100000000000000);
	OP_ADD("TW", OP_RN, 0b0100010000000000);
	OP_ADD("LS", OP_RN, 0b0100100000000000);
	OP_ADD("RI", OP_RN, 0b0100110000000000);
	OP_ADD("RW", OP_RN, 0b0101000000000000);
	OP_ADD("PW", OP_RN, 0b0101010000000000);
	OP_ADD("RJ", OP_RN, 0b0101100000000000);
	OP_ADD("IS", OP_RN, 0b0101110000000000);
	OP_ADD("BB", OP_RN, 0b0110000000000000);
	OP_ADD("BM", OP_RN, 0b0110010000000000);
	OP_ADD("BS", OP_RN, 0b0110100000000000);
	OP_ADD("BC", OP_RN, 0b0110110000000000);
	OP_ADD("BN", OP_RN, 0b0111000000000000);
	OP_ADD("OU", OP_RN, 0b0111010000000000);
	OP_ADD("IN", OP_RN, 0b0111100000000000);

	OP_ADD("AD", OP_N, 0b0111110000000000);
	OP_ADD("SD", OP_N, 0b0111110001000000);
	OP_ADD("MW", OP_N, 0b0111110010000000);
	OP_ADD("DW", OP_N, 0b0111110011000000);
	OP_ADD("AF", OP_N, 0b0111110100000000);
	OP_ADD("SF", OP_N, 0b0111110101000000);
	OP_ADD("MF", OP_N, 0b0111110110000000);
	OP_ADD("DF", OP_N, 0b0111110111000000);

	OP_ADD("AW", OP_RN, 0b1000000000000000);
	OP_ADD("AC", OP_RN, 0b1000010000000000);
	OP_ADD("SW", OP_RN, 0b1000100000000000);
	OP_ADD("CW", OP_RN, 0b1000110000000000);
	OP_ADD("OR", OP_RN, 0b1001000000000000);
	OP_ADD("OM", OP_RN, 0b1001010000000000);
	OP_ADD("NR", OP_RN, 0b1001100000000000);
	OP_ADD("NM", OP_RN, 0b1001110000000000);
	OP_ADD("ER", OP_RN, 0b1010000000000000);
	OP_ADD("EM", OP_RN, 0b1010010000000000);
	OP_ADD("XR", OP_RN, 0b1010100000000000);
	OP_ADD("XM", OP_RN, 0b1010110000000000);
	OP_ADD("CL", OP_RN, 0b1011000000000000);
	OP_ADD("LB", OP_RN, 0b1011010000000000);
	OP_ADD("RB", OP_RN, 0b1011100000000000);
	OP_ADD("CB", OP_RN, 0b1011110000000000);

	OP_ADD("AWT", OP_RT, 0b1100000000000000);
	OP_ADD("TRB", OP_RT, 0b1100010000000000);
	OP_ADD("IRB", OP_RT, 0b1100100000000000);
	OP_ADD("DRB", OP_RT, 0b1100110000000000);
	OP_ADD("CWT", OP_RT, 0b1101000000000000);
	OP_ADD("LWT", OP_RT, 0b1101010000000000);
	OP_ADD("LWS", OP_RT, 0b1101100000000000);
	OP_ADD("RWS", OP_RT, 0b1101110000000000);

	OP_ADD("UJS", OP_T, 0b1110000000000000);
	OP_ADD("JLS", OP_T, 0b1110000001000000);
	OP_ADD("JES", OP_T, 0b1110000010000000);
	OP_ADD("JGS", OP_T, 0b1110000011000000);
	OP_ADD("JVS", OP_T, 0b1110000100000000);
	OP_ADD("JXS", OP_T, 0b1110000101000000);
	OP_ADD("JYS", OP_T, 0b1110000110000000);
	OP_ADD("JCS", OP_T, 0b1110000111000000);

	OP_ADD("BLC", OP_BLC, 0b1110010000000000);
	OP_ADD("EXL", OP_EXL, 0b1110010100000000);
	OP_ADD("BRC", OP_BRC, 0b1110011000000000);
	OP_ADD("NRF", OP_NRF, 0b1110011100000000);

	OP_ADD("RIC", OP_R, 0b1110100000000000);
	OP_ADD("ZLB", OP_R, 0b1110100000000001);
	OP_ADD("SXU", OP_R, 0b1110100000000010);
	OP_ADD("NGA", OP_R, 0b1110100000000011);
	OP_ADD("SLZ", OP_R, 0b1110100000000100);
	OP_ADD("SLY", OP_R, 0b1110100000000101);
	OP_ADD("SLX", OP_R, 0b1110100000000110);
	OP_ADD("SRY", OP_R, 0b1110100000000111);
	OP_ADD("NGL", OP_R, 0b1110100000001000);
	OP_ADD("RPC", OP_R, 0b1110100000001001);
	OP_ADD("SHC", OP_SHC, 0b1110100000010000);
	OP_ADD("RKY", OP_R, 0b1110101000000000);
	OP_ADD("ZRB", OP_R, 0b1110101000000001);
	OP_ADD("SXL", OP_R, 0b1110101000000010);
	OP_ADD("NGC", OP_R, 0b1110101000000011);
	OP_ADD("SVZ", OP_R, 0b1110101000000100);
	OP_ADD("SVY", OP_R, 0b1110101000000101);
	OP_ADD("SVX", OP_R, 0b1110101000000110);
	OP_ADD("SRX", OP_R, 0b1110101000000111);
	OP_ADD("SRZ", OP_R, 0b1110101000001000);
	OP_ADD("LPC", OP_R, 0b1110101000001001);

	OP_ADD("HLT", OP_HLT, 0b1110110000000000);
	OP_ADD("MCL", OP__, 0b1110110001000000);
	OP_ADD("CIT", OP__, 0b1110110010000000);
	OP_ADD("SIL", OP__, 0b1110110010000001);
	OP_ADD("SIU", OP__, 0b1110110010000010);
	OP_ADD("SIT", OP__, 0b1110110010000011);
	OP_ADD("GIU", OP__, 0b1110110011000000);
	OP_ADD("LIP", OP__, 0b1110110100000000);
	OP_ADD("GIL", OP__, 0b1110111011000000);
	OP_ADD("SINT", OP_X, 0b1110110010000100);
	OP_ADD("SIND", OP_X, 0b1110111010000100);
	OP_ADD("CRON", OP_X, 0b1110110101000000);
	// fake UJS 0
	OP_ADD("NOP", OP__, 0b1110000000000000);

	OP_ADD("UJ", OP_N, 0b1111000000000000);
	OP_ADD("JL", OP_N, 0b1111000001000000);
	OP_ADD("JE", OP_N, 0b1111000010000000);
	OP_ADD("JG", OP_N, 0b1111000011000000);
	OP_ADD("JZ", OP_N, 0b1111000100000000);
	OP_ADD("JM", OP_N, 0b1111000101000000);
	OP_ADD("JN", OP_N, 0b1111000110000000);
	OP_ADD("LJ", OP_N, 0b1111000111000000);

	OP_ADD("LD", OP_N, 0b1111010000000000);
	OP_ADD("LF", OP_N, 0b1111010001000000);
	OP_ADD("LA", OP_N, 0b1111010010000000);
	OP_ADD("LL", OP_N, 0b1111010011000000);
	OP_ADD("TD", OP_N, 0b1111010100000000);
	OP_ADD("TF", OP_N, 0b1111010101000000);
	OP_ADD("TA", OP_N, 0b1111010110000000);
	OP_ADD("TL", OP_N, 0b1111010111000000);

	OP_ADD("RD", OP_N, 0b1111100000000000);
	OP_ADD("RF", OP_N, 0b1111100001000000);
	OP_ADD("RA", OP_N, 0b1111100010000000);
	OP_ADD("RL", OP_N, 0b1111100011000000);
	OP_ADD("PD", OP_N, 0b1111100100000000);
	OP_ADD("PF", OP_N, 0b1111100101000000);
	OP_ADD("PA", OP_N, 0b1111100110000000);
	OP_ADD("PL", OP_N, 0b1111100111000000);

	OP_ADD("MB", OP_N, 0b1111110000000000);
	OP_ADD("IM", OP_N, 0b1111110001000000);
	OP_ADD("KI", OP_N, 0b1111110010000000);
	OP_ADD("FI", OP_N, 0b1111110011000000);
	OP_ADD("SP", OP_N, 0b1111110100000000);
	OP_ADD("MD", OP_N, 0b1111110101000000);
	OP_ADD("RZ", OP_N, 0b1111110110000000);
	OP_ADD("IB", OP_N, 0b1111110111000000);

	return 0;
}

// -----------------------------------------------------------------------
void kw_destroy()
{
    dh_destroy(keywords);
}

// vim: tabstop=4 autoindent
