/*************************************************************************
Name: HW_INOUT
                                   
Module description: File funzioni di base per l'input e l'output dei 
					dati per i modelli di calcolo rete acqua e gas 
					bassa\media pressione
            
Author: Maria Paola Deprati & Poltini Roberto
                                         
(c) Copyright 1996-2012 by IREN ACQUA GAS  S.p.A.

*************************************************************************/

#include "StdAfx.h"
#include "hy_inout.h"
// Definizioni fittizie per includere gs_ui_organizer.h senza errori
#define IDD_ONEGEOCOMPONENTSELECT_DLG 0
#define IDD_MULTIGEOCOMPONENTSELECT_DLG 0

#include "gs_ui_organizer.h"

void hy_sim_h2o()
{
	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}

	CHydroMap simulation;
	
	gsc_disable_reactors();
	if (simulation.sim_h2o() != GS_GOOD)
	{ 
		gs_errmsg(get_GS_ERR_COD());
   	gsc_enable_reactors();
		return; 
	}

	gsc_enable_reactors();
}

void hy_sim_gb()
{
	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}
	
	CHydroMap simulation;
	
	gsc_disable_reactors();
	if (simulation.sim_gb() != GS_GOOD)
	{ 
		gs_errmsg(get_GS_ERR_COD());
   	gsc_enable_reactors();
		return; 
	}

	gsc_enable_reactors();
}

void hy_sim_gm()
{
	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}
	
	CHydroMap simulation;
   
	gsc_disable_reactors();
	if (simulation.sim_gm() != GS_GOOD)
	{ 
		gs_errmsg(get_GS_ERR_COD());
   	gsc_enable_reactors();
		return; 
	}

	gsc_enable_reactors();
}


void hy_sim_evid()
{
	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}

	CHydroMap simulation;
   
	gsc_disable_reactors();
	simulation.sim_evid();
	gsc_enable_reactors();
}

void hy_sim_cd()
{
	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}
	
	CHydroMap simulation;
   
	gsc_disable_reactors();
	if (simulation.sim_cd() != GS_GOOD)
	{ 
		gs_errmsg(get_GS_ERR_COD());
   	gsc_enable_reactors();
		return; 
	}

	gsc_enable_reactors();
}


void hy_diamc()
{
	CDiamC diamc;

	if (diamc.Init() == GS_BAD)
		return;

	diamc.DiamC();
}


// Funzione che carica la classe simulazione voluta
int gsc_GetFluido(NetworkTypesEnum fluido, int *Cls)
{
	C_CLASS             *pSub = NULL;
	C_ATTRIB            *pAttrib = NULL;
	C_STR_LIST           NameAtt;
	C_CLS_PUNT_LIST      ClassExtracted;
	C_CLS_PUNT          *punt = NULL;
	C_INT_INT_STR_LIST   OnlyFluid;
	C_INT_INT_STR       *pOnlyFluid;
	int                  TotSub = 0, numSub = 0; 
	int                  Code = 0, SubCode = 0, tot = 0, vSelected; 

	// Verifico che siamo in una sessione di lavoro attiva
	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return GS_BAD; 
	}

	do
	{
		// Ricavo le classi estratte di tipo simulazione del 
		// fluido passato come parametro
		if (get_GS_CURRENT_WRK_SESSION()->get_pPrj()->extracted_class(ClassExtracted) == GS_BAD) 
   			return GS_BAD; 

		// Scorro la lista delle classi estratte per ricavare solo 
		// quelle simulazione del fluido voluto
		punt = (C_CLS_PUNT *) ClassExtracted.get_head();
		while (punt != NULL)
		{ 
			// Inserisco nella lista solo le semplici e spaghetti
			if (((C_CLASS *) punt->cls)->ptr_id()->category == CAT_EXTERN)
			{
				// Se è un simulazione esterna di GEOnetwork del fluido voluto 
				if (is_GEOnetworkClass((C_CLASS *) punt->cls, fluido) == GS_GOOD)
				{
					if ((pOnlyFluid = new(C_INT_INT_STR)) == NULL) 
					{ gs_errmsg(eGSOutOfMem); return GS_BAD; }
					pOnlyFluid->set_name(((C_CLASS *) punt->cls)->ptr_id()->name);
					pOnlyFluid->set_key(((C_CLASS *) punt->cls)->ptr_id()->code);
					OnlyFluid.add_tail(pOnlyFluid);
					Code = ((C_CLASS *) punt->cls)->ptr_id()->code;
					SubCode = ((C_CLASS *) punt->cls)->ptr_id()->sub_code;
					tot++;
				}
			}
			punt = (C_CLS_PUNT *) ClassExtracted.get_next();
		}
		if (tot == 0) 
		{
			acutPrintf(_T("\nNon esistono simulazioni compatibili con l'operazione voluta."));
			return GS_GOOD;
		}
		else if (tot > 1)
		{
			Csceltasim data(&OnlyFluid);

       		if (data.DoModal() == IDOK)
				vSelected = data.pos; // 0-based
			else
				return GS_GOOD;

			// A questo punto ho la posizione nella lista OnlyFluid della 
			// simulazione selezionata per l'inserimento rapido, mi ricavo il 
			// codice.
			pOnlyFluid = (C_INT_INT_STR *) OnlyFluid.getptr_at(vSelected+1); // 1-based
			if (pOnlyFluid)
				Code = pOnlyFluid->get_key();

			// acutPrintf("\nComando non ancora disponibile.");
		}
	}  
	while (0);

	*Cls = Code;
   
	return GS_GOOD;
}
   
 
// Funzione di inserimento di una valvola su un lato della 
// simulazione, fatta per far si che i due lati derivati dal
// comando spezza ereditino la portata distribuita
// Il parametro fluido stabilisce se si deve inserire 
// una valvola del Gas, dell' Acqua oppure una Prv
int gsc_insSub(int cls, int sub)
{
	C_CLASS          *pTrace = NULL, *pValve = NULL;
	C_RB_LIST         rbDescr, AdjList, RecValve, RecTrace, rb1, rb2;
	C_TOPOLOGY        Topa;
	presbuf           p = NULL;
	long              id = 0, KeyValve;
	int               res = 0, CValve = 0, tot = 0;
	int               i = 0;
	double            Tot_Len = 0.0, Tot_Port = 0.0, param = 0.0, len1 = 0.0, len2 = 0.0;
	double            Port1 = 0.0, Port2 = 0.0;
	ads_name          eLast;
   
	// Nel caso di inserimento di sottoclasse che non sia lato 
	// posso utilizzare sempre lo stesso procedimento, e cioè 
	// il calcolo della redistribuzione della PORTATA_D
	if (sub != 2)
	{
		// Ricavo il puntatore alla classe Valvole
		if ((pValve = get_GS_CURRENT_WRK_SESSION()->find_class(cls, sub)) == NULL) return GS_BAD;
		// Ricavo il puntatore alla classe Traccia
		if ((pTrace = get_GS_CURRENT_WRK_SESSION()->find_class(cls, 2)) == NULL) return GS_BAD;
		// Memorizzo ultima entita prima dell' inserimento della Valvola
		if (acdbEntLast(eLast) != RTNORM) ads_name_clear(eLast);
		// Chiamata alla funzione di inserimento Valvola
      if (gsc_ui_insdata(cls, sub) == GS_BAD) return GS_BAD; // roby
      //if (gsc_ddinsert(cls, sub) == GS_BAD) return GS_BAD;

		// Ricavo le nuove entità e le memorizzo per poi rielaborarle
		while (gsc_mainentnext(eLast, eLast) == GS_GOOD)
		{
			// Ricavo le caratteristiche dell' entità
			rbDescr << ads_entget(eLast);
			// Ricavo il Type 0
			if ((p = rbDescr.SearchType(0)) == NULL) return GS_BAD; 
			// Se non è un blocco continuo
			if (gsc_strcmp(p->resval.rstring, _T("INSERT")) != 0) continue;
			else
			{
				// Ricavo il nome del blocco
				if ((p = rbDescr.SearchType(2)) == NULL) return GS_BAD; 
				// Se non è la valvola continuo
				if (gsc_strcmp(pValve->ptr_fas()->block, p->resval.rstring, FALSE) == 0)
				{
					// Interrogo il codice della valvola
					if (pValve->getKeyValue(eLast, &KeyValve) == GS_BAD) return GS_BAD;
					break;
				}
				else
					continue;
			}
		}
      
		Topa.set_type(TYPE_POLYLINE);          // Tipologia di tipo rete
		// Setto la classe madre 
		Topa.set_cls(get_GS_CURRENT_WRK_SESSION()->find_class(cls));
		// Ricavo i link collegati es. ((<sub1> <id1>)(<sub2> <id2>)))
		if ((AdjList << Topa.elemadj(KeyValve, TYPE_POLYLINE)) == NULL) return GS_BAD;

		// Ciclo di lettura delle polilinee
		i = 0;
		while ((p = AdjList.nth(i++)))
		{
			gsc_rb2Lng(gsc_nth(1, p), &KeyValve);
			// Per ognuna della 2 polilinee connesse eseguo il query_data
			if (pTrace->query_data(KeyValve, RecTrace) == GS_BAD) return GS_BAD;
			// Memorizzo la lunghezza e la portata distribuita
			if ((p = RecTrace.CdrAssoc(_T("LUNGHEZZA"))) == NULL) return GS_BAD;
			if (gsc_rb2Dbl(p, &param) == GS_BAD) return GS_BAD;
			if (i == 1)
			{
				len1 = param;
				RecTrace.copy(rb1);
			}
			else
			{
				len2 = param;        
				RecTrace.copy(rb2);
			}

			Tot_Len = Tot_Len + param;
			param = 0.0;
			if ((p = RecTrace.CdrAssoc(_T("PORTATA_D"))) == NULL) return GS_BAD;
			if (gsc_rb2Dbl(p, &param) == GS_BAD) return GS_BAD;
			Tot_Port = param;
		}

		// Calcolo della ripartizione della Portata distribuita sul lato
		Port1 = len1 * Tot_Port / Tot_Len;
		Port2 = len2 * Tot_Port / Tot_Len;
      
		// Sostituisco i valori di portata nei due record da aggiornare
		if (rb1.CdrAssocSubst(_T("PORTATA_D"), Port1) == GS_BAD) return GS_BAD;
		if (rb2.CdrAssocSubst(_T("PORTATA_D"), Port2) == GS_BAD) return GS_BAD;
      
		// Ciclo di aggiornamento delle polilinee
		i = 0;
		while ((p = AdjList.nth(i++)))
		{
			gsc_rb2Lng(gsc_nth(1, p), &KeyValve);
			if (i == 1)
			{
				// Per ognuna della 2 polilinee connesse eseguo il query_data
				if (pTrace->upd_data(KeyValve, rb1) == GS_BAD) return GS_BAD;
			}
			else
			{
				// Per ognuna della 2 polilinee connesse eseguo il query_data
				if (pTrace->upd_data(KeyValve, rb2) == GS_BAD) return GS_BAD;
			}
		}
	}
	else
	{
		// Chiamata alla funzione di inserimento Valvola
		if (gsc_ui_insdata(cls, sub) == GS_BAD) return GS_BAD;
	}

	return GS_GOOD;
}


///////////////////////////////////////////////////////////

void gs_insNodeH2O()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(H2O, &Cod);

	if (Cod > 0)
		gsc_insSub(Cod, 1);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insNodeGasB()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(GB, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 1);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insNodeGasM()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(GM, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 1);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insTraceH2O()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(H2O, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 2);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insTraceGasB()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(GB, &Cod);

	if (Cod > 0)
		gsc_insSub(Cod, 2);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insTraceGasM()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(GM, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 2);
      
	gsc_enable_reactors();
   
	return; 
}

void gs_insValveGasB()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(GB, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 4);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insValveGasM()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(GM, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 4);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insValveH2O()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(H2O, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 5);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insPrvH2O()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(H2O, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 6);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insPrvGasB()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(GB, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 5);
   
	gsc_enable_reactors();
   
	return; 
}

void gs_insPrvGasM()
{
	int   Cod = 0;

	gsc_disable_reactors();

	gsc_GetFluido(GM, &Cod);
   
	if (Cod > 0)
		gsc_insSub(Cod, 5);
   
	gsc_enable_reactors();
   
	return; 
}

///////////////////////////////////////////////////////////