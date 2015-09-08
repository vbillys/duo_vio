//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: getScaledMap.h
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 06-Sep-2015 10:04:04
//
#ifndef __GETSCALEDMAP_H__
#define __GETSCALEDMAP_H__

// Include Files
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_defines.h"
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "SLAM_types.h"

// Function Declarations
extern void getScaledMap(const emxArray_real_T *x, const emxArray_real_T
  *b_anchorFeatures, const emxArray_real_T *b_m_vect, double numTrackFeatures,
  double stateSize, double numStatesPerAnchorxt, emxArray_real_T *map,
  emxArray_real_T *anchorInd, emxArray_real_T *featureAnchorInd);

#endif

//
// File trailer for getScaledMap.h
//
// [EOF]
//