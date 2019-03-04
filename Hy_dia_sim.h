// Hy_dia_sim.h : header file
//

#ifndef _hy_dia_sim_h
#define _hy_dia_sim_h 1

#ifndef _gs_hy_h
	#include "gs_hy.h"
#endif

#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// CHy_dia_h2o dialog

class CHy_dia_h2o : public CDialog
{
public:
	double coeff_molt_portate;

	int tipologia;
	double controllo_carico;
	double controllo_portata;

	int convergenza;
	double convergenza_pompe;
	double precisione_pompe;
			
	double min_velocita_prv;
	double min_portata_prv;

   bool   rete_tutta;
   bool   aggiorna_grafica;

// Construction
public:
	CHy_dia_h2o(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHy_dia_h2o)
	enum { IDD = IDD_DIALOG_PARAMETERS_H2O };
	CEdit	m_coeff_molt_portate;
	CEdit	m_convergenza_pompe;
	CEdit	m_precisione_pompe;
	CEdit	m_min_velocita_prv;
	CEdit	m_min_portata_prv;
	CButton	m_conv_si;
	CEdit	m_controllo_portata;
	CEdit	m_controllo_carico;
	CComboBox	m_tipologia;
	CString	m_tipo_controllo;
   CButton m_rete_tutta;
   CButton m_aggiorna_grafica;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHy_dia_h2o)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   static C_STRING FILE_EXT;

	// Generated message map functions
	//{{AFX_MSG(CHy_dia_h2o)
	afx_msg void OnSelchangeComboTipologia();
	virtual BOOL OnInitDialog();
   void InitCtrls();
	afx_msg void OnChangeCarico();
	afx_msg void OnChangePortata();
	afx_msg void OnConvNo();
	afx_msg void OnConvSi();
	afx_msg void OnChangeCoeffPortate();
	afx_msg void OnChangeMinportata();
	afx_msg void OnChangeSmorzamento();
	afx_msg void OnChangeVelocita();
	afx_msg void OnChangeControllo();
   afx_msg void OnBnClickedAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedHelp();
   afx_msg void OnBnClickedUpdGph();

   void SetDefault(void);
   int Load(C_STRING &Path);
   int Save(C_STRING &Path);
   afx_msg void OnBnClickedLoad();
   afx_msg void OnBnClickedSave();
   afx_msg void OnBnClickedDefault();
};


class CHy_dia_gb : public CDialog
{
public:
	int tipologia;
	double densita_relativa;
	double controllo_carico;
	double controllo_portata;
	double coeff_molt_portate;
			
	double min_velocita_prv;
	double min_portata_prv;

   bool   rete_tutta;
   bool   aggiorna_grafica;

// Construction
public:
	CHy_dia_gb(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHy_dia_gb)
	enum { IDD = IDD_DIALOG_PARAMETERS_GB };
	CEdit	m_min_velocita_prv;
	CEdit	m_min_portata_prv;
	CEdit	m_densita_relativa;
	CEdit	m_coeff_molt_portate;
	CEdit	m_controllo_portata;
	CComboBox m_tipologia;
	CEdit	m_controllo_carico;
   CButton m_rete_tutta;
   CButton m_aggiorna_grafica;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHy_dia_gb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   static C_STRING FILE_EXT;

	// Generated message map functions
	//{{AFX_MSG(CHy_dia_gb)
	afx_msg void OnChangeCarico();
	afx_msg void OnChangePortata();
	virtual BOOL OnInitDialog();
   void InitCtrls();
	afx_msg void OnSelchangeComboTipologia();
	afx_msg void OnChangeCoeffPortate();
	afx_msg void OnChangeDensRelativa();
   afx_msg void OnBnClickedAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedHelp();
   afx_msg void OnEnChangeVelocita();
   afx_msg void OnEnChangeMinportata();
   afx_msg void OnBnClickedUpdGph();

   void SetDefault(void);
   int Load(C_STRING &Path);
   int Save(C_STRING &Path);
   afx_msg void OnBnClickedLoad();
   afx_msg void OnBnClickedSave();
   afx_msg void OnBnClickedDefault();
};


class CHy_dia_gm : public CDialog
{
public:
	int tipologia;
	double densita_relativa;
	double controllo_carico;
	double controllo_portata;
	double coeff_molt_portate;
			
	double min_velocita_prv;
	double min_portata_prv;

   bool   rete_tutta;
   bool   aggiorna_grafica;

// Construction
public:
	CHy_dia_gm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHy_dia_gm)
	enum { IDD = IDD_DIALOG_PARAMETERS_GM };
	CEdit	m_min_velocita_prv;
	CEdit	m_min_portata_prv;
	CEdit	m_coeff_molt_portate;
	CEdit	m_densita_relativa;
	CComboBox	m_tipologia;
	CEdit	m_controllo_portata;
	CEdit	m_controllo_carico;
   CButton m_rete_tutta;
   CButton m_aggiorna_grafica;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHy_dia_gm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   static C_STRING FILE_EXT;

	// Generated message map functions
	//{{AFX_MSG(CHy_dia_gm)
	afx_msg void OnChangeCarico();
	afx_msg void OnChangePortata();
	virtual BOOL OnInitDialog();
   void InitCtrls();
	afx_msg void OnSelchangeComboTipologia();
	afx_msg void OnChangeCoeffPortate();
	afx_msg void OnChangeDensRelativa();
   afx_msg void OnBnClickedAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedHelp();
   afx_msg void OnEnChangeVelocita();
   afx_msg void OnEnChangeMinportata();
   afx_msg void OnBnClickedUpdGph();

   void SetDefault(void);
   int Load(C_STRING &Path);
   int Save(C_STRING &Path);
   afx_msg void OnBnClickedLoad();
   afx_msg void OnBnClickedSave();
   afx_msg void OnBnClickedDefault();
};

#endif
