/*************************************************************************
Name: HY_INOUT
                                   
Module description: File funzioni di base per l'input e l'output
                    dei dati per i modelli di calcolo 
            
Author: Maria Paola Deprati

(c) Copyright 1995-2010 by AMGA S.P.A

*************************************************************************/

#ifndef _hy_inout_h
#define _hy_inout_h 1


#ifndef _hy_base_h
   #include "hy_base.h"
#endif

#ifndef _gs_hy_h
   #include "gs_hy.h"
#endif

#ifndef _hy_diamc_h
	#include "hy_diamc.h"
#endif

#ifndef _hy_sceltasim_h
	#include "sceltasim.h"
#endif

#include "gs_topo.h"

void hy_sim_h2o();
void hy_sim_gb();
void hy_sim_gm();
void hy_sim_evid();
void hy_sim_cd();
void hy_diamc();

void gs_insNodeH2O();
void gs_insNodeGasB();
void gs_insNodeGasM();

void gs_insTraceH2O();
void gs_insTraceGasB();
void gs_insTraceGasM();

// void gs_insSourceH2O();
// void gs_insSourceGasB();
// void gs_insSourceGasM();

void gs_insValveH2O();
void gs_insValveGasB();
void gs_insValveGasM();

// void gs_insPumpH2O();

void gs_insPrvH2O();
void gs_insPrvGasB();
void gs_insPrvGasM();


#endif


