// gs_HydroMap.h : main header file for the GS_HYDROMAP DLL
//

#ifndef _gs_hy_h
#define _gs_hy_h 1


#include "stdafx.h"

#define INITGUID
#import "msado15.dll" no_namespace rename ("EOF", "EndOfFile") rename ("EOS", "ADOEOS")


#ifndef _INC_MATH
	#include <math.h>
#endif

#ifndef _gs_def_h
   #include "gs_def.h"   
#endif

#ifndef _gs_utily_h
   #include "gs_utily.h"
#endif

#ifndef _gs_init_h
   #include "gs_init.h"
#endif

#ifndef _gs_class_h
   #include "gs_class.h" 
#endif

#ifndef _gs_query_h
   #include "gs_query.h" 
#endif

#ifndef _gs_area_h
   #include "gs_area.h" 
#endif

#ifndef _gs_graph_h
   #include "gs_graph.h"
#endif

#ifndef _gs_sql_h
   #include "gs_sql.h"
#endif

#ifndef _gs_topo_h
   #include "gs_topo.h"
#endif

#ifndef _hy_dia_sim_h
	#include "hy_dia_sim.h"
#endif

#ifndef _hy_dia_diamc_h
	#include "hy_dia_diamc.h"
#endif

#ifndef _hy_dia_cd_h
	#include "hy_dia_cd.h"
#endif

#ifndef _hy_diamc_h
	#include "hy_diamc.h"
#endif

#ifndef _gs_evid_h
   #include "gs_evid.h"
#endif

#include "resource.h"

#define   NETWORK_INI_FILE _T("NETWORK.INI") // file di inizializzazione del modulo NETWORK di GEOsim in GEOsimAppl::WORKDIR\NETWORK


/////////////////////////////////////////////////////////////////////////////
// CGs_HydroMapApp
// See gs_HydroMap.cpp for the implementation of this class
//

class CGs_HydroMapApp : public CWinApp
{
public:
	CGs_HydroMapApp();
	virtual ~CGs_HydroMapApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGs_HydroMapApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGs_HydroMapApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

class CSub1
{
public:
	long	gs_id;
	CString	numero;
	CString	quota;
	CString	consumo;

	CString carico;
	CString	pressione;
	CString	erogazione;

public:
	CSub1();
	~CSub1();
};


class CSub2
{
public:
	long	gs_id;
	CString	lunghezza;
	double	diametro;
	CString	portata_d;
	CString	velocita;
	CString	perc_caric;
	CString	portata_in;
	CString	portata_fi;
	CString	verso_q_in;
	CString	verso_q_fi;
	long	nodo_iniz;
	long	nodo_final;
	CString	legge_sc;
	CString	param_sc;
	int		calc;

public:
	CSub2();
	~CSub2();
};


class CSub3
{
public:
	long	gs_id;
	CString	numero;
	CString	quota;
	CString	pressione;
	CString	q_uscente;
	CString	max_erogazione;

public:
	CSub3();
	~CSub3();
};

class CSub4
{
public:
	long	gs_id;
	CString	numero;
	double	quota_asp;
	CString	curva;
	double	q_nom;
	double	h_nom;
	long	on_off;

public:
	CSub4();
	~CSub4();
};

class CSub5
{
public:		
	long	gs_id;
	CString	numero;
	long	on_off;
	CString	quota;

public:
	CSub5();
	~CSub5();
};


class CSub6
{
public:
	long	gs_id;
	CString	numero;
	double	quota;
	double	pressione;
	CString	nodo_rego;
	CString	pres_monte;
	CString	pres_valle;
	CString	cari_monte;
	CString	cari_valle;

public:
	CSub6();
	~CSub6();
};





/////////////////////////////////////////////////////////////////////////////

class CHydroMap
{
public:

	CHydroMap();
   ~CHydroMap();

   C_CLASS	*pCls;

	int SelectNetwork(long *id = NULL, int *sub = NULL);

private:
	CString	dir_server, dir_gs, file_dir, temp_dir;	// rispettivamente   -  directory di WORKDIR

	NetworkTypesEnum tipo_fluido;		// H2O=acqua     GB=gas bassa pressione     GM=gas media pressione
	int		tipo_simulazione;			// 0=statica     1=dinamica                 2=calcolo diametri         3=connect

// Variabili settate a valori di default, vengono variate attraverso 
// la DCL "Impostazione parametri di calcolo"
	int		tipo_controllo;			// 0=carico    1=portata   2=carico e portata   3=carico o portata
	double	controllo_portata;		// portata di controllo
	double	controllo_carico;		// carico di controllo
	double   coeff_molt_portate;		// coeff. moltiplicativo portate
	double	convergenza_pompe;		// fattore di convergenza pompe
	double	precisione_pompe;		// precisione pompe
	int		pompa_in_curva;			// flag di pompe in curva 1=Sì 0=No
	double	min_velocita_prv;		// minima velocità PRV
	double	min_portata_prv;		// minima portata PRV
	double	densita_relativa;		// densità relativa acqua

	int		numero_iterazioni;			// numero iterazioni per il "calcolo diametri"
	double	velocita_approssimazione;	// velocità per una prima approssimazione dei diametri

//*?* VEDERE DOVE METTERLI o SETTARLI
	double	perdita_approsimazione;		// (!!NON USATO!!) perdita di carico per una prima approssimazione dei diametri     
	double	tolleranza_diametri;		// (!!NON USATO!!) tolleranza diametri per il "calcolo diametri"
	
	int		prj;
	int		cls;

	C_2INT_LONG_LIST  deleted_list;
	C_INT_LONG_LIST   lastcode_list;
	C_4LONG_LIST		valve_list;

	CString		errore, nodi, lati;
	CString		simula;

	long		lastN, nodiN, latiN, valvoleN, prvN, regolatoriN;

	long		nodiPos, latiPos;

	C_SUB		*pSub;

   bool     rete_tutta;
   bool     aggiorna_grafica;

   int      startSub;
   long     startId;

   // Per controllo su rete connessa
   C_TOPOLOGY Topo;

private:
	int OpenDialogParameters_h2o();
	int OpenDialogParameters_gb();
	int OpenDialogParameters_gm();
	int OpenDialogParameters_cd_h2o(C_DBCONNECTION *pConn);
	int OpenDialogParameters_cd_gb(C_DBCONNECTION *pConn);
	int OpenDialogParameters_cd_gm(C_DBCONNECTION *pConn);

	int FileControl();
	int FileClear();
	int FileRemove();
	int SimulaExec();		
	
	int UpdateSub1(C_PREPARED_CMD &pCmd, C_CLASS *pSub, double value1, 
				   double value2, double value3, long gs_id, int *found);
	int UpdateSub2(C_PREPARED_CMD &pCmd, C_CLASS *pSub, double value1, double value2, 
				   double value3, double value4, double value5,
               long gs_id, long nodo_in, long nodo_fi);
	int UpdateSub3(C_PREPARED_CMD &pCmd, C_CLASS *pSub, double value1, double value2,
				   long gs_id, int *found);
	int UpdateSub4(C_PREPARED_CMD &pCmd, C_CLASS *pSub, double value1, 
				   double value2, long gs_id, int *found);
	int UpdateSub6(C_PREPARED_CMD &pCmd, 
				   C_CLASS *pSub, double value1, double value2, long gs_id);

	int UpdateNullSub1(C_CLASS *pSub, C_SELSET &SelSet);
	int UpdateNullSub2(C_CLASS *pSub, C_SELSET &SelSet);
	int UpdateNullSub4(C_CLASS *pSub, C_SELSET &SelSet);
	int UpdateNullSub6(C_CLASS *pSub, C_SELSET &SelSet);
	
	int OutSub1(C_CLASS *pSub, CString *fields, long *count, FILE *filetemp1, FILE *filecons1);
	int OutSub2(C_CLASS *pSub, CString *fields, long *count, FILE *filetemp1, FILE *filecons1);
	int OutSub3(C_CLASS *pSub, CString *fields, long *count, long *countR, FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2);
	int OutSub4(C_CLASS *pSub, CString *fields, long *count, FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2);
	int OutSub5(C_CLASS *pSub, CString *fields, long *count, FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2);
	int OutPrv(C_CLASS *pSub, CString *fields, long *count, FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2);

	int PartialOutSub1(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList, long *count, FILE *filetemp1, FILE *filecons1);
	int PartialOutSub2(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &linksList, long *count, FILE *filetemp1, FILE *filecons1);
	int PartialOutSub3(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList, long *count, long *countR, FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2);
	int PartialOutSub4(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList, long *count, FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2);
	int PartialOutSub5(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList, long *count, FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2);
	int PartialOutPrv(C_CLASS *pSub, CString *fields, C_INT_LONG_LIST &nodesList, long *count, FILE *filetemp1, FILE *filetemp2, FILE *filecons1, FILE *filecons2);

	int InSub1(C_CLASS *pSub, C_PREPARED_CMD &pCmd);
   _RecordsetPtr prepare_link_where_node_iniz(C_DBCONNECTION *pConn,
                                              C_STRING &TableName);
	int InSub2(C_CLASS *pSub, C_PREPARED_CMD &pCmd);
	int InSub3(C_CLASS *pSub, C_PREPARED_CMD &pCmd);
	int InSub4(C_CLASS *pSub, C_PREPARED_CMD &pCmd);
	int InSub6(C_CLASS *pSub, C_PREPARED_CMD &pCmd);
	
	int OutFromGEOsim();
	int PartialOutFromGEOsim();
	int InToGEOsim();

	int OutSub5(C_CLASS *pSub, CString *fields, long *count, FILE *file);
	int OutSub2(C_CLASS *pSub, CString *fields, long *count, FILE *file, long gs_id, long *in);

	int EvidConduits();
	
	int IsValueInDB(C_PREPARED_CMD &pCmd, long value, C_RB_LIST &ColValues);

	TCHAR* gsc_max_number(C_CLASS *pCls, TCHAR *field, double *num);
	TCHAR* gsc_min_number(C_CLASS *pCls, TCHAR *field, double *num);

	int UpdData(C_CLASS *pSub, C_RB_LIST &ColValues, _RecordsetPtr &pRs);

public:
	int sim_h2o();
	int sim_gb();
	int sim_gm();
	int sim_evid();
	int sim_cd();
};

#endif
