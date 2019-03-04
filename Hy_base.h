/******************************************************************************/
/*Name: HY_BASE
                                   
  Module description: File funzioni di utilità e di base
            
  Author: Maria Paola Deprati
                                         
  (c) Copyright 1998-2010 by AMGA S.P.A

/******************************************************************************/

#ifndef _hy_base_h
#define _hy_base_h 1


#include "stdafx.h"

#define INITGUID
#import "msado15.dll" no_namespace rename ("EOF", "EndOfFile") rename ("EOS", "ADOEOS")

#ifndef _gs_list_h
   #include "gs_list.h"
#endif

#ifndef _ERROR_
	#include "gs_error.h"
#endif

#ifndef _gs_utily_h
   #include "gs_utily.h"
#endif   

#ifndef _gs_area_h
   #include "gs_area.h"
#endif   

#ifndef _gs_resbf_h
   #include "gs_resbf.h"
#endif   

#ifndef _gs_dbref_h
	#include "gs_dbref.h"
#endif   

#ifndef _gs_init_h
	#include "gs_init.h"
#endif   


#define	SUB1		1
#define	SUB2		2
#define	SUB3		3
#define	SUB4		4
#define	SUB5		5
#define	SUB6		6

#define	DIAMC	"DIAMC.ACCDB"


enum NetworkTypesEnum
{
   H2O = 1, // non cambiare questi valori
	GB  = 2,
   GM  = 3
};


void gs_errmsg(int msg);
void hy_errmsg(int msg);

int hy_list_deleted(int cls, C_2INT_LONG_LIST *list);
int hy_last(C_CLASS *pCls,  C_INT_LONG_LIST *list);

int hy_outRecordSet(C_CLASS *pCls, CString *fields, CString *where, _RecordsetPtr &pRs, enum CursorTypeEnum CursorType = adOpenKeyset,
					enum LockTypeEnum LockType = adLockOptimistic, int type = TEMP);

int write_udl(CString udlFile, CString connStr);

int gsc_clear_file(CString filename, int div, long *position);

int is_GEOnetworkClass(C_CLASS *pCls, NetworkTypesEnum type);


class HyAppl
{
public:
	HyAppl();
   ~HyAppl();

	static C_LINK_SET GS_LS;     // resbuf per gs_evid
};

class C_4LONG : public C_NODE
{
	friend class C_4LONG_LIST;
	protected:
		long valveId;
		long newId;
		long line1Id;
		long line2Id;
   
	public :
		C_4LONG();
		C_4LONG(long l1, long l2, long l3, long l4);
		~C_4LONG(); 

		long  get_valveId();
		long  get_newId();
		long  get_line1Id();
		long  get_line2Id();
};

class C_4LONG_LIST : public C_LIST
{
   public :
		DllExport C_4LONG_LIST();
		DllExport virtual ~C_4LONG_LIST();		// chiama ~C_LIST

		C_4LONG *searchLine1(long id, long line_id);
		C_4LONG *searchLine2(long id, long line_id);
};

#endif







