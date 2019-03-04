/******************************************************************************/
/*Name: HY_BASE
                                
	Module description: File funzioni di utilità e di base
         
	Author: Maria Paola Deprati
                                      
(c) Copyright 1995-2010 by IREN ACQUA GAS  S.p.A

/******************************************************************************/

#include "StdAfx.h"
#include "hy_base.h"


C_LINK_SET HyAppl::GS_LS;     // resbuf per gs_evid


//	Send an error message to the command window.
void gs_errmsg(int msg)
{
	acutPrintf(_T("\nErrore GEOsim: [%d] - %s\n"), msg, gsc_err(msg));
}

//	Send an error message to the command window.
void hy_errmsg(int msg)
{
	CString		err;

	err.LoadString(msg);

	acutPrintf(_T("\nErrore HySim: [%d] - %s\n"), msg, err);
}


/******************************************************************************/
/*.doc hy_list_deleted                                                       */
/*+
	Esegue uno statement SELECT sulla tabella GS_DELET, ed inserisce,
	nella lista <deleted_list>, gli eventuali record reperiti.
	Parametri:
	int					cls						Codice classe
	C_2INT_LONG_LIST	*deleted_list			lista delle cancellate

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/******************************************************************************/
int hy_list_deleted(int cls, C_2INT_LONG_LIST *list)
{
	int		ret = GS_GOOD;
	
	C_DBCONNECTION	*pConn;
	C_STRING		TableRef, statement;

	_RecordsetPtr	pRs;
	
	C_RB_LIST		ColValues;
	presbuf			p;

	long			gs_id, sub;

	C_2INT_LONG		*item;

	TRY
	{
		if (get_GS_CURRENT_WRK_SESSION()->getDeletedTabInfo(&pConn, &TableRef) == GS_BAD) 
			AfxThrowUserException();
	
		statement = _T("SELECT SUB_CL_ID, KEY_ATTRIB FROM ");
		statement += TableRef;
		statement += _T(" WHERE CLASS_ID = "); 
		statement += cls;

		if (pConn->OpenRecSet(statement, pRs) == GS_BAD)
			AfxThrowUserException();

		// per ogni relazione
		while (gsc_isEOF(pRs) == GS_BAD)
		{
			if (gsc_DBReadRow(pRs, ColValues) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(_T("SUB_CL_ID"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo il key attrib
			if (gsc_rb2Lng(p, &sub) == GS_BAD) 
				AfxThrowUserException( );

			if ((p = ColValues.CdrAssoc(_T("KEY_ATTRIB"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo il gs_id
			if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
				AfxThrowUserException( );

			if ((item = new C_2INT_LONG()) == NULL)
			{ 
				gs_errmsg(eGSOutOfMem);
				AfxThrowUserException();
			}

			item->set_key(cls);
			item->set_type(sub);
			item->set_id(gs_id);

			list->add_tail(item);
			
			gsc_Skip(pRs);
		}

		// chiusura record set
		gsc_DBCloseRs(pRs);
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret	= GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc hy_last																*/
/*+
	Esegue uno statement SELECT sulla tabella GS_CLASS per determinare i 
	last per le <entità nodali> e per i <lati>.
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int hy_last(C_CLASS *pCls, C_INT_LONG_LIST *lastList)
{
	int			ret = GS_GOOD;
	
	C_INT_LONG	*lastCode;

	C_SUB		*pSub;

	TRY
	{
		lastList->remove_all();
		
		pSub = (C_SUB *) pCls->ptr_sub_list()->get_head();
		while (pSub)
		{
			if ((lastCode = new C_INT_LONG(pSub->ptr_id()->sub_code, pSub->ptr_info()->TempLastId)) == NULL)
			{ 
				gs_errmsg(eGSOutOfMem); 
				AfxThrowUserException();
			}
			
			lastList->add_tail(lastCode);

			pSub = (C_SUB *) pCls->ptr_sub_list()->get_next();
		}
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH
	
	return ret;
}


/***************************************************************************/
/*.doc hy_outRecordSet																      */
/*+
	Esegue una SELECT sui campi e sulla tabella determinati dai parametri in 
	ingresso "what_fields" e "pCls"; in particolare legge le righe della 
	tabella senza bloccarla.

	Parametri
	input:	C_CLASS			*pCls		classe per determinare la tabella
										sulla quale effettuare la SELECT
			CString			*fields		campi sui quali effettuare la SELECT
			CString			*SQLWhere		condizione WHERE per la SELECT
	output: _RecordsetPtr	&pRs

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/***************************************************************************/
int hy_outRecordSet(C_CLASS *pCls, CString *fields, CString *SQLWhere, _RecordsetPtr &pRs, 
					enum CursorTypeEnum CursorType,	enum LockTypeEnum LockType, int type)
{
	int	ret = GS_GOOD;

	C_INFO		*pInfo=pCls->ptr_info();

	C_STRING	statement;
	C_STRING	TableRef;

	C_DBCONNECTION   *pConn;
	
	TRY
	{
		if (!fields) 
		{ 
			gs_errmsg(eGSInvalidArg); 
			AfxThrowUserException();
		}

		// Ricavo la connessione OLE-DB alla tabella type
		if ((pConn = pInfo->getDBConnection(type)) == NULL)
			AfxThrowUserException();

		if (type==TEMP)
		{ 
			// Ricavo il riferimento completo alla tabella temporanea
			if (pCls->getTempTableRef(TableRef)==GS_BAD) 
				AfxThrowUserException();
		}
		else
			// Ricavo il riferimento completo alla tabella 
			TableRef = pInfo->OldTableRef;

		// condizione WHERE perchè i record con identificatore negativo sono cancellati
		// e quindi da non considerare, mentre l'identificatore 0 è il record di default
		statement = _T("SELECT ");
		statement += *fields;
		statement += _T(" FROM ");
		statement += TableRef;
		statement += _T(" WHERE ");
		if (SQLWhere)
			statement += *SQLWhere;
		else
		{
			statement += pInfo->key_attrib;
			statement += _T(" >0");
		}
		statement += _T(" ORDER BY ");
		statement += pInfo->key_attrib;

		// Eseguo l'istruzione della select sulla tabella
		if (pConn->OpenRecSet(statement, pRs, CursorType, LockType) == GS_BAD) 
			AfxThrowUserException();
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH
		
	return ret;
}

int write_udl(CString udlFile, CString connStr)
{
	int ret = GS_GOOD;

	CString	oleDB, dataSource, UDLMainFile;

	int	pos;
	
	TRY
	{
		if (gsc_path_exist(udlFile) == GS_GOOD)
		{		
			oleDB = gsc_LoadOleDBIniStrFromFile(udlFile);

			pos = -1;

			if (!oleDB.IsEmpty() && ((pos = oleDB.Find(_T("OLEDB"))) != -1))
			{
				pos = oleDB.Find(_T("Data Source="));
				dataSource = oleDB.Right(oleDB.GetLength() - (pos + 12));

				if ((pos = dataSource.Find(_T(";"))) != -1)
					dataSource = dataSource.Left(pos);

				if (dataSource != connStr)
				{
					oleDB.Replace(dataSource, connStr);

					if (gsc_WriteOleDBIniStrToFile(oleDB, udlFile) == GS_BAD)
						AfxThrowUserException();
				}
			}
			else
				AfxThrowUserException();
		}
		else
		{
			UDLMainFile = get_GEODIR();
			UDLMainFile += _T('\\');
			UDLMainFile += GEODATALINKDIR;
			UDLMainFile += _T('\\');
			UDLMainFile += GEOSIM_DATASOURCE;

			if (gsc_path_exist(UDLMainFile) == GS_GOOD)
			{		
				oleDB = gsc_LoadOleDBIniStrFromFile(UDLMainFile);

				pos = -1;

				if (!oleDB.IsEmpty() && ((pos = oleDB.Find(_T("OLEDB"))) != -1))
				{
					pos = oleDB.Find(_T("Data Source="));
					dataSource = oleDB.Right(oleDB.GetLength() - (pos + 12));

					if ((pos = dataSource.Find(_T(";"))) != -1)
						dataSource = dataSource.Left(pos);

					if (dataSource != connStr)
					{
						oleDB.Replace(dataSource, connStr);

						if (gsc_WriteOleDBIniStrToFile(oleDB, udlFile) == GS_BAD)
							AfxThrowUserException();
					}
				}
				else
					AfxThrowUserException();
			}
		}
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/***************************************************************************/
/*.doc gsc_clear_file(TCHAR *filename)                                      */                                
/*+
   Determina, nel file passato come parametro, la posizione dopo aver 
   eliminato un numero di righe in funzione del numero passato in div.

   Restituisce RTNORM in caso di successo altrimenti restituisce RTERROR.
-*/  
/***************************************************************************/
int gsc_clear_file(CString filename, int div, long *position)
{
   int   result=GS_GOOD, res, i=-1, iter=0;
   FILE  *file = NULL;
   TCHAR *readrow = NULL;;
   bool  Unicode = false;

   do
   {
      if ((file = gsc_fopen(filename, _T("r"), MORETESTS, &Unicode)) == NULL)
         { result = GS_BAD; break; }

      do
      {
         res = gsc_readline(file, &readrow);        
         i++;
         free(readrow); 
         readrow = NULL;
      }
      while (res == GS_GOOD);

      iter=i/div;
      iter--;
      iter=iter*div;

      res = GS_GOOD;
      if (fseek(file, 0, SEEK_SET) == 0)
         while (iter > 0 && res == GS_GOOD)
         {
            res = gsc_readline(file, &readrow);
            iter--;
            free(readrow); 
            readrow = NULL;
         }
      (*position)=ftell(file);
   }
   while(0);

   if (file)
      fclose(file);

   return result;
}


/***************************************************************************/
/*.doc is_GEOnetworkClass                                                  */
/*+
   Determina se la classe è una simulazione network del tipo indicato.
   Parametri:
   C_CLASS *pCls;             Classe di geosim
   NetworkTypesEnum type;   tipo di rete

   Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/***************************************************************************/
int is_GEOnetworkClass(C_CLASS *pCls, NetworkTypesEnum type)
{
	int	     ret = GS_GOOD;
	int	     nsub, i;	
	CString	  str;
	C_STR		  *pAttrib = NULL;
	C_STR_LIST attrib_list[6];
   C_SUB		  *pSub;

	TRY
	{
	   if (!pCls || pCls->ptr_id()->category != CAT_EXTERN)
		   AfxThrowUserException( );

		switch (type)
		{
			case H2O:
				nsub = 6;
				str = _T("QUOTA,CONSUMO,CARICO,PRESSIONE,EROGAZIONE");
				attrib_list[0].from_str(str);
				str = _T("LUNGHEZZA,DIAMETRO,PORTATA_D,VELOCITA,PERD_CARIC,PORTATA_IN,PORTATA_FI,VERSO_Q_IN,VERSO_Q_FI,NODO_INIZ,NODO_FINAL,LEGGE_SC,PARAM_SC,CALC");
				attrib_list[1].from_str(str);
				str = _T("QUOTA_PL,Q_USCENTE");
				attrib_list[2].from_str(str);
				str = _T("QUOTA_ASP,PREVALENZA,PORTATA,CURVA,Q_NOM,H_NOM,ON_OFF");
				attrib_list[3].from_str(str);
				str = _T("CHIUSA,QUOTA");
				attrib_list[4].from_str(str);
				str = _T("QUOTA,PRESSIONE,NODO_REGO,PRES_MONTE,PRES_VALLE,CARI_MONTE,CARI_VALLE");
				attrib_list[5].from_str(str);
				break;
			case GB:
				nsub = 5;
				str = _T("QUOTA,CONSUMO,CARICO,PRESSIONE,EROGAZIONE");
				attrib_list[0].from_str(str);
				str = _T("LUNGHEZZA,DIAMETRO,PORTATA_D,VELOCITA,PERD_CARIC,PORTATA_IN,PORTATA_FI,VERSO_Q_IN,VERSO_Q_FI,NODO_INIZ,NODO_FINAL,CALC");
				attrib_list[1].from_str(str);
				str = _T("QUOTA,PRESSIONEB,Q_USCENTE");
				attrib_list[2].from_str(str);
				str = _T("CHIUSA,QUOTA");
				attrib_list[3].from_str(str);
				str = _T("QUOTA,PRESSIONE,NODO_REGO,PRES_MONTE,PRES_VALLE,CARI_MONTE,CARI_VALLE");
				attrib_list[4].from_str(str);
				break;
			case GM:
				nsub = 5;
				str = _T("QUOTA,CONSUMO,CARICO,PRESSIONE,EROGAZIONE");
				attrib_list[0].from_str(str);
				str = _T("LUNGHEZZA,DIAMETRO,PORTATA_D,VELOCITA,PERD_CARIC,PORTATA_IN,PORTATA_FI,VERSO_Q_IN,VERSO_Q_FI,NODO_INIZ,NODO_FINAL,CALC");
				attrib_list[1].from_str(str);
				str = _T("QUOTA,PRESSIONEM,Q_USCENTE");
				attrib_list[2].from_str(str);
				str = _T("CHIUSA,QUOTA");
				attrib_list[3].from_str(str);
				str = _T("QUOTA,PRESSIONE,NODO_REGO,PRES_MONTE,PRES_VALLE,CARI_MONTE,CARI_VALLE");
				attrib_list[4].from_str(str);
				break;
		}

		if ((pCls->ptr_sub_list())->get_count() != nsub)
			AfxThrowUserException( );

		for (i = 1 ; i <= nsub; i++)        // ciclo di lettura sotto-classi
		{   
			pSub = (C_SUB *) pCls->ptr_sub_list()->getptr_at(i);

			if (pSub->get_category() == CAT_SPAGHETTI)
				continue;

			pAttrib = (C_STR *) attrib_list[i-1].get_head();
			while (pAttrib)
			{
				if (pSub->ptr_attrib_list()->search_name(pAttrib->get_name(), FALSE) == NULL)
					AfxThrowUserException( );

				pAttrib = (C_STR *) attrib_list[i-1].get_next();
			}
		}
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


C_4LONG::C_4LONG() : C_NODE() {valveId=0; newId=0; line1Id=0; line2Id=0;}
C_4LONG::C_4LONG(long l1, long l2, long l3, long l4) : C_NODE() {valveId=l1; newId=l2; line1Id=l3; line2Id=l4;}
C_4LONG::~C_4LONG() {}

long C_4LONG::get_valveId() 
{ 
   return valveId; 
}

long C_4LONG::get_newId() 
{ 
   return newId; 
}

long C_4LONG::get_line1Id() 
{ 
   return line1Id; 
}

long C_4LONG::get_line2Id() 
{ 
   return line2Id; 
}



C_4LONG_LIST::C_4LONG_LIST() : C_LIST() {}

C_4LONG_LIST::~C_4LONG_LIST() {}

C_4LONG *C_4LONG_LIST::searchLine1(long id, long line_id)
{
   C_4LONG *punt = (C_4LONG *) get_head();

   while (punt)
   {
	  if ((id == punt->get_valveId()) && (line_id == punt->get_line1Id()))
		 break;
      punt = (C_4LONG *) get_next();
   }
   return punt;
}

C_4LONG *C_4LONG_LIST::searchLine2(long id, long line_id)
{
   C_4LONG *punt = (C_4LONG *) get_head();

   while (punt)
   {
	  if ((id == punt->get_valveId()) && (line_id == punt->get_line2Id()))
		 break;
      punt = (C_4LONG *) get_next();
   }
   return punt;
}