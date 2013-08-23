// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define _CRT_SECURE_NO_WARNINGS

#if !defined(AFX_STDAFX_H__CB9F33DF_9FB8_475D_9950_1A13C9BF6C24__INCLUDED_)
#define AFX_STDAFX_H__CB9F33DF_9FB8_475D_9950_1A13C9BF6C24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <malloc.h>
#include "LogFile.h"

#define _log(a,b,...) LogFile::Instance(".\\logs.0\\nwnx_funcs.txt").LOG(a, b, __VA_ARGS__)

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__CB9F33DF_9FB8_475D_9950_1A13C9BF6C24__INCLUDED_)
