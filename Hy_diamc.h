/****************************************************************
Name: HY_DIAMC.H
                                   
Module description: File header per la gestione dei cataloghi di 
                    diametri commerciali.
            
Author: Maria Paola Deprati

(c) Copyright 1997-2010 by A.M.G.A. S.p.a.

*****************************************************************/

#ifndef _hy_diamc_h
#define _hy_diamc_h 1

#ifndef _hy_dia_diamc_h
	#include "hy_dia_diamc.h"
#endif

#ifndef _gs_def_h
   #include "gs_def.h"   
#endif

#ifndef _gs_init_h
   #include "gs_init.h"
#endif



class CDiamC
{
public:	   
	CDiamC();
   ~CDiamC();

private:
	CString	dir;

	C_INT_STR_LIST diamC_list;

	C_DBCONNECTION *pConn;


public:
	int Init();
	int DiamC();
	int GetListDiamC(C_INT_STR_LIST *list);
};

#endif
