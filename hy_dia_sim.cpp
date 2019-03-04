// Hy_dia_sim.cpp : implementation file
//

#include "StdAfx.h"
#include "hy_dia_sim.h"


#include "hy_utily.h" // chiamata all'help html
#include "d2hMap.h" // doc to help


/////////////////////////////////////////////////////////////////////////////
// CHy_dia_h2o dialog


C_STRING CHy_dia_h2o::FILE_EXT = _T("H2O_calc_params"); // estensione dei file dei parametri di calcolo
CHy_dia_h2o::CHy_dia_h2o(CWnd* pParent /*=NULL*/)
	: CDialog(CHy_dia_h2o::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHy_dia_h2o)
	//}}AFX_DATA_INIT

	SetDefault();
}

/*****************************************************************************/
/*.doc CHy_dia_h2o::SetDefault                                               */
/*+
  Questa funzione setta i parametri di calcolo di default.
-*/  
/*****************************************************************************/
void CHy_dia_h2o::SetDefault()
{
	coeff_molt_portate = 1;

	tipologia = 0;
	controllo_carico = 0.50;;
	controllo_portata = 0.20;

	convergenza = 1;
	convergenza_pompe = 0.10; 
	precisione_pompe = 1;
			
	min_velocita_prv = 0.1;
	min_portata_prv = 0.50;

   rete_tutta       = false;
   aggiorna_grafica = true;
}

/*****************************************************************************/
/*.doc CHy_dia_h2o::Load                                                     */
/*+
  Questa funzione carica i parametri di calcolo da un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_h2o::Load(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;

   SezName = _T("H2O");

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

   if (gsc_get_profile(f, SezName.get_name(), _T("SIMULATE_ALL"), EntryValue, Unicode) == GS_GOOD)
   	if (EntryValue.comp(_T("0")) == 0) rete_tutta = false;
      else rete_tutta = true;

   if (gsc_get_profile(f, SezName.get_name(), _T("GRAPHIC_UPDATE"), EntryValue, Unicode) == GS_GOOD)
   	if (EntryValue.comp(_T("0")) == 0) aggiorna_grafica = false;
      else aggiorna_grafica = true;

   return gsc_close_profile(f);
}


/*****************************************************************************/
/*.doc CHy_dia_h2o::Save                                                     */
/*+
  Questa funzione salva i parametri di calcolo in un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_h2o::Save(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("H2O");

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

   EntryValue = (rete_tutta) ? _T("1") : _T("0");
   if (gsc_set_profile(f, SezName.get_name(), _T("SIMULATE_ALL"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = (aggiorna_grafica) ? _T("1") : _T("0");
   if (gsc_set_profile(f, SezName.get_name(), _T("GRAPHIC_UPDATE"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   return gsc_close_profile(f);
}


void CHy_dia_h2o::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHy_dia_h2o)
	DDX_Control(pDX, IDC_COEFF_PORTATE, m_coeff_molt_portate);
	DDX_Control(pDX, IDC_SMORZAMENTO, m_convergenza_pompe);
	DDX_Control(pDX, IDC_CONTROLLO, m_precisione_pompe);
	DDX_Control(pDX, IDC_VELOCITA, m_min_velocita_prv);
	DDX_Control(pDX, IDC_MINPORTATA, m_min_portata_prv);
	DDX_Control(pDX, IDC_CONV_SI, m_conv_si);
	DDX_Control(pDX, IDC_PORTATA, m_controllo_portata);
	DDX_Control(pDX, IDC_CARICO, m_controllo_carico);
	DDX_Control(pDX, IDC_COMBO_TIPOLOGIA, m_tipologia);
	DDX_Control(pDX, IDC_ALL, m_rete_tutta);
	DDX_Control(pDX, IDC_UPD_GPH, m_aggiorna_grafica);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHy_dia_h2o, CDialog)
	//{{AFX_MSG_MAP(CHy_dia_h2o)
	ON_CBN_SELCHANGE(IDC_COMBO_TIPOLOGIA, OnSelchangeComboTipologia)
	ON_EN_CHANGE(IDC_CARICO, OnChangeCarico)
	ON_EN_CHANGE(IDC_PORTATA, OnChangePortata)
	ON_BN_CLICKED(IDC_CONV_NO, OnConvNo)
	ON_BN_CLICKED(IDC_CONV_SI, OnConvSi)
	ON_EN_CHANGE(IDC_COEFF_PORTATE, OnChangeCoeffPortate)
	ON_EN_CHANGE(IDC_MINPORTATA, OnChangeMinportata)
	ON_EN_CHANGE(IDC_SMORZAMENTO, OnChangeSmorzamento)
	ON_EN_CHANGE(IDC_VELOCITA, OnChangeVelocita)
	ON_EN_CHANGE(IDC_CONTROLLO, OnChangeControllo)
   ON_BN_CLICKED(IDC_ALL, OnBnClickedAll)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
   ON_BN_CLICKED(IDC_UPD_GPH, &CHy_dia_h2o::OnBnClickedUpdGph)
   ON_BN_CLICKED(IDLOAD, &CHy_dia_h2o::OnBnClickedLoad)
   ON_BN_CLICKED(IDSAVE, &CHy_dia_h2o::OnBnClickedSave)
   ON_BN_CLICKED(IDDEFAULT, &CHy_dia_h2o::OnBnClickedDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHy_dia_h2o message handlers

BOOL CHy_dia_h2o::OnInitDialog() 
{
	CDialog::OnInitDialog();

   InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHy_dia_h2o::InitCtrls()
{
   C_STRING StrValue;

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

   m_rete_tutta.SetCheck(rete_tutta);
   m_aggiorna_grafica.SetCheck(aggiorna_grafica);
}

void CHy_dia_h2o::OnSelchangeComboTipologia() 
{
	tipologia = m_tipologia.GetCurSel();
   InitCtrls();
}

void CHy_dia_h2o::OnChangeCarico() 
{
	CString carico;
	
	m_controllo_carico.GetWindowText(carico);

   if (gsc_is_numeric(carico) == GS_GOOD)
	   controllo_carico = _wtof(carico);
   else
	   m_controllo_carico.SetWindowText(_T(""));
}

void CHy_dia_h2o::OnChangePortata() 
{
	CString portata;
	
	m_controllo_portata.GetWindowText(portata);

   if (gsc_is_numeric(portata) == GS_GOOD)
	   controllo_portata = _wtof(portata);
   else
	   m_controllo_portata.SetWindowText(_T(""));
}

void CHy_dia_h2o::OnConvNo() 
{
	convergenza = 0;	
}

void CHy_dia_h2o::OnConvSi() 
{
	convergenza = 1;	
}

void CHy_dia_h2o::OnChangeCoeffPortate() 
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

void CHy_dia_h2o::OnChangeMinportata() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CString valore;
	
	m_min_portata_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
	   min_portata_prv = _wtof(valore);
   else
	   m_min_portata_prv.SetWindowText(_T(""));
}

void CHy_dia_h2o::OnChangeSmorzamento() 
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

void CHy_dia_h2o::OnChangeVelocita() 
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

void CHy_dia_h2o::OnChangeControllo() 
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

void CHy_dia_h2o::OnBnClickedLoad()
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

void CHy_dia_h2o::OnBnClickedSave()
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

void CHy_dia_h2o::OnBnClickedDefault()
{
   SetDefault();
   InitCtrls();
}


/////////////////////////////////////////////////////////////////////////////
// CHy_dia_gb dialog


C_STRING CHy_dia_gb::FILE_EXT = _T("LPG_calc_params"); // estensione dei file dei parametri di calcolo
CHy_dia_gb::CHy_dia_gb(CWnd* pParent /*=NULL*/)
	: CDialog(CHy_dia_gb::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHy_dia_gb)
	//}}AFX_DATA_INIT

   SetDefault();
}

/*****************************************************************************/
/*.doc CHy_dia_gb::SetDefault                                                */
/*+
  Questa funzione setta i parametri di calcolo di default.
-*/  
/*****************************************************************************/
void CHy_dia_gb::SetDefault()
{
	coeff_molt_portate = 1;
   densita_relativa = 0.554;
   tipologia = 0;
	controllo_carico = 0.5;
	controllo_portata = 0.2;
		
	min_velocita_prv = 0.1;
	min_portata_prv = 0.50;

   rete_tutta       = false;
   aggiorna_grafica = true;
}

/*****************************************************************************/
/*.doc CHy_dia_gb::Load                                                      */
/*+
  Questa funzione carica i parametri di calcolo da un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_gb::Load(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("LOW PRESSURE GAS");

	if ((f = gsc_open_profile(Path, READONLY, MORETESTS, &Unicode)) == NULL) return GS_BAD;

   if (gsc_get_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue, Unicode) == GS_GOOD)
   	coeff_molt_portate = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("RELATIVE_DENSITY"), EntryValue, Unicode) == GS_GOOD)
   	densita_relativa = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("CONTROL_TYPE"), EntryValue, Unicode) == GS_GOOD)
   	tipologia = EntryValue.toi();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	controllo_carico = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_FLOW"), EntryValue, Unicode) == GS_GOOD)
   	controllo_portata = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue, Unicode) == GS_GOOD)
   	min_velocita_prv = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	min_portata_prv = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("SIMULATE_ALL"), EntryValue, Unicode) == GS_GOOD)
   	if (EntryValue.comp(_T("0")) == 0) rete_tutta = false;
      else rete_tutta = true;

   if (gsc_get_profile(f, SezName.get_name(), _T("GRAPHIC_UPDATE"), EntryValue, Unicode) == GS_GOOD)
   	if (EntryValue.comp(_T("0")) == 0) aggiorna_grafica = false;
      else aggiorna_grafica = true;

   return gsc_close_profile(f);
}


/*****************************************************************************/
/*.doc CHy_dia_gb::Save                                                      */
/*+
  Questa funzione salva i parametri di calcolo in un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_gb::Save(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("LOW PRESSURE GAS");

   if ((f = gsc_open_profile(Path, UPDATEABLE, MORETESTS, &Unicode)) == NULL) return GS_BAD;
   
   EntryValue = coeff_molt_portate;
   if (gsc_set_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = densita_relativa;
   if (gsc_set_profile(f, SezName.get_name(), _T("RELATIVE_DENSITY"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
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

   EntryValue = min_velocita_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = min_portata_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = (rete_tutta) ? _T("1") : _T("0");
   if (gsc_set_profile(f, SezName.get_name(), _T("SIMULATE_ALL"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = (aggiorna_grafica) ? _T("1") : _T("0");
   if (gsc_set_profile(f, SezName.get_name(), _T("GRAPHIC_UPDATE"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   return gsc_close_profile(f);
}


void CHy_dia_gb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHy_dia_gb)
	DDX_Control(pDX, IDC_DENS_RELATIVA, m_densita_relativa);
	DDX_Control(pDX, IDC_COEFF_PORTATE, m_coeff_molt_portate);
	DDX_Control(pDX, IDC_PORTATA, m_controllo_portata);
	DDX_Control(pDX, IDC_COMBO_TIPOLOGIA, m_tipologia);
	DDX_Control(pDX, IDC_CARICO, m_controllo_carico);
	DDX_Control(pDX, IDC_ALL, m_rete_tutta);
	DDX_Control(pDX, IDC_UPD_GPH, m_aggiorna_grafica);
	DDX_Control(pDX, IDC_VELOCITA, m_min_velocita_prv);
	DDX_Control(pDX, IDC_MINPORTATA, m_min_portata_prv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHy_dia_gb, CDialog)
	//{{AFX_MSG_MAP(CHy_dia_gb)
	ON_EN_CHANGE(IDC_CARICO, OnChangeCarico)
	ON_EN_CHANGE(IDC_PORTATA, OnChangePortata)
	ON_CBN_SELCHANGE(IDC_COMBO_TIPOLOGIA, OnSelchangeComboTipologia)
	ON_EN_CHANGE(IDC_COEFF_PORTATE, OnChangeCoeffPortate)
	ON_EN_CHANGE(IDC_DENS_RELATIVA, OnChangeDensRelativa)
   ON_BN_CLICKED(IDC_ALL, OnBnClickedAll)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
   ON_EN_CHANGE(IDC_VELOCITA, OnEnChangeVelocita)
   ON_EN_CHANGE(IDC_MINPORTATA, OnEnChangeMinportata)
   ON_BN_CLICKED(IDC_UPD_GPH, &CHy_dia_gb::OnBnClickedUpdGph)
   ON_BN_CLICKED(IDLOAD, &CHy_dia_gb::OnBnClickedLoad)
   ON_BN_CLICKED(IDSAVE, &CHy_dia_gb::OnBnClickedSave)
   ON_BN_CLICKED(IDDEFAULT, &CHy_dia_gb::OnBnClickedDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHy_dia_gb message handlers

void CHy_dia_gb::OnChangeCarico() 
{
	CString carico;
	
	m_controllo_carico.GetWindowText(carico);

   if (gsc_is_numeric(carico) == GS_GOOD)
	   controllo_carico = _wtof(carico);
   else
	   m_controllo_carico.SetWindowText(_T(""));
}

void CHy_dia_gb::OnChangePortata() 
{
	CString portata;
	
	m_controllo_portata.GetWindowText(portata);

   if (gsc_is_numeric(portata) == GS_GOOD)
	   controllo_portata = _wtof(portata);
   else
	   m_controllo_portata.SetWindowText(_T(""));
}

BOOL CHy_dia_gb::OnInitDialog() 
{
   CDialog::OnInitDialog();
	
   InitCtrls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHy_dia_gb::InitCtrls()
{
   C_STRING StrValue;

   // Delete every item from the combo box m_tipologia
   for (int i = m_tipologia.GetCount() - 1; i >= 0; i--)
      m_tipologia.DeleteString(i);
	
   m_tipologia.AddString(_T("carico"));
	m_tipologia.AddString(_T("portata"));
	m_tipologia.AddString(_T("carico e portata"));
	m_tipologia.AddString(_T("carico o portata"));
	m_tipologia.SetCurSel(tipologia);

   StrValue = coeff_molt_portate;
	m_coeff_molt_portate.SetWindowText(StrValue.get_name());

   StrValue = densita_relativa;
   m_densita_relativa.SetWindowText(StrValue.get_name());
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

   StrValue = min_velocita_prv;
	m_min_velocita_prv.SetWindowText(StrValue.get_name());
	StrValue = min_portata_prv;
	m_min_portata_prv.SetWindowText(StrValue.get_name());

   m_rete_tutta.SetCheck(rete_tutta);
   m_aggiorna_grafica.SetCheck(aggiorna_grafica);
}

void CHy_dia_gb::OnSelchangeComboTipologia() 
{
	tipologia = m_tipologia.GetCurSel();
   InitCtrls();
}

void CHy_dia_gb::OnChangeCoeffPortate() 
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

void CHy_dia_gb::OnChangeDensRelativa() 
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

void CHy_dia_gb::OnBnClickedLoad()
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

void CHy_dia_gb::OnBnClickedSave()
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

   if (gsc_GetFileD(_T("GEOsim - Salva parametri di calcolo per rete gas bassa pressione"),
                    str.get_name(), FILE_EXT.get_name(), UI_SAVEFILE_FLAGS, Path) == RTNORM)
      Save(Path);
}

void CHy_dia_gb::OnBnClickedDefault()
{
   SetDefault();
   InitCtrls();
}


/////////////////////////////////////////////////////////////////////////////
// CHy_dia_gm dialog


C_STRING CHy_dia_gm::FILE_EXT = _T("MPG_calc_params"); // estensione dei file dei parametri di calcolo
CHy_dia_gm::CHy_dia_gm(CWnd* pParent /*=NULL*/)
	: CDialog(CHy_dia_gm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHy_dia_gm)
	//}}AFX_DATA_INIT

	SetDefault();
}

/*****************************************************************************/
/*.doc CHy_dia_gm::SetDefault                                                */
/*+
  Questa funzione setta i parametri di calcolo di default.
-*/  
/*****************************************************************************/
void CHy_dia_gm::SetDefault()
{
	coeff_molt_portate = 1;
   tipologia = 0;
   densita_relativa = 0.544;
	controllo_carico = 0.5;
	controllo_portata = 0.2;
			
	min_velocita_prv = 0.1;
	min_portata_prv = 0.50;

   rete_tutta       = false;
   aggiorna_grafica = true;
}

/*****************************************************************************/
/*.doc CHy_dia_gm::Load                                                      */
/*+
  Questa funzione carica i parametri di calcolo da un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_gm::Load(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("MEDIUM PRESSURE GAS");

	if ((f = gsc_open_profile(Path, READONLY, MORETESTS, &Unicode)) == NULL) return GS_BAD;

   if (gsc_get_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue, Unicode) == GS_GOOD)
   	coeff_molt_portate = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("RELATIVE_DENSITY"), EntryValue, Unicode) == GS_GOOD)
   	densita_relativa = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("CONTROL_TYPE"), EntryValue, Unicode) == GS_GOOD)
   	tipologia = EntryValue.toi();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	controllo_carico = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("TOLERANCE_ON_HYDRAULIC_FLOW"), EntryValue, Unicode) == GS_GOOD)
   	controllo_portata = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue, Unicode) == GS_GOOD)
   	min_velocita_prv = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue, Unicode) == GS_GOOD)
   	min_portata_prv = EntryValue.tof();

   if (gsc_get_profile(f, SezName.get_name(), _T("SIMULATE_ALL"), EntryValue, Unicode) == GS_GOOD)
   	if (EntryValue.comp(_T("0")) == 0) rete_tutta = false;
      else rete_tutta = true;

   if (gsc_get_profile(f, SezName.get_name(), _T("GRAPHIC_UPDATE"), EntryValue, Unicode) == GS_GOOD)
   	if (EntryValue.comp(_T("0")) == 0) aggiorna_grafica = false;
      else aggiorna_grafica = true;

   return gsc_close_profile(f);
}


/*****************************************************************************/
/*.doc CHy_dia_gm::Save                                                      */
/*+
  Questa funzione salva i parametri di calcolo in un file.
  Parametri:
  C_STRING &Path;

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/*****************************************************************************/
int CHy_dia_gm::Save(C_STRING &Path)
{
   C_STRING SezName, EntryValue;
   FILE     *f;
   bool     Unicode = false;
   
   SezName = _T("MEDIUM PRESSURE GAS");
   
   if ((f = gsc_open_profile(Path, UPDATEABLE, MORETESTS, &Unicode)) == NULL) return GS_BAD;

   EntryValue = coeff_molt_portate;
   if (gsc_set_profile(f, SezName.get_name(), _T("HYDRAULIC_FLOW_COEFF"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = densita_relativa;
   if (gsc_set_profile(f, SezName.get_name(), _T("RELATIVE_DENSITY"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
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

   EntryValue = min_velocita_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_SPEED"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = min_portata_prv;
   if (gsc_set_profile(f, SezName.get_name(), _T("MIN_PRV_HYDRAULIC_HEAD"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = (rete_tutta) ? _T("1") : _T("0");
   if (gsc_set_profile(f, SezName.get_name(), _T("SIMULATE_ALL"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   EntryValue = (aggiorna_grafica) ? _T("1") : _T("0");
   if (gsc_set_profile(f, SezName.get_name(), _T("GRAPHIC_UPDATE"), EntryValue.get_name(), 0, Unicode) != GS_GOOD)
      return GS_BAD;

   return gsc_close_profile(f);
}


void CHy_dia_gm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHy_dia_gm)
	DDX_Control(pDX, IDC_COEFF_PORTATE, m_coeff_molt_portate);
	DDX_Control(pDX, IDC_DENS_RELATIVA, m_densita_relativa);
	DDX_Control(pDX, IDC_COMBO_TIPOLOGIA, m_tipologia);
	DDX_Control(pDX, IDC_PORTATA, m_controllo_portata);
	DDX_Control(pDX, IDC_CARICO, m_controllo_carico);
	DDX_Control(pDX, IDC_ALL, m_rete_tutta);
	DDX_Control(pDX, IDC_UPD_GPH, m_aggiorna_grafica);
	DDX_Control(pDX, IDC_VELOCITA, m_min_velocita_prv);
	DDX_Control(pDX, IDC_MINPORTATA, m_min_portata_prv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHy_dia_gm, CDialog)
	//{{AFX_MSG_MAP(CHy_dia_gm)
	ON_EN_CHANGE(IDC_CARICO, OnChangeCarico)
	ON_EN_CHANGE(IDC_PORTATA, OnChangePortata)
	ON_CBN_SELCHANGE(IDC_COMBO_TIPOLOGIA, OnSelchangeComboTipologia)
	ON_EN_CHANGE(IDC_COEFF_PORTATE, OnChangeCoeffPortate)
	ON_EN_CHANGE(IDC_DENS_RELATIVA, OnChangeDensRelativa)
   ON_BN_CLICKED(IDC_ALL, OnBnClickedAll)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
   ON_EN_CHANGE(IDC_VELOCITA, OnEnChangeVelocita)
   ON_EN_CHANGE(IDC_MINPORTATA, OnEnChangeMinportata)
   ON_BN_CLICKED(IDC_UPD_GPH, &CHy_dia_gm::OnBnClickedUpdGph)
   ON_BN_CLICKED(IDLOAD, &CHy_dia_gm::OnBnClickedLoad)
   ON_BN_CLICKED(IDSAVE, &CHy_dia_gm::OnBnClickedSave)
   ON_BN_CLICKED(IDDEFAULT, &CHy_dia_gm::OnBnClickedDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHy_dia_gm message handlers

void CHy_dia_gm::OnChangeCarico() 
{
	CString carico;
	
	m_controllo_carico.GetWindowText(carico);

   if (gsc_is_numeric(carico) == GS_GOOD)
 	   controllo_carico = _wtof(carico);
   else
	   m_controllo_carico.SetWindowText(_T(""));
}

void CHy_dia_gm::OnChangePortata() 
{
	CString portata;
	
	m_controllo_portata.GetWindowText(portata);

   if (gsc_is_numeric(portata) == GS_GOOD)
   	controllo_portata = _wtof(portata);
   else
	   m_controllo_portata.SetWindowText(_T(""));
}

BOOL CHy_dia_gm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHy_dia_gm::InitCtrls()
{
   C_STRING StrValue;

   // Delete every item from the combo box m_tipologia
   for (int i = m_tipologia.GetCount() - 1; i >= 0; i--)
      m_tipologia.DeleteString(i);
	
	m_tipologia.AddString(_T("carico"));
	m_tipologia.AddString(_T("portata"));
	m_tipologia.AddString(_T("carico e portata"));
	m_tipologia.AddString(_T("carico o portata"));
	m_tipologia.SetCurSel(tipologia);

   StrValue = coeff_molt_portate;
	m_coeff_molt_portate.SetWindowText(StrValue.get_name());

   StrValue = densita_relativa;
   m_densita_relativa.SetWindowText(StrValue.get_name());
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

   StrValue = min_velocita_prv;
	m_min_velocita_prv.SetWindowText(StrValue.get_name());
	StrValue = min_portata_prv;
	m_min_portata_prv.SetWindowText(StrValue.get_name());

   m_rete_tutta.SetCheck(rete_tutta);
   m_aggiorna_grafica.SetCheck(aggiorna_grafica);
}

void CHy_dia_gm::OnSelchangeComboTipologia() 
{
	tipologia = m_tipologia.GetCurSel();
   InitCtrls();
}

void CHy_dia_gm::OnChangeCoeffPortate() 
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

void CHy_dia_gm::OnChangeDensRelativa() 
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

void CHy_dia_gm::OnBnClickedLoad()
{
   C_STRING str, Path;

	if (get_GS_CURRENT_WRK_SESSION())
	{
		str = get_GS_CURRENT_WRK_SESSION()->get_pPrj()->get_dir();
      str += _T('\\');
      str += GEOSUPPORTDIR;
	}

   if (gsc_GetFileD(_T("GEOsim - Carica parametri di calcolo per rete media pressione"),
                    str.get_name(), FILE_EXT.get_name(), UI_LOADFILE_FLAGS, Path) == RTNORM)
      if (Load(Path) == GS_GOOD)
         InitCtrls();
}

void CHy_dia_gm::OnBnClickedSave()
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

   if (gsc_GetFileD(_T("GEOsim - Salva parametri di calcolo per rete media pressione"),
                    str.get_name(), FILE_EXT.get_name(), UI_SAVEFILE_FLAGS, Path) == RTNORM)
      Save(Path);
}

void CHy_dia_gm::OnBnClickedDefault()
{
   SetDefault();
   InitCtrls();
}

void CHy_dia_gb::OnBnClickedAll()
{
   rete_tutta = (m_rete_tutta.GetCheck() == 0) ? false : true;
}

void CHy_dia_gm::OnBnClickedAll()
{
   rete_tutta = (m_rete_tutta.GetCheck() == 0) ? false : true;
}

void CHy_dia_h2o::OnBnClickedAll()
{
   rete_tutta = (m_rete_tutta.GetCheck() == 0) ? false : true;
}

void CHy_dia_gm::OnBnClickedUpdGph()
{
   aggiorna_grafica = (m_aggiorna_grafica.GetCheck() == BST_CHECKED) ? true : false;
}

void CHy_dia_gb::OnBnClickedUpdGph()
{
   aggiorna_grafica = (m_aggiorna_grafica.GetCheck() == BST_CHECKED) ? true : false;
}

void CHy_dia_h2o::OnBnClickedUpdGph()
{
   aggiorna_grafica = (m_aggiorna_grafica.GetCheck() == BST_CHECKED) ? true : false;
}

void CHy_dia_gb::OnBnClickedHelp()
{
   CallHelp(m_hWnd, IDH_Simulazioneretegas);
}

void CHy_dia_gm::OnBnClickedHelp()
{
   CallHelp(m_hWnd, IDH_Simulazioneretegas);
}

void CHy_dia_h2o::OnBnClickedHelp()
{
   CallHelp(m_hWnd, IDH_Simulazionereteacqua);
}

void CHy_dia_gb::OnEnChangeVelocita()
{
	CString valore;
	
	m_min_velocita_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
   	min_velocita_prv = _wtof(valore);
   else
	   m_min_velocita_prv.SetWindowText(_T(""));
}

void CHy_dia_gm::OnEnChangeVelocita()
{
	CString valore;
	
	m_min_velocita_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
   	min_velocita_prv = _wtof(valore);
   else
	   m_min_velocita_prv.SetWindowText(_T(""));
}

void CHy_dia_gb::OnEnChangeMinportata()
{
	CString valore;
	
	m_min_portata_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
	   min_portata_prv = _wtof(valore);
   else
	   m_min_portata_prv.SetWindowText(_T(""));
}

void CHy_dia_gm::OnEnChangeMinportata()
{
	CString valore;
	
	m_min_portata_prv.GetWindowText(valore);

   if (gsc_is_numeric(valore) == GS_GOOD)
	   min_portata_prv = _wtof(valore);
   else
	   m_min_portata_prv.SetWindowText(_T(""));
}
