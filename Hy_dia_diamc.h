// Hy_dia_diamc.h : header file
//

#ifndef _hy_dia_diamc_h
#define _hy_dia_diamc_h 1


#ifndef _hy_base_h
	#include "hy_base.h"
#endif

#include "resource.h"



#define MAX_LEN_NAMEDIAMCOMM     30 + 1

/////////////////////////////////////////////////////////////////////////////
// Hy_dia_diamc_list dialog

class CHy_dia_diamc_list : public CDialog
{
private:
	C_INT_STR_LIST	*diamC_list;
	C_DBCONNECTION	*pConn;

	int		code;
	CString	diamC;

	int		diamC_list_pos;

// Construction
public:
	CHy_dia_diamc_list(C_DBCONNECTION *p, C_INT_STR_LIST *list, CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CHy_dia_diamc_list)
	enum { IDD = IDD_DIALOG_DIAMC_LIST };
	CButton	m_modi;
	CButton	m_canc;
	CListBox	m_list_diamc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHy_dia_diamc_list)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHy_dia_diamc_list)
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnModi();
	afx_msg void OnCanc();
	afx_msg void OnSelchangeDiamc();
	afx_msg void OnDblClickedDiamc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int NewListDiamC(C_REAL_LIST *value_list);
	int ModiListDiamC(C_REAL_LIST *value_list, CString old);
	int CancListDiamC();

	int CancSetDiamC();
public:
   afx_msg void OnBnClickedHelp();
};




/////////////////////////////////////////////////////////////////////////////
// Hy_dia_diamc_set dialog

class CHy_dia_diamc_set : public CDialog
{
private:
	CString		value;
	C_REAL_LIST	value_list;

	C_INT_STR_LIST	*diamC_list;
	
	CString	*diamC_name;
	int		*diamC_code;

	C_DBCONNECTION	*pConn;

// Construction
public:
	CHy_dia_diamc_set(C_DBCONNECTION *p, C_INT_STR_LIST *list, int *code = NULL, CString *name = NULL, CWnd* pParent = NULL);   // standard constructor

	CString	Get_diamC_name();
	C_REAL_LIST* Get_value_list();

// Dialog Data
	//{{AFX_DATA(CHy_dia_diamc_set)
	enum { IDD = IDD_DIALOG_DIAMC_SET };
	CButton	m_ok;
	CEdit	m_value;
	CButton	m_update;
	CButton	m_insert;
	CButton	m_erase;
	CEdit	m_name;
	CListBox	m_diamc_set;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHy_dia_diamc_set)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHy_dia_diamc_set)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDiamcSet();
	afx_msg void OnInsert();
	afx_msg void OnUpdate();
	afx_msg void OnErase();
	afx_msg void OnChangeName();
	afx_msg void OnChangeValue();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedHelp();
};










// PAOLINO

/****************************************************************/
/* class C_DIAMCOMM												*/
/****************************************************************/
class C_DIAMCOMM : public C_REAL
{
	friend class C_LIST_DIAMCOMM;

public:
	C_DIAMCOMM() {}
	virtual ~C_DIAMCOMM() {}

	double get_diamc(void) {return get_key_double();}
	int set_diamc(double val) {return set_key(val);}
};


/****************************************************************/
/* class C_LIST_DIAMCOMM                                        */
/****************************************************************/
class C_LIST_DIAMCOMM : public C_REAL_LIST
{
	friend class C_DIAMCOMM;

	CString DiamL;

public :
	C_LIST_DIAMCOMM();
	virtual ~C_LIST_DIAMCOMM();

	int load_from_db(int cod, C_DBCONNECTION *pConn);
	int save_to_db(int cod, C_DBCONNECTION *pConn);
	int del_to_db(int cod, C_DBCONNECTION *pConn);

//	int fromMemory2Class(void);
};


/****************************************************************/
/* class C_SET_DIAMCOMM                                         */
/****************************************************************/
class C_SET_DIAMCOMM : public C_NODE
{
private :
	int	code;							// codice dell' elenco diametri commerciali.
	TCHAR name[MAX_LEN_NAMEDIAMCOMM];	// nome dell' elenco diametri commerciali.
	C_LIST_DIAMCOMM	list_diamc;			// elenco di diametri.

	CString	Diam;

public :
	C_SET_DIAMCOMM();
	virtual ~C_SET_DIAMCOMM();

	int      create(void);
	resbuf  *to_rb(void);
	int      from_rb(resbuf *rb);
	TCHAR   *get_name();
	
	int	set_name(const TCHAR *nname);

	int load_from_db(int cod, C_DBCONNECTION *pConn);
	int save_to_db(C_DBCONNECTION *pConn);
	int update_to_db(C_DBCONNECTION *pConn);
	int del_to_db(C_DBCONNECTION *pConn);

	int getLastCode(int *cod, C_DBCONNECTION *pConn);
   
	int fromMemory2Class(C_REAL_LIST *list);
    int fromClass2Memory(C_REAL_LIST *list);
   
    int write2file(CString path, int code, C_DBCONNECTION *pConn);

    C_LIST_DIAMCOMM *ptr_list_diamc(void);
};

#endif
