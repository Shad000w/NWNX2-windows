NWNX Core 2.71-alpha

Changelog:
2.71-alpha (01.26.2013)
- Removed madChook library and DLL dependencies
- New server registration system
- 64-bit OS compatibility enhancements
- Added optional line support to NWNX.INI file:
		ListingService = "service"
		Currently defaults to "nwn1.mst.valhallalegends.com"


2.7-beta4 (07.05.2008)
- [Linux/Win32] Fixed compatibility problems with Hashset plugin

2.7-beta3 (28.01.2008)
---
- [Linux] Fixed a bug in CNWNXBase::Log function.

2.7-beta2 (15.04.2007)
- [Linux/Win32] Added dynamic allocation for results (= no more need for spacers)
- [Linux] Improved the INI parser 
- [Win32] Added debuglevel, confKey variables to NWNXBase (the main plugin class)
- [Linux/Win32] Added OBJECT_INVALID constant

2.7-beta1 (10.03.2007)
- [Linux/Win32] Hooked GetLocalObject, added OnRequestObject to NWNXBase