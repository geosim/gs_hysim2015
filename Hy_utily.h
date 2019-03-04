#ifndef _hy_utily_h
#define _hy_utily_h 1


#include "stdafx.h"

#define INITGUID
#import "msado15.dll" no_namespace rename ("EOF", "EndOfFile") rename ("EOS", "ADOEOS")

#include "resource.h"

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

#ifndef _gs_evid_h
   #include "gs_evid.h"
#endif

#ifndef _hy_base_h
   #include "hy_base.h"   
#endif

#ifndef _hy_inout_h
   #include "hy_inout.h"   
#endif

#pragma once


// finestra di dialogo CHy_connect
enum FlowControlEnum
{
   NO_FLOW_CONTROL = 1,   // nessun controllo
   DIRECT_FLOW     = 2,   // direzione flusso
   INVERSE_FLOW    = 4    // inverso direzione flusso
};

class CHy_connect : public CDialog
{
	DECLARE_DYNAMIC(CHy_connect)

public:
	CHy_connect(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CHy_connect();

// Dati della finestra di dialogo
	enum { IDD = IDD_DIALOG_CONNECT };

   CButton m_NoFlowRadio;
   CButton m_FlowRadio;
   CButton m_InverseFlowRadio;

   CButton m_ValveCheck;

   int DisplayFlowCtrls; // abilita i controlli (flag a bit)

   FlowControlEnum FlowControl; // 1 nessun controllo, 2 direzione flusso, 3 inverso direzione flusso
   bool ValveControl; // true verifica stato valvole, false nessuno controllo
   int HelpIndexPage; // indice della pagina di help (questa finestra è usata in diversi contesti)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV

	virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedNoFlowRadio();
   afx_msg void OnBnClickedFlowRadio();
   afx_msg void OnBnClickedInverseFlowRadio();
   afx_msg void OnBnClickedValveCheck();
   afx_msg void OnBnClickedHelp();
	DECLARE_MESSAGE_MAP()
};


// finestra di dialogo CHy_evid_nets

class CHy_evid_nets : public CDialog
{
	DECLARE_DYNAMIC(CHy_evid_nets)

public:
	CHy_evid_nets(CWnd* pParent = NULL);   // costruttore standard
   CHy_evid_nets(C_CLASS *_pCls, 
                 C_INT_LONG_LIST **_pIdsVett, int _IdsVettLen,
                 C_LONG_LIST *_pNodeList, CWnd* pParent = NULL);
	virtual ~CHy_evid_nets();

// Dati della finestra di dialogo
	enum { IDD = IDD_DIALOG_NETS };
   
   C_CLASS         *m_pCls;
   C_INT_LONG_LIST **m_pIdsVett;
   C_INT_LONG_LIST *pCurrIds;
   int             m_IdsVettLen;
   C_LONG_LIST     *m_pNodeList;

   CListCtrl m_NetList;
   int       CurrSel;
   int       OrderedColumn; // 0 nessuna, 1 links, 2 nodes
   bool      AscendingOrder;     // usato da funzione di ordinamento
   int       Prev_iHeader;       // usato da funzione di ordinamento

   void DisplayNetList(void);

protected:
	CImageList m_ImageHdrList;

   virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV

   int Top;
   int Left;
	virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedEvidNet();
   afx_msg void OnBnClickedHelp();
   afx_msg void OnLvnColumnclickNetList(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnDblclkNetList(NMHDR *pNMHDR, LRESULT *pResult) ;
	DECLARE_MESSAGE_MAP()
};


void CallHelp(HWND Wndcaller, int TopicID);


int hy_PutSym_hyss(C_LINK_SET &LinkSet);
void hy_EvidValveToIsolate(void);
void hy_EvidConnected(void);
void hy_EvidNets(void);
void hy_EvidWaterAge(void);
void hy_EvidWaterConc(void);


#endif
