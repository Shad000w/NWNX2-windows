#include "stdafx.h"
#include "nwnx_funcs.h"
//#include "madchook.h"
#include "..\NWNXdll\hook_funcs.h"
#include <math.h>

extern CNWNXFuncs NWNFuncs;

CHookFunctions::CHookFunctions() {
	org_CreateNewGeometry = 0x005A9250;
	org_ExecuteCommandEffectAppear = 0x00585110;
	CNWVirtualMachineCommands__ExecuteCommandGetItemPropertyTypeORG = 0x0058EA00;
	CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffectORG = 0x0058EBD0;
	CNWSMessage__TestObjectVisibleORG = 0x004409F0;
	CNWSCreatureStats__GetEffectImmunityORG = 0x0048B770;
	CServerExoAppInternal__RemovePCFromWorldORG = 0x0045B990;

	CNWSModule__UpdateTimeORG = 0x004DB1A0;

	HookFunctions();
}

int __fastcall CHookFunctions::ExecuteCommandEffectAppear_Hook(void *pTHIS, void *pVOID, int a2, int a3) {
	int iRet = NWNFuncs.NssEffects->CreateCustomEffect(a3);
	if (iRet != -1) return iRet;

	return ExecuteCommandEffectAppear_Next(pTHIS, NULL, a2, a3);
}

int __fastcall CHookFunctions::CNWVirtualMachineCommands__ExecuteCommandGetItemPropertyTypeHOOK(void *pTHIS, void *pVOID, int a1, int a2) {
	int iRet = NWNFuncs.NssItemProperty->GetItemPropertyInformation();
	if (iRet != -1) return iRet;

	return CNWVirtualMachineCommands__ExecuteCommandGetItemPropertyTypeNEXT(pTHIS, NULL, a1, a2);
}

int __fastcall CHookFunctions::CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffectHOOK(void *pTHIS, void *pVOID, int a1, int a2) {
	// I'm hooking ItemPropertyNoDamage because I'm hoping it isn't used much and has no parameters to pop from the stack
	int iRet = NWNFuncs.NssItemProperty->ItemPropertyCustom(pTHIS, a1);
	if (iRet != -1) return iRet;

	return CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffectNEXT(pTHIS, NULL, a1, a2);
}

int __fastcall CHookFunctions::CNWSMessage__TestObjectVisibleHOOK(void *pMessage, void *pVOID, CNWSObject *pObject1, CNWSObject *pObject2) {
	nwn_objid_t oObject1 = pObject1->obj_id;
	nwn_objid_t oObject2 = pObject2->obj_id;
	int nResult = 0;
	if(!NWNFuncs.Visibility->TestVisibility(oObject1, oObject2, nResult)){
		nResult = CNWSMessage__TestObjectVisibleNEXT(pMessage, NULL, pObject1, pObject2);
	}
//	_log(3, "Visibility check: %x - %x: %d\n", pObject1->obj_id, pObject2->obj_id, nResult);
	return nResult;
}

void __fastcall CHookFunctions::CreateNewGeometryHookProc(CNWSTrigger *pTHIS, void *pVOID, float size, CScriptLocation *Loc, CNWSArea *Area) {
	if (NWNFuncs.NssCustomTrigger->CreateCustomGeometry(pTHIS, Loc, Area)) {
		return;
	}
	else {
		nwn_CreateNewGeometryNextHook(pTHIS, NULL, size, Loc, Area);
	}
}

signed int __fastcall CHookFunctions::CNWSCreatureStats__GetEffectImmunityHOOK(CNWSCreatureStats *Stats_this, void *pVOID, unsigned __int8 a2, CNWSCreature *Versus) {
	CNWSCreature * cre = Stats_this->cs_original;

	CExoString OverrideVar("NWNXFUNCS_IMMOVERRIDE");
	unsigned long int ImmunityOverride = cre->obj_vartable.GetInt(OverrideVar);
	unsigned long int Immunity = (2 << (a2-1));
	int bOverride = ImmunityOverride & Immunity;
	_log(3, "ImmunityOverride: %d\ta2: %d\t[override: %d]\n", ImmunityOverride, a2, bOverride);
	if (bOverride) {
		_log(3, "returning 0\n");
		return 0;
	}
	else return CNWSCreatureStats__GetEffectImmunityNEXT(Stats_this, NULL, a2, Versus);
}

int __fastcall CHookFunctions::CServerExoAppInternal__RemovePCFromWorldHOOK(CServerExoAppInternal *pTHIS, void *pVOID, CNWSPlayer_s *a2) {
	_log(3, "o CServerExoAppInternal__RemovePCFromWorldHOOK running\n");
	(*NWN_VirtualMachine)->Runscript(&CExoString(NWNFuncs.OnPlayerLeavingScript), a2->obj_id);
	return CServerExoAppInternal__RemovePCFromWorldNEXT(pTHIS, NULL, a2);
}

char __fastcall CHookFunctions::CNWSModule__UpdateTimeHOOK(CNWSModule *pTHIS, void *pVOID, int a2, int i, int a4) {
	uint8_t Time = pTHIS->mod_timeofday;
	char ret = CNWSModule__UpdateTimeNEXT(pTHIS, NULL, a2, i, a4);
	if (Time != pTHIS->mod_timeofday) {
		(*NWN_VirtualMachine)->Runscript(&CExoString(NWNFuncs.OnTimeOfDayChangeScript), 0);
	}
	return ret;
}

void CHookFunctions::HookFunctions() {
	if(NWNFuncs.bHookCreateGeometry) {
//		if (HookCode((PVOID) org_CreateNewGeometry, CreateNewGeometryHookProc, (PVOID*)&nwn_CreateNewGeometryNextHook))
		if (HookFunction(CreateNewGeometryHookProc, (PVOID*)&nwn_CreateNewGeometryNextHook, (PVOID) org_CreateNewGeometry, 0))
			_log(1, "* CreateNewGeometry hooked\n");
		else _log(1, "* CreateNewGeometry hook failed\n");
	}

	// used for custom effects
//	if (HookCode((PVOID) org_ExecuteCommandEffectAppear, ExecuteCommandEffectAppear_Hook, (PVOID*)&ExecuteCommandEffectAppear_Next))
	if (HookFunction(ExecuteCommandEffectAppear_Hook, (PVOID*)&ExecuteCommandEffectAppear_Next, (PVOID) org_ExecuteCommandEffectAppear, 2))
		_log(1, "* CustomEffect function hooked\n");
	else _log(1, "X CustomEffect function hook failed\n");

	// used to get more information about item properties
	if (HookFunction(CNWVirtualMachineCommands__ExecuteCommandGetItemPropertyTypeHOOK, (PVOID*)&CNWVirtualMachineCommands__ExecuteCommandGetItemPropertyTypeNEXT, (PVOID)CNWVirtualMachineCommands__ExecuteCommandGetItemPropertyTypeORG, 2))
		_log(1, "* CNWVirtualMachineCommands__ExecuteCommandGetItemPropertyType hooked\n");
	else _log(1, "X CNWVirtualMachineCommands__ExecuteCommandGetItemPropertyType hook failed\n");

	// used for custom item properties
	//if (HookCode((PVOID)CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffectORG, CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffectHOOK, (PVOID*)&CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffectNEXT))
	if (HookFunction( CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffectHOOK, (PVOID*)&CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffectNEXT, (PVOID)CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffectORG, 2))
		_log(1, "* CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffect hooked\n");
	else _log(1, "X CNWVirtualMachineCommands__ExecuteCommandItemPropertyEffect hook failed\n");

	/*
	// LOAD FAIL!!!
	//if (HookCode((PVOID)CNWSMessage__TestObjectVisibleORG, CNWSMessage__TestObjectVisibleHOOK, (PVOID*)&CNWSMessage__TestObjectVisibleNEXT))
	if (HookFunction(CNWSMessage__TestObjectVisibleHOOK, (PVOID*)&CNWSMessage__TestObjectVisibleNEXT, (PVOID)CNWSMessage__TestObjectVisibleORG, 0))
		_log(0, "* CNWSMessage__TestObjectVisible hooked\n");
	else _log(0, "X CNWSMessage__TestObjectVisible hook failed\n");
	*/

	
	//if (HookCode((PVOID)CNWSCreatureStats__GetEffectImmunityORG, CNWSCreatureStats__GetEffectImmunityHOOK, (PVOID*)&CNWSCreatureStats__GetEffectImmunityNEXT))
	if (HookFunction(CNWSCreatureStats__GetEffectImmunityHOOK, (PVOID*)&CNWSCreatureStats__GetEffectImmunityNEXT, (PVOID)CNWSCreatureStats__GetEffectImmunityORG, 0))
		_log(0, "* CNWSCreatureStats__GetEffectImmunity hooked\n");
	else _log(0, "X CNWSCreatureStats__GetEffectImmunity hook failed\n");
	
	
	if (NWNFuncs.bHookRemovePCFromWorld) {
		if (HookFunction(CServerExoAppInternal__RemovePCFromWorldHOOK, (PVOID*)&CServerExoAppInternal__RemovePCFromWorldNEXT, (PVOID)CServerExoAppInternal__RemovePCFromWorldORG, 2))
			_log(0, "* CServerExoAppInternal__RemovePCFromWorld hooked; script to run: %s.nss\n", NWNFuncs.OnPlayerLeavingScript);
		else _log(0, "X CServerExoAppInternal__RemovePCFromWorld hook failed\n");
	}

	if (NWNFuncs.bHookTimeOfDayChange) {
		if (HookFunction(CNWSModule__UpdateTimeHOOK, (PVOID*)&CNWSModule__UpdateTimeNEXT, (PVOID)CNWSModule__UpdateTimeORG, 0))
			_log(0, "* CNWSModule__UpdateTime hooked; script to run: %s\n", NWNFuncs.OnTimeOfDayChangeScript);
		else _log(0, "X CNWSModule__UpdateTime hook failed\n");
	}
	

}


/*char __fastcall CHookFunctions::CNWSCreatureStats__GetDEXModHook(CNWSCreatureStats *Stats, void *pVOID, int a3) {
	CNWSCreature *cre = Stats->cs_original;
	int ArmorDexMod=0;

	if (a3 != 1) {
		CNWSItem *Armor = NULL;
		Armor = cre->cre_equipment->GetItemInSlot(2);
		if (Armor) {
			CExoString ExoString("FUNCS_OVERRIDEMAXDEX");
			ArmorDexMod = Armor->obj.obj_vartable.GetInt(ExoString);
			_log(3, "ArmorDexBonusOverride: %d\n", ArmorDexMod);
			if (ArmorDexMod == 0) {
				int ArmorBaseAC = Armor->ComputeArmorClass();
				_log(3, "ArmorBaseAC: %d\n", ArmorBaseAC);
				if (ArmorBaseAC > 0) {
					ExoString = "DEXBONUS";
					C2DA *TDA = (*NWN_Rules)->ru_2das->tda_armor;
					TDA->GetINTEntry_strcol(ArmorBaseAC, ExoString, &ArmorDexMod);
					_log(3, "TDA_DEXMOD: %d\n", ArmorDexMod);

				}
			}
		}
	}

	int DexMod = Stats->cs_dex_mod;
	int iRet = DexMod;
	_log(3, "ArmorDexMod: %d\tDexMod: %d\n", ArmorDexMod, DexMod);
	if (ArmorDexMod > 0 && DexMod > ArmorDexMod)
		iRet = ArmorDexMod;
	
	_log(3, "iRet: %d\n\n", iRet);
	return iRet;
}
*/
/*
signed int __fastcall CHookFunctions::CNWSCreatureStats__GetEffectImmunityHook(CNWSCreatureStats *Stats_this, void *pVOID, unsigned __int8 a2, CNWSCreature *Versus) {
	_log(3, "CNWSCreatureStats__GetEffectImmunityHook, a2: %d\n", a2);
	CNWSCreature * cre = Stats_this->cs_original;
	if (cre->obj.obj_is_invulnerable) {
		return 1;
	}

	uint16_t Race = 0x1C; //race_invalid
	int8_t GoodEvilAbsolute = 0;
	int8_t LawChaosAbsolute = 0;

	if (Versus) {
		CNWSCreatureStats *Stats = Versus->cre_stats;
		Race = Stats->cs_race;
		uint8_t Alignment = Stats->cs_al_goodevil;
		if (Alignment > 30) {
			if (Alignment > 70) GoodEvilAbsolute = 4;
			else GoodEvilAbsolute = 1;
		}
		else GoodEvilAbsolute = 5;

		Alignment = Stats->cs_al_lawchaos;
		if (Alignment > 30) {
			if (Alignment > 70) LawChaosAbsolute = 2;
			else LawChaosAbsolute = 1;
		}
		else LawChaosAbsolute = 3;
	}
	else {
		GoodEvilAbsolute = -1;
		LawChaosAbsolute = -1;
	}

	int i=0;
	CGameEffect *Effect = NULL;
	int int0=0, int1=0, int2=0,int3=0;

	CExoString OverrideVar("NWNXFUNCS_IMMOVERRIDE1");
	unsigned long int ImmunityOverride = cre->obj.obj_vartable.GetInt(OverrideVar);
	OverrideVar = "NWNXFUNCS_IMMOVERRIDE2";
	ImmunityOverride |= cre->obj.obj_vartable.GetInt(OverrideVar);
	_log(3, "ImmunityOverride: %d\ta2: %d\n", ImmunityOverride, a2);

	while (i < cre->obj.obj_effects_len) {
		Effect = cre->obj.obj_effects[i];
		if (Effect->eff_type > 0x16) break; // CNWSEffectListHandler__EFFECT_IMMUNITY
		int0 = Effect->GetInteger(0);
		int1 = Effect->GetInteger(1);
		int2 = Effect->GetInteger(2);
		int3 = Effect->GetInteger(3);
		if (
			   Effect->eff_type = 0x16				//EFFECT_IMMUNITY
			&& (int0 == 0x00 || int0 == a2)			//IMMUNITY_TYPE_ALL
			&& (int1 == 0x1C || int1 == Race)		//CRACE_INVALID_RACE
			&& (!int3 || int3 == GoodEvilAbsolute)
			&& (!int2 || int2 == LawChaosAbsolute)
			) {
				_log(3, "a2: %d\n", (1 << (a2-1)));
				_log(3, "%d\n", ((((unsigned long int)(1 << (a2-1))) & ImmunityOverride)));
				if (ImmunityOverride == 0 || ((((unsigned long int)(1 << (a2-1))) & ImmunityOverride) == 0))
				return 1;
		}
		i++;
	}
	return 0;
}
*/
/*
int __fastcall CHookFunctions::CNWSEffectListHandler__OnApplyModifyNumAttacksHook(void *pTHIS, void *pVOID, CNWSObject* obj, CGameEffect *Eff, int a3) {
	if (obj->GetDead() && !a3)
		return 1;
	if (obj->obj_type2 == 5) {
		CNWSCreature *cre = (CNWSCreature*)obj;
		cre->cre_combat_round->EffectAttacks += Eff->GetInteger(0);
	}
	return 0;
}
*/