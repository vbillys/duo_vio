//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: Mahalanobis_EKF.cpp
//
// MATLAB Coder version            : 2.8
// C/C++ source code generated on  : 19-Aug-2015 11:35:06
//

// Include Files
#include "rt_nonfinite.h"
#include "SLAM.h"
#include "Mahalanobis_EKF.h"
#include "mrdivide.h"
#include "quatmultJ.h"
#include "quatPlusThetaJ.h"
#include "any.h"
#include "SLAM_emxutil.h"
#include "eye.h"
#include "fprintf.h"
#include "norm.h"
#include "getH_R_res.h"
#include "getMap.h"
#include <stdio.h>

// Function Definitions

//
// MAHALANOBIS_EKF Update the state and perform mahalanobis outlier rejection
// Arguments    : emxArray_real_T *b_xt
//                emxArray_real_T *b_P
//                const double z_all_l[32]
//                double c_numStatesxt
//                double c_numStates
//                double b_numPointsPerAnchor
//                const double c_cameraparams_CameraParameters[3]
//                const double d_cameraparams_CameraParameters[2]
//                const double e_cameraparams_CameraParameters[2]
//                const emxArray_real_T *b_anchorFeatures
//                const emxArray_real_T *b_m_vect
//                const double b_imNoise[2]
//                const double IMU_measurements[23]
//                emxArray_real_T *validFeatures
//                double *b_model_prob
// Return Type  : void
//
void Mahalanobis_EKF(emxArray_real_T *b_xt, emxArray_real_T *b_P, const double
                     z_all_l[32], double c_numStatesxt, double c_numStates,
                     double b_numPointsPerAnchor, const double
                     c_cameraparams_CameraParameters[3], const double
                     d_cameraparams_CameraParameters[2], const double
                     e_cameraparams_CameraParameters[2], const emxArray_real_T
                     *b_anchorFeatures, const emxArray_real_T *b_m_vect, const
                     double b_imNoise[2], const double IMU_measurements[23],
                     emxArray_real_T *validFeatures, double *b_model_prob)
{
  emxArray_boolean_T *c_anchorFeatures;
  int i20;
  int ia;
  emxArray_boolean_T *inlierStatus;
  emxArray_int32_T *ii;
  int nx;
  int idx;
  int b_ii;
  boolean_T exitg1;
  boolean_T guard1 = false;
  emxArray_real_T *indMeas;
  emxArray_real_T *x;
  emxArray_real_T *K;
  emxArray_real_T *H;
  emxArray_real_T *x_apo;
  emxArray_real_T *map;
  emxArray_real_T *featureAnchorInd;
  int k;
  emxArray_real_T *S;
  emxArray_real_T *C;
  emxArray_real_T *b;
  emxArray_real_T *b_indMeas;
  emxArray_real_T *r;
  emxArray_real_T *unusedU0;
  emxArray_real_T *R;
  int i21;
  int ib;
  double a[2];
  int m;
  int ic;
  int ar;
  emxArray_real_T *b_a;
  double b_x;
  double y;
  double b_r[2];
  double b_S[4];
  emxArray_real_T *b_C;
  double b_x_apo[3];
  double dv21[4];
  emxArray_int32_T *r30;
  emxArray_int32_T *r31;
  emxArray_int32_T *r32;
  emxArray_real_T *c_x;
  emxArray_boolean_T *d_anchorFeatures;
  double d_numStates;
  double d_x[4];
  double dv22[4];
  emxArray_real_T *c_S;
  emxInit_boolean_T(&c_anchorFeatures, 2);
  i20 = c_anchorFeatures->size[0] * c_anchorFeatures->size[1];
  c_anchorFeatures->size[0] = b_anchorFeatures->size[0];
  c_anchorFeatures->size[1] = b_anchorFeatures->size[1];
  emxEnsureCapacity((emxArray__common *)c_anchorFeatures, i20, (int)sizeof
                    (boolean_T));
  ia = b_anchorFeatures->size[0] * b_anchorFeatures->size[1];
  for (i20 = 0; i20 < ia; i20++) {
    c_anchorFeatures->data[i20] = (b_anchorFeatures->data[i20] == 1.0);
  }

  b_emxInit_boolean_T(&inlierStatus, 1);
  emxInit_int32_T(&ii, 1);
  b_any(c_anchorFeatures, inlierStatus);
  nx = inlierStatus->size[0];
  idx = 0;
  i20 = ii->size[0];
  ii->size[0] = inlierStatus->size[0];
  emxEnsureCapacity((emxArray__common *)ii, i20, (int)sizeof(int));
  b_ii = 1;
  emxFree_boolean_T(&c_anchorFeatures);
  exitg1 = false;
  while ((!exitg1) && (b_ii <= nx)) {
    guard1 = false;
    if (inlierStatus->data[b_ii - 1]) {
      idx++;
      ii->data[idx - 1] = b_ii;
      if (idx >= nx) {
        exitg1 = true;
      } else {
        guard1 = true;
      }
    } else {
      guard1 = true;
    }

    if (guard1) {
      b_ii++;
    }
  }

  if (inlierStatus->size[0] == 1) {
    if (idx == 0) {
      i20 = ii->size[0];
      ii->size[0] = 0;
      emxEnsureCapacity((emxArray__common *)ii, i20, (int)sizeof(int));
    }
  } else {
    i20 = ii->size[0];
    if (1 > idx) {
      ii->size[0] = 0;
    } else {
      ii->size[0] = idx;
    }

    emxEnsureCapacity((emxArray__common *)ii, i20, (int)sizeof(int));
  }

  emxInit_real_T(&indMeas, 1);
  i20 = indMeas->size[0];
  indMeas->size[0] = ii->size[0];
  emxEnsureCapacity((emxArray__common *)indMeas, i20, (int)sizeof(double));
  ia = ii->size[0];
  for (i20 = 0; i20 < ia; i20++) {
    indMeas->data[i20] = ii->data[i20];
  }

  emxInit_real_T(&x, 1);
  i20 = x->size[0];
  x->size[0] = b_xt->size[0];
  emxEnsureCapacity((emxArray__common *)x, i20, (int)sizeof(double));
  ia = b_xt->size[0];
  for (i20 = 0; i20 < ia; i20++) {
    x->data[i20] = b_xt->data[i20];
  }

  b_emxInit_real_T(&K, 2);
  b_emxInit_real_T(&H, 2);

  //  get a copy of the state for iteration
  i20 = K->size[0] * K->size[1];
  K->size[0] = 1;
  K->size[1] = 1;
  emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
  K->data[0] = 0.0;

  //  for coder
  i20 = H->size[0] * H->size[1];
  H->size[0] = 1;
  H->size[1] = 1;
  emxEnsureCapacity((emxArray__common *)H, i20, (int)sizeof(double));
  H->data[0] = 0.0;

  //  for coder
  b_ii = indMeas->size[0];
  i20 = inlierStatus->size[0];
  inlierStatus->size[0] = indMeas->size[0];
  emxEnsureCapacity((emxArray__common *)inlierStatus, i20, (int)sizeof(boolean_T));
  ia = indMeas->size[0];
  for (i20 = 0; i20 < ia; i20++) {
    inlierStatus->data[i20] = true;
  }

  emxInit_real_T(&x_apo, 1);
  b_emxInit_real_T(&map, 2);
  emxInit_real_T(&featureAnchorInd, 1);

  // % debug
  //  persistent res_handle xapo_handle
  //  global P_handle
  //  if isempty(res_handle)
  //      figure('units','normalized','outerposition',[-.25 0 .25 1]);
  //      subplot(3,1,1)
  //      res_handle = plot(zeros(size(z_all_l,1)*2,1), '.-');
  //      subplot(3,1,2)
  //      xapo_handle = plot(zeros(size(10,1)*2,1), '.-');
  //      subplot(3,1,3)
  //      P_handle = imagesc(P~=0);
  //  end
  *b_model_prob = 1.0;

  //     %% Perform the update with the existing features
  getMap(b_xt, b_anchorFeatures, b_m_vect, (double)b_anchorFeatures->size[1] *
         b_numPointsPerAnchor, c_numStatesxt, 7.0 + b_numPointsPerAnchor, map,
         x_apo, featureAnchorInd);
  nx = 0;
  for (k = 0; k < b_ii; k++) {
    nx++;
  }

  b_emxInit_real_T(&S, 2);
  b_emxInit_real_T(&C, 2);
  b_emxInit_real_T(&b, 2);
  if (nx != 0) {
    b_ii = indMeas->size[0];
    nx = b_ii - 1;
    b_ii = 0;
    for (idx = 0; idx <= nx; idx++) {
      b_ii++;
    }

    i20 = ii->size[0];
    ii->size[0] = b_ii;
    emxEnsureCapacity((emxArray__common *)ii, i20, (int)sizeof(int));
    b_ii = 0;
    for (idx = 0; idx <= nx; idx++) {
      ii->data[b_ii] = idx + 1;
      b_ii++;
    }

    emxInit_real_T(&b_indMeas, 1);
    i20 = b_indMeas->size[0];
    b_indMeas->size[0] = ii->size[0];
    emxEnsureCapacity((emxArray__common *)b_indMeas, i20, (int)sizeof(double));
    ia = ii->size[0];
    for (i20 = 0; i20 < ia; i20++) {
      b_indMeas->data[i20] = indMeas->data[ii->data[i20] - 1];
    }

    emxInit_real_T(&r, 1);
    emxInit_real_T(&unusedU0, 1);
    b_emxInit_real_T(&R, 2);
    getH_R_res(b_xt, c_numStates, c_numStatesxt, z_all_l, b_indMeas, map,
               c_cameraparams_CameraParameters, d_cameraparams_CameraParameters,
               e_cameraparams_CameraParameters, (double)b_anchorFeatures->size[1],
               b_numPointsPerAnchor, x_apo, featureAnchorInd, b_m_vect,
               b_imNoise, IMU_measurements, r, H, unusedU0, R);
    emxFree_real_T(&b_indMeas);
    emxFree_real_T(&unusedU0);
    if ((H->size[1] == 1) || (b_P->size[0] == 1)) {
      i20 = K->size[0] * K->size[1];
      K->size[0] = H->size[0];
      K->size[1] = b_P->size[1];
      emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
      ia = H->size[0];
      for (i20 = 0; i20 < ia; i20++) {
        nx = b_P->size[1];
        for (i21 = 0; i21 < nx; i21++) {
          K->data[i20 + K->size[0] * i21] = 0.0;
          b_ii = H->size[1];
          for (ib = 0; ib < b_ii; ib++) {
            K->data[i20 + K->size[0] * i21] += H->data[i20 + H->size[0] * ib] *
              b_P->data[ib + b_P->size[0] * i21];
          }
        }
      }
    } else {
      k = H->size[1];
      a[0] = H->size[0];
      a[1] = b_P->size[1];
      m = H->size[0];
      i20 = K->size[0] * K->size[1];
      K->size[0] = (int)a[0];
      emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
      i20 = K->size[0] * K->size[1];
      K->size[1] = (int)a[1];
      emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
      ia = (int)a[0] * (int)a[1];
      for (i20 = 0; i20 < ia; i20++) {
        K->data[i20] = 0.0;
      }

      if ((H->size[0] == 0) || (b_P->size[1] == 0)) {
      } else {
        b_ii = H->size[0] * (b_P->size[1] - 1);
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          i20 = nx + m;
          for (ic = nx; ic + 1 <= i20; ic++) {
            K->data[ic] = 0.0;
          }

          nx += m;
        }

        idx = 0;
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          ar = 0;
          i20 = idx + k;
          for (ib = idx; ib + 1 <= i20; ib++) {
            if (b_P->data[ib] != 0.0) {
              ia = ar;
              i21 = nx + m;
              for (ic = nx; ic + 1 <= i21; ic++) {
                ia++;
                K->data[ic] += b_P->data[ib] * H->data[ia - 1];
              }
            }

            ar += m;
          }

          idx += k;
          nx += m;
        }
      }
    }

    i20 = b->size[0] * b->size[1];
    b->size[0] = H->size[1];
    b->size[1] = H->size[0];
    emxEnsureCapacity((emxArray__common *)b, i20, (int)sizeof(double));
    ia = H->size[0];
    for (i20 = 0; i20 < ia; i20++) {
      nx = H->size[1];
      for (i21 = 0; i21 < nx; i21++) {
        b->data[i21 + b->size[0] * i20] = H->data[i20 + H->size[0] * i21];
      }
    }

    if ((K->size[1] == 1) || (b->size[0] == 1)) {
      i20 = S->size[0] * S->size[1];
      S->size[0] = K->size[0];
      S->size[1] = b->size[1];
      emxEnsureCapacity((emxArray__common *)S, i20, (int)sizeof(double));
      ia = K->size[0];
      for (i20 = 0; i20 < ia; i20++) {
        nx = b->size[1];
        for (i21 = 0; i21 < nx; i21++) {
          S->data[i20 + S->size[0] * i21] = 0.0;
          b_ii = K->size[1];
          for (ib = 0; ib < b_ii; ib++) {
            S->data[i20 + S->size[0] * i21] += K->data[i20 + K->size[0] * ib] *
              b->data[ib + b->size[0] * i21];
          }
        }
      }
    } else {
      k = K->size[1];
      a[0] = (unsigned int)K->size[0];
      a[1] = (unsigned int)b->size[1];
      m = K->size[0];
      i20 = S->size[0] * S->size[1];
      S->size[0] = (int)a[0];
      emxEnsureCapacity((emxArray__common *)S, i20, (int)sizeof(double));
      i20 = S->size[0] * S->size[1];
      S->size[1] = (int)a[1];
      emxEnsureCapacity((emxArray__common *)S, i20, (int)sizeof(double));
      ia = (int)a[0] * (int)a[1];
      for (i20 = 0; i20 < ia; i20++) {
        S->data[i20] = 0.0;
      }

      if ((K->size[0] == 0) || (b->size[1] == 0)) {
      } else {
        b_ii = K->size[0] * (b->size[1] - 1);
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          i20 = nx + m;
          for (ic = nx; ic + 1 <= i20; ic++) {
            S->data[ic] = 0.0;
          }

          nx += m;
        }

        idx = 0;
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          ar = 0;
          i20 = idx + k;
          for (ib = idx; ib + 1 <= i20; ib++) {
            if (b->data[ib] != 0.0) {
              ia = ar;
              i21 = nx + m;
              for (ic = nx; ic + 1 <= i21; ic++) {
                ia++;
                S->data[ic] += b->data[ib] * K->data[ia - 1];
              }
            }

            ar += m;
          }

          idx += k;
          nx += m;
        }
      }
    }

    i20 = S->size[0] * S->size[1];
    emxEnsureCapacity((emxArray__common *)S, i20, (int)sizeof(double));
    b_ii = S->size[0];
    nx = S->size[1];
    ia = b_ii * nx;
    for (i20 = 0; i20 < ia; i20++) {
      S->data[i20] += R->data[i20];
    }

    b_emxInit_real_T(&b_a, 2);
    b_x = 6.2831853071795862 * c_norm(S);
    i20 = b_a->size[0] * b_a->size[1];
    b_a->size[0] = 1;
    b_a->size[1] = r->size[0];
    emxEnsureCapacity((emxArray__common *)b_a, i20, (int)sizeof(double));
    ia = r->size[0];
    for (i20 = 0; i20 < ia; i20++) {
      b_a->data[b_a->size[0] * i20] = -0.5 * r->data[i20];
    }

    b_mrdivide(b_a, S);
    if ((b_a->size[1] == 1) || (r->size[0] == 1)) {
      y = 0.0;
      for (i20 = 0; i20 < b_a->size[1]; i20++) {
        y += b_a->data[b_a->size[0] * i20] * r->data[i20];
      }
    } else {
      y = 0.0;
      for (i20 = 0; i20 < b_a->size[1]; i20++) {
        y += b_a->data[b_a->size[0] * i20] * r->data[i20];
      }
    }

    emxFree_real_T(&b_a);
    *b_model_prob = 1.0 / sqrt(b_x) * exp(y);
    if (*b_model_prob > 1.0) {
      //                  error('debug')
      d_fprintf(*b_model_prob);
    }

    //  outlier rejection
    for (idx = 0; idx < indMeas->size[0]; idx++) {
      b_x = ((1.0 + (double)idx) - 1.0) * 2.0;
      for (i20 = 0; i20 < 2; i20++) {
        b_r[i20] = r->data[(int)(b_x + (1.0 + (double)i20)) - 1];
      }

      b_x = ((1.0 + (double)idx) - 1.0) * 2.0;
      y = ((1.0 + (double)idx) - 1.0) * 2.0;
      for (i20 = 0; i20 < 2; i20++) {
        for (i21 = 0; i21 < 2; i21++) {
          b_S[i21 + (i20 << 1)] = S->data[((int)(b_x + (1.0 + (double)i21)) +
            S->size[0] * ((int)(y + (1.0 + (double)i20)) - 1)) - 1];
        }
      }

      mrdivide(b_r, b_S, a);
      b_x = ((1.0 + (double)idx) - 1.0) * 2.0;
      for (i20 = 0; i20 < 2; i20++) {
        b_r[i20] = r->data[(int)(b_x + (1.0 + (double)i20)) - 1];
      }

      //  mahalanobis distance
      b_x = 0.0;
      for (i20 = 0; i20 < 2; i20++) {
        b_x += a[i20] * b_r[i20];
      }

      if (b_x > 9.487729036781154) {
        //                  fprintf('Rejecting feature %i due to mahalanobis (%f > %f), residual: (%f, %f)\n', int32(j), mal, mal_thresh, r((i-1)*measDim + 1), r((i-1)*measDim + 2)); 
        inlierStatus->data[idx] = false;
        ia = H->size[1];
        b_x = ((1.0 + (double)idx) - 1.0) * 2.0;
        for (i20 = 0; i20 < ia; i20++) {
          for (i21 = 0; i21 < 2; i21++) {
            H->data[((int)(b_x + (1.0 + (double)i21)) + H->size[0] * i20) - 1] =
              0.0;
          }
        }

        b_x = ((1.0 + (double)idx) - 1.0) * 2.0;
        for (i20 = 0; i20 < 2; i20++) {
          r->data[(int)(b_x + (1.0 + (double)i20)) - 1] = 0.0;
        }
      }
    }

    if ((H->size[1] == 1) || (b_P->size[0] == 1)) {
      i20 = K->size[0] * K->size[1];
      K->size[0] = H->size[0];
      K->size[1] = b_P->size[1];
      emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
      ia = H->size[0];
      for (i20 = 0; i20 < ia; i20++) {
        nx = b_P->size[1];
        for (i21 = 0; i21 < nx; i21++) {
          K->data[i20 + K->size[0] * i21] = 0.0;
          b_ii = H->size[1];
          for (ib = 0; ib < b_ii; ib++) {
            K->data[i20 + K->size[0] * i21] += H->data[i20 + H->size[0] * ib] *
              b_P->data[ib + b_P->size[0] * i21];
          }
        }
      }
    } else {
      k = H->size[1];
      a[0] = H->size[0];
      a[1] = b_P->size[1];
      m = H->size[0];
      i20 = K->size[0] * K->size[1];
      K->size[0] = (int)a[0];
      emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
      i20 = K->size[0] * K->size[1];
      K->size[1] = (int)a[1];
      emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
      ia = (int)a[0] * (int)a[1];
      for (i20 = 0; i20 < ia; i20++) {
        K->data[i20] = 0.0;
      }

      if ((H->size[0] == 0) || (b_P->size[1] == 0)) {
      } else {
        b_ii = H->size[0] * (b_P->size[1] - 1);
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          i20 = nx + m;
          for (ic = nx; ic + 1 <= i20; ic++) {
            K->data[ic] = 0.0;
          }

          nx += m;
        }

        idx = 0;
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          ar = 0;
          i20 = idx + k;
          for (ib = idx; ib + 1 <= i20; ib++) {
            if (b_P->data[ib] != 0.0) {
              ia = ar;
              i21 = nx + m;
              for (ic = nx; ic + 1 <= i21; ic++) {
                ia++;
                K->data[ic] += b_P->data[ib] * H->data[ia - 1];
              }
            }

            ar += m;
          }

          idx += k;
          nx += m;
        }
      }
    }

    i20 = b->size[0] * b->size[1];
    b->size[0] = H->size[1];
    b->size[1] = H->size[0];
    emxEnsureCapacity((emxArray__common *)b, i20, (int)sizeof(double));
    ia = H->size[0];
    for (i20 = 0; i20 < ia; i20++) {
      nx = H->size[1];
      for (i21 = 0; i21 < nx; i21++) {
        b->data[i21 + b->size[0] * i20] = H->data[i20 + H->size[0] * i21];
      }
    }

    if ((K->size[1] == 1) || (b->size[0] == 1)) {
      i20 = C->size[0] * C->size[1];
      C->size[0] = K->size[0];
      C->size[1] = b->size[1];
      emxEnsureCapacity((emxArray__common *)C, i20, (int)sizeof(double));
      ia = K->size[0];
      for (i20 = 0; i20 < ia; i20++) {
        nx = b->size[1];
        for (i21 = 0; i21 < nx; i21++) {
          C->data[i20 + C->size[0] * i21] = 0.0;
          b_ii = K->size[1];
          for (ib = 0; ib < b_ii; ib++) {
            C->data[i20 + C->size[0] * i21] += K->data[i20 + K->size[0] * ib] *
              b->data[ib + b->size[0] * i21];
          }
        }
      }
    } else {
      k = K->size[1];
      a[0] = (unsigned int)K->size[0];
      a[1] = (unsigned int)b->size[1];
      m = K->size[0];
      i20 = C->size[0] * C->size[1];
      C->size[0] = (int)a[0];
      emxEnsureCapacity((emxArray__common *)C, i20, (int)sizeof(double));
      i20 = C->size[0] * C->size[1];
      C->size[1] = (int)a[1];
      emxEnsureCapacity((emxArray__common *)C, i20, (int)sizeof(double));
      ia = (int)a[0] * (int)a[1];
      for (i20 = 0; i20 < ia; i20++) {
        C->data[i20] = 0.0;
      }

      if ((K->size[0] == 0) || (b->size[1] == 0)) {
      } else {
        b_ii = K->size[0] * (b->size[1] - 1);
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          i20 = nx + m;
          for (ic = nx; ic + 1 <= i20; ic++) {
            C->data[ic] = 0.0;
          }

          nx += m;
        }

        idx = 0;
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          ar = 0;
          i20 = idx + k;
          for (ib = idx; ib + 1 <= i20; ib++) {
            if (b->data[ib] != 0.0) {
              ia = ar;
              i21 = nx + m;
              for (ic = nx; ic + 1 <= i21; ic++) {
                ia++;
                C->data[ic] += b->data[ib] * K->data[ia - 1];
              }
            }

            ar += m;
          }

          idx += k;
          nx += m;
        }
      }
    }

    i20 = b->size[0] * b->size[1];
    b->size[0] = H->size[1];
    b->size[1] = H->size[0];
    emxEnsureCapacity((emxArray__common *)b, i20, (int)sizeof(double));
    ia = H->size[0];
    for (i20 = 0; i20 < ia; i20++) {
      nx = H->size[1];
      for (i21 = 0; i21 < nx; i21++) {
        b->data[i21 + b->size[0] * i20] = H->data[i20 + H->size[0] * i21];
      }
    }

    if ((b_P->size[1] == 1) || (b->size[0] == 1)) {
      i20 = K->size[0] * K->size[1];
      K->size[0] = b_P->size[0];
      K->size[1] = b->size[1];
      emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
      ia = b_P->size[0];
      for (i20 = 0; i20 < ia; i20++) {
        nx = b->size[1];
        for (i21 = 0; i21 < nx; i21++) {
          K->data[i20 + K->size[0] * i21] = 0.0;
          b_ii = b_P->size[1];
          for (ib = 0; ib < b_ii; ib++) {
            K->data[i20 + K->size[0] * i21] += b_P->data[i20 + b_P->size[0] * ib]
              * b->data[ib + b->size[0] * i21];
          }
        }
      }
    } else {
      k = b_P->size[1];
      a[0] = b_P->size[0];
      a[1] = b->size[1];
      m = b_P->size[0];
      i20 = K->size[0] * K->size[1];
      K->size[0] = (int)a[0];
      emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
      i20 = K->size[0] * K->size[1];
      K->size[1] = (int)a[1];
      emxEnsureCapacity((emxArray__common *)K, i20, (int)sizeof(double));
      ia = (int)a[0] * (int)a[1];
      for (i20 = 0; i20 < ia; i20++) {
        K->data[i20] = 0.0;
      }

      if ((b_P->size[0] == 0) || (b->size[1] == 0)) {
      } else {
        b_ii = b_P->size[0] * (b->size[1] - 1);
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          i20 = nx + m;
          for (ic = nx; ic + 1 <= i20; ic++) {
            K->data[ic] = 0.0;
          }

          nx += m;
        }

        idx = 0;
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          ar = 0;
          i20 = idx + k;
          for (ib = idx; ib + 1 <= i20; ib++) {
            if (b->data[ib] != 0.0) {
              ia = ar;
              i21 = nx + m;
              for (ic = nx; ic + 1 <= i21; ic++) {
                ia++;
                K->data[ic] += b->data[ib] * b_P->data[ia - 1];
              }
            }

            ar += m;
          }

          idx += k;
          nx += m;
        }
      }
    }

    b_emxInit_real_T(&b_C, 2);
    i20 = b_C->size[0] * b_C->size[1];
    b_C->size[0] = C->size[0];
    b_C->size[1] = C->size[1];
    emxEnsureCapacity((emxArray__common *)b_C, i20, (int)sizeof(double));
    ia = C->size[0] * C->size[1];
    for (i20 = 0; i20 < ia; i20++) {
      b_C->data[i20] = C->data[i20] + R->data[i20];
    }

    emxFree_real_T(&R);
    c_mrdivide(K, b_C);
    emxFree_real_T(&b_C);
    if ((K->size[1] == 1) || (r->size[0] == 1)) {
      i20 = x_apo->size[0];
      x_apo->size[0] = K->size[0];
      emxEnsureCapacity((emxArray__common *)x_apo, i20, (int)sizeof(double));
      ia = K->size[0];
      for (i20 = 0; i20 < ia; i20++) {
        x_apo->data[i20] = 0.0;
        nx = K->size[1];
        for (i21 = 0; i21 < nx; i21++) {
          x_apo->data[i20] += K->data[i20 + K->size[0] * i21] * r->data[i21];
        }
      }
    } else {
      k = K->size[1];
      a[0] = K->size[0];
      m = K->size[0];
      i20 = x_apo->size[0];
      x_apo->size[0] = (int)a[0];
      emxEnsureCapacity((emxArray__common *)x_apo, i20, (int)sizeof(double));
      ia = (int)a[0];
      for (i20 = 0; i20 < ia; i20++) {
        x_apo->data[i20] = 0.0;
      }

      if (K->size[0] == 0) {
      } else {
        nx = 0;
        while ((m > 0) && (nx <= 0)) {
          for (ic = 1; ic <= m; ic++) {
            x_apo->data[ic - 1] = 0.0;
          }

          nx = m;
        }

        idx = 0;
        nx = 0;
        while ((m > 0) && (nx <= 0)) {
          ar = 0;
          i20 = idx + k;
          for (ib = idx; ib + 1 <= i20; ib++) {
            if (r->data[ib] != 0.0) {
              ia = ar;
              for (ic = 0; ic + 1 <= m; ic++) {
                ia++;
                x_apo->data[ic] += r->data[ib] * K->data[ia - 1];
              }
            }

            ar += m;
          }

          idx += k;
          nx = m;
        }
      }
    }

    emxFree_real_T(&r);

    //      P_apo = (eye(numStates+numStatesFeatures)-K*H)*P_apr;
    for (i20 = 0; i20 < 3; i20++) {
      x->data[i20] = b_xt->data[i20] + x_apo->data[i20];
    }

    for (i20 = 0; i20 < 3; i20++) {
      b_x_apo[i20] = x_apo->data[3 + i20];
    }

    quatPlusThetaJ(b_x_apo, dv21);
    quatmultJ(dv21, *(double (*)[4])&x->data[3], b_S);
    for (i20 = 0; i20 < 4; i20++) {
      x->data[3 + i20] = b_S[i20];
    }

    if (8.0 > c_numStatesxt) {
      i20 = 1;
      i21 = 0;
    } else {
      i20 = 8;
      i21 = (int)c_numStatesxt;
    }

    if (7.0 > c_numStates) {
      ib = 1;
      nx = 0;
    } else {
      ib = 7;
      nx = (int)c_numStates;
    }

    if (8.0 > c_numStatesxt) {
      idx = 0;
      ar = 0;
    } else {
      idx = 7;
      ar = (int)c_numStatesxt;
    }

    b_emxInit_int32_T(&r30, 2);
    b_ii = r30->size[0] * r30->size[1];
    r30->size[0] = 1;
    r30->size[1] = ar - idx;
    emxEnsureCapacity((emxArray__common *)r30, b_ii, (int)sizeof(int));
    ia = ar - idx;
    for (ar = 0; ar < ia; ar++) {
      r30->data[r30->size[0] * ar] = idx + ar;
    }

    emxInit_int32_T(&r31, 1);
    idx = r31->size[0];
    r31->size[0] = (i21 - i20) + 1;
    emxEnsureCapacity((emxArray__common *)r31, idx, (int)sizeof(int));
    ia = i21 - i20;
    for (i21 = 0; i21 <= ia; i21++) {
      r31->data[i21] = i20 + i21;
    }

    emxInit_int32_T(&r32, 1);
    i20 = r32->size[0];
    r32->size[0] = (nx - ib) + 1;
    emxEnsureCapacity((emxArray__common *)r32, i20, (int)sizeof(int));
    ia = nx - ib;
    for (i20 = 0; i20 <= ia; i20++) {
      r32->data[i20] = ib + i20;
    }

    emxInit_real_T(&c_x, 1);
    i20 = c_x->size[0];
    c_x->size[0] = r30->size[0] * r30->size[1];
    emxEnsureCapacity((emxArray__common *)c_x, i20, (int)sizeof(double));
    ia = r30->size[0] * r30->size[1];
    for (i20 = 0; i20 < ia; i20++) {
      c_x->data[i20] = x->data[r31->data[i20] - 1] + x_apo->data[r32->data[i20]
        - 1];
    }

    emxFree_int32_T(&r32);
    emxFree_int32_T(&r31);
    ia = c_x->size[0];
    for (i20 = 0; i20 < ia; i20++) {
      x->data[r30->data[i20]] = c_x->data[i20];
    }

    emxFree_real_T(&c_x);
    emxFree_int32_T(&r30);
    nx = 0;
    b_emxInit_boolean_T(&d_anchorFeatures, 1);
    while (nx <= b_anchorFeatures->size[1] - 1) {
      ia = b_anchorFeatures->size[0];
      i20 = d_anchorFeatures->size[0];
      d_anchorFeatures->size[0] = ia;
      emxEnsureCapacity((emxArray__common *)d_anchorFeatures, i20, (int)sizeof
                        (boolean_T));
      for (i20 = 0; i20 < ia; i20++) {
        d_anchorFeatures->data[i20] = (b_anchorFeatures->data[i20 +
          b_anchorFeatures->size[0] * nx] == 1.0);
      }

      if (any(d_anchorFeatures)) {
        b_x = c_numStatesxt + ((1.0 + (double)nx) - 1.0) * (7.0 +
          b_numPointsPerAnchor);
        y = c_numStatesxt + ((1.0 + (double)nx) - 1.0) * (7.0 +
          b_numPointsPerAnchor);
        d_numStates = c_numStates + ((1.0 + (double)nx) - 1.0) * (6.0 +
          b_numPointsPerAnchor);
        for (i20 = 0; i20 < 3; i20++) {
          b_x_apo[i20] = x->data[(int)(y + (1.0 + (double)i20)) - 1] +
            x_apo->data[(int)(d_numStates + (1.0 + (double)i20)) - 1];
        }

        for (i20 = 0; i20 < 3; i20++) {
          x->data[(int)(b_x + (1.0 + (double)i20)) - 1] = b_x_apo[i20];
        }

        d_numStates = c_numStates + ((1.0 + (double)nx) - 1.0) * (6.0 +
          b_numPointsPerAnchor);
        for (i20 = 0; i20 < 3; i20++) {
          b_x_apo[i20] = x_apo->data[(int)(d_numStates + (4.0 + (double)i20)) -
            1];
        }

        b_x = c_numStatesxt + ((1.0 + (double)nx) - 1.0) * (7.0 +
          b_numPointsPerAnchor);
        for (i20 = 0; i20 < 4; i20++) {
          d_x[i20] = x->data[(int)(b_x + (4.0 + (double)i20)) - 1];
        }

        quatPlusThetaJ(b_x_apo, dv22);
        quatmultJ(dv22, d_x, b_S);
        b_x = c_numStatesxt + ((1.0 + (double)nx) - 1.0) * (7.0 +
          b_numPointsPerAnchor);
        for (i20 = 0; i20 < 4; i20++) {
          x->data[(int)(b_x + (4.0 + (double)i20)) - 1] = b_S[i20];
        }

        for (b_ii = 0; b_ii < (int)b_numPointsPerAnchor; b_ii++) {
          x->data[(int)(((c_numStatesxt + ((1.0 + (double)nx) - 1.0) * (7.0 +
            b_numPointsPerAnchor)) + 7.0) + (1.0 + (double)b_ii)) - 1] +=
            x_apo->data[(int)(((c_numStates + ((1.0 + (double)nx) - 1.0) * (6.0
            + b_numPointsPerAnchor)) + 6.0) + (1.0 + (double)b_ii)) - 1];
        }
      }

      nx++;
    }

    emxFree_boolean_T(&d_anchorFeatures);
  }

  emxFree_int32_T(&ii);
  emxFree_real_T(&featureAnchorInd);
  emxFree_real_T(&map);
  emxFree_real_T(&x_apo);
  b_ii = 0;
  for (k = 0; k < inlierStatus->size[0]; k++) {
    if (inlierStatus->data[k]) {
      b_ii++;
    }
  }

  if (b_ii != 0) {
    i20 = b_xt->size[0];
    b_xt->size[0] = x->size[0];
    emxEnsureCapacity((emxArray__common *)b_xt, i20, (int)sizeof(double));
    ia = x->size[0];
    for (i20 = 0; i20 < ia; i20++) {
      b_xt->data[i20] = x->data[i20];
    }

    eye(c_numStates + (double)b_anchorFeatures->size[1] * (6.0 +
         b_numPointsPerAnchor), S);
    if ((K->size[1] == 1) || (H->size[0] == 1)) {
      i20 = C->size[0] * C->size[1];
      C->size[0] = K->size[0];
      C->size[1] = H->size[1];
      emxEnsureCapacity((emxArray__common *)C, i20, (int)sizeof(double));
      ia = K->size[0];
      for (i20 = 0; i20 < ia; i20++) {
        nx = H->size[1];
        for (i21 = 0; i21 < nx; i21++) {
          C->data[i20 + C->size[0] * i21] = 0.0;
          b_ii = K->size[1];
          for (ib = 0; ib < b_ii; ib++) {
            C->data[i20 + C->size[0] * i21] += K->data[i20 + K->size[0] * ib] *
              H->data[ib + H->size[0] * i21];
          }
        }
      }
    } else {
      k = K->size[1];
      a[0] = (unsigned int)K->size[0];
      a[1] = (unsigned int)H->size[1];
      m = K->size[0];
      i20 = C->size[0] * C->size[1];
      C->size[0] = (int)a[0];
      emxEnsureCapacity((emxArray__common *)C, i20, (int)sizeof(double));
      i20 = C->size[0] * C->size[1];
      C->size[1] = (int)a[1];
      emxEnsureCapacity((emxArray__common *)C, i20, (int)sizeof(double));
      ia = (int)a[0] * (int)a[1];
      for (i20 = 0; i20 < ia; i20++) {
        C->data[i20] = 0.0;
      }

      if ((K->size[0] == 0) || (H->size[1] == 0)) {
      } else {
        b_ii = K->size[0] * (H->size[1] - 1);
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          i20 = nx + m;
          for (ic = nx; ic + 1 <= i20; ic++) {
            C->data[ic] = 0.0;
          }

          nx += m;
        }

        idx = 0;
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          ar = 0;
          i20 = idx + k;
          for (ib = idx; ib + 1 <= i20; ib++) {
            if (H->data[ib] != 0.0) {
              ia = ar;
              i21 = nx + m;
              for (ic = nx; ic + 1 <= i21; ic++) {
                ia++;
                C->data[ic] += H->data[ib] * K->data[ia - 1];
              }
            }

            ar += m;
          }

          idx += k;
          nx += m;
        }
      }
    }

    i20 = S->size[0] * S->size[1];
    emxEnsureCapacity((emxArray__common *)S, i20, (int)sizeof(double));
    b_ii = S->size[0];
    nx = S->size[1];
    ia = b_ii * nx;
    for (i20 = 0; i20 < ia; i20++) {
      S->data[i20] -= C->data[i20];
    }

    i20 = b->size[0] * b->size[1];
    b->size[0] = b_P->size[0];
    b->size[1] = b_P->size[1];
    emxEnsureCapacity((emxArray__common *)b, i20, (int)sizeof(double));
    ia = b_P->size[0] * b_P->size[1];
    for (i20 = 0; i20 < ia; i20++) {
      b->data[i20] = b_P->data[i20];
    }

    b_emxInit_real_T(&c_S, 2);
    if ((S->size[1] == 1) || (b_P->size[0] == 1)) {
      i20 = c_S->size[0] * c_S->size[1];
      c_S->size[0] = S->size[0];
      c_S->size[1] = b_P->size[1];
      emxEnsureCapacity((emxArray__common *)c_S, i20, (int)sizeof(double));
      ia = S->size[0];
      for (i20 = 0; i20 < ia; i20++) {
        nx = b_P->size[1];
        for (i21 = 0; i21 < nx; i21++) {
          c_S->data[i20 + c_S->size[0] * i21] = 0.0;
          b_ii = S->size[1];
          for (ib = 0; ib < b_ii; ib++) {
            c_S->data[i20 + c_S->size[0] * i21] += S->data[i20 + S->size[0] * ib]
              * b_P->data[ib + b_P->size[0] * i21];
          }
        }
      }

      i20 = b_P->size[0] * b_P->size[1];
      b_P->size[0] = c_S->size[0];
      b_P->size[1] = c_S->size[1];
      emxEnsureCapacity((emxArray__common *)b_P, i20, (int)sizeof(double));
      ia = c_S->size[1];
      for (i20 = 0; i20 < ia; i20++) {
        nx = c_S->size[0];
        for (i21 = 0; i21 < nx; i21++) {
          b_P->data[i21 + b_P->size[0] * i20] = c_S->data[i21 + c_S->size[0] *
            i20];
        }
      }
    } else {
      k = S->size[1];
      a[0] = S->size[0];
      a[1] = b_P->size[1];
      m = S->size[0];
      i20 = b_P->size[0] * b_P->size[1];
      b_P->size[0] = (int)a[0];
      b_P->size[1] = (int)a[1];
      emxEnsureCapacity((emxArray__common *)b_P, i20, (int)sizeof(double));
      ia = (int)a[1];
      for (i20 = 0; i20 < ia; i20++) {
        nx = (int)a[0];
        for (i21 = 0; i21 < nx; i21++) {
          b_P->data[i21 + b_P->size[0] * i20] = 0.0;
        }
      }

      if ((S->size[0] == 0) || (b->size[1] == 0)) {
      } else {
        b_ii = S->size[0] * (b->size[1] - 1);
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          i20 = nx + m;
          for (ic = nx; ic + 1 <= i20; ic++) {
            b_P->data[ic] = 0.0;
          }

          nx += m;
        }

        idx = 0;
        nx = 0;
        while ((m > 0) && (nx <= b_ii)) {
          ar = 0;
          i20 = idx + k;
          for (ib = idx; ib + 1 <= i20; ib++) {
            if (b->data[ib] != 0.0) {
              ia = ar;
              i21 = nx + m;
              for (ic = nx; ic + 1 <= i21; ic++) {
                ia++;
                b_P->data[ic] += b->data[ib] * S->data[ia - 1];
              }
            }

            ar += m;
          }

          idx += k;
          nx += m;
        }
      }
    }

    emxFree_real_T(&c_S);
  }

  emxFree_real_T(&b);
  emxFree_real_T(&C);
  emxFree_real_T(&S);
  emxFree_real_T(&H);
  emxFree_real_T(&K);
  emxFree_real_T(&x);
  nx = inlierStatus->size[0] - 1;
  b_ii = 0;
  for (idx = 0; idx <= nx; idx++) {
    if (inlierStatus->data[idx]) {
      b_ii++;
    }
  }

  i20 = validFeatures->size[0];
  validFeatures->size[0] = b_ii;
  emxEnsureCapacity((emxArray__common *)validFeatures, i20, (int)sizeof(double));
  b_ii = 0;
  for (idx = 0; idx <= nx; idx++) {
    if (inlierStatus->data[idx]) {
      validFeatures->data[b_ii] = indMeas->data[idx];
      b_ii++;
    }
  }

  emxFree_boolean_T(&inlierStatus);
  emxFree_real_T(&indMeas);
}

//
// File trailer for Mahalanobis_EKF.cpp
//
// [EOF]
//
