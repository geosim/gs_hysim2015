// Hy_dia_cd.cpp : implementation file
//

#include "StdAfx.h"
#include "hy_dia_cd.h"

#include "hy_utily.h" // chiamata all'help html
#include "d2hMap.h" // doc to help

/////////////////////////////////////////////////////////////////////////////
// CHy_dia_cd_h2o dialog


C_STRING CHy_dia_cd_h2o::FILE_EXT = _T("H2O_diam_calc_params"); // estensione dei file dei parametri di calcolo
CHy_dia_cd_h2o::CHy_dia_cd_h2o(CWnd* pParent /*=NULL*/)
	: CDialog(CHy_dia_cd_h2o::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHy_dia_cd_h2o)
	//}}AFX_DATA_INIT
	SetDefault();
}

/*****************************************************************************/
/*.doc CHy_dia_cd_h2o::SetDefault                                            */
/*+
  Questa funzione setta i parametri di calcolo di default.
-*/  
/*****************************************************************************/
void CHy_dia_cd_h2o::SetDefault()
{
	coeff_molt_portate = 1;

   tipologia = 0;
   controllo_carico = 0.50;
	controllo_portata = 0.20;

	convergenza = 1;
	convergenza_pompe = 0.10; 
   precisione_pompe = 1;
			
   min_velocita_prv = 0.50;
   min_portata_prv = 0.50;

	codice_diamC = 0;

   iterazioni = 1;
   velocita_riferimento = 1;
}

/*****************************************************************************/
/*.doc CHy_dia_cd_h2o::Load                                                  */
/*+
  Questa funzione carica i parametri di calcolo da un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_cd_h2o::Load(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("H2O DIAM");

	if ((f = gsc_open_profile(Path, READONLY, MORETESTS, &Unicode)) == NULL) return GS_BAD;

   if (gsc_get_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue, Unicode) == GS_GOOD)
   	coeff_molt_portate = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("CONTROL_TYPE"), EntryValue, Unicode) == GS_GOOD)
   	tipologia = EntryValue.toi();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	controllo_carico = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_FLOW"), EntryValue, Unicode) == GS_GOOD)
   	controllo_portata = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("CONVERGENCE"), EntryValue, Unicode) == GS_GOOD)
   	convergenza = EntryValue.toi();

   if (gsc_get_profile(f, SezName.get_name(), _T("PUMP_CONVERGENCE"), EntryValue, Unicode) == GS_GOOD)
   	convergenza_pompe = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("PUMP_PRECISION"), EntryValue, Unicode) == GS_GOOD)
   	precisione_pompe = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue, Unicode) == GS_GOOD)
   	min_velocita_prv = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	min_portata_prv = EntryValue.tof();

   return gsc_close_profile(f);
}


/*****************************************************************************/
/*.doc CHy_dia_cd_h2o::Save                                                  */
/*+
  Questa funzione salva i parametri di calcolo in un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_cd_h2o::Save(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("H2O DIAM");

   if ((f = gsc_open_profile(Path, UPDATEABLE, MORETESTS, &Unicode)) == NULL) return GS_BAD;
   
   EntryValue = coeff_molt_portate;
   if (gsc_set_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = tipologia;
   if (gsc_set_profile(f, SezName.get_name(), _T("CONTROL_TYPE"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = controllo_carico;
   if (gsc_set_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_HEAD"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = controllo_portata;
   if (gsc_set_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_FLOW"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = convergenza;
   if (gsc_set_profile(f, SezName.get_name(), _T("CONVERGENCE"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = convergenza_pompe;
   if (gsc_set_profile(f, SezName.get_name(), _T("PUMP_CONVERGENCE"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = precisione_pompe;
   if (gsc_set_profile(f, SezName.get_name(), _T("PUMP_PRECISION"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = min_velocita_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = min_portata_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   return gsc_close_profile(f);
}


void CHy_dia_cd_h2o::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHy_dia_cd_h2o)
	DDX_Control(pDX, IDC_VELOCITA, m_min_velocita_prv);
	DDX_Control(pDX, IDC_VEL_RIF, m_velocita_riferimento);
	DDX_Control(pDX, IDC_SMORZAMENTO, m_convergenza_pompe);
	DDX_Control(pDX, IDC_MINPORTATA, m_min_portata_prv);
	DDX_Control(pDX, IDC_ITERAZ, m_iterazioni);
	DDX_Control(pDX, IDC_CONTROLLO, m_precisione_pompe);
	DDX_Control(pDX, IDC_COEFF_PORTATE, m_coeff_molt_portate);
	DDX_Control(pDX, IDC_CONV_SI, m_conv_si);
	DDX_Control(pDX, IDC_LISTA_DC, m_lista_diametri);
	DDX_Control(pDX, IDC_COMBO_TIPOLOGIA, m_tipologia);
	DDX_Control(pDX, IDC_PORTATA, m_controllo_portata);
	DDX_Control(pDX, IDC_CARICO, m_controllo_carico);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHy_dia_cd_h2o, CDialog)
	//{{AFX_MSG_MAP(CHy_dia_cd_h2o)
	ON_EN_CHANGE(IDC_CARICO, OnChangeCarico)
	ON_EN_CHANGE(IDC_PORTATA, OnChangePortata)
	ON_CBN_SELCHANGE(IDC_COMBO_TIPOLOGIA, OnSelchangeComboTipologia)
	ON_CBN_SELCHANGE(IDC_LISTA_DC, OnSelchangeListaDc)
	ON_BN_CLICKED(IDC_CONV_NO, OnConvNo)
	ON_BN_CLICKED(IDC_CONV_SI, OnConvSi)
	ON_EN_CHANGE(IDC_COEFF_PORTATE, OnChangeCoeffPortate)
	ON_EN_CHANGE(IDC_CONTROLLO, OnChangeControllo)
	ON_EN_CHANGE(IDC_ITERAZ, OnChangeIteraz)
	ON_EN_CHANGE(IDC_MINPORTATA, OnChangeMinportata)
	ON_EN_CHANGE(IDC_SMORZAMENTO, OnChangeSmorzamento)
	ON_EN_CHANGE(IDC_VEL_RIF, OnChangeVelRif)
	ON_EN_CHANGE(IDC_VELOCITA, OnChangeVelocita)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
   ON_BN_CLICKED(IDLOAD, &CHy_dia_cd_h2o::OnBnClickedLoad)
   ON_BN_CLICKED(IDSAVE, &CHy_dia_cd_h2o::OnBnClickedSave)
   ON_BN_CLICKED(IDDEFAULT, &CHy_dia_cd_h2o::OnBnClickedDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHy_dia_cd_h2o message handlers

void CHy_dia_cd_h2o::OnChangeCarico() 
{
	CString carico;
	
	m_controllo_carico.GetWindowText(carico);

   if (gsc_is_numeric(carico) == GS_GOOD)
   	controllo_carico = _wtof(carico);
   else
	   m_controllo_carico.SetWindowText(_T(""));
}

void CHy_dia_cd_h2o::OnChangePortata() 
{
	CString portata;
	
	m_controllo_portata.GetWindowText(portata);

   if (gsc_is_numeric(portata) == GS_GOOD)
   	controllo_portata = _wtof(portata);
   else
	   m_controllo_portata.SetWindowText(_T(""));
}

BOOL CHy_dia_cd_h2o::OnInitDialog()
{
	CDialog::OnInitDialog();

   InitCtrls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHy_dia_cd_h2o::InitCtrls()
{
   C_STRING StrValue;
	C_INT_STR	*punt;

   StrValue = coeff_molt_portate;
	m_coeff_molt_portate.SetWindowText(StrValue.get_name());

   // Delete every item from the combo box m_tipologia
   for (int i = m_tipologia.GetCount() - 1; i >= 0; i--)
      m_tipologia.DeleteString(i);

   m_tipologia.AddString(_T("carico"));
	m_tipologia.AddString(_T("portata"));
	m_tipologia.AddString(_T("carico e portata"));
	m_tipologia.AddString(_T("carico o portata"));
	m_tipologia.SetCurSel(tipologia);

   StrValue = controllo_carico;
	m_controllo_carico.SetWindowText(StrValue.get_name());
   StrValue = controllo_portata;
	m_controllo_portata.SetWindowText(StrValue.get_name());

	switch (tipologia)
	{
		case 0:
			m_controllo_carico.EnableWindow(TRUE);
			m_controllo_portata.EnableWindow(FALSE);
			break;
		case 1:
			m_controllo_carico.EnableWindow(FALSE);
			m_controllo_portata.EnableWindow(TRUE);
			break;
		case 2:
		case 3:
			m_controllo_carico.EnableWindow(TRUE);
			m_controllo_portata.EnableWindow(TRUE);
			break;
	}
	
	m_conv_si.SetCheck(convergenza);
   StrValue = convergenza_pompe;
	m_convergenza_pompe.SetWindowText(StrValue.get_name());
   StrValue = precisione_pompe;
	m_precisione_pompe.SetWindowText(StrValue.get_name());

   StrValue = min_velocita_prv;
	m_min_velocita_prv.SetWindowText(StrValue.get_name());
	StrValue = min_portata_prv;
	m_min_portata_prv.SetWindowText(StrValue.get_name());

	diamC.Init();
   diamC.GetListDiamC(&list);

   // Delete every item from the combo box m_lista_diametri
   for (int i = m_lista_diametri.GetCount() - 1; i >= 0; i--)
      m_lista_diametri.DeleteString(i);

	punt = (C_INT_STR *) list.get_head();
	while (punt != NULL)
	{ 
		m_lista_diametri.AddString(punt->get_name());
		punt = (C_INT_STR *)punt->get_next();
	}

   StrValue = iterazioni;
	m_iterazioni.SetWindowText(StrValue.get_name());
   StrValue = velocita_riferimento;
	m_velocita_riferimento.SetWindowText(StrValue.get_name());
}

void CHy_dia_cd_h2o::OnSelchangeComboTipologia() 
{
	tipologia = m_tipologia.GetCurSel();
   InitCtrls();
}

void CHy_dia_cd_h2o::OnSelchangeListaDc() 
{
	CString nome_diamC;
	
	C_INT_STR *punt;

	m_lista_diametri.GetLBText(m_lista_diametri.GetCurSel(), nome_diamC);

	punt = (C_INT_STR *) list.get_head();
	while (punt != NULL)
	{ 
		if (punt->get_name() == nome_diamC)
		{
			codice_diamC = punt->get_key();
			break;
		}
		punt = (C_INT_STR *)punt->get_next();
	}
}


void CHy_dia_cd_h2o::OnConvNo() 
{
	convergenza = 0;	
}

void CHy_dia_cd_h2o::OnConvSi() 
{
	convergenza = 1;	
}

void CHy_dia_cd_h2o::OnChangeCoeffPortate() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString valore;
	
	m_coeff_molt_portate.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
	   coeff_molt_portate = _wtof(valore);
   else
	   m_coeff_molt_portate.SetWindowText(_T(""));
}

void CHy_dia_cd_h2o::OnChangeControllo() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_precisione_pompe.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
   	precisione_pompe = _wtof(valore);
   else
	   m_precisione_pompe.SetWindowText(_T(""));
}

void CHy_dia_cd_h2o::OnChangeIteraz() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_iterazioni.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
	   iterazioni = _wtoi(valore);
   else
	   m_iterazioni.SetWindowText(_T(""));
}

void CHy_dia_cd_h2o::OnChangeMinportata() 
{
	CString valore;
	
	m_min_portata_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      min_portata_prv = _wtof(valore);
   else
	   m_min_portata_prv.SetWindowText(_T(""));
}

void CHy_dia_cd_h2o::OnChangeSmorzamento() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_convergenza_pompe.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      convergenza_pompe = _wtof(valore);
   else
	   m_convergenza_pompe.SetWindowText(_T(""));
}

void CHy_dia_cd_h2o::OnChangeVelRif() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_velocita_riferimento.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      velocita_riferimento = _wtof(valore);
   else
	   m_velocita_riferimento.SetWindowText(_T(""));
}

void CHy_dia_cd_h2o::OnChangeVelocita() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_min_velocita_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      min_velocita_prv = _wtof(valore);
   else
	   m_min_velocita_prv.SetWindowText(_T(""));
}

void CHy_dia_cd_h2o::OnBnClickedLoad()
{
   C_STRING str, Path;

	if (get_GS_CURRENT_WRK_SESSION())
	{
		str = get_GS_CURRENT_WRK_SESSION()->get_pPrj()->get_dir();
      str += _T('\\');
      str += GEOSUPPORTDIR;
	}

   if (gsc_GetFileD(_T("GEOsim - Carica parametri di calcolo per rete acqua"),
                    str.get_name(), FILE_EXT.get_name(), UI_LOADFILE_FLAGS, Path) == RTNORM)
      if (Load(Path) == GS_GOOD)
         InitCtrls();
}

void CHy_dia_cd_h2o::OnBnClickedSave()
{
   C_STRING str, Path;

	if (get_GS_CURRENT_WRK_SESSION())
	{
		str = get_GS_CURRENT_WRK_SESSION()->get_pPrj()->get_dir();
      str += _T('\\');
      str += GEOSUPPORTDIR;
      str += _T('\\');
	}
   Path = _T('.');
   Path += FILE_EXT;
   if (gsc_get_tmp_filename(str.get_name(), _T("sim"), Path.get_name(), str) == GS_BAD)
      return;
   Path.clear();

   if (gsc_GetFileD(_T("GEOsim - Salva parametri di calcolo per rete acqua"),
                    str.get_name(), FILE_EXT.get_name(), UI_SAVEFILE_FLAGS, Path) == RTNORM)
      Save(Path);
}

void CHy_dia_cd_h2o::OnBnClickedDefault()
{
   SetDefault();
   InitCtrls();
}


/////////////////////////////////////////////////////////////////////////////
// CHy_dia_cd_gb dialog


C_STRING CHy_dia_cd_gb::FILE_EXT = _T("LPG_diam_calc_params"); // estensione dei file dei parametri di calcolo
CHy_dia_cd_gb::CHy_dia_cd_gb(CWnd* pParent /*=NULL*/)
	: CDialog(CHy_dia_cd_gb::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHy_dia_cd_gb)
	//}}AFX_DATA_INIT
	SetDefault();
}

/*****************************************************************************/
/*.doc CHy_dia_cd_gb::SetDefault                                             */
/*+
  Questa funzione setta i parametri di calcolo di default.
-*/  
/*****************************************************************************/
void CHy_dia_cd_gb::SetDefault()
{
	coeff_molt_portate = 1;

   tipologia = 0;
	controllo_carico = 0.5;
	controllo_portata = 0.2;

   densita_relativa = 0.544;

	codice_diamC = 0;
   iterazioni = 1;
   velocita_riferimento = 1;
			
   min_velocita_prv = 0.50; // roby chiedere a stefano
   min_portata_prv = 0.50;
}

/*****************************************************************************/
/*.doc CHy_dia_cd_gb::Load                                                   */
/*+
  Questa funzione carica i parametri di calcolo da un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_cd_gb::Load(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("LOW PRESSURE GAS DIAM");

	if ((f = gsc_open_profile(Path, READONLY, MORETESTS, &Unicode)) == NULL) return GS_BAD;

   if (gsc_get_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue, Unicode) == GS_GOOD)
   	coeff_molt_portate = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("CONTROL_TYPE"), EntryValue, Unicode) == GS_GOOD)
   	tipologia = EntryValue.toi();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	controllo_carico = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_FLOW"), EntryValue, Unicode) == GS_GOOD)
   	controllo_portata = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("RELATIVE_DENSITY"), EntryValue, Unicode) == GS_GOOD)
   	densita_relativa = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue, Unicode) == GS_GOOD)
   	min_velocita_prv = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	min_portata_prv = EntryValue.tof();

   return gsc_close_profile(f);
}


/*****************************************************************************/
/*.doc CHy_dia_cd_gb::Save                                                   */
/*+
  Questa funzione salva i parametri di calcolo in un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_cd_gb::Save(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("LOW PRESSURE GAS DIAM");

   if ((f = gsc_open_profile(Path, UPDATEABLE, MORETESTS, &Unicode)) == NULL) return GS_BAD;
   
   EntryValue = coeff_molt_portate;
   if (gsc_set_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = tipologia;
   if (gsc_set_profile(f, SezName.get_name(), _T("CONTROL_TYPE"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = controllo_carico;
   if (gsc_set_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_HEAD"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = controllo_portata;
   if (gsc_set_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_FLOW"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = densita_relativa;
   if (gsc_set_profile(f, SezName.get_name(), _T("RELATIVE_DENSITY"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = min_velocita_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = min_portata_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   return gsc_close_profile(f);
}


void CHy_dia_cd_gb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHy_dia_cd_gb)
	DDX_Control(pDX, IDC_VEL_RIF, m_velocita_riferimento);
	DDX_Control(pDX, IDC_ITERAZ, m_iterazioni);
	DDX_Control(pDX, IDC_DENS_RELATIVA, m_densita_relativa);
	DDX_Control(pDX, IDC_COEFF_PORTATE, m_coeff_molt_portate);
	DDX_Control(pDX, IDC_PORTATA, m_controllo_portata);
	DDX_Control(pDX, IDC_LISTA_DC, m_lista_diametri);
	DDX_Control(pDX, IDC_COMBO_TIPOLOGIA, m_tipologia);
	DDX_Control(pDX, IDC_CARICO, m_controllo_carico);
	DDX_Control(pDX, IDC_VELOCITA, m_min_velocita_prv);
	DDX_Control(pDX, IDC_MINPORTATA, m_min_portata_prv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHy_dia_cd_gb, CDialog)
	//{{AFX_MSG_MAP(CHy_dia_cd_gb)
	ON_EN_CHANGE(IDC_CARICO, OnChangeCarico)
	ON_EN_CHANGE(IDC_PORTATA, OnChangePortata)
	ON_CBN_SELCHANGE(IDC_COMBO_TIPOLOGIA, OnSelchangeComboTipologia)
	ON_CBN_SELCHANGE(IDC_LISTA_DC, OnSelchangeListaDc)
	ON_EN_CHANGE(IDC_COEFF_PORTATE, OnChangeCoeffPortate)
	ON_EN_CHANGE(IDC_DENS_RELATIVA, OnChangeDensRelativa)
	ON_EN_CHANGE(IDC_ITERAZ, OnChangeIteraz)
	ON_EN_CHANGE(IDC_VEL_RIF, OnChangeVelRif)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
   ON_EN_CHANGE(IDC_VELOCITA, OnEnChangeVelocita)
   ON_EN_CHANGE(IDC_MINPORTATA, OnEnChangeMinportata)
   ON_BN_CLICKED(IDOK, &CHy_dia_cd_gb::OnBnClickedOk)
   ON_BN_CLICKED(IDLOAD, &CHy_dia_cd_gb::OnBnClickedLoad)
   ON_BN_CLICKED(IDSAVE, &CHy_dia_cd_gb::OnBnClickedSave)
   ON_BN_CLICKED(IDDEFAULT, &CHy_dia_cd_gb::OnBnClickedDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHy_dia_cd_gb message handlers

BOOL CHy_dia_cd_gb::OnInitDialog() 
{
	CDialog::OnInitDialog();

   InitCtrls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHy_dia_cd_gb::InitCtrls()
{
   C_STRING  StrValue;
	C_INT_STR *punt;

   StrValue = coeff_molt_portate;
	m_coeff_molt_portate.SetWindowText(StrValue.get_name());
   
   // Delete every item from the combo box m_tipologia
   for (int i = m_tipologia.GetCount() - 1; i >= 0; i--)
      m_tipologia.DeleteString(i);

   m_tipologia.AddString(_T("carico"));
	m_tipologia.AddString(_T("portata"));
	m_tipologia.AddString(_T("carico e portata"));
	m_tipologia.AddString(_T("carico o portata"));
	m_tipologia.SetCurSel(tipologia);

   StrValue = controllo_carico;
	m_controllo_carico.SetWindowText(StrValue.get_name());
   StrValue = controllo_portata;
	m_controllo_portata.SetWindowText(StrValue.get_name());

	switch (tipologia)
	{
		case 0:
			m_controllo_carico.EnableWindow(TRUE);
			m_controllo_portata.EnableWindow(FALSE);
			break;
		case 1:
			m_controllo_carico.EnableWindow(FALSE);
			m_controllo_portata.EnableWindow(TRUE);
			break;
		case 2:
		case 3:
			m_controllo_carico.EnableWindow(TRUE);
			m_controllo_portata.EnableWindow(TRUE);
			break;
	}
	
   StrValue = densita_relativa;
   m_densita_relativa.SetWindowText(StrValue.get_name());

   StrValue = min_velocita_prv;
	m_min_velocita_prv.SetWindowText(StrValue.get_name());
	StrValue = min_portata_prv;
	m_min_portata_prv.SetWindowText(StrValue.get_name());

	diamC.Init();
   diamC.GetListDiamC(&list);

   // Delete every item from the combo box m_lista_diametri
   for (int i = m_lista_diametri.GetCount() - 1; i >= 0; i--)
      m_lista_diametri.DeleteString(i);

	punt = (C_INT_STR *) list.get_head();
	while (punt != NULL)
	{ 
		m_lista_diametri.AddString(punt->get_name());
		punt = (C_INT_STR *)punt->get_next();
	}

   StrValue = iterazioni;
	m_iterazioni.SetWindowText(StrValue.get_name());
   StrValue = velocita_riferimento;
	m_velocita_riferimento.SetWindowText(StrValue.get_name());
}

void CHy_dia_cd_gb::OnChangeCarico() 
{
	CString carico;
	
	m_controllo_carico.GetWindowText(carico);

   if (gsc_is_numeric(carico) == GS_GOOD)
      controllo_carico = _wtof(carico);
   else
	   m_controllo_carico.SetWindowText(_T(""));
}

void CHy_dia_cd_gb::OnChangePortata() 
{
	CString portata;
	
	m_controllo_portata.GetWindowText(portata);

   if (gsc_is_numeric(portata) == GS_GOOD)
   	controllo_portata = _wtof(portata);
   else
	   m_controllo_portata.SetWindowText(_T(""));
}

void CHy_dia_cd_gb::OnSelchangeComboTipologia() 
{
	tipologia = m_tipologia.GetCurSel();
   InitCtrls();
}

void CHy_dia_cd_gb::OnSelchangeListaDc() 
{
	CString nome_diamC;
	
	C_INT_STR *punt;

	m_lista_diametri.GetLBText(m_lista_diametri.GetCurSel(), nome_diamC);

	punt = (C_INT_STR *) list.get_head();
	while (punt != NULL)
	{ 
		if (punt->get_name() == nome_diamC)
		{
			codice_diamC = punt->get_key();
			break;
		}
		punt = (C_INT_STR *)punt->get_next();
	}
}

void CHy_dia_cd_gb::OnChangeCoeffPortate() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_coeff_molt_portate.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
	   coeff_molt_portate = _wtof(valore);
   else
	   m_coeff_molt_portate.SetWindowText(_T(""));
}

void CHy_dia_cd_gb::OnChangeDensRelativa() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_densita_relativa.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      densita_relativa = _wtof(valore);
   else
	   m_densita_relativa.SetWindowText(_T(""));
}

void CHy_dia_cd_gb::OnChangeIteraz() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
   m_iterazioni.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
	   iterazioni = _wtoi(valore);
   else
	   m_iterazioni.SetWindowText(_T(""));
}

void CHy_dia_cd_gb::OnChangeVelRif() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_velocita_riferimento.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      velocita_riferimento = _wtof(valore);
   else
	   m_velocita_riferimento.SetWindowText(_T(""));
}


/////////////////////////////////////////////////////////////////////////////
// CHy_dia_cd_gm dialog


C_STRING CHy_dia_cd_gm::FILE_EXT = _T("MPG_diam_calc_params"); // estensione dei file dei parametri di calcolo
CHy_dia_cd_gm::CHy_dia_cd_gm(CWnd* pParent /*=NULL*/)
	: CDialog(CHy_dia_cd_gm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHy_dia_cd_gm)
	//}}AFX_DATA_INIT
	SetDefault();
}

/*****************************************************************************/
/*.doc CHy_dia_cd_gm::SetDefault                                             */
/*+
  Questa funzione setta i parametri di calcolo di default.
-*/  
/*****************************************************************************/
void CHy_dia_cd_gm::SetDefault()
{
	coeff_molt_portate = 1;

   tipologia = 0;
	controllo_carico = 0.5;
	controllo_portata = 0.2;

   densita_relativa = 0.544;

	codice_diamC = 0;
   iterazioni = 1;
   velocita_riferimento = 1;
			
   min_velocita_prv = 0.50; // roby chiedere a stefano
   min_portata_prv = 0.50;
}

/*****************************************************************************/
/*.doc CHy_dia_cd_gm::Load                                                   */
/*+
  Questa funzione carica i parametri di calcolo da un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_cd_gm::Load(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("MEDIUM PRESSURE GAS DIAM");

	if ((f = gsc_open_profile(Path, READONLY, MORETESTS, &Unicode)) == NULL) return GS_BAD;

   if (gsc_get_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue, Unicode) == GS_GOOD)
   	coeff_molt_portate = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("CONTROL_TYPE"), EntryValue, Unicode) == GS_GOOD)
   	tipologia = EntryValue.toi();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	controllo_carico = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_FLOW"), EntryValue, Unicode) == GS_GOOD)
   	controllo_portata = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("RELATIVE_DENSITY"), EntryValue, Unicode) == GS_GOOD)
   	densita_relativa = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue, Unicode) == GS_GOOD)
   	min_velocita_prv = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	min_portata_prv = EntryValue.tof();

   return gsc_close_profile(f);
}


/*****************************************************************************/
/*.doc CHy_dia_cd_gm::Save                                                   */
/*+
  Questa funzione salva i parametri di calcolo in un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_cd_gm::Save(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("MEDIUM PRESSURE GAS DIAM");

   if ((f = gsc_open_profile(Path, UPDATEABLE, MORETESTS, &Unicode)) == NULL) return GS_BAD;
   
   EntryValue = coeff_molt_portate;
   if (gsc_set_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = tipologia;
   if (gsc_set_profile(f, SezName.get_name(), _T("CONTROL_TYPE"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = controllo_carico;
   if (gsc_set_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_HEAD"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = controllo_portata;
   if (gsc_set_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_FLOW"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = densita_relativa;
   if (gsc_set_profile(f, SezName.get_name(), _T("RELATIVE_DENSITY"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = min_velocita_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = min_portata_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   return gsc_close_profile(f);
}


void CHy_dia_cd_gm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHy_dia_cd_gm)
	DDX_Control(pDX, IDC_VEL_RIF, m_velocita_riferimento);
	DDX_Control(pDX, IDC_ITERAZ, m_iterazioni);
	DDX_Control(pDX, IDC_DENS_RELATIVA, m_densita_relativa);
	DDX_Control(pDX, IDC_COEFF_PORTATE, m_coeff_molt_portate);
	DDX_Control(pDX, IDC_LISTA_DC, m_lista_diametri);
	DDX_Control(pDX, IDC_COMBO_TIPOLOGIA, m_tipologia);
	DDX_Control(pDX, IDC_PORTATA, m_controllo_portata);
	DDX_Control(pDX, IDC_CARICO, m_controllo_carico);
	DDX_Control(pDX, IDC_VELOCITA, m_min_velocita_prv);
	DDX_Control(pDX, IDC_MINPORTATA, m_min_portata_prv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHy_dia_cd_gm, CDialog)
	//{{AFX_MSG_MAP(CHy_dia_cd_gm)
	ON_EN_CHANGE(IDC_CARICO, OnChangeCarico)
	ON_EN_CHANGE(IDC_PORTATA, OnChangePortata)
	ON_CBN_SELCHANGE(IDC_COMBO_TIPOLOGIA, OnSelchangeComboTipologia)
	ON_CBN_SELCHANGE(IDC_LISTA_DC, OnSelchangeListaDc)
	ON_EN_CHANGE(IDC_COEFF_PORTATE, OnChangeCoeffPortate)
	ON_EN_CHANGE(IDC_DENS_RELATIVA, OnChangeDensRelativa)
	ON_EN_CHANGE(IDC_ITERAZ, OnChangeIteraz)
	ON_EN_CHANGE(IDC_VEL_RIF, OnChangeVelRif)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
   ON_EN_CHANGE(IDC_VELOCITA, OnEnChangeVelocita)
   ON_EN_CHANGE(IDC_MINPORTATA, OnEnChangeMinportata)
   ON_BN_CLICKED(IDLOAD, &CHy_dia_cd_gm::OnBnClickedLoad)
   ON_BN_CLICKED(IDSAVE, &CHy_dia_cd_gm::OnBnClickedSave)
   ON_BN_CLICKED(IDDEFAULT, &CHy_dia_cd_gm::OnBnClickedDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHy_dia_cd_gm message handlers

BOOL CHy_dia_cd_gm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   InitCtrls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHy_dia_cd_gm::InitCtrls()
{
   C_STRING  StrValue;
	C_INT_STR *punt;
	
   StrValue = coeff_molt_portate;
	m_coeff_molt_portate.SetWindowText(StrValue.get_name());
   
   // Delete every item from the combo box m_tipologia
   for (int i = m_tipologia.GetCount() - 1; i >= 0; i--)
      m_tipologia.DeleteString(i);

   m_tipologia.AddString(_T("carico"));
	m_tipologia.AddString(_T("portata"));
	m_tipologia.AddString(_T("carico e portata"));
	m_tipologia.AddString(_T("carico o portata"));
	m_tipologia.SetCurSel(tipologia);

   StrValue = controllo_carico;
	m_controllo_carico.SetWindowText(StrValue.get_name());
   StrValue = controllo_portata;
	m_controllo_portata.SetWindowText(StrValue.get_name());

	switch(tipologia)
	{
		case 0:
			m_controllo_carico.EnableWindow(TRUE);
			m_controllo_portata.EnableWindow(FALSE);
			break;
		case 1:
			m_controllo_carico.EnableWindow(FALSE);
			m_controllo_portata.EnableWindow(TRUE);
			break;
		case 2:
		case 3:
			m_controllo_carico.EnableWindow(TRUE);
			m_controllo_portata.EnableWindow(TRUE);
			break;
	}
	
   StrValue = densita_relativa;
   m_densita_relativa.SetWindowText(StrValue.get_name());

   StrValue = min_velocita_prv;
	m_min_velocita_prv.SetWindowText(StrValue.get_name());
	StrValue = min_portata_prv;
	m_min_portata_prv.SetWindowText(StrValue.get_name());

	diamC.Init();
   diamC.GetListDiamC(&list);

   // Delete every item from the combo box m_lista_diametri
   for (int i = m_lista_diametri.GetCount() - 1; i >= 0; i--)
      m_lista_diametri.DeleteString(i);

	punt = (C_INT_STR *) list.get_head();
	while (punt != NULL)
	{ 
		m_lista_diametri.AddString(punt->get_name());
		punt = (C_INT_STR *)punt->get_next();
	}

   StrValue = iterazioni;
	m_iterazioni.SetWindowText(StrValue.get_name());
   StrValue = velocita_riferimento;
	m_velocita_riferimento.SetWindowText(StrValue.get_name());
}

void CHy_dia_cd_gm::OnChangeCarico() 
{
	CString carico;
	
	m_controllo_carico.GetWindowText(carico);

   if (gsc_is_numeric(carico) == GS_GOOD)
   	controllo_carico = _wtof(carico);
   else
	   m_controllo_carico.SetWindowText(_T(""));
}

void CHy_dia_cd_gm::OnChangePortata() 
{
	CString portata;
	
	m_controllo_portata.GetWindowText(portata);

   if (gsc_is_numeric(portata) == GS_GOOD)
   	controllo_portata = _wtof(portata);
   else
	   m_controllo_portata.SetWindowText(_T(""));
}

void CHy_dia_cd_gm::OnSelchangeComboTipologia() 
{
	tipologia = m_tipologia.GetCurSel();
   InitCtrls();
}

void CHy_dia_cd_gm::OnSelchangeListaDc() 
{
	CString nome_diamC;
	
	C_INT_STR *punt;

	m_lista_diametri.GetLBText(m_lista_diametri.GetCurSel(), nome_diamC);

	punt = (C_INT_STR *) list.get_head();
	while (punt != NULL)
	{ 
		if (punt->get_name() == nome_diamC)
		{
			codice_diamC = punt->get_key();
			break;
		}
		punt = (C_INT_STR *)punt->get_next();
	}
}

void CHy_dia_cd_gm::OnChangeCoeffPortate() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_coeff_molt_portate.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
	   coeff_molt_portate = _wtof(valore);
   else
	   m_coeff_molt_portate.SetWindowText(_T(""));
}

void CHy_dia_cd_gm::OnChangeDensRelativa() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_densita_relativa.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      densita_relativa = _wtof(valore);
   else
	   m_densita_relativa.SetWindowText(_T(""));
}

void CHy_dia_cd_gm::OnChangeIteraz() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_iterazioni.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
   	iterazioni = _wtoi(valore);
   else
	   m_iterazioni.SetWindowText(_T(""));
}

void CHy_dia_cd_gm::OnChangeVelRif() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_velocita_riferimento.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      velocita_riferimento = _wtof(valore);
   else
	   m_velocita_riferimento.SetWindowText(_T(""));
}

void CHy_dia_cd_gm::OnBnClickedLoad()
{
   C_STRING str, Path;

	if (get_GS_CURRENT_WRK_SESSION())
	{
		str = get_GS_CURRENT_WRK_SESSION()->get_pPrj()->get_dir();
      str += _T('\\');
      str += GEOSUPPORTDIR;
	}

   if (gsc_GetFileD(_T("GEOsim - Carica parametri di calcolo per rete gas media pressione"),
                    str.get_name(), FILE_EXT.get_name(), UI_LOADFILE_FLAGS, Path) == RTNORM)
      if (Load(Path) == GS_GOOD)
         InitCtrls();
}

void CHy_dia_cd_gm::OnBnClickedSave()
{
   C_STRING str, Path;

	if (get_GS_CURRENT_WRK_SESSION())
	{
		str = get_GS_CURRENT_WRK_SESSION()->get_pPrj()->get_dir();
      str += _T('\\');
      str += GEOSUPPORTDIR;
      str += _T('\\');
	}
   Path = _T('.');
   Path += FILE_EXT;
   if (gsc_get_tmp_filename(str.get_name(), _T("sim"), Path.get_name(), str) == GS_BAD)
      return;
   Path.clear();

   if (gsc_GetFileD(_T("GEOsim - Salva parametri di calcolo per rete acqua"),
                    str.get_name(), FILE_EXT.get_name(), UI_SAVEFILE_FLAGS, Path) == RTNORM)
      Save(Path);
}

void CHy_dia_cd_gm::OnBnClickedDefault()
{
   SetDefault();
   InitCtrls();
}

void CHy_dia_cd_gb::OnBnClickedHelp()
{
   CallHelp(m_hWnd, IDH_Calcolodeidiametri);
}

void CHy_dia_cd_gm::OnBnClickedHelp()
{
   CallHelp(m_hWnd, IDH_Calcolodeidiametri);
}

void CHy_dia_cd_h2o::OnBnClickedHelp()
{
   CallHelp(m_hWnd, IDH_Calcolodeidiametri);
}

void CHy_dia_cd_gm::OnEnChangeVelocita()
{
	CString valore;
	
	m_min_velocita_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      min_velocita_prv = _wtof(valore);
   else
	   m_min_velocita_prv.SetWindowText(_T(""));
}

void CHy_dia_cd_gm::OnEnChangeMinportata()
{
	CString valore;
	
	m_min_portata_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      min_portata_prv = _wtof(valore);
   else
	   m_min_portata_prv.SetWindowText(_T(""));
}

void CHy_dia_cd_gb::OnEnChangeVelocita()
{
	CString valore;
	
	m_min_velocita_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      min_velocita_prv = _wtof(valore);
   else
	   m_min_velocita_prv.SetWindowText(_T(""));
}

void CHy_dia_cd_gb::OnEnChangeMinportata()
{
	CString valore;
	
	m_min_portata_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
      min_portata_prv = _wtof(valore);
   else
	   m_min_portata_prv.SetWindowText(_T(""));
}
void CHy_dia_cd_gb::OnBnClickedOk()
{
   OnOK();
}

void CHy_dia_cd_gb::OnBnClickedLoad()
{
   C_STRING str, Path;

	if (get_GS_CURRENT_WRK_SESSION())
	{
		str = get_GS_CURRENT_WRK_SESSION()->get_pPrj()->get_dir();
      str += _T('\\');
      str += GEOSUPPORTDIR;
	}

   if (gsc_GetFileD(_T("GEOsim - Carica parametri di calcolo per rete gas bassa pressione"),
                    str.get_name(), FILE_EXT.get_name(), UI_LOADFILE_FLAGS, Path) == RTNORM)
      if (Load(Path) == GS_GOOD)
         InitCtrls();
}

void CHy_dia_cd_gb::OnBnClickedSave()
{
   C_STRING str, Path;

	if (get_GS_CURRENT_WRK_SESSION())
	{
		str = get_GS_CURRENT_WRK_SESSION()->get_pPrj()->get_dir();
      str += _T('\\');
      str += GEOSUPPORTDIR;
      str += _T('\\');
	}
   Path = _T('.');
   Path += FILE_EXT;
   if (gsc_get_tmp_filename(str.get_name(), _T("sim"), Path.get_name(), str) == GS_BAD)
      return;
   Path.clear();

   if (gsc_GetFileD(_T("GEOsim - Salva parametri di calcolo per rete gassa bassa pressione"),
                    str.get_name(), FILE_EXT.get_name(), UI_SAVEFILE_FLAGS, Path) == RTNORM)
      Save(Path);
}

void CHy_dia_cd_gb::OnBnClickedDefault()
{
   SetDefault();
   InitCtrls();
}
