// gs_HydroMap.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"

// roby
#include <sys/timeb.h>  // Solo per debug
#include <time.h>       // Solo per debug

#include "gs_hy.h"
#include "hy_utily.h"

#define REGOLATORI_DI_TAGLIO 1  // roby per regolatori di taglio


/*
  PARTE GENERATA AUTOMATICAMENTE
/////////////////////////////////////////////////////////////////////////////
// CGs_HydroMapApp

BEGIN_MESSAGE_MAP(CGs_HydroMapApp, CWinApp)
	//{{AFX_MSG_MAP(CGs_HydroMapApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGs_HydroMapApp construction

CGs_HydroMapApp::CGs_HydroMapApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGs_HydroMapApp object

CGs_HydroMapApp theApp;
*/

//extern "C" HWND adsw_acadMainWnd();


CSub1::CSub1() 
{  
}

CSub1::~CSub1() 
{  
}

CSub2::CSub2() 
{  
}

CSub2::~CSub2() 
{  
}

CSub3::CSub3() 
{  
}

CSub3::~CSub3() 
{  
}

CSub4::CSub4() 
{  
}

CSub4::~CSub4() 
{  
}

CSub5::CSub5() 
{  
}

CSub5::~CSub5() 
{  
}

CSub6::CSub6() 
{  
}

CSub6::~CSub6() 
{  
}


CHydroMap::CHydroMap() 
{  
	dir_gs = get_WORKDIR();
	
	temp_dir = dir_gs;
	temp_dir += _T("/");
	temp_dir += GEOTEMPDIR;

   gsc_mkdir(temp_dir); // per sicurezza

   temp_dir += _T("/");
	
	if (get_GS_CURRENT_WRK_SESSION() != NULL)
	{
		file_dir = (get_GS_CURRENT_WRK_SESSION()->get_dir());
		file_dir += _T("/TEMP/");
	}

	tipo_fluido = H2O;
	tipo_simulazione = 0;

	tipo_controllo = 0;
	controllo_portata = 0.2;
	controllo_carico = 0.5;
	coeff_molt_portate = 1;
	convergenza_pompe = 0.100;
	precisione_pompe = 1.00;
	pompa_in_curva = 1;
	min_velocita_prv = 0.5;
	min_portata_prv = 0.5;
	densita_relativa = 0.544;

	numero_iterazioni = 1;
	velocita_approssimazione = 1;

	perdita_approsimazione = 1;		// (!!NON USATO!!) perdita di carico per una prima approssimazione dei diametri     
	tolleranza_diametri = 30;		// (!!NON USATO!!) tolleranza diametri per il "calcolo diametri"


	errore = temp_dir;
	errore += _T("errore.out");
	nodi = temp_dir;
	nodi += _T("nodi.out");
	lati = temp_dir;
	lati += _T("lati.out");
	
   simula = dir_gs;
	simula += _T("/network/simula.arx");

   #if defined(GSDEBUG) // se versione per debugging
      simula = _T("C:\\simula\\simula2015\\x64\\debug\\simula.arx");
   #endif

	lastN = nodiN = latiN = valvoleN = prvN = 0;

	nodiPos = latiPos = 0;

	C_SUB		*pSub = NULL;
}

CHydroMap::~CHydroMap() {}


int CHydroMap::OpenDialogParameters_h2o()
{
	INT_PTR      ret = GS_GOOD;
   C_STRING Path;

	// When resource from this ARX app is needed, just
	// instantiate a local CTemporaryResourceOverride
	CAcModuleResourceOverride thisResource;

	CHy_dia_h2o	dia_h2o;

	TRY
	{
      // leggo da NETWORK.INI nella cartella GEOsimAppl::WORKDIR\NETWORK
      Path = get_WORKDIR(); 
      Path += _T('\\');
      Path += NETWORK_INI_FILE;
      // se esiste il file e la entry nel file
      if (gsc_path_exist(Path) == GS_GOOD) dia_h2o.Load(Path);

		if ((ret = dia_h2o.DoModal()) == IDOK)
		{
         dia_h2o.Save(Path);

			coeff_molt_portate = dia_h2o.coeff_molt_portate;

			tipo_controllo = dia_h2o.tipologia;
			controllo_portata = dia_h2o.controllo_portata;
			controllo_carico = dia_h2o.controllo_carico;

			pompa_in_curva = dia_h2o.convergenza;
			precisione_pompe = dia_h2o.precisione_pompe;
			convergenza_pompe = dia_h2o.convergenza_pompe;
			
			min_velocita_prv = dia_h2o.min_velocita_prv;
			min_portata_prv = dia_h2o.min_portata_prv;

         rete_tutta       = dia_h2o.rete_tutta;
         aggiorna_grafica = dia_h2o.aggiorna_grafica;
		}

		if (ret==IDABORT)
			AfxThrowUserException();
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return (int) ret;
}


int CHydroMap::OpenDialogParameters_gb()
{
	INT_PTR      ret = GS_GOOD;
   C_STRING Path;

	// When resource from this ARX app is needed, just
	// instantiate a local CTemporaryResourceOverride
	CAcModuleResourceOverride thisResource;

	CHy_dia_gb	dia_gb;

	TRY
	{
      // leggo da NETWORK.INI nella cartella GEOsimAppl::WORKDIR\NETWORK
      Path = get_WORKDIR(); 
      Path += _T('\\');
      Path += NETWORK_INI_FILE;
      // se esiste il file e la entry nel file
      if (gsc_path_exist(Path) == GS_GOOD) dia_gb.Load(Path);

		if ((ret=dia_gb.DoModal()) == IDOK)
		{
         dia_gb.Save(Path);

			tipo_controllo = dia_gb.tipologia;
			controllo_portata = dia_gb.controllo_portata;
			controllo_carico = dia_gb.controllo_carico;

			densita_relativa = dia_gb.densita_relativa;
			coeff_molt_portate = dia_gb.coeff_molt_portate;

			min_velocita_prv = dia_gb.min_velocita_prv;
			min_portata_prv = dia_gb.min_portata_prv;

         rete_tutta       = dia_gb.rete_tutta;
         aggiorna_grafica = dia_gb.aggiorna_grafica;
		}

		if (ret==IDABORT)
			AfxThrowUserException();
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return (int) ret;
}


int CHydroMap::OpenDialogParameters_gm()
{
	INT_PTR      ret = GS_GOOD;
   C_STRING Path;

	// When resource from this ARX app is needed, just
	// instantiate a local CTemporaryResourceOverride
	CAcModuleResourceOverride thisResource;

	CHy_dia_gm	dia_gm;

	TRY
	{
      // leggo da NETWORK.INI nella cartella GEOsimAppl::WORKDIR\NETWORK
      Path = get_WORKDIR(); 
      Path += _T('\\');
      Path += NETWORK_INI_FILE;
      // se esiste il file e la entry nel file
      if (gsc_path_exist(Path) == GS_GOOD) dia_gm.Load(Path);

		if ((ret = dia_gm.DoModal()) == IDOK)
		{
         dia_gm.Save(Path);

			tipo_controllo = dia_gm.tipologia;
			densita_relativa = dia_gm.densita_relativa;
			controllo_portata = dia_gm.controllo_portata;
			controllo_carico = dia_gm.controllo_carico;
			coeff_molt_portate = dia_gm.coeff_molt_portate;
			
			min_velocita_prv = dia_gm.min_velocita_prv;
			min_portata_prv = dia_gm.min_portata_prv;

         rete_tutta       = dia_gm.rete_tutta;
         aggiorna_grafica = dia_gm.aggiorna_grafica;
		}

		if (ret==IDABORT)
			AfxThrowUserException();
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return (int) ret;
}


int CHydroMap::OpenDialogParameters_cd_h2o(C_DBCONNECTION *pConn)
{
	INT_PTR    ret = GS_GOOD;
   C_STRING Path;

	// When resource from this ARX app is needed, just
	// instantiate a local CTemporaryResourceOverride

	CAcModuleResourceOverride thisResource;

	int		codice_diamC;
	CString	data;
	C_SET_DIAMCOMM	set_diamC;

	CHy_dia_cd_h2o	dia_h2o;

	TRY
	{
      // leggo da NETWORK.INI nella cartella GEOsimAppl::WORKDIR\NETWORK
      Path = get_WORKDIR(); 
      Path += _T('\\');
      Path += NETWORK_INI_FILE;
      // se esiste il file e la entry nel file
      if (gsc_path_exist(Path) == GS_GOOD) dia_h2o.Load(Path);

      if ((ret = dia_h2o.DoModal()) == IDOK)
		{
         dia_h2o.Save(Path);

			tipo_controllo = dia_h2o.tipologia;
			controllo_portata = dia_h2o.controllo_portata;
			controllo_carico = dia_h2o.controllo_carico;
			coeff_molt_portate = dia_h2o.coeff_molt_portate;
			
			pompa_in_curva = dia_h2o.convergenza;
			convergenza_pompe = dia_h2o.convergenza_pompe;
			precisione_pompe = dia_h2o.precisione_pompe;
			
			min_velocita_prv = dia_h2o.min_velocita_prv;
			min_portata_prv = dia_h2o.min_portata_prv;

			codice_diamC = dia_h2o.codice_diamC;
			numero_iterazioni = dia_h2o.iterazioni;
			velocita_approssimazione = dia_h2o.velocita_riferimento;
		}

		if ((ret==IDCANCEL)||(ret==IDABORT)||(ret==-1))
			AfxThrowUserException();

		data = temp_dir;
		data += _T("diam.inp");
         
		// cancello il file 
		if (gsc_path_exist(data) == GS_GOOD)
			_wremove(data);
		
		if (codice_diamC != 0)
		{
			// diametri commerciali
			if (set_diamC.load_from_db(codice_diamC, pConn) == GS_BAD) 
				AfxThrowUserException();

			if (set_diamC.write2file(data, codice_diamC, pConn) == GS_BAD)
				AfxThrowUserException();

			codice_diamC = 0;
		}
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return (int) ret;
}


int CHydroMap::OpenDialogParameters_cd_gb(C_DBCONNECTION *pConn)
{
	INT_PTR      ret = GS_GOOD;
   C_STRING Path;

	// When resource from this ARX app is needed, just
	// instantiate a local CTemporaryResourceOverride

	CAcModuleResourceOverride thisResource;

	int		codice_diamC;
	CString	data;
	C_SET_DIAMCOMM	set_diamC;

	CHy_dia_cd_gb	dia_gb;

	TRY
	{
      // leggo da NETWORK.INI nella cartella GEOsimAppl::WORKDIR\NETWORK
      Path = get_WORKDIR(); 
      Path += _T('\\');
      Path += NETWORK_INI_FILE;
      // se esiste il file e la entry nel file
      if (gsc_path_exist(Path) == GS_GOOD) dia_gb.Load(Path);

      if ((ret = dia_gb.DoModal()) == IDOK)
		{
         dia_gb.Save(Path);

			tipo_controllo = dia_gb.tipologia;
			controllo_portata = dia_gb.controllo_portata;
			controllo_carico = dia_gb.controllo_carico;
			coeff_molt_portate = dia_gb.coeff_molt_portate;
			densita_relativa = dia_gb.densita_relativa;
			
			min_velocita_prv = dia_gb.min_velocita_prv;
			min_portata_prv = dia_gb.min_portata_prv;

			codice_diamC = dia_gb.codice_diamC;
			numero_iterazioni = dia_gb.iterazioni;
			velocita_approssimazione = dia_gb.velocita_riferimento;
   		}

		if ((ret==IDCANCEL)||(ret==IDABORT)||(ret==-1))
			AfxThrowUserException();

		data = temp_dir;
		data += _T("diam.inp");
         
		// cancello il file 
		if (gsc_path_exist(data) == GS_GOOD)
			_wremove(data);
		
		if (codice_diamC != 0)
		{
			// diametri commerciali
			if (set_diamC.load_from_db(codice_diamC, pConn) == GS_BAD) 
				AfxThrowUserException();

			if (set_diamC.write2file(data, codice_diamC, pConn) == GS_BAD)
				AfxThrowUserException();
		
			codice_diamC = 0;
		}
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return (int) ret;
}


int CHydroMap::OpenDialogParameters_cd_gm(C_DBCONNECTION *pConn)
{
	INT_PTR     ret = GS_GOOD;
   C_STRING Path;

	// When resource from this ARX app is needed, just
	// instantiate a local CTemporaryResourceOverride

	CAcModuleResourceOverride thisResource;

	int		codice_diamC;
	CString	data;
	C_SET_DIAMCOMM	set_diamC;

	CHy_dia_cd_gm	dia_gm;

	TRY
	{
      // leggo da NETWORK.INI nella cartella GEOsimAppl::WORKDIR\NETWORK
      Path = get_WORKDIR(); 
      Path += _T('\\');
      Path += NETWORK_INI_FILE;
      // se esiste il file e la entry nel file
      if (gsc_path_exist(Path) == GS_GOOD) dia_gm.Load(Path);

      if ((ret = dia_gm.DoModal()) == IDOK)
		{
         dia_gm.Save(Path);

			tipo_controllo = dia_gm.tipologia;
			controllo_portata = dia_gm.controllo_portata;
			controllo_carico = dia_gm.controllo_carico;
			coeff_molt_portate = dia_gm.coeff_molt_portate;
			densita_relativa = dia_gm.densita_relativa;
			
			min_velocita_prv = dia_gm.min_velocita_prv;
			min_portata_prv = dia_gm.min_portata_prv;

			codice_diamC = dia_gm.codice_diamC;
			numero_iterazioni = dia_gm.iterazioni;
			velocita_approssimazione = dia_gm.velocita_riferimento;
		}

		if ((ret==IDCANCEL)||(ret==IDABORT)||(ret==-1))
			AfxThrowUserException();

		data = temp_dir;
		data += _T("diam.inp");
         
		// cancello il file 
		if (gsc_path_exist(data) == GS_GOOD)
			_wremove(data);
		
		if (codice_diamC != 0)
		{
			// diametri commerciali
			if (set_diamC.load_from_db(codice_diamC, pConn) == GS_BAD) 
				AfxThrowUserException();

			if (set_diamC.write2file(data, codice_diamC, pConn) == GS_BAD)
				AfxThrowUserException();
		
			codice_diamC = 0;
		}
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return (int) ret;
}

int CHydroMap::SelectNetwork(long *id, int *sub)
{
	int			ret=GS_GOOD;
   long        Internal_id;
   
	ads_name	ent;
	CString		msg;
	CString		cpt;

	TRY
	{
		cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
		msg.LoadString(HY_STRING227); // "Selezionare la rete."

		//if ((ret=CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OKCANCEL | MB_ICONQUESTION)) != IDOK)
		//	AfxThrowUserException();

      acutPrintf(_T("\n"));
      acutPrintf(msg);
		prj = get_GS_CURRENT_WRK_SESSION()->get_PrjId();
		if ((ret = gsc_select_entity(NULL, &Internal_id, NULL, ent, &pCls)) != GS_GOOD)
			AfxThrowUserException();

      if (id) *id = Internal_id;
		
		if (pCls)
		{
			if (sub)
				(*sub) = pCls->ptr_id()->sub_code;

         startSub = pCls->ptr_id()->sub_code;
         startId = Internal_id;
			
			pCls = get_GS_CURRENT_WRK_SESSION()->find_class(pCls->ptr_id()->code);
			cls = pCls->ptr_id()->code;
		}
		else
			AfxThrowUserException();
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::gsc_max_number											*/
/*+
	Determina il massimo numero (cio� composto da tutti 9) che pu� essere 
	contenuto nell'attributo <field> della classe <pCls> calcolandone la 
	lunghezza ed il numero di decimali.
	Parametri input:
   C_CLASS *pCls;    classe
	TCHAR	  *field;   nome attributo
	double  *num;     massimo numero in formato numerico	
    
	Restituisce massimo numero in formato stringa.
-*/  
/****************************************************************************/
TCHAR* CHydroMap::gsc_max_number(C_CLASS *pCls, TCHAR *field, double *num)
{
	int		len, dec, i;
	double	nn;
	TCHAR		*number=NULL;	

	len = ((C_ATTRIB*) (pCls->ptr_attrib_list())->search_name(field, FALSE))->len;
	dec = ((C_ATTRIB*) (pCls->ptr_attrib_list())->search_name(field, FALSE))->dec;
	
	nn=0;
	for (i=len-dec-2; i>=0; i--)
		nn = nn + 9*pow((double)10, i);
	for (i=dec; i>0; i--)
		nn = nn + 9*pow((double)10, -i);

	if (num)
		(*num)=nn;

	number=gsc_tostring(nn, len-dec-1, dec);

	return number;
}


/****************************************************************************/
/*.doc CHydroMap::gsc_min_number											*/
/*+
	Determina il minimo numero (cio� composto da tutti 9) che pu� essere
	contenuto nell'attributo <field> della	classe <pCls> calcolandone
	la lunghezza ed il numero di decimali.
	Parametri input:
   C_CLASS *pCls;    classe
	TCHAR   *field;   nome attributo
	double  *num;     massimo numero in formato numerico	
    
	Restituisce massimo numero in formato stringa.
-*/  
/****************************************************************************/
TCHAR* CHydroMap::gsc_min_number(C_CLASS *pCls, TCHAR *field, double *num)
{
	int		len, dec, i;
	double	nn;
	TCHAR	*number=NULL;	

	len = ((C_ATTRIB*) (pCls->ptr_attrib_list())->search_name(field, FALSE))->len;
	dec = ((C_ATTRIB*) (pCls->ptr_attrib_list())->search_name(field, FALSE))->dec;
	
	nn=0;
	for (i=len-dec-3; i>=0; i--)
		nn = nn + 9*pow((double)10, i);
	for (i=dec; i>0; i--)
		nn = nn + 9*pow((double)10, -i);

	nn=-nn;

	if (num)
		(*num)=nn;

	number=gsc_tostring(nn, len-dec-1, dec);

	return number;
}


/****************************************************************************/
/*.doc CHydroMap::IsValueInDB(...)											*/
/*+
	Esegue lo statement SELECT "pCmd", dopo aver sostituito il parametro 
	"value", e restituisce il recordset dei dati.
	Parametri input:	
   C_PREPARED_CMD &pCmd;   statement compilato
   long			   value;   parametro
	
	output:
   C_RB_LIST &ColValues;		dati letti

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::IsValueInDB(C_PREPARED_CMD &pCmd, long value, C_RB_LIST &ColValues)
{                 
	int		ret=GS_GOOD; 

	TRY
	{
      if (gsc_get_data(pCmd, value, ColValues) == GS_BAD)
			AfxThrowUserException();
	}   
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::UpdData(...)												*/
/*+
	Aggiorna l'oggetto modificando i valori	nel db.

	Parametri
	input:	C_CLASS			*pSub		classe da aggiornare
			C_RB_LIST		&ColValues	nuovi valori
			_RecordsetPtr	&pRs		recordset

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::UpdData(C_CLASS *pSub, C_RB_LIST &ColValues, _RecordsetPtr &pRs)
{                 
	int		ret=GS_GOOD; 

	TRY
	{
		// validazione e ricalcolo dati
		if (pSub->CalcValidData(ColValues, MODIFY) == GS_BAD)
			AfxThrowUserException();

		if (gsc_DBUpdRow(pRs, ColValues) == GS_BAD)
			AfxThrowUserException();
	}   
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::OutSub1													*/
/*+
	Questa funzione tratta i NODI della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output;
	conta inoltre il numero dei NODI.     
   
	Parametri
	input:	C_CLASS	*pSub		classe per determinare la tabella
								sulla quale effettuare la SELECT 
			CString	*fields		campi sui quali effettuare la SELECT
	output:	long	*count		numero dei NODI
			FILE	*filetemp1	file dei dati
			FILE	*filecons1  file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::OutSub1(C_CLASS *pSub, CString *fields, long *count, 
					   FILE *filetemp1, FILE *filecons1)
{
	int           ret = GS_GOOD; 
	C_ID          *pId = pSub->ptr_id();
	C_INFO        *pInfo = pSub->ptr_info();
	CSub1         sub1;
	long          i = 0;	
	long          gs_id, last;
	C_2INT_LONG	  *el = NULL;
	C_SELSET      ss;
	int           in_graphic = TRUE;	
	C_RB_LIST     ColValues;
	presbuf       p = NULL;
	_RecordsetPtr pRs;
	double        dbl;

	TRY
	{
		if (hy_outRecordSet(pSub, fields, NULL, pRs) == GS_BAD) 
			AfxThrowUserException( );

		// Conto le righe che soddisfano l' istruzione SQL
		if (gsc_RowsQty(pRs, count) == GS_BAD)
			AfxThrowUserException( );

		if ((*count) > 0)
		{
         CString msg, msg2;

         msg.LoadString(IDS_STRING274);	// "Scrittura"
         msg2.LoadString(IDS_STRING267);	// "nodi"
         C_STATUSBAR_PROGRESSMETER StatusBarProgressMeter(_T("%s %s"), msg, msg2);
         StatusBarProgressMeter.Init(*count);

			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();
		
			// Scorro il record set 
			while (gsc_isEOF(pRs) == GS_BAD)
			{
            StatusBarProgressMeter.Set(++i);

				// Leggo la riga
				if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il gs_id
				if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
					AfxThrowUserException( );
               
				// controlla se l'entit� � nella lista delle cancellate
				if (deleted_list.get_count()>0)
					el = deleted_list.search(pId->code, pId->sub_code, gs_id);
		
				if ((gs_id>last)||(el))
				{
					el = NULL;

					// ricavo il link set collegato alla scheda
					if (pSub->get_SelSet(gs_id, ss) == GS_BAD)
						AfxThrowUserException();

					if (ss.length() <= 0) in_graphic = FALSE;
				}

				if (in_graphic)
				{				
					sub1.gs_id = gs_id;

					if ((p = ColValues.CdrAssoc(_T("QUOTA"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						dbl = 0;
					else
						// Memorizzo quota
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub1.quota = gsc_tostring(dbl);
					
					if ((p = ColValues.CdrAssoc(_T("CONSUMO"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}					
					if (p->restype == RTNIL)
						dbl = 0;	
					else
						// Memorizzo consumo
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub1.consumo = gsc_tostring(dbl);

					if (fwprintf(filetemp1, _T("%s,%s\n"), 
											gsc_tostring(sub1.gs_id), 
											sub1.quota) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

					if (fwprintf(filecons1, _T("%s\n"), 
											sub1.consumo) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}
				}
				else
				{
					(*count)--;
					in_graphic = TRUE;
				}
				gsc_Skip(pRs);
			}
         StatusBarProgressMeter.End(gsc_msg(1090)); // "Terminato."

			// chiusura record set
			gsc_DBCloseRs(pRs);
		}
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs); 

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}

/****************************************************************************/
/*.doc CHydroMap::OutSub2													*/
/*+
	Questa funzione tratta i LATI della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero dei LATI.     
   
	Parametri
	input:	C_CLASS		*pSub	classe per determinare la tabella sulla
								quale effettuare la SELECT 
			CString		*fields	campi sui quali effettuare la SELECT
   
	output:	long		*count		numero dei LATI
			FILE		*filetemp1	file dei dati
			FILE		*filecons1	file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::OutSub2(C_CLASS *pSub, CString *fields, long *count, 
                       FILE *filetemp1, FILE *filecons1)
{
	int				ret = GS_GOOD; 
	C_CLASS			*prvCls, *valCls, *regCls;
	C_ID			   *pId = pSub->ptr_id();
	C_INFO			*pInfo = pSub->ptr_info();
	C_ATTRIB_LIST	*pAttrib_list = pSub->ptr_attrib_list();
	CSub2		      sub2;
	int			   calc_exist = 0;
	int			   isreg_iniz = FALSE, isreg_final = FALSE, isprv_iniz = FALSE, isprv_final = FALSE;
	long		      i = 0;	
	long		      gs_id, last;
	C_2INT_LONG	   *el = NULL;
	C_SELSET    	ss;
	int			   in_graphic = TRUE;
	C_RB_LIST	   ColValues, ColValues1, ColValues2;
	presbuf		   p = NULL;
	_RecordsetPtr	pRs;
	C_PREPARED_CMD	pCmdPrv, pCmdVal, pCmdReg;
	C_LONG_LIST    ValveList;
	double		   dbl;
	long		      lng;
	int			   integ;
	double		   max_erogazione;
	CString		   str, sr1, sr2;
	C_4LONG		   *item;

	TRY
	{
		if (tipo_fluido == H2O)
		{
			if ((prvCls = gsc_find_class(prj, cls, SUB6)) == NULL)
				AfxThrowUserException();
			if ((valCls = gsc_find_class(prj, cls, SUB5)) == NULL)
				AfxThrowUserException();
		}
		else
		{
			if ((prvCls = gsc_find_class(prj, cls, SUB5)) == NULL)
				AfxThrowUserException();
			if ((valCls = gsc_find_class(prj, cls, SUB4)) == NULL)
				AfxThrowUserException();
			if ((regCls = gsc_find_class(prj, cls, SUB3)) == NULL)
				AfxThrowUserException();
		}

		if (tipo_simulazione == 2)		// calcolo diametri
		{
			if ((pAttrib_list->search_name(_T("CALC"), FALSE))==NULL)
			{
				hy_errmsg(HY_STRING225); // "Il campo <CALC> non esiste."
				AfxThrowUserException();
			}
			else
			{
				calc_exist = 1;   
				(*fields) += _T(", CALC ");
			}
		}
		
		if (hy_outRecordSet(pSub, fields, NULL, pRs) == GS_BAD) 
			AfxThrowUserException( );

		// Conto le righe che soddisfano l' istruzione SQL
		if (gsc_RowsQty(pRs, count) == GS_BAD)
			AfxThrowUserException( );

		if ((*count) > 0)
		{
         CString msg, msg2;

         msg.LoadString(IDS_STRING274);  // "Scrittura"
         msg2.LoadString(IDS_STRING268);	// "lati"
         msg2.LoadString(IDS_STRING271); // "valvole"
         C_STATUSBAR_PROGRESSMETER StatusBarProgressMeter(_T("%s %s"), msg, msg2);
         StatusBarProgressMeter.Init(*count);

			if (regolatoriN > 0)
				// la seguente funzione compila uno statement SQL parametrico di SELECT 
				// per i REGOLATORI della SIMULAZIONE; tale statement permette di verificare 
				// se l'origine di un LATO della SIMULAZIONE coincide con un REGOLATORE.
				if (regCls->prepare_data(pCmdReg, TEMP) == GS_BAD)
					AfxThrowUserException( );
			if (prvN > 0)
				// la seguente funzione compila uno statement SQL parametrico di SELECT 
				// per i PRV della SIMULAZIONE; tale statement permette di verificare 
				// se l'origine di un LATO della SIMULAZIONE coincide con un PRV.
				if (prvCls->prepare_data(pCmdPrv, TEMP) == GS_BAD)
					AfxThrowUserException( );
			if (valvoleN > 0)
				// la seguente funzione compila uno statement SQL parametrico di SELECT
				// per le VALVOLE della SIMULAZIONE; tale statement permette di verificare
				// se la fine di un LATO della SIMULAZIONE coincide con una VALVOLA.
				if (valCls->prepare_data(pCmdVal, TEMP) == GS_BAD)
					AfxThrowUserException( );
				
			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();
		
			// Scorro il record set 
			while (gsc_isEOF(pRs) == GS_BAD)
         {
            StatusBarProgressMeter.Set(++i);

				max_erogazione = -1;

				// Leggo la riga
				if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il key attrib
				if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
					AfxThrowUserException( );
				sub2.gs_id = gs_id;
               
				// controlla se l'entit� � nella lista delle cancellate
				if (deleted_list.get_count()>0)
					el = deleted_list.search(pId->code, pId->sub_code, gs_id);
		
				if ((gs_id > last) || (el))
				{
					el = NULL;

					// ricavo il link set collegato alla scheda
					if (pSub->get_SelSet(gs_id, ss) == GS_BAD)
						AfxThrowUserException();

					if (ss.length() <= 0) in_graphic = FALSE;
				}

				if (in_graphic)
				{
					if ((p = ColValues.CdrAssoc(_T("NODO_INIZ"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					// Memorizzo nodo_iniziale
					if (gsc_rb2Lng(p, &lng) == GS_BAD) 
						AfxThrowUserException( );
					sub2.nodo_iniz = lng;

					if ((p = ColValues.CdrAssoc(_T("NODO_FINAL"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					// Memorizzo nodo_finale
					if (gsc_rb2Lng(p, &lng) == GS_BAD) 
						AfxThrowUserException( );
					sub2.nodo_final = lng;

					if ((p = ColValues.CdrAssoc(_T("LUNGHEZZA"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						dbl = 0;
					else
						// Memorizzo lunghezza
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub2.lunghezza = gsc_tostring(dbl);
				
					if ((p = ColValues.CdrAssoc(_T("DIAMETRO"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						dbl = 100;
					else
						// Memorizzo diametro
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub2.diametro = dbl;

					if (sub2.diametro == 0)
					{ 
						hy_errmsg(HY_STRING226); // "Diametro nullo."
						AfxThrowUserException();
					}

					if (tipo_fluido == H2O)		// acqua
					{
						if ((p = ColValues.CdrAssoc(_T("LEGGE_SC"))) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException( );
						}
						if (p->restype == RTNIL)
							lng = 3;
						else
							// Memorizzo legge_sc
							if (gsc_rb2Lng(p, &lng) == GS_BAD) 
								AfxThrowUserException( );
						sub2.legge_sc = gsc_tostring(lng);
					
						if ((p = ColValues.CdrAssoc(_T("PARAM_SC"))) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException( );
						}
						if (p->restype == RTNIL)
							dbl = 90;
						else
							// Memorizzo param_sc
							if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
								AfxThrowUserException( );
						sub2.param_sc = gsc_tostring(dbl);
					}
					
					if ((p = ColValues.CdrAssoc(_T("PORTATA_D"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						dbl = 0;
					else
						// Memorizzo portata_d
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub2.portata_d = gsc_tostring(dbl);
	
					str = gsc_tostring(sub2.gs_id);
					str += _T(","); 
					str += gsc_tostring(sub2.nodo_iniz);

					isreg_iniz = FALSE;
					isreg_final = FALSE;
					isprv_iniz=FALSE;
					isprv_final=FALSE;

					// la seguente funzione, utilizzando lo statement SQL 
					// parametrico di SELECT per i REGOLATORI della SIMULAZIONE,
					if (regolatoriN>0)														// PAOLETTA
					{
						// verifica se il parametro "nodo_iniz" � un REGOLATORE
						if (sub2.nodo_iniz != 0)
							if (IsValueInDB(pCmdReg, sub2.nodo_iniz, ColValues1) == GS_GOOD)
							{ 
								if ((p = ColValues1.CdrAssoc(_T("MAX_EROGAZIONE"))) == NULL) 
								{ 
									set_GS_ERR_COD(eGSInvRBType);
									AfxThrowUserException();
								}
								if (p->restype == RTNIL)
									max_erogazione = -1;
								else
									if (gsc_rb2Dbl(p, &max_erogazione) == GS_BAD) 
										AfxThrowUserException();

								if (max_erogazione > 0)
								{ 
								   str = gsc_tostring(sub2.gs_id + 4*lastN);
  								   str += ","; 
								   str += gsc_tostring(sub2.nodo_iniz);
									str += _T(","); 
									str += gsc_tostring(sub2.nodo_iniz + 4*lastN);
									str += _T(",1,");
									str += gsc_tostring(sub2.diametro);
									str += _T(",0,0,0\n");
									str += gsc_tostring(sub2.gs_id);
  									str += ","; 
									str += gsc_tostring(sub2.nodo_iniz + 4*lastN);
                           dbl = _wtof(sub2.lunghezza)-1;
                           sub2.lunghezza = gsc_tostring(dbl);
									isreg_iniz = TRUE;
								}
                        else
								{ 
								   str = gsc_tostring(sub2.gs_id);
  								   str += ","; 
								   str += gsc_tostring(sub2.nodo_iniz);
   							}
							}

						// verifica se il parametro "nodo_final" � un REGOLATORE
						if (sub2.nodo_final != 0)
							if (IsValueInDB(pCmdReg, sub2.nodo_final, ColValues1) == GS_GOOD)
							{ 
								if ((p = ColValues1.CdrAssoc(_T("MAX_EROGAZIONE"))) == NULL) 
								{ 
									set_GS_ERR_COD(eGSInvRBType);
									AfxThrowUserException();
								}
								if (p->restype == RTNIL)
									max_erogazione = -1;
								else
									if (gsc_rb2Dbl(p, &max_erogazione) == GS_BAD) 
										AfxThrowUserException();

								if (max_erogazione > 0)
								{ 
								   str = gsc_tostring(sub2.gs_id + 4*lastN);
  								   str += ","; 
								   str += gsc_tostring(sub2.nodo_iniz);
									str += _T(","); 
									str += gsc_tostring(sub2.nodo_final + 4*lastN);
									str += _T(",");
                           dbl = _wtof(sub2.lunghezza)-1;
                           sub2.lunghezza = gsc_tostring(dbl);
									str += sub2.lunghezza;
									str += _T(",");
									str += gsc_tostring(sub2.diametro);
									str += _T(",0,0,0\n");
									str += gsc_tostring(sub2.gs_id);
  									str += ","; 
									str += gsc_tostring(sub2.nodo_final + 4*lastN);
									sub2.lunghezza = "1";
									isreg_final = TRUE;
								}
                        else
								{ 
								   str = gsc_tostring(sub2.gs_id);
  								   str += ","; 
								   str += gsc_tostring(sub2.nodo_iniz);
   							}
							}
					}

					// la seguente funzione, utilizzando lo statement SQL parametrico di SELECT per i PRV
					// della SIMULAZIONE, verifica se il parametro "nodo_iniz" � un PRV.
					if (prvN>0)
					{
						long nodo_rego, altro_nodo;

						if ((sub2.nodo_iniz != 0) && (!isreg_iniz))
						{
							// lettura record
							if (IsValueInDB(pCmdPrv, sub2.nodo_iniz, ColValues1) == GS_GOOD)
							{ 	
								if ((p = ColValues1.CdrAssoc(_T("NODO_REGO"))) == NULL) 
								{ 
									set_GS_ERR_COD(eGSInvRBType);
									AfxThrowUserException();
								}
								if (p->restype == RTNIL)
									nodo_rego = 0;
								else
									if (gsc_rb2Lng(p, &nodo_rego) == GS_BAD)
										AfxThrowUserException();

								if (sub2.nodo_final == nodo_rego)
								{
									altro_nodo = sub2.nodo_final;
									if (valvoleN > 0)
									{
										if (IsValueInDB(pCmdVal, sub2.nodo_final, ColValues2) == GS_GOOD)
											altro_nodo += lastN;
									}

									str = gsc_tostring(sub2.gs_id);
									str += _T(","); 
									str += gsc_tostring(altro_nodo);
									sub2.nodo_final = sub2.nodo_iniz + (3*lastN);
									isprv_iniz = TRUE;
								}
							}				
						}

						if ((sub2.nodo_final != 0) && (!isreg_final))
						{
							if (IsValueInDB(pCmdPrv, sub2.nodo_final, ColValues1) == GS_GOOD)
							{ 	
								if ((p = ColValues1.CdrAssoc(_T("NODO_REGO"))) == NULL) 
								{ 
									set_GS_ERR_COD(eGSInvRBType);
									AfxThrowUserException();
								}
								if (p->restype == RTNIL)
									nodo_rego = 0;
								else
									if (gsc_rb2Lng(p, &nodo_rego) == GS_BAD)
										AfxThrowUserException();

								if (sub2.nodo_iniz == nodo_rego)
								{
									str = gsc_tostring(sub2.gs_id);
									str += _T(","); 
									str += gsc_tostring(sub2.nodo_iniz);
									sub2.nodo_final = sub2.nodo_final + (3*lastN);
									isprv_final=TRUE;
								}
							}
						}
					}
					
					// la seguente funzione, utilizzando lo statement SQL 
					// parametrico di SELECT per le VALVOLE della SIMULAZIONE,
  					if (valvoleN>0)
					{
						// verifica se il parametro "nodo_final" � una VALVOLA.
						if ((sub2.nodo_final != 0) && ((!isreg_final) || (!isprv_final)))
							if (IsValueInDB(pCmdVal, sub2.nodo_final, ColValues1) == GS_GOOD)
							{
								if (!isreg_iniz)
								{
									str = gsc_tostring(sub2.gs_id);
									str += ","; 
									str += gsc_tostring(sub2.nodo_iniz);
								}

								item = NULL;
								item = valve_list.searchLine1(sub2.nodo_final, sub2.gs_id);
								if (item)
									sub2.nodo_final = item->get_valveId();
								else
								{
									item = valve_list.searchLine2(sub2.nodo_final, sub2.gs_id);
									if (item)
										sub2.nodo_final = item->get_newId();
								}
							}

						// verifica se il parametro "nodo_iniz" � una VALVOLA.
						if ((sub2.nodo_iniz != 0) && ((!isreg_iniz) || (!isprv_iniz)))
							if (IsValueInDB(pCmdVal, sub2.nodo_iniz, ColValues1) == GS_GOOD)
							{ 
								item = NULL;
								item = valve_list.searchLine1(sub2.nodo_iniz, sub2.gs_id);
								if (item)
									sub2.nodo_iniz = item->get_valveId();
								else
								{
									item = valve_list.searchLine2(sub2.nodo_iniz, sub2.gs_id);
									if (item)
									{
										sub2.nodo_iniz = item->get_newId();
										sr1 = ",";
										sr1 += gsc_tostring(item->get_valveId());
										sr1 += ",";
										sr2 = ",";
										sr2 += gsc_tostring(sub2.nodo_iniz);
										sr2 += ",";
										str.Replace(sr1, sr2);
									}
								}

								if (!isreg_final)
								{
									str = gsc_tostring(sub2.gs_id);
		  							str += ",";
									str += gsc_tostring(sub2.nodo_iniz);
								}
							}
					}

					str += _T(","); 
					str += gsc_tostring(sub2.nodo_final);
					str += _T(",");
					str += sub2.lunghezza;
					str += _T(",");
					str += gsc_tostring(sub2.diametro);
					str += _T(",");
			
					if (tipo_simulazione == 2)     // calcolo diametri
					{
						if (calc_exist == 1)
						{
							if ((p = ColValues.CdrAssoc(_T("CALC"))) == NULL) 
							{ 
								set_GS_ERR_COD(eGSInvRBType);
								AfxThrowUserException();
							}
							if (p->restype == RTNIL)
								integ = 0;
							else
								if (gsc_rb2Int(p, &integ) == GS_BAD) 
									AfxThrowUserException( );
							sub2.calc = integ;
				
							if (sub2.calc == 0)
								str += _T("0");
							else
								str += _T("1");
						}
						else
						{ 
							hy_errmsg(HY_STRING225); // "Il campo <CALC> non esiste."
							AfxThrowUserException();
						}
					}
					else
						str += _T("0");
					
					str += _T(",");

					if (tipo_fluido == H2O)
					{
						str += sub2.legge_sc;
						str += _T(",");
						str += sub2.param_sc;
					}
					else
						str += _T("0,0");

					if (fwprintf(filetemp1, _T("%s\n"), str) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}
						
					if (max_erogazione > 0)							// PAOLETTA
						if (fwprintf(filecons1, _T("%s\n"), _T("0")) < 0)
						{
							gs_errmsg(eGSWriteFile);
							AfxThrowUserException();
						}


					if (fwprintf(filecons1, _T("%s\n"), sub2.portata_d) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}
				}
				else
				{
					(*count)--;
					in_graphic = TRUE;
				}
				gsc_Skip(pRs);
			}
         StatusBarProgressMeter.End(gsc_msg(1090)); // "Terminato."

			// Chiudo il record set
			gsc_DBCloseRs(pRs);
		}
	}
	CATCH( CUserException, e )
	{
		gsc_DBCloseRs(pRs); 

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::PartialOutSub1													      */
/*+
	Questa funzione tratta i NODI della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output;
	conta inoltre il numero dei NODI.     
   
	Parametri
	input:	C_CLASS	*pSub		classe per determinare la tabella
								sulla quale effettuare la SELECT 
			CString	*fields		campi sui quali effettuare la SELECT
	output:	long	*count		numero dei NODI
			FILE	*filetemp1	file dei dati
			FILE	*filecons1  file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::PartialOutSub1(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList,
                              long *count, 
					               FILE *filetemp1, FILE *filecons1)
{
	int			        ret = GS_GOOD; 
	C_ID		           *pId = pSub->ptr_id();
	C_INFO		        *pInfo = pSub->ptr_info();
	CSub1		           sub1;
	long		           i = 0;
	CString		        msg, msg2;
	long		           gs_id, last;
	C_2INT_LONG	        *el = NULL;
	C_SELSET	           ss;
	int			        in_graphic = TRUE;
	C_RB_LIST	        ColValues;
	presbuf		        p = NULL;
	double			     dbl;
   C_INT_LONG	        *pNode = (C_INT_LONG *) nodesList.get_head();
   C_PREPARED_CMD_LIST CmdList;

   msg.LoadString(IDS_STRING274);	// "Scrittura"
   msg2.LoadString(IDS_STRING267);	// "nodi"
   C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);

	TRY
	{
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

      // Preparo i comandi di lettura dei dati della classe dal temp/old
      if (pSub->prepare_data(CmdList) == GS_BAD) return GS_BAD;

      *count = 0;

		if (pNode)
			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();

      while (pNode)
		{
         if (pNode->get_key() != SUB1)
         {
            pNode = (C_INT_LONG *) nodesList.get_next();
            continue;
         }

         *count = *count + 1;

         if (pSub->query_data(pNode->get_id(), ColValues, &CmdList) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo il gs_id
			if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
				AfxThrowUserException( );
               
         StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."
			
			sub1.gs_id = gs_id;

			if ((p = ColValues.CdrAssoc(_T("QUOTA"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				dbl = 0;
			else
				// Memorizzo quota
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub1.quota = gsc_tostring(dbl);
			
			if ((p = ColValues.CdrAssoc(_T("CONSUMO"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}					
			if (p->restype == RTNIL)
				dbl = 0;	
			else
				// Memorizzo consumo
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub1.consumo = gsc_tostring(dbl);

			if (fwprintf(filetemp1, _T("%s,%s\n"), 
									gsc_tostring(sub1.gs_id), 
									sub1.quota) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

			if (fwprintf(filecons1, _T("%s\n"), 
									sub1.consumo) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

         pNode = (C_INT_LONG *) nodesList.get_next();
      }

      StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::PartialOutSub2													       */
/*+
	Questa funzione tratta i LATI della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero dei LATI.     
   
	Parametri
	input:	C_CLASS		*pSub	classe per determinare la tabella sulla
								quale effettuare la SELECT 
			CString		*fields	campi sui quali effettuare la SELECT
   
	output:	long		*count		numero dei LATI
			FILE		*filetemp1	file dei dati
			FILE		*filecons1	file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::PartialOutSub2(C_CLASS *pSub, CString *fields, 
                              C_INT_LONG_LIST &linksList, long *count, 
                              FILE *filetemp1, FILE *filecons1)
{
	int				     ret = GS_GOOD; 
	C_CLASS			     *prvCls, *valCls, *regCls;
	C_ID			        *pId = pSub->ptr_id();
	C_INFO			     *pInfo = pSub->ptr_info();
	C_ATTRIB_LIST	     *pAttrib_list = pSub->ptr_attrib_list();
	CSub2		           sub2;
	int			        calc_exist=0;
	int			        isreg_iniz = FALSE, isreg_final = FALSE, isprv_iniz = FALSE, isprv_final = FALSE;
	long		           i = 0,  gs_id, last;
	CString		        msg, msg2;
	C_2INT_LONG	        *el = NULL;
	C_SELSET	           ss;
	int			        in_graphic = TRUE;
	C_RB_LIST	        ColValues, ColValues1, ColValues2;
	presbuf		        p = NULL;
	C_PREPARED_CMD      pCmdPrv, pCmdVal, pCmdReg;
	double		        dbl, max_erogazione;
	long		           lng;
	int			        integ;
	CString		        str, sr1, sr2;
	C_INT_LONG	        *pLink = (C_INT_LONG *) linksList.get_head();
	C_PREPARED_CMD_LIST CmdList;
	C_LONG_LIST         ValveList;
	C_4LONG		        *item;

   msg.LoadString(IDS_STRING274);	// "Scrittura"
   msg2.LoadString(IDS_STRING268);	// "lati"
   C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);

	TRY
	{
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

		// Preparo i comandi di lettura dei dati della classe dal temp/old
		if (pSub->prepare_data(CmdList) == GS_BAD) return GS_BAD;

		*count = 0;

		if (tipo_fluido == H2O)
		{
			if ((prvCls = gsc_find_class(prj, cls, SUB6)) == NULL)
				AfxThrowUserException();
			if ((valCls = gsc_find_class(prj, cls, SUB5)) == NULL)
				AfxThrowUserException();
		}
		else
		{
			if ((prvCls = gsc_find_class(prj, cls, SUB5)) == NULL)
				AfxThrowUserException();
			if ((valCls = gsc_find_class(prj, cls, SUB4)) == NULL)
				AfxThrowUserException();
			if ((regCls = gsc_find_class(prj, cls, SUB3)) == NULL)
				AfxThrowUserException();
		}

		if (tipo_simulazione == 2)		// calcolo diametri
		{
			if ((pAttrib_list->search_name(_T("CALC"), FALSE))==NULL)
			{
				hy_errmsg(HY_STRING225); // "Il campo <CALC> non esiste."
				AfxThrowUserException();
			}
			else
			{
				calc_exist = 1;   
				(*fields) += _T(", CALC ");
			}
		}
		
		if (pLink)
		{
			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();

			if (regolatoriN > 0)				
				// la seguente funzione compila uno statement SQL parametrico di SELECT
				// per i REGOLATORI della SIMULAZIONE; tale statement permette di verificare
				// se la fine di un LATO della SIMULAZIONE coincide con un REGOLATORE.
				if (regCls->prepare_data(pCmdReg, TEMP) == GS_BAD)
					AfxThrowUserException( );

			if (prvN > 0)
				// la seguente funzione compila uno statement SQL parametrico di SELECT 
				// per i PRV della SIMULAZIONE; tale statement permette di verificare 
				// se l'origine di un LATO della SIMULAZIONE coincide con un PRV.
				if (prvCls->prepare_data(pCmdPrv, TEMP) == GS_BAD)
					AfxThrowUserException( );

			if (valvoleN > 0)
				// la seguente funzione compila uno statement SQL parametrico di SELECT
				// per le VALVOLE della SIMULAZIONE; tale statement permette di verificare
				// se la fine di un LATO della SIMULAZIONE coincide con una VALVOLA.
				if (valCls->prepare_data(pCmdVal, TEMP) == GS_BAD)
					AfxThrowUserException( );
		}

		while (pLink)
		{
			*count = *count + 1;
			max_erogazione = -1;

			if (pSub->query_data(pLink->get_id(), ColValues, &CmdList) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo il key attrib
			if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
				AfxThrowUserException( );
			sub2.gs_id = gs_id;
               
			StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."
			
			if ((p = ColValues.CdrAssoc(_T("NODO_INIZ"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo nodo_iniziale
			if (gsc_rb2Lng(p, &lng) == GS_BAD) 
				AfxThrowUserException( );
			sub2.nodo_iniz = lng;

			if ((p = ColValues.CdrAssoc(_T("NODO_FINAL"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo nodo_finale
			if (gsc_rb2Lng(p, &lng) == GS_BAD) 
				AfxThrowUserException( );
			sub2.nodo_final = lng;

			if ((p = ColValues.CdrAssoc(_T("LUNGHEZZA"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				dbl = 0;
			else
				// Memorizzo lunghezza
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub2.lunghezza = gsc_tostring(dbl);
			
			if ((p = ColValues.CdrAssoc(_T("DIAMETRO"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				dbl = 100;
			else
				// Memorizzo lunghezza
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub2.diametro = dbl;

			if (sub2.diametro == 0)
			{ 
				hy_errmsg(HY_STRING226); // "Diametro nullo."
				AfxThrowUserException();
			}

			if (tipo_fluido == H2O)		// acqua
			{
				if ((p = ColValues.CdrAssoc(_T("LEGGE_SC"))) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				if (p->restype == RTNIL)
					lng = 3;
				else
					// Memorizzo legge_sc
					if (gsc_rb2Lng(p, &lng) == GS_BAD) 
						AfxThrowUserException( );
				sub2.legge_sc = gsc_tostring(lng);
			
				if ((p = ColValues.CdrAssoc(_T("PARAM_SC"))) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				if (p->restype == RTNIL)
					dbl = 90;
				else
					// Memorizzo param_sc
					if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
						AfxThrowUserException( );
				sub2.param_sc = gsc_tostring(dbl);
			}
			
			if ((p = ColValues.CdrAssoc(_T("PORTATA_D"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				dbl = 0;
			else
				// Memorizzo param_sc
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub2.portata_d = gsc_tostring(dbl);

			str = gsc_tostring(sub2.gs_id);
			str += _T(","); 
			str += gsc_tostring(sub2.nodo_iniz);
	
			isreg_iniz = FALSE;
			isreg_final = FALSE;
			isprv_iniz = FALSE;
			isprv_final = FALSE;

			// la seguente funzione, utilizzando lo statement SQL 
			// parametrico di SELECT per i REGOLATORI della SIMULAZIONE,
			if (regolatoriN>0)													// PAOLETTA
			{
				// verifica se il parametro "nodo_iniz" � un REGOLATORE
				if (sub2.nodo_iniz != 0)
					if (IsValueInDB(pCmdReg, sub2.nodo_iniz, ColValues1) == GS_GOOD)
					{ 
						if ((p = ColValues1.CdrAssoc(_T("MAX_EROGAZIONE"))) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException();
						}
						if (p->restype == RTNIL)
							max_erogazione = -1;
						else
							if (gsc_rb2Dbl(p, &max_erogazione) == GS_BAD) 
								AfxThrowUserException();

						if (max_erogazione > 0)
						{ 
						   str = gsc_tostring(sub2.gs_id + 4*lastN);
						   str += ","; 
						   str += gsc_tostring(sub2.nodo_iniz);
							str += _T(","); 
							str += gsc_tostring(sub2.nodo_iniz + 4*lastN);
							str += _T(",1,");
							str += gsc_tostring(sub2.diametro);
							str += _T(",0,0,0\n");
							str += gsc_tostring(sub2.gs_id);
							str += ","; 
							str += gsc_tostring(sub2.nodo_iniz + 4*lastN);
                     dbl = _wtof(sub2.lunghezza)-1;
                     sub2.lunghezza = gsc_tostring(dbl);
							isreg_iniz = TRUE;
						}
                  else
						{ 
						   str = gsc_tostring(sub2.gs_id);
						   str += ","; 
						   str += gsc_tostring(sub2.nodo_iniz);
   					}
					}

				// verifica se il parametro "nodo_final" � un REGOLATORE
				if (sub2.nodo_final != 0)
					if (IsValueInDB(pCmdReg, sub2.nodo_final, ColValues1) == GS_GOOD)
					{ 
						if ((p = ColValues1.CdrAssoc(_T("MAX_EROGAZIONE"))) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException();
						}

						if (p->restype == RTNIL)
							max_erogazione = -1;
						else
							if (gsc_rb2Dbl(p, &max_erogazione) == GS_BAD) 
								AfxThrowUserException();

						if (max_erogazione > 0)
						{ 
						   str = gsc_tostring(sub2.gs_id + 4*lastN);
						   str += ","; 
						   str += gsc_tostring(sub2.nodo_iniz);
							str += _T(","); 
							str += gsc_tostring(sub2.nodo_final + 4*lastN);
							str += _T(",");
                     dbl = _wtof(sub2.lunghezza)-1;
                     sub2.lunghezza = gsc_tostring(dbl);
							str += sub2.lunghezza;
							str += _T(",");
							str += gsc_tostring(sub2.diametro);
							str += _T(",0,0,0\n");
							str += gsc_tostring(sub2.gs_id);
							str += ","; 
							str += gsc_tostring(sub2.nodo_final + 4*lastN);
							sub2.lunghezza = "1";
							isreg_final = TRUE;
						}
                  else
						{ 
						   str = gsc_tostring(sub2.gs_id);
						   str += ","; 
						   str += gsc_tostring(sub2.nodo_iniz);
   					}
					}
			}

			// la seguente funzione, utilizzando lo statement SQL parametrico di SELECT per i PRV
			// della SIMULAZIONE, verifica se il parametro "nodo_iniz" � un PRV.
			if (prvN>0)
			{
				long nodo_rego, altro_nodo;

				if ((sub2.nodo_iniz != 0) && (!isreg_iniz))
				{
					// lettura record
					if (IsValueInDB(pCmdPrv, sub2.nodo_iniz, ColValues1) == GS_GOOD)
					{ 	
						if ((p = ColValues1.CdrAssoc(_T("NODO_REGO"))) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException();
						}
						if (p->restype == RTNIL)
							nodo_rego = 0;
						else
							if (gsc_rb2Lng(p, &nodo_rego) == GS_BAD)
								AfxThrowUserException();

						if (sub2.nodo_final == nodo_rego)
						{
							altro_nodo = sub2.nodo_final;
							if (valvoleN > 0)
							{
								if (IsValueInDB(pCmdVal, sub2.nodo_final, ColValues2) == GS_GOOD)
									altro_nodo += lastN;
							}

							str = gsc_tostring(sub2.gs_id);
							str += _T(","); 
							str += gsc_tostring(altro_nodo);
							sub2.nodo_final = sub2.nodo_iniz + (3*lastN);
							isprv_iniz = TRUE;
						}
					}				
				}

				if ((sub2.nodo_final != 0) && (!isreg_final))
				{
					if (IsValueInDB(pCmdPrv, sub2.nodo_final, ColValues1) == GS_GOOD)
					{ 	
						if ((p = ColValues1.CdrAssoc(_T("NODO_REGO"))) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException();
						}
						if (p->restype == RTNIL)
							nodo_rego = 0;
						else
							if (gsc_rb2Lng(p, &nodo_rego) == GS_BAD)
								AfxThrowUserException();

						if (sub2.nodo_iniz == nodo_rego)
						{
							str = gsc_tostring(sub2.gs_id);
							str += _T(","); 
							str += gsc_tostring(sub2.nodo_iniz);
							sub2.nodo_final = sub2.nodo_final + (3*lastN);
							isprv_final=TRUE;
						}
					}
				}
			}
			
			// la seguente funzione, utilizzando lo statement SQL 
			// parametrico di SELECT per le VALVOLE della SIMULAZIONE,
			if (valvoleN>0)
			{
				// verifica se il parametro "nodo_final" � una VALVOLA.
				if ((sub2.nodo_final != 0) && ((!isreg_final) || (!isprv_final)))
					if (IsValueInDB(pCmdVal, sub2.nodo_final, ColValues1) == GS_GOOD)
					{ 
						if (!isreg_iniz)
						{
							str = gsc_tostring(sub2.gs_id);
							str += ","; 
							str += gsc_tostring(sub2.nodo_iniz);
						}

						item = NULL;
						item = valve_list.searchLine1(sub2.nodo_final, sub2.gs_id);
						if (item)
							sub2.nodo_final = item->get_valveId();
						else
						{
							item = valve_list.searchLine2(sub2.nodo_final, sub2.gs_id);
							if (item)
								sub2.nodo_final = item->get_newId();
						}
					}

				// verifica se il parametro "nodo_iniz" � una VALVOLA.
				if ((sub2.nodo_iniz != 0) && ((!isreg_iniz) || (!isprv_iniz)))
					if (IsValueInDB(pCmdVal, sub2.nodo_iniz, ColValues1) == GS_GOOD)
					{ 
						item = NULL;
						item = valve_list.searchLine1(sub2.nodo_iniz, sub2.gs_id);
						if (item)
							sub2.nodo_iniz = item->get_valveId();
						else
						{
							item = valve_list.searchLine2(sub2.nodo_iniz, sub2.gs_id);
							if (item)
							{
								sub2.nodo_iniz = item->get_newId();
								sr1 = ",";
								sr1 += gsc_tostring(item->get_valveId());
								sr1 += ",";
								sr2 = ",";
								sr2 += gsc_tostring(sub2.nodo_iniz);
								sr2 += ",";
								str.Replace(sr1, sr2);
							}
						}

						if (!isreg_final)
						{
							str = gsc_tostring(sub2.gs_id);
  							str += ",";
							str += gsc_tostring(sub2.nodo_iniz);
						}
					}
			}

			str += _T(","); 
			str += gsc_tostring(sub2.nodo_final);
			str += _T(",");
			str += sub2.lunghezza;
			str += _T(",");
			str += gsc_tostring(sub2.diametro);
			str += _T(",");
		
			if (tipo_simulazione == 2)     // calcolo diametri
			{
				if (calc_exist == 1)
				{
					if ((p = ColValues.CdrAssoc(_T("CALC"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException();
					}
					if (p->restype == RTNIL)
						integ = 0;
					else
						if (gsc_rb2Int(p, &integ) == GS_BAD) 
							AfxThrowUserException( );
					sub2.calc = integ;
		
					if (sub2.calc == 0)
						str += _T("0");
					else
						str += _T("1");
				}
				else
				{ 
					hy_errmsg(HY_STRING225); // "Il campo <CALC> non esiste."
					AfxThrowUserException();
				}
			}
			else
				str += _T("0");
			
			str += _T(",");

			if (tipo_fluido == H2O)
			{
				str += sub2.legge_sc;
				str += _T(",");
				str += sub2.param_sc;
			}
			else
				str += _T("0,0");

			if (fwprintf(filetemp1, _T("%s\n"), str) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
			
			if (max_erogazione > 0)							// PAOLETTA
				if (fwprintf(filecons1, _T("%s\n"), _T("0")) < 0)
				{
					gs_errmsg(eGSWriteFile);
					AfxThrowUserException();
				}

			if (fwprintf(filecons1, _T("%s\n"), sub2.portata_d) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		
         pLink = (C_INT_LONG *) linksList.get_next();
      }

      StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::OutSub3													*/
/*+
	Questa funzione tratta i SERBATOI/REGOLATORI della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero dei SERBATOI/REGOLATORI.     
   
	Parametri
	input:	C_CLASS *pSub		classe per determinare la tabella 
								sulla quale effettuare la SELECT 
			CString	*fields		campi sui quali effettuare la SELECT
   
	output:	long	*count			numero dei SERBATOI/REGOLATORI
			long	*countR			numero dei REGOLATORI fittizi
			FILE	*filetemp1		file dei dati
			FILE	*filecons1		file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::OutSub3(C_CLASS *pSub, CString *fields, long *count, long *countR, 
                       FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2)
{
	int            ret = GS_GOOD; 
	C_ID           *pId = pSub->ptr_id();
	C_INFO         *pInfo = pSub->ptr_info();
	CSub3	         sub3;
	long		      i = 0, gs_id, last, countL;	
	CString		   idMsg, strMsg;
	C_2INT_LONG	   *el = NULL;
	C_SELSET	      ss;
	int			   in_graphic = TRUE;
	C_RB_LIST	   ColValues;
	presbuf		   p = NULL;
	_RecordsetPtr	pRs;
	double		   dbl;
   C_CLASS			*latiCls;
	C_STRING		   statement, TableRef;
	C_DBCONNECTION	*pConn;
	_RecordsetPtr	pRSub2;

	TRY
	{
		if (hy_outRecordSet(pSub, fields, NULL, pRs)==GS_BAD) 
			AfxThrowUserException( );

		// Conto le righe che soddisfano l' istruzione SQL
		if (gsc_RowsQty(pRs, count) == GS_BAD)
			AfxThrowUserException( );	

		if ((*count) > 0)
		{
         CString msg, msg2;

         msg.LoadString(IDS_STRING274);	// "Scrittura"
	      if (tipo_fluido == H2O) // acqua
		      msg2.LoadString(IDS_STRING269); // "serbatoi"
	      else
		      msg2.LoadString(IDS_STRING273); // "regolatori"
         C_STATUSBAR_PROGRESSMETER StatusBarProgressMeter(_T("%s %s"), msg, msg2);
         StatusBarProgressMeter.Init(*count);

			if ((latiCls = gsc_find_class(prj, cls, SUB2)) == NULL)		// PAOLETTA
				AfxThrowUserException();

			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();

			idMsg = "";
		
			// Scorro il record set 
			while (gsc_isEOF(pRs) == GS_BAD)
			{
            StatusBarProgressMeter.Set(++i);

				// Leggo la riga
				if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il key attrib
				if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
					AfxThrowUserException( );
               
				// controlla se l'entit� � nella lista delle cancellate
				if (deleted_list.get_count()>0)
					el = deleted_list.search(pId->code, pId->sub_code, gs_id);
		
				if ((gs_id>last)||(el))
				{
					el = NULL;

					// ricavo il link set collegato alla scheda
					if (pSub->get_SelSet(gs_id, ss) == GS_BAD)
						AfxThrowUserException();

					if (ss.length() <= 0) in_graphic = FALSE;
				}

				if (in_graphic)
				{					
					sub3.gs_id = gs_id;

					if (tipo_fluido == H2O) // acqua
					{
						if ((p = ColValues.CdrAssoc(_T("QUOTA_PL"))) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException( );
						}

						if (p->restype == RTNIL)
							dbl = 0;
						else
							// Memorizzo quota
							if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
								AfxThrowUserException( );
						sub3.quota = gsc_tostring(dbl);

						if (fwprintf(filetemp1, _T("%s,%s\n"), 
												gsc_tostring(sub3.gs_id), 
												sub3.quota) < 0)
						{
							gs_errmsg(eGSWriteFile);
							AfxThrowUserException();
						}
					}
					else // gas
					{
						if ((p = ColValues.CdrAssoc(_T("QUOTA"))) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException( );
						}

						if (p->restype == RTNIL)
							dbl = 0;
						else
							// Memorizzo quota
							if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
								AfxThrowUserException( );
						sub3.quota = gsc_tostring(dbl);

						// regolatori di taglio
   						if (!(p = ColValues.CdrAssoc(_T("MAX_EROGAZIONE"))) || p->restype == RTNIL)
							dbl = -1;
						else
							// Memorizzo max_erogazione
							if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
								AfxThrowUserException( );
						sub3.max_erogazione = gsc_tostring(dbl);

// roby per regolatori di taglio
#ifdef REGOLATORI_DI_TAGLIO
						if (fwprintf(filetemp1, _T("%s,%s,%s\n"), 
											gsc_tostring(sub3.gs_id), 
											sub3.quota,
											sub3.max_erogazione) < 0)
						{
							gs_errmsg(eGSWriteFile);
							AfxThrowUserException();
						}

						if (sub3.max_erogazione != "-1")		// PAOLETTA
						{	
							*countR = *countR + 1;

							if (latiCls->getTempTableRef(TableRef)==GS_BAD)
								AfxThrowUserException();

							statement = _T("SELECT GS_ID FROM ");
							statement += TableRef;
							statement += _T(" WHERE NODO_INIZ=");
							statement += sub3.gs_id;
							statement += _T(" OR NODO_FINAL=");
							statement += sub3.gs_id;
				        
							// Ricavo la connessione OLE - DB
							if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
								AfxThrowUserException();

							// Eseguo l' istruzione e ricavo il record set
							if (pConn->OpenRecSet(statement, pRSub2, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
								AfxThrowUserException();

							// Conto le righe che soddisfano l' istruzione SQL
							if (gsc_RowsQty(pRSub2, &countL) == GS_BAD)
								AfxThrowUserException( );

							if (countL > 1)
							{
								if (idMsg.GetLength() > 0)
									idMsg += " - ";
								idMsg += gsc_tostring(gs_id);
							}
							else
							{
								if (fwprintf(filetemp2, _T("%s,%s\n"), 
												gsc_tostring(sub3.gs_id + 4*lastN), 
												sub3.quota) < 0)
								{
									gs_errmsg(eGSWriteFile);
									AfxThrowUserException();
								}
								if (fwprintf(filecons2, _T("%s\n"),	_T("0")) < 0)
								{
									gs_errmsg(eGSWriteFile);
									AfxThrowUserException();
								}
							}
						}
#else
						if (fwprintf(filetemp1, _T("%s,%s\n"), 
											gsc_tostring(sub3.gs_id), 
											sub3.quota) < 0)
						{
							gs_errmsg(eGSWriteFile);
							AfxThrowUserException();
						}
#endif
					}

					if (tipo_fluido == H2O) // ACQUA
						sub3.pressione = sub3.quota;
					else
					{
						if (tipo_fluido == GB) // gas bassa
						{              
							if ((p = ColValues.CdrAssoc(_T("PRESSIONEB"))) == NULL) 
							{ 
								set_GS_ERR_COD(eGSInvRBType);
								AfxThrowUserException( );
							}
						}
						else
							if ((p = ColValues.CdrAssoc(_T("PRESSIONEM"))) == NULL) 
							{ 
								set_GS_ERR_COD(eGSInvRBType);
								AfxThrowUserException( );
							}
						if (p->restype == RTNIL)
							dbl=0;
						else
							if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
								AfxThrowUserException( );
						sub3.pressione = gsc_tostring(dbl);
					}
					
					if (fwprintf(filecons1, _T("%s\n"),	sub3.pressione) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}
				}
				else
				{
					(*count)--;
					in_graphic = TRUE;
				}
				gsc_Skip(pRs);
			}
         StatusBarProgressMeter.End(gsc_msg(1090)); // "Terminato."

			// chiusura record set
			gsc_DBCloseRs(pRs);

			if (idMsg.GetLength() > 0)
			{
				strMsg = "\nI regolatori ";
				strMsg += idMsg;
				strMsg += " presentano pi� linee di uscita, la simulazione viene interrotta in quanto i risultati non sarebbero attendibili. Modificare affinch� da ciascun regolatore esca una singola linea.";
				acutPrintf(strMsg);
				AfxThrowUserException( );
			}
		}
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::OutSub4													*/
/*+
	Questa funzione tratta le POMPE della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero di POMPE.     
   
	Parametri
	input:	C_CLASS	*pSub	classe per determinare la tabella sulla quale
							effettuare la SELECT 
			CString	*fields	campi sui quali effettuare la SELECT
   
	output:	long	*count		numero dei nodi
			FILE	*filetemp1	file dei dati
			FILE	*filetemp2	altro file dei dati
			FILE	*filecons1	file dei consumi
			FILE	*filecons2	altro file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::OutSub4(C_CLASS *pSub, CString *fields, long *count, FILE *filetemp1,
					        FILE *filetemp2, FILE *filecons1, FILE *filecons2)
{
	int           ret = GS_GOOD; 
	C_ID          *pId = pSub->ptr_id();
	C_INFO        *pInfo = pSub->ptr_info();
	CSub4	        sub4;
	long          i = 0, gs_id, last, numpompe = 0, lng;
	C_2INT_LONG	  *el = NULL;
	C_SELSET	     ss;
	int           j, in_graphic = TRUE;
	C_RB_LIST     ColValues;
	presbuf       p = NULL;
	_RecordsetPtr pRs;
	double        dbl, h, q;
	CString       curva, temp, cons, stpf, pompe;
	FILE	        *filetemp = NULL, *filecons = NULL, *filestp = NULL, *filepompe = NULL;
	TCHAR         *entry = NULL, *buf = NULL;
	CString	     str, stp;
   bool          Unicode = false;

	TRY
	{
		if (hy_outRecordSet(pSub, fields, NULL, pRs)==GS_BAD) 
			AfxThrowUserException( );

		// Conto le righe che soddisfano l' istruzione SQL
		if (gsc_RowsQty(pRs, count) == GS_BAD)
			AfxThrowUserException( );

		if ((*count) > 0)
		{
         CString msg, msg2;

         msg.LoadString(IDS_STRING274);  // "Scrittura"
         msg2.LoadString(IDS_STRING270); // "pompe"
         C_STATUSBAR_PROGRESSMETER StatusBarProgressMeter(_T("%s %s"), msg, msg2);
         StatusBarProgressMeter.Init(*count);

			temp = temp_dir;
			temp += _T("temp3.inp");
         
			if ((filetemp = gsc_fopen(temp, _T("w"), MORETESTS, &Unicode)) == NULL)
			{ 
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();
			}

			cons = temp_dir;
			cons += _T("cons3.inp");

			if ((filecons = gsc_fopen(cons, _T("w"), MORETESTS, &Unicode)) == NULL)
			{ 
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();
			}

			stpf = temp_dir;
			stpf += _T("stp.inp");

         if ((filestp = gsc_fopen(stpf, _T("w"), MORETESTS, &Unicode)) == NULL)
			{ 
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();
			}

         // Prima provo nel direttorio del progetto
			pompe = get_GS_CURRENT_WRK_SESSION()->get_pPrj()->get_dir();
			pompe += _T("\\pompe.ini");

			if ((filepompe = gsc_open_profile(pompe, READONLY, MORETESTS, &Unicode)) == NULL)
			{   
            // Provo nel direttorio EXT sotto il direttorio server di GEOsim
				pompe = get_GEODIR();
				pompe += _T("\\");
				pompe += GEOEXT;
				pompe += _T("\\pompe.ini");

				if ((filepompe = gsc_open_profile(pompe, READONLY, MORETESTS, &Unicode)) == NULL)
				{ 
					hy_errmsg(HY_STRING222); // "Il file <pompe.ini>, contenente le curve caratteristiche delle pompe, non esiste."
					AfxThrowUserException();
				}
			}

			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();
		
			// Scorro il record set 
			while (gsc_isEOF(pRs) == GS_BAD)
         {
            StatusBarProgressMeter.Set(++i);

				// Leggo la riga
				if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il key attrib
				if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
					AfxThrowUserException( );
               
				// controlla se l'entit� � nella lista delle cancellate
				if (deleted_list.get_count()>0)
					el = deleted_list.search(pId->code, pId->sub_code, gs_id);
		
				if ((gs_id>last)||(el))
				{
					el = NULL;

					// ricavo il link set collegato alla scheda
					if (pSub->get_SelSet(gs_id, ss) == GS_BAD)
						AfxThrowUserException();

					if (ss.length() <= 0) in_graphic = FALSE;
				}

				if (in_graphic)
				{
					sub4.gs_id = gs_id;

					if ((p = ColValues.CdrAssoc(_T("QUOTA_ASP"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					
					if (p->restype == 0)
						dbl=0;
					else
						// Memorizzo quota_asp
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );					
					sub4.quota_asp = dbl;
					
					if ((p = ColValues.CdrAssoc(_T("CURVA"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						curva = _T("standard");
					else
						curva = gsc_rb2str(p);
					sub4.curva = curva;

					if ((p = ColValues.CdrAssoc(_T("Q_NOM"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvAttribName);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						dbl=1;
					else
						// Memorizzo q_nom
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub4.q_nom = dbl;

					if ((p = ColValues.CdrAssoc(_T("H_NOM"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvAttribName);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						dbl=1;
					else
						// Memorizzo q_nom
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub4.h_nom = dbl;

					if ((p = ColValues.CdrAssoc(_T("ON_OFF"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvAttribName);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						lng=0;
					else
						// Memorizzo q_nom
						if (gsc_rb2Lng(p, &lng) == GS_BAD) 
							AfxThrowUserException( );
					sub4.on_off = lng;
					
					numpompe++;

					str = gsc_tostring((2*lastN) + numpompe);

					if (sub4.quota_asp == 0)
						sub4.quota_asp = (double)0.1;
					
					str += _T(",");
					str += gsc_tostring(sub4.quota_asp);

					if (fwprintf(filetemp1, _T("%s\n"), str) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

					str = gsc_tostring(gs_id);
					str += _T(",");
					str += gsc_tostring(sub4.quota_asp);

					if (fwprintf(filetemp, _T("%s\n"), str) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

					str = gsc_tostring(sub4.quota_asp);

					if (fwprintf(filecons1, _T("%s\n"), str) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}
			
					if (fwprintf(filecons, _T("%s\n"), _T("0")) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

               // lato fittizio
               str = _T("0,");
					str += gsc_tostring((2*lastN) + numpompe);
					str += _T(",");
					str += gsc_tostring(gs_id);
					str += _T(",1,100,0,");
					str += gsc_tostring(20 + numpompe);
					str += _T(",90");

					if (fwprintf(filetemp2, _T("%s\n"), str) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}
					
					if (fwprintf(filecons2, _T("%s\n"), ((sub4.on_off==0) ? _T("0") : _T("-1"))) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}
					
					for (j=0; j<10; j++)
					{
						entry = gsc_tostring(j);
                  // Leggo in ANSI
						if (gsc_get_profile(filepompe, sub4.curva, entry, &buf, 0 , 0, false) == GS_BAD) 
						{ 
							hy_errmsg(HY_STRING224); // "La curva caratteristica di una pompa della rete non � definita nel file <pompe.ini>."
							AfxThrowUserException();
						}
               
						gsc_strsep(buf, _T(' '));
						swscanf(buf, _T("%lf%lf"), &h, &q);
						free(entry);
						free(buf);                 
						entry=NULL;
						buf=NULL;            

						h=(double)(h*sub4.h_nom);
						q=(double)(q*sub4.q_nom);

						stp = gsc_tostring(q);
						stp += _T(",");
						stp += gsc_tostring(h);

						if (fwprintf(filestp, _T("%s\n"), stp) < 0)
						{
							gs_errmsg(eGSWriteFile);
							AfxThrowUserException();
						}
					}
				}
				else
				{
					(*count)--;
					in_graphic = TRUE;
				}
				gsc_Skip(pRs);
         }
         StatusBarProgressMeter.End(gsc_msg(1090)); // "Terminato."

			// chiusura record set
			gsc_DBCloseRs(pRs);

			if (gsc_close_profile(filepompe) == GS_BAD) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}

			if (fclose(filetemp) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			if ((filetemp = gsc_fopen(temp, _T("r"), MORETESTS, &Unicode)) == NULL)
			{
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();
			}

			if (fclose(filecons) == EOF)
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			if ((filecons = gsc_fopen(cons, _T("r"), MORETESTS, &Unicode)) == NULL)
			{
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();
			}

			int c;
			while ((c=getc(filetemp))!=EOF)
			{      
				if (putc(c,filetemp1)==EOF)
				{
					gs_errmsg(eGSWriteFile);
					AfxThrowUserException();
				}
			}

			while ((c=getc(filecons))!=EOF)
			{      
				if (putc(c,filecons1)==EOF)
				{
					gs_errmsg(eGSWriteFile);
					AfxThrowUserException();
				}
			}

			if (fclose(filetemp) == EOF)
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			if (gsc_path_exist(temp)==GS_GOOD)
				_wremove(temp);
      
			if (fclose(filecons) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			if (gsc_path_exist(cons)==GS_GOOD)
				_wremove(cons);

			if (fclose(filestp) == EOF)
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
		}       
	}

	CATCH( CUserException, e )
	{
		if (filetemp)
		{
			fclose(filetemp);
			filetemp = NULL;
		}		

		if (filecons)
		{
			fclose(filecons);
			filecons = NULL;
		}		
		
		if (filestp)
		{
			fclose(filestp);
			filestp = NULL;
		}		

		if (filepompe)
		{
			fclose(filepompe);
			filepompe = NULL;
		}		

		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH
	
	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::OutSub5													*/
/*+
	Questa funzione tratta le VALVOLE della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero delle VALVOLE.     
   
	Parametri
	input:	C_CLASS	*pSub   classe per determinare la tabella sulla 
							quale effettuare la SELECT 
            CString	*fields	campi sui quali effettuare la SELECT
	
	output:	long	*count		numero delle VALVOLE
			FILE	*filetemp1	file dei dati
			FILE	*filetemp2	altro file dei dati
			FILE	*filecons1	file dei consumi
			FILE	*filecons2	altro file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::OutSub5(C_CLASS *pSub, CString *fields, long *count, FILE *filetemp1, 
                       FILE *filetemp2, FILE *filecons1, FILE *filecons2)
{
	int            ret = GS_GOOD;
	C_ID		      *pId = pSub->ptr_id();
	C_INFO         *pInfo = pSub->ptr_info();
	CSub5		      sub5;
	long           i = 0, lng, gs_idL, line1Id, line2Id;
	long		      gs_id, last;
	C_2INT_LONG		*el = NULL;
	C_SELSET	      ss;
	int            in_graphic = TRUE;
	C_RB_LIST	   ColValues, ColValuesSub2;
	presbuf		   p = NULL;
	_RecordsetPtr	pRs;
	double		   dbl;
	CString		   str;
	C_CLASS			*latiCls;
	C_STRING		   statement, TableRef;
	C_DBCONNECTION	*pConn;
	_RecordsetPtr	pRSub2;
	C_4LONG			*item;

	TRY
	{
		if ((latiCls = gsc_find_class(prj, cls, SUB2)) == NULL)		// PAOLETTA
			AfxThrowUserException();

		if (hy_outRecordSet(pSub, fields, NULL, pRs)==GS_BAD) 
			AfxThrowUserException( );

      // nella versione compilata a 64 bit da un pc a 32 lanciando gsc_RowsQty prvN va a 0
      // ci deve essere qualche problema di memoria e come super tapullo uso oldPrvN
      // per ripristinarne il valore
      long oldPrvN = prvN;
//acutPrintf(_T("\nNumero prvN:%ld\n"), prvN);
		// Conto le righe che soddisfano l' istruzione SQL
		if (gsc_RowsQty(pRs, count) == GS_BAD)
			AfxThrowUserException( );
//acutPrintf(_T("\nNumero prvN:%ld\n"), prvN);
      prvN = oldPrvN;

		if ((*count) > 0)
		{
         CString msg, msg2;

         msg.LoadString(IDS_STRING274);  // "Scrittura"
         msg2.LoadString(IDS_STRING271); // "valvole"
         C_STATUSBAR_PROGRESSMETER StatusBarProgressMeter(_T("%s %s"), msg, msg2);
         StatusBarProgressMeter.Init(*count);

			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();

			// Scorro il record set 
			while (gsc_isEOF(pRs) == GS_BAD)
         {
            StatusBarProgressMeter.Set(++i);

				// Leggo la riga
				if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il key attrib
				if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
					AfxThrowUserException( );
               
				// controlla se l'entit� � nella lista delle cancellate
				if (deleted_list.get_count()>0)
					el = deleted_list.search(pId->code, pId->sub_code, gs_id);
		
				if ((gs_id>last)||(el))
				{
					el = NULL;

					// ricavo il link set collegato alla scheda
					if (pSub->get_SelSet(gs_id, ss) == GS_BAD)
						AfxThrowUserException();

					if (ss.length() <= 0) in_graphic = FALSE;
				}

				if (in_graphic)
				{
					sub5.gs_id = gs_id;

					if ((p = ColValues.CdrAssoc(_T("QUOTA"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						dbl=0;
					else
						// Memorizzo quota
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub5.quota = gsc_tostring(dbl);
					
					if ((p = ColValues.CdrAssoc(_T("CHIUSA"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						lng=0;
					else
						// Memorizzo on_off
						if (gsc_rb2Lng(p, &lng) == GS_BAD) 
							AfxThrowUserException( );
					sub5.on_off = lng;

					str = gsc_tostring(gs_id);
					str += _T(",");
					str += sub5.quota;
					str += _T("\n");
					str += gsc_tostring(lastN + gs_id);
					str += _T(",");
					str += sub5.quota;

					if (fwprintf(filetemp1, _T("%s\n"), str) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

					if (fwprintf(filecons1, _T("%s\n%s\n"), _T("0"), _T("0")) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

               // lato fittizio
               str = _T("0,");
					str += gsc_tostring(gs_id);
					str += _T(",");
					str += gsc_tostring(lastN + gs_id);
					str += _T(",1,100,0,-2,90");

					if (fwprintf(filetemp2, _T("%s\n"), str) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

					if (fwprintf(filecons2, _T("%s\n"), ((sub5.on_off==0) ? _T("0") : _T("-1"))) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

					if (latiCls->getTempTableRef(TableRef)==GS_BAD)
						AfxThrowUserException();

					// Ricavo la connessione OLE - DB
					if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
						AfxThrowUserException();


					line1Id = 0;
					line2Id = 0;

					statement = _T("SELECT GS_ID FROM ");
					statement += TableRef;
					statement += _T(" WHERE NODO_INIZ=");
					statement += sub5.gs_id;
		        
					// Eseguo l' istruzione e ricavo il record set
					if (pConn->OpenRecSet(statement, pRSub2, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
						AfxThrowUserException();

					while (gsc_isEOF(pRSub2) == GS_BAD)
					{
						// Leggo la riga
						if (gsc_DBReadRow(pRSub2, ColValuesSub2) == GS_BAD) 
							AfxThrowUserException( );

						if ((p = ColValuesSub2.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException( );
						}
						// Memorizzo il gs_id
						if (gsc_rb2Lng(p, &gs_idL) == GS_BAD) 
							AfxThrowUserException( );
						if (line1Id == 0)
							line1Id = gs_idL;
						else
							line2Id = gs_idL;

						gsc_Skip(pRSub2);
					}

					statement = _T("SELECT GS_ID FROM ");
					statement += TableRef;
					statement += _T(" WHERE NODO_FINAL=");
					statement += sub5.gs_id;
		        
					// Eseguo l' istruzione e ricavo il record set
					if (pConn->OpenRecSet(statement, pRSub2, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
						AfxThrowUserException();

					while (gsc_isEOF(pRSub2) == GS_BAD)
					{
						// Leggo la riga
						if (gsc_DBReadRow(pRSub2, ColValuesSub2) == GS_BAD) 
							AfxThrowUserException( );

						if ((p = ColValuesSub2.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
						{ 
							set_GS_ERR_COD(eGSInvRBType);
							AfxThrowUserException( );
						}
						// Memorizzo il gs_id
						if (gsc_rb2Lng(p, &gs_idL) == GS_BAD) 
							AfxThrowUserException( );
						if (line2Id == 0)
							line2Id = gs_idL;
						else
							line1Id = gs_idL;

						gsc_Skip(pRSub2);
					}

					item = new C_4LONG(sub5.gs_id, lastN + sub5.gs_id, line1Id, line2Id);
					valve_list.add_tail(item);
				}
				else
				{
					(*count)--;
					in_graphic = TRUE;
				}
				gsc_Skip(pRs);
         }
         StatusBarProgressMeter.End(gsc_msg(1090)); // "Terminato."

			// chiusura record set
			gsc_DBCloseRs(pRs);
		}
	}

	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret=GS_BAD;
	}
	END_CATCH
	
	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::OutPrv													*/
/*+
	Questa funzione tratta i PRV della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero di PRV.     
   
	Parametri
	input:	C_CLASS		*pSub	classe per determinare la tabella sulla 
								quale effettuare la SELECT 
			CString		*fields	campi sui quali effettuare la SELECT
   
	output:	long		*count		numero dei PRV
            FILE		*filetemp1	file dei dati
            FILE		*filetemp2	altro file dei dati
            FILE		*filecons1	file dei consumi
            FILE		*filecons2	altro file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::OutPrv(C_CLASS *pSub, CString *fields, long *count, FILE *filetemp1, 
					       FILE *filetemp2, FILE *filecons1, FILE *filecons2)
{
	int           ret = GS_GOOD; 
	C_ID          *pId = pSub->ptr_id();
	C_INFO        *pInfo = pSub->ptr_info();
	CSub6	        sub6;
	long          i=0;	
	long          gs_id, last;
	C_2INT_LONG	  *el = NULL;
	C_SELSET	     ss;
	int           in_graphic = TRUE;
	C_RB_LIST     ColValues;
	presbuf       p = NULL;
	_RecordsetPtr pRs;
	double		  dbl;
	CString		  str;

	TRY
	{
		if (hy_outRecordSet(pSub, fields, NULL, pRs)==GS_BAD) 
			AfxThrowUserException( );

		// Conto le righe che soddisfano l' istruzione SQL
		if (gsc_RowsQty(pRs, count) == GS_BAD)
			AfxThrowUserException( );

		if ((*count) > 0)
		{
         CString msg, msg2;

         msg.LoadString(IDS_STRING274);  // "Scrittura"
         msg2.LoadString(IDS_STRING272); // "prv"
         C_STATUSBAR_PROGRESSMETER StatusBarProgressMeter(_T("%s %s"), msg, msg2);
         StatusBarProgressMeter.Init(*count);

			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();

			// Scorro il record set 
			while (gsc_isEOF(pRs) == GS_BAD)
         {
            StatusBarProgressMeter.Set(++i);

				// Leggo la riga
				if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il key attrib
				if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
					AfxThrowUserException( );
               
				// controlla se l'entit� � nella lista delle cancellate
				if (deleted_list.get_count()>0)
					el = deleted_list.search(pId->code, pId->sub_code, gs_id);
		
				if (gs_id > last || el)
				{
					el = NULL;

					// ricavo il link set collegato alla scheda
					if (pSub->get_SelSet(gs_id, ss) == GS_BAD)
						AfxThrowUserException();

					if (ss.length() <= 0) in_graphic = FALSE;
				}

				if (in_graphic)
				{
					sub6.gs_id = gs_id;

					if ((p = ColValues.CdrAssoc(_T("QUOTA"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						dbl=0;
					else
						// Memorizzo quota
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub6.quota = dbl;

					if ((p = ColValues.CdrAssoc(_T("PRESSIONE"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
					if (p->restype == RTNIL)
						dbl=0;
					else
						// Memorizzo pressione
						if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
							AfxThrowUserException( );
					sub6.pressione = dbl;

					str = gsc_tostring(gs_id);
					str += _T(",");
					str += gsc_tostring(sub6.quota);
					str += _T("\n");
					str += gsc_tostring((3*lastN) + gs_id);
					str += _T(",");
					str += gsc_tostring(sub6.quota + sub6.pressione);
         
            	if (fwprintf(filetemp1, _T("%s\n"), str) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

            	if (fwprintf(filecons1, _T("%s\n%s\n"), _T("0"), _T("0")) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

               // lato fittizio
               str = _T("0,");
					str += gsc_tostring(gs_id);
					str += _T(",");
					str += gsc_tostring((3*lastN) + gs_id);
					str += _T(",1,100,0,-1,90");
         
            	if (fwprintf(filetemp2, _T("%s\n"), str) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

            	if (fwprintf(filecons2, _T("%s\n"), _T("0")) < 0)
					{
						gs_errmsg(eGSWriteFile);
						AfxThrowUserException();
					}

				}
				else
				{
					(*count)--;
					in_graphic = TRUE;
				}

				gsc_Skip(pRs);
         }
         StatusBarProgressMeter.End(gsc_msg(1090)); // "Terminato."

			// chiusura record set
			gsc_DBCloseRs(pRs);
		}
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret=GS_BAD;
	}
	END_CATCH
	
	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::InSub1													*/
/*+
   Questa funzione tratta i NODI della SIMULAZIONE.
   Dopo aver eseguito un primo statement di UPDATE sugli opportuni campi 
   della tabella per azzerarli, compila lo statement parametrico pCmd.
   
   Parametri
   input:   C_CLASS		*pSub		classe per ricostruire la tabella 
									sulla quale effettuare le operazioni

   output:	_CommandPtr			&pCmd	statement compilato

   Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::InSub1(C_CLASS *pSub, C_PREPARED_CMD &pCmd)
{
	int				ret = GS_GOOD;	
	C_STRING		   TableRef, statement;
	C_INFO			*pInfo = pSub->ptr_info();
	C_DBCONNECTION	*pConn;	
	CString		   msg, msg2;

	TRY
	{
      msg.LoadString(IDS_STRING276);	// "Inizializzazione"
      msg2.LoadString(IDS_STRING267);	// "nodi"
      C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

		// ricavo tabella temporanea
		if (pSub->getTempTableRef(TableRef) == GS_BAD) 
			AfxThrowUserException();

		// Ricavo la connessione OLE - DB
		if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
			AfxThrowUserException();

		if (pSub->prepare_data(pCmd, TEMP) == GS_BAD) AfxThrowUserException();

		if (rete_tutta == true)
		{
			// Alcuni campi della tabella vengono azzerati, per essere pronti per 
			// l'aggiornamento con i dati prodotti dalla simulazione. In particolare
			// PRESSIONE=-5 (ovvero un valore "impossibile"), questo trucco serve
			// per recuperare, eventualmente in seguito, tutti quei "NODI" che, 
			// nel caso nella rete siano presenti VALVOLE o PRV chiusi, non sono 
			// coinvolti nella simulazione. 
			statement = _T("UPDATE ");
			statement += TableRef;
			statement += _T(" SET PRESSIONE = -5, EROGAZIONE = -5");

			if (pConn->ExeCmd(statement) == GS_BAD)
				AfxThrowUserException();

         StatusLineMsg.End(gsc_msg(1090)); // "Terminato."
		}
		else
      {
			// come sopra ma solo per i nodi tolopologicamente connessi alla rete 
			// considerata dal simulatore
			C_INT_LONG    *pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
			_RecordsetPtr pRs;
			int           IsRecordsetCloseable;
			C_RB_LIST     ColValues;
			presbuf       p;
			long          i = 0;

			while (pItem)
			{
				if (pItem->get_key() == pSub->ptr_id()->sub_code)
				{
					// cerco nel temp
					if (gsc_get_data(pCmd, pItem->get_id(), pRs, &IsRecordsetCloseable) == GS_BAD)
   						AfxThrowUserException();

					if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
						AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("PRESSIONE"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, -5) == GS_BAD) AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("EROGAZIONE"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, -5) == GS_BAD) AfxThrowUserException();

					if (UpdData(pSub, ColValues, pRs) == GS_BAD)
						AfxThrowUserException();

					// chiusura record set
					if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

					StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."
				}

				pItem = (C_INT_LONG *) pItem->get_next();
			}

         StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
		}
	}

	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::prepare_link_where_node_iniz  									*/
/*+
	Questa funzione prova a preparare un recordset per ricercare un lato 
   per nodo_iniziale con metodo seek.
	Parametri
	input:	C_CLASS		*pSub		classe per ricostruire la tabella 
									sulla quale effettuare le operazioni
   
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
_RecordsetPtr CHydroMap::prepare_link_where_node_iniz(C_DBCONNECTION *pConn,
                                                      C_STRING &TableName)
{
   _RecordsetPtr pRs;
   C_STRING      Table(TableName), InitQuotedId, FinalQuotedId;

   // istanzio un recordset
   if (FAILED(pRs.CreateInstance(__uuidof(Recordset)))) return NULL;

   // rimuove se esite il prefisso e il suffisso (es. per ACCESS = [ ])
   InitQuotedId = pConn->get_InitQuotedIdentifier();
   FinalQuotedId = pConn->get_FinalQuotedIdentifier();
   Table.removePrefixSuffix(InitQuotedId.get_name(), FinalQuotedId.get_name());

	try
   {  
      pRs->CursorLocation = adUseServer;
      // apro il recordset
      pRs->Open(Table.get_name(), pConn->get_Connection().GetInterfacePtr(), 
                adOpenDynamic, adLockOptimistic, adCmdTableDirect);
      
      if (!pRs->Supports(adIndex) || !pRs->Supports(adSeek))
      {  // non supportato
         pRs->Close(); 
         pRs.Release();
         return NULL;
      }
      // Setto l'indice di ricerca
      if (gsc_DBSetIndexRs(pRs, _T("NODO_INIZ")) == GS_BAD)
      {
         pRs->Close(); 
         pRs.Release();
         return NULL;
      }
   }

	catch (_com_error)
   {
      if (pRs.GetInterfacePtr()) pRs.Release();
      return NULL;
   }

   return pRs;
}

/****************************************************************************/
/*.doc CHydroMap::InSub2													*/
/*+
	Questa funzione tratta i LATI della SIMULAZIONE.
	Dopo aver eseguito un primo statement di UPDATE sugli opportuni campi 
	della tabella per azzerarli, compila lo statement parametrico pCmd.
   
	Parametri
	input:	C_CLASS		*pSub		classe per ricostruire la tabella 
									sulla quale effettuare le operazioni

	output:	C_PREPARED_CMD	&pCmd	statement compilato
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::InSub2(C_CLASS *pSub, C_PREPARED_CMD &pCmd)
{
	int				ret = GS_GOOD;	
	C_STRING		   TableRef, statement;	
	C_INFO			*pInfo = pSub->ptr_info();	
	C_DBCONNECTION	*pConn;
	CString		   msg, msg2;

	TRY
	{
      msg.LoadString(IDS_STRING276);	// "Inizializzazione"
      msg2.LoadString(IDS_STRING268);	// "lati"
      C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

		// ricavo tabella temporanea
		if (pSub->getTempTableRef(TableRef) == GS_BAD) 
			AfxThrowUserException();

		// Ricavo la connessione OLE - DB
		if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
			AfxThrowUserException();

		if (pSub->prepare_data(pCmd, TEMP) == GS_BAD) AfxThrowUserException();

      if (rete_tutta == true)
		{
			// Alcuni campi della tabella vengono azzerati, per essere pronti 
			// per l'aggiornamento con i dati prodotti dalla simulazione. 
			// In particolare VELOCITA=-5 (ovvero un valore "impossibile"), 
			// questo trucco serve per recuperare, eventualmente in seguito, 
			// tutti quei "LATI" che, nel caso nella rete siano presenti
			// VALVOLE o PRV chiusi, non sono coinvolti nella simulazione. 
			statement = _T("UPDATE ");
			statement += TableRef;
			statement += _T(" SET VELOCITA = -5, PERD_CARIC = 0, PORTATA_IN = 0, PORTATA_FI = 0");

			if (pConn->ExeCmd(statement) == GS_BAD)
				AfxThrowUserException();

         StatusLineMsg.End(gsc_msg(1090)); // "Terminato."
		}
		else
		{
			// come sopra ma solo per i lati tolopologicamente connessi alla rete 
			// considerata dal simulatore
			C_INT_LONG    *pItem = (C_INT_LONG *) Topo.ptr_NetLinks()->get_head();
			_RecordsetPtr pRs;
			int           IsRecordsetCloseable;
			C_RB_LIST     ColValues;
			presbuf       p;
			long          i = 0;

			while (pItem)
			{
				if (pItem->get_key() == pSub->ptr_id()->sub_code)
				{
					// cerco nel temp
					if (gsc_get_data(pCmd, pItem->get_id(), pRs, &IsRecordsetCloseable) == GS_BAD)
   						AfxThrowUserException();

					if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
						AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("VELOCITA"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, -5) == GS_BAD) AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("PERD_CARIC"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("PORTATA_IN"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("PORTATA_FI"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

					if (UpdData(pSub, ColValues, pRs) == GS_BAD)
						AfxThrowUserException();

					// chiusura record set
					if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

					StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."
				}

				pItem = (C_INT_LONG *) pItem->get_next();
			}

         StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
		}
	}

	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc int CHydroMap::InSub3												*/
/*+
	Questa funzione tratta i SERBATOI/REGOLATORI della SIMULAZIONE.
	Dopo aver eseguito un primo statement di UPDATE sugli opportuni campi
	della tabella per azzerarli, compila lo statement parametrico pCmd.
   
	Parametri
	input:	C_CLASS		*pSub	classe per ricostruire la tabella 
								sulla quale effettuare le operazioni

	output:	C_PREPARED_CMD			&pCmd	statement compilato

   Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::InSub3(C_CLASS *pSub, C_PREPARED_CMD &pCmd)
{
	int				ret=GS_GOOD;	
	C_STRING		   TableRef, statement;	
	C_INFO			*pInfo = pSub->ptr_info();
	C_DBCONNECTION	*pConn;	
	CString		   msg, msg2;

	TRY
	{
      msg.LoadString(IDS_STRING276);	// "Inizializzazione"
		if (tipo_fluido == H2O) // acqua
			msg2.LoadString(IDS_STRING269); // "serbatoi"
		else // gas
			msg2.LoadString(IDS_STRING273); // "regolatori"

      C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

		// ricavo tabella temporanea
		if (pSub->getTempTableRef(TableRef) == GS_BAD) 
			AfxThrowUserException();

		// Ricavo la connessione OLE - DB
		if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
			AfxThrowUserException();
		
		if (pSub->prepare_data(pCmd, TEMP) == GS_BAD) AfxThrowUserException();
   
		if (rete_tutta == true)
		{
			statement = _T("UPDATE ");
			statement += TableRef;
			statement += _T(" SET Q_USCENTE = 0");
			
			if ((tipo_fluido != H2O) && ((C_ATTRIB*) (pSub->ptr_attrib_list())->search_name("PRESSIONEE", FALSE)))
				statement += _T(", PRESSIONEE = 0");

			if (pConn->ExeCmd(statement) == GS_BAD)
				AfxThrowUserException();

         StatusLineMsg.End(gsc_msg(1090)); // "Terminato."
		}
		else
		{
			// come sopra ma solo per i serbatoi/regolatori tolopologicamente connessi alla rete 
			// considerata dal simulatore
			C_INT_LONG    *pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
			_RecordsetPtr pRs;
			int           IsRecordsetCloseable;
			C_RB_LIST     ColValues;
			presbuf       p;
			long          i = 0;

			while (pItem)
			{
				if (pItem->get_key() == pSub->ptr_id()->sub_code)
				{
					// cerco nel temp
					if (gsc_get_data(pCmd, pItem->get_id(), pRs, &IsRecordsetCloseable) == GS_BAD)
   						AfxThrowUserException();

					if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
						AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("Q_USCENTE"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

					if (tipo_fluido != H2O)
					{
						if ((p = ColValues.CdrAssoc(_T("PRESSIONEE"))) != NULL) 
							if (gsc_RbSubst(p, 0) == GS_BAD) 
								AfxThrowUserException();
					}

					if (UpdData(pSub, ColValues, pRs) == GS_BAD)
						AfxThrowUserException();

					// chiusura record set
					if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

					StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."
				}

				pItem = (C_INT_LONG *) pItem->get_next();
			}

         StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
		}
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::InSub4													*/
/*+
	Questa funzione tratta le POMPE della SIMULAZIONE.
	Dopo aver eseguito un primo statement di UPDATE sugli opportuni campi
	della tabella per azzerarli, compila lo statement parametrico pCmd.
   
	Parametri
	input:	C_CLASS		*pSub	classe per ricostruire la tabella 
								sulla quale effettuare le operazioni

	output:	_CommandPtr			&pCmd	statement compilato
      
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::InSub4(C_CLASS *pSub, C_PREPARED_CMD &pCmd)
{
	int				ret=GS_GOOD;	
	C_STRING		   TableRef, statement;
	C_INFO			*pInfo = pSub->ptr_info();
	C_DBCONNECTION	*pConn;
	CString		   msg, msg2;

	TRY
	{
      msg.LoadString(IDS_STRING276);	// "Inizializzazione"
      msg2.LoadString(IDS_STRING270);	// "pompe"
      C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

		// ricavo tabella temporanea
		if (pSub->getTempTableRef(TableRef) == GS_BAD) 
			AfxThrowUserException();

		// Ricavo la connessione OLE - DB
		if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
			AfxThrowUserException();

		if (pSub->prepare_data(pCmd, TEMP) == GS_BAD) AfxThrowUserException();
   
		if (rete_tutta == true)
		{
			// Alcuni campi della tabella vengono azzerati, per essere pronti per 
			// l'aggiornamento con i dati prodotti dalla simulazione. In particolare
			// PREVALENZA=-5 (ovvero un valore "impossibile"), questo trucco serve
			// per recuperare, eventualmente in seguito, tutte quelle "POMPE" che,
			// nel caso nella rete siano presenti VALVOLE o PRV chiusi, non sono 
			// coinvolte nella simulazione. 
			statement = _T("UPDATE ");
			statement += TableRef;
			statement += _T(" SET PREVALENZA = -5, PORTATA = 0");

			if (pConn->ExeCmd(statement) == GS_BAD)
				AfxThrowUserException();

         StatusLineMsg.End(gsc_msg(1090)); // "Terminato."
		}
		else
		{
			// come sopra ma solo per le pompe tolopologicamente connesse alla rete 
			// considerata dal simulatore
			C_INT_LONG    *pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
			_RecordsetPtr pRs;
			int           IsRecordsetCloseable;
			C_RB_LIST     ColValues;
			presbuf       p;
			long          i = 0;

			while (pItem)
			{
	            if (pItem->get_key() == pSub->ptr_id()->sub_code)
				{
					// cerco nel temp
					if (gsc_get_data(pCmd, pItem->get_id(), pRs, &IsRecordsetCloseable) == GS_BAD)
   						AfxThrowUserException();

					if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
						AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("PREVALENZA"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, -5) == GS_BAD) AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("PORTATA"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

					if (UpdData(pSub, ColValues, pRs) == GS_BAD)
						AfxThrowUserException();

					// chiusura record set
					if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

					StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."
				}

				pItem = (C_INT_LONG *) pItem->get_next();
			}

         StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
		}
	}

	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::InSub6													*/
/*+
	Questa funzione tratta i PRV della SIMULAZIONE.
	Dopo aver eseguito un primo statement di UPDATE sugli opportuni campi
	della tabella per azzerarli, compila gli statement parametrico pCmd1.
   
	Parametri
	input:	C_CLASS		*pSub	classe per ricostruire la tabella 
								sulla quale effettuare le operazioni

	output:	C_PREPARED_CMD			&pCmd	statement compilato
    
   Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::InSub6(C_CLASS *pSub, C_PREPARED_CMD &pCmd)
{
	int				ret=GS_GOOD;	
	C_STRING		   TableRef, statement;	
	C_INFO			*pInfo = pSub->ptr_info();	
	C_DBCONNECTION	*pConn;
	CString		   msg, msg2;

	TRY
	{
      msg.LoadString(IDS_STRING276);	// "Inizializzazione"
      msg2.LoadString(IDS_STRING272);	// "prv"
      C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

      // ricavo tabella temporanea
		if (pSub->getTempTableRef(TableRef) == GS_BAD) 
			AfxThrowUserException();

		// Ricavo la connessione OLE - DB
		if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
			AfxThrowUserException();

		if (pSub->prepare_data(pCmd, TEMP) == GS_BAD) AfxThrowUserException();
   
		if (rete_tutta == true)
		{
			// Alcuni campi della tabella vengono azzerati, per essere pronti per 
			// l'aggiornamento con i dati prodotti dalla simulazione. In particolare
			// PRES_MONTE=-5 e PRES_VALLE=-5 (ovvero un valore "impossibile"), 
			// questo trucco serve per recuperare, eventualmente in seguito, tutte 
			// quelle "POMPE" che, nel caso nella rete siano presenti VALVOLE o PRV
			// chiusi, non sono coinvolte nella simulazione. 
			statement = _T("UPDATE ");
			statement += TableRef;
			statement += _T(" SET PRES_MONTE = -5, PRES_VALLE = -5");

			if (pConn->ExeCmd(statement) == GS_BAD)
				AfxThrowUserException();

         StatusLineMsg.End(gsc_msg(1090)); // "Terminato."
      }
		else
		{
			// come sopra ma solo per i prv tolopologicamente connessi alla rete 
			// considerata dal simulatore
			C_INT_LONG    *pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
			_RecordsetPtr pRs;
			int           IsRecordsetCloseable;
			C_RB_LIST     ColValues;
			presbuf       p;
			long          i = 0;

			while (pItem)
			{
				if (pItem->get_key() == pSub->ptr_id()->sub_code)
				{
					// cerco nel temp
					if (gsc_get_data(pCmd, pItem->get_id(), pRs, &IsRecordsetCloseable) == GS_BAD)
   						AfxThrowUserException();

					if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
						AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("PRES_MONTE"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, -5) == GS_BAD) AfxThrowUserException();

					if ((p = ColValues.CdrAssoc(_T("PRES_VALLE"))) == NULL) AfxThrowUserException();
					if (gsc_RbSubst(p, -5) == GS_BAD) AfxThrowUserException();

					if (UpdData(pSub, ColValues, pRs) == GS_BAD)
						AfxThrowUserException();

					// chiusura record set
					if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

					StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."
				}

				pItem = (C_INT_LONG *) pItem->get_next();
			}

         StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
		}
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


int CHydroMap::FileControl()
{
	if (gsc_path_exist(nodi)==GS_BAD)
      { set_GS_ERR_COD(eGSFileNoExisting); return GS_BAD; }
	
	if (gsc_path_exist(lati)==GS_BAD)
      { set_GS_ERR_COD(eGSFileNoExisting); return GS_BAD; }

	return GS_GOOD;
}


int CHydroMap::FileClear()
{
   if (gsc_clear_file(nodi, nodiN, &nodiPos) == GS_BAD)
		return GS_BAD;

   if (gsc_clear_file(lati, latiN, &latiPos) == GS_BAD)
		return GS_BAD;

	return GS_GOOD;
}


int CHydroMap::FileRemove()
{
	if (gsc_path_exist(errore)==GS_GOOD)
		if (_wremove(errore))
			return GS_BAD;
	
	if (gsc_path_exist(nodi)==GS_GOOD)
		if (_wremove(nodi)!=0)
			return GS_BAD;
	
	if (gsc_path_exist(lati)==GS_GOOD)
		if (_wremove(lati)!=0)
			return GS_BAD;

	return GS_GOOD;
}

int CHydroMap::SimulaExec()
{
	presbuf	command=NULL, res=NULL;

	if (gsc_path_exist(simula)==GS_BAD)
	{
		hy_errmsg(HY_STRING223); // "Il modulo di simulazione idraulica <simula.arx> non esiste."
		return GS_BAD;
	}

	ads_arxunload(simula);
	ads_arxload(simula);

	command = acutBuildList(RTSTR, _T("simula"), RTSHORT, 2, RTSTR, temp_dir, 0);
	ads_invoke(command, &res);
	if (command) acutRelRb(command);
	if (res) acutRelRb(res);

	ads_arxunload(simula);

	return GS_GOOD;
}


/****************************************************************************/
/*.doc CHydroMap::OutFromGEOsim												*/
/*+
   Produce i file di output da utilizzare nella simulazione HYDROMAP.
   Acquisiti determinati parametri, esegue una precisa sequenza di 
   operazioni, operando sugli oggetti della simulazione nel seguente 
   ordine:
   -  SERBATOI
   -  POMPE
   -  PRV
   -  VALVOLE
   -  NODI
   -  LATI

   E' stato necessario utilizzare numerosi file temporanei, a causa 
   della complessit� dei file finali, che risultano infatti una 
   composizione di tali file intermedi.    

   Restituisce GS_GOOD in caso di successo, GS_CAN in caso di annullamento,
   altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::OutFromGEOsim()
{
	int ret = GS_GOOD;

	CString		data, cons, temp1, temp2, cons2, temp3, cons3;
	FILE		*datafile=NULL, *consfile=NULL, *tempfile1=NULL, *tempfile2=NULL, *consfile2=NULL, *tempfile3=NULL, *consfile3=NULL;

	long		nodi=0, lati=0, serbatoi=0, pompe=0;
	CString		campi;
	
	C_CLASS		*pSub;

	CString		str;

	int   		c;

	CString		msg;
	bool        Unicode = false;

	TRY
	{
		hy_list_deleted(cls, &deleted_list);
		hy_last(pCls, &lastcode_list);
	
		data = temp_dir;
		data += _T("data.inp");
		if ((datafile = gsc_fopen(data, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}

		cons = temp_dir;
		cons += _T("cons.inp");
		if ((consfile = gsc_fopen(cons, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}

		temp1 = temp_dir;
		temp1 += _T("temp1.inp");
		if ((tempfile1 = gsc_fopen(temp1, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}

		temp2 = temp_dir;
		temp2 += _T("temp2.inp");
		if ((tempfile2 = gsc_fopen(temp2, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}
		
		cons2 = temp_dir;
		cons2 += _T("cons2.inp");
		if ((consfile2 = gsc_fopen(cons2, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();	
		}

		temp3 = temp_dir;
		temp3 += _T("temp3.inp");
		if ((tempfile3 = gsc_fopen(temp3, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}
		
		cons3 = temp_dir;
		cons3 += _T("cons3.inp");
		if ((consfile3 = gsc_fopen(cons3, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();	
		}

		acutPrintf(_T("\n"));
		msg.LoadString(HY_STRING205);				// Elaborati
		acutPrintf(msg);

	// SERBATOI/REGOLATORI
		// Campi della tabella diversi per simulazione ACQUA e simulazione GAS, quindi comportamento differente
		if ((pSub = gsc_find_class(prj, cls, SUB3)) == NULL)
			AfxThrowUserException();

		lastN = pSub->GetNewEntCode();			// relativamente alle entit� nodali

		campi = pSub->ptr_info()->key_attrib.get_name();
		if (tipo_fluido == H2O)
			campi += _T(", QUOTA_PL");    // caso simulazione ACQUA
		else
			if (tipo_fluido == GB)
				campi += _T(", QUOTA, PRESSIONEB, MAX_EROGAZIONE"); // caso simulazione GAS BASSA
			else
				campi += _T(", QUOTA, PRESSIONEM, MAX_EROGAZIONE");	// caso simulazione GAS MEDIA

		regolatoriN = 0;
		if (OutSub3(pSub, &campi, &serbatoi, &regolatoriN, tempfile1, tempfile3, consfile, consfile3) == GS_BAD)
			AfxThrowUserException();

		if (fclose(tempfile3) == EOF)
		{
			gs_errmsg(eGSCloseFile); 
			AfxThrowUserException();
		}
		if ((tempfile3 = gsc_fopen(temp3, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		if (fclose(consfile3) == EOF)
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if ((consfile3 = gsc_fopen(cons3, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		while ((c=getc(tempfile3))!=EOF)
		{      
			if (putc(c,tempfile1)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		while ((c=getc(consfile3))!=EOF)
		{      
			if (putc(c,consfile)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		// Solo nel caso di simulazione ACQUA
		if (tipo_fluido == H2O)
		{
		   // POMPE
			if ((pSub = gsc_find_class(prj, cls, SUB4)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA_ASP, CURVA, Q_NOM, H_NOM, ON_OFF");
			if (OutSub4(pSub, &campi, &pompe, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)
				AfxThrowUserException();

		   // PRV
			if ((pSub = gsc_find_class(prj, cls, SUB6)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA, PRESSIONE");
			if (OutPrv(pSub, &campi, &prvN, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)       
				AfxThrowUserException();

		// VALVOLE (nel caso di simulazione ACQUA il codice identificativo della classe <VALVOLE> � 5)
			if ((pSub = gsc_find_class(prj, cls, SUB5)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA, CHIUSA");
			if (OutSub5(pSub, &campi, &valvoleN, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)         
				AfxThrowUserException();
		}
		else // nel caso di simulazione GAS
		{
		   // PRV
			if ((pSub = gsc_find_class(prj, cls, SUB5)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA, PRESSIONE");
			if (OutPrv(pSub, &campi, &prvN, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)       
				AfxThrowUserException();

		   // VALVOLE (nel caso di simulazione GAS il codice identificativo della classe <VALVOLE> � 4)
			if ((pSub = gsc_find_class(prj, cls, SUB4)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA, CHIUSA");
			if (OutSub5(pSub, &campi, &valvoleN, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)         
				AfxThrowUserException();
		}
	
	   // NODI
		// Stesso comportamento (e stessi campi della tabella) sia per simulazione ACQUA sia per simulazione GAS
		if ((pSub = gsc_find_class(prj, cls, SUB1)) == NULL)
			AfxThrowUserException();
		campi = pSub->ptr_info()->key_attrib.get_name();
		campi += _T(", QUOTA, CONSUMO");
		if (OutSub1(pSub, &campi, &nodi, tempfile1, consfile) == GS_BAD)
			AfxThrowUserException();

	   // LATI
		// Campi della tabella diversi per simulazione ACQUA e simulazione GAS, quindi comportamento differente
		if ((pSub = gsc_find_class(prj, cls, SUB2)) == NULL)
			AfxThrowUserException();
		campi = pSub->ptr_info()->key_attrib.get_name();
		if (tipo_fluido == H2O)
			campi += _T(", NODO_INIZ, NODO_FINAL, LUNGHEZZA, DIAMETRO, LEGGE_SC, PARAM_SC, PORTATA_D");
		else
			campi += _T(", NODO_INIZ, NODO_FINAL, LUNGHEZZA, DIAMETRO, PORTATA_D");	// caso simulazione GAS
		if (OutSub2(pSub, &campi, &lati, tempfile1, consfile) == GS_BAD)
			AfxThrowUserException();

		nodiN = nodi + serbatoi + regolatoriN + 2*pompe + 2*prvN + 2*valvoleN;
		latiN = lati + pompe + regolatoriN + prvN + valvoleN;
		serbatoi = serbatoi + ((tipo_fluido == H2O) ? pompe : 0); // se simulazione=H2O si devono aggiungere le pompe

		str = gsc_tostring((int)tipo_fluido);
		str += _T(",");
		str += gsc_tostring(tipo_simulazione);
		str += _T("\n");
		str += gsc_tostring(nodiN);
		str += _T(",");
		str += gsc_tostring(latiN);
		str += _T(",");
		str += gsc_tostring(serbatoi);
		str += _T(",");
		str += gsc_tostring(pompe);
		str += _T(",");
		str += gsc_tostring(prvN);
		str += _T(",");
		str += gsc_tostring(tipo_controllo);
		str += _T(",");
		str += gsc_tostring(lastN);
		str += _T("\n");
		str += gsc_tostring(controllo_portata);
		str += _T(",");
		str += gsc_tostring(controllo_carico);
		str += _T(",");
		str += gsc_tostring(min_velocita_prv);
		str += _T(",");
		str += gsc_tostring(min_portata_prv);
		str += _T(",");
		str += gsc_tostring(coeff_molt_portate);
		str += _T(",");
		str += ((pompa_in_curva == 1) ? _T("1") : _T("0"));
		str += _T(",");
		str += gsc_tostring(precisione_pompe/1000);
		str += _T(",");
		str += gsc_tostring(convergenza_pompe);
		str += _T(",");
		str += gsc_tostring(velocita_approssimazione);
		str += _T(",");
		str += (tipo_fluido==H2O ? _T("0") : gsc_tostring(densita_relativa));

		if (tipo_simulazione == 2)
		{
			str += _T("\n");
			str += gsc_tostring(numero_iterazioni);
		}   
   
		if (fwprintf(datafile, _T("%s\n"), str) < 0)
		{
			gs_errmsg(eGSWriteFile); 
			AfxThrowUserException();
		}
 
		if (fclose(tempfile1) == EOF) 
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if ((tempfile1 = gsc_fopen(temp1, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile); 
			AfxThrowUserException();
		}

		if (fclose(tempfile2) == EOF)
		{
			gs_errmsg(eGSCloseFile); 
			AfxThrowUserException();
		}
		if ((tempfile2 = gsc_fopen(temp2, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		if (fclose(consfile2) == EOF)
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if ((consfile2 = gsc_fopen(cons2, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		while ((c=getc(tempfile1))!=EOF)
		{      
			if (putc(c,datafile)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		while ((c=getc(tempfile2))!=EOF)
		{      
			if (putc(c,datafile)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		while ((c=getc(consfile2))!=EOF)
		{      
			if (putc(c,consfile)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}


		if (datafile)
		{
			if (fclose(datafile) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				datafile=NULL;
		}

		if (consfile)
		{
			if (fclose(consfile) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				consfile=NULL;
		}

		if (tempfile1)
		{
			if (fclose(tempfile1) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				tempfile1=NULL;
		}

		if (tempfile2)
		{
			if (fclose(tempfile2) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				tempfile2=NULL;
		}

		if (consfile2)
		{
			if (fclose(consfile2) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				consfile2=NULL;
		}

		if (tempfile3)
		{
			if (fclose(tempfile3) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				tempfile3=NULL;
		}

		if (consfile3)
		{
			if (fclose(consfile3) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				consfile3=NULL;
		}

		if (gsc_path_exist(temp1)==GS_GOOD)
			_wremove(temp1);

		if (gsc_path_exist(temp2)==GS_GOOD)
			_wremove(temp2);

		if (gsc_path_exist(cons2)==GS_GOOD)
			_wremove(cons2);

		if (gsc_path_exist(temp3)==GS_GOOD)
			_wremove(temp3);

		if (gsc_path_exist(cons3)==GS_GOOD)
			_wremove(cons3);
	}
	CATCH( CUserException, e )
	{
		if (datafile)
		{
			fclose(datafile);
			datafile=NULL;
		}

		if (consfile)
		{
			fclose(consfile);
			consfile=NULL;
		}

		if (tempfile1)
		{
			fclose(tempfile1);
			tempfile1=NULL;
		}

		if (tempfile2)
		{
			fclose(tempfile2);
			tempfile2=NULL;
		}

		if (consfile2)
		{
			fclose(consfile2);
			consfile2=NULL;
		}
		
		if (tempfile3)
		{
			fclose(tempfile3);
			tempfile3=NULL;
		}

		if (consfile3)
		{
			fclose(consfile3);
			consfile3=NULL;
		}

		if (gsc_path_exist(temp1)==GS_GOOD)
			_wremove(temp1);

		if (gsc_path_exist(temp2)==GS_GOOD)
			_wremove(temp2);

		if (gsc_path_exist(cons2)==GS_GOOD)
			_wremove(cons2);

		if (gsc_path_exist(temp3)==GS_GOOD)
			_wremove(temp3);

		if (gsc_path_exist(cons3)==GS_GOOD)
			_wremove(cons3);
		
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}

int CHydroMap::PartialOutFromGEOsim()
{
	int ret = GS_GOOD;

	CString	data, cons, temp1, temp2, cons2, temp3, cons3;
	FILE		*datafile=NULL, *consfile=NULL, *tempfile1=NULL, *tempfile2=NULL, *consfile2=NULL, *tempfile3=NULL, *consfile3=NULL;

	long		nodi=0, lati=0, serbatoi=0, pompe=0;
	CString	campi;
	
	C_CLASS	*pSub;

	CString	str;

	int      c;

	CString	msg;

	long     iNode;
	bool     Unicode = false;  

	TRY
	{
		hy_last(pCls, &lastcode_list);
	
		data = temp_dir;
		data += _T("data.inp");
		if ((datafile = gsc_fopen(data, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}

		cons = temp_dir;
		cons += _T("cons.inp");
		if ((consfile = gsc_fopen(cons, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}

		temp1 = temp_dir;
		temp1 += _T("temp1.inp");
		if ((tempfile1 = gsc_fopen(temp1, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}

		temp2 = temp_dir;
		temp2 += _T("temp2.inp");
		if ((tempfile2 = gsc_fopen(temp2, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}
		
		cons2 = temp_dir;
		cons2 += _T("cons2.inp");
		if ((consfile2 = gsc_fopen(cons2, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();	
		}


		temp3 = temp_dir;
		temp3 += _T("temp3.inp");
		if ((tempfile3 = gsc_fopen(temp3, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();		
		}
		
		cons3 = temp_dir;
		cons3 += _T("cons3.inp");
		if ((consfile3 = gsc_fopen(cons3, _T("w"), MORETESTS, &Unicode)) == NULL)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();	
		}

		acutPrintf(_T("\n"));
		msg.LoadString(HY_STRING205);				// Elaborati
		acutPrintf(msg);

		Topo.set_type(TYPE_POLYLINE); // tipologia di tipo rete
		Topo.set_cls(pCls);

		if (Topo.LoadInMemory() == GS_BAD) return GS_BAD;

		// se si tratta di un link cerco gli elementi connessi al
		// nodo iniziale e poi a quello finale
		if (startSub == 2)
		{
			long iLink = gsc_searchTopoNetLink(Topo.LinksVett, Topo.nLinksVett, startId);
			iNode = Topo.LinksVett[iLink].InitialNode;
			Topo.GetNetPropagation(iNode);
			iNode = Topo.LinksVett[iLink].FinalNode;
		}
		else
			iNode = gsc_searchTopoNetNode(Topo.NodesVett, Topo.nNodesVett, startId);

		Topo.GetNetPropagation(iNode);
      
		// Libero un p� di memoria
		gsc_freeTopoNetNode(&(Topo.NodesVett), &(Topo.nNodesVett));
		gsc_freeTopoNetLink(&(Topo.LinksVett), &(Topo.nLinksVett));

		// SERBATOI/REGOLATORI
		// Campi della tabella diversi per simulazione ACQUA e simulazione GAS, quindi comportamento differente
		if ((pSub = gsc_find_class(prj, cls, SUB3)) == NULL)
			AfxThrowUserException();

		lastN = pSub->GetNewEntCode();			// relativamente alle entit� nodali

		campi = pSub->ptr_info()->key_attrib.get_name();
		if (tipo_fluido == H2O)
			campi += _T(", QUOTA_PL");    // caso simulazione ACQUA
		else
			if (tipo_fluido == GB)
				campi += _T(", QUOTA, PRESSIONEB"); // caso simulazione GAS BASSA
			else
				campi += _T(", QUOTA, PRESSIONEM");	// caso simulazione GAS MEDIA

		regolatoriN = 0;
		if (PartialOutSub3(pSub, &campi, *(Topo.ptr_NetNodes()), &serbatoi, &regolatoriN, tempfile1, tempfile3, consfile, consfile3) == GS_BAD)
			AfxThrowUserException();

		if (fclose(tempfile3) == EOF)
		{
			gs_errmsg(eGSCloseFile); 
			AfxThrowUserException();
		}
		if ((tempfile3 = gsc_fopen(temp3, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		if (fclose(consfile3) == EOF)
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if ((consfile3 = gsc_fopen(cons3, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		while ((c=getc(tempfile3))!=EOF)
		{      
			if (putc(c,tempfile1)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		while ((c=getc(consfile3))!=EOF)
		{      
			if (putc(c,consfile)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		// Solo nel caso di simulazione ACQUA
		if (tipo_fluido == H2O)
		{
		   // POMPE
			if ((pSub = gsc_find_class(prj, cls, SUB4)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA_ASP, CURVA, Q_NOM, H_NOM, ON_OFF");
			if (PartialOutSub4(pSub, &campi, *(Topo.ptr_NetNodes()), &pompe, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)
				AfxThrowUserException();

		   // PRV
			if ((pSub = gsc_find_class(prj, cls, SUB6)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA, PRESSIONE");
			if (PartialOutPrv(pSub, &campi, *(Topo.ptr_NetNodes()), &prvN, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)
				AfxThrowUserException();

   		// VALVOLE (nel caso di simulazione ACQUA il codice identificativo della classe <VALVOLE> � 5)
			if ((pSub = gsc_find_class(prj, cls, SUB5)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA, CHIUSA");
			if (PartialOutSub5(pSub, &campi, *(Topo.ptr_NetNodes()), &valvoleN, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)
				AfxThrowUserException();
		}
		else
		// nel caso di simulazione GAS
		{
			// PRV
			if ((pSub = gsc_find_class(prj, cls, SUB5)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA, PRESSIONE");
			if (PartialOutPrv(pSub, &campi, *(Topo.ptr_NetNodes()), &prvN, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)
				AfxThrowUserException();

		   // VALVOLE (nel caso di simulazione GAS il codice identificativo della classe <VALVOLE> � 4)
			if ((pSub = gsc_find_class(prj, cls, SUB4)) == NULL)
				AfxThrowUserException();
			campi = pSub->ptr_info()->key_attrib.get_name();
			campi += _T(", QUOTA, CHIUSA");
			if (PartialOutSub5(pSub, &campi, *(Topo.ptr_NetNodes()), &valvoleN, tempfile1, tempfile2, consfile, consfile2) == GS_BAD)
				AfxThrowUserException();
		}
	
	   // NODI
		// Stesso comportamento (e stessi campi della tabella) sia per simulazione ACQUA sia per simulazione GAS
		if ((pSub = gsc_find_class(prj, cls, SUB1)) == NULL)
			AfxThrowUserException();
		campi = pSub->ptr_info()->key_attrib.get_name();
		campi += _T(", QUOTA, CONSUMO");
		if (PartialOutSub1(pSub, &campi, *(Topo.ptr_NetNodes()), &nodi, tempfile1, consfile) == GS_BAD)
			AfxThrowUserException();

      // LATI
		// Campi della tabella diversi per simulazione ACQUA e simulazione GAS, quindi comportamento differente
		if ((pSub = gsc_find_class(prj, cls, SUB2)) == NULL)
			AfxThrowUserException();
		campi = pSub->ptr_info()->key_attrib.get_name();
		if (tipo_fluido == H2O)
			campi += _T(", NODO_INIZ, NODO_FINAL, LUNGHEZZA, DIAMETRO, LEGGE_SC, PARAM_SC, PORTATA_D");
		else
			campi += _T(", NODO_INIZ, NODO_FINAL, LUNGHEZZA, DIAMETRO, PORTATA_D");	// caso simulazione GAS
		if (PartialOutSub2(pSub, &campi, *(Topo.ptr_NetLinks()), &lati, tempfile1, consfile) == GS_BAD)
			AfxThrowUserException();

		nodiN = nodi + serbatoi + regolatoriN + 2*pompe + 2*prvN + 2*valvoleN;
		latiN = lati + pompe + regolatoriN + prvN + valvoleN;
		serbatoi = serbatoi + ((tipo_fluido == H2O) ? pompe : 0); // se simulazione=H2O si devono aggiungere le pompe

		str = gsc_tostring((int) tipo_fluido);
		str += _T(",");
		str += gsc_tostring(tipo_simulazione);
		str += _T("\n");
		str += gsc_tostring(nodiN);
		str += _T(",");
		str += gsc_tostring(latiN);
		str += _T(",");
		str += gsc_tostring(serbatoi);
		str += _T(",");
		str += gsc_tostring(pompe);
		str += _T(",");
		str += gsc_tostring(prvN);
		str += _T(",");
		str += gsc_tostring(tipo_controllo);
		str += _T(",");
		str += gsc_tostring(lastN);
		str += _T("\n");
		str += gsc_tostring(controllo_portata);
		str += _T(",");
		str += gsc_tostring(controllo_carico);
		str += _T(",");
		str += gsc_tostring(min_velocita_prv);
		str += _T(",");
		str += gsc_tostring(min_portata_prv);
		str += _T(",");
		str += gsc_tostring(coeff_molt_portate);
		str += _T(",");
		str += ((pompa_in_curva==1) ? _T("1") : _T("0"));
		str += _T(",");
		str += gsc_tostring(precisione_pompe/1000);
		str += _T(",");
		str += gsc_tostring(convergenza_pompe);
		str += _T(",");
		str += gsc_tostring(velocita_approssimazione);
		str += _T(",");
		str += (tipo_fluido == H2O ? _T("0") : gsc_tostring(densita_relativa));

		if (tipo_simulazione == 2)
		{
			str += _T("\n");
			str += gsc_tostring(numero_iterazioni);
		}   
   
		if (fwprintf(datafile, _T("%s\n"), str) < 0)
		{
			gs_errmsg(eGSWriteFile); 
			AfxThrowUserException();
		}
 
		if (fclose(tempfile1) == EOF) 
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if ((tempfile1 = gsc_fopen(temp1, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile); 
			AfxThrowUserException();
		}

		if (fclose(tempfile2) == EOF)
		{
			gs_errmsg(eGSCloseFile); 
			AfxThrowUserException();
		}
		if ((tempfile2 = gsc_fopen(temp2, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		if (fclose(consfile2) == EOF)
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if ((consfile2 = gsc_fopen(cons2, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		while ((c=getc(tempfile1))!=EOF)
		{      
			if (putc(c,datafile)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		while ((c=getc(tempfile2))!=EOF)
		{      
			if (putc(c,datafile)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		while ((c=getc(consfile2))!=EOF)
		{      
			if (putc(c,consfile)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}


		if (datafile)
		{
			if (fclose(datafile) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				datafile=NULL;
		}

		if (consfile)
		{
			if (fclose(consfile) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				consfile=NULL;
		}

		if (tempfile1)
		{
			if (fclose(tempfile1) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				tempfile1=NULL;
		}

		if (tempfile2)
		{
			if (fclose(tempfile2) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				tempfile2=NULL;
		}

		if (consfile2)
		{
			if (fclose(consfile2) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				consfile2=NULL;
		}

		if (tempfile3)
		{
			if (fclose(tempfile3) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				tempfile3=NULL;
		}

		if (consfile3)
		{
			if (fclose(consfile3) == EOF) 
			{
				gs_errmsg(eGSCloseFile);
				AfxThrowUserException();
			}
			else
				consfile3=NULL;
		}

		if (gsc_path_exist(temp1)==GS_GOOD)
			_wremove(temp1);

		if (gsc_path_exist(temp2)==GS_GOOD)
			_wremove(temp2);

		if (gsc_path_exist(temp3)==GS_GOOD)
			_wremove(temp3);

		if (gsc_path_exist(cons2)==GS_GOOD)
			_wremove(cons2);
		
		if (gsc_path_exist(cons3)==GS_GOOD)
			_wremove(cons3);
	}
	CATCH( CUserException, e )
	{
		if (datafile)
		{
			fclose(datafile);
			datafile=NULL;
		}

		if (consfile)
		{
			fclose(consfile);
			consfile=NULL;
		}

		if (tempfile1)
		{
			fclose(tempfile1);
			tempfile1=NULL;
		}

		if (tempfile2)
		{
			fclose(tempfile2);
			tempfile2=NULL;
		}

		if (consfile2)
		{
			fclose(consfile2);
			consfile2=NULL;
		}
		
		if (tempfile3)
		{
			fclose(tempfile3);
			tempfile3=NULL;
		}

		if (consfile3)
		{
			fclose(consfile3);
			consfile3=NULL;
		}

		if (gsc_path_exist(temp1)==GS_GOOD)
			_wremove(temp1);

		if (gsc_path_exist(temp2)==GS_GOOD)
			_wremove(temp2);

		if (gsc_path_exist(cons2)==GS_GOOD)
			_wremove(cons2);

		if (gsc_path_exist(temp3)==GS_GOOD)
			_wremove(temp3);

		if (gsc_path_exist(cons3)==GS_GOOD)
			_wremove(cons3);
		
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}

/****************************************************************************/
/*.doc CHydroMap::PartialOutSub3													*/
/*+
	Questa funzione tratta i SERBATOI/REGOLATORI della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero dei SERBATOI/REGOLATORI.     
   
	Parametri
	input:	C_CLASS *pSub		classe per determinare la tabella 
								sulla quale effettuare la SELECT 
			CString	*fields		campi sui quali effettuare la SELECT
   
	output:	long	*count			numero dei SERBATOI/REGOLATORI
			long	*countR			numero dei REGOLATORI fittizi
			FILE	*filetemp1		file dei dati
			FILE	*filecons1		file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::PartialOutSub3(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList,
                              long *count, long *countR, FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2)
{
	int			   ret = GS_GOOD; 
	C_ID		      *pId = pSub->ptr_id();
	C_INFO		   *pInfo = pSub->ptr_info();
	CSub3		      sub3;
	long		      i = 0;	
	CString		   msg, msg2, idMsg, strMsg;
	long		      gs_id, last;
	C_2INT_LONG	   *el = NULL;
	C_SELSET	      ss;
	C_RB_LIST	   ColValues;
	presbuf		   p = NULL;
	double		   dbl;
	C_INT_LONG	   *pNode = (C_INT_LONG *) nodesList.get_head();
	C_PREPARED_CMD_LIST CmdList;
	C_CLASS        *latiCls;
	C_STRING		   statement, TableRef;
	C_DBCONNECTION	*pConn;
	_RecordsetPtr	pRSub2;
	long		      countL;

   msg.LoadString(IDS_STRING274);	// "Scrittura"
	if (tipo_fluido == H2O)
		msg2.LoadString(IDS_STRING269); // "serbatoi"
	else
		msg2.LoadString(IDS_STRING273); // "regolatori"
   C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);

	TRY
	{
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

		// Preparo i comandi di lettura dei dati della classe dal temp/old
		if (pSub->prepare_data(CmdList) == GS_BAD) return GS_BAD;

		*count = 0;
     
		if (pNode)
			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();
  
		if ((latiCls = gsc_find_class(prj, cls, SUB2)) == NULL)		// PAOLETTA
			AfxThrowUserException();

		idMsg = "";

		while (pNode)
		{
			if (pNode->get_key() != SUB3)
			{
				pNode = (C_INT_LONG *) nodesList.get_next();
				continue;
			}

			*count = *count + 1;

			if (pSub->query_data(pNode->get_id(), ColValues, &CmdList) == GS_BAD)
				AfxThrowUserException();

   			if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo il key attrib
			if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
				AfxThrowUserException( );              

			StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."
			
			sub3.gs_id = gs_id;

			if (tipo_fluido == H2O)
			{
				if ((p = ColValues.CdrAssoc(_T("QUOTA_PL"))) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}

			   if (p->restype == RTNIL)
				   dbl = 0;
			   else
				   // Memorizzo quota
				   if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					   AfxThrowUserException( );
			   sub3.quota = gsc_tostring(dbl);
   			
			   if (fwprintf(filetemp1, _T("%s,%s\n"), 
                                   gsc_tostring(sub3.gs_id), 
									        sub3.quota) < 0)
			   {
				   gs_errmsg(eGSWriteFile);
				   AfxThrowUserException();
			   }
			}
			else // gas
			{
				if ((p = ColValues.CdrAssoc(_T("QUOTA"))) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}

				if (p->restype == RTNIL)
					dbl = 0;
				else
					// Memorizzo quota
					if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
						AfxThrowUserException( );
				sub3.quota = gsc_tostring(dbl);
   			
				// regolatori di taglio
   				if (!(p = ColValues.CdrAssoc(_T("MAX_EROGAZIONE"))) || p->restype == RTNIL)
					dbl = -1;
				else	
					// Memorizzo max_erogazione
					if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
						AfxThrowUserException( );
				sub3.max_erogazione = gsc_tostring(dbl);

            // roby per regolatori di taglio
#ifdef REGOLATORI_DI_TAGLIO
				if (fwprintf(filetemp1, _T("%s,%s,%s\n"), 
									gsc_tostring(sub3.gs_id), 
									sub3.quota,
									sub3.max_erogazione) < 0)
				{
					gs_errmsg(eGSWriteFile);
					AfxThrowUserException();
				}
	
				if (sub3.max_erogazione != "-1")		// PAOLETTA
				{
					*countR = *countR + 1;

					if (latiCls->getTempTableRef(TableRef)==GS_BAD)
						AfxThrowUserException();

					statement = _T("SELECT GS_ID FROM ");
					statement += TableRef;
					statement += _T(" WHERE NODO_INIZ=");
					statement += sub3.gs_id;
					statement += _T(" OR NODO_FINAL=");
					statement += sub3.gs_id;
				        
					// Ricavo la connessione OLE - DB
					if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
						AfxThrowUserException();

					// Eseguo l' istruzione e ricavo il record set
					if (pConn->OpenRecSet(statement, pRSub2, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
						AfxThrowUserException();

					// Conto le righe che soddisfano l' istruzione SQL
					if (gsc_RowsQty(pRSub2, &countL) == GS_BAD)
						AfxThrowUserException( );

					if (countL > 1)
					{
						if (idMsg.GetLength() > 0)
							idMsg += " - ";
						idMsg += gsc_tostring(gs_id);
					}
					else
					{
						if (fwprintf(filetemp2, _T("%s,%s\n"), 
										gsc_tostring(sub3.gs_id + 4*lastN), 
										sub3.quota) < 0)
						{
							gs_errmsg(eGSWriteFile);
							AfxThrowUserException();
						}
						if (fwprintf(filecons2, _T("%s\n"),	_T("0")) < 0)
						{
							gs_errmsg(eGSWriteFile);
							AfxThrowUserException();
						}
					}
				}
#else
				if (fwprintf(filetemp1, _T("%s,%s\n"), 
									gsc_tostring(sub3.gs_id), 
									sub3.quota) < 0)
				{
					gs_errmsg(eGSWriteFile);
					AfxThrowUserException();
				}
#endif
			}

			if (tipo_fluido == H2O) 
				sub3.pressione = sub3.quota;
			else
			{
				if (tipo_fluido == GB) 
				{              
					if ((p = ColValues.CdrAssoc(_T("PRESSIONEB"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}
				}
				else
					if ((p = ColValues.CdrAssoc(_T("PRESSIONEM"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvRBType);
						AfxThrowUserException( );
					}

				if (p->restype == RTNIL)
					dbl=0;
				else
					if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
						AfxThrowUserException( );
				sub3.pressione = gsc_tostring(dbl);
			}
			
			if (fwprintf(filecons1, _T("%s\n"),	sub3.pressione) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		
			pNode = (C_INT_LONG *) nodesList.get_next();
		}

		if (idMsg.GetLength() > 0)
		{
			strMsg = "\nConsiderato che i regolatori ";
			strMsg += idMsg;
			strMsg += " presentano pi� linee di uscita, la simulazione viene interrotta in quanto i risultati non sarebbero attendibili. Modificare affinch� da ciascun regolatore esca una singola linea.";
			acutPrintf(strMsg);
			AfxThrowUserException( );
		}

      StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}

/****************************************************************************/
/*.doc CHydroMap::PartialOutSub4                                            */
/*+
	Questa funzione tratta le POMPE della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero di POMPE.     
   
	Parametri
	input:	C_CLASS	*pSub	classe per determinare la tabella sulla quale
							effettuare la SELECT 
			CString	*fields	campi sui quali effettuare la SELECT
   
	output:	long	*count		numero dei nodi
			FILE	*filetemp1	file dei dati
			FILE	*filetemp2	altro file dei dati
			FILE	*filecons1	file dei consumi
			FILE	*filecons2	altro file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::PartialOutSub4(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList, 
                              long *count, FILE *filetemp1,
					               FILE *filetemp2, FILE *filecons1, FILE *filecons2)
{
	int		      ret = GS_GOOD, in_graphic = TRUE; 
	C_ID		      *pId = pSub->ptr_id();
	C_INFO	      *pInfo = pSub->ptr_info();
	CSub4		      sub4;
	long		      i = 0, gs_id, last, numpompe = 0, lng;
	CString	      msg, msg2;
	C_2INT_LONG 	*el = NULL;
	C_SELSET	      ss;
	C_RB_LIST	   ColValues;
	presbuf		   p = NULL;
	double		   dbl, h, q;
	CString		   curva, temp, cons, stpf, pompe, str, stp;
	FILE		      *filetemp = NULL, *filecons = NULL, *filestp = NULL, *filepompe = NULL;
	int		      j;
	TCHAR          *entry=NULL, *buf=NULL;
   C_INT_LONG	   *pNode = (C_INT_LONG *) nodesList.get_head();
   bool           Unicode = false;
   C_PREPARED_CMD_LIST CmdList;

   msg.LoadString(IDS_STRING274);  // "Scrittura"
   msg2.LoadString(IDS_STRING270); // "pompe"
   C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);

	TRY
	{
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

      // Preparo i comandi di lettura dei dati della classe dal temp/old
      if (pSub->prepare_data(CmdList) == GS_BAD) return GS_BAD;

     *count = 0;

		if (pNode)
		{
			temp = temp_dir;
			temp += _T("temp3.inp");
			if ((filetemp = gsc_fopen(temp, _T("w"), MORETESTS, &Unicode)) == NULL)
			{ 
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();
			}

			cons = temp_dir;
			cons += _T("cons3.inp");
			if ((filecons = gsc_fopen(cons, _T("w"), MORETESTS, &Unicode)) == NULL)
			{ 
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();
			}

			stpf = temp_dir;
			stpf += _T("stp.inp");
			if ((filestp = gsc_fopen(stpf, _T("w"), MORETESTS, &Unicode)) == NULL)
			{ 
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();
			}

         // Prima provo nel direttorio del progetto
			pompe = get_GS_CURRENT_WRK_SESSION()->get_pPrj()->get_dir();
			pompe += _T("\\pompe.ini");

			if ((filepompe = gsc_open_profile(pompe, READONLY, MORETESTS, &Unicode)) == NULL)
			{   
            // Provo nel direttorio EXT sotto il direttorio server di GEOsim
				pompe = get_GEODIR();
				pompe += _T("\\");
				pompe += GEOEXT;
				pompe += _T("\\pompe.ini");

				if ((filepompe = gsc_open_profile(pompe, READONLY, MORETESTS, &Unicode)) == NULL)
				{ 
					hy_errmsg(HY_STRING222); // "Il file <pompe.ini>, contenente le curve caratteristiche delle pompe, non esiste."
					AfxThrowUserException();
				}
			}

			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();
		}
		
		// Scorro il record set 
		while (pNode)
      {
         if (pNode->get_key() != SUB4)
         {
            pNode = (C_INT_LONG *) nodesList.get_next();
            continue;
         }

         *count = *count + 1;

         if (pSub->query_data(pNode->get_id(), ColValues, &CmdList) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
			{
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo il key attrib
			if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
				AfxThrowUserException( );
               
         StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."

			sub4.gs_id = gs_id;

			if ((p = ColValues.CdrAssoc(_T("QUOTA_ASP"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			
			if (p->restype == 0)
				dbl=0;
			else
				// Memorizzo quota_asp
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );					
			sub4.quota_asp = dbl;
			
			if ((p = ColValues.CdrAssoc(_T("CURVA"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				curva = _T("standard");
			else
				curva = gsc_rb2str(p);
			sub4.curva = curva;

			if ((p = ColValues.CdrAssoc(_T("Q_NOM"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvAttribName);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				dbl=1;
			else
				// Memorizzo q_nom
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub4.q_nom = dbl;

			if ((p = ColValues.CdrAssoc(_T("H_NOM"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvAttribName);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				dbl=1;
			else
				// Memorizzo q_nom
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub4.h_nom = dbl;

			if ((p = ColValues.CdrAssoc(_T("ON_OFF"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvAttribName);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				lng=0;
			else
				// Memorizzo q_nom
				if (gsc_rb2Lng(p, &lng) == GS_BAD) 
					AfxThrowUserException( );
			sub4.on_off = lng;
			
			numpompe++;
			str = gsc_tostring((2*lastN) + numpompe);

			if (sub4.quota_asp == 0)
				sub4.quota_asp = (double)0.1;
			
			str += _T(",");
			str += gsc_tostring(sub4.quota_asp);

			if (fwprintf(filetemp1, _T("%s\n"), str) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

			str = gsc_tostring(gs_id);
			str += _T(",");
			str += gsc_tostring(sub4.quota_asp);

			if (fwprintf(filetemp, _T("%s\n"), str) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

			str = gsc_tostring(sub4.quota_asp);

			if (fwprintf(filecons1, _T("%s\n"), str) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
	
			if (fwprintf(filecons, _T("%s\n"), _T("0")) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
	
         // lato fittizio
			str = _T("0,");
         str += gsc_tostring((2*lastN) + numpompe);
			str += _T(",");
			str += gsc_tostring(gs_id);
			str += _T(",1,100,0,");
			str += gsc_tostring(20 + numpompe);
			str += _T(",90");

			if (fwprintf(filetemp2, _T("%s\n"), str) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
			
			if (fwprintf(filecons2, _T("%s\n"), ((sub4.on_off==0) ? _T("0") : _T("-1"))) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
			
			for (j=0; j<10; j++)
			{
				entry = gsc_tostring(j);
				if (gsc_get_profile(filepompe, sub4.curva, entry, &buf,0,0) == GS_BAD) 
				{ 
					hy_errmsg(HY_STRING224); // "La curva caratteristica di una pompa della rete non � definita nel file <pompe.ini>."
					AfxThrowUserException();
				}
         
				gsc_strsep(buf, _T(' '));
				swscanf(buf, _T("%lf%lf"), &h, &q);
				free(entry);
				free(buf);                 
				entry=NULL;
				buf=NULL;            

				h=(double)(h*sub4.h_nom);
				q=(double)(q*sub4.q_nom);

				stp = gsc_tostring(q);
				stp += _T(",");
				stp += gsc_tostring(h);

				if (fwprintf(filestp, _T("%s\n"), stp) < 0)
				{
					gs_errmsg(eGSWriteFile);
					AfxThrowUserException();
				}
         }
		
         pNode = (C_INT_LONG *) nodesList.get_next();
      }         

		if (gsc_close_profile(filepompe) == GS_BAD) 
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}

		if (fclose(filetemp) == EOF) 
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if ((filetemp = gsc_fopen(temp, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		if (fclose(filecons) == EOF)
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if ((filecons = gsc_fopen(cons, _T("r"), MORETESTS, &Unicode)) == NULL)
		{
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();
		}

		int c;
		while ((c=getc(filetemp))!=EOF)
		{      
			if (putc(c,filetemp1)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		while ((c=getc(filecons))!=EOF)
		{      
			if (putc(c,filecons1)==EOF)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}
		}

		if (fclose(filetemp) == EOF)
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if (gsc_path_exist(temp)==GS_GOOD)
			_wremove(temp);
   
		if (fclose(filecons) == EOF) 
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}
		if (gsc_path_exist(cons)==GS_GOOD)
			_wremove(cons);

		if (fclose(filestp) == EOF)
		{
			gs_errmsg(eGSCloseFile);
			AfxThrowUserException();
		}

      StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
	}
	CATCH( CUserException, e )
	{
		if (filetemp)
		{
			fclose(filetemp);
			filetemp = NULL;
		}		

		if (filecons)
		{
			fclose(filecons);
			filecons = NULL;
		}		
		
		if (filestp)
		{
			fclose(filestp);
			filestp = NULL;
		}		

		if (filepompe)
		{
			fclose(filepompe);
			filepompe = NULL;
		}		

		ret = GS_BAD;
	}
	END_CATCH
	
	return ret;
}

/****************************************************************************/
/*.doc CHydroMap::PartialOutSub5													*/
/*+
	Questa funzione tratta le VALVOLE della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero delle VALVOLE.     
   
	Parametri
	input:	C_CLASS	*pSub   classe per determinare la tabella sulla 
							quale effettuare la SELECT 
            CString	*fields	campi sui quali effettuare la SELECT
	
	output:	long	*count		numero delle VALVOLE
			FILE	*filetemp1	file dei dati
			FILE	*filetemp2	altro file dei dati
			FILE	*filecons1	file dei consumi
			FILE	*filecons2	altro file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::PartialOutSub5(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList,
                              long *count, FILE *filetemp1, 
					               FILE *filetemp2, FILE *filecons1, FILE *filecons2)
{
	int			   ret = GS_GOOD; 
	C_ID		      *pId = pSub->ptr_id();
	C_INFO		   *pInfo = pSub->ptr_info();
	CSub5		      sub5;
	long		      i = 0, gs_id, last;	
	CString		   msg, msg2;
	C_2INT_LONG	   *el = NULL;
	C_SELSET	    ss;
	int			   in_graphic = TRUE;
	C_RB_LIST	   ColValues, ColValuesSub2;
	presbuf		   p = NULL;
	double		   dbl;
	long		      lng, gs_idL, line1Id, line2Id;
	CString		   str;
	C_INT_LONG	   *pNode = (C_INT_LONG *) nodesList.get_head();
	C_PREPARED_CMD_LIST CmdList;
  	C_CLASS		   *latiCls;
	C_STRING		   statement, TableRef;
   C_DBCONNECTION	*pConn;
	_RecordsetPtr	pRSub2;
	C_4LONG			*item;

   msg.LoadString(IDS_STRING274);  // "Scrittura"
	msg2.LoadString(IDS_STRING271); // "valvole"
   C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);

	TRY
	{
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

		if ((latiCls = gsc_find_class(prj, cls, SUB2)) == NULL)		// PAOLETTA
			AfxThrowUserException();

		// Preparo i comandi di lettura dei dati della classe dal temp/old
		if (pSub->prepare_data(CmdList) == GS_BAD) return GS_BAD;

		*count = 0;

		if (pNode)
			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();

		while (pNode)
		{
			if (pNode->get_key() != pId->sub_code)
			{
				pNode = (C_INT_LONG *) nodesList.get_next();
				continue;
			}

			*count = *count + 1;

			if (pSub->query_data(pNode->get_id(), ColValues, &CmdList) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo il key attrib
			if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
				AfxThrowUserException( );
               
         StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."

			sub5.gs_id = gs_id;

			if ((p = ColValues.CdrAssoc(_T("QUOTA"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				dbl=0;
			else
				// Memorizzo quota
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub5.quota = gsc_tostring(dbl);
			
			if ((p = ColValues.CdrAssoc(_T("CHIUSA"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				lng=0;
			else
				// Memorizzo on_off
				if (gsc_rb2Lng(p, &lng) == GS_BAD) 
					AfxThrowUserException( );
			sub5.on_off = lng;

			str = gsc_tostring(gs_id);
			str += _T(",");
			str += sub5.quota;
			str += _T("\n");
			str += gsc_tostring(lastN + gs_id);
			str += _T(",");
			str += sub5.quota;

			if (fwprintf(filetemp1, _T("%s\n"), str) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

			if (fwprintf(filecons1, _T("%s\n%s\n"), _T("0"), _T("0")) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

			// lato fittizio
			str = _T("0,");
			str += gsc_tostring(gs_id);
			str += _T(",");
			str += gsc_tostring(lastN + gs_id);
			str += _T(",1,100,0,-2,90");

			if (fwprintf(filetemp2, _T("%s\n"), str) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

			if (fwprintf(filecons2, _T("%s\n"), ((sub5.on_off==0 )? _T("0") : _T("-1"))) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

			if (latiCls->getTempTableRef(TableRef)==GS_BAD)
				AfxThrowUserException();

			// Ricavo la connessione OLE - DB
			if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
				AfxThrowUserException();

			line1Id = 0;
			line2Id = 0;

			statement = _T("SELECT GS_ID FROM ");
			statement += TableRef;
			statement += _T(" WHERE NODO_INIZ=");
			statement += sub5.gs_id;
        
			// Eseguo l' istruzione e ricavo il record set
			if (pConn->OpenRecSet(statement, pRSub2, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
				AfxThrowUserException();

			while (gsc_isEOF(pRSub2) == GS_BAD)
			{
				// Leggo la riga
				if (gsc_DBReadRow(pRSub2, ColValuesSub2) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValuesSub2.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il gs_id
				if (gsc_rb2Lng(p, &gs_idL) == GS_BAD) 
					AfxThrowUserException( );
				if (line1Id == 0)
					line1Id = gs_idL;
				else
					line2Id = gs_idL;

				gsc_Skip(pRSub2);
			}

			statement = _T("SELECT GS_ID FROM ");
			statement += TableRef;
			statement += _T(" WHERE NODO_FINAL=");
			statement += sub5.gs_id;
        
			// Eseguo l' istruzione e ricavo il record set
			if (pConn->OpenRecSet(statement, pRSub2, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
				AfxThrowUserException();

			while (gsc_isEOF(pRSub2) == GS_BAD)
			{
				// Leggo la riga
				if (gsc_DBReadRow(pRSub2, ColValuesSub2) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValuesSub2.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il gs_id
				if (gsc_rb2Lng(p, &gs_idL) == GS_BAD) 
					AfxThrowUserException( );
				if (line2Id == 0)
					line2Id = gs_idL;
				else
					line1Id = gs_idL;

				gsc_Skip(pRSub2);
			}
		
			item = new C_4LONG(sub5.gs_id, lastN + sub5.gs_id, line1Id, line2Id);
			valve_list.add_tail(item);

			pNode = (C_INT_LONG *) nodesList.get_next();
		}

      StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH
	
	return ret;
}

/****************************************************************************/
/*.doc CHydroMap::OutPrv                                                    */
/*+
	Questa funzione tratta i PRV della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e 
	tabella, scandisce i dati ottenuti (utilizzando il cursore restituito),
	li modifica secondo precise regole, e li scrive in file di output; 
	conta inoltre il numero di PRV.     
   
	Parametri
	input:	C_CLASS		*pSub	classe per determinare la tabella sulla 
								quale effettuare la SELECT 
    			CString		*fields	campi sui quali effettuare la SELECT
   
	output:	long		*count		numero dei PRV
            FILE		*filetemp1	file dei dati
            FILE		*filetemp2	altro file dei dati
            FILE		*filecons1	file dei consumi
            FILE		*filecons2	altro file dei consumi
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::PartialOutPrv(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList,
                             long *count, FILE *filetemp1, 
					              FILE *filetemp2, FILE *filecons1, FILE *filecons2)
{
	int		           ret = GS_GOOD; 
	C_ID		           *pId = pSub->ptr_id();
	C_INFO	           *pInfo = pSub->ptr_info();
	CSub6		           sub6;
	long		           i = 0, gs_id, last;	
	CString	           msg, msg2;
	C_2INT_LONG	        *el = NULL;
	C_SELSET	           ss;
	C_RB_LIST	        ColValues;
	presbuf		        p = NULL;
	double		        dbl;
	CString		        str;
   C_INT_LONG	        *pNode = (C_INT_LONG *) nodesList.get_head();
   C_PREPARED_CMD_LIST CmdList;

   msg.LoadString(IDS_STRING274);  // "Scrittura"
   msg2.LoadString(IDS_STRING272); // "prv"
   C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);

	TRY
	{
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

      // Preparo i comandi di lettura dei dati della classe dal temp/old
      if (pSub->prepare_data(CmdList) == GS_BAD) return GS_BAD;

      *count = 0;

		if (pNode)
			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();

		while (pNode)
      {
         if (pNode->get_key() != pSub->ptr_id()->sub_code) // solo PRV
         {
            pNode = (C_INT_LONG *) nodesList.get_next();
            continue;
         }

         *count = *count + 1;

         if (pSub->query_data(pNode->get_id(), ColValues, &CmdList) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			// Memorizzo il key attrib
			if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
				AfxThrowUserException( );
               
         StatusLineMsg.Set(++i); // "%ld entit� GEOsim elaborate."

			sub6.gs_id = gs_id;

			if ((p = ColValues.CdrAssoc(_T("QUOTA"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				dbl=0;
			else
				// Memorizzo quota
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub6.quota = dbl;

			if ((p = ColValues.CdrAssoc(_T("PRESSIONE"))) == NULL) 
			{ 
				set_GS_ERR_COD(eGSInvRBType);
				AfxThrowUserException( );
			}
			if (p->restype == RTNIL)
				dbl=0;
			else
				// Memorizzo pressione
				if (gsc_rb2Dbl(p, &dbl) == GS_BAD) 
					AfxThrowUserException( );
			sub6.pressione = dbl;

			str = gsc_tostring(gs_id);
			str += _T(",");
			str += gsc_tostring(sub6.quota);
			str += _T("\n");
			str += gsc_tostring((3*lastN) + gs_id);
			str += _T(",");
			str += gsc_tostring(sub6.quota + sub6.pressione);
   
         if (fwprintf(filetemp1, _T("%s\n"), str) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

         if (fwprintf(filecons1, _T("%s\n%s\n"), _T("0"), _T("0")) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

         // lato fittizio
			str = _T("0,");
			str += gsc_tostring(gs_id);
			str += _T(",");
			str += gsc_tostring((3*lastN) + gs_id);
			str += _T(",1,100,0,-1,90");
   
         if (fwprintf(filetemp2, _T("%s\n"), str) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

         if (fwprintf(filecons2, _T("%s\n"), _T("0")) < 0)
			{
				gs_errmsg(eGSWriteFile);
				AfxThrowUserException();
			}

         pNode = (C_INT_LONG *) nodesList.get_next();
		}

      StatusLineMsg.End(gsc_msg(310), i); // "%ld entit� GEOsim elaborate."
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH
	
	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::UpdateSub1												             */
/*+
	Esegue lo statement SELECT "pCmd", compilato precedentemente
	dopo aver sostituito il parametro: 
	-  "gs_id"  identificatore dell'oggetto da ricercare
	Ottenuto il cursore sui dati aggiorna la riga con:
	-  "value1"	nuovo valore da assegnare al carico
	-  "value2" nuovo valore da assegnare alla pressione
	-  "value3" nuovo valore da assegnare all'erogazione

	Parametri
	input: C_PREPARED_CMD &pCmd
			C_CLASS		*pSub
			double		value1
			double		value2
			double		value3
			long		gs_id
			int			*found
  
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::UpdateSub1(C_PREPARED_CMD &pCmd, C_CLASS *pSub, double value1, 
						        double value2, double value3, long gs_id, int *found)
{
	int ret = GS_GOOD;

   int            IsRecordsetCloseable = GS_GOOD;
	_RecordsetPtr	pRs;
	
	C_RB_LIST	ColValues;
	presbuf		p;

	TRY 
	{
		// cerco nel temp
      if (gsc_get_data(pCmd, gs_id, pRs, &IsRecordsetCloseable) == GS_BAD)
		{ 
			if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
			(*found)=FALSE; 
			return ret; 
		}

		if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc(_T("CARICO"))) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, value1) == GS_BAD)
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc(_T("PRESSIONE"))) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, value2) == GS_BAD)
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc(_T("EROGAZIONE"))) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, value3) == GS_BAD)
			AfxThrowUserException();

		if (UpdData(pSub, ColValues, pRs) == GS_BAD)
			AfxThrowUserException();

		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::UpdateSub2												*/
/*+
	Esegue lo statement SELECT "pCmd", compilato precedentemente
	dopo aver sostituito i parametri: 
	-  "nodo_in" identificatore del nodo iniziale da ricercare
	-  "nodo_fi" identificatore del nodo finale da ricercare
	Ottenuto il cursore aggiorna la riga con:
	-  "value1"	nuovo valore da assegnare alla portata iniziale
	-  "value2"	nuovo valore da assegnare alla portata finale
	-  "value3"	nuovo valore da assegnare alla velocit�
	-  "value4"	nuovo valore da assegnare alla perdita del carico
	-  "value5"	nuovo valore da assegnare al diametro se il tipo di 
				simulazione � "calcolo diametri"

	Parametri
	input:	_CommandPtr &pCmd
			C_CLASS		*pSub
			double		value1
			double		value2
			double		value3
			double		value4
			double		value5
			long		nodo_in
			long		nodo_fi
  
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
/*
int CHydroMap::UpdateSub2(_CommandPtr &pCmd, C_CLASS *pSub, double value1, double value2, 
						  double value3, double value4, double value5, long nodo_in, long nodo_fi)
{
	int ret = GS_GOOD;

	_RecordsetPtr	pRs;
	
	C_RB_LIST	ColValues;
	presbuf		p;

	long		concorde;
	TCHAR		tempin[2], tempout[2];

	TRY 
	{
		// cerco nel temp
		if (gsc_SetDBParam(pCmd, 0, nodo_in) == GS_BAD)
			AfxThrowUserException();
		if (gsc_SetDBParam(pCmd, 1, nodo_fi) == GS_BAD)
			AfxThrowUserException();

		if (gsc_ExeCmd(pCmd, pRs, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
			AfxThrowUserException();

		if (gsc_isEOF(pRs) == GS_GOOD)
		{ 
			// chiusura record set
			gsc_DBCloseRs(pRs);

			// cerco nel temp
			if (gsc_SetDBParam(pCmd, 0, nodo_fi) == GS_BAD)
				AfxThrowUserException();
			if (gsc_SetDBParam(pCmd, 1, nodo_in) == GS_BAD)
				AfxThrowUserException();
			
			if (gsc_ExeCmd(pCmd, pRs, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
				AfxThrowUserException();

			if (gsc_isEOF(pRs) == GS_GOOD)
			{ 
				// chiusura record set
				gsc_DBCloseRs(pRs);
				return ret; 
			}

			concorde = 0;
		} 
		else
			concorde = 1;

		if (concorde == 1)
		{
			if (value1 < 0)
				wcscpy(tempin, "<");
			else
			{
				if (value1 == 0)
					wcscpy(tempin, " ");
				else
					wcscpy(tempin, ">");
			}
			if (value2 < 0)
				wcscpy(tempout, "<");
			else
			{
				if (value2 == 0)
					wcscpy(tempout, " ");
				else
					wcscpy(tempout, ">");
			}
			// wcscpy(tempin, ((value1 >= 0) ? ">" : "<"));
			// wcscpy(tempout, ((value2 >= 0) ? ">" : "<"));
			value1=fabs(value1);
			value2=fabs(value2);
		}
		else
		{
			if (value2 < 0)
				wcscpy(tempin, ">");
			else
			{
				if (value2 == 0)
					wcscpy(tempin, " ");
				else
					wcscpy(tempin, "<");
			}
			if (value1 < 0)
				wcscpy(tempout, ">");
			else
			{
				if (value1 == 0)
					wcscpy(tempout, " ");
				else
					wcscpy(tempout, "<");
			}
			// wcscpy(tempin, ((value2 >= 0) ? "<" : ">"));
			// wcscpy(tempout, ((value1 >= 0) ? "<" : ">"));
  
			double temp;
			temp=fabs(value1);
			value1=fabs(value2);
			value2=temp;
      
			long   swap;
			swap=nodo_in;
			nodo_in=nodo_fi;
			nodo_fi=swap;
		}

		if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc("PORTATA_IN")) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, value1) == GS_BAD)
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc("PORTATA_FI")) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, value2) == GS_BAD)
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc("VELOCITA")) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, value3) == GS_BAD)
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc("PERD_CARIC")) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, value4) == GS_BAD)
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc("VERSO_Q_IN")) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, tempin) == GS_BAD)
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc("VERSO_Q_FI")) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, tempout) == GS_BAD)
			AfxThrowUserException();

		if (value5)
		{
			if ((p = ColValues.CdrAssoc("DIAMETRO")) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, value5) == GS_BAD)
				AfxThrowUserException();
		}

		if (UpdData(pSub, ColValues, pRs) == GS_BAD)
			AfxThrowUserException();

		// chiusura record set
		gsc_DBCloseRs(pRs);
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}
*/
int CHydroMap::UpdateSub2(C_PREPARED_CMD &pCmd, C_CLASS *pSub, double value1, double value2, 
      						  double value3, double value4, double value5, 
                          long gs_id, long nodo_in, long nodo_fi)
{
	int ret = GS_GOOD;

	_RecordsetPtr pRs;
	int           IsRecordsetCloseable = GS_BAD; 	
	C_RB_LIST	  ColValues;
	presbuf		  p;
	long		     NodeCode;
	TCHAR		     tempin[2], tempout[2];
	
	double		perdita_carico=0;

	TRY 
	{
		if (gs_id >= 4*lastN)
		{
			gs_id = gs_id - 4*lastN;

			// cerco nel temp
			if (gsc_get_data(pCmd, gs_id, pRs, &IsRecordsetCloseable) == GS_BAD)
				AfxThrowUserException();
			if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(_T("PERD_CARIC"))) == NULL)
				AfxThrowUserException();
			if (gsc_rb2Dbl(p, &perdita_carico) == GS_BAD) 
				AfxThrowUserException( );
			if (gsc_RbSubst(p, perdita_carico + value4) == GS_BAD)
				AfxThrowUserException();
		}
		else
		{
			// cerco nel temp
			if (gsc_get_data(pCmd, gs_id, pRs, &IsRecordsetCloseable) == GS_BAD)
				AfxThrowUserException();

			if (gsc_DBReadRow(pRs, ColValues) == GS_BAD ||
				(p = ColValues.CdrAssoc(_T("NODO_INIZ"))) == NULL ||
				gsc_rb2Lng(p, &NodeCode) == GS_BAD)
				AfxThrowUserException();

			if (NodeCode == nodo_in)
			{
				if (value1 < 0)
					wcscpy(tempin, _T("<"));
				else
				{
					if (value1 == 0)
						wcscpy(tempin, _T(" "));
					else
						wcscpy(tempin, _T(">"));
				}
				if (value2 < 0)
					wcscpy(tempout, _T("<"));
				else
				{
					if (value2 == 0)
						wcscpy(tempout, _T(" "));
					else
						wcscpy(tempout, _T(">"));
				}

				value1 = fabs(value1);
				value2 = fabs(value2);
			}
			else
			{
				if (value2 < 0)
					wcscpy(tempin, _T(">"));
				else
				{
					if (value2 == 0)
						wcscpy(tempin, _T(" "));
					else
						wcscpy(tempin, _T("<"));
				}
				if (value1 < 0)
					wcscpy(tempout, _T(">"));
				else
				{
					if (value1 == 0)
						wcscpy(tempout, _T(" "));
					else
						wcscpy(tempout, _T("<"));
				}
  
				double temp;
				temp=fabs(value1);
				value1=fabs(value2);
				value2=temp;
      
				long   swap;
				swap=nodo_in;
				nodo_in=nodo_fi;
				nodo_fi=swap;
			}

			if ((p = ColValues.CdrAssoc(_T("PORTATA_IN"))) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, value1) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(_T("PORTATA_FI"))) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, value2) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(_T("VELOCITA"))) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, value3) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(_T("PERD_CARIC"))) == NULL)
				AfxThrowUserException();
			if (gsc_rb2Dbl(p, &perdita_carico) == GS_BAD) 
				AfxThrowUserException( );
			if (gsc_RbSubst(p, perdita_carico + value4) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(_T("VERSO_Q_IN"))) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, tempin) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(_T("VERSO_Q_FI"))) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, tempout) == GS_BAD)
				AfxThrowUserException();

			if (value5)
			{	
				if ((p = ColValues.CdrAssoc(_T("DIAMETRO"))) == NULL)
					AfxThrowUserException();
				if (gsc_RbSubst(p, value5) == GS_BAD)
					AfxThrowUserException();
			}
		}

		if (UpdData(pSub, ColValues, pRs) == GS_BAD)
			AfxThrowUserException();

		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::UpdateSub3												*/
/*+
	Esegue lo statement SELECT "pCmd", compilato precedentemente
	dopo aver sostituito il parametro: 
	- "gs_id"	identificatore dell'oggetto da ricercare
	Ottenuto il cursore sui dati aggiorna la riga con:
	- "value1"	nuovo valore da assegnare alla quantit� uscente

	Parametri
	input:	C_PREPARED_CMD &pCmd
			C_CLASS		*pSub
			double		value1
			double		value2
			long		gs_id
			int			*found
		  
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::UpdateSub3(C_PREPARED_CMD &pCmd, C_CLASS *pSub, 
							double value1, double value2, long gs_id, int *found)
{
	int ret = GS_GOOD;

   int            IsRecordsetCloseable = GS_GOOD;
	_RecordsetPtr	pRs;
	
	C_RB_LIST	ColValues;
	presbuf		p;

	double		q_uscente;
	TCHAR 		*number=NULL;
	double		max, min;
	
	TRY 
	{
		if (gs_id >= 4*lastN)
			gs_id = gs_id - 4*lastN;

		if (gsc_get_data(pCmd, gs_id, pRs, &IsRecordsetCloseable) == GS_BAD)
		{ 
			if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
			(*found)=FALSE; 
			return ret; 
		}

		if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
			AfxThrowUserException();
		
		if ((p = ColValues.CdrAssoc(_T("Q_USCENTE"))) == NULL)
			AfxThrowUserException();
		if (gsc_rb2Dbl(p, &q_uscente) == GS_BAD) 
			AfxThrowUserException();

		number = gsc_max_number(pSub, _T("Q_USCENTE"), &max);
		free(number);
		number = gsc_min_number(pSub, _T("Q_USCENTE"), &min);
		free(number);

		if (q_uscente > max)			// q_uscente � il valore impostato dalla funzione gsc_in_sub3(...),
			q_uscente = 0;				// in particolare � arrotondato al valore superiore.

		if (q_uscente + value1 == max)
		{
			value1 = ceil(q_uscente + value1);			// parte intera
			q_uscente = 0;
		}

		if (q_uscente + value1 == min)
		{
			value1 = floor(q_uscente + value1);			// parte intera
			q_uscente = 0;
		}
		
		if (gsc_RbSubst(p, value1 + q_uscente) == GS_BAD)
			AfxThrowUserException();

		if (value2 != 0)
		{
			if ((p = ColValues.CdrAssoc(_T("PRESSIONEE"))) != NULL)
				if (gsc_RbSubst(p, value2) == GS_BAD)
					AfxThrowUserException();
		}
	
		if (UpdData(pSub, ColValues, pRs) == GS_BAD)
			AfxThrowUserException();

		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::UpdateSub4												*/
/*+
	Esegue lo statement SELECT "pCmd", compilato precedentemente
	dopo aver sostituito i parametri: 
	- "gs_id"	identificatore dell'oggetto da ricercare
	Ottenuto il cursore sui dati aggiorna la riga con:
	- "value1"	nuovo valore da assegnare alla prevalenza
	- "value2"	nuovo valore da assegnare alla portata

	Parametri
	input:	C_PREPARED_CMD &pCmd
			   C_CLASS		*pSub	classe
			   double 		value1	valore da assegnare
            double 		value2	valore da assegnare
            long  		gs_id	codice oggetto da sostituire al parametro 
								dello statement
			   int			*found
		
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::UpdateSub4(C_PREPARED_CMD &pCmd, C_CLASS *pSub, double value1, 
						        double value2, long gs_id, int *found)
{
	int ret = GS_GOOD;

   int            IsRecordsetCloseable = GS_GOOD;
	_RecordsetPtr	pRs;
	
	C_RB_LIST	ColValues;
	presbuf		p;

	TRY 
	{
		// cerco nel temp
      if (gsc_get_data(pCmd, gs_id, pRs, &IsRecordsetCloseable) == GS_BAD)
		{ 
			if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
			(*found)=FALSE; 
			return ret; 
		}

		if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
			AfxThrowUserException();
		
		if ((p = ColValues.CdrAssoc(_T("PREVALENZA"))) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, value1) == GS_BAD)
			AfxThrowUserException();

		if ((p = ColValues.CdrAssoc(_T("PORTATA"))) == NULL)
			AfxThrowUserException();
		if (gsc_RbSubst(p, value2) == GS_BAD)
			AfxThrowUserException();

		if (UpdData(pSub, ColValues, pRs) == GS_BAD)
			AfxThrowUserException();

		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::UpdateSub6												*/
/*+
	Esegue lo statement SELECT "pCmd" compilato precedentemente
	dopo aver sostituito il parametro: 
	- "gs_id"	identificatore dell'oggetto da ricercare
	Ottenuto il cursore sui dati aggiorna la riga con:
	- "value1"	nuovo valore da assegnare alla pressione
	- "value2"	nuovo valore da assegnare al carico

	Utilizza anche gli statement "pCmdV" e "pCmdM".

	Parametri
	input:	C_PREPARED_CMD		&pCmd
			   C_CLASS			*pSub
			   double			value1
			   double			value2
			   long			   gs_id

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::UpdateSub6(C_PREPARED_CMD &pCmd,
      						  C_CLASS *pSub, double value1, double value2, long gs_id)
{
	int ret = GS_GOOD;

   int            IsRecordsetCloseable = GS_GOOD;
	_RecordsetPtr	pRs;
	
	C_RB_LIST	ColValues;
	presbuf		p;

	long		tgs_id;

	TRY 
	{
		// cerco nel temp
		if (gs_id>=(3*lastN))
			tgs_id = gs_id - (3*lastN);
		else
			tgs_id = gs_id;
			
      if (gsc_get_data(pCmd, tgs_id, pRs, &IsRecordsetCloseable) == GS_BAD)
		{ 
			if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
			return ret; 
		}

		if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
			AfxThrowUserException();

		if (gs_id>=(3*lastN))
		{
			if ((p = ColValues.CdrAssoc(_T("PRES_VALLE"))) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, value1) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(_T("CARI_VALLE"))) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, value2) == GS_BAD)
				AfxThrowUserException();
		}
		else
		{
			if ((p = ColValues.CdrAssoc(_T("PRES_MONTE"))) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, value1) == GS_BAD)
				AfxThrowUserException();

			if ((p = ColValues.CdrAssoc(_T("CARI_MONTE"))) == NULL)
				AfxThrowUserException();
			if (gsc_RbSubst(p, value2) == GS_BAD)
				AfxThrowUserException();
		}

		if (UpdData(pSub, ColValues, pRs) == GS_BAD)
			AfxThrowUserException();

		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::UpdateNullSub1                                            */
/*+
	Aggiorna a video le entit� determinate eseguendo una SELECT sul campo 
	PRESSIONE = -5 (valore "impossibile", utilizzato proprio per recuperare 
	i record non aggiornati con i risultati della simulazione) ed azzera
	successivamente tale campo.

	Parametri
	input:	C_CLASS  *pSub;   classe
            C_SELSET &SelSet;

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::UpdateNullSub1(C_CLASS *pSub, C_SELSET &SelSet)
{
	int            ret = GS_GOOD;
	C_INFO		   *pInfo = pSub->ptr_info();
	C_STRING	      TableRef, statement;
	C_DBCONNECTION	*pConn;
	_RecordsetPtr	pRs;
	C_RB_LIST		ColValues;
	presbuf			p;

	TRY 
	{
		if (rete_tutta == true)
      {
		   if (pSub->getTempTableRef(TableRef)==GS_BAD)
			   AfxThrowUserException();

		   statement = _T("SELECT * FROM ");
		   statement += TableRef;
		   statement += _T(" WHERE ");
		   statement += pInfo->key_attrib;
		   statement += _T(">0 AND PRESSIONE=-5");

		   // Ricavo la connessione OLE - DB
		   if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
			   AfxThrowUserException();

		   // Eseguo l' istruzione e ricavo il record set
		   if (pConn->OpenRecSet(statement, pRs, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
			   AfxThrowUserException();

		   while (gsc_isEOF(pRs) == GS_BAD)
         {
			   // Leggo la riga
			   if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
				   AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("CARICO"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0.0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PRESSIONE"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0.0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("EROGAZIONE"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0.0) == GS_BAD) AfxThrowUserException();

			   if (UpdData(pSub, ColValues, pRs) == GS_BAD) AfxThrowUserException();

			   gsc_Skip(pRs);
		   }

		   // chiusura record set
		   gsc_DBCloseRs(pRs);
      }
      else
      {
         // come sopra ma solo per i nodi tolopologicamente connessi alla rete 
         // considerata dal simulatore (a questo punto ptr_NetNodes contiene solo
         // i nodi NON ancora aggiornati)
         C_PREPARED_CMD pCmd;
         C_INT_LONG     *pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
         int            IsRecordsetCloseable;
         C_SELSET       ss;

         if (pSub->prepare_data(pCmd, TEMP) == GS_BAD) AfxThrowUserException();

         while (pItem)
         {
            if (pItem->get_key() == 1)
            {
   			   if (pSub->get_SelSet(pItem->get_id(), ss, ALL) == GS_GOOD)
      			   SelSet.add_selset(ss);

		         // cerco nel temp
               if (gsc_get_data(pCmd, pItem->get_id(), pRs, &IsRecordsetCloseable) == GS_BAD)
   			      AfxThrowUserException();

		         if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
			         AfxThrowUserException();

			      if ((p = ColValues.CdrAssoc(_T("CARICO"))) == NULL) AfxThrowUserException();
			      if (gsc_RbSubst(p, 0.0) == GS_BAD) AfxThrowUserException();

			      if ((p = ColValues.CdrAssoc(_T("PRESSIONE"))) == NULL) AfxThrowUserException();
			      if (gsc_RbSubst(p, 0.0) == GS_BAD) AfxThrowUserException();

			      if ((p = ColValues.CdrAssoc(_T("EROGAZIONE"))) == NULL) AfxThrowUserException();
			      if (gsc_RbSubst(p, 0.0) == GS_BAD) AfxThrowUserException();

			      if (UpdData(pSub, ColValues, pRs) == GS_BAD) AfxThrowUserException();

		         // chiusura record set
		         if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
            }

            pItem = (C_INT_LONG *) pItem->get_next();
         }
      }
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::UpdateNullSub2											*/
/*+
   Aggiorna a video le entit� determinate eseguendo una SELECT sul campo 
   VELOCITA=-5 (valore "impossibile", utilizzato proprio per recuperare i 
   record non aggiornati con i risultati della simulazione) ed azzera
   successivamente tale campo.

   Parametri
   input:   C_CLASS  *pSub;   classe
            C_SELSET &SelSet;

   Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::UpdateNullSub2(C_CLASS *pSub, C_SELSET &SelSet)
{
	int            ret = GS_GOOD;
	C_INFO			*pInfo = pSub->ptr_info();
	C_STRING		   TableRef, statement;
	C_DBCONNECTION	*pConn;
	_RecordsetPtr	pRs;
	C_RB_LIST		ColValues;
	presbuf			p;

	TRY 
	{
		if (rete_tutta == true)
      {
		   if (pSub->getTempTableRef(TableRef)==GS_BAD)
			   AfxThrowUserException();

		   statement = _T("SELECT * FROM ");
		   statement += TableRef;
		   statement += _T(" WHERE ");
		   statement += pInfo->key_attrib;
		   statement += _T(">0 AND VELOCITA=-5");

		   // Ricavo la connessione OLE - DB
		   if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
			   AfxThrowUserException();
   		
		   // Eseguo l' istruzione e ricavo il record set
		   if (pConn->OpenRecSet(statement, pRs, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
			   AfxThrowUserException();

		   while (gsc_isEOF(pRs) == GS_BAD)
         {
			   if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
				   AfxThrowUserException();
   			
			   if ((p = ColValues.CdrAssoc(_T("VELOCITA"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PERD_CARIC"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PORTATA_IN"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PORTATA_FI"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if (UpdData(pSub, ColValues, pRs) == GS_BAD) AfxThrowUserException();

			   gsc_Skip(pRs);
		   }

		   // chiusura record set
		   gsc_DBCloseRs(pRs);
      }
      else
      {
         // come sopra ma solo per i links tolopologicamente connessi alla rete 
         // considerata dal simulatore (a questo punto ptr_NetLinks contiene solo
         // i links NON ancora aggiornati)
         C_PREPARED_CMD pCmd;
         C_INT_LONG     *pItem = (C_INT_LONG *) Topo.ptr_NetLinks()->get_head();
         int            IsRecordsetCloseable;
         C_SELSET       ss;

         if (pSub->prepare_data(pCmd, TEMP) == GS_BAD) AfxThrowUserException();

         while (pItem)
         {
   			if (pSub->get_SelSet(pItem->get_id(), ss, ALL) == GS_GOOD)
      			SelSet.add_selset(ss);

		      // cerco nel temp
            if (gsc_get_data(pCmd, pItem->get_id(), pRs, &IsRecordsetCloseable) == GS_BAD)
   			   AfxThrowUserException();

		      if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
			      AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("VELOCITA"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PERD_CARIC"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PORTATA_IN"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PORTATA_FI"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if (UpdData(pSub, ColValues, pRs) == GS_BAD) AfxThrowUserException();
		         // chiusura record set
		         if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);

            pItem = (C_INT_LONG *) pItem->get_next();
         }
      }
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc int CHydroMap::UpdateNullSub4										*/
/*+
   Aggiorna a video le entit� determinate eseguendo una SELECT sul campo 
   PREVALENZA=-5 (valore "impossibile", utilizzato proprio per recuperare 
   i record non aggiornati con i risultati della simulazione) ed azzera 
   successivamente tale campo.

   Parametri
   input:   C_CLASS        *pSub                 classe
            C_SELSET &SelSet;

   Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::UpdateNullSub4(C_CLASS *pSub, C_SELSET &SelSet)
{
	int ret = GS_GOOD;

	C_INFO			*pInfo = pSub->ptr_info();
	C_STRING		TableRef, statement;

	C_DBCONNECTION	*pConn;

	_RecordsetPtr	pRs;

	C_RB_LIST		ColValues;
	presbuf			p;

	TRY 
	{
		if (rete_tutta == true)
      {
		   if (pSub->getTempTableRef(TableRef)==GS_BAD)
			   AfxThrowUserException();

		   statement = _T("SELECT * FROM ");
		   statement += TableRef;
		   statement += _T(" WHERE ");
		   statement += pInfo->key_attrib;
		   statement += _T(">0 AND PREVALENZA=-5");

		   // Ricavo la connessione OLE - DB
		   if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
			   AfxThrowUserException();
   		
		   // Eseguo l' istruzione e ricavo il record set
		   if (pConn->OpenRecSet(statement, pRs, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
			   AfxThrowUserException();

		   while (gsc_isEOF(pRs) == GS_BAD)
         {
			   if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
				   AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PREVALENZA"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PORTATA"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if (UpdData(pSub, ColValues, pRs) == GS_BAD) AfxThrowUserException();

			   gsc_Skip(pRs);
		   }

		   // chiusura record set
		   gsc_DBCloseRs(pRs);
      }
      else
      {
         // come sopra ma solo per i nodi tolopologicamente connessi alla rete 
         // considerata dal simulatore (a questo punto VisitedNodes contiene solo
         // i nodi NON ancora aggiornati)
         C_PREPARED_CMD pCmd;
         C_INT_LONG     *pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
         int            IsRecordsetCloseable;
         C_SELSET       ss;

         if (pSub->prepare_data(pCmd, TEMP) == GS_BAD) AfxThrowUserException();

         while (pItem)
         {
            if (pItem->get_key() == 4)
            {
   			   if (pSub->get_SelSet(pItem->get_id(), ss, ALL) == GS_GOOD)
      			   SelSet.add_selset(ss);

		         // cerco nel temp
               if (gsc_get_data(pCmd, pItem->get_id(), pRs, &IsRecordsetCloseable) == GS_BAD)
   			      AfxThrowUserException();

		         if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
			         AfxThrowUserException();

			      if ((p = ColValues.CdrAssoc(_T("PREVALENZA"))) == NULL) AfxThrowUserException();
			      if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			      if ((p = ColValues.CdrAssoc(_T("PORTATA"))) == NULL) AfxThrowUserException();
			      if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			      if (UpdData(pSub, ColValues, pRs) == GS_BAD) AfxThrowUserException();

		         // chiusura record set
		         if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
            }

            pItem = (C_INT_LONG *) pItem->get_next();
         }
      }
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc int CHydroMap::UpdateNullSub6										*/
/*+
	Aggiorna a video le entit� determinate eseguendo una SELECT sui campi 
	PRES_MONTE=-5 e PRES_VALLE=-5 (valori "impossibili", utilizzati proprio 
	per recuperare i record non aggiornati con i risultati della simulazione)
	ed azzera successivamente tali campi.

	Parametri
	input:   C_CLASS        *pSub                         classe
            C_SELSET &SelSet;

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::UpdateNullSub6(C_CLASS *pSub, C_SELSET &SelSet)
{
	int ret = GS_GOOD;

	C_INFO			*pInfo = pSub->ptr_info();
	C_STRING		TableRef, statement;

	C_DBCONNECTION	*pConn;

	_RecordsetPtr	pRs;

	C_RB_LIST		ColValues;
	presbuf			p;

	TRY 
	{
		if (rete_tutta == true)
      {
		   if (pSub->getTempTableRef(TableRef)==GS_BAD)
			   AfxThrowUserException();

		   statement = _T("SELECT * FROM ");
		   statement += TableRef;
 		   statement += _T(" WHERE ");
		   statement += pInfo->key_attrib;
		   statement += _T(">0 AND PRES_MONTE=-5");

		   // Ricavo la connessione OLE - DB
		   if ((pConn = pInfo->getDBConnection(TEMP)) == NULL)
			   AfxThrowUserException();
   		
		   // Eseguo l' istruzione e ricavo il record set
		   if (pConn->OpenRecSet(statement, pRs, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
			   AfxThrowUserException();

		   while (gsc_isEOF(pRs) == GS_BAD)
         {
			   if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
				   AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PRES_MONTE"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("CARI_MONTE"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if (UpdData(pSub, ColValues, pRs) == GS_BAD) AfxThrowUserException();

			   gsc_Skip(pRs);
		   }

		   // chiusura record set
		   gsc_DBCloseRs(pRs);

		   statement = _T("SELECT * FROM ");
		   statement += TableRef;
 		   statement += _T(" WHERE ");
		   statement += pInfo->key_attrib;
		   statement += _T(">0 AND PRES_VALLE=-5");

		   // Eseguo l' istruzione e ricavo il record set
		   if (pConn->OpenRecSet(statement, pRs, adOpenForwardOnly, adLockPessimistic) == GS_BAD)
			   AfxThrowUserException();

		   while (gsc_isEOF(pRs) == GS_BAD)
         {
			   if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
				   AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("PRES_VALLE"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if ((p = ColValues.CdrAssoc(_T("CARI_VALLE"))) == NULL) AfxThrowUserException();
			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			   if (UpdData(pSub, ColValues, pRs) == GS_BAD) AfxThrowUserException();
   		
			   gsc_Skip(pRs);
		   }

		   // chiusura record set
		   gsc_DBCloseRs(pRs);
      }
      else
      {
         // come sopra ma solo per i nodi tolopologicamente connessi alla rete 
         // considerata dal simulatore
         C_PREPARED_CMD pCmd;
         C_INT_LONG     *pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
         int            IsRecordsetCloseable;
         double         Num;
         C_SELSET       ss;

         if (pSub->prepare_data(pCmd, TEMP) == GS_BAD) AfxThrowUserException();

         while (pItem)
         {
            if (pItem->get_key() == 6)
            {
   			   if (pSub->get_SelSet(pItem->get_id(), ss, ALL) == GS_GOOD)
      			   SelSet.add_selset(ss);

               // cerco nel temp
               if (gsc_get_data(pCmd, pItem->get_id(), pRs, &IsRecordsetCloseable) == GS_BAD)
   			      AfxThrowUserException();

		         if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
			         AfxThrowUserException();

			      if ((p = ColValues.CdrAssoc(_T("PRES_MONTE"))) == NULL) AfxThrowUserException();
               if (gsc_rb2Dbl(p, &Num) == GS_GOOD && Num == -5)
               {
      			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			         if ((p = ColValues.CdrAssoc(_T("CARI_MONTE"))) == NULL) AfxThrowUserException();
			         if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();
               }

			      if ((p = ColValues.CdrAssoc(_T("PRES_VALLE"))) == NULL) AfxThrowUserException();
               if (gsc_rb2Dbl(p, &Num) == GS_GOOD && Num == -5)
               {
      			   if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();

			         if ((p = ColValues.CdrAssoc(_T("CARI_VALLE"))) == NULL) AfxThrowUserException();
			         if (gsc_RbSubst(p, 0) == GS_BAD) AfxThrowUserException();
               }

			      if (UpdData(pSub, ColValues, pRs) == GS_BAD) AfxThrowUserException();

		         // chiusura record set
		         if (IsRecordsetCloseable == GS_GOOD) gsc_DBCloseRs(pRs);
            }

            pItem = (C_INT_LONG *) pItem->get_next();
         }
      }
	}
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc int CHydroMap::InToGEOsim()											*/
/*+
	Leggendo i file prodotti dalla simulazione HYDROMAP (NODI.OUT e 
	LATI.OUT) modifica opportunamente i dati (ovvero le tabelle) e 
	aggiorna il video.

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::InToGEOsim()
{
	int             ret = GS_GOOD, found=TRUE;
	FILE		       *nodifile = NULL, *latifile=NULL;
	C_CLASS		    *pSub1=NULL, *pSub2=NULL, *pSub3=NULL, *pSub4=NULL, *pSub5=NULL, *pSub6=NULL;
	C_PREPARED_CMD  pCmdSub1;
	C_PREPARED_CMD  pCmdSub2;
	C_PREPARED_CMD  pCmdSub3;
	C_PREPARED_CMD  pCmdSub4;
	C_PREPARED_CMD  pCmdSub5;
	C_PREPARED_CMD  pCmdSub6;
	long		       count = 0, gs_id;
	double		    erogazione, carico, pressione, pressioneE; 
	long		       nodo_in, nodo_fi, temp_in, temp_fi;
	double		    portata_in, portata_fi, velocita, perd_carico, diametro;
	CString		    msg, msg2;
	C_SELSET	       SelSet, Sub1SS, Sub2SS, Sub3SS, Sub4SS, Sub5SS, Sub6SS;
   C_INT_LONG      *pItem, *pUpdateNode;
   C_INT_LONG_LIST UpdatedNodes;
   bool            Unicode = false, foundL = false;

   msg.LoadString(IDS_STRING278);  // "Lettura risultati della simulazione"
   C_STATUSBAR_PROGRESSMETER StatusBarProgressMeter(msg);
   StatusBarProgressMeter.Init(4); // 3 fasi

	TRY
	{
      StatusBarProgressMeter.Set(1); // prima fase

		if (gsc_path_exist(nodi)==GS_BAD)
			AfxThrowUserException();
		else
			if ((nodifile = gsc_fopen(nodi, _T("r"), MORETESTS, &Unicode)) == NULL)
			{ 
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();	
			}

		if (gsc_path_exist(lati)==GS_BAD)
			AfxThrowUserException();
		else
			if ((latifile = gsc_fopen(lati, _T("r"), MORETESTS, &Unicode)) == NULL)
			{ 
				gs_errmsg(eGSOpenFile);
				AfxThrowUserException();	
			}

		if (fseek(nodifile, nodiPos, SEEK_SET)!=0)
		{ 
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();	
		}
 
		if (fseek(latifile, latiPos, SEEK_SET)!=0)
		{	
			gs_errmsg(eGSOpenFile);
			AfxThrowUserException();	
		}
		
		// NODI
		if ((pSub1 = gsc_find_class(prj, cls, SUB1)) == NULL) 
			AfxThrowUserException();
		if (InSub1(pSub1, pCmdSub1)==GS_BAD)                                   
			AfxThrowUserException();	
		
		// LATI
		if ((pSub2 = gsc_find_class(prj, cls, SUB2)) == NULL)
			AfxThrowUserException();	
		if (InSub2(pSub2, pCmdSub2)==GS_BAD)
			AfxThrowUserException();	

		// SERBATOI/REGOLATORI
		if ((pSub3 = gsc_find_class(prj, cls, SUB3)) == NULL)
			AfxThrowUserException();	
		if (InSub3(pSub3, pCmdSub3)==GS_BAD)                                   
			AfxThrowUserException();	

		if (tipo_fluido == H2O)
		{
         // POMPE
			if ((pSub4 = gsc_find_class(prj, cls, SUB4)) == NULL)
				AfxThrowUserException();	
			if (InSub4(pSub4, pCmdSub4)==GS_BAD)
				AfxThrowUserException();	
  
         // PRV
			if ((pSub6 = gsc_find_class(prj, cls, SUB6)) == NULL)
				AfxThrowUserException();	
			if (InSub6(pSub6, pCmdSub6)==GS_BAD)
				AfxThrowUserException();	
		}
		else
		{
         // PRV
			if ((pSub5 = gsc_find_class(prj, cls, SUB5)) == NULL)
				AfxThrowUserException();	
			if (InSub6(pSub5, pCmdSub5)==GS_BAD)
				AfxThrowUserException();	
		}

      StatusBarProgressMeter.Set(2); // seconda fase

		msg.LoadString(IDS_STRING279); // "Aggiornamento"
      msg2.LoadString(IDS_STRING277); // "entit� puntuali"
      C_STATUSLINE_MESSAGE StatusLineMsg(_T("%s %s"), msg, msg2);
      StatusLineMsg.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

		// leggo tutto il file NODI.OUT
		while (fwscanf(nodifile, _T("%ld,%lf,%lf,%lf"), &gs_id, &erogazione, &carico, &pressione) != EOF)
		{
			StatusLineMsg.Set(++count); // "%ld entit� GEOsim elaborate."

			if ((lastN <= gs_id) && (gs_id <= (3*lastN)))
            continue; // la riga non interessa, quindi la salto
			
			pressioneE = 0;
			if (gs_id >= 4*lastN)
				pressioneE = pressione;

			if (rete_tutta == true)
			{
				found=TRUE;
        
				// NODI
				if (UpdateSub1(pCmdSub1, pSub1, carico, pressione, erogazione, gs_id, &found) == GS_BAD)
					AfxThrowUserException();	

				if (found==FALSE)
				{
					found=TRUE;

					// SERBATOI
					if (UpdateSub3(pCmdSub3, pSub3, erogazione, pressioneE, gs_id, &found) == GS_BAD)
						AfxThrowUserException();	

					if (tipo_fluido == H2O)
					{
						if (found==FALSE)
						{ 
							found=TRUE;
              
							// POMPE
							if (UpdateSub4(pCmdSub4, pSub4, pressione, erogazione, gs_id, &found) == GS_BAD)
								AfxThrowUserException();	

							if (found==FALSE)
							{ 
   							// PRV
								if (UpdateSub6(pCmdSub6, pSub6, pressione, carico, gs_id) == GS_BAD)
									AfxThrowUserException();	
							}
						}
					}
					else // gas
					{ 
						if (found==FALSE)
   						// PRV
							if (UpdateSub6(pCmdSub5, pSub5, pressione, carico, gs_id) == GS_BAD)
								AfxThrowUserException();	
					}
				}
			}
			else // simula solo la rete connessa ad un punto
			{
				foundL = false;

				// lo rimuovo dai nodi esportati al simulatore          
				if (!(pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->search_key(gs_id)) && gs_id >= (3 * lastN))
				{
					// si tratta di un nodo virtuale per valvole, pompe, prv
					pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->search_key(gs_id - (3 * lastN));
					// si tratta di un nodo virtuale per regolatori ed allora necessaria finta
					if ((!pItem) && (gs_id >= 4 * lastN))
					{
						pItem = new C_INT_LONG(3, 0);
						foundL = true;
					}
				}

				if (pItem || foundL)
				{
					if (!foundL)
					{
						// Segno in una lista i nodi che ho aggiornato
						pUpdateNode = new C_INT_LONG(pItem->get_key(), pItem->get_id());
						UpdatedNodes.add_tail(pUpdateNode);
					}

					switch (pItem->get_key())
					{
						case 1:
			            // NODI
							if (UpdateSub1(pCmdSub1, pSub1, carico, pressione, erogazione, gs_id, &found) == GS_BAD ||
								found == FALSE)
								AfxThrowUserException();	
   							if (pSub1->get_SelSet(gs_id, SelSet, ALL) == GS_GOOD)
      							Sub1SS.add_selset(SelSet);
							break;
						case 3:
						// SERBATOI-REGOLATORI
							if (UpdateSub3(pCmdSub3, pSub3, erogazione, pressioneE, gs_id, &found) == GS_BAD ||
								found == FALSE)
								AfxThrowUserException();	
   							if (pSub3->get_SelSet(gs_id, SelSet, ALL) == GS_GOOD)
      							Sub3SS.add_selset(SelSet);
							break;
						case 4:
							if (tipo_fluido == H2O)
							{
								// POMPE
								if (UpdateSub4(pCmdSub4, pSub4, pressione, erogazione, gs_id, &found) == GS_BAD ||
									found == FALSE)
									AfxThrowUserException();	
   								if (pSub4->get_SelSet(gs_id, SelSet, ALL) == GS_GOOD)
      								Sub4SS.add_selset(SelSet);
							}
							break;
						case 5:
							if (tipo_fluido != H2O) // gas
							{
   								// PRV
								if (UpdateSub6(pCmdSub5, pSub5, pressione, carico, gs_id) == GS_BAD ||
									found == FALSE)
									AfxThrowUserException();	
   								if (pSub5->get_SelSet(gs_id, SelSet, ALL) == GS_GOOD)
      								Sub5SS.add_selset(SelSet);
							}
							break;
						case 6:
							if (tipo_fluido == H2O)
							{
   								// PRV
								if (UpdateSub6(pCmdSub6, pSub6, pressione, carico, gs_id) == GS_BAD ||
									found == FALSE)
									AfxThrowUserException();	
   								if (pSub6->get_SelSet(gs_id, SelSet, ALL) == GS_GOOD)
      								Sub6SS.add_selset(SelSet);
							}
							break;
					}
				}
				if (foundL)
					free(pItem);
			}   
		}

		// Cancello dalla ptr_NetNodes i nodi che ho aggiornato
		pUpdateNode = (C_INT_LONG *) UpdatedNodes.get_head();
		while (pUpdateNode)
		{
			Topo.ptr_NetNodes()->remove_key(pUpdateNode->get_id());
			pUpdateNode = (C_INT_LONG *) pUpdateNode->get_next();
		}

      StatusLineMsg.End(gsc_msg(310), count); // "%ld entit� GEOsim elaborate."
		
      StatusBarProgressMeter.Set(3); // terza fase

		msg.LoadString(IDS_STRING279); // "Aggiornamento"
      msg2.LoadString(IDS_STRING280); // "entit� lineari"
      C_STATUSLINE_MESSAGE StatusLineMsg2(_T("%s %s"), msg, msg2);
      StatusLineMsg2.Init(gsc_msg(310), MEDIUM_STEP); // ogni 100 "%ld entit� GEOsim elaborate."

		count=0;
			
		// leggo tutto il file LATI.OUT
		while (fwscanf(latifile, _T("%ld,%ld,%ld,%lf,%lf,%lf,%lf,%lf"), 
                              &gs_id, &nodo_in, &nodo_fi, &portata_in,
                              &portata_fi, &velocita, &perd_carico,
                              &diametro) != EOF)
		{
			count++;

			if (gs_id == 0 || nodo_in < 0) continue; // la riga non interessa, quindi la salto
  
			if (rete_tutta == false)
			{
				Topo.ptr_NetLinks()->remove_key(gs_id); // lo rimuovo dai lati esportati al simulatore
   				if (pSub2->get_SelSet(gs_id, SelSet, ALL) == GS_GOOD)
      				Sub2SS.add_selset(SelSet);
			}

			StatusLineMsg2.Set(count); // "%ld entit� GEOsim elaborate."

			temp_in=nodo_in;
			if (temp_in>(3*lastN)) temp_in-=(3*lastN);         
			if (temp_in>lastN) temp_in-=lastN;         
			temp_fi=nodo_fi;
			if (temp_fi>(3*lastN)) temp_fi-=(3*lastN);         
			if (temp_fi>lastN) temp_fi-=lastN;   

			if (UpdateSub2(pCmdSub2, pSub2, portata_in, portata_fi, velocita, perd_carico, 
	   						(tipo_simulazione == 2) ? diametro:NULL, gs_id, temp_in, temp_fi)==GS_BAD)
				AfxThrowUserException();	
		}

      StatusLineMsg2.End(gsc_msg(310), count); // "%ld entit� GEOsim elaborate."

		// Alcune entit� potrebbero non essere coinvolte nell'ultima simulazione eseguita,
		// (esempio esistenza valvola chiusa),
		// � pertanto necessario aggiornare tali entit� con valori nulli.
		if (UpdateNullSub1(pSub1, Sub1SS)==GS_BAD) 
			AfxThrowUserException();	
		if (UpdateNullSub2(pSub2, Sub2SS)==GS_BAD) 
			AfxThrowUserException();	

		if (tipo_fluido == H2O)
		{ 
			if (UpdateNullSub4(pSub4, Sub4SS)==GS_BAD) 
				AfxThrowUserException();	
			if (UpdateNullSub6(pSub6, Sub6SS)==GS_BAD) 
				AfxThrowUserException();	
		}

		// Controllo user break
		if (acedUsrBrk())	AfxThrowUserException();	

		if (rete_tutta == true)
		{
			if (pSub1->get_SelSet(Sub1SS, ALL) == GS_BAD)
				AfxThrowUserException();	

			if (pSub2)
			{
				if (pSub2->get_SelSet(Sub2SS, ALL) == GS_BAD)
					AfxThrowUserException();	
			}
			if (pSub3)
			{
				if (pSub3->get_SelSet(Sub3SS, ALL) == GS_BAD)
					AfxThrowUserException();	
			}
			if (pSub4)
			{
				if (pSub4->get_SelSet(Sub4SS, ALL) == GS_BAD)
					AfxThrowUserException();	
			}
			if (pSub5)
			{
				if (pSub5->get_SelSet(Sub5SS, ALL) == GS_BAD)
					AfxThrowUserException();	
			}
			if (pSub6)
			{
				if (pSub6->get_SelSet(Sub6SS, ALL) == GS_BAD)
					AfxThrowUserException();	
			}
		}

      StatusBarProgressMeter.Set(4); // quarta fase
		if (aggiorna_grafica)
		{
			if (gsc_class_align(Sub1SS, GS_BAD, GS_BAD, NULL, GS_GOOD, RECORD_MOD) == -1)
				AfxThrowUserException();	
         
			if (gsc_class_align(Sub2SS, GS_BAD, GS_BAD, NULL, GS_GOOD, RECORD_MOD) == -1)
				AfxThrowUserException();	
         
			if (gsc_class_align(Sub3SS, GS_BAD, GS_BAD, NULL, GS_GOOD, RECORD_MOD) == -1)
				AfxThrowUserException();	
         
			if (gsc_class_align(Sub4SS, GS_BAD, GS_BAD, NULL, GS_GOOD, RECORD_MOD) == -1)
				AfxThrowUserException();	

			if (gsc_class_align(Sub5SS, GS_BAD, GS_BAD, NULL, GS_GOOD, RECORD_MOD) == -1)
				AfxThrowUserException();	

			if (gsc_class_align(Sub6SS, GS_BAD, GS_BAD, NULL, GS_GOOD, RECORD_MOD) == -1)
				AfxThrowUserException();	
		}	

		if (nodifile)
		{
			if (fclose(nodifile) == EOF) 
				AfxThrowUserException();	
			else
				nodifile=NULL;
		}

		if (latifile)
		{
			if (fclose(latifile) == EOF) 
				AfxThrowUserException();	
			else
				latifile=NULL;
		}
	}
	CATCH( CUserException, e )
	{
		if (nodifile)
		{
			fclose(nodifile);
			nodifile=NULL;
		}

		if (latifile)
		{
			fclose(latifile);
			latifile=NULL;
		}

		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


int CHydroMap::sim_h2o()
{
	int		ret=GS_GOOD; 

	CString	msg;
	CString	cpt;
	
	tipo_fluido = H2O;
	tipo_simulazione = 0;
	CAcModuleResourceOverride	thisResource;

	TRY
	{
		if ((ret=FileRemove())!=GS_GOOD)
			AfxThrowUserException();

		do
		{
			if ((ret=SelectNetwork())!=GS_GOOD)
				AfxThrowUserException();

			if ((ret = is_GEOnetworkClass(pCls, H2O)) == GS_BAD)
			{
				cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
				msg.LoadString(HY_STRING228); // "Selezione non valida per Simulazione Rete Acqua."
				CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OK | MB_ICONEXCLAMATION);
			}
		}
		while (ret==GS_BAD);
      
		if (OpenDialogParameters_h2o() == GS_GOOD)
		{
			if (rete_tutta == true)
			{
				if ((ret = OutFromGEOsim()) != GS_GOOD) AfxThrowUserException();
			}
			else
				if ((ret = PartialOutFromGEOsim()) != GS_GOOD) AfxThrowUserException();

			if ((ret=SimulaExec())!=GS_GOOD)
				AfxThrowUserException();
   	
			if ((ret=FileControl())!=GS_GOOD)
				AfxThrowUserException();

			if ((ret=InToGEOsim())!=GS_GOOD)
				AfxThrowUserException();
		}

		ret = GS_GOOD;
	}   
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}

int CHydroMap::sim_gb()
{
	int		ret=GS_GOOD; 

	CString	msg;
	CString	cpt;
	
	tipo_fluido = GB;
	tipo_simulazione = 0;
	CAcModuleResourceOverride	thisResource;

	TRY
	{
      // roby test
		if ((ret=FileRemove())!=GS_GOOD)
			AfxThrowUserException();

		do
		{
			if ((ret=SelectNetwork())!=GS_GOOD)
				AfxThrowUserException();

			if ((ret=is_GEOnetworkClass(pCls, GB))==GS_BAD)
			{
				cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
				msg.LoadString(HY_STRING229); // "Selezione non valida per Simulazione Rete Gas Bassa."
				CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OK | MB_ICONEXCLAMATION);
			}
		}
		while (ret==GS_BAD);
      
		if (OpenDialogParameters_gb() == GS_GOOD)
      {
		   if (rete_tutta == true)
		   {
		      if ((ret = OutFromGEOsim()) != GS_GOOD) AfxThrowUserException();
		   }
         else
		      if ((ret = PartialOutFromGEOsim()) != GS_GOOD) AfxThrowUserException();

		   if ((ret=SimulaExec())!=GS_GOOD)
			   AfxThrowUserException();
   	
		   if ((ret=FileControl())!=GS_GOOD)
			   AfxThrowUserException();
   	
		   if ((ret=InToGEOsim())!=GS_GOOD)
			   AfxThrowUserException();
      }

      ret = GS_GOOD;
   }   
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}

int CHydroMap::sim_gm()
{
	int		ret=GS_GOOD; 

	CString	msg;
	CString	cpt;
	
	tipo_fluido = GM;
	tipo_simulazione = 0;
	CAcModuleResourceOverride	thisResource;

	TRY
	{
		if ((ret=FileRemove())!=GS_GOOD)
			AfxThrowUserException();

		do
		{
			if ((ret=SelectNetwork())!=GS_GOOD)
				AfxThrowUserException();

			if ((ret=is_GEOnetworkClass(pCls, GM))==GS_BAD)
			{
				cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
				msg.LoadString(HY_STRING230); // "Selezione non valida per Simulazione Rete Gas Media."
				CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OK | MB_ICONEXCLAMATION);
			}
		}
		while (ret==GS_BAD);
      
		if (OpenDialogParameters_gm() == GS_GOOD)
      {
		   if (rete_tutta == true)
		   {
		      if ((ret = OutFromGEOsim()) != GS_GOOD) AfxThrowUserException();
		   }
         else
		      if ((ret = PartialOutFromGEOsim()) != GS_GOOD) AfxThrowUserException();

		   if ((ret=SimulaExec())!=GS_GOOD)
			   AfxThrowUserException();
   	
		   if ((ret=FileControl())!=GS_GOOD)
			   AfxThrowUserException();
   	
		   if ((ret=InToGEOsim())!=GS_GOOD)
			   AfxThrowUserException();
      }

      ret = GS_GOOD;
	}   
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc int CHydroMap::OutSub5												*/
/*+
	Questa funzione tratta le VALVOLE della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e tabella,
	scandisce i dati ottenuti (utilizzando il cursore restituito), li modifica 
	secondo precise regole, e li scrive in file di output; conta inoltre il 
	numero delle VALVOLE.     
   
	Parametri
	input:
   C_CLASS *pSub;    classe per determinare la tabella sulla 
                     quale effettuare la SELECT 
	CString *fields;  campi sui quali effettuare la SELECT

	output:
   long *count;      numero delle VALVOLE
   FILE *file;       file dei dati
    
   Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::OutSub5(C_CLASS *pSub, CString *fields, long *count, FILE *file)
{
	int           ret = GS_GOOD; 
	C_ID		     *pId = pSub->ptr_id();
	C_INFO        *pInfo = pSub->ptr_info();
	long		     i = 0;	
	CString       str;
	long		     gs_id, last, on_off;
	C_2INT_LONG	  *el = NULL;
	C_SELSET	     ss;
	int           in_graphic = TRUE;
	C_RB_LIST     ColValues;
	presbuf       p = NULL;
	_RecordsetPtr pRs;

	TRY
	{
		if (hy_outRecordSet(pSub, fields, NULL, pRs)==GS_BAD) 
			AfxThrowUserException( );

		// Conto le righe che soddisfano l' istruzione SQL
		if (gsc_RowsQty(pRs, count) == GS_BAD)
			AfxThrowUserException( );

		if ((*count) > 0)
		{
         CString msg, msg2;

         msg.LoadString(IDS_STRING274);  // "Scrittura"
         msg2.LoadString(IDS_STRING271); // "valvole"
         C_STATUSBAR_PROGRESSMETER StatusBarProgressMeter(_T("%s %s"), msg, msg2);
         StatusBarProgressMeter.Init(*count);

			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();
		
			// Scorro il record set 
			while (gsc_isEOF(pRs) == GS_BAD)
			{
            StatusBarProgressMeter.Set(++i);

				// Leggo la riga
				if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il key attrib
				if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
					AfxThrowUserException( );
               
				// controlla se l'entit� � nella lista delle cancellate
				if (deleted_list.get_count()>0)
					el = deleted_list.search(pId->code, pId->sub_code, gs_id);
		
				if ((gs_id>last)||(el))
				{
					el = NULL;

					// ricavo il link set collegato alla scheda
					if (pSub->get_SelSet(gs_id, ss) == GS_BAD)
						AfxThrowUserException();

					if (ss.length() <= 0) in_graphic = FALSE;
				}

				if (in_graphic)
				{         
					if ((p = ColValues.CdrAssoc(_T("CHIUSA"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvAttribName);
						AfxThrowUserException( );
					}
					
					// Memorizzo on_off
					if (p -> restype == RTNIL) 
						on_off = 0;
					else
						if (gsc_rb2Lng(p, &on_off) == GS_BAD) 
							AfxThrowUserException( );
					
					if (on_off != 0)
					{
						str = gsc_tostring(gs_id+lastN);
						if (fwprintf(file, _T("%s\n"), str) < 0)
						{ 
							gs_errmsg(eGSWriteFile); 
							AfxThrowUserException( );
						}
						(*count)++;
					}

					str = gsc_tostring(gs_id);
					if (fwprintf(file, _T("%s\n"), str) < 0)
					{ 
						gs_errmsg(eGSWriteFile); 
						AfxThrowUserException( );
					}
				}
				else
				{
					(*count)--;
					in_graphic = TRUE;
				}
				gsc_Skip(pRs);
			}
         StatusBarProgressMeter.End(gsc_msg(1090)); // "Terminato."

			// chiusura record set
			gsc_DBCloseRs(pRs);
		}
	}   
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc int CHydroMap::OutSub2												*/
/*+
	Questa funzione tratta i LATI della SIMULAZIONE.
	Dopo aver effettuato lo statement SELECT sugli opportuni campi e tabella, 
	scandisce i dati ottenuti (utilizzando il cursore restituito), li modifica
	secondo precise regole, e li scrive in file di output; conta inoltre il 
	numero dei LATI.     
   
	Parametri
	input:	C_CLASS		*pSub		classe per determinare la tabella sulla 
									quale effettuare la SELECT 
			CString		*fields		campi sui quali effettuare la SELECT
	output:	long		*count		numero dei LATI
			FILE		*file		file dei dati
    
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/****************************************************************************/
int CHydroMap::OutSub2(C_CLASS *pSub, CString *fields, long *count, FILE *file,
                       long id, long *in)
{
	int	         ret = GS_GOOD; 
	C_ID	         *pId = pSub->ptr_id();
	C_INFO         *pInfo = pSub->ptr_info();
	long	         i = 0;	
	long	         gs_id, n_iniziale, n_finale, last;
	C_2INT_LONG	   *el = NULL;
	C_SELSET	      ss;
	int		      in_graphic = TRUE, OnOffFlag;
	C_RB_LIST      ColValues, dummy;
	presbuf	      p = NULL;
	_RecordsetPtr  pRs;
	CString		   str;            // Str = nodo iniziale, nodo finale, handle
	C_CLASS		   *valCls;
	C_PREPARED_CMD pCmdVal;
	ads_name	      entity;
	TCHAR		      handle[MAX_LEN_HANDLE];
	long		      nhandle;

	TRY
	{
		if (tipo_fluido == H2O)
		{
			if ((valCls = gsc_find_class(prj, pId->code, SUB5)) == NULL)
				AfxThrowUserException();
		}
		else
			if ((valCls = gsc_find_class(prj, pId->code, SUB4)) == NULL)
				AfxThrowUserException();

		if (hy_outRecordSet(pSub, fields, NULL, pRs)==GS_BAD) 
			AfxThrowUserException( );

		// Conto le righe che soddisfano l' istruzione SQL
		if (gsc_RowsQty(pRs, count) == GS_BAD)
			AfxThrowUserException( );

		if ((*count) > 0)
		{
         CString msg, msg2;

         msg.LoadString(IDS_STRING274);  // "Scrittura"
         msg2.LoadString(IDS_STRING268); // "lati"
         C_STATUSBAR_PROGRESSMETER StatusBarProgressMeter(_T("%s %s"), msg, msg2);
         StatusBarProgressMeter.Init(*count);

			// la seguente funzione compila uno statement SQL parametrico di SELECT
			// per le VALVOLE della SIMULAZIONE; tale statement permette di 
			// verificare se la fine di un LATO della SIMULAZIONE coincide con 
			// una VALVOLA.
			if (valvoleN>0)
				if (valCls->prepare_data(pCmdVal, TEMP) == GS_BAD)
					AfxThrowUserException( );
				
			// se l'entit� � "nuova" (ovvero � stata inserita nella sessione corrente) oppure � nella lista delle
			// cancellate si controlla l'esistenza di tale entit� in grafica.
			last = ((C_INT_LONG*) lastcode_list.search_key(pId->sub_code))->get_id();
		
			// Scorro il record set 
			while (gsc_isEOF(pRs) == GS_BAD)
			{
            StatusBarProgressMeter.Set(++i);

				// Leggo la riga
				if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
					AfxThrowUserException( );

				if ((p = ColValues.CdrAssoc(pInfo->key_attrib.get_name())) == NULL) 
				{ 
					set_GS_ERR_COD(eGSInvRBType);
					AfxThrowUserException( );
				}
				// Memorizzo il key attrib
				if (gsc_rb2Lng(p, &gs_id) == GS_BAD) 
					AfxThrowUserException( );
               
				// controlla se l'entit� � nella lista delle cancellate
				if (deleted_list.get_count()>0)
					el = deleted_list.search(pId->code, pId->sub_code, gs_id);
		
				// ricavo il link set collegato alla scheda
				if (pSub->get_SelSet(gs_id, ss, GRAPHICAL) == GS_BAD)
					AfxThrowUserException();

				if ((gs_id>last)||(el))
				{
					el = NULL;
				
					if (ss.length() <= 0) in_graphic = FALSE;
				}

				if (in_graphic)
				{
					if ((p = ColValues.CdrAssoc(_T("NODO_INIZ"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvAttribName);
						AfxThrowUserException( );
					}
					// Memorizzo n_iniziale
					if (gsc_rb2Lng(p, &n_iniziale) == GS_BAD) 
						AfxThrowUserException( );

					if ((p = ColValues.CdrAssoc(_T("NODO_FINAL"))) == NULL) 
					{ 
						set_GS_ERR_COD(eGSInvAttribName);
						AfxThrowUserException( );
					}
					// Memorizzo n_finale
					if (gsc_rb2Lng(p, &n_finale) == GS_BAD) 
						AfxThrowUserException( );
  
					if (id == gs_id)
						(*in) = n_iniziale;

					// la seguente funzione, utilizzando lo statement SQL 
					// parametrico di SELECT per le VALVOLE della SIMULAZIONE,
					// verifica se il parametro "n_iniziale" � una VALVOLA
               // con CHIUSA diverso da 0
					if ((n_iniziale!=0) && (valvoleN>0))
					{
						if (IsValueInDB(pCmdVal, n_iniziale, dummy) == GS_GOOD &&
                     gsc_rb2Int(dummy.CdrAssoc(_T("CHIUSA")), &OnOffFlag) == GS_GOOD &&
                     OnOffFlag != 0)
							n_iniziale += lastN;
					}

					str = gsc_tostring(gs_id); // Scrivo ID lato
					str += _T(",");
					str += gsc_tostring(n_iniziale);
					str += _T(",");
					str += gsc_tostring(n_finale);
					str += _T(",");

					if (ss.length() > 0)
					{					
						// ricavo l'oggetto grafico collegato
						if (ss.entname(0, entity) != GS_GOOD) 
							AfxThrowUserException( );
               
						if (gsc_enthand(entity, handle) == GS_BAD)
						{ 
							gs_errmsg(eGSUnknown); 
							AfxThrowUserException( );
						}

						nhandle=(long)wcstol(handle, NULL, 16);
						str += gsc_tostring(nhandle);

					}
					else
						str += gsc_tostring(0);

					if (fwprintf(file, _T("%s\n"), str) < 0)
					{ 
						gs_errmsg(eGSWriteFile); 
						AfxThrowUserException( );
					}
         
					str = _T("");
				}
				else
				{
					(*count)--;
					in_graphic = TRUE;
				}
				gsc_Skip(pRs);
			}
         StatusBarProgressMeter.End(gsc_msg(1090)); // "Terminato."

			// chiusura record set
			gsc_DBCloseRs(pRs);
		}
	}   
	CATCH( CUserException, e )
	{
		// chiusura record set
		gsc_DBCloseRs(pRs);

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


/****************************************************************************/
/*.doc CHydroMap::EvidConduits()											*/
/*+

   Restituisce RTNORM in caso di successo altrimenti restituisce RTERROR.
-*/  
/****************************************************************************/
int	CHydroMap::EvidConduits()
{
	int	ret=GS_GOOD;

	CString	hydro2;
	FILE		*hydro2file=NULL;

	TCHAR		handle[MAX_LEN_HANDLE];
	long		nhandle;

	ads_name	ent;
	C_SELSET	SelSet;

	C_CLASS	*pSub;
   bool     Unicode = false;
	
	TRY
	{
		hydro2 = temp_dir;
		hydro2 += _T("hydro2.u28");
		if ((hydro2file = gsc_fopen(hydro2, _T("r"), MORETESTS, &Unicode)) == NULL)
			AfxThrowUserException( );

		// leggo tutto il file hydro2.u28
		while (fwscanf(hydro2file, _T("%ld"), &nhandle) != EOF)
		{
			if (nhandle!=0)
			{
				_itow_s(nhandle, handle, 16);
				// Attraverso l'handle risalgo all'ads_name dell' entit�
				if (acdbHandEnt(handle, ent) != RTNORM)
					AfxThrowUserException( );
       
				if (SelSet.add(ent) != GS_GOOD) 
				{ 
					set_GS_ERR_COD(eGSInvalidSelectionSet);
					AfxThrowUserException( );
				}
			}
		}

		if ((pSub = gsc_find_class(prj, cls, SUB2)) == NULL)
			AfxThrowUserException( );

		HyAppl::GS_LS.clear();

		if (SelSet.length() > 0) 
		{
			HyAppl::GS_LS.ptr_SelSet()->add_selset(SelSet);
			HyAppl::GS_LS.cls = cls;
			HyAppl::GS_LS.sub = SUB2;
         hy_PutSym_hyss(HyAppl::GS_LS);

			if ((ret = gsc_ddevid(pSub, SelSet)) != GS_GOOD)
				AfxThrowUserException( );
		}   
	  
		if (hydro2file)
			fclose(hydro2file);
	}   
	CATCH( CUserException, e )
	{
		if (hydro2file)
		{
			fclose(hydro2file);
			hydro2file = NULL;
		}

		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}

int CHydroMap::sim_evid()
{
	int		ret=GS_GOOD; 

	int		len;
	C_CLASS	*pSub;

	TRY
	{
		acedRetVoid();
		gsc_startTransaction();

		len = HyAppl::GS_LS.ptr_SelSet()->length();
		if (len == 0 || HyAppl::GS_LS.cls == 0)
			acutPrintf(gsc_msg(117), 0); // "\n%ld oggetti grafici filtrati."
		else
		{
			if ((pSub = gsc_find_class(get_GS_CURRENT_WRK_SESSION()->get_PrjId(), HyAppl::GS_LS.cls, HyAppl::GS_LS.sub)) == NULL)
				AfxThrowUserException( );

			if (gsc_ddevid(pSub, *(HyAppl::GS_LS.ptr_SelSet())) != GS_GOOD)
			{ 
				gsc_abortTransaction(); 
				AfxThrowUserException( );
			}
		}

		gsc_endTransaction();
	}   
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}

int CHydroMap::sim_cd()
{
	int		ret=GS_GOOD; 

	CString	msg;
	CString	cpt;

	CString	Catalog, pathFile;

	C_DBCONNECTION *pConn;

	tipo_simulazione = 2;
	CAcModuleResourceOverride	thisResource;

	TRY
	{
		Catalog = get_GEODIR();
		Catalog += _T('\\');
		Catalog += DIAMC;
      
		C_2STR_LIST UDLProp(_T("Data Source"), Catalog);

		if ((ret=FileRemove())!=GS_GOOD)
			AfxThrowUserException();

		do
		{
			if ((ret=SelectNetwork())!=GS_GOOD)
				AfxThrowUserException();

			if (is_GEOnetworkClass(pCls, H2O)==GS_GOOD)
				tipo_fluido=H2O;
			else
			{
				if (is_GEOnetworkClass(pCls, GB)==GS_GOOD)
					tipo_fluido=GB;
				else
				{
					if (is_GEOnetworkClass(pCls, GM)==GS_GOOD)
						tipo_fluido=GM;
					else
					{
						ret=GS_BAD;	
						cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
						msg.LoadString(HY_STRING201); // "Selezione non valida per il Calcolo Diametri"
						CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OK | MB_ICONEXCLAMATION);
					}
				}
			}
		}
		while (ret==GS_BAD);
				
		pConn = get_pDBCONNECTION_LIST()->get_Connection(NULL, &UDLProp);
		
		switch (tipo_fluido)
		{
			case H2O:
				ret = OpenDialogParameters_cd_h2o(pConn);
				break;
			case GB:
				ret = OpenDialogParameters_cd_gb(pConn);
				break;
			case GM:
				ret = OpenDialogParameters_cd_gm(pConn);
				break;
		}		

		if (ret == GS_GOOD)
		{
			if (rete_tutta == true)
			{
				if ((ret = OutFromGEOsim()) != GS_GOOD) AfxThrowUserException();
			}
			else
				if ((ret = PartialOutFromGEOsim()) != GS_GOOD) AfxThrowUserException();

			if ((ret=SimulaExec())!=GS_GOOD)
				AfxThrowUserException();
   	
			if ((ret=FileControl())!=GS_GOOD)
				AfxThrowUserException();

			if ((ret=FileClear())!=GS_GOOD)
				AfxThrowUserException();
   	
			if ((ret=InToGEOsim())!=GS_GOOD)
				AfxThrowUserException();
	
			if (pConn)
			{
				pConn = NULL;
			}
		}
      
		ret = GS_GOOD;
	}   
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// GESTIONE TOOLBAR molto articolato vedere esempio in ObjectARX
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


//	CMDIFrameWnd *pAcadFrame = acedGetAcadFrame();
// BOOL s2 = theApp.pToolBar->LoadToolBar(IDR_LATOOLBAR);

// class theApp
// {

// }