/**********************************************/
/* Name: HY_DIAMC
                                   
   Module description:  File funzioni di base per la gestione dei 
                        diametri commerciali.
            
   Author: Paolo De Sole.
                                         
(c) Copyright 1996-2010 by IREN ACQUA GAS  S.p.A.

/***********************************************/

#include "StdAfx.h"
#include "hy_diamc.h"

#define ACCESSDIAMCSAMPLEDB "Access 2010 Sample DiamC.accdb"

CDiamC::CDiamC() 
{  
	dir = get_GEODIR();
}

CDiamC::~CDiamC() 
{
	if (pConn)
	{
		get_pDBCONNECTION_LIST()->remove(pConn);
		pConn = NULL;
	}
}

int CDiamC::Init()
{
	int		ret = GS_GOOD;	
	C_STRING Catalog;
   CDiamC   dummy;

	TRY
	{	
		Catalog = dir;
		Catalog += _T('\\');
		Catalog += DIAMC;

      // se non esiste il database lo creo
      if (gsc_path_exist(Catalog) == GS_BAD)  // se non esisteva
      {
         C_STRING SrcFile(get_GEODIR());
         
         SrcFile += _T('\\');
         SrcFile += GEOSAMPLEDIR;
         SrcFile += _T('\\');
         SrcFile += ACCESSDIAMCSAMPLEDB;

         if (gsc_copyfile(SrcFile, Catalog) == GS_BAD) 
            return GS_BAD;
      }

		C_2STR_LIST UDLProp(_T("Data Source"), Catalog.get_name());

		pConn = get_pDBCONNECTION_LIST()->get_Connection(NULL, &UDLProp);
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}

int CDiamC::DiamC()
{
	int		ret = GS_GOOD;
	
	acedRetVoid();

	TRY
	{
		if (GetListDiamC(&diamC_list) == GS_BAD)
			AfxThrowUserException( );

		CHy_dia_diamc_list	dia(pConn, &diamC_list);

		dia.DoModal();
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}

int CDiamC::GetListDiamC(C_INT_STR_LIST *list)
{
	int ret = GS_GOOD;
	
	C_STRING	statement, TableRef;
	
	long		code;
	CString		name;

	_RecordsetPtr	pRs;
	
	C_RB_LIST		ColValues;
	presbuf			p;

	C_INT_STR    *pSet;

	TRY
	{
		if (list == NULL) 
		{ 
			gs_errmsg(eGSInvalidArg); 
			AfxThrowUserException( );
		}
		
      list->remove_all();

		TableRef = _T("GS_DIAM");
				
		statement = _T("SELECT CODE, NAME FROM ");
		statement += TableRef;
  
		// Eseguo l' istruzione e ricavo il record set
		if (pConn->OpenRecSet(statement, pRs) == GS_BAD)
			AfxThrowUserException();
		
		// ciclo di modificia delle schede da "azzerare"
		while (gsc_isEOF(pRs) == GS_BAD)
		{
			if (gsc_DBReadRow(pRs, ColValues) == GS_BAD)
				AfxThrowUserException();

			// Code ... codice della lista dei diametri comm.
			p = ColValues.CdrAssoc(_T("CODE"));
			// Memorizzo il code
			if (gsc_rb2Lng(p, &code) == GS_BAD) 
				AfxThrowUserException( );
			
			// Name ... nome della lista dei diametri comm.
			p = ColValues.CdrAssoc(_T("NAME"));
			// Memorizzo il name
			name = gsc_rb2str(p);

			if ((pSet = new C_INT_STR) == NULL) 
				AfxThrowUserException( );
			if (pSet->set_key(code) == GS_BAD || pSet->set_name(name) == GS_BAD)
				AfxThrowUserException( );
			list->add_tail(pSet);
			
			gsc_Skip(pRs);
		}

		gsc_DBCloseRs(pRs);
	}
	CATCH( CUserException, e )
	{
		list->remove_all();

		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}

