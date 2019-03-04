#ifndef _hy_sceltasim_h
#define _hy_sceltasim_h 1

/////////////////////////////////////////////////////////////////////////////
// Csceltasim dialog

class Csceltasim : public CDialog
{
// Construction
public:
   
   int   pos;
   C_INT_INT_STR_LIST *list;

	Csceltasim(C_INT_INT_STR_LIST *l, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Csceltasim)
	enum { IDD = IDD_DIALOG_SCELTASIMULAZIONI };
	CListBox	m_ListaSim;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Csceltasim)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Csceltasim)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkListaSimulazioni();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 
