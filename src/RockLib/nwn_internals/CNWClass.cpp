#include "stdafx.h"
#include "types.h"
#include "nwn_internals.h"

unsigned char (__thiscall *CNWClass__GetFortSaveBonus)(CNWClass *pTHIS, unsigned char Level) = (unsigned char(__thiscall*)(CNWClass *pTHIS, unsigned char Level))0x00504B90;
unsigned char (__thiscall *CNWClass__GetWillSaveBonus)(CNWClass *pTHIS, unsigned char Level) = (unsigned char(__thiscall*)(CNWClass *pTHIS, unsigned char Level))0x00504BD0;
unsigned char (__thiscall *CNWClass__GetRefSaveBonus)(CNWClass *pTHIS, unsigned char Level) = (unsigned char(__thiscall*)(CNWClass *pTHIS, unsigned char Level))0x00504BB0;
char		  (__thiscall *CNWClass__GetAttackBonus)(CNWClass *pTHIS, unsigned __int8 Level) = (char(__thiscall*)(CNWClass *pTHIS, unsigned __int8 Level))0x00504B50;

unsigned char CNWClass_s::GetFortSaveBonus(uint8_t Level) {
	return CNWClass__GetFortSaveBonus(this, Level);
}

unsigned char CNWClass_s::GetWillSaveBonus(uint8_t Level) {
	return CNWClass__GetWillSaveBonus(this, Level);
}

unsigned char CNWClass_s::GetRefSaveBonus(uint8_t Level) {
	return CNWClass__GetRefSaveBonus(this, Level);
}

char CNWClass_s::GetAttackBonus(uint8_t Level) {
	return CNWClass__GetAttackBonus(this, Level);
}