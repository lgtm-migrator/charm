/***************************************************************************
 * RCS INFORMATION:
 *
 *	$RCSfile$
 *	$Author$	$Locker$		$State$
 *	$Revision$	$Date$
 *
 ***************************************************************************
 * DESCRIPTION:
 *
 ***************************************************************************
 * REVISION HISTORY:
 *
 * $Log$
 * Revision 2.3  1995-07-22 23:44:13  jyelon
 * *** empty log message ***
 *
 * Revision 2.2  1995/06/14  19:39:26  gursoy
 * *** empty log message ***
 *
 * Revision 2.1  1995/06/08  17:09:41  gursoy
 * Cpv macro changes done
 *
 * Revision 1.6  1995/05/04  22:11:15  jyelon
 * *** empty log message ***
 *
 * Revision 1.5  1995/04/13  20:53:46  sanjeev
 * Changed Mc to Cmi
 *
 * Revision 1.4  1994/12/10  19:00:55  sanjeev
 * added extern decls for register fns
 *
 * Revision 1.3  1994/12/02  00:01:57  sanjeev
 * interop stuff
 *
 * Revision 1.2  1994/11/11  05:31:26  brunner
 * Removed ident added by accident with RCS header
 *
 * Revision 1.1  1994/11/07  15:39:49  brunner
 * Initial revision
 *
 ***************************************************************************/

#ifndef C_PLUS_INTERFACE_H
#define C_PLUS_INTERFACE_H

/*  #define BASE_EP_NUM 0x00008001	*/

#define NULL_EP -1

typedef unsigned short MAGIC_NUMBER_TYPE ;

extern "C" void CollectValue(int, int, ChareIDType *) ;
extern "C" void * MonoValue(int) ;

class _CK_Object ;

/* EPTYPE is no longer used  : SANJEEV, Jan 5
 * EPTYPE is a pointer to a member function of a chare 
 * typedef void (_CK_Object::*EPTYPE)(void *) ;  
 */

class _CK_Object {  /* Top level chare object at root of chare hierarchy */
public:
	ChareIDType thishandle ;   
	/* This is put in by the translator for use by Charm */

	_CK_Object() {}

	_CK_Object(int magic) {
        	SetID_onPE(thishandle, CmiMyPe());
        	SetID_chare_magic_number(thishandle,(int)magic) ;
        	SetID_chareBlockPtr(thishandle, ((CHARE_BLOCK *)this));
	}

/*	virtual void CallEP(EPTYPE EpPtr, void *msg)
 *      {
 *              (this->*EpPtr)(msg) ;
 *      }
 *
 *	virtual void SwitchEP(int EpIndex, void * msg) = 0 ; 
 */
} ;



class _CK_BOC : public _CK_Object {  /* top level BOC object */

public:
	int _CK_MyBocNum ;  /* BocNum of this instance of the BOC */
	int thishandle ;  /* also stores BocNum */

	_CK_BOC() {}
	_CK_BOC(int magic) : _CK_Object(magic) {}

	void setBocNum(int num) {
		thishandle = _CK_MyBocNum = num ;
	}

/*	virtual void SwitchEP(int EpIndex, void * msg) = 0 ;   */
} ;


class _CK_Accumulator { /* top level Accumulator object */

public:
	int _CK_MyId ;

	virtual void * _CK_GetMsgPtr() = 0 ;

	virtual void _CK_Combine(void *) = 0 ;

	void CollectValue(int EpNum, ChareIDType cid)
	{
		::CollectValue(_CK_MyId, EpNum, &cid) ; 
		/* in node_acc.c */
	}
} ;

class _CK_Monotonic { /* top level Monotonic object */

public:
	int _CK_MyId ;

	virtual void * _CK_GetMsgPtr() = 0 ;

	virtual void _CK_SysUpdate(void *) = 0 ;  /* called by system */
} ;



/* EPFnType is a pointer to a _CK_call_Chare_EP() function */
typedef void (*EPFnType)(void *, _CK_Object *) ;

/* CHAREFNTYPE is a pointer to a _CK_create_ChareName() function */
typedef _CK_Object * (*CHAREFNTYPE)(int) ;

/* BOCFNTYPE is a pointer to a _CK_create_BocName() function */
typedef _CK_BOC * (*BOCFNTYPE)(void) ;

/* ACCFNTYPE is a pointer to a _CK_create_AccName() function */
typedef void * (*ACCFNTYPE)(void *) ;

/* ALLOCFNTYPE is a pointer to a _CK_alloc_MsgName() function */
typedef void * (*ALLOCFNPTR)(int, int, int*, int) ;

/*
struct argc_argv {
    int argc ;
    char **argv ;
} ;
*/


/* this is the handle of the main chare, used in place of MainChareID */
extern ChareIDType mainhandle ;
extern ChareIDType NULL_HANDLE ;


/* These are the external definitions for all Charm calls */

/* This is generated by the translator function */
extern int _CK_FindLocalID(int *, int *, int, int) ;

/* These are C++ runtime system functions */
/*
extern void CPlus_MyChareID(ChareIDType *) ;
extern void CPlus_MainChareID(ChareIDType *) ;
extern void CPlus_MyBranchID(ChareIDType *, int) ;
*/
extern void CPlus_ChareExit() ;

/* These are C runtime system functions */

extern "C" int registerMsg(char *name, FUNCTION_PTR allocf, FUNCTION_PTR packf, FUNCTION_PTR unpackf, int size) ;

extern "C" int registerBocEp(char *name, FUNCTION_PTR epFunc , int epType , int msgIndx, int chareIndx) ;

extern "C" int registerEp(char *name, FUNCTION_PTR epFunc , int epType , int msgIndx, int chareIndx) ;

extern "C" int registerChare(char *name, int dataSz, FUNCTION_PTR createfn) ;

extern "C" int registerFunction(FUNCTION_PTR fn) ;

extern "C" int registerMonotonic(char *name , FUNCTION_PTR initfn, FUNCTION_PTR updatefn , int language) ;

extern "C" int registerTable(char *name , FUNCTION_PTR initfn, FUNCTION_PTR hashfn) ;

extern "C" int registerAccumulator(char *name , FUNCTION_PTR initfn, FUNCTION_PTR addfn, FUNCTION_PTR combinefn , int language) ;

extern "C" int registerReadOnlyMsg() ;

extern "C" void registerReadOnly(int size , FUNCTION_PTR fnCopyFromBuffer, FUNCTION_PTR fnCopyToBuffer) ;

extern "C" void registerMainChare(int m, int ep , int type) ;



extern "C" void * GenericCkAlloc(int, int, int) ;
extern "C" void * VarSizeCkAlloc(int, int, int, int[]) ;
extern "C" void * CkAllocPackBuffer(void *, int) ;
extern "C" int CreateBoc(int, int, void *, int, ChareIDType *) ;
extern "C" void CreateChare(int, int, void *, ChareIDType *, int) ;
extern "C" int CreateAcc(int, void *, int, ChareIDType *) ;
extern "C" int CreateMono(int, void *, int, ChareIDType *) ;
extern "C" void CkExit() ;
extern "C" void CkFreeMsg(void *) ;
extern "C" void CmiFree(void *) ;
extern "C" int CmiNumPe() ;
extern "C" int CmiMyPe() ;
extern "C" void GeneralSendMsgBranch(int, void *, int, char, int, int) ;
extern "C" void GeneralBroadcastMsgBranch(int, void *, char, int, int) ;
extern "C" void SendMsg(int, void *, ChareIDType *) ;
extern "C" void *GetBocDataPtr(int) ;
extern "C" void SetBocDataPtr(int, _CK_BOC *) ;
extern "C" double CmiTimer() ;
extern "C" int CmiSpanTreeRoot() ;
extern "C" int CmiNumSpanTreeChildren(int) ;
extern "C" int CmiSpanTreeParent(int) ;
extern "C" void CmiSpanTreeChild(int node, int *children) ;

extern "C" void VidSend(CHARE_BLOCK *, PeNumType, VID_BLOCK *) ;
// extern "C" void RegisterDynamicBocInitMsg(int *) ;
// extern "C" void OtherCreateBoc(void *) ;
extern "C" void SendNodeStatistics() ;
extern "C" void close_log() ;
extern "C" void PrintStsFile(char *) ;
extern "C" void trace_creation(int, int, ENVELOPE *) ;
extern "C" void trace_begin_execute(ENVELOPE *) ;
extern "C" void trace_end_execute(int, int, int) ;

extern "C" int CPlus_GetMagicNumber(_CK_Object *) ;
extern "C" void CPlus_StartQuiescence(int, ChareIDType) ;
// extern "C" void _CK_CallCharmInit(_CK_Object *, argc_argv *) ;

/*
extern "C" _CK_Object * _CK_SwitchChare(int, int) ;
extern "C" void _CK_SwitchBOC(int, int) ;
extern "C" void * _CK_SwitchAcc(int, void *) ;
extern "C" void * _CK_SwitchMono(int, void *) ;
*/

extern "C" void * _CK_9GetAccDataPtr(void *) ;
extern "C" void * _CK_9GetMonoDataPtr(void *) ;
extern "C" void _CK_BroadcastMono(void *, int) ;

/* this is no longer needed for VarSize 
extern "C" int _CK_3GetSizeOfMsg(void *) ;
*/

extern "C" void * CkPriorityPtr(void *) ;
extern "C" ENVELOPE *CkCopyEnv(ENVELOPE *) ;

/* DONT put these here else even system code will get modified !! */
/*****
#define MyChareID CPlus_MyChareID
#define MainChareID CPlus_MainChareID
#define MyBranchID CPlus_MyBranchID
#define ChareExit CPlus_ChareExit
******/

#endif
