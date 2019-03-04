// Hy_dia_diamc.cpp : implementation file
//

#include "StdAfx.h"
#include "hy_dia_diamc.h"

#include "hy_utily.h" // chiamata all'help html
#include "d2hMap.h" // doc to help

//extern "C" HWND adsw_acadMainWnd();

/////////////////////////////////////////////////////////////////////////////
// CHy_dia_diamc_list dialog

CHy_dia_diamc_list::CHy_dia_diamc_list(C_DBCONNECTION *p, C_INT_STR_LIST *list, CWnd* pParent /*=NULL*/)
	: CDialog(CHy_dia_diamc_list::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHy_dia_diamc_list)
	//}}AFX_DATA_INIT

	diamC_list = list;
	pConn = p;
}


void CHy_dia_diamc_list::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHy_dia_diamc_list)
	DDX_Control(pDX, IDC_MODI, m_modi);
	DDX_Control(pDX, IDC_CANC, m_canc);
	DDX_Control(pDX, IDC_DIAMC, m_list_diamc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHy_dia_diamc_list, CDialog)
	//{{AFX_MSG_MAP(CHy_dia_diamc_list)
	ON_BN_CLICKED(IDC_CANC, OnCanc)
	ON_BN_CLICKED(IDC_MODI, OnModi)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_LBN_SELCHANGE(IDC_DIAMC, OnSelchangeDiamc)
   ON_LBN_DBLCLK(IDC_DIAMC, OnDblClickedDiamc)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_DIAMHELP, OnBnClickedHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Hy_dia_diamc_list message handlers

BOOL CHy_dia_diamc_list::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	C_INT_STR  *punt = NULL;

	if (diamC_list != NULL)
	{
		punt = (C_INT_STR *) diamC_list->get_head();
		while (punt != NULL)
		{ 
			m_list_diamc.AddString(punt->get_name());
			punt = (C_INT_STR *)punt->get_next();
		}
	}
	
	m_modi.EnableWindow(FALSE);
	m_canc.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CHy_dia_diamc_list::OnNew() 
{
	// TODO: Add your control notification handler code here
	CHy_dia_diamc_set	dia(pConn, diamC_list);

	if (dia.DoModal() == IDOK)
	{
		diamC = dia.Get_diamC_name();

		NewListDiamC(dia.Get_value_list());
	}
}


void CHy_dia_diamc_list::OnModi() 
{
	// TODO: Add your control notification handler code here
	CString old;

	old = diamC;

	CHy_dia_diamc_set	dia(pConn, diamC_list, &code, &diamC);
	
	if (dia.DoModal() == IDOK)
		ModiListDiamC(dia.Get_value_list(), old);
		
	m_list_diamc.SetCurSel(-1);
	
	m_modi.EnableWindow(FALSE);
	m_canc.EnableWindow(FALSE);
}


void CHy_dia_diamc_list::OnCanc() 
{
	// TODO: Add your control notification handler code here
	CString		msg, temp;
	CString		cpt;

	cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
	msg.LoadString(HY_STRING200); // "Cancellazione dell'elenco "
	msg += diamC;
	temp.LoadString(HY_STRING203); // "."
	msg += temp;

	if (CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		CancListDiamC();

	m_list_diamc.SetCurSel(-1);
	
	m_modi.EnableWindow(FALSE);
	m_canc.EnableWindow(FALSE);
}


int CHy_dia_diamc_list::NewListDiamC(C_REAL_LIST *value_list) 
{
	int ret = GS_GOOD;

	C_SET_DIAMCOMM    listd;

	C_INT_STR	*punt;

	TRY
	{
		// numero di codice per il nuovo elenco
		listd.getLastCode(&code, pConn);

		listd.set_name(diamC);
	
		// dalla memoria alla classe per poi salvare su db
		listd.fromMemory2Class(value_list);

		// salvo il set e rispettive relazioni
		listd.save_to_db(pConn);
		
 		punt = new C_INT_STR();
		punt->set_key(code);
		punt->set_name(diamC);
		diamC_list->add_tail(punt);

		m_list_diamc.InsertString(m_list_diamc.GetCount(), diamC);
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


int CHy_dia_diamc_list::ModiListDiamC(C_REAL_LIST *value_list, CString old) 
{
	int ret = GS_GOOD;

	C_SET_DIAMCOMM    listd;

	TRY
	{
		// Carico l' elenco in memoria
		listd.load_from_db(code, pConn);

		if (diamC != old)
		{
			listd.set_name(diamC);
			
			diamC_list->getptr_at(diamC_list_pos)->set_name(diamC);
			
			m_list_diamc.DeleteString(m_list_diamc.GetCurSel());
			m_list_diamc.InsertString(m_list_diamc.GetCurSel(), diamC);
		}

		// dalla memoria alla classe per poi salvare su db
		listd.fromMemory2Class(value_list);

		// salvo il set e rispettive relazioni
		listd.save_to_db(pConn);
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


int CHy_dia_diamc_list::CancListDiamC() 
{
	int ret = GS_GOOD;

	TRY
	{
		if (CancSetDiamC() == GS_BAD)
			AfxThrowUserException( );

		if (diamC_list->remove_at(diamC_list_pos) == GS_BAD)
			AfxThrowUserException( );

		m_list_diamc.DeleteString(m_list_diamc.GetCurSel());
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH

	return ret;
}


int CHy_dia_diamc_list::CancSetDiamC() 
{
	int ret = GS_GOOD;

	C_SET_DIAMCOMM    listd;

	TRY
	{
		// Carico l' elenco in memoria
		if ((listd.load_from_db(code, pConn)) == GS_BAD)
			AfxThrowUserException( );

		// Cancello dai data base e rispettiva lista di diam
		if ((listd.del_to_db(pConn)) == GS_BAD)
			AfxThrowUserException( );
	}
	CATCH( CUserException, e )
	{
		ret = GS_BAD;
	}
	END_CATCH
		
	return ret;
}


void CHy_dia_diamc_list::OnSelchangeDiamc() 
{
	C_INT_STR	*punt;
	
	if (m_list_diamc.GetCurSel() == -1) return; // nessuna selezione
	m_list_diamc.GetText(m_list_diamc.GetCurSel(), diamC);
	
	if (diamC_list != NULL)
	{
		diamC_list_pos = 0;

		punt = (C_INT_STR *) diamC_list->get_head();
		while (punt != NULL)
		{ 
			diamC_list_pos++;
			if (gsc_strcmp(punt->get_name(), diamC) == 0)
			{
				code = punt->get_key();
				break;
			}
			
			punt = (C_INT_STR *)punt->get_next();
		}
	}

	m_modi.EnableWindow(TRUE);
	m_canc.EnableWindow(TRUE);
}

void CHy_dia_diamc_list::OnDblClickedDiamc()
{
   OnSelchangeDiamc();
   OnModi();
}


/////////////////////////////////////////////////////////////////////////////
// Hy_dia_diamc_set dialog

CHy_dia_diamc_set::CHy_dia_diamc_set(C_DBCONNECTION *p, C_INT_STR_LIST *list, int *code, CString *name, CWnd* pParent /*=NULL*/)
	: CDialog(CHy_dia_diamc_set::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHy_dia_diamc_set)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	pConn = p;
	diamC_list = list;
	
	if ((code != NULL) && (name != NULL))
	{
		diamC_code = code;
		diamC_name = name;
	}
	else
	{
		diamC_code = NULL;
		diamC_name = NULL;
	}
}


void CHy_dia_diamc_set::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHy_dia_diamc_set)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_VALUE, m_value);
	DDX_Control(pDX, IDC_UPDATE, m_update);
	DDX_Control(pDX, IDC_INSERT, m_insert);
	DDX_Control(pDX, IDC_ERASE, m_erase);
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_DIAMC_SET, m_diamc_set);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHy_dia_diamc_set, CDialog)
	//{{AFX_MSG_MAP(CHy_dia_diamc_set)
	ON_LBN_SELCHANGE(IDC_DIAMC_SET, OnSelchangeDiamcSet)
	ON_BN_CLICKED(IDC_ERASE, OnErase)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_VALUE, OnChangeValue)
	//}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_DIAMHELP, OnBnClickedHelp)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Hy_dia_diamc_set message handlers

BOOL CHy_dia_diamc_set::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	C_SET_DIAMCOMM	listd;

	if ((diamC_code != NULL) && (diamC_name != NULL))
	{
		m_name.SetWindowText(*diamC_name);

		m_erase.EnableWindow(FALSE);
		m_update.EnableWindow(FALSE);

		// Carico l' elenco in memoria
		if ((listd.load_from_db(*diamC_code, pConn)) == GS_BAD) 
			return FALSE; 

		if ((listd.fromClass2Memory(&value_list)) == GS_BAD) return FALSE; 

		C_REAL  *punt = NULL;

		punt = (C_REAL *) value_list.get_head();
		while (punt != NULL)
		{ 
			m_diamc_set.AddString(gsc_tostring(punt->get_key_double()));
			punt = (C_REAL *)punt->get_next();
		}
	}
	else
	{
		m_diamc_set.EnableWindow(FALSE);
		m_value.EnableWindow(FALSE);
	}

	// m_insert.EnableWindow(FALSE); roby
	m_erase.EnableWindow(FALSE);
	m_update.EnableWindow(FALSE);
	if (diamC_name == NULL)
		m_ok.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


CString CHy_dia_diamc_set::Get_diamC_name()
{
	return *diamC_name;
}


C_REAL_LIST *CHy_dia_diamc_set::Get_value_list()
{
	return &value_list;
}


void CHy_dia_diamc_set::OnSelchangeDiamcSet() 
{
	if (m_diamc_set.GetCurSel() == -1) return; // nessuna selezione

	m_diamc_set.GetText(m_diamc_set.GetCurSel(), value);
	m_value.SetWindowText(value);
	
	m_erase.EnableWindow(TRUE);
}


void CHy_dia_diamc_set::OnErase() 
{
	// TODO: Add your control notification handler code here
	int i;
	
	i = m_diamc_set.GetCurSel();
	value_list.remove_at(i+1);
	m_diamc_set.DeleteString(i);

	value = _T("");
	m_value.SetWindowText(value);

	m_erase.EnableWindow(FALSE);
	//m_insert.EnableWindow(FALSE); roby
	m_update.EnableWindow(FALSE);
}


void CHy_dia_diamc_set::OnInsert() 
{
	// TODO: Add your control notification handler code here
	int  j;
	bool ins;

	C_REAL	*tmp;

	CString val;
	C_STRING temp;

	CString		msg;
	CString		cpt;

	m_value.GetWindowText(val);
	temp = val;

	if ((val == _T("")) || (gsc_is_numeric(temp.get_name()) == GS_BAD))
	{
		cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
		msg.LoadString(HY_STRING231); // "Valore non valido."
		
		CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OK | MB_ICONQUESTION);
	}
	else
	{
		// Verifico che il nuovo valore non esista già
		if (value_list.search_key(_wtof(val)) != NULL) 
		{
			cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
			msg.LoadString(HY_STRING204); // "Valore già presente."
			
			CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OK | MB_ICONQUESTION);
		}
		else
		{
			// Inserisco il valore nella lista
			value_list.insert_ordered_double(_wtof(val), &ins);
				
			if (ins == true)
			{
            tmp = (C_REAL *) value_list.get_cursor();
				j = value_list.getpos(tmp);
			
				m_diamc_set.InsertString(j-1, val);

				value = _T("");
				m_value.SetWindowText(value);

				//m_insert.EnableWindow(FALSE); roby
				m_update.EnableWindow(FALSE);
				m_erase.EnableWindow(FALSE);
			}
		}
	}
}


void CHy_dia_diamc_set::OnUpdate() 
{
	// TODO: Add your control notification handler code here
	int  i, j;
	bool ins;

	C_REAL	*tmp;

	CString val;
	C_STRING temp;

	CString		msg;
	CString		cpt;

	m_value.GetWindowText(val);
	temp = val;

	if ((val == _T("")) || (gsc_is_numeric(temp.get_name()) == GS_BAD))
	{
		cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
		msg.LoadString(HY_STRING231); // "Valore non valido."
		
		CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OK | MB_ICONQUESTION);
	}
	else
	{
		// Verifico che il nuovo valore non esista già
		if (value_list.search_key(_wtof(val)) != NULL) 
		{
			cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
			msg.LoadString(HY_STRING204); // "Valore già presente."
			
			CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OK | MB_ICONQUESTION);
		}
		else
		{
			i = m_diamc_set.GetCurSel();

			value_list.remove_at(i+1);

			// Inserisco il valore nella lista
			value_list.insert_ordered_double(_wtof(val), &ins);
				
			if (ins == true)
			{
            tmp = (C_REAL *) value_list.get_cursor();
				j = value_list.getpos(tmp);
			
				m_diamc_set.DeleteString(i);
				m_diamc_set.InsertString(j-1, val);

				value = _T("");
				m_value.SetWindowText(value);

//				m_insert.EnableWindow(FALSE); roby
				m_update.EnableWindow(FALSE);
				m_erase.EnableWindow(FALSE);
			}
		}
	}
}


void CHy_dia_diamc_set::OnChangeName() 
{
	m_diamc_set.EnableWindow(TRUE);
	m_value.EnableWindow(TRUE);
	m_insert.EnableWindow(TRUE);
	m_ok.EnableWindow(TRUE);
}


void CHy_dia_diamc_set::OnChangeValue() 
{
	//if (value == _T("")) roby
	//	m_insert.EnableWindow(TRUE);
	//else
	//	m_update.EnableWindow(TRUE);

	if (value != _T(""))
		m_update.EnableWindow(TRUE);
}


void CHy_dia_diamc_set::OnOK() 
{
	// TODO: Add extra validation here
	CString	temp;

	C_INT_STR *punt = NULL;

	CString		msg;
	CString		cpt;
	
	m_name.GetWindowText(temp);

	if (diamC_name == NULL)
		diamC_name = new CString();
	
	if ((diamC_list != NULL) && (*diamC_name != temp))
	{
		punt = (C_INT_STR *) diamC_list->get_head();
		while (punt != NULL)
		{ 
			if (gsc_strcmp(punt->get_name(), temp) == 0)
			{
				cpt.LoadString(HY_STRING202); // "GEOsim PRESSURE NETWORK"
				msg.LoadString(HY_STRING232); // "Nome già presente."
				
				CWnd::FromHandle(adsw_acadMainWnd())->MessageBox(msg, cpt, MB_OK | MB_ICONQUESTION);
				return;
			}
			
			punt = (C_INT_STR *)punt->get_next();
		}

		*diamC_name = temp;
	}

	CDialog::OnOK();
}


// PAOLINO


C_SET_DIAMCOMM::C_SET_DIAMCOMM():C_NODE()
{
	code    = 0;
	name[0] = _T('\0');
	Diam    = _T("GS_DIAM");
}

C_SET_DIAMCOMM::~C_SET_DIAMCOMM() {}


C_LIST_DIAMCOMM::C_LIST_DIAMCOMM()
{
	DiamL = _T("GS_DIAML");
}

C_LIST_DIAMCOMM::~C_LIST_DIAMCOMM() {}


/************************************************************************/
/*.doc C_LIST_DIAMCOMM* C_SET_DIAMCOMM::ptr_list_diamc()				*/
/*+
	Questa funzione restituisce il puntatore alla lista dei diametri.
-*/  
/************************************************************************/
C_LIST_DIAMCOMM* C_SET_DIAMCOMM::ptr_list_diamc()
{ 
	return &list_diamc; 
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::set_name                        						*/
/*+
	Questa funzione controlla e setta il nome di una lista di 
	diametri commerciali.
  
	Parametri:
		TCHAR *nname   nome input da controllare e settare.

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::set_name(const TCHAR *nname)
{ 
	if (nname == NULL)
		return GS_BAD;
	wcsncpy(name, nname, MAX_LEN_NAMEDIAMCOMM - 1); 
   
	return GS_GOOD;
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::get_name()										*/
/*+
	Questa funzione ritorna il nome di una lista di diametri commerciali.
-*/  
/************************************************************************/
TCHAR *C_SET_DIAMCOMM::get_name()
{ 
	return name; 
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::to_rb											*/
/*+
	Questa funzione restituisce una lista di diametri commerciali
	in una lista di resbuf.
-*/  
/************************************************************************/
resbuf *C_SET_DIAMCOMM::to_rb()
{
	C_RB_LIST   list;
   
	if ((list << acutBuildList(RTSHORT, code,
							         RTSTR, name, 0)) == NULL)
		{ gs_errmsg(eGSOutOfMem); return NULL; }

	if ((list += list_diamc.to_rb()) == NULL) return NULL;
   
	if ((list += acutBuildList(RTLE, 0)) == NULL) return NULL;
   
	list.ReleaseAllAtDistruction(GS_BAD);   
   
	return list.get_head();
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::from_rb											*/
/*+
	Questa funzione carica una lista di diametri commerciali 
	da un resbuf.

	Parametri:
		resbuf *rb   resbuf contenente i dati dei diametri.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::from_rb(resbuf *rb)
{
	TCHAR *new_name = NULL;

	// Controllo correttezza del resbuf di input
	if (rb == NULL) { gs_errmsg(eGSListError); return GS_BAD; }
   
	if (rb->restype != RTLB) { gs_errmsg(eGSInvRBType); return GS_BAD; }
   
	// Name
	if ((rb = rb->rbnext) == NULL || rb->restype != RTSTR)
		{ gs_errmsg(eGSInvRBType); return GS_BAD; }
	new_name = rb->resval.rstring;

	// Setto il nome della lista diametri
	if (set_name(new_name) == GS_BAD)
		return GS_BAD;
   
	// Memorizzo la lista dei diametri
	if (list_diamc.from_rb(rb) == GS_BAD) return GS_BAD;

	return GS_GOOD;
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::load_from_db(int cod, C_DBCONNECTION *pConn)		*/
/*+
	Questa funzione carica una lista di diametri commerciali dal database.
  
	Parametri:
		int cod		codice lista diametri

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::load_from_db(int cod, C_DBCONNECTION *pConn)
{
	int		ret = GS_GOOD;
	
	C_STRING	statement;
	
	_RecordsetPtr	pRs;
	
	C_RB_LIST		ColValues;
	presbuf			p;

	TRY
	{
		statement = _T("SELECT * FROM ");
		statement += Diam;
		statement += _T(" WHERE CODE= ");
		statement += cod;

		// Eseguo l' istruzione e ricavo il record set
		if (pConn->OpenRecSet(statement, pRs) == GS_BAD)
			AfxThrowUserException();

		if (gsc_DBReadRow(pRs, ColValues) == GS_BAD)
			AfxThrowUserException();
	
		p = ColValues.CdrAssoc(_T("NAME"));
		if (p->restype != RTSTR)
		{ 
			gs_errmsg(eGSInvRBType); 
			AfxThrowUserException();
		}
		set_name(p->resval.rstring);

		gsc_DBCloseRs(pRs);

		// Memorizzo il codice del set di diametri
		code = cod;
	
		// Leggo la lista dei diametri del set letto
		if (list_diamc.load_from_db(cod, pConn) == GS_BAD) 
			AfxThrowUserException();
	}
	CATCH( CUserException, e )
	{
		gsc_DBCloseRs(pRs);

		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::save_to_db(C_DBCONNECTION *pConn)						*/
/*+
	Questa funzione salva una lista di diametri commerciali nel database.

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::save_to_db(C_DBCONNECTION *pConn)
{
	int ret = GS_GOOD;

	_RecordsetPtr	pRs;

	C_RB_LIST		ColValues;

	TRY
	{
		// Cancellazione lista diametri precedente
		if (del_to_db(pConn) == GS_BAD)
			AfxThrowUserException();

		if (pConn->InitInsRow(Diam, pRs) == GS_BAD)
			AfxThrowUserException();

		if ((ColValues << acutBuildList(RTLB,
										RTLB, RTSTR, _T("CODE"), RTSHORT, code, RTLE,
										RTLB, RTSTR, _T("NAME"), RTSTR, name, RTLE,
										RTLE, 0)) == NULL)
			AfxThrowUserException();

		if (gsc_DBInsRow(pRs, ColValues) == GS_BAD)
			AfxThrowUserException();

		gsc_DBCloseRs(pRs);

		if (list_diamc.save_to_db(code, pConn) == GS_BAD)
		{  
			// Cancellazione set precedente
			del_to_db(pConn);
			AfxThrowUserException();
		}
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::update_to_db(C_DBCONNECTION *pConn)						*/
/*+
	Questa funzione aggiorna una lista di diametri commerciali.

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::update_to_db(C_DBCONNECTION *pConn)
{
	int ret = GS_GOOD;

	C_STRING	statement;
	
	_RecordsetPtr	pRs;

	TRY
	{
		statement = _T("UPDATE [");
		statement += Diam;
		statement += _T("] SET NAME='"); 
		statement += name;
		statement += _T("' ");
		statement += _T("WHERE CODE=");
		statement += code;

		// Eseguo lo statement
		if (pConn->ExeCmd(statement, pRs) == GS_BAD) 
			AfxThrowUserException();
		
		gsc_DBCloseRs(pRs);
		
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::del_to_db(C_DBCONNECTION *pConn)						*/
/*+
	Questa funzione cancella un set di trasferimento nei database.

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::del_to_db(C_DBCONNECTION *pConn)
{
	int ret = GS_GOOD;
	
	CString SqlWhere;

	TRY
	{
		SqlWhere = _T("CODE = ");
		SqlWhere += gsc_tostring(code);

		if (pConn->DelRows(Diam, SqlWhere) == GS_BAD)
			AfxThrowUserException();

		if (list_diamc.del_to_db(code, pConn) == GS_BAD)
			AfxThrowUserException();
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/************************************************************************/
/*.doc int C_SET_DIAMCOMM::getLastCode(int *cod, C_DBCONNECTION *pConn)		*/
/*+
	Questa funzione calcola l' ultimo numero di codice valido di 
	un elenco di diametri commerciali.

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::getLastCode(int *cod, C_DBCONNECTION *pConn)
{
	int ret = GS_GOOD;

	C_STRING	TableRef;
		
	TRY
	{
		TableRef = Diam;

		*cod = 1;
	
		// cerco nuovo codice
		if (gsc_newcode(pConn, TableRef, cod, _T("CODE")) == GS_BAD)
			AfxThrowUserException();

		code = *cod;
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::fromMemory2Class(C_REAL_LIST *list)              */
/*+
  
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::fromMemory2Class(C_REAL_LIST *list)
{
	// copio il contenuto della lista attuale dei diametri
	if (list->copy(&list_diamc) == GS_BAD) 
		return GS_BAD;

	return GS_GOOD;
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::fromClass2Memory(C_REAL_LIST *list)				*/
/*+
	Questa funzione scarica il contenuto delle variabili globali 
	(che contengono i dati dell' elenco in cosiderazione) 
	nell' oggetto della classe.
  
	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::fromClass2Memory(C_REAL_LIST *list)
{
	// Elenco dei diametri
	if (list_diamc.copy(list) == GS_BAD) return GS_BAD;

	return GS_GOOD;
}


/************************************************************************/
/*.doc C_SET_DIAMCOMM::write2file                                       */
/*+
	Questa funzione prepara il file di diametri da dare in input 
	al simulatore.
  
	Parametri:
		CString   *path    path del file da creare.
		int     cod     codice del set da scrivere su file.

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_SET_DIAMCOMM::write2file(CString path, int cod, C_DBCONNECTION *pConn)
{ 
	FILE       *fp = NULL;
	C_DIAMCOMM *pDiam;
	int        res = GS_GOOD;
   bool       Unicode = false;

	load_from_db(cod, pConn);

	// Apro il file path in scrittura
	if ((fp = gsc_fopen(path, _T("w"), MORETESTS, &Unicode)) == NULL)
		return GS_BAD;
	
	do
	{
		if (fwprintf(fp, _T("%d\n"), list_diamc.get_count()) < 0)
			{ gs_errmsg(eGSWriteFile); res = GS_BAD; break; }

		// Scorro la lista dei diametri e scrivo su file 
		if ((pDiam = (C_DIAMCOMM *)list_diamc.get_head()) == NULL) 
			{ res = GS_BAD; break; }
		
		while (pDiam)
		{
			if (fwprintf(fp, _T("%f\n"), pDiam->get_diamc()) < 0)
				{ gs_errmsg(eGSWriteFile); res = GS_BAD; break; }
			pDiam = (C_DIAMCOMM *)list_diamc.get_next();
		}
		break;
	}
	while (0);

	if (fclose(fp) == EOF) 
		{ gs_errmsg(eGSCloseFile); res = GS_BAD; }

	return res;
}


/************************************************************************/
/*.doc C_LIST_DIAMCOMM::load_from_db(int cod, C_DBCONNECTION *pConn)			*/
/*+
	Questa funzione carica la lista dei diametri commerciali dal database.
	
	Parametri:
		int cod   codice della lista da caricare.

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_LIST_DIAMCOMM::load_from_db(int cod, C_DBCONNECTION *pConn)
{
	int ret = GS_GOOD;

	C_STRING	statement;
	
	double		valDiam;

	_RecordsetPtr	pRs;
	
	C_RB_LIST	ColValues;
	presbuf		p;
	
	C_DIAMCOMM   *p_diam;

	TRY
	{
		if (remove_all() == GS_BAD)
			AfxThrowUserException();

		statement = _T("SELECT * FROM ");
		statement += DiamL;
		statement += _T(" WHERE CODE=");
		statement += cod;
      
		// Eseguo l' istruzione e ricavo il record set
		if (pConn->OpenRecSet(statement, pRs) == GS_BAD)
			AfxThrowUserException();

		// Scorro il record set 
		while (gsc_isEOF(pRs) == GS_BAD)
		{
			// Leggo la riga
			if (gsc_DBReadRow(pRs, ColValues) == GS_BAD) 
				AfxThrowUserException( );

			if ((p = ColValues.CdrAssoc(_T("DIAMETER"))) == NULL) 
				AfxThrowUserException( );
			// Memorizzo il valDiam
			if (gsc_rb2Dbl(p, &valDiam) == GS_BAD) 
				AfxThrowUserException( );

			if ((p_diam = new C_DIAMCOMM) == NULL)
				AfxThrowUserException( );
			if (p_diam->set_key(valDiam) == GS_BAD)
				AfxThrowUserException( );

			// Aggiungo in fondo
			add_tail(p_diam);
			
			gsc_Skip(pRs);
		}

		// chiusura record set
		gsc_DBCloseRs(pRs);

	}
	CATCH( CUserException, e )
	{
		remove_all();

		// chiusura record set
		gsc_DBCloseRs(pRs);
		
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/************************************************************************/
/*.doc C_LIST_DIAMCOMM::save_to_db(int cod, C_DBCONNECTION *pConn)				*/
/*+
	Questa funzione salva la lista dei diametri nel database.
  
    Parametri:
		int cod		codice lista diametri

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_LIST_DIAMCOMM::save_to_db(int cod, C_DBCONNECTION *pConn) 
{
	int	ret = GS_GOOD;
	
	_RecordsetPtr	pRs;

	C_DIAMCOMM	*p_diam;

	C_RB_LIST	ColValues;

	TRY
	{
		// Cancellazione delle relazioni precedenti
		if (del_to_db(cod, pConn) == GS_BAD)
			AfxThrowUserException( );

		if (pConn->InitInsRow(DiamL, pRs) == GS_BAD)
			AfxThrowUserException();

		// Scorro l'elenco delle relazioni
		p_diam = (C_DIAMCOMM *) get_head();
		while (p_diam)
		{
			if ((ColValues << acutBuildList(RTLB,
											RTLB, RTSTR, _T("CODE"), RTSHORT, cod, RTLE,
											RTLB, RTSTR, _T("DIAMETER"), RTREAL, p_diam->get_key_double(), RTLE,
											RTLE, 0)) == NULL)
				AfxThrowUserException();

			if (gsc_DBInsRow(pRs, ColValues) == GS_BAD)
				AfxThrowUserException();

			p_diam = (C_DIAMCOMM *) get_next();
		}

		gsc_DBCloseRs(pRs);
	}
	CATCH( CUserException, e )
	{
		// Cancellazione delle relazioni precedenti
		del_to_db(cod, pConn);

		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


/************************************************************************/
/*.doc C_LIST_DIAMCOMM::del_to_db(int cod, C_DBCONNECTION *pConn)		*/
/*+
	Questa funzione cancella una lista di diametri nel database.
  
	Parametri:
		int code;   codice della lista da cancellare

	Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD.
-*/  
/************************************************************************/
int C_LIST_DIAMCOMM::del_to_db(int cod, C_DBCONNECTION *pConn)
{
	int ret = GS_GOOD;

	CString	SqlWhere;
	
	TRY
	{
		SqlWhere = _T("CODE = ");
		SqlWhere += gsc_tostring(cod);

		if (pConn->DelRows(DiamL, SqlWhere) == GS_BAD)
			AfxThrowUserException();
	}
	CATCH( CUserException, e )
	{
		ret=GS_BAD;
	}
	END_CATCH

	return ret;
}


void CHy_dia_diamc_list::OnBnClickedHelp()
{
   CallHelp(m_hWnd, IDH_Gestionedeidiametricommerciali);
}

void CHy_dia_diamc_set::OnBnClickedHelp()
{
   CallHelp(m_hWnd, IDH_Gestionedeidiametricommerciali);
}
