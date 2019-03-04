// Hy_dia_cd.h : header file
//

#ifndef _hy_dia_cd_h
#define _hy_dia_cd_h 1

#ifndef _gs_hy_h
	#include "gs_hy.h"
#endif

#ifndef _hy_diamc_h
	#include "hy_diamc.h"
#endif

#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// CHy_dia_cd_h2o dialog

class CHy_dia_cd_h2o : public CDialog
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

	int	codice_diamC;
   int   iterazioni;
   double velocita_riferimento;

private:
	C_INT_STR_LIST list;

	CDiamC	diamC;

// Construction
public:
	CHy_dia_cd_h2o(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHy_dia_cd_h2o)
	enum { IDD = IDD_DIALOG_PARAMETERS_CD_H2O };
	CEdit	m_min_velocita_prv;
	CEdit	m_min_portata_prv;
	CEdit	m_velocita_riferimento;
	CEdit	m_convergenza_pompe;
	CEdit	m_iterazioni;
	CEdit	m_precisione_pompe;
	CEdit	m_coeff_molt_portate;
	CButton	m_conv_si;
	CComboBox	m_lista_diametri;
	CComboBox	m_tipologia;
	CEdit	m_controllo_portata;
	CEdit	m_controllo_carico;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHy_dia_cd_h2o)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   static C_STRING FILE_EXT;

	// Generated message map functions
	//{{AFX_MSG(CHy_dia_cd_h2o)
	afx_msg void OnChangeCarico();
	afx_msg void OnChangePortata();
	virtual BOOL OnInitDialog();
   void InitCtrls();
	afx_msg void OnSelchangeComboTipologia();
	afx_msg void OnSelchangeListaDc();
	afx_msg void OnConvNo();
	afx_msg void OnConvSi();
	afx_msg void OnChangeCoeffPortate();
	afx_msg void OnChangeControllo();
	afx_msg void OnChangeIteraz();
	afx_msg void OnChangeMinportata();
	afx_msg void OnChangeSmorzamento();
	afx_msg void OnChangeVelRif();
	afx_msg void OnChangeVelocita();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedHelp();

   void SetDefault(void);
   int Load(C_STRING &Path);
   int Save(C_STRING &Path);
   afx_msg void OnBnClickedLoad();
   afx_msg void OnBnClickedSave();
   afx_msg void OnBnClickedDefault();
};

/////////////////////////////////////////////////////////////////////////////
// CHy_dia_cd_gb dialog
class CHy_dia_cd_gb : public CDialog
{
public:
	double coeff_molt_portate;

   int tipologia;
	double controllo_carico;
	double controllo_portata;
   double densita_relativa;
			
   double min_velocita_prv;
   double min_portata_prv;

	int	codice_diamC;
   int   iterazioni;
   double velocita_riferimento;

private:
	C_INT_STR_LIST list;

	CDiamC	diamC;

// Construction
public:
	CHy_dia_cd_gb(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHy_dia_cd_gb)
	enum { IDD = IDD_DIALOG_PARAMETERS_CD_GB };
	CEdit	m_min_velocita_prv;
	CEdit	m_min_portata_prv;
	CEdit	m_velocita_riferimento;
	CEdit	m_iterazioni;
	CEdit	m_densita_relativa;
	CEdit	m_coeff_molt_portate;
	CEdit	m_controllo_portata;
	CComboBox	m_lista_diametri;
	CComboBox	m_tipologia;
	CEdit	m_controllo_carico;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHy_dia_cd_gb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   static C_STRING FILE_EXT;

	// Generated message map functions
	//{{AFX_MSG(CHy_dia_cd_gb)
	virtual BOOL OnInitDialog();
   void InitCtrls();
	afx_msg void OnChangeCarico();
	afx_msg void OnChangePortata();
	afx_msg void OnSelchangeComboTipologia();
	afx_msg void OnSelchangeListaDc();
	afx_msg void OnChangeCoeffPortate();
	afx_msg void OnChangeDensRelativa();
	afx_msg void OnChangeIteraz();
	afx_msg void OnChangeVelRif();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedHelp();
   afx_msg void OnEnChangeVelocita();
   afx_msg void OnEnChangeMinportata();

   void SetDefault(void);
   int Load(C_STRING &Path);
   int Save(C_STRING &Path);
   afx_msg void OnBnClickedOk();
   afx_msg void OnBnClickedLoad();
   afx_msg void OnBnClickedSave();
   afx_msg void OnBnClickedDefault();
};


/////////////////////////////////////////////////////////////////////////////
// CHy_dia_cd_gm dialog
class CHy_dia_cd_gm : public CDialog
{
public:
	double coeff_molt_portate;

   int tipologia;
	double controllo_carico;
	double controllo_portata;
   double densita_relativa;
			
   double min_velocita_prv;
   double min_portata_prv;

	int	codice_diamC;
   int   iterazioni;
   double velocita_riferimento;

private:
	C_INT_STR_LIST list;

	CDiamC	diamC;

// Construction
public:
	CHy_dia_cd_gm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHy_dia_cd_gm)
	enum { IDD = IDD_DIALOG_PARAMETERS_CD_GM };
	CEdit	m_min_velocita_prv;
	CEdit	m_min_portata_prv;
	CEdit	m_velocita_riferimento;
	CEdit	m_iterazioni;
	CEdit	m_densita_relativa;
	CEdit	m_coeff_molt_portate;
	CComboBox	m_lista_diametri;
	CComboBox	m_tipologia;
	CEdit	m_controllo_portata;
	CEdit	m_controllo_carico;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHy_dia_cd_gm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   static C_STRING FILE_EXT;

	// Generated message map functions
	//{{AFX_MSG(CHy_dia_cd_gm)
	virtual BOOL OnInitDialog();
   void InitCtrls();
	afx_msg void OnChangeCarico();
	afx_msg void OnChangePortata();
	afx_msg void OnSelchangeComboTipologia();
	afx_msg void OnSelchangeListaDc();
	afx_msg void OnChangeCoeffPortate();
	afx_msg void OnChangeDensRelativa();
	afx_msg void OnChangeIteraz();
	afx_msg void OnChangeVelRif();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedHelp();
   afx_msg void OnEnChangeVelocita();
   afx_msg void OnEnChangeMinportata();

   void SetDefault(void);
   int Load(C_STRING &Path);
   int Save(C_STRING &Path);
   afx_msg void OnBnClickedLoad();
   afx_msg void OnBnClickedSave();
   afx_msg void OnBnClickedDefault();
};

#endif
