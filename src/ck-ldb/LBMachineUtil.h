/*****************************************************************************
 * $Source$
 * $Author$
 * $Date$
 * $Revision$
 *****************************************************************************/

/**
 * \addtogroup CkLdb
*/
/*@{*/

#ifndef _LDMACHINEUTIL_H_
#define _LDMACHINEUTIL_H_

#include "charm++.h"

class LBMachineUtil {
public:
  LBMachineUtil();
  void StatsOn();
  void StatsOff();
  void Clear();
  void TotalTime(double* walltime, double* cputime);
  void IdleTime(double* walltime) { *walltime = total_idletime; };

private:
  enum { off, on } state;
  double total_walltime;
  double total_cputime;
  double total_idletime;
  double start_totalwall;
  double start_totalcpu;
  double start_idle;

  int cancel_idleStart, cancel_idleEnd;

  void IdleStart(double curWallTime);
  void IdleEnd(double curWallTime);
  static void staticIdleStart(LBMachineUtil *util,double curWallTime);
  static void staticIdleEnd(LBMachineUtil *util,double curWallTime);
};

#endif  // _LDMACHINEUTIL_H_

/*@}*/
