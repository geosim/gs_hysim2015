/*************************************************************************
Name: HY_UTILY
                                   
Module description: File funzioni di utilità per le reti acqua e gas in pressione
                    - ricerca valvole per isolare tratto di rete
            
Author: Poltini Roberto
                                         
(c) Copyright 1996-2015 by IREN ACQUA GAS  S.p.A.

*************************************************************************/


#include "StdAfx.h"
#include "hy_utily.h"
#include <limits>
#include <htmlhelp.h>

#include "d2hMap.h" // doc to help


//---------------------------------------------------------------------------//
// Struttura per memorizzare per ciascun link
// la portata iniziale, la portata finale, la velocità, la lunghezza
// l'età dell'acqua a inizio link, la concentrazione a inizio link
//---------------------------------------------------------------------------//
struct TOPONET_H2O_FLOW_INFO
{
   double portata_in;   // portata iniziale (se negativa è uscente)
   double portata_fi;   // portata finale (se negativa è entrante)
   double speed;        // velocità
   double length;       // lunghezza
   double KFactor;      // E' un fattore dato dall'utente mediante prove
                        // empiriche che determina il decadimento della
                        // concentrazione delle sostanze reattive nel tempo
                        // (coefficiente di reazione).
                        // Se il Fattore = 0 significa che deve essere 
                        // calcolata la concentrazione di sostanze non reattive.
                        // Se il Fattore > 0 significa che deve essere 
                        // calcolata la concentrazione di sostanze 
                        // reattive che decadono nel tempo.
                        // Se il Fattore < 0 significa che deve essere 
                        // calcolata la concentrazione di sostanze 
                        // reattive che crescono nel tempo.
                        // Il coefficiente di reazione K è il risultato 
                        // della formula:
                        // K = (-1 / T) * ln (C sample / C init)
                        // dove 
                        // T = tempo di viaggio dell'acqua tra la
                        //     sorgente e il punto di campionamento
                        // C sample = concentrazione misurata nel punto di
                        //            campionamento dopo il tempo T 
                        // C init = concentrazione iniziale misurata alla sorgente

};

struct TOPONET_H2O_NODE_INFO
{
   double age;          // età dell'acqua
   double concentr;     // concentrazione
};


bool hy_AllFlowsKnown(TOPONET_NODE *NodesVett, long iNode,
                      TOPONET_LINK *LinksVett, double *NodeAge,
                      bool CalcConc, double *NodeConc);
int hy_GetWaterAgeConc(TOPONET_LINK *LinksVett, TOPONET_NODE *NodesVett,
                       long iNode, C_LONG_LIST &StoppedNodeList, bool CalcConc = false);
int hy_DrawOnAge(C_CLASS *pCls, TOPONET_LINK *LinksVett, long nLinks,
                 TOPONET_NODE *NodesVett, long nNodes, long TimeFactor);
int hy_DrawOnConc(C_CLASS *pCls, TOPONET_LINK *LinksVett, long nLinks,
                 TOPONET_NODE *NodesVett, long nNodes, double GeneralKFactor);
int hy_getPtsOnAge(ads_name ent, TOPONET_H2O_NODE_INFO *pNodeInfo, C_POINT_LIST &PtList);
int hy_getPtsOnAge(ads_name ent, double InitAge, TOPONET_H2O_FLOW_INFO *pFlowInfo, double Step,
                   C_POINT_LIST &PtList);
int hy_getInvertPtsOnAge(ads_name ent, double InitAge, TOPONET_H2O_FLOW_INFO *pFlowInfo, double Step,
                         C_POINT_LIST &PtList);
int hy_getVirtualPtsOnAge(C_CLASS *pCls, TOPONET_NODE *NodesVett, long iNode, 
                          int *Extract, CString &msg,
                          TOPONET_LINK *LinksVett, double Step, C_POINT_LIST &PtList);
int hy_extractEnt(C_CLASS *pSub, long Id, int *Extract, CString &msg, C_SELSET &SelSet);

int hy_getPtsOnConc(ads_name ent, TOPONET_H2O_NODE_INFO *pNodeInfo, C_POINT_LIST &PtList);
int hy_getPtsOnConc(ads_name ent, double InitAge, double InitConc, 
                    TOPONET_H2O_FLOW_INFO *pFlowInfo, double Step,
                    C_POINT_LIST &PtList);
int hy_getInvertPtsOnConc(ads_name ent, double InitAge, double InitConc,
                          TOPONET_H2O_FLOW_INFO *pFlowInfo, double Step,
                          C_POINT_LIST &PtList);
int hy_getVirtualPtsOnConc(C_CLASS *pCls, TOPONET_NODE *NodesVett, long iNode, 
                           int *Extract, CString &msg,
                           TOPONET_LINK *LinksVett, double Step, C_POINT_LIST &PtList);
int hy_TopologyUpdForInputNodes(C_CLASS *pCls, TOPONET_NODE *NodesVett, long nNodes,
                                double *GeneralKFactor = NULL);

void hy_NetDisplayManager(C_CLASS *pCls, C_INT_LONG_LIST *IdsVett[], int IdsVettLen, 
                          C_LONG_LIST &nNodeList);


#define DSPDxax 0x00E20746L  
const int HDR_BITMAP_WIDTH  = 8;
const int HDR_BITMAP_HEIGHT = 8;
#define IDEVIDNET 12

#define MAX_NETS 1000


///////////////////////////////////////////////////////////////////////////////
// Fine funzione di utilità grafica

/*************************************************************************/
/*.doc gsui_ChangeColorToBmp                                             */
/*+
  Funzione per variare il colore di una bitmap.

  La funzione restituisce TRUE in caso di successo altrimenti FALSE.  
-*/
/*************************************************************************/
bool gsui_ChangeColorToBmp(HBITMAP hBitmap, COLORREF crFrom, COLORREF crTo)
{
   register int cx, cy;     
   BITMAP       bm;     
   HDC          hdcBmp, hdcMask;     
   HBITMAP      hbmMask, hbmOld1, hbmOld2;     
   HBRUSH       hBrush, hbrOld;      
   
   if (!hBitmap) return FALSE;      

   GetObject (hBitmap, sizeof (bm), &bm);     
   cx = bm.bmWidth;     
   cy = bm.bmHeight;      
   hbmMask = CreateBitmap(cx, cy, 1, 1, NULL);     
   hdcMask = CreateCompatibleDC(NULL);     
   hdcBmp  = CreateCompatibleDC(NULL);     
   hBrush  = CreateSolidBrush(crTo);      

   if (!hdcMask || !hdcBmp || !hBrush || !hbmMask)     
   {         
      DeleteObject(hbmMask);         
      DeleteObject(hBrush);         
      DeleteDC(hdcMask);         
      DeleteDC(hdcBmp);         
      return FALSE;     
   }      
   
   hbmOld1 = (HBITMAP) SelectObject (hdcBmp,  hBitmap);     
   hbmOld2 = (HBITMAP) SelectObject (hdcMask, hbmMask);     
   hbrOld  = (HBRUSH) SelectObject (hdcBmp, hBrush);      

   SetBkColor(hdcBmp, crFrom);
   BitBlt(hdcMask, 0, 0, cx, cy, hdcBmp,  0, 0, SRCCOPY);     
   SetBkColor(hdcBmp, RGB(255,255,255));     
   BitBlt(hdcBmp,  0, 0, cx, cy, hdcMask, 0, 0, DSPDxax);      

   SelectObject(hdcBmp,  hbmOld1);     
   SelectObject(hdcMask, hbmOld2);     
   DeleteDC(hdcBmp);     
   DeleteDC(hdcMask);     
   DeleteObject(hBrush);     
   DeleteObject(hbmMask);      

   return TRUE; 
}

/*************************************************************************/
/*.doc gsui_SetBmpColorToDlgBkColor                                      */
/*+
  Funzione per variare il colore di sfondo di una bitmap. Si può usare questa funzione quando
  si deve inserire una BMP in una finestra e cambiare il colore dello sfondo di 
  quest'ultima per adeguarlo a quello della finestra.

  La funzione restituisce TUE in caso di successo altrimenti FALSE.  
-*/
/*************************************************************************/
bool gsui_SetBmpColorToDlgBkColor(HBITMAP hBitmap, COLORREF crFrom)
{
   COLORREF crTo;

   // Get the color of the dialog background. 
   if ((crTo = GetSysColor(COLOR_BTNFACE)) == 0) return FALSE;

   return gsui_ChangeColorToBmp(hBitmap, crFrom, crTo);
}

// Fine funzione di utilità grafica
///////////////////////////////////////////////////////////////////////////////


/******************************************************************************/
/*.doc hy_free_h2o_flow_info                                                  */
/*+
  Questa funzione rilascia le informazioni addizionali sui flussi dal vettore dei link
  usato dalla funzione hy_TopologyCorrectionForFlow.
  Parametri:
  TOPONET_LINK *LinksVett;   Vettore dei link
  long nLinks;               Lunghezza vettore dei link

  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
void hy_free_h2o_flow_info(TOPONET_LINK *LinksVett, long nLinks)
{
   if (!LinksVett) return;
   for (long i = 0; i < nLinks; i++)
      if (LinksVett[i].Punt) { free(LinksVett[i].Punt); LinksVett[i].Punt = NULL; }

   return;
}


/******************************************************************************/
/*.doc hy_free_h2o_node_info                                                  */
/*+
  Questa funzione rilascia le informazioni addizionali sui nodi dal vettore dei nodi.
  Parametri:
  TOPONET_NODE *NodesVett;   Vettore dei nodi
  long nNodes;               Lunghezza vettore dei nodi

  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
void hy_free_h2o_node_info(TOPONET_NODE *NodesVett, long nNodes)
{
   if (!NodesVett) return;
   for (long i = 0; i < nNodes; i++)
      if (NodesVett[i].Punt) { free(NodesVett[i].Punt); NodesVett[i].Punt = NULL; }

   return;
}


/*********************************************************/
/*.doc hy_insertVirtualNodesIntoVetts         <external> */
/*+                                                                       
  Funzione che inserisce dei nodi virtuali la cui posizione
  nel link deve essere in proporzione alla portata_in e alla portata portata_fi
  La funzione deve quindi modificare la topologia nei vettori
  "LinksVett" e "NodesVett" e creare:
  - due nuovi link nel vettore di link (gs_id = -1 * gs_id link originale)
    aggiungendo le informazioni supplementari hy_flow 
    (portata_in, portata_fi, len, vel, age, concentr)
  - un nuovo nodo nel vettore dei nodi (gs_id = -1)

  La funzione non deve e allocare nuovi link o nodi perchè si è già provveduto.
  La funzione utilizza le strutture addizionali TOPONET_H2O_FLOW_INFO
  già allocate per i link.

  Parametri:
  TOPONET_LINK *LinksVett;   puntatore a vettore dei links
  long nLinks,               dimensione del vettore dei links
  long *FirstEmptyLink;      indice nel vettore del primo link vuoto da usare
  TOPONET_NODE *NodesVett;   puntatore a vettore dei nodi
  long nNodes;               dimensione del vettore dei nodi
  long *FirstEmptyNode;      indice nel vettore del primo nodo vuoto da usare
  long gs_id;                codice chiave del link da dividere

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD. 
-*/  
/*********************************************************/
void hy_insertVirtualNodesIntoVetts(TOPONET_LINK *LinksVett, long nLinks, long *FirstEmptyLink,
                                    TOPONET_NODE *NodesVett, long Nodes, long *FirstEmptyNode,
                                    long gs_id)
{
   // cerco link da dividere
   long   iLink = gsc_searchTopoNetLink(LinksVett, nLinks, gs_id);
   long   iNodeIn = LinksVett[iLink].InitialNode;
   long   iNodeFi = LinksVett[iLink].FinalNode;
   long   OrigInitialNode;
   double OrigPortataIn, OrigLength;
   C_LONG_REAL_LIST *pLinkList;
   C_LONG_REAL      *pLink;
   TOPONET_H2O_FLOW_INFO *pFlowInfo;

   // Aggiungo un nuovo nodo virtuale in mezzo al lato da dividere
   NodesVett[*FirstEmptyNode].sub     = 1;
   NodesVett[*FirstEmptyNode].gs_id   = -1;
   NodesVett[*FirstEmptyNode].visited = FALSE;
   // il nuovo nodo risulta collegato a iLink (lato esistente virtualizzato)
   // e a *FirstEmptyLink (lato nuovo virtuale)
   pLinkList = new C_LONG_REAL_LIST();
   pLink = new C_LONG_REAL(iLink, 0.0);
   pLinkList->add_tail(pLink);
   pLink = new C_LONG_REAL(*FirstEmptyLink, 0.0);
   pLinkList->add_tail(pLink);
   NodesVett[*FirstEmptyNode].pLinkList = pLinkList;
   NodesVett[*FirstEmptyNode].Punt      = NULL;

   // Ricodifico il link imponendo un gs_id = -1 * gs_id link originale 
   // per virtualizzarlo
   LinksVett[iLink].gs_id = -1 * LinksVett[iLink].gs_id;
   // memorizzo il nodo iniziale
   OrigInitialNode = LinksVett[iLink].InitialNode;
   // lo collego come nodo iniziale al nuovo nodo virtuale
   LinksVett[iLink].InitialNode = *FirstEmptyNode;
   // Il verso è solo da fi -> in
   LinksVett[iLink].In2Fi_Cost  = (std::numeric_limits<double>::max)();
   LinksVett[iLink].Fi2In_Cost  = 0.0;
   // memorizzo la portata iniziale
   pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[iLink].Punt;
   OrigPortataIn = pFlowInfo->portata_in;
   // impongo la portata iniziale = a quella finale
   pFlowInfo->portata_in = pFlowInfo->portata_fi;
   OrigLength            = pFlowInfo->length; 
   // la lunghezza è in proporzione alla portata
   pFlowInfo->length = OrigLength * fabs(OrigPortataIn) / (fabs(OrigPortataIn) + fabs(pFlowInfo->portata_fi));

   // Aggiungo un nuovo link virtuale con gs_id = -1 * gs_id link originale
   // come ho fatto per LinksVett[iLink].gs_id
   LinksVett[*FirstEmptyLink].sub   = LinksVett[iLink].sub;
   LinksVett[*FirstEmptyLink].gs_id = LinksVett[iLink].gs_id;
   LinksVett[*FirstEmptyLink].visited     = FALSE;
   LinksVett[*FirstEmptyLink].InitialNode = OrigInitialNode;
   LinksVett[*FirstEmptyLink].FinalNode   = *FirstEmptyNode;
   // Il verso è solo da in -> fi
   LinksVett[*FirstEmptyLink].In2Fi_Cost  = 0.0;
   LinksVett[*FirstEmptyLink].Fi2In_Cost  = (std::numeric_limits<double>::max)();
   LinksVett[*FirstEmptyLink].Punt = (TOPONET_H2O_FLOW_INFO *) malloc(sizeof(TOPONET_H2O_FLOW_INFO));
   pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[*FirstEmptyLink].Punt;
   // impongo la portata iniziale = a quella finale
   pFlowInfo->portata_in = OrigPortataIn;
   pFlowInfo->portata_fi = OrigPortataIn;
   // la velocità è ereditata dall'altro link
   pFlowInfo->speed = ((TOPONET_H2O_FLOW_INFO *) LinksVett[iLink].Punt)->speed;
   // la lunghezza è in proporzione alla portata
   pFlowInfo->length   = OrigLength - ((TOPONET_H2O_FLOW_INFO *) LinksVett[iLink].Punt)->length;
   // il fattore di reazione è ereditato dall'altro link
   pFlowInfo->KFactor = ((TOPONET_H2O_FLOW_INFO *) LinksVett[iLink].Punt)->KFactor;

   // Modifico il nodo iniziale esistente per collegarlo al nuovo link virtuale
   pLinkList = NodesVett[OrigInitialNode].pLinkList;
   pLink = (C_LONG_REAL *) pLinkList->search(iLink);
   pLink->set_id(*FirstEmptyLink);

   (*FirstEmptyLink)++;  
   (*FirstEmptyNode)++;
}


/*********************************************************/
/*.doc hy_TopologyCorrectionForFlow           <external> */
/*+                                                                       
  Poichè la simulazione hydromap prevede che nei link il flusso possa
  essere entrante sia da parte del nodo iniziale che da quello finale
  (perchè vi è un consumo distribuito nel link) questo genera una
  eccezione topologica.
  Per superare l'ostacolo bisogna inserire dei nodi virtuali la cui posizione
  nel link deve essere in proporzione alla portata_in e alla portata portata_fi
  La funzione deve quindi modificare la topologia nei vettori
  "LinksVett" e "NodesVett" e creare:
  - due nuovi link nel vettore dei link (gs_id = -1 * gs_id link originale)
  - un nuovo nodo nel vettore dei nodi (gs_id = -1)

  Inoltre verranno aggiunte delle informazioni supplementari 
  (TOPONET_H2O_FLOW_INFO) per tutti i link:
  portata_in, portata_fi, speed, length, KFactor)

  Parametri:
  C_CLASS *pCls;              puntatore a classe simulazione
  TOPONET_LINK **LinksVett;   puntatore a vettore dei links
  long *nLinks,               dimensione del vettore dei links
  TOPONET_NODE **NodesVett;   puntatore a vettore dei nodi
  long *nNodes;               dimensione del vettore dei nodi
  double *GeneralKFactor;     Fattore di reazione generale per la rete. Se indicato
                              devono essere caricati anche i fattori di reazione 
                              per ogni link. Qualora questi non fossero inizializzati 
                              saranno sostituiti con GeneralKFactor (default = NULL).

  Restituisce GS_GOOD in caso di successo altrimenti restituisce GS_BAD. 
-*/  
/*********************************************************/
int hy_TopologyCorrectionForFlow(C_CLASS *pCls, TOPONET_LINK **LinksVett, long *nLinks,
                                 TOPONET_NODE **NodesVett, long *nNodes,
                                 double *GeneralKFactor = NULL)
{
   C_CLASS        *pSub;
   C_PREPARED_CMD Cmd;
   C_RB_LIST      ColValues;
   presbuf        pRbPortataIn = NULL, pRbPortataFi = NULL, pRbLen = NULL, pRbVel = NULL;
   presbuf        pRbVersoIn = NULL, pRbVersoFi = NULL, pRbKFactor = NULL;
   long           nNewLinks, i, Orig_nLinks = *nLinks;
   long           FirstEmptyLink, FirstEmptyNode;
   TOPONET_H2O_FLOW_INFO *pFlowInfo;
   CString               msg;

   // Leggo tutte le informazioni necessarie per gestire i flussi
   // dal database dei lati
   msg.LoadString(HY_STRING239); // "\nCaricamento informazioni relative ai flussi di rete..."
   acutPrintf(msg);

   // Sottoclasse link
   if (!(pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(2)))
      return GS_BAD;

   // Compilo le istruzioni di lettura dei dati della classe dal temp
   if (pSub->prepare_data(Cmd, TEMP) == GS_BAD) return GS_BAD; 

   for (i = 0; i < Orig_nLinks; i++)
   {
      if (gsc_get_data(Cmd, ((*LinksVett)[i]).gs_id, ColValues) == GS_BAD) return GS_BAD;

      if (!pRbPortataIn)
      {
         pRbPortataIn = ColValues.CdrAssoc(_T("PORTATA_IN"));
         pRbPortataFi = ColValues.CdrAssoc(_T("PORTATA_FI"));
         pRbLen       = ColValues.CdrAssoc(_T("LUNGHEZZA"));
         pRbVel       = ColValues.CdrAssoc(_T("VELOCITA"));
         pRbVersoIn   = ColValues.CdrAssoc(_T("VERSO_Q_IN"));
         pRbVersoFi   = ColValues.CdrAssoc(_T("VERSO_Q_FI"));
         
         // si vuole modellare una sostanza reattiva
         if (GeneralKFactor && *GeneralKFactor != 0) 
            if (!(pRbKFactor = ColValues.CdrAssoc(_T("FATTORE_REAZIONE"))))
            {
               // \nLa struttura della simulazione deve essere modificata:
               // \ni lati devono avere l'attributo <FATTORE_REAZIONE> (vedi manuale).
               msg.LoadString(HY_STRING256);
               return GS_BAD;
            }
      }

      if ((((*LinksVett)[i]).Punt = (TOPONET_H2O_FLOW_INFO *) malloc(sizeof(TOPONET_H2O_FLOW_INFO))) == NULL)
         return GS_BAD;
      pFlowInfo = (TOPONET_H2O_FLOW_INFO *) ((*LinksVett)[i]).Punt;

      if (gsc_rb2Dbl(pRbPortataIn, &(pFlowInfo->portata_in)) == GS_BAD) pFlowInfo->portata_in = 0.0;
      if (pRbVersoIn->restype == RTSTR && gsc_strcmp(pRbVersoIn->resval.rstring, _T("<")) == 0)
         pFlowInfo->portata_in = pFlowInfo->portata_in * -1;

      if (gsc_rb2Dbl(pRbPortataFi, &(pFlowInfo->portata_fi)) == GS_BAD) pFlowInfo->portata_fi = 0.0;
      if (pRbVersoFi->restype == RTSTR && gsc_strcmp(pRbVersoFi->resval.rstring, _T("<")) == 0)
         pFlowInfo->portata_fi = pFlowInfo->portata_fi * -1;

      if (gsc_rb2Dbl(pRbLen, &(pFlowInfo->length)) == GS_BAD) pFlowInfo->length = 0.0;
      if (gsc_rb2Dbl(pRbVel, &(pFlowInfo->speed)) == GS_BAD) pFlowInfo->speed = 0.0;

      // si vuole modellare una sostanza reattiva
      if (pRbKFactor) 
      {
         if (gsc_rb2Dbl(pRbKFactor, &(pFlowInfo->KFactor)) == GS_BAD) pFlowInfo->KFactor = *GeneralKFactor;
      }
      else pFlowInfo->KFactor = 0;
   }

   // Conto quanti sono i lati con flusso irregolare
   // Per i lati con portata_in > 0 e portata_fi <= 0 oppure
   // per i lati con portata_in >= 0 e portata_fi < 0
   // (lati in cui il flusso entra da una o più parti ma non esce)
   nNewLinks = 0;
   for (i = 0; i < Orig_nLinks; i++)
   {
      pFlowInfo = (TOPONET_H2O_FLOW_INFO *) ((*LinksVett)[i]).Punt;

      if ((pFlowInfo->portata_in > 0 && pFlowInfo->portata_fi <= 0) ||
          (pFlowInfo->portata_in >= 0 && pFlowInfo->portata_fi < 0))
         nNewLinks++;
   }

   if (nNewLinks == 0) return GS_GOOD;

   // rialloco i vettori per i nodi e i link virtuali
   if ((*LinksVett = (TOPONET_LINK *) realloc(*LinksVett, (*nLinks + nNewLinks) * sizeof(TOPONET_LINK))) == NULL)
      return GS_BAD;
   FirstEmptyLink = *nLinks;
   *nLinks = *nLinks + nNewLinks;
   if ((*NodesVett = (TOPONET_NODE *) realloc(*NodesVett, (*nNodes + nNewLinks) * sizeof(TOPONET_NODE))) == NULL)
      return GS_BAD;
   FirstEmptyNode = *nNodes;
   *nNodes = *nNodes + nNewLinks;

   // inserisco i nodi e i link virtuali
   for (i = 0; i < Orig_nLinks; i++)
   {
      pFlowInfo = (TOPONET_H2O_FLOW_INFO *) ((*LinksVett)[i]).Punt;

      if ((pFlowInfo->portata_in > 0 && pFlowInfo->portata_fi <= 0) ||
          (pFlowInfo->portata_in >= 0 && pFlowInfo->portata_fi < 0))
         hy_insertVirtualNodesIntoVetts(*LinksVett, *nLinks, &FirstEmptyLink,
                                       *NodesVett, *nNodes, &FirstEmptyNode,
                                       ((*LinksVett)[i]).gs_id);
   }

   return GS_GOOD;
}


/*********************************************************/
/*.doc gsc_PutSym_ssfilter                    <external> */
/*+                                                                       
  Funzione che esporta in ambiente LISP (creandone una copia)
  il gruppo di selezione del filtro chiamdolo "sshy".
  Parametri:
  C_LINK_SET &LinkSet;

  Restituisce RTNORM in caso di successo altrimenti restituisce RTERROR. 
-*/  
/*********************************************************/
int hy_PutSym_hyss(C_LINK_SET &LinkSet)
{
	CString msg;
   presbuf p;

   if (LinkSet.ptr_SelSet()->length() == 0)
   {
      if ((p = acutBuildList(RTNIL, 0)) == NULL) return GS_BAD;
   }
   else
   {
      ads_name dummy;

      LinkSet.ptr_SelSet()->get_selection(dummy);
      if ((p = acutBuildList(RTPICKS, dummy, 0)) == NULL) return GS_BAD;
	   msg.LoadString(HY_STRING237); // "\nE' stato impostato il gruppo di selezione <sshy>."
      acutPrintf(msg);
   }
   
   if (acedPutSym(_T("sshy"), p) != RTNORM)
      { acutRelRb(p); return GS_BAD; }
   acutRelRb(p);

   return GS_GOOD;
}


/*********************************************************/
/*.doc hy_EvidValveToIsolate                  <external> */
/*+                                                                       
  Funzione che evidenzia le valvole da chiudere per isolare 
  un elemento noto della rete.
-*/  
/*********************************************************/
void hy_EvidValveToIsolate(void)
{
	int		       ret = GS_GOOD, ValveSub, RegSub = -1, PumpSub = -1, TankSub = -1, sub;
   long            startId;
	CString         msg;
   C_CLASS         *pCls, *pSub;
   C_PROJECT       *pPrj;
   C_TOPOLOGY      Topo;
   long            iNode;
   C_INT_LONG      *pItem;
   C_2STR_INT      *pCostSQL;
   C_2STR_INT_LIST CostSQLList;
	CHydroMap       simulation;
	CHy_connect	    ConnectDialog;
	CAcModuleResourceOverride thisResource;

	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}
   pPrj = get_GS_CURRENT_WRK_SESSION()->get_pPrj();

	gsc_disable_reactors();

	TRY
	{
		if (simulation.SelectNetwork(&startId, &sub) != GS_GOOD)
         AfxThrowUserException();

      // Imposto come default di non verificare la direzione del flusso e lo stato delle valvole
      ConnectDialog.DisplayFlowCtrls = NO_FLOW_CONTROL;
      ConnectDialog.ValveControl = false;
      ConnectDialog.HelpIndexPage = IDH_VERIFICAVALVOLEPERISOLARELARETE;
		if (ConnectDialog.DoModal() != IDOK)
         AfxThrowUserException();

      // cerco la classe madre
      pCls = simulation.pCls;

      if (is_GEOnetworkClass(pCls, GB) == GS_GOOD ||
          is_GEOnetworkClass(pCls, GM) == GS_GOOD) // gas
      {
         ValveSub = 4;
         RegSub   = 3;
      }
      else
      if (is_GEOnetworkClass(pCls, H2O) == GS_GOOD) // acqua
      {
         ValveSub = 5;
         TankSub  = 3;
         PumpSub  = 4;
      }
      else
  	      AfxThrowUserException();

      pCostSQL = new C_2STR_INT();
      pCostSQL->set_name(_T("0.0")); // COSTO SE VALVOLA APERTA
      if (ConnectDialog.ValveControl) // se si deve verificare lo stato delle valvole
      {
         C_SUB *pValveSub = (C_SUB *) pCls->ptr_sub_list()->search_key(ValveSub);
         C_ATTRIB *pAttrib = (C_ATTRIB *) pValveSub->ptr_attrib_list()->search_name(_T("CHIUSA"), FALSE);
         if (gsc_DBIsBoolean(pAttrib->ADOType) == GS_GOOD)
            pCostSQL->set_name2(_T("CHIUSA=FALSE")); // condizione di valvola aperta
         else
            pCostSQL->set_name2(_T("CHIUSA=0")); // condizione di valvola aperta
      }
      else
         pCostSQL->set_name2(_T("GS_ID<0")); // condizione mai soddisfatta
      CostSQLList.add_tail(pCostSQL);

      pCostSQL->set_type(ValveSub); // sub della valvola

      if (ConnectDialog.FlowControl == INVERSE_FLOW)
      {
         C_STRING SQL_cond;

         /////////////////////////////////////////
         // Condizione di passaggio da nodo in->fi
         pCostSQL = new C_2STR_INT();
         pCostSQL->set_type(2); // sub del link
         pCostSQL->set_name(_T("0.0")); // COSTO SE Link passante

         // direzione opposta al flusso (ricerca a monte)
         SQL_cond = _T("VELOCITA<>0 AND ");
         SQL_cond += _T("VERSO_Q_IN='<' AND (VERSO_Q_FI='<' OR VERSO_Q_FI='' OR VERSO_Q_FI IS NULL)"); // condizione di link passante
         pCostSQL->set_name2(SQL_cond.get_name());
         CostSQLList.add_tail(pCostSQL);

         /////////////////////////////////////////
         // Condizione di passaggio da nodo fi->in
         pCostSQL = new C_2STR_INT();
         pCostSQL->set_type(2); // sub del link
         pCostSQL->set_name(_T("0.0")); // COSTO SE Link passante

         // direzione opposta al flusso (ricerca a monte)
         SQL_cond = _T("VELOCITA<>0 AND ");
         SQL_cond += _T("VERSO_Q_FI='>' AND (VERSO_Q_IN='>' OR VERSO_Q_IN='' OR VERSO_Q_IN IS NULL)"); // condizione di link passante

         pCostSQL->set_name2(SQL_cond.get_name());
         CostSQLList.add_tail(pCostSQL);
      }

      Topo.set_type(TYPE_POLYLINE); // tipologia di tipo rete
      Topo.set_cls(pCls);
      if (Topo.LoadInMemory(&CostSQLList) == GS_BAD) return;

      // se si tratta di un link cerco gli elementi connessi al
      // nodo iniziale e poi a quello finale
      if (sub == 2)
      {
         long iLink = gsc_searchTopoNetLink(Topo.LinksVett, Topo.nLinksVett, startId);
         iNode = Topo.LinksVett[iLink].InitialNode;
         Topo.GetNetPropagation(iNode);
         iNode = Topo.LinksVett[iLink].FinalNode;
      }
      else
         iNode = gsc_searchTopoNetNode(Topo.NodesVett, Topo.nNodesVett, startId);

      Topo.GetNetPropagation(iNode);

      // Libero un pò di memoria
      gsc_freeTopoNetNode(&(Topo.NodesVett), &(Topo.nNodesVett));
      gsc_freeTopoNetLink(&(Topo.LinksVett), &(Topo.nLinksVett));

      // Verifico se tra i nodi c'e almeno un'entità che alimenta la rete
      // nodes
      pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
      while (pItem)
      {
         if (pItem->get_key() == TankSub ||
             pItem->get_key() == PumpSub ||
             pItem->get_key() == RegSub)
         {
            msg.LoadString(HY_STRING236); // "\nL'elemento della rete non è isolabile."
            acutPrintf(msg);
    	      AfxThrowUserException();
         }

         pItem = (C_INT_LONG *) pItem->get_next();
      }

      // estraggo tutti gli elementi della simulazione
      C_SELSET SelSet;
      int      Extract = GS_CAN, AlwaysExtract = GS_GOOD;

      msg.LoadString(HY_STRING238); // "Estrarre tutte le entità connesse ?"

      // links
      pItem = (C_INT_LONG *) Topo.ptr_NetLinks()->get_head();
      while (pItem)
      {
         pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(pItem->get_key());

         if (hy_extractEnt(pSub, pItem->get_id(), &Extract, msg, SelSet) == GS_BAD)
   		   AfxThrowUserException();

         pItem = (C_INT_LONG *) pItem->get_next();
      }

      HyAppl::GS_LS.clear();
      HyAppl::GS_LS.cls = pCls->ptr_id()->code;
      HyAppl::GS_LS.sub = ValveSub;

      // nodes
      pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
      while (pItem)
      {
         pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(pItem->get_key());

         if (pItem->get_key() != ValveSub)
         {
            if (hy_extractEnt(pSub, pItem->get_id(), &Extract, msg, SelSet) == GS_BAD)
   		      AfxThrowUserException();
         }
         else // Le valvole vanno sempre estratte
         {
            if (hy_extractEnt(pSub, pItem->get_id(), &AlwaysExtract, msg, SelSet) == GS_BAD)
    		      AfxThrowUserException();
            HyAppl::GS_LS.ptr_SelSet()->add_selset(SelSet);
         }

         pItem = (C_INT_LONG *) pItem->get_next();
      }

      hy_PutSym_hyss(HyAppl::GS_LS);
      // evidenzio solo le valvole
      hy_sim_evid();

      // a questo punto si potrebbe dare un'indicazione all'utente
      // mostrando l'ordine di chiusura delle valvole per minimizzare
      // il colpo di ariete (ordine crescente di portata)
	}   

   CATCH( CUserException, e )
	{}
	END_CATCH

	gsc_enable_reactors();

	return;
}


/*********************************************************/
/*.doc hy_EvidConnected                       <external> */
/*+                                                                       
  Funzione che evidenzia la rete connessa ad uno o più elementi noti
  considerando le direzioni dei flussi (verso valle o verso monte)
  e, opzionalmente, lo stato delle valvole.
-*/  
/*********************************************************/
void hy_EvidConnected(void)
{
	// When resource from this ARX app is needed, just
	// instantiate a local CTemporaryResourceOverride
	CAcModuleResourceOverride thisResource;

   int		       ret = GS_GOOD, ValveSub, sub;
   long            startId;
	CString         msg;
   C_CLASS         *pCls, *pSub;
   C_PROJECT       *pPrj;
   C_TOPOLOGY      Topo;
   long            iNode;
   C_INT_LONG      *pItem;
   C_2STR_INT      *pCostSQL;
   C_2STR_INT_LIST CostSQLList;
	CHydroMap       simulation;
	CHy_connect	    ConnectDialog;
   C_INT_LONG      *pSub_Id;        // Lista degli elementi da cui incominciare
   C_INT_LONG_LIST Sub_Id_list;     // la ricerca di connessione
   C_SELSET        SelSet;

	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}
   pPrj = get_GS_CURRENT_WRK_SESSION()->get_pPrj();

	gsc_disable_reactors();

	TRY
	{
      // seleziono la rete
		if (simulation.SelectNetwork(&startId, &sub) != GS_GOOD)
         AfxThrowUserException();

      // cerco la classe madre
      pCls = simulation.pCls;
      if (is_GEOnetworkClass(pCls, GB) == GS_GOOD ||
          is_GEOnetworkClass(pCls, GM) == GS_GOOD) // gas
         // gas
         ValveSub = 4;
      else
      if (is_GEOnetworkClass(pCls, H2O) == GS_GOOD) // acqua
         ValveSub = 5;
      else
  	      AfxThrowUserException();

      // Seleziono gli oggetti della rete
      if (gsc_ssget(NULL, NULL, NULL, NULL, SelSet) == RTNORM)
      {
         ads_name ent;
         C_LINK   Link;
         int      Cls;

         iNode = 0;
         while (SelSet.entname(iNode++, ent) == GS_GOOD)
         {
            if (Link.SetEnt(ent) == GS_BAD) AfxThrowUserException();
            if (Link.GetKey(&startId, &Cls, &sub) == GS_BAD) AfxThrowUserException();
            // Considero solo gli oggetti della classe selezionata
            if (Cls == pCls->ptr_id()->code)
               // Cerco nella lista per evitare duplicazioni
               if (Sub_Id_list.search(sub, startId) == NULL)
                  Sub_Id_list.add_tail_int_long(sub, startId);
         }

         if (Sub_Id_list.get_count() == 0) AfxThrowUserException();
      }
      else
         Sub_Id_list.add_tail_int_long(sub, startId);

      ConnectDialog.DisplayFlowCtrls = NO_FLOW_CONTROL | DIRECT_FLOW | INVERSE_FLOW;
      ConnectDialog.HelpIndexPage = IDH_VERIFICACONNESSIONI;
		if (ConnectDialog.DoModal() != IDOK)
         AfxThrowUserException();

      // Controlli sui flussi
      if (ConnectDialog.FlowControl != NO_FLOW_CONTROL)
      {
         C_STRING SQL_cond;

         /////////////////////////////////////////
         // Condizione di passaggio da nodo in->fi
         pCostSQL = new C_2STR_INT();
         pCostSQL->set_type(2); // sub del link
         pCostSQL->set_name(_T("0.0")); // COSTO SE Link passante

         SQL_cond = _T("VELOCITA<>0 AND ");
         if (ConnectDialog.FlowControl == DIRECT_FLOW) // direzione del flusso (ricerca a valle)
            SQL_cond += _T("VERSO_Q_IN='>' AND (VERSO_Q_FI='>' OR VERSO_Q_FI='' OR VERSO_Q_FI IS NULL)"); // condizione di link passante
         else // direzione opposta al flusso (ricerca a monte)
            SQL_cond += _T("VERSO_Q_IN='<' AND (VERSO_Q_FI='<' OR VERSO_Q_FI='' OR VERSO_Q_FI IS NULL)"); // condizione di link passante

         pCostSQL->set_name2(SQL_cond.get_name());
         CostSQLList.add_tail(pCostSQL);

         /////////////////////////////////////////
         // Condizione di passaggio da nodo fi->in
         pCostSQL = new C_2STR_INT();
         pCostSQL->set_type(2); // sub del link
         pCostSQL->set_name(_T("0.0")); // COSTO SE Link passante

         SQL_cond = _T("VELOCITA<>0 AND ");
         if (ConnectDialog.FlowControl == DIRECT_FLOW) // direzione del flusso (ricerca a valle)
            SQL_cond += _T("VERSO_Q_FI='<' AND (VERSO_Q_IN='<' OR VERSO_Q_IN='' OR VERSO_Q_IN IS NULL)"); // condizione di link passante
         else // direzione opposta al flusso (ricerca a monte)
            SQL_cond += _T("VERSO_Q_FI='>' AND (VERSO_Q_IN='>' OR VERSO_Q_IN='' OR VERSO_Q_IN IS NULL)"); // condizione di link passante

         pCostSQL->set_name2(SQL_cond.get_name());
         CostSQLList.add_tail(pCostSQL);
      }

      if (ConnectDialog.ValveControl)
      {
         pCostSQL = new C_2STR_INT();
         pCostSQL->set_name(_T("0.0")); // COSTO SE VALVOLA APERTA

         C_SUB *pValveSub = (C_SUB *) pCls->ptr_sub_list()->search_key(ValveSub);
         C_ATTRIB *pAttrib = (C_ATTRIB *) pValveSub->ptr_attrib_list()->search_name(_T("CHIUSA"), FALSE);
         if (gsc_DBIsBoolean(pAttrib->ADOType) == GS_GOOD)
            pCostSQL->set_name2(_T("CHIUSA=FALSE")); // condizione di valvola aperta
         else
            pCostSQL->set_name2(_T("CHIUSA=0")); // condizione di valvola aperta

         pCostSQL->set_type(ValveSub); // sub della valvola
         CostSQLList.add_tail(pCostSQL);
      }

      //if (ValveSub == 4) // gas
      //{
      //   pCostSQL = new C_2STR_INT();
      //   pCostSQL->set_name("0.0");
      //   pCostSQL->set_name2("1=2"); // condizione impossibile per non considerarli percorribili
      //   pCostSQL->set_type(3); // sub dei regolatori
      //   CostSQLList.add_tail(pCostSQL);
      //}
      //else // h2o
      //{
      //   pCostSQL = new C_2STR_INT();
      //   pCostSQL->set_name("0.0");
      //   pCostSQL->set_name2("1=2"); // condizione impossibile per non considerarli percorribili
      //   pCostSQL->set_type(3); // sub dei serbatoi
      //   CostSQLList.add_tail(pCostSQL);

      //   pCostSQL = new C_2STR_INT();
      //   pCostSQL->set_name("0.0");
      //   pCostSQL->set_name2("1=2"); // condizione impossibile per non considerarli percorribili
      //   pCostSQL->set_type(4); // sub delle pompe
      //   CostSQLList.add_tail(pCostSQL);
      //}

      Topo.set_type(TYPE_POLYLINE); // tipologia di tipo rete
      Topo.set_cls(pCls);
      if (Topo.LoadInMemory(&CostSQLList) == GS_BAD) AfxThrowUserException();

      pSub_Id = (C_INT_LONG *) Sub_Id_list.get_head();
      while (pSub_Id)
      {
         sub = pSub_Id->get_key();    // codice sottoclasse
         startId = pSub_Id->get_id(); // codice entità

         // se si tratta di un link cerco gli elementi connessi al
         // nodo iniziale e poi a quello finale
         if (sub == 2)
         {
            long iLink = gsc_searchTopoNetLink(Topo.LinksVett, Topo.nLinksVett, startId);

            // Se il costo dal nodo finale a quello iniziale = 0
            if (Topo.LinksVett[iLink].Fi2In_Cost == 0.0)
            {
               iNode = Topo.LinksVett[iLink].InitialNode;
               Topo.GetNetPropagation(iNode);
            }
            // Se il costo dal nodo iniziale a quello finale = 0
            if (Topo.LinksVett[iLink].In2Fi_Cost == 0.0)
            {
               iNode = Topo.LinksVett[iLink].FinalNode;
               Topo.GetNetPropagation(iNode);
            }
            // Se non è stato inserito da GetNetConnected inserisco il link di partenza
            if (!Topo.ptr_NetLinks()->search_key(startId))
            {
               pItem = new C_INT_LONG(2, startId);
               Topo.ptr_NetLinks()->add_tail(pItem);
            }
         }
         else
         {
            iNode = gsc_searchTopoNetNode(Topo.NodesVett, Topo.nNodesVett, startId);
            Topo.GetNetPropagation(iNode);
         }

         if (ConnectDialog.FlowControl == DIRECT_FLOW) // direzione del flusso (ricerca a valle)
            // Per tutti i link aventi fluido entrante sia dal nodo finale
            // che da quello iniziale verifico se entrambi i nodi sono alimentati
            // dallo stesso punto.
            for (long i = 0; i < Topo.nLinksVett; i++)
               if (Topo.LinksVett[i].In2Fi_Cost == (std::numeric_limits<double>::max)() && 
                   Topo.LinksVett[i].Fi2In_Cost == (std::numeric_limits<double>::max)() &&
                   Topo.ptr_NetNodes()->search_key(Topo.NodesVett[Topo.LinksVett[i].InitialNode].gs_id) &&
                   Topo.ptr_NetNodes()->search_key(Topo.NodesVett[Topo.LinksVett[i].FinalNode].gs_id))
               {
                  pItem = (C_INT_LONG *) new C_INT_LONG(Topo.LinksVett[i].sub, Topo.LinksVett[i].gs_id);
                  Topo.ptr_NetLinks()->add_tail(pItem);
               }

         // estraggo tutti gli elementi della simulazione
         int Extract = GS_CAN;

         msg.LoadString(HY_STRING238); // "Estrarre tutte le entità connesse ?"

         HyAppl::GS_LS.clear();
         HyAppl::GS_LS.cls = pCls->ptr_id()->code;
         HyAppl::GS_LS.sub = 2;

         // links
         pItem = (C_INT_LONG *) Topo.ptr_NetLinks()->get_head();
         while (pItem)
         {
            pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(pItem->get_key());

            if (hy_extractEnt(pSub, pItem->get_id(), &Extract, msg, SelSet) == GS_BAD)
   		      AfxThrowUserException();

            HyAppl::GS_LS.ptr_SelSet()->add_selset(SelSet);
               
            pItem = (C_INT_LONG *) pItem->get_next();
         }

         // nodes
         pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
         while (pItem)
         {
            pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(pItem->get_key());

            if (hy_extractEnt(pSub, pItem->get_id(), &Extract, msg, SelSet) == GS_BAD)
  	            AfxThrowUserException();

            pItem = (C_INT_LONG *) pItem->get_next();
         }

         // procedo con l'entità successiva
         pSub_Id = (C_INT_LONG *) Sub_Id_list.get_next();
      }

      // Libero un pò di memoria
      gsc_freeTopoNetNode(&(Topo.NodesVett), &(Topo.nNodesVett));
      gsc_freeTopoNetLink(&(Topo.LinksVett), &(Topo.nLinksVett));

      hy_PutSym_hyss(HyAppl::GS_LS);
      // evidenzio solo i link
      hy_sim_evid();
	}   

   CATCH( CUserException, e )
	{
		ret = GS_BAD;      
	}
	END_CATCH

	gsc_enable_reactors();
   
	return;
}


/*********************************************************/
/*.doc hy_GetTimeMeasure                      <external> */
/*+                                                                       
  Funzione che chiede l'unità di misura per visualizzare i
  risultati dell'età dell'acqua.
  Poichè i calcoli vengono utilizzati i secondi, il parametro sarà:
  1 se si vuole il risultato espresso in secondi
  60 se si vuole il risultato espresso in minuti
  60 * 60 se si vuole il risultato espresso in ore
  60 * 60 * 24 se si vuole il risultato espresso in giorni
  -1 se deve essere automatico cioè in base al valore di tempo maggiore
  Parametri:
  long *TimeFactor;

  Ritorna GS_GOOD in caso di successo altrimenti GS_BAD;
-*/  
/*********************************************************/
int hy_GetTimeMeasure(long *TimeFactor)
{
   TCHAR   str[133];
   int     res;
   CString msg;

   acutPrintf(_T("\n"));
   msg.LoadString(HY_STRING246); // "Sec Min Ore Giorni Auto _ 1 60 3600 86400 -1"
   acedInitGet(0, msg);
   msg.LoadString(HY_STRING247); // "Scegliere l'unità di misura del tempo [Sec/Min/Ore/Giorni/<Auto>]: "
   if ((res = acedGetKword(msg, str)) == RTERROR || res == RTCAN)
      return GS_BAD;
   if (gsc_strcmp(str, _T("")) == 0) *TimeFactor = -1;
   else *TimeFactor = _wtol(str);

   return GS_GOOD;
}


/*********************************************************/
/*.doc hy_EvidWaterAge                        <external> */
/*+                                                                       
  Funzione che evidenzia l'età dell'acqua per le maglie
  connesse all'oggetto selezionato.
-*/  
/*********************************************************/
void hy_EvidWaterAge(void)
{
	int		       ValveSub, RegSub = -1, PumpSub = -1, TankSub = -1, sub;
   long            startId, TimeFactor;
	CString         msg;
   C_CLASS         *pCls;
   C_PROJECT       *pPrj;
   C_TOPOLOGY      Topo;
   long            iNode;
   C_2STR_INT      *pCostSQL;
   C_2STR_INT_LIST CostSQLList;
   C_LONG_LIST     StartNodeList, StoppedNodeList;
   C_LONG          *pNode;
   C_INT_LONG_LIST SupplyConnectedNodes; // lista degli nodi connessi alla rete che la alimentano idraulicamente
   C_INT_LONG      *pItem;
	CHydroMap       simulation;
	CAcModuleResourceOverride thisResource;

	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}
   pPrj = get_GS_CURRENT_WRK_SESSION()->get_pPrj();

	gsc_disable_reactors();

	TRY
	{
		if (simulation.SelectNetwork(&startId, &sub) != GS_GOOD)
         AfxThrowUserException();

      // cerco la classe madre
      pCls = simulation.pCls;

      if (is_GEOnetworkClass(pCls, H2O) == GS_GOOD) // acqua
      {        
         ValveSub = 5;
         TankSub  = 3;
         PumpSub  = 4;
         RegSub   = 3; // per comodità
      }
      else
      if (is_GEOnetworkClass(pCls, GB) == GS_GOOD ||
          is_GEOnetworkClass(pCls, GM) == GS_GOOD) // gas
      {        
         ValveSub = 4;
         RegSub   = 3;
         TankSub  = 3; // per comodità
      }
      else
         AfxThrowUserException();

      if (hy_GetTimeMeasure(&TimeFactor) == GS_BAD) AfxThrowUserException();

      Topo.set_type(TYPE_POLYLINE); // tipologia di tipo rete
      Topo.set_cls(pCls);

      // Ricavo quali sono gli elementi che alimentano la rete connessa
      // all'entità selezionata
      pCostSQL = new C_2STR_INT();
      pCostSQL->set_name(_T("0.0")); // COSTO SE VALVOLA APERTA
      pCostSQL->set_name2(_T("GS_ID<0")); // condizione mai soddisfatta
      pCostSQL->set_type(ValveSub); // sub della valvola
      CostSQLList.add_tail(pCostSQL);

      if (Topo.LoadInMemory(&CostSQLList) == GS_BAD) AfxThrowUserException();

      // se si tratta di un link cerco gli elementi connessi al
      // nodo iniziale e poi a quello finale
      if (sub == 2)
      {
         long iLink = gsc_searchTopoNetLink(Topo.LinksVett, Topo.nLinksVett, startId);
         iNode = Topo.LinksVett[iLink].InitialNode;
         Topo.GetNetPropagation(iNode);
         iNode = Topo.LinksVett[iLink].FinalNode;
      }
      else
         iNode = gsc_searchTopoNetNode(Topo.NodesVett, Topo.nNodesVett, startId);

      Topo.GetNetPropagation(iNode);

      // Libero un pò di memoria
      gsc_freeTopoNetNode(&(Topo.NodesVett), &(Topo.nNodesVett));
      gsc_freeTopoNetLink(&(Topo.LinksVett), &(Topo.nLinksVett));
      Topo.ptr_NetLinks()->remove_all();

      // Copio in SupplyConnectedNodes solo i nodi che alimentano la rete
      pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
      while (pItem)
      {
         if (pItem->get_key() == TankSub || pItem->get_key() == PumpSub ||
             pItem->get_key() == RegSub)
            SupplyConnectedNodes.add_tail(new C_INT_LONG(pItem->get_key(), pItem->get_id()));

         pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_next();
      }

      // Libero un pò di memoria
      Topo.ptr_NetNodes()->remove_all();      

      CostSQLList.remove_all();
      // Condizione di passaggio da nodo in->fi
      pCostSQL = new C_2STR_INT();
      pCostSQL->set_type(2); // sub del link
      pCostSQL->set_name(_T("0.0")); // COSTO SE Link passante
      pCostSQL->set_name2(_T("VERSO_Q_IN='>' AND VERSO_Q_FI='>' AND PORTATA_IN<>0 AND PORTATA_FI<>0")); // condizione di link passante
      CostSQLList.add_tail(pCostSQL);
      // Condizione di passaggio da nodo fi->in
      pCostSQL = new C_2STR_INT();
      pCostSQL->set_type(2); // sub del link
      pCostSQL->set_name(_T("0.0")); // COSTO SE VALVOLA APERTA
      pCostSQL->set_name2(_T("VERSO_Q_IN='<' AND VERSO_Q_FI='<' AND PORTATA_IN<>0 AND PORTATA_FI<>0")); // condizione di link passante
      CostSQLList.add_tail(pCostSQL);

      pCostSQL = new C_2STR_INT();
      pCostSQL->set_name(_T("0.0")); // COSTO SE VALVOLA APERTA

      C_SUB *pValveSub = (C_SUB *) pCls->ptr_sub_list()->search_key(ValveSub);
      C_ATTRIB *pAttrib = (C_ATTRIB *) pValveSub->ptr_attrib_list()->search_name(_T("CHIUSA"), FALSE);
      if (gsc_DBIsBoolean(pAttrib->ADOType) == GS_GOOD)
         pCostSQL->set_name2(_T("CHIUSA=FALSE")); // condizione di valvola aperta
      else
         pCostSQL->set_name2(_T("CHIUSA=0")); // condizione di valvola aperta

      CostSQLList.add_tail(pCostSQL);

      pCostSQL->set_type(ValveSub); // sub della valvola

      if (Topo.LoadInMemory(&CostSQLList) == GS_BAD) AfxThrowUserException();

      // Poichè la simulazione hydromap prevede che nei link il flusso possa
      // essere entrante sia da parte del nodo iniziale che da quello finale
      // (perchè vi è un consumo distribuito nel link) questo genera una
      // eccezione topologica.
      // Per superare l'ostacolo bisogna inserire dei nodi virtuali la cui posizione
      // nel link deve essere in proporzione alla portata_in e alla portata portata_fi
      // La funzione deve quindi modificare la topologia in memoria nei vettori
      // "LinksVett" e "NodesVett".
      if (hy_TopologyCorrectionForFlow(pCls, &(Topo.LinksVett), &(Topo.nLinksVett),
                                       &(Topo.NodesVett), &(Topo.nNodesVett)) == GS_BAD)
     		AfxThrowUserException();

      // Funzione che legge i tempi iniziali dell'acqua immessa dagli
      // elementi che alimentano la rete (serbatoi, pompe)
      if (hy_TopologyUpdForInputNodes(pCls, Topo.NodesVett, Topo.nNodesVett) == GS_BAD)
     		AfxThrowUserException();

      // Per ciascun componente che alimenta la rete
      // la funzione deve calcolare l'età dell'acqua per tutti i link
      // alimentati esclusivamente da quel componente.
      // All'inizio i componenti che alimentano la rete saranno
      // solo i serbatoi e le pompe. Seguendo la direzione dei flussi
      // si calcola l'età dell'acqua a fine link. Quando si incontra un nodo è possibile
      // passare ai link successivi solo se sono note le età dell'acqua per tutti
      // i link che alimentano il nodo stesso.
      // Una volta terminato, i componenti che alimentano la rete saranno
      // solo i nodi a cui ci si era fermato nei passaggi precedenti.
      // Si procede così finchè sono terminati i nodi che facevano fermare il calcolo

      // anche se non ha senso la prendo in considerazione per i test
      // caso gas
      if (ValveSub == 4)
      {
         for (long i = 0; i < Topo.nNodesVett; i++)
            if (Topo.NodesVett[i].sub == 3 && // regolatore
                SupplyConnectedNodes.search_key(Topo.NodesVett[i].gs_id)) // connesso alla rete selezionata
            {
               if ((pNode = new C_LONG(i)) == NULL)
                  { gs_errmsg(eGSOutOfMem); AfxThrowUserException(); }
               StartNodeList.add_tail(pNode);
            }
      }
      else // caso acqua
      {
         for (long i = 0; i < Topo.nNodesVett; i++)
            if ((Topo.NodesVett[i].sub == 3 || Topo.NodesVett[i].sub == 4) && // serbatoio o pompa
                SupplyConnectedNodes.search_key(Topo.NodesVett[i].gs_id)) // connesso alla rete selezionata
            {
               if ((pNode = new C_LONG(i)) == NULL)
                  { gs_errmsg(eGSOutOfMem); AfxThrowUserException(); }
               StartNodeList.add_tail(pNode);
            }
      }

      Topo.ptr_NetNodes()->remove_all();

      StoppedNodeList.remove_all();
      StartNodeList.sort_id(); // ordino in modo crescente
      pNode = (C_LONG *) StartNodeList.get_head();
      while (pNode)
      {
         while (pNode)
         {
            // calcolo le età dell'acqua della rete connessa al nodo
            if (hy_GetWaterAgeConc(Topo.LinksVett, Topo.NodesVett, pNode->get_id(),
                                   StoppedNodeList) == GS_BAD)
               AfxThrowUserException(); 

            pNode = (C_LONG *) StartNodeList.get_next();
         }
         
         // se StartNodeList = StoppedNodeList allora interrompo perchè
         // le direzioni dei fluidi non consentono di risolvere il comando
         StoppedNodeList.sort_id(); // ordino in modo crescente
         if (StartNodeList == StoppedNodeList)
         {
            msg.LoadString(HY_STRING240); // le direzioni dei fluidi non consentono di risolvere il comando
            acutPrintf(msg);

            msg.LoadString(HY_STRING241); // \nVerificare le seguenti entità:
            acutPrintf(msg);

            msg.LoadString(HY_STRING242); // \nSottoclasse: %d, codice chiave %ld
            pNode = (C_LONG *) StartNodeList.get_head();
            while (pNode)
            {
               acutPrintf(msg,
                          Topo.NodesVett[pNode->get_id()].sub,
                          Topo.NodesVett[pNode->get_id()].gs_id);
               pNode = (C_LONG *) StartNodeList.get_next();
            }

            AfxThrowUserException(); 
         }

         StoppedNodeList.copy(&StartNodeList);
         StoppedNodeList.remove_all();
         pNode = (C_LONG *) StartNodeList.get_head();
      }

      // Disegno le isolinee dell'età dell'acqua
      if (hy_DrawOnAge(pCls, Topo.LinksVett, Topo.nLinksVett,
                       Topo.NodesVett, Topo.nNodesVett, TimeFactor) == GS_BAD)
         AfxThrowUserException(); 
	}

   CATCH( CUserException, e )
	{
	}
	END_CATCH

	gsc_enable_reactors();
   
	return;
}


/******************************************************************************/
/*.doc hy_GetWaterAgeConc                                                     */
/*+
  Questa funzione calcola l'età dell'acqua per quella parte di
  rete connessa al nodo indicato e per cui ci siano i dati
  necessari al calcolo. Infatti Quando si incontra un nodo è possibile
  passare ai link successivi solo se sono note le età dell'acqua per tutti
  i link che alimentano il nodo stesso.
  Parametri:
  La funzione modificherà le informazioni supplementari ai nodi 
  (age e concentr) e restituirà una lista di nodi in cui non
  sono note tutte le età dell'acqua per tutti i link che alimentano il nodo stesso.
  Parametri:
  TOPONET_LINK *LinksVett;       Vettori di topologia per i link
  TOPONET_NODE *NodesVett;       Vettori di topologia per i nodi
  long iNode;                    Nodo da cui partire
  C_LONG_LIST &StoppedNodeList;  Lista dei nodi in cui ci si è fermati
  bool CalcConc;                 Flag se = true significa che si deve fare 
                                 il calcolo delle concentrazioni (default = false).
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_GetWaterAgeConc(TOPONET_LINK *LinksVett, TOPONET_NODE *NodesVett,
                       long iNode, C_LONG_LIST &StoppedNodeList, bool CalcConc)
{
   C_LONG_REAL *pLink;
   C_LONG      *p;
   long        iAdjNode;
   double      NodeAge, NodeConc;

   // visited = true significa che il nodo era già stato visitato e
   // che erano note tutte le età dell'acqua per tutti i link lo alimentavano.
   if (!NodesVett[iNode].visited)
   {
      // verifico se per tutti i link entranti sono note le età dell'acqua
      // ed in caso positivo calcolo l'età nel nodo
      if (!hy_AllFlowsKnown(NodesVett, iNode, LinksVett, &NodeAge, CalcConc, &NodeConc))
      {
         // Inserisco nella lista dei nodi in cui ci si è fermati
         // (se non ancora inserito)
         if (!StoppedNodeList.search_key(iNode))
         {
            if ((p = new C_LONG(iNode)) == NULL)
               { gs_errmsg(eGSOutOfMem); return GS_BAD; }
            StoppedNodeList.add_tail(p);
         }

         return GS_GOOD;
      }
      else
      {
         // Lo segno come già visitato avente, per tutti i link entranti,
         // le età dell'acqua note
         NodesVett[iNode].visited = true;
         // memorizzo l'età dell'acqua nel nodo
         if ((NodesVett[iNode].Punt = (TOPONET_H2O_NODE_INFO *) malloc(sizeof(TOPONET_H2O_NODE_INFO))) == NULL)
            return GS_BAD;
         ((TOPONET_H2O_NODE_INFO *) NodesVett[iNode].Punt)->age = NodeAge;
         if (CalcConc) // memorizzo la concentrazione nell'acqua nel nodo
            ((TOPONET_H2O_NODE_INFO *) NodesVett[iNode].Punt)->concentr = NodeConc;

         // Se il nodo era stato inserito nella lista dei nodi in cui
         // ci si era fermati allora lo elimino da tale lista
         StoppedNodeList.remove_key(iNode);
      }
   }

   // Lista dei link collegati
   if (NodesVett[iNode].pLinkList)
      pLink = (C_LONG_REAL*) NodesVett[iNode].pLinkList->get_head();
   else
      pLink = NULL;

   while (pLink)
   {
      // Se il costo per passare dal nodo al link è il limite massimo di un double
      // significa che non si può passare da quel link
      if (pLink->get_real() == (std::numeric_limits<double>::max)())
         { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }

      // Se si sta attraversando il link dal nodo iniziale a quello finale
      if (LinksVett[pLink->get_id()].InitialNode == iNode)
      {
         // Se il costo per passare sul link è il limite massimo di un double
         // significa che non si può passare su quel link
         if (LinksVett[pLink->get_id()].In2Fi_Cost == (std::numeric_limits<double>::max)())
            { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
         // Leggo nodo all'altro capo del link
         iAdjNode = LinksVett[pLink->get_id()].FinalNode;
      }
      else // altrimenti si sta attraversando il link dal nodo finale a quello iniziale
      {
         // Se il costo per passare sul link è il limite massimo di un double
         // significa che non si può passare su quel link
         if (LinksVett[pLink->get_id()].Fi2In_Cost == (std::numeric_limits<double>::max)())
            { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
         // Leggo nodo all'altro capo del link
         iAdjNode = LinksVett[pLink->get_id()].InitialNode;
      }

      // Se il link non era già stato visitato
      if (!LinksVett[pLink->get_id()].visited)
      {
         // Lo segno come già visitato
         LinksVett[pLink->get_id()].visited = true;
         
         // continuo la visita della rete
         if (hy_GetWaterAgeConc(LinksVett, NodesVett, iAdjNode, StoppedNodeList,
                                CalcConc) == GS_BAD)
            break;               
      }

      pLink = (C_LONG_REAL*) pLink->get_next();
   }

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_AllFlowsKnown                                                       */
/*+
  Questa funzione, di ausilio alla hy_GetWaterAgeConc, verifica se sono note 
  le età dell'acqua per tutti i link che alimentano un nodo noto.
  Se il nodo non è alimentato l'età dell'acqua in quel punto = 0.
  Se il nodo è alimentato da più link, l'età dell'acqua sarà uguale alla
  media pesata delle età in base alle portate.
  Oltre all'età dell'acqua la funzione calcola anche la concentrazione di
  sostanze reattive e non reattive (vedi parametro KFactor).
  Se il nodo è alimentato da più link, la concentrazione sarà uguale alla
  media pesata delle concentrazioni in base alle portate.
  Parametri:
  Parametri:
  TOPONET_NODE *NodesVett;       Vettori di topologia per i nodi
  long iNode;                    Nodo in questione
  TOPONET_LINK *LinksVett;       Vettori di topologia per i link
  double *NodeAge;               Età del nodo (out)
  bool CalcConc;                 Flag se = true significa che si deve fare
                                 il calcolo delle concentrazioni.
  double *NodeConc;              Concentrazione nel nodo (out) usato 
                                 se <CalcConc> = true

  Restituisce true nel caso in cui siano note le età dell'acqua per tutti
  i link che alimentano il nodo (e quindi la funzione è in grado di calcolare
  sia l'età dell'acqua sia la concentrazione di sostanze).
-*/  
/******************************************************************************/
bool hy_AllFlowsKnown(TOPONET_NODE *NodesVett, long iNode,
                      TOPONET_LINK *LinksVett, double *NodeAge,
                      bool CalcConc, double *NodeConc)
{
   C_LONG_REAL_LIST *pLinkList;
   C_LONG_REAL      *pLink;
   double           SumPortate = 0.0, SumAgePortate = 0.0, Portata;
   double           LinkAge, FinalLinkAge, FinalLinkConc, SumConcPortate = 0.0;
   TOPONET_H2O_FLOW_INFO *pFlowInfo;
   TOPONET_H2O_NODE_INFO *pNodeInfo;

   *NodeAge = 0.0;
     
   // Lista dei link collegati
   if (!(pLinkList = (C_LONG_REAL_LIST *) NodesVett[iNode].pLinkList))
      return true; // nodo isolato

   pLink = (C_LONG_REAL *) pLinkList->get_head();
   while (pLink)
   {
      pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[pLink->get_id()].Punt;

      // Se il link non è entrante nel nodo lo salto

      // Se si sta attraversando il link dal nodo iniziale a quello finale
      if (LinksVett[pLink->get_id()].InitialNode == iNode)
      {
         // Se il costo per passare sul link è il limite massimo di un double
         // significa che il link non alimenta il nodo
         if (LinksVett[pLink->get_id()].Fi2In_Cost == (std::numeric_limits<double>::max)())
            { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
         Portata = fabs(pFlowInfo->portata_in);

         // Informazioni del nodo a inizio link (nella direzione del flusso)
         pNodeInfo = (TOPONET_H2O_NODE_INFO *) (NodesVett[LinksVett[pLink->get_id()].FinalNode].Punt);
      }
      else // altrimenti si sta attraversando il link dal nodo finale a quello iniziale
      {
         // Se il costo per passare sul link è il limite massimo di un double
         // significa che il link non alimenta il nodo
         if (LinksVett[pLink->get_id()].In2Fi_Cost == (std::numeric_limits<double>::max)())
            { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
         Portata = fabs(pFlowInfo->portata_fi);

         // Informazioni del nodo a inizio link (nella direzione del flusso)
         pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[LinksVett[pLink->get_id()].InitialNode].Punt;
      }


      // Se l'età a inizio link non è nota
      if (!pNodeInfo) return false;
      if (pNodeInfo->age == -1.0) return false;

      // Tempo di attraversamento nel link
      LinkAge       = (pFlowInfo->length / pFlowInfo->speed);
      FinalLinkAge  = pNodeInfo->age + LinkAge;
      SumAgePortate += FinalLinkAge * Portata;
      SumPortate    += Portata;

      // Se si deve calcolare anche la concentrazione
      if (CalcConc)
      {
         // Se la concentrazione a inizio link non è nota
         if (pNodeInfo->concentr == -1.0) return false;

         // La concentrazione a fine lato è data dalla formula:
         // C sample = C init * exp(-K * T)
         // dove
         // T = tempo di viaggio dell'acqua tra la
         //     sorgente e il punto di campionamento
         // C sample = concentrazione misurata nel punto di
         //             campionamento dopo il tempo T 
         // C init = concentrazione iniziale misurata alla sorgente
         // K = coefficiente di reazione
         FinalLinkConc = pNodeInfo->concentr * exp(-1 * pFlowInfo->KFactor * LinkAge);

         SumConcPortate += FinalLinkConc * Portata;
      }

      pLink = (C_LONG_REAL*) pLink->get_next();
   }

   if (SumPortate == 0) *NodeAge = 0.0;
   else *NodeAge = SumAgePortate / SumPortate;

   // Se si deve calcolare anche la concentrazione
   if (CalcConc)
   {
      if (SumPortate == 0) *NodeConc = 0.0;
      else *NodeConc = SumConcPortate / SumPortate;
   }

   return true;
}


/******************************************************************************/
/*.doc hy_DrawOnAge                                                           */
/*+
  Questa funzione disegna dei punti con la Z rappresentante l'età dell'acqua.
  Vengono considerati tutti i nodi e le polilinee vengono campionate ad intervalli
  di tempo noto.
  Parametri:
  C_EXTERN *pCls;             puntatori alla classe simulazione
  TOPONET_LINK *LinksVett;    Vettori di topologia per i link
  long nLinks;                Lunghezza vettore LinksVett
  TOPONET_NODE *NodesVett;    Vettori di topologia per i nodi
  long nNodes;                Lunghezza vettore NodesVett
  long TimeFactor;            Unità di misura del tempo

  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_DrawOnAge(C_CLASS *pCls, TOPONET_LINK *LinksVett, long nLinks,
                 TOPONET_NODE *NodesVett, long nNodes, long TimeFactor)
{
   long                  i, Step;
   TOPONET_H2O_FLOW_INFO *pFlowInfo;
   TOPONET_H2O_NODE_INFO *pNodeInfo;
   C_CLASS               *pSub;
   C_SELSET              SelSet;
   int                   Extract = GS_CAN;
   C_POINT_LIST          PtList;
   CString               msg, msg1;
   ads_name              ent;
   C_LONG_REAL_LIST      *pLinkList;
   C_LONG_REAL           *pLink;

   msg.LoadString(HY_STRING243); // "Estrarre tutte le entità con età dell'acqua nota ?"

   if (TimeFactor == -1) // unità di misura in automatico
   {
      double MaxAge = 0;

      // Cerco valore maggiore
      for (i = 1; i < nNodes; i++)
         // Se il nodo ha le informazioni sull'età dell'acqua
         if ((pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[i].Punt) != NULL)
            if (pNodeInfo->age > MaxAge) MaxAge = pNodeInfo->age;

      if (MaxAge < 60) TimeFactor = 1; // Secondi
      else if (MaxAge < 60 * 60) TimeFactor = 60; // Minuti
      else if (MaxAge < 60 * 60 * 24) TimeFactor = 60 * 60; // Ore
      else TimeFactor = 60 * 60 * 24; // Giorni
   }

   // Il passo di campionamento per le tubazioni (secondi) è valutato in base
   // all'unità di misura del tempo e si sceglie l'unità di misura inferiore
   // ad esempio se l'unità di misura fosse minuti il campionamento sarebbe secondi
   if (TimeFactor == 1 || TimeFactor == 60) Step = 1;
   else if (TimeFactor == 60 * 60) Step = 60;
   else if (TimeFactor == 60 * 60 * 24) Step = 60 * 60;
   
   // Nodes
   for (i = 0; i < nNodes; i++)
      // Se il nodo ha le informazioni sull'età dell'acqua
      if ((pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[i].Punt) != NULL)
         if (NodesVett[i].gs_id > 0) // nodo normale
         {
            pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(NodesVett[i].sub);

            if (hy_extractEnt(pSub, NodesVett[i].gs_id, &Extract, msg, SelSet) == GS_BAD)
   		      return GS_BAD;

            SelSet.intersectType(GRAPHICAL);
            if (SelSet.entname(0, ent) == GS_GOOD)
               if (hy_getPtsOnAge(ent, pNodeInfo, PtList) == GS_BAD)
                  return GS_BAD;
         }
         else // nodo virtuale
            // La funzione tratta il nodo virtuale e i due link virtuali
            // ad esso collegati
            if (hy_getVirtualPtsOnAge(pCls, NodesVett, i, &Extract, msg,
                                      LinksVett, Step, PtList) == GS_BAD)
               return GS_BAD;

   // Links
   for (i = 0; i < nNodes; i++)
   {
      // Se il nodo ha le informazioni sull'età dell'acqua
      if ((pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[i].Punt) == NULL)
         continue;
      // ciclo su tutti i link in uscita dal nodo in questione
      if (!(pLinkList = (C_LONG_REAL_LIST *) NodesVett[i].pLinkList))
         continue;
      pLink = (C_LONG_REAL *) pLinkList->get_head();
      while (pLink)
      {
         if (LinksVett[pLink->get_id()].gs_id < 0) // salto i link virtuali
            { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
         
         // se il flusso è in uscita dal nodo

         // Se si sta attraversando il link dal nodo iniziale a quello finale
         if (LinksVett[pLink->get_id()].InitialNode == i)
         {
            // Se il costo per passare sul link è il limite massimo di un double
            // significa che il link non esce dal nodo
            if (LinksVett[pLink->get_id()].In2Fi_Cost == (std::numeric_limits<double>::max)())
               { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
            pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[pLink->get_id()].Punt;
            pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(LinksVett[pLink->get_id()].sub);

            if (hy_extractEnt(pSub, LinksVett[pLink->get_id()].gs_id, &Extract, msg, SelSet) == GS_BAD)
   		      return GS_BAD;

            SelSet.intersectType(GRAPHICAL);
            if (SelSet.entname(0, ent) == GS_GOOD)
               if (hy_getPtsOnAge(ent, pNodeInfo->age, pFlowInfo, Step, PtList) == GS_BAD)
                  return GS_BAD;
         }
         else // altrimenti si sta attraversando il link dal nodo finale a quello iniziale
         {
            // Se il costo per passare sul link è il limite massimo di un double
            // significa che il link non esce dal nodo
            if (LinksVett[pLink->get_id()].Fi2In_Cost == (std::numeric_limits<double>::max)())
               { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
            pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[pLink->get_id()].Punt;
            pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(LinksVett[pLink->get_id()].sub);

            if (hy_extractEnt(pSub, LinksVett[pLink->get_id()].gs_id, &Extract, msg, SelSet) == GS_BAD)
   		      return GS_BAD;

            SelSet.intersectType(GRAPHICAL);
            if (SelSet.entname(0, ent) == GS_GOOD)
               if (hy_getInvertPtsOnAge(ent, pNodeInfo->age, pFlowInfo, Step, PtList) == GS_BAD)
                  return GS_BAD;
         }

         pLink = (C_LONG_REAL*) pLink->get_next();
      }
   }

   C_STRING Layer;
         
   if (gsc_get_tmp_layer(_T("Water Age Points "), Layer) == GS_BAD) return GS_BAD;
   msg.LoadString(HY_STRING244); // "\nDisegno punti con quota uguale all'età dell'acqua (%s)."
   if (TimeFactor == 1) msg1.LoadString(HY_STRING248); // "Sec"
   else if (TimeFactor == 60) msg1.LoadString(HY_STRING249); // "Min"
   else if (TimeFactor == 60 * 60) msg1.LoadString(HY_STRING250); // "Ore"
   else msg1.LoadString(HY_STRING251); // "Giorni"

   acutPrintf(msg, msg1);

   msg.LoadString(HY_STRING245); // "\nI punti saranno disegnati sul layer <%s>..."
   acutPrintf(msg, Layer.get_name());

   if (TimeFactor > 1)
   {
      C_POINT *pPt = (C_POINT *) PtList.get_head();
      
      while (pPt)
      {
         pPt->point[Z] /= TimeFactor;
         pPt = (C_POINT *) pPt->get_next();
      }
   }

   PtList.Draw(Layer.get_name());

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_getPtsOnAge                                                         */
/*+
  Questa funzione ottiene una lista di punti di una polilinea
  in cui la coordinata Z si riferisce all'età dell'acqua.
  Verranno letti tutti i vertici esclusi quello iniziale
  e finale con un passo di tempo noto (es. ogni 5 min) partendo
  dal punto iniziale della polilinea verso quello finale
  (punti geometrici).
  Parametri:
  ads_name ent;                     Oggetto grafico polilinea
  double InitAge;                   Età a inizio link
  TOPONET_H2O_FLOW_INFO *pFlowInfo; Informazioni addizionali sul flusso
  double Step;                      Passo di campionamento in secondi
  C_POINT_LIST &PtList;             Lista dei punti notevoli (out)
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_getPtsOnAge(ads_name ent, double InitAge, TOPONET_H2O_FLOW_INFO *pFlowInfo,
                   double Step, C_POINT_LIST &PtList)
{
   C_POINT   *pPt;
   double    Age, Angle, offset;
   ads_point pt;

   Age = Step - fmod(InitAge, Step);

   offset = pFlowInfo->speed * Age;
   while (offset < pFlowInfo->length &&
          gsc_getPtOnLinearObj(ent, offset, pt, &Angle) == GS_GOOD)
   {
      pt[Z] = Age + InitAge;
      if ((pPt = new C_POINT(pt)) == NULL) return GS_BAD;
      PtList.add_tail(pPt);

      Age += Step;
      offset = pFlowInfo->speed * Age;
   }

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_getInvertPtsOnAge                                                   */
/*+
  Questa funzione ottiene una lista di punti di una polilinea
  in cui la coordinata Z si riferisce all'età dell'acqua.
  Verranno letti tutti i vertici esclusi quello iniziale
  e finale con un passo di tempo noto (es. ogni 5 min) partendo
  dal punto finale della polilinea verso quello iniziale
  (punti geometrici).
  Parametri:
  ads_name ent;                     Oggetto grafico polilinea
  double InitAge;                   Età a inizio link
  TOPONET_H2O_FLOW_INFO *pFlowInfo; Informazioni addizionali sul flusso
  double Step;                      Passo di campionamento in secondi
  C_POINT_LIST &PtList;             Lista dei punti notevoli (out)
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_getInvertPtsOnAge(ads_name ent, double InitAge, TOPONET_H2O_FLOW_INFO *pFlowInfo,
                         double Step, C_POINT_LIST &PtList)
{
   C_POINT   *pPt;
   double    Age, Angle, offset, Len;
   ads_point pt;
   presbuf   pRb;

   // Calcolo la lunghezza della polilinea originale
   if (!(pRb = gsc_get_graphical_data(ent, GS_LISP_LENGTH)) ||
       gsc_rb2Dbl(pRb, &Len) == GS_BAD)
      return GS_BAD;

   Age = Step - fmod(InitAge, Step);

   offset = pFlowInfo->speed * Age;
   while (offset < pFlowInfo->length &&
          gsc_getPtOnLinearObj(ent, Len - offset, pt, &Angle) == GS_GOOD)
   {
      pt[Z] = Age + InitAge;
      if ((pPt = new C_POINT(pt)) == NULL) return GS_BAD;
      PtList.add_tail(pPt);

      Age += Step;
      offset = pFlowInfo->speed * Age;
   }

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_getPtsOnAge                                                         */
/*+
  Questa funzione ottiene una lista di punti di un blocco 
  in cui la coordinata Z si riferisce all'età dell'acqua.
  Verrà letto il punto di inserimento.
  Parametri:
  ads_name ent;                     Oggetto grafico
  TOPONET_H2O_NODE_INFO *pNodeInfo;
  C_POINT_LIST &PtList;             Lista dei punti notevoli (out)
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_getPtsOnAge(ads_name ent, TOPONET_H2O_NODE_INFO *pNodeInfo,
                   C_POINT_LIST &PtList)
{
   C_POINT   *pPt;
   ads_point pt;

   if (gsc_get_firstPoint(ent, pt) == GS_BAD) return GS_BAD;
   pt[Z] = pNodeInfo->age;
   if ((pPt = new C_POINT(pt)) == NULL) return GS_BAD;
   PtList.add_tail(pPt);

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_extractEnt                                                         */
/*+
  Questa funzione ha lo scopo di ottenere un gruppo di selezione degli 
  oggetti di un'entità GEOsim. A tale scopo, se necessario, 
  chiede o estrae direttamente (a seconda del parametro Extract) 
  tutti gli oggetti di un'entità.
  Parametri:
  C_CLASS *pSub;     Puntatore ala sottoclasse;
  long Id;           Codice chiave
  int *Extract;      Flag per estrarre direttamente (GS_GOOD)
                     per richiedere di estrarre (GS_CAN) o
                     non estrarre (GS_BAD). Questo parametro verrà 
                     variato dalla funzione
  CString &msg;      messaggiO di richiesta se *Extract = GS_CAN
  C_SELSET &SelSet;  Gruppo di selezione risultante
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_extractEnt(C_CLASS *pSub, long Id, int *Extract, CString &msg, C_SELSET &SelSet)
{
   int WhyNot;

   if (pSub->get_SelSet(Id, SelSet) == GS_BAD) return GS_BAD;

   if (SelSet.length() == 0)
   {
      if (*Extract == GS_CAN)
      {  // Chiedo all'utente se vuole estrarre gli oggetti
         msg.LoadString(HY_STRING238); // "Estrarre tutte le entità connesse ?"
         gsc_ddgetconfirm((LPCTSTR) msg, Extract);
      }

      // Provo ad estrarre totalmente le istanze grafiche dell'entità
      if (*Extract == GS_GOOD)
      {
         // Provo ad estrarre totalmente le istanze grafiche dell'entità
         if (pSub->entExtract(Id) == GS_BAD) return GS_BAD;
         pSub->get_SelSet(Id, SelSet);
      }
   }
   else
   {
      // se entità estratta parzialmente provo ad estrarre totalmente le istanze grafiche dell'entità
      if (pSub->is_updateableSS(Id, SelSet, &WhyNot, GS_BAD, GS_BAD) == GS_BAD &&
          WhyNot == eGSPartialEntExtract)
      {
         if (*Extract == GS_CAN)
         {  // Chiedo all'utente se vuole estrarre gli oggetti
            // se non si tratta di valvole
            msg.LoadString(HY_STRING238); // "Estrarre tutte le entità connesse ?"
            gsc_ddgetconfirm((LPCTSTR) msg, Extract);
         }

         // Provo ad estrarre totalmente le istanze grafiche dell'entità
         if (*Extract == GS_GOOD)
         {
            if (pSub->entExtract(Id) == GS_BAD) return GS_BAD;
            pSub->get_SelSet(Id, SelSet);
         }
      }
   }

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_getVirtualPtsOnAge                                                  */
/*+
  Questa funzione ha lo scopo di ottenere un gruppo di selezione degli 
  oggetti di un'entità GEOsim. A tale scopo, se necessario, 
  chiede o estrae direttamente (a seconda del parametro Extract) 
  tutti gli oggetti di un'entità.
  Parametri:
  C_CLASS *pSub;     Puntatore ala sottoclasse;
  long Id;           Codice chiave
  int *Extract;      Flag per estrarre direttamente (GS_GOOD)
                     per richiedere di estrarre (GS_CAN) o
                     non estrarre (GS_BAD). Questo parametro verrà 
                     variato dalla funzione
  CString &msg;      messaggiO di richiesta se *Extract = GS_CAN
  C_SELSET &SelSet;  Gruppo di selezione risultante
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_getVirtualPtsOnAge(C_CLASS *pCls, TOPONET_NODE *NodesVett, long iNode, 
                          int *Extract, CString &msg,
                          TOPONET_LINK *LinksVett, double Step, C_POINT_LIST &PtList)
{
   C_LONG_REAL_LIST *pLinkList;
   C_LONG_REAL      *pLink;
   ads_name         ent;
   ads_point        pt;
   double           Angle;
   C_POINT               *pPt;
   TOPONET_H2O_FLOW_INFO *pFlowInfo;
   TOPONET_H2O_NODE_INFO *pNodeInfo;
   C_CLASS               *pSub;
   C_SELSET              SelSet;

   pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[iNode].Punt;

   // cerco il link con nodo finale = al nodo corrente
   pLinkList = (C_LONG_REAL_LIST *) NodesVett[iNode].pLinkList;

   pLink = (C_LONG_REAL *) pLinkList->get_head();
   while (pLink)
   {
      if (LinksVett[pLink->get_id()].FinalNode == iNode) break;
      pLink = (C_LONG_REAL*) pLink->get_next();
   }
   if (!pLink) return GS_BAD;
    
   pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[pLink->get_id()].Punt;
   pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(LinksVett[pLink->get_id()].sub);

   if (hy_extractEnt(pSub, -1 * LinksVett[pLink->get_id()].gs_id, Extract, msg, SelSet) == GS_BAD)
   	return GS_BAD;
   SelSet.intersectType(GRAPHICAL);
   if (SelSet.entname(0, ent) == GS_BAD) return GS_BAD;
   
   // inserisco il punto del nodo virtuale
   if (gsc_getPtOnLinearObj(ent, pFlowInfo->length, pt, &Angle) != GS_GOOD)
      return GS_BAD;
   pt[Z] = pNodeInfo->age;
   if ((pPt = new C_POINT(pt)) == NULL) return GS_BAD;
   PtList.add_tail(pPt);

   // Primo link virtuale 
   pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[LinksVett[pLink->get_id()].InitialNode].Punt;
   if (hy_getPtsOnAge(ent, pNodeInfo->age, pFlowInfo, Step, PtList) == GS_BAD)
      return GS_BAD;

   // Secondo link virtuale
   // cerco il link con nodo iniziale = al nodo corrente
   pLinkList = (C_LONG_REAL_LIST *) NodesVett[iNode].pLinkList;

   pLink = (C_LONG_REAL *) pLinkList->get_head();
   while (pLink)
   {
      if (LinksVett[pLink->get_id()].InitialNode == iNode) break;
      pLink = (C_LONG_REAL*) pLink->get_next();
   }
   if (!pLink) return GS_BAD;
   
   pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[pLink->get_id()].Punt;
   pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[LinksVett[pLink->get_id()].FinalNode].Punt;
   if (hy_getInvertPtsOnAge(ent, pNodeInfo->age, pFlowInfo, Step, PtList) == GS_BAD)
      return GS_BAD;

   return GS_GOOD;
}


/*********************************************************/
/*.doc hy_EvidWaterConc                       <external> */
/*+                                                                       
  Funzione che evidenzia la concentrazione di sostanze reattive nell'acqua
  per le maglie connesse all'oggetto selezionato.
-*/  
/*********************************************************/
void hy_EvidWaterConc(void)
{
	int		       ValveSub, RegSub = -1, PumpSub = -1, TankSub = -1, sub;
   long            startId;
	CString         msg;
   C_CLASS         *pCls;
   C_PROJECT       *pPrj;
   C_TOPOLOGY      Topo;
   long            iNode;
   C_2STR_INT      *pCostSQL;
   C_2STR_INT_LIST CostSQLList;
   C_LONG_LIST     StartNodeList, StoppedNodeList;
   C_LONG          *pNode;
   C_INT_LONG_LIST SupplyConnectedNodes; // lista degli nodi connessi alla rete che la alimentano idraulicamente
   C_INT_LONG      *pItem;
	CHydroMap       simulation;
   double          GeneralKFactor = 0.0; // Sostanze non reattive
	CAcModuleResourceOverride thisResource;

	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}
   pPrj = get_GS_CURRENT_WRK_SESSION()->get_pPrj();

	gsc_disable_reactors();

	TRY
	{
		if (simulation.SelectNetwork(&startId, &sub) != GS_GOOD)
         AfxThrowUserException();

      // cerco la classe madre
      pCls = simulation.pCls;

      if (is_GEOnetworkClass(pCls, H2O) == GS_GOOD) // acqua
      {        
         ValveSub = 5;
         TankSub  = 3;
         PumpSub  = 4;
         RegSub   = 3; // per comodità
      }
      else
      if (is_GEOnetworkClass(pCls, GB) == GS_GOOD ||
          is_GEOnetworkClass(pCls, GM) == GS_GOOD) // gas
      {        
         ValveSub = 4;
         RegSub   = 3;
         TankSub  = 3; // per comodità
      }
      else
         AfxThrowUserException();

      // Chiedo il coefficiente di reazione
      msg.LoadString(HY_STRING255); // "\nCoefficiente di reazione (0 per sostanze non reattive) <0>: "
      acedInitGet(RSG_NONEG, _T(""));
      if (acedGetReal(msg, &GeneralKFactor) == RTCAN) AfxThrowUserException();

      Topo.set_type(TYPE_POLYLINE); // tipologia di tipo rete
      Topo.set_cls(pCls);

      // Ricavo quali sono gli elementi che alimentano la rete connessa
      // all'entità selezionata
      pCostSQL = new C_2STR_INT();
      pCostSQL->set_name(_T("0.0"));      // COSTO SE VALVOLA APERTA
      pCostSQL->set_name2(_T("GS_ID<0")); // condizione mai soddisfatta
      pCostSQL->set_type(ValveSub);       // sub della valvola
      CostSQLList.add_tail(pCostSQL);

      if (Topo.LoadInMemory(&CostSQLList) == GS_BAD) AfxThrowUserException();

      // se si tratta di un link cerco gli elementi connessi al
      // nodo iniziale e poi a quello finale
      if (sub == 2)
      {
         long iLink = gsc_searchTopoNetLink(Topo.LinksVett, Topo.nLinksVett, startId);
         iNode = Topo.LinksVett[iLink].InitialNode;
         Topo.GetNetPropagation(iNode);
         iNode = Topo.LinksVett[iLink].FinalNode;
      }
      else
         iNode = gsc_searchTopoNetNode(Topo.NodesVett, Topo.nNodesVett, startId);

      Topo.GetNetPropagation(iNode);

      // Libero un pò di memoria
      gsc_freeTopoNetNode(&(Topo.NodesVett), &(Topo.nNodesVett));
      gsc_freeTopoNetLink(&(Topo.LinksVett), &(Topo.nLinksVett));
      Topo.ptr_NetLinks()->remove_all();

      // Cancello tutti i nodi lasciando solo quelli che alimentano la rete
      pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_head();
      while (pItem)
      {
         if (pItem->get_key() == TankSub || pItem->get_key() == PumpSub ||
             pItem->get_key() == RegSub)
            SupplyConnectedNodes.add_tail(new C_INT_LONG(pItem->get_key(), pItem->get_id()));

         pItem = (C_INT_LONG *) Topo.ptr_NetNodes()->get_next();
      }

      // Libero un pò di memoria
      Topo.ptr_NetNodes()->remove_all();      

      CostSQLList.remove_all();
      // Condizione di passaggio da nodo in->fi
      pCostSQL = new C_2STR_INT();
      pCostSQL->set_type(2); // sub del link
      pCostSQL->set_name(_T("0.0")); // COSTO SE Link passante
      pCostSQL->set_name2(_T("VERSO_Q_IN='>' AND VERSO_Q_FI='>' AND PORTATA_IN<>0 AND PORTATA_FI<>0")); // condizione di link passante
      CostSQLList.add_tail(pCostSQL);
      // Condizione di passaggio da nodo fi->in
      pCostSQL = new C_2STR_INT();
      pCostSQL->set_type(2); // sub del link
      pCostSQL->set_name(_T("0.0")); // COSTO SE VALVOLA APERTA
      pCostSQL->set_name2(_T("VERSO_Q_IN='<' AND VERSO_Q_FI='<' AND PORTATA_IN<>0 AND PORTATA_FI<>0")); // condizione di link passante
      CostSQLList.add_tail(pCostSQL);

      pCostSQL = new C_2STR_INT();
      pCostSQL->set_name(_T("0.0")); // COSTO SE VALVOLA APERTA

      C_SUB *pValveSub = (C_SUB *) pCls->ptr_sub_list()->search_key(ValveSub);
      C_ATTRIB *pAttrib = (C_ATTRIB *) pValveSub->ptr_attrib_list()->search_name(_T("CHIUSA"), FALSE);
      if (gsc_DBIsBoolean(pAttrib->ADOType) == GS_GOOD)
         pCostSQL->set_name2(_T("CHIUSA=FALSE")); // condizione di valvola aperta
      else
         pCostSQL->set_name2(_T("CHIUSA=0")); // condizione di valvola aperta

      CostSQLList.add_tail(pCostSQL);

      pCostSQL->set_type(ValveSub); // sub della valvola

      if (Topo.LoadInMemory(&CostSQLList) == GS_BAD) AfxThrowUserException();

      // Poichè la simulazione hydromap prevede che nei link il flusso possa
      // essere entrante sia da parte del nodo iniziale che da quello finale
      // (perchè vi è un consumo distribuito nel link) questo genera una
      // eccezione topologica.
      // Per superare l'ostacolo bisogna inserire dei nodi virtuali la cui posizione
      // nel link deve essere in proporzione alla portata_in e alla portata portata_fi
      // La funzione deve quindi modificare la topologia in memoria nei vettori
      // "LinksVett" e "NodesVett".
      if (hy_TopologyCorrectionForFlow(pCls, &(Topo.LinksVett), &(Topo.nLinksVett),
                                       &(Topo.NodesVett), &(Topo.nNodesVett), &GeneralKFactor) == GS_BAD)
     		AfxThrowUserException();

      // Funzione che legge le concentrazioni e i tempi iniziali 
      // delle sostanze immesse dagli
      // elementi che alimentano la rete (serbatoi, pompe)
      if (hy_TopologyUpdForInputNodes(pCls, Topo.NodesVett, Topo.nNodesVett, &GeneralKFactor) == GS_BAD)
     		AfxThrowUserException();

      // Per ciascun componente che alimenta la rete
      // la funzione deve calcolare l'età dell'acqua per tutti i link
      // alimentati esclusivamente da quel componente.
      // All'inizio i componenti che alimentano la rete saranno
      // solo i serbatoi e le pompe. Seguendo la direzione dei flussi
      // si calcola l'età dell'acqua a fine link. Quando si incontra un nodo è possibile
      // passare ai link successivi solo se sono note le età dell'acqua per tutti
      // i link che alimentano il nodo stesso.
      // Una volta terminato, i componenti che alimentano la rete saranno
      // solo i nodi a cui ci si era fermato nei passaggi precedenti.
      // Si procede così finchè sono terminati i nodi che facevano fermare il calcolo

      // anche se non ha senso la prendo in considerazione per i test
      // caso gas
      if (ValveSub == 4)
      {
         for (long i = 0; i < Topo.nNodesVett; i++)
            if (Topo.NodesVett[i].sub == 3 && // regolatore
                SupplyConnectedNodes.search_key(Topo.NodesVett[i].gs_id)) // connesso alla rete selezionata
            {
               if ((pNode = new C_LONG(i)) == NULL)
                  { gs_errmsg(eGSOutOfMem); AfxThrowUserException(); }
               StartNodeList.add_tail(pNode);
            }
      }
      else // caso acqua
      {
         for (long i = 0; i < Topo.nNodesVett; i++)
            if ((Topo.NodesVett[i].sub == 3 || Topo.NodesVett[i].sub == 4) && // serbatoio o pompa
                SupplyConnectedNodes.search_key(Topo.NodesVett[i].gs_id)) // connesso alla rete selezionata
            {
               if ((pNode = new C_LONG(i)) == NULL)
                  { gs_errmsg(eGSOutOfMem); AfxThrowUserException(); }
               StartNodeList.add_tail(pNode);
            }
      }

      Topo.ptr_NetNodes()->remove_all();

      StoppedNodeList.remove_all();
      StartNodeList.sort_id(); // ordino in modo crescente
      pNode = (C_LONG *) StartNodeList.get_head();
      while (pNode)
      {
         while (pNode)
         {
            // calcolo le concentrazioni di sostanze reattive nell'acqua della rete connessa al nodo
            if (hy_GetWaterAgeConc(Topo.LinksVett, Topo.NodesVett, pNode->get_id(),
                                   StoppedNodeList, true) == GS_BAD)
               AfxThrowUserException(); 

            pNode = (C_LONG *) StartNodeList.get_next();
         }
         
         // se StartNodeList = StoppedNodeList allora interrompo perchè
         // le direzioni dei fluidi non consentono di risolvere il comando
         StoppedNodeList.sort_id(); // ordino in modo crescente
         if (StartNodeList == StoppedNodeList)
         {
            msg.LoadString(HY_STRING240); // le direzioni dei fluidi non consentono di risolvere il comando
            acutPrintf(msg);

            msg.LoadString(HY_STRING241); // \nVerificare le seguenti entità:
            acutPrintf(msg);

            msg.LoadString(HY_STRING242); // \nSottoclasse: %d, codice chiave %ld
            pNode = (C_LONG *) StartNodeList.get_head();
            while (pNode)
            {
               acutPrintf(msg,
                          Topo.NodesVett[pNode->get_id()].sub,
                          Topo.NodesVett[pNode->get_id()].gs_id);
               pNode = (C_LONG *) StartNodeList.get_next();
            }

            AfxThrowUserException(); 
         }

         StoppedNodeList.copy(&StartNodeList);
         StoppedNodeList.remove_all();
         pNode = (C_LONG *) StartNodeList.get_head();
      }

      // Disegno le isolinee dell'età dell'acqua
      if (hy_DrawOnConc(pCls, Topo.LinksVett, Topo.nLinksVett,
                        Topo.NodesVett, Topo.nNodesVett, GeneralKFactor) == GS_BAD)
         AfxThrowUserException(); 
	}

   CATCH( CUserException, e )
	{
	}
	END_CATCH

	gsc_enable_reactors();
   
	return;
}


/******************************************************************************/
/*.doc hy_DrawOnConc                                                          */
/*+
  Questa funzione disegna dei punti con la Z rappresentante la concentrazione dell'acqua.
  Vengono considerati tutti i nodi e le polilinee vengono campionate ad intervalli
  di tempo.
  Parametri:
  C_CLASS *pCls;              puntatori alla classe simulazione
  TOPONET_LINK *LinksVett;    Vettori di topologia per i link
  long nLinks;                Lunghezza vettore LinksVett
  TOPONET_NODE *NodesVett;    Vettori di topologia per i nodi
  long nNodes;                Lunghezza vettore NodesVett
  double GeneralKFactor;      Fattore di reazione generale per la rete

  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_DrawOnConc(C_CLASS *pCls, TOPONET_LINK *LinksVett, long nLinks,
                  TOPONET_NODE *NodesVett, long nNodes, double GeneralKFactor)
{
   long                  i, Step, TimeFactor;
   TOPONET_H2O_FLOW_INFO *pFlowInfo;
   TOPONET_H2O_NODE_INFO *pNodeInfo;
   C_CLASS               *pSub;
   C_SELSET              SelSet;
   int                   Extract = GS_CAN;
   C_POINT_LIST          PtList;
   CString               msg, msg1;
   ads_name              ent;
   C_LONG_REAL_LIST      *pLinkList;
   C_LONG_REAL           *pLink;
   double                MaxAge = 0;

   msg.LoadString(HY_STRING243); // "Estrarre tutte le entità con età dell'acqua nota ?"

   // Cerco valore maggiore
   for (i = 1; i < nNodes; i++)
      // Se il nodo ha le informazioni sull'età dell'acqua
      if ((pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[i].Punt) != NULL)
         if (pNodeInfo->age > MaxAge) MaxAge = pNodeInfo->age;

   if (MaxAge < 60) TimeFactor = 1; // Secondi
   else if (MaxAge < 60 * 60) TimeFactor = 60; // Minuti
   else if (MaxAge < 60 * 60 * 24) TimeFactor = 60 * 60; // Ore
   else TimeFactor = 60 * 60 * 24; // Giorni

   // Il passo di campionamento per le tubazioni (secondi) è valutato in base
   // all'unità di misura del tempo e si sceglie l'unità di misura inferiore
   // ad esempio se l'unità di misura fosse minuti il campionamento sarebbe secondi
   if (TimeFactor == 1 || TimeFactor == 60) Step = 1;
   else if (TimeFactor == 60 * 60) Step = 60;
   else if (TimeFactor == 60 * 60 * 24) Step = 60 * 60;
   
   // Nodes
   for (i = 0; i < nNodes; i++)
      // Se il nodo ha le informazioni sull'età dell'acqua
      if ((pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[i].Punt) != NULL)
         if (NodesVett[i].gs_id > 0) // nodo normale
         {
            pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(NodesVett[i].sub);

            if (hy_extractEnt(pSub, NodesVett[i].gs_id, &Extract, msg, SelSet) == GS_BAD)
   		      return GS_BAD;

            SelSet.intersectType(GRAPHICAL);
            if (SelSet.entname(0, ent) == GS_GOOD)
               if (hy_getPtsOnConc(ent, pNodeInfo, PtList) == GS_BAD)
                  return GS_BAD;
         }
         else // nodo virtuale
            // La funzione tratta il nodo virtuale e i due link virtuali
            // ad esso collegati
            if (hy_getVirtualPtsOnConc(pCls, NodesVett, i, &Extract, msg,
                                       LinksVett, Step, PtList) == GS_BAD)
               return GS_BAD;

   // Links
   for (i = 0; i < nNodes; i++)
   {
      // Se il nodo ha le informazioni sull'età dell'acqua
      if ((pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[i].Punt) == NULL)
         continue;
      // ciclo su tutti i link in uscita dal nodo in questione
      if (!(pLinkList = (C_LONG_REAL_LIST *) NodesVett[i].pLinkList))
         continue;
      pLink = (C_LONG_REAL *) pLinkList->get_head();
      while (pLink)
      {
         if (LinksVett[pLink->get_id()].gs_id < 0) // salto i link virtuali
            { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
         
         // se il flusso è in uscita dal nodo

         // Se si sta attraversando il link dal nodo iniziale a quello finale
         if (LinksVett[pLink->get_id()].InitialNode == i)
         {
            // Se il costo per passare sul link è il limite massimo di un double
            // significa che il link non esce dal nodo
            if (LinksVett[pLink->get_id()].In2Fi_Cost == (std::numeric_limits<double>::max)())
               { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
            pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[pLink->get_id()].Punt;
            pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(LinksVett[pLink->get_id()].sub);

            if (hy_extractEnt(pSub, LinksVett[pLink->get_id()].gs_id, &Extract, msg, SelSet) == GS_BAD)
   		      return GS_BAD;

            SelSet.intersectType(GRAPHICAL);
            if (SelSet.entname(0, ent) == GS_GOOD)
               if (hy_getPtsOnConc(ent, pNodeInfo->age, pNodeInfo->concentr,
                                   pFlowInfo, Step, PtList) == GS_BAD)
                  return GS_BAD;
         }
         else // altrimenti si sta attraversando il link dal nodo finale a quello iniziale
         {
            // Se il costo per passare sul link è il limite massimo di un double
            // significa che il link non esce dal nodo
            if (LinksVett[pLink->get_id()].Fi2In_Cost == (std::numeric_limits<double>::max)())
               { pLink = (C_LONG_REAL*) pLink->get_next(); continue; }
            pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[pLink->get_id()].Punt;
            pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(LinksVett[pLink->get_id()].sub);

            if (hy_extractEnt(pSub, LinksVett[pLink->get_id()].gs_id, &Extract, msg, SelSet) == GS_BAD)
   		      return GS_BAD;

            SelSet.intersectType(GRAPHICAL);
            if (SelSet.entname(0, ent) == GS_GOOD)
               if (hy_getInvertPtsOnConc(ent, pNodeInfo->age, pNodeInfo->concentr,
                                         pFlowInfo, Step, PtList) == GS_BAD)
                  return GS_BAD;
         }

         pLink = (C_LONG_REAL*) pLink->get_next();
      }
   }

   C_STRING Layer;

   if (GeneralKFactor != 0)
   {
      if (gsc_get_tmp_layer(_T("Reactive Substance Concentration Points "), Layer) == GS_BAD) return GS_BAD;
   }
   else
      if (gsc_get_tmp_layer(_T("Non-Reactive Substance Concentration Points "), Layer) == GS_BAD) return GS_BAD;

   msg.LoadString(HY_STRING254); // "\nDisegno punti con quota uguale alla concentrazione."
   acutPrintf(msg);

   msg.LoadString(HY_STRING245); // "\nI punti saranno disegnati sul layer <%s>..."
   acutPrintf(msg, Layer.get_name());

   PtList.Draw(Layer.get_name());

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_getPtsOnConc                                                        */
/*+
  Questa funzione ottiene una lista di punti di una polilinea
  in cui la coordinata Z si riferisce alla concentrazione (mg/l) di
  sostanze nell'acqua.
  Verranno letti tutti i vertici esclusi quello iniziale
  e finale con un passo di tempo noto (es. ogni 5 min) partendo
  dal punto iniziale della polilinea verso quello finale
  (punti geometrici).
  Parametri:
  ads_name ent;                     Oggetto grafico polilinea
  double InitAge;                   Età a inizio link
  double InitConc;                  Concentrazione a inizio link
  TOPONET_H2O_FLOW_INFO *pFlowInfo; Informazioni addizionali sul flusso
  double Step;                      Passo di campionamento in secondi
  C_POINT_LIST &PtList;             Lista dei punti notevoli (out)
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_getPtsOnConc(ads_name ent, double InitAge, double InitConc,
                    TOPONET_H2O_FLOW_INFO *pFlowInfo, double Step,
                    C_POINT_LIST &PtList)
{
   C_POINT   *pPt;
   double    Age, Angle, offset;
   ads_point pt;

   Age = Step - fmod(InitAge, Step);

   offset = pFlowInfo->speed * Age;
   while (offset < pFlowInfo->length &&
          gsc_getPtOnLinearObj(ent, offset, pt, &Angle) == GS_GOOD)
   {
      pt[Z] = InitConc * exp(-1 * pFlowInfo->KFactor * Age);
      if ((pPt = new C_POINT(pt)) == NULL) return GS_BAD;
      PtList.add_tail(pPt);

      Age += Step;
      offset = pFlowInfo->speed * Age;
   }

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_getInvertPtsOnConc                                                  */
/*+
  Questa funzione ottiene una lista di punti di una polilinea
  in cui la coordinata Z si riferisce alla concentrazione (mg/l) di
  sostanze nell'acqua.
  Verranno letti tutti i vertici esclusi quello iniziale
  e finale con un passo di tempo noto (es. ogni 5 min) partendo
  dal punto finale della polilinea verso quello iniziale
  (punti geometrici).
  Parametri:
  ads_name ent;                     Oggetto grafico polilinea
  double InitAge;                   Età a inizio link
  double InitConc;                  Concentrazione a inizio link
  TOPONET_H2O_FLOW_INFO *pFlowInfo; Informazioni addizionali sul flusso
  double Step;                      Passo di campionamento in secondi
  C_POINT_LIST &PtList;             Lista dei punti notevoli (out)
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_getInvertPtsOnConc(ads_name ent, double InitAge, double InitConc,
                          TOPONET_H2O_FLOW_INFO *pFlowInfo, double Step,
                          C_POINT_LIST &PtList)
{
   C_POINT   *pPt;
   double    Age, Angle, offset, Len;
   ads_point pt;
   presbuf   pRb;

   // Calcolo la lunghezza della polilinea originale
   if (!(pRb = gsc_get_graphical_data(ent, GS_LISP_LENGTH)) ||
       gsc_rb2Dbl(pRb, &Len) == GS_BAD)
      return GS_BAD;

   Age = Step - fmod(InitAge, Step);

   offset = pFlowInfo->speed * Age;
   while (offset < pFlowInfo->length &&
          gsc_getPtOnLinearObj(ent, Len - offset, pt, &Angle) == GS_GOOD)
   {
      pt[Z] = InitConc * exp(-1 * pFlowInfo->KFactor * Age);
      if ((pPt = new C_POINT(pt)) == NULL) return GS_BAD;
      PtList.add_tail(pPt);

      Age += Step;
      offset = pFlowInfo->speed * Age;
   }

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_getPtsOnConc                                                        */
/*+
  Questa funzione ottiene una lista di punti di un blocco 
  in cui la coordinata Z si riferisce alla concentrazione (mg/l) di
  sostanze nell'acqua.
  Verrà letto il punto di inserimento.
  Parametri:
  ads_name ent;                     Oggetto grafico
  TOPONET_H2O_NODE_INFO *pNodeInfo;
  C_POINT_LIST &PtList;             Lista dei punti notevoli (out)
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_getPtsOnConc(ads_name ent, TOPONET_H2O_NODE_INFO *pNodeInfo,
                    C_POINT_LIST &PtList)
{
   C_POINT   *pPt;
   ads_point pt;

   if (gsc_get_firstPoint(ent, pt) == GS_BAD) return GS_BAD;
   pt[Z] = pNodeInfo->concentr;
   if ((pPt = new C_POINT(pt)) == NULL) return GS_BAD;
   PtList.add_tail(pPt);

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_getVirtualPtsOnConc                                                 */
/*+
  Questa funzione ha lo scopo di ottenere un gruppo di selezione degli 
  oggetti di un'entità GEOsim. A tale scopo, se necessario, 
  chiede o estrae direttamente (a seconda del parametro Extract) 
  tutti gli oggetti di un'entità.
  Parametri:
  C_CLASS *pCls;
  TOPONET_NODE *NodesVett;
  long iNode;
  int *Extract;            Flag per estrarre direttamente (GS_GOOD)
                           per richiedere di estrarre (GS_CAN) o
                           non estrarre (GS_BAD). Questo parametro verrà 
                           variato dalla funzione
  CString &msg;            messaggiO di richiesta se *Extract = GS_CAN
  TOPONET_LINK *LinksVett;
  double Step;
  C_POINT_LIST &PtList;
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_getVirtualPtsOnConc(C_CLASS *pCls, TOPONET_NODE *NodesVett, long iNode, 
                           int *Extract, CString &msg,
                           TOPONET_LINK *LinksVett, double Step, C_POINT_LIST &PtList)
{
   C_LONG_REAL_LIST *pLinkList;
   C_LONG_REAL      *pLink;
   ads_name         ent;
   ads_point        pt;
   double           Angle;
   C_POINT               *pPt;
   TOPONET_H2O_FLOW_INFO *pFlowInfo;
   TOPONET_H2O_NODE_INFO *pNodeInfo;
   C_CLASS               *pSub;
   C_SELSET              SelSet;

   pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[iNode].Punt;

   // cerco il link con nodo finale = al nodo corrente
   pLinkList = (C_LONG_REAL_LIST *) NodesVett[iNode].pLinkList;

   pLink = (C_LONG_REAL *) pLinkList->get_head();
   while (pLink)
   {
      if (LinksVett[pLink->get_id()].FinalNode == iNode) break;
      pLink = (C_LONG_REAL*) pLink->get_next();
   }
   if (!pLink) return GS_BAD;
    
   pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[pLink->get_id()].Punt;
   pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(LinksVett[pLink->get_id()].sub);

   if (hy_extractEnt(pSub, -1 * LinksVett[pLink->get_id()].gs_id, Extract, msg, SelSet) == GS_BAD)
   	return GS_BAD;
   SelSet.intersectType(GRAPHICAL);
   if (SelSet.entname(0, ent) == GS_BAD) return GS_BAD;
   
   // inserisco il punto del nodo virtuale
   if (gsc_getPtOnLinearObj(ent, pFlowInfo->length, pt, &Angle) != GS_GOOD)
      return GS_BAD;
   pt[Z] = pNodeInfo->concentr;
   if ((pPt = new C_POINT(pt)) == NULL) return GS_BAD;
   PtList.add_tail(pPt);

   // Primo link virtuale 
   pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[LinksVett[pLink->get_id()].InitialNode].Punt;
   if (hy_getPtsOnConc(ent, pNodeInfo->age, pNodeInfo->concentr,
                       pFlowInfo, Step, PtList) == GS_BAD)
      return GS_BAD;

   // Secondo link virtuale
   // cerco il link con nodo iniziale = al nodo corrente
   pLinkList = (C_LONG_REAL_LIST *) NodesVett[iNode].pLinkList;

   pLink = (C_LONG_REAL *) pLinkList->get_head();
   while (pLink)
   {
      if (LinksVett[pLink->get_id()].InitialNode == iNode) break;
      pLink = (C_LONG_REAL*) pLink->get_next();
   }
   if (!pLink) return GS_BAD;
   
   pFlowInfo = (TOPONET_H2O_FLOW_INFO *) LinksVett[pLink->get_id()].Punt;
   pNodeInfo = (TOPONET_H2O_NODE_INFO *) NodesVett[LinksVett[pLink->get_id()].FinalNode].Punt;
   if (hy_getInvertPtsOnConc(ent, pNodeInfo->age, pNodeInfo->concentr,
                             pFlowInfo, Step, PtList) == GS_BAD)
      return GS_BAD;

   return GS_GOOD;
}


/******************************************************************************/
/*.doc hy_TopologyUpdForInputNodes                                            */
/*+
  Funzione che legge le concentrazioni delle sostanze immesse dagli
  elementi che alimentano la rete (serbatoi, pompe).
  Parametri:
  C_CLASS *pCls;
  TOPONET_NODE *NodesVett;
  long nNodes;
  bool OnlyAgeCheck;          Flag, se vero controlla e inizializza
                              solo l'attributo <tempo_iniziale> altrimenti
                              controlla e inizializza anche l'attributo
                              <concentrazione> per pompe e serbatoi
  double GeneralKFactor;      Coefficiente di reazione generico per tutta la rete,
                              usato per caricare oltre il <tempo_iniziale>
                              anche la <concentrazione> e il <fattore di reazione>.
                              Usato nel caso in cui serbatoi o pompe non lo
                              avessero già inizializzato (default = NULL)
  
  Restituisce GS_GOOD in caso di successo altrimenti GS_BAD.
-*/  
/******************************************************************************/
int hy_TopologyUpdForInputNodes(C_CLASS *pCls, TOPONET_NODE *NodesVett, long nNodes,
                                double *GeneralKFactor)
{
   C_SUB     *pSub;
   C_RB_LIST ColValues;
   presbuf   pRb;
   CString   msg;
   double    Concentration, KFactor;
   long      Time;

   msg.LoadString(HY_STRING258); // \nCaricamento informazioni relative ai nodi di rete...
   acutPrintf(msg);

   for (long i = 0; i < nNodes; i++)
      if (NodesVett[i].sub == 3 || // serbatoio
          NodesVett[i].sub == 4) // pompe
      {
         pSub = (C_SUB *) pCls->ptr_sub_list()->search_key(NodesVett[i].sub);
         ColValues.remove_all();
         if (pSub->query_data(NodesVett[i].gs_id, ColValues) == GS_BAD)
            return GS_BAD;

         if (!(pRb = ColValues.CdrAssoc(_T("TEMPO_INIZIALE"), FALSE)))
         {
            if (!GeneralKFactor)
               // \nLa struttura della simulazione deve essere modificata:
               // \ni serbatoi e le pompe devono avere l'attributo
               // <TEMPO_INIZIALE> (vedi manuale).
               msg.LoadString(HY_STRING253);
            else
               if (*GeneralKFactor == 0)
                  // \nLa struttura della simulazione deve essere modificata:
                  // \ni serbatoi e le pompe devono avere gli attributi 
                  // <TEMPO_INIZIALE> e <CONCENTRAZIONE> (vedi manuale).
                  msg.LoadString(HY_STRING257);
               else
                  // \nLa struttura della simulazione deve essere modificata:
                  // \ni serbatoi e le pompe devono avere gli attributi 
                  // <TEMPO_INIZIALE>, <CONCENTRAZIONE> e <FATTORE_REAZIONE> (vedi manuale).
                  msg.LoadString(HY_STRING252);

            acutPrintf(msg);
            return GS_BAD;
         }
         if (gsc_rb2Lng(pRb, &Time) == GS_BAD) Time = 0;

         // memorizzo l'età dell'acqua nel nodo
         NodesVett[i].visited = true;
         if ((NodesVett[i].Punt = (TOPONET_H2O_NODE_INFO *) malloc(sizeof(TOPONET_H2O_NODE_INFO))) == NULL)
            return GS_BAD;
         ((TOPONET_H2O_NODE_INFO *) NodesVett[i].Punt)->age = Time;

         if (GeneralKFactor) // verifico anche l'attributo <concentrazione>
         {
            if (!(pRb = ColValues.CdrAssoc(_T("CONCENTRAZIONE"), FALSE)))
            {
               if (*GeneralKFactor == 0)
                  // \nLa struttura della simulazione deve essere modificata:
                  // \ni serbatoi e le pompe devono avere gli attributi 
                  // <TEMPO_INIZIALE> e <CONCENTRAZIONE> (vedi manuale).
                  msg.LoadString(HY_STRING257);
               else
                  // \nLa struttura della simulazione deve essere modificata:
                  // \ni serbatoi e le pompe devono avere gli attributi 
                  // <TEMPO_INIZIALE>, <CONCENTRAZIONE> e <FATTORE_REAZIONE> (vedi manuale).
                  msg.LoadString(HY_STRING252);

               acutPrintf(msg);
               return GS_BAD;
            }
            if (gsc_rb2Dbl(pRb, &Concentration) == GS_BAD) Concentration = 0.0;

            // si vuole modellare una sostanza non-reattiva
            if (*GeneralKFactor == 0) KFactor = 0;
            else
            {   // verifico anche l'attributo <fattore_reazione>
               if (!(pRb = ColValues.CdrAssoc(_T("FATTORE_REAZIONE"), FALSE)))
               {
                  // \nLa struttura della simulazione deve essere modificata:
                  // \ni serbatoi e le pompe devono avere gli attributi 
                  // <TEMPO_INIZIALE>, <CONCENTRAZIONE> e <FATTORE_REAZIONE> (vedi manuale).
                  msg.LoadString(HY_STRING252);
                  acutPrintf(msg);
                  return GS_BAD;
               }

               if (gsc_rb2Dbl(pRb, &KFactor) == GS_BAD) KFactor = *GeneralKFactor;
            }

            // La concentrazione a fine lato è data dalla formula:
            // C sample = C init * exp(-K * T)
            // dove
            // T = tempo di viaggio dell'acqua tra la
            //     sorgente e il punto di campionamento
            // C sample = concentrazione misurata nel punto di
            //             campionamento dopo il tempo T 
            // C init = concentrazione iniziale misurata alla sorgente
            // K = coefficiente di reazione
            Concentration = Concentration * exp(-1 * KFactor * Time);

            // memorizzo la concentrazione nel nodo
            ((TOPONET_H2O_NODE_INFO *) NodesVett[i].Punt)->concentr = Concentration;
         }
      }

   return GS_GOOD;
}


/////////////////////////////////////////////////////////
// finestra di dialogo CHy_connect

IMPLEMENT_DYNAMIC(CHy_connect, CDialog)
CHy_connect::CHy_connect(CWnd* pParent /*=NULL*/)
	: CDialog(CHy_connect::IDD, pParent)
{
   DisplayFlowCtrls = NO_FLOW_CONTROL + DIRECT_FLOW + INVERSE_FLOW; // visualizza i controlli di flusso
   FlowControl      = NO_FLOW_CONTROL; // nessuno controllo di flusso
   ValveControl     = true; // controllo stato valvole
   HelpIndexPage    = 0;
}

CHy_connect::~CHy_connect()
{
}

BOOL CHy_connect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   if (DisplayFlowCtrls == 0) // Nascondi i controlli di flusso
   {
      CRect rcItem;
      int   OffSetY, BorderHeight;

	   GetDlgItem(IDC_FLOWBORDER)->GetWindowRect(rcItem);
      BorderHeight = rcItem.Height();
	   GetDlgItem(IDC_FLOWBORDER)->ShowWindow(SW_HIDE);
      m_NoFlowRadio.ShowWindow(SW_HIDE);
      m_FlowRadio.ShowWindow(SW_HIDE);
      m_InverseFlowRadio.ShowWindow(SW_HIDE);

      // riposiziono gli altri controlli
      m_ValveCheck.GetWindowRect(rcItem);
      OffSetY = rcItem.top;
	   m_ValveCheck.SetWindowPos(NULL, rcItem.left, 15, 0, 0, SWP_NOSIZE);

	   GetDlgItem(IDC_FLOWHELP)->GetWindowRect(rcItem);
      OffSetY = rcItem.top - OffSetY;
	   GetDlgItem(IDC_FLOWHELP)->SetWindowPos(NULL, rcItem.left, 15 + OffSetY, 0, 0, SWP_NOSIZE);
	   GetDlgItem(IDCANCEL)->GetWindowRect(rcItem);
	   GetDlgItem(IDCANCEL)->SetWindowPos(NULL, rcItem.left, 15 + OffSetY, 0, 0, SWP_NOSIZE);
	   GetDlgItem(IDOK)->GetWindowRect(rcItem);
	   GetDlgItem(IDOK)->SetWindowPos(NULL, rcItem.left, 15 + OffSetY, 0, 0, SWP_NOSIZE);
      
      GetWindowRect(rcItem);
	   SetWindowPos(NULL, 0, 0, rcItem.Width(), rcItem.Height() - BorderHeight, SWP_NOMOVE);
   }
   else
   {
      m_NoFlowRadio.EnableWindow((DisplayFlowCtrls & NO_FLOW_CONTROL) ? TRUE : FALSE);
      m_FlowRadio.EnableWindow((DisplayFlowCtrls & DIRECT_FLOW) ? TRUE : FALSE);
      m_InverseFlowRadio.EnableWindow((DisplayFlowCtrls & INVERSE_FLOW) ? TRUE : FALSE);

      switch (FlowControl)
      {
         case NO_FLOW_CONTROL:
            OnBnClickedNoFlowRadio();
            break;
         case DIRECT_FLOW:
            OnBnClickedFlowRadio();
            break;
         case INVERSE_FLOW:
            OnBnClickedInverseFlowRadio();
            break;
      }
   }

   m_ValveCheck.SetCheck((ValveControl) ? 1 : 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHy_connect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_NOFLOWRADIO, m_NoFlowRadio);
	DDX_Control(pDX, IDC_FLOWRADIO, m_FlowRadio);
	DDX_Control(pDX, IDC_INVERSEFLOWRADIO, m_InverseFlowRadio);
	DDX_Control(pDX, IDC_VALVECHECK, m_ValveCheck);
}


BEGIN_MESSAGE_MAP(CHy_connect, CDialog)   
   ON_BN_CLICKED(IDC_NOFLOWRADIO, OnBnClickedNoFlowRadio)
   ON_BN_CLICKED(IDC_FLOWRADIO, OnBnClickedFlowRadio)
   ON_BN_CLICKED(IDC_INVERSEFLOWRADIO, OnBnClickedInverseFlowRadio)
   ON_BN_CLICKED(IDC_VALVECHECK, OnBnClickedValveCheck)
   ON_BN_CLICKED(IDC_FLOWHELP, OnBnClickedHelp)      
END_MESSAGE_MAP()


// gestori di messaggi CHy_connect
void CHy_connect::OnBnClickedNoFlowRadio()
{
   FlowControl = NO_FLOW_CONTROL;
   if (m_NoFlowRadio.GetCheck() == 0) m_NoFlowRadio.SetCheck(1);
   m_FlowRadio.SetCheck(0);
   m_InverseFlowRadio.SetCheck(0);
}

void CHy_connect::OnBnClickedFlowRadio()
{
   FlowControl = DIRECT_FLOW;
   m_NoFlowRadio.SetCheck(0);
   if (m_FlowRadio.GetCheck() == 0) m_FlowRadio.SetCheck(1);
   m_InverseFlowRadio.SetCheck(0);
}

void CHy_connect::OnBnClickedInverseFlowRadio()
{
   FlowControl = INVERSE_FLOW;
   m_NoFlowRadio.SetCheck(0);
   m_FlowRadio.SetCheck(0);
   if (m_InverseFlowRadio.GetCheck() == 0) m_InverseFlowRadio.SetCheck(1);
}


void CHy_connect::OnBnClickedValveCheck()
{
   ValveControl = (m_ValveCheck.GetCheck() == 0) ? false : true;
}


void CHy_connect::OnBnClickedHelp()
{
   CallHelp(m_hWnd, HelpIndexPage);
}


/*********************************************************/
/*.doc hy_EvidNets                            <external> */
/*+                                                                       
  Funzione che evidenzia le reti di una classe simulazione
  considerando, opzionalmente, lo stato delle valvole.
-*/  
/*********************************************************/
void hy_EvidNets(void)
{
	// When resource from this ARX app is needed, just
	// instantiate a local CTemporaryResourceOverride
	CAcModuleResourceOverride thisResource;

   int		       ret = GS_GOOD, ValveSub, IdsVettLen = 0;
   C_CLASS         *pCls;
   C_PROJECT       *pPrj;
   C_TOPOLOGY      Topo;
   long            iNode, iLink;
   C_INT_LONG      *pItem;
   C_2STR_INT      *pCostSQL;
   C_2STR_INT_LIST CostSQLList;
	CHydroMap       simulation;
	CHy_connect	    ConnectDialog;
   bool            Stop;
   C_LONG          *p_nID;
   C_LONG_LIST     nNodeList;
   C_INT_LONG_LIST *IdsVett[MAX_NETS]; // Si gestiscono al max MAX_NETS reti
   C_LONG_BTREE    ListToVisitedInLinksVett;
   C_BLONG         *pToVisitedInLinksVett;

	if (get_GS_CURRENT_WRK_SESSION() == NULL)
	{ 
		gs_errmsg(eGSNotCurrentSession);
		return; 
	}
   pPrj = get_GS_CURRENT_WRK_SESSION()->get_pPrj();

   // Inizializzo il vettore
   for (iLink = 0; iLink < MAX_NETS; iLink++) IdsVett[iLink] = NULL;

	gsc_disable_reactors();

	TRY
	{
		if (simulation.SelectNetwork() != GS_GOOD)
         AfxThrowUserException();

      // cerco la classe madre
      pCls = simulation.pCls;
      if (is_GEOnetworkClass(pCls, GB) == GS_GOOD ||
          is_GEOnetworkClass(pCls, GM) == GS_GOOD) // gas
         // gas
         ValveSub = 4;
      else
      if (is_GEOnetworkClass(pCls, H2O) == GS_GOOD) // acqua
         ValveSub = 5;
      else
  	      AfxThrowUserException();

      // Non visualizzo i controlli sui flussi
      ConnectDialog.DisplayFlowCtrls = 0;
      // Imposto come default di non verificare lo stato delle valvole
      ConnectDialog.ValveControl = false;
      ConnectDialog.HelpIndexPage = IDH_Elencodellesottoreti;
		if (ConnectDialog.DoModal() != IDOK)
         AfxThrowUserException();

      if (ConnectDialog.ValveControl)
      {
         pCostSQL = new C_2STR_INT();
         pCostSQL->set_name(_T("0.0")); // COSTO SE VALVOLA APERTA

         C_SUB *pValveSub = (C_SUB *) pCls->ptr_sub_list()->search_key(ValveSub);
         C_ATTRIB *pAttrib = (C_ATTRIB *) pValveSub->ptr_attrib_list()->search_name(_T("CHIUSA"), FALSE);
         if (gsc_DBIsBoolean(pAttrib->ADOType) == GS_GOOD)
            pCostSQL->set_name2(_T("CHIUSA=FALSE")); // condizione di valvola aperta
         else
            pCostSQL->set_name2(_T("CHIUSA=0")); // condizione di valvola aperta

         pCostSQL->set_type(ValveSub); // sub della valvola
         CostSQLList.add_tail(pCostSQL);
      }

      Topo.set_type(TYPE_POLYLINE); // tipologia di tipo rete
      Topo.set_cls(pCls);
      if (Topo.LoadInMemory(&CostSQLList) == GS_BAD) AfxThrowUserException();

      // segno tutti i link come ancora da visitare
      for (iLink = 0; iLink < Topo.nLinksVett; iLink++)
         if (!Topo.LinksVett[iLink].visited)
            ListToVisitedInLinksVett.add(&iLink);

      // cerco il primo link non ancora visitato
      if ((pToVisitedInLinksVett = (C_BLONG *) ListToVisitedInLinksVett.go_top()))
      {
         Stop = false;
         iLink = pToVisitedInLinksVett->get_key();
      }
      else
         Stop = true;

      //Stop = true;
      //for (iLink = 0; iLink < Topo.nLinksVett; iLink++)
      //   if (!Topo.LinksVett[iLink].visited) { Stop = false; break; }

      while (!Stop && IdsVettLen < MAX_NETS)
      {
         Topo.InitForNetAnalysis();

         // Verifico la rete connessa al nodo iniziale
         iNode = Topo.LinksVett[iLink].InitialNode;
         Topo.GetNetPropagation(iNode);

         // Verifico la rete connessa al nodo finale
         iNode = Topo.LinksVett[iLink].FinalNode;
         Topo.GetNetPropagation(iNode);

         // Se non è stato inserito da GetNetConnected inserisco il link di partenza
         if (!Topo.ptr_NetLinks()->search_key(Topo.LinksVett[iLink].gs_id))
         {
            pItem = new C_INT_LONG(2, Topo.LinksVett[iLink].gs_id);
            Topo.ptr_NetLinks()->add_tail(pItem);
         }

         // Se non è stato inserito il nodo iniziale (es. valvola chiusa)
         // inserisco il codice del nodo iniziale
         if (!Topo.ptr_NetNodes()->search_key(Topo.NodesVett[Topo.LinksVett[iLink].InitialNode].gs_id))
         {
            pItem = new C_INT_LONG(Topo.NodesVett[Topo.LinksVett[iLink].InitialNode].sub,
                                   Topo.NodesVett[Topo.LinksVett[iLink].InitialNode].gs_id);
            Topo.ptr_NetNodes()->add_tail(pItem);
         }
         // Se non è stato inserito il nodo finale (es. valvola chiusa)
         // inserisco il codice del nodo finale
         if (!Topo.ptr_NetNodes()->search_key(Topo.NodesVett[Topo.LinksVett[iLink].FinalNode].gs_id))
         {
            pItem = new C_INT_LONG(Topo.NodesVett[Topo.LinksVett[iLink].FinalNode].sub, 
                                   Topo.NodesVett[Topo.LinksVett[iLink].FinalNode].gs_id);
            Topo.ptr_NetNodes()->add_tail(pItem);
         }

         // memorizzo il numero di nodes
         if ((p_nID = new C_LONG(Topo.ptr_NetNodes()->get_count())) == NULL)
            { gs_errmsg(eGSOutOfMem); AfxThrowUserException(); }
         nNodeList.add_tail(p_nID);

         // memorizzo la lista degli ID dei link
         if ((IdsVett[IdsVettLen] = new C_INT_LONG_LIST()) == NULL)
            { gs_errmsg(eGSOutOfMem); AfxThrowUserException(); }
         IdsVett[IdsVettLen]->paste_head(*Topo.ptr_NetLinks()); 

         //// cerco il primo link non ancora visitato
         //Stop = true;
         //for (iLink = 0; iLink < Topo.nLinksVett; iLink++)
         //   if (!Topo.LinksVett[iLink].visited) { Stop = false; break; }

         // tolgo i link già visitati dalla lista dei link da visitare
         for (iLink = 0; iLink < Topo.nLinksVett; iLink++)
            if (Topo.LinksVett[iLink].visited)
               ListToVisitedInLinksVett.remove(&iLink);

         // cerco il primo link non ancora visitato
         if ((pToVisitedInLinksVett = (C_BLONG *) ListToVisitedInLinksVett.go_top()))
         {
            Stop = false;
            iLink = pToVisitedInLinksVett->get_key();
         }
         else
            Stop = true;

         IdsVettLen++;
      }

      // Libero un pò di memoria
      gsc_freeTopoNetNode(&(Topo.NodesVett), &(Topo.nNodesVett));
      gsc_freeTopoNetLink(&(Topo.LinksVett), &(Topo.nLinksVett));

      // evidenzio le reti
      hy_NetDisplayManager(pCls, IdsVett, IdsVettLen, nNodeList);
	}

   for (iLink = 0; iLink < MAX_NETS; iLink++)
      if (IdsVett[iLink]) delete IdsVett[iLink];

   CATCH( CUserException, e )
	{
      for (iLink = 0; iLink < MAX_NETS; iLink++)
         if (IdsVett[iLink]) delete IdsVett[iLink];

		ret = GS_BAD;      
	}
	END_CATCH

	gsc_enable_reactors();
   
	return;
}


/*********************************************************/
/*.doc hy_NetDisplayManager                   <internal> */
/*+                                                                       
  Funzione che gestisce l'interfaccia grafica per evidenziare
  le reti di una classe simulazione trovate dalla <hy_EvidNets>
  Parametri:
  C_CLASS *pCls;              Puntatore a classe simulazione
  C_INT_LONG_LIST *IdsVett;   Lista dei codici dei link presenti in ciascuna rete
  int IdsVettLen;             Lunghezza del vettore dei codici dei link
  C_LONG_LIST &nNodeList;     Lista dei numeri di nodi presenti in ciascuna rete

  Non ritorna valori.
-*/  
/*********************************************************/
void hy_NetDisplayManager(C_CLASS *pCls, C_INT_LONG_LIST *IdsVett[], int IdsVettLen, 
                          C_LONG_LIST &nNodeList)
{
	CAcModuleResourceOverride thisResource;
   INT_PTR    ret;
   int        Extract;
   C_INT_LONG *pItem;
   C_CLASS    *pSub;
   CString    msg;
   C_SELSET   SelSet;

   msg.LoadString(HY_STRING238); // "Estrarre tutte le entità connesse ?"

   CHy_evid_nets NetsDialog(pCls, IdsVett, IdsVettLen, &nNodeList);

   do
   {
   	if ((ret = NetsDialog.DoModal()) == IDEVIDNET)
      {
         HyAppl::GS_LS.clear();
         HyAppl::GS_LS.cls = pCls->ptr_id()->code;
         HyAppl::GS_LS.sub = 2;
         
         Extract = GS_CAN;
         
         pItem = (C_INT_LONG *) NetsDialog.pCurrIds->get_head();
         while (pItem)
         {
            pSub = (C_CLASS *) pCls->ptr_sub_list()->search_key(pItem->get_key());

            if (hy_extractEnt(pSub, pItem->get_id(), &Extract, msg, SelSet) == GS_BAD)
   		      AfxThrowUserException();
            HyAppl::GS_LS.ptr_SelSet()->add_selset(SelSet);

            pItem = (C_INT_LONG *) pItem->get_next();
         }

         hy_PutSym_hyss(HyAppl::GS_LS);
         // evidenzio i links
         hy_sim_evid();

         NetsDialog.CurrSel++;
      }
   }
   while (ret == IDEVIDNET);
}


// finestra di dialogo CHy_evid_nets

/*********************************************************/
// Funzione che fa la comparazione tra 2 valori
// per l'ordinamento delle righe della listctrl con i dati
// delle entità visualizzati in orizzontale 
/*********************************************************/
static int CALLBACK NetListDataCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   CHy_evid_nets *pCHy_evid_nets = (CHy_evid_nets *) lParamSort;
   long n1, n2;

   if (pCHy_evid_nets->Prev_iHeader == 1) // nlinks
   {
      n1 = pCHy_evid_nets->m_pIdsVett[lParam1]->get_count();
      n2 = pCHy_evid_nets->m_pIdsVett[lParam2]->get_count();
   }
   else // nNodes
   {
      C_LONG *pNode = (C_LONG *) pCHy_evid_nets->m_pNodeList->getptr_at((int) lParam1 + 1);

      n1 = pNode->get_id();
      pNode = (C_LONG *) pCHy_evid_nets->m_pNodeList->getptr_at((int) lParam2 + 1);
      n2 = pNode->get_id();
   }

   if (pCHy_evid_nets->AscendingOrder)
      return (n1 > n2) ? 1 : ((n1 == n2) ? 0 : -1); // ordine crescente
   else 
      return (n1 < n2) ? 1 : ((n1 == n2) ? 0 : -1); // ordine decrescente
}

IMPLEMENT_DYNAMIC(CHy_evid_nets, CDialog)
CHy_evid_nets::CHy_evid_nets(CWnd* pParent /*=NULL*/)
	: CDialog(CHy_evid_nets::IDD, pParent)
{
   m_pCls       = NULL;
   m_pIdsVett   = NULL;
   m_IdsVettLen = 0;
   m_pNodeList  = NULL;
   Prev_iHeader  = -1;
}

CHy_evid_nets::CHy_evid_nets(C_CLASS *_pCls,
                             C_INT_LONG_LIST **_pIdsVett, int _IdsVettLen,
                             C_LONG_LIST *_pNodeList, CWnd* pParent /*=NULL*/)
	: CDialog(CHy_evid_nets::IDD, pParent)
{
   m_pCls       = _pCls;
   m_pIdsVett   = _pIdsVett;
   m_IdsVettLen = _IdsVettLen;
   m_pNodeList  = _pNodeList;

   CurrSel       = 0;
   OrderedColumn = 0;
   Prev_iHeader  = -1;

   Top  = -1;
   Left = -1;
}

CHy_evid_nets::~CHy_evid_nets()
{
}

BOOL CHy_evid_nets::OnInitDialog() 
{
   LVCOLUMN col;
   CBitmap  CBMP;
   HBITMAP  hBmp;
   COLORREF crFrom = RGB(255, 0, 0); // rosso
	CRect    rect;

	CDialog::OnInitDialog();

   if (Top != -1 && Left != -1)
	   SetWindowPos(NULL, Left, Top, 0, 0, SWP_NOSIZE);

   // creazione lista vuota
   m_ImageHdrList.Create(HDR_BITMAP_WIDTH, HDR_BITMAP_HEIGHT,
                         ILC_COLORDDB, 
                         2, // n. bitmap
                         0);
   // bitmap simbolo "ordine crescente"
   CBMP.LoadBitmap(MAKEINTRESOURCE(IDB_ASCENDING_ORDER));
   hBmp = HBITMAP(CBMP);
   gsui_SetBmpColorToDlgBkColor(hBmp, crFrom);
   m_ImageHdrList.Add(&CBMP, (CBitmap *) NULL);
   // bitmap simbolo "ordine decrescente"
   CBMP.LoadBitmap(MAKEINTRESOURCE(IDB_DESCENDING_ORDER));
   hBmp = HBITMAP(CBMP);
   gsui_SetBmpColorToDlgBkColor(hBmp, crFrom);
   m_ImageHdrList.Add(&CBMP, (CBitmap *) NULL);

   m_NetList.GetHeaderCtrl()->SetImageList(&m_ImageHdrList);

	m_NetList.SetExtendedStyle(m_NetList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

   col.mask     = LVCF_TEXT | LVCF_SUBITEM | LVCF_FMT;
   col.fmt      = LVCFMT_LEFT;
   col.pszText  = _T("");
   col.iSubItem = 0;
   m_NetList.InsertColumn(0, &col);
   m_NetList.SetColumnWidth(0, 40);

   col.pszText  = _T("Lati");
   col.iSubItem = 1;
   m_NetList.InsertColumn(1, &col);
   m_NetList.SetColumnWidth(1, 71);

   col.pszText  = _T("Nodi");
   col.iSubItem = 2;
   m_NetList.InsertColumn(2, &col);
   m_NetList.SetColumnWidth(2, 71);

   DisplayNetList();
   if (CurrSel >= m_NetList.GetItemCount()) CurrSel = 0;
   m_NetList.SetItemState(CurrSel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
   m_NetList.EnsureVisible(CurrSel, FALSE);
   m_NetList.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHy_evid_nets::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_NET_LIST, m_NetList);
}


BEGIN_MESSAGE_MAP(CHy_evid_nets, CDialog)
   ON_BN_CLICKED(IDC_EVID_BUTTON, OnBnClickedEvidNet)
   ON_BN_CLICKED(IDC_NET_HELP, OnBnClickedHelp)      
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_NET_LIST, OnLvnColumnclickNetList)
	ON_NOTIFY(NM_DBLCLK, IDC_NET_LIST, OnDblclkNetList)
END_MESSAGE_MAP()


// gestori di messaggi CHy_evid_nets

void CHy_evid_nets::OnBnClickedEvidNet()
{
   CRect    rcItem;
   POSITION pos = m_NetList.GetFirstSelectedItemPosition();

   CurrSel = (int) pos - 1;
   pCurrIds = m_pIdsVett[m_NetList.GetItemData(CurrSel)];

	GetWindowRect(rcItem);
	Left = rcItem.left;
   Top  = rcItem.top;

   EndDialog(IDEVIDNET); // This value is returned by DoModal!
}


void CHy_evid_nets::OnBnClickedHelp()
{
   CallHelp(m_hWnd, IDH_Elencodellesottoreti);
}


void CHy_evid_nets::DisplayNetList(void)
{
	LV_ITEM  lvitem;
   long     i;
   C_STRING msg;
   C_LONG   *pNode = (C_LONG *) m_pNodeList->get_head();

   for (i = 0; i < m_IdsVettLen; i++)
   {
      lvitem.iItem = i;

      // numero progressivo
      lvitem.mask     = LVIF_TEXT | LVIF_PARAM;
      msg             = i + 1;
      lvitem.iSubItem = 0;
      lvitem.pszText  = msg.get_name();
      lvitem.lParam   = i;
      m_NetList.InsertItem(&lvitem);

      // numero di link
      lvitem.mask     = LVIF_TEXT;
      msg             = m_pIdsVett[i]->get_count();
      lvitem.iSubItem = 1;
      lvitem.pszText  = msg.get_name();
      m_NetList.SetItem(&lvitem);

      // numero di nodi
      msg             = pNode->get_id();
      lvitem.iSubItem = 2;
      lvitem.pszText  = msg.get_name();
      m_NetList.SetItem(&lvitem);

      pNode = (C_LONG *) pNode->get_next();
   }

   if (OrderedColumn > 0)
      m_NetList.SortItems(NetListDataCompare, (LPARAM) this);
}

void CHy_evid_nets::OnLvnColumnclickNetList(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	HD_ITEM      curItem;
	CHeaderCtrl  *pHdrCtrl = m_NetList.GetHeaderCtrl();

   if (pNMLV->iSubItem == 0) return;

   curItem.pszText    = (TCHAR *) calloc(sizeof(TCHAR), 100);
   curItem.cchTextMax = 100 - 1;

   // se non era già stata selezionata questa casella
   if (Prev_iHeader != pNMLV->iSubItem)
   {
      if (Prev_iHeader != -1)
      {
         curItem.mask = HDI_TEXT;
	      pHdrCtrl->GetItem(Prev_iHeader, &curItem);
   	   curItem.mask = HDI_FORMAT; //  Testo
         curItem.fmt  = HDF_LEFT | HDF_STRING;
	      pHdrCtrl->SetItem(Prev_iHeader, &curItem);
      }
      AscendingOrder = TRUE;
      Prev_iHeader = pNMLV->iSubItem;
   }
   else
      AscendingOrder = !AscendingOrder;

   // add bmaps to header item
	pHdrCtrl->GetItem(pNMLV->iSubItem, &curItem);

   // Nome attributo con Flag di Visibilità
	curItem.mask = HDI_IMAGE | HDI_FORMAT; //  Testo e immagine
   curItem.iImage = (AscendingOrder) ? 0 : 1; // crescente oppure decrescente

	curItem.fmt = HDF_STRING | HDF_LEFT | HDF_IMAGE | HDF_BITMAP_ON_RIGHT;
	pHdrCtrl->SetItem(pNMLV->iSubItem, &curItem);

   m_NetList.SortItems(NetListDataCompare, (LPARAM) this);
   OrderedColumn = Prev_iHeader;

   free(curItem.pszText);
   *pResult = 0;
}

void CHy_evid_nets::OnDblclkNetList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnBnClickedEvidNet();
	*pResult = 0;
}


/*********************************************************/
// Funzione che avvia il file di help html
/*********************************************************/
void CallHelp(HWND Wndcaller, int TopicID)
{
   C_STRING path;

   path = get_WORKDIR();
   path += _T("\\NETWORK\\");
   path += GEODOCDIR;
   path += _T("\\");
   path += _T("NETWORK.CHM");
   
   // Controlla Correttezza Path
   if (gsc_nethost2drive(path) == GS_BAD) return;

   HtmlHelp(Wndcaller, path.get_name(), HH_HELP_CONTEXT, DWORD(TopicID));
}
