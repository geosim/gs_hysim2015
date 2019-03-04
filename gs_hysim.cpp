// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//- GS_HYSIM.cpp : Initialization functions
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include <afxdllx.h>

#ifndef _hy_inout_h
	#include "hy_inout.h"
#endif

#ifndef _hy_utily_h
	#include "hy_utily.h"
#endif

//-----------------------------------------------------------------------------
//- Define the sole extension module object.
AC_IMPLEMENT_EXTENSION_MODULE(GS_HYSIMDLL)
//- Now you can use the CAcModuleResourceOverride class in
//- your application to switch to the correct resource instance.
//- Please see the ObjectARX Documentation for more details

//-----------------------------------------------------------------------------
//- DLL Entry Point
extern "C"
BOOL WINAPI DllMain (HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	//- Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved) ;

	if ( dwReason == DLL_PROCESS_ATTACH ) {
        _hdllInstance =hInstance ;
		GS_HYSIMDLL.AttachInstance (hInstance) ;
		InitAcUiDLL () ;
	} else if ( dwReason == DLL_PROCESS_DETACH ) {
		GS_HYSIMDLL.DetachInstance () ;
	}
	return (TRUE) ;
}

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CGS_HYSIMApp : public AcRxArxApp {

public:
	CGS_HYSIMApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
    	CString text;

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		text.LoadString(HY_STRING234); // "\nCaricamento GEOsim Network.\n1995-2011 IREN ACQUA GAS S.p.A.\nVersione 2011.0.a\n"
		acutPrintf(text);

		acedRegCmds->addCommand(_T("HY"), _T("gs_sim_h2o"),            _T("gs_sim_h2o"), ACRX_CMD_MODAL, &hy_sim_h2o);
		acedRegCmds->addCommand(_T("HY"), _T("gs_sim_gasbassa"),       _T("gs_sim_gasbassa"), ACRX_CMD_MODAL, &hy_sim_gb);
		acedRegCmds->addCommand(_T("HY"), _T("gs_sim_gasmedia"),       _T("gs_sim_gasmedia"), ACRX_CMD_MODAL, &hy_sim_gm);
		acedRegCmds->addCommand(_T("HY"), _T("gs_evid"),               _T("gs_evid"), ACRX_CMD_MODAL, &hy_sim_evid);
		acedRegCmds->addCommand(_T("HY"), _T("gs_sim_cd"),             _T("gs_sim_cd"), ACRX_CMD_MODAL, &hy_sim_cd);
		acedRegCmds->addCommand(_T("HY"), _T("gs_diamc"),              _T("gs_diamc"), ACRX_CMD_MODAL, &hy_diamc);
		acedRegCmds->addCommand(_T("HY"), _T("hy_EvidValveToIsolate"), _T("hy_EvidValveToIsolate"), ACRX_CMD_MODAL, &hy_EvidValveToIsolate);
		acedRegCmds->addCommand(_T("HY"), _T("hy_EvidConnected"),      _T("hy_EvidConnected"), ACRX_CMD_MODAL, &hy_EvidConnected);
		acedRegCmds->addCommand(_T("HY"), _T("hy_EvidNets"),           _T("hy_EvidNets"), ACRX_CMD_MODAL, &hy_EvidNets);
		acedRegCmds->addCommand(_T("HY"), _T("hy_EvidWaterAge"),       _T("hy_EvidWaterAge"), ACRX_CMD_MODAL, &hy_EvidWaterAge);      
		acedRegCmds->addCommand(_T("HY"), _T("hy_EvidWaterConc"),      _T("hy_EvidWaterConc"), ACRX_CMD_MODAL, &hy_EvidWaterConc);      

		acedRegCmds->addCommand(_T("HY"), _T("gs_insNodeH2O"),         _T("gs_insNodeH2O"), ACRX_CMD_MODAL,  &gs_insNodeH2O);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insNodeGasB"),        _T("gs_insNodeGasB"), ACRX_CMD_MODAL, &gs_insNodeGasB);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insNodeGasM"),        _T("gs_insNodeGasM"), ACRX_CMD_MODAL, &gs_insNodeGasM);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insTraceH2O"),        _T("gs_insTraceH2O"), ACRX_CMD_MODAL,  &gs_insTraceH2O);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insTraceGasB"),       _T("gs_insTraceGasB"), ACRX_CMD_MODAL, &gs_insTraceGasB);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insTraceGasM"),       _T("gs_insTraceGasM"), ACRX_CMD_MODAL, &gs_insTraceGasM);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insValveH2O"),        _T("gs_insValveH2O"), ACRX_CMD_MODAL,  &gs_insValveH2O);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insValveGasB"),       _T("gs_insValveGasB"), ACRX_CMD_MODAL, &gs_insValveGasB);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insValveGasM"),       _T("gs_insValveGasM"), ACRX_CMD_MODAL, &gs_insValveGasM);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insPrvH2O"),          _T("gs_insPrvH2O"), ACRX_CMD_MODAL, &gs_insPrvH2O);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insPrvGasB"),         _T("gs_insPrvGasB"), ACRX_CMD_MODAL, &gs_insPrvGasB);
		acedRegCmds->addCommand(_T("HY"), _T("gs_insPrvGasM"),         _T("gs_insPrvGasM"), ACRX_CMD_MODAL, &gs_insPrvGasM);

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		acedRegCmds->removeGroup(_T("HY"));

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
	// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member 
	// function of the CGS_HYSIMApp class.
	// The function should take no arguments and return nothing.
	//
	// NOTE: ACED_ARXCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid and
	// have arguments to define context and command mechanism.
	
	// ACED_ARXCOMMAND_ENTRY_AUTO(classname, group, globCmd, locCmd, cmdFlags, UIContext)
	// ACED_ARXCOMMAND_ENTRYBYID_AUTO(classname, group, globCmd, locCmdId, cmdFlags, UIContext)
	// only differs that it creates a localized name using a string in the resource file
	//   locCmdId - resource ID for localized command

	// Modal Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CGS_HYSIMApp, MyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL)
	static void MyGroupMyCommand () {
		// Put your command code here

	}

	// Modal Command with pickfirst selection
	// ACED_ARXCOMMAND_ENTRY_AUTO(CGS_HYSIMApp, MyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET)
	static void MyGroupMyPickFirst () {
		ads_name result ;
		int iRet =acedSSGet (ACRX_T("_I"), NULL, NULL, NULL, result) ;
		if ( iRet == RTNORM )
		{
			// There are selected entities
			// Put your command using pickfirst set code here
		}
		else
		{
			// There are no selected entities
			// Put your command code here
		}
	}

	// Application Session Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CGS_HYSIMApp, MyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION)
	static void MyGroupMySessionCmd () {
		// Put your command code here
	}

	// The ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO macros can be applied to any static member 
	// function of the CGS_HYSIMApp class.
	// The function may or may not take arguments and have to return RTNORM, RTERROR, RTCAN, RTFAIL, RTREJ to AutoCAD, but use
	// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal to return
	// a value to the Lisp interpreter.
	//
	// NOTE: ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid.
	
	//- ACED_ADSFUNCTION_ENTRY_AUTO(classname, name, regFunc) - this example
	//- ACED_ADSSYMBOL_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file
	//- ACED_ADSCOMMAND_ENTRY_AUTO(classname, name, regFunc) - a Lisp command (prefix C:)
	//- ACED_ADSCOMMAND_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file

	// Lisp Function is similar to ARX Command but it creates a lisp 
	// callable function. Many return types are supported not just string
	// or integer.
	// ACED_ADSFUNCTION_ENTRY_AUTO(CGS_HYSIMApp, MyLispFunction, false)
	static int ads_MyLispFunction () {
		//struct resbuf *args =acedGetArgs () ;
		
		// Put your command code here

		//acutRelRb (args) ;
		
		// Return a value to the AutoCAD Lisp Interpreter
		// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal

		return (RTNORM) ;
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CGS_HYSIMApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CGS_HYSIMApp, MyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CGS_HYSIMApp, MyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CGS_HYSIMApp, MyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION, NULL)
ACED_ADSSYMBOL_ENTRY_AUTO(CGS_HYSIMApp, MyLispFunction, false)
