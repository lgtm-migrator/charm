/**
 * \addtogroup CkPerf
*/
/*@{*/

#ifndef _SUMMARY_H
#define _SUMMARY_H

#include <stdio.h>
#include <errno.h>

#include "trace.h"
#include "envelope.h"
#include "register.h"
#include "trace-common.h"

// initial bin size, time in seconds
#define  BIN_SIZE	0.001

#define  MAX_MARKS       256

#define  MAX_PHASES       100

int sumonly = 0;
int sumDetail = 0;

/// Bin entry record CPU time in an interval
class BinEntry {
  public:
    void *operator new(size_t s) {void*ret=malloc(s);_MEMCHECK(ret);return ret;}
    void *operator new(size_t, void *ptr) { return ptr; }
    void operator delete(void *ptr) { free(ptr); }
#if defined(_WIN32) || CMK_MULTIPLE_DELETE
    void operator delete(void *, void *) { }
#endif
    BinEntry(): _time(0.), _idleTime(0.),
                 _msgSize(0), _msgCount(0),
                 _recvSize(0), _recvCount(0),
                 _extRecvSize(0), _extRecvCount(0),
                 _defaultVal(0)
    {
      if(sumDetail) {
        _msgCountPerEP = std::vector<unsigned long>(_entryTable.size() + 10);
        _msgSizePerEP = std::vector<unsigned long>(_entryTable.size() + 10);
        _recvCountPerEP = std::vector<unsigned long>(_entryTable.size() + 10);
        _recvSizePerEP = std::vector<unsigned long>(_entryTable.size() + 10);
        _extRecvCountPerEP = std::vector<unsigned long>(_entryTable.size() + 10);
        _extRecvSizePerEP = std::vector<unsigned long>(_entryTable.size() + 10);
        for (int i = 0; i < _msgCountPerEP.size(); ++i) {
          _msgCountPerEP[i] = _msgSizePerEP[i] =
          _recvCountPerEP[i] = _recvSizePerEP[i] =
          _extRecvCountPerEP[i] = _extRecvCountPerEP[i] = 0;
        }
      }
    }

    BinEntry(double t, double idleT): _time(t), _idleTime(idleT), _defaultVal(0) {}

    BinEntry(double t, double idleT, unsigned long msgSize, unsigned long msgCount,
             std::vector<unsigned long> msgSizePerEP, std::vector<unsigned long> msgCountPerEP,
             unsigned long recvSize, unsigned long recvCount,
             std::vector<unsigned long> recvSizePerEP, std::vector<unsigned long> recvCountPerEP,
             unsigned long extRecvSize, unsigned long extRecvCount,
             std::vector<unsigned long> extRecvSizePerEP, std::vector<unsigned long> extRecvCountPerEP
             ): _time(t), _idleTime(idleT),
                 _msgSize(msgSize), _msgCount(msgCount),
                 _recvSize(recvSize), _recvCount(recvCount),
                 _extRecvSize(extRecvSize), _extRecvCount(extRecvCount),
                 _defaultVal(0)

    {
      if(_msgSizePerEP.size() == 0)
      {
        _msgCountPerEP = std::vector<unsigned long>(msgSizePerEP.size());
        _msgSizePerEP = std::vector<unsigned long>(msgSizePerEP.size());
        _recvCountPerEP = std::vector<unsigned long>(msgSizePerEP.size());
        _recvSizePerEP = std::vector<unsigned long>(msgSizePerEP.size());
        _extRecvCountPerEP = std::vector<unsigned long>(msgSizePerEP.size());
        _extRecvSizePerEP = std::vector<unsigned long>(msgSizePerEP.size());

        for(int i = 0; i < _msgSizePerEP.size(); ++i) {
            _msgSizePerEP[i] = _msgCountPerEP[i] =
            _recvSizePerEP[i] = _recvCountPerEP[i] =
            _extRecvSizePerEP[i] = _extRecvCountPerEP[i] = 0;
        }
      }
      for(int i = 0; i < msgSizePerEP.size(); ++i) {
        _msgSizePerEP[i] = msgSizePerEP[i];
        _msgCountPerEP[i] = msgCountPerEP[i];
        _recvSizePerEP[i] = recvSizePerEP[i];
        _recvCountPerEP[i] = recvCountPerEP[i];
        _extRecvSizePerEP[i] = extRecvSizePerEP[i];
        _extRecvCountPerEP[i] = extRecvCountPerEP[i];
      }
  }
    double &time() { return _time; }
    double &getIdleTime() { return _idleTime; }
    unsigned long &getSize() { return _msgSize; }
    unsigned long &getCount() {return _msgCount; }
    std::vector<unsigned long> &getSizePerEP() { return _msgSizePerEP;}
    std::vector<unsigned long> &getCountPerEP() {return _msgCountPerEP; }
    unsigned long &getSizePerEPPerIndex(int j) {return (j < _msgSizePerEP.size())?_msgSizePerEP[j]:_defaultVal; }
    unsigned long &getCountPerEPPerIndex(int j) {return (j < _msgCountPerEP.size())?_msgCountPerEP[j]:_defaultVal; }

    unsigned long &getRecvSize() { return _recvSize; }
    unsigned long &getRecvCount() { return _recvCount; }
    std::vector<unsigned long> &getRecvSizePerEP() {return _recvSizePerEP; }
    std::vector<unsigned long> &getRecvCountPerEP() {return _recvCountPerEP; }
    unsigned long &getRecvSizePerEPPerIndex(int j) {return (j < _recvSizePerEP.size())?_recvSizePerEP[j]:_defaultVal; }
    unsigned long &getRecvCountPerEPPerIndex(int j) {return (j < _recvCountPerEP.size())?_recvCountPerEP[j]:_defaultVal; }

    unsigned long &getExtRecvSize() {return _extRecvSize;}
    unsigned long &getExtRecvCount() {return _extRecvCount;}
    std::vector<unsigned long> &getExtRecvSizePerEP() {return _extRecvSizePerEP; }
    std::vector<unsigned long> &getExtRecvCountPerEP() {return _extRecvCountPerEP; }
    unsigned long &getExtRecvSizePerEPPerIndex(int j) {return (j < _extRecvSizePerEP.size())?_extRecvSizePerEP[j]:_defaultVal; }
    unsigned long &getExtRecvCountPerEPPerIndex(int j) {return (j < _extRecvCountPerEP.size())?_extRecvCountPerEP[j]:_defaultVal; }
    void write(FILE *fp);
    int  getU();
    int getUIdle();

  private:
    double _time;
    double _idleTime;
    unsigned long _msgSize;
    unsigned long _msgCount;
    std::vector<unsigned long> _msgSizePerEP;
    std::vector<unsigned long> _msgCountPerEP;
    unsigned long _recvSize;
    unsigned long _recvCount;
    std::vector<unsigned long> _recvSizePerEP;
    std::vector<unsigned long> _recvCountPerEP;
    unsigned long _extRecvSize;
    unsigned long _extRecvCount;
    std::vector<unsigned long> _extRecvSizePerEP;
    std::vector<unsigned long> _extRecvCountPerEP;
    unsigned long _defaultVal;
};

/// a phase entry for trace summary
class PhaseEntry {
  private:
    int nEPs;
    int *count;
    double *times;
    double *maxtimes;
  public:
    PhaseEntry();
    ~PhaseEntry() { delete [] count; delete [] times; delete [] maxtimes; }
    /// one entry is called for 'time' seconds.
    void setEp(int epidx, double time) {
	if (epidx>=nEPs) CmiAbort("Too many entry functions!\n");
	count[epidx]++;
	times[epidx] += time;
	if (maxtimes[epidx] < time) maxtimes[epidx] = time;
    }
    /**
        write two lines for each phase:
        1. number of calls for each entry;
        2. time in us spent for each entry.
    */
    void write(FILE *fp, int seq) {
	int i;
	fprintf(fp, "[%d] ", seq);
	int _numEntries=_entryTable.size();
	for (i=0; i<_numEntries; i++) 
	    fprintf(fp, "%d ", count[i]);
	fprintf(fp, "\n");

	fprintf(fp, "[%d] ", seq);
	for (i=0; i<_numEntries; i++) 
	    fprintf(fp, "%ld ", (long)(times[i]*1.0e6) );
	fprintf(fp, "\n");

	fprintf(fp, "[%d] ", seq);
	for (i=0; i<_numEntries; i++) 
	    fprintf(fp, "%ld ", (long)(maxtimes[i]*1.0e6) );
	fprintf(fp, "\n");
    }
};

/// table of PhaseEntry
class PhaseTable {
  private:
    PhaseEntry **phases;
    int numPhase;         /**< phase table size */
    int cur_phase;	  /**< current phase */
    int phaseCalled;      /**< total number of phases */
  public:
    PhaseTable(int n): numPhase(n) {
        phases = new PhaseEntry*[n];
        _MEMCHECK(phases);
        for (int i=0; i<n; i++) phases[i] = NULL;
        cur_phase = -1;
        phaseCalled = 0;
    }
    ~PhaseTable() {
	for (int i=0; i<numPhase; i++) delete phases[i];
	delete [] phases;
    }
    inline int numPhasesCalled() { return phaseCalled; };
    /**
      start a phase. If new, create a new PhaseEntry
    */
    void startPhase(int p) { 
	if (p<0 && p>=numPhase) CmiAbort("Invalid Phase number. \n");
	cur_phase = p; 
	if (phases[cur_phase] == NULL) {
	    phases[cur_phase] = new PhaseEntry;
	    _MEMCHECK(phases[cur_phase]);
	    phaseCalled ++;
        }
    }
    void setEp(int epidx, double time) {
	if (cur_phase == -1) return;
	if (phases[cur_phase] == NULL) CmiAbort("No current phase!\n");
	phases[cur_phase]->setEp(epidx, time);
    }
    void write(FILE *fp) {
	for (int i=0; i<numPhase; i++ )
	    if (phases[i]) { 
		phases[i]->write(fp, i);
            }
    }
};

double epThreshold;
double epInterval;

/// info for each entry
class SumEntryInfo {
public:
  double epTime;
  double epMaxTime;
  int epCount;
  enum {HIST_SIZE = 10};
  int hist[HIST_SIZE];
public:
  SumEntryInfo(): epTime(0.), epMaxTime(0.), epCount(0) {}
  void clear() {
    epTime = epMaxTime = 0.;
    epCount = 0;
    for (int i=0; i<HIST_SIZE; i++) hist[i]=0;
  }
  void setTime(double t) {
    epTime += t;
    epCount ++;
    if (epMaxTime < t) epMaxTime = t;
    for (int i=HIST_SIZE-1; i>=0; i--) {
      if (t>epThreshold+i*epInterval) {
        hist[i]++; break;
      }
    }
  }
};

/// summary log pool
class SumLogPool {
  private:
    UInt poolSize;
    UInt numBins;
    BinEntry *pool;	/**< bins */
    FILE *fp, *stsfp, *sdfp ;
    char *pgm;

    SumEntryInfo  *epInfo;
    UInt epInfoSize;

    /// a mark entry for trace summary
    typedef struct {
      double time;
    } MarkEntry;
    CkVec<MarkEntry *> events[MAX_MARKS];
    int markcount;

    /// for phases
    PhaseTable phaseTab;

    /// for Summary-Detail
    double *cpuTime;    //[MAX_INTERVALS * MAX_ENTRIES];
    int *numExecutions; //[MAX_INTERVALS * MAX_ENTRIES];

    enum LogWriter {
        MsgSentCount,
        MsgSentSize,
        MsgRecvCount,
        MsgRecvSize,
        ExternalMsgRecvCount,
        ExternalMsgRecvSize
    };

  public:
    SumLogPool(char *pgm);
    ~SumLogPool();
    double *getCpuTime() {return cpuTime;}
    void initMem();
    void write(void) ;
    void writeEncoder(int numBins, LogWriter event);
    void writeEncoderRow(int numBins, LogWriter event);
    void writeSts(void);
    void add(double time, double idleTime, unsigned long msgSize, unsigned long msgCount,
             std::vector<unsigned long> msgSizePerEP, std::vector<unsigned long> msgCountPerEP,
             unsigned long recvSize, unsigned long recvCount,
             std::vector<unsigned long> recvSizePerEP, std::vector<unsigned long> recvCountPerEP,
             unsigned long extRecvSize, unsigned long extRecvCount,
             std::vector<unsigned long> extRecvSizePerEP, std::vector<unsigned long> extRecvCountPerEP,
             int pe);
    void add(double time, double idleTime, int pe);
    void setEp(int epidx, double time);
    void clearEps() {
      for(int i=0; i < epInfoSize; i++) {
	epInfo[i].clear();
      }
    }
    void shrink(void);
    void shrink(double max);
    void addEventType(int eventType, double time);
    void startPhase(int phase) { phaseTab.startPhase(phase); }
    BinEntry *bins() { return pool; }
    UInt getNumEntries() { return numBins; }
    UInt getEpInfoSize() {return epInfoSize;} 
    UInt getPoolSize() {return poolSize;}
    // accessors to normal summary data
    inline double getTime(unsigned int interval) {
      return pool[interval].time();
    }


    inline double getCPUtime(unsigned int interval, unsigned int ep){
      if(cpuTime != NULL)
        return cpuTime[interval*epInfoSize+ep]; 
      else 
	return 0.0;
    }
    
    inline void setCPUtime(unsigned int interval, unsigned int ep, double val){
        cpuTime[interval*epInfoSize+ep] = val; }
    inline double addToCPUtime(unsigned int interval, unsigned int ep, double val){
        cpuTime[interval*epInfoSize+ep] += val;
        return cpuTime[interval*epInfoSize+ep]; }
    inline int getNumExecutions(unsigned int interval, unsigned int ep){
        return numExecutions[interval*epInfoSize+ep]; }
    inline void setNumExecutions(unsigned int interval, unsigned int ep, unsigned int val){
        numExecutions[interval*epInfoSize+ep] = val; }
    inline int incNumExecutions(unsigned int interval, unsigned int ep){
        ++numExecutions[interval*epInfoSize+ep];
        return numExecutions[interval*epInfoSize+ep]; }
    inline int getUtilization(int interval, int ep);


    void updateSummaryDetail(int epIdx, double startTime, double endTime);


};

/// class for recording trace summary events 
/**
  TraceSummary calculate CPU utilizations in bins, and will record
  number of calls and total wall time for each entry. 
*/
class TraceSummary : public Trace {
    SumLogPool*  _logPool;
    int execEvent;
    int execEp;
    int execPe;
    int msgNum; /* used to handle multiple endComputation calls?? */

    /* per-log metadata maintained to derive cross-event information */
    double binStart; /* time of last filled bin? */
    double start, packstart, unpackstart, idleStart;
    double binTime, binIdle;
    int inIdle;
    int inExec;
    int depth;
    unsigned long msgSize;
    unsigned long msgCount;
    std::vector<unsigned long> msgSizePerEP;
    std::vector<unsigned long> msgCountPerEP;
    unsigned long recvSize;
    unsigned long recvCount;
    std::vector<unsigned long> recvSizePerEP;
    std::vector<unsigned long> recvCountPerEP;
    unsigned long extRecvSize;
    unsigned long extRecvCount;
    std::vector<unsigned long> extRecvSizePerEP;
    std::vector<unsigned long> extRecvCountPerEP;

  public:
    TraceSummary(char **argv);

    void beginExecute(envelope *e, void *obj);
    void beginExecute(char *msg);
    void beginExecute(CmiObjId  *tid);
    void beginExecute(int event,int msgType,int ep,int srcPe, int mlen=0, CmiObjId *idx=NULL, void *obj=NULL);
    void endExecute(void);
    void endExecute(char *msg);
    void beginIdle(double currT);
    void endIdle(double currT);
    void traceBegin(void);
    void traceEnd(void);
    void beginPack(void);
    void endPack(void);
    void beginUnpack(void);
    void endUnpack(void);
    void beginComputation(void);
    void endComputation(void);

    void creation(envelope *e, int epIdx, int num=1);
    void creationMulticast(envelope *e, int epIdx, int num=1, const int *pelist=NULL);
    void resetCounters();
    void setCounters();

    void traceClearEps();
    void traceWriteSts();
    void traceClose();

    /**
       for trace summary event mark
    */
    void addEventType(int eventType);
    /** 
       for starting a new phase
    */
    void startPhase(int phase);

    /**
       query utilities
    */
    SumLogPool *pool() { return _logPool; }

    /**
     *  Supporting methods for CCS queries
     */
    void traceEnableCCS();
    void fillData(double *buffer, double reqStartTime, 
		  double reqBinSize, int reqNumBins);
};

#endif

/*@}*/
