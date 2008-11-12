 /*****************************************************************************
 * $Source$
 * $Author$
 * $Date$
 * $Revision$
 *****************************************************************************/

 /** \file CrayNid.c
 *  Author: Abhinav S Bhatele
 *  Date created: October 10th, 2007  
 *  
 *  This file is needed because including the cnos_mpi_os.h in a C++ leads 
 *  to a compiler error. Hence we have defined a wrapper function here which
 *  can be called from C++ files
 */

#include "converse.h"

#if XT3_TOPOLOGY || XT4_TOPOLOGY

#include <rca_lib.h>

#if XT3_TOPOLOGY
#include <catamount/cnos_mpi_os.h>
#define MAXNID 2784
#define TDIM 2

#elif XT4_TOPOLOGY
#include <pmi.h>
#define MAXNID 9000
#define TDIM 4

#endif

int *pid2nid;                   /* rank to node ID */
int nid2pid[MAXNID][TDIM];      /* node ID to rank */

/** \function getMeshCoord
 *  wrapper function for rca_get_meshcoord
 */
int getMeshCoord(int nid, int *x, int *y, int *z) {
  rca_mesh_coord_t xyz;
  rca_get_meshcoord(nid, &xyz);
  *x = xyz.mesh_x;
  *y = xyz.mesh_y;
  *z = xyz.mesh_z;
}

/** \function getXTNodeID
 *  returns nodeID corresponding to the CkMyPe() passed to it
 */
int getXTNodeID(int mype, int numpes) {
  int nid;

#if XT3_TOPOLOGY
  cnos_nidpid_map_t *nidpid; 
  int ierr;
  
  nidpid = (cnos_nidpid_map_t *)malloc(sizeof(cnos_nidpid_map_t) * numpes);

  ierr = cnos_get_nidpid_map(&nidpid);
  nid = nidpid[mype].nid;
  /* free(nidpid); */

#elif XT4_TOPOLOGY
  PMI_Portals_get_nid(mype, &nid);
#endif

  return nid;
}

/** \function pidtonid
 *  finds nids for pids 1 to CmiNumPes and stores them in an array
 *  correspondingly also creates an array for nids to pids
 */
void pidtonid(int numpes) {
#if XT3_TOPOLOGY
  cnos_nidpid_map_t *nidpid; 
  int ierr, i, j, nid;
  
  nidpid = (cnos_nidpid_map_t *)malloc(sizeof(cnos_nidpid_map_t) * numpes);
  pid2nid = (int *)malloc(sizeof(int) * numpes);

  for(i=0; i<MAXNID; i++) {
    nid2pid[i][0] = -1;
    nid2pid[i][1] = -1;
  }
      
  ierr = cnos_get_nidpid_map(&nidpid);
  for(i=0; i<numpes; i++) {
    nid = nidpid[i].nid;
    pid2nid[i] = nid;
    
    /* if the first position on the node is not filled */
    /* put it there (0) else at (1) */
    if (nid2pid[nid][0] == -1)
      nid2pid[nid][0] = i;
    else
      nid2pid[nid][1] = i;
  }
  /* free(nidpid); */

  /* CORRECTION FOR MPICH_RANK_REORDER_METHOD */

  int k = -1;
  for(i=0; i<MAXNID; i++) {
    if(nid2pid[i][0] != -1) {
      nid2pid[i][0] = k++;
      pid2nid[k] = i;
      nid2pid[i][1] = k++;
      pid2nid[k] = i;
    }
  }
  
#elif XT4_TOPOLOGY
  int i, j, nid;
  pid2nid = (int *)malloc(sizeof(int) * numpes);

  for(i=0; i<MAXNID; i++)
    for(j=0; j<4; j++)
      nid2pid[i][j] = -1;

  for (i=0; i<numpes; i++) {
    PMI_Portals_get_nid(i, &nid);
    pid2nid[i] = nid;
    if (nid2pid[nid][0] == -1)
      nid2pid[nid][0] = i;
    else if (nid2pid[nid][1] == -1)
      nid2pid[nid][1] = i;
    else if (nid2pid[nid][2] == -1)
      nid2pid[nid][2] = i;
    else
      nid2pid[nid][3] = i;
  }
#endif
}

#endif /* XT3 or XT4 TOPOLOGY */
