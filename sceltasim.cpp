// sceltasim.cpp : implementation file
//

#include "stdafx.h"
#include "gs_hy.h"
#include "sceltasim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static TCHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Csceltasim dialog


Csceltasim::Csceltasim(C_INT_INT_STR_LIST *l, CWnd* pParent /*=NULL*/)
	: CDialog(Csceltasim::IDD, pParent)
{
	//{{AFX_DATA_INIT(Csceltasim)
	//}}AFX_DATA_INIT
	list = l;
}


void Csceltasim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Csceltasim)
	DDX_Control(pDX, IDC_ListaSimulazioni, m_ListaSim);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Csceltasim, CDialog)
	//{{AFX_MSG_MAP(Csceltasim)
	ON_LBN_DBLCLK(IDC_ListaSimulazioni, OnDblclkListaSimulazioni)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Csceltasim message handlers

BOOL Csceltasim::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	C_INT_INT_STR *pS = NULL;

	// Inizializzazione lista simulazione dcl
	pS = (C_INT_INT_STR *) list->get_head();
	while (pS != NULL)
	{ 
		m_ListaSim.AddString(pS->get_name());
		pS = (C_INT_INT_STR *) list->get_next();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Csceltasim::OnDblclkListaSimulazioni()
{
   OnOK();
}

void Csceltasim::OnOK()
{
	if ((pos = m_ListaSim.GetCurSel()) >= 0)
	   CDialog::OnOK();
}
