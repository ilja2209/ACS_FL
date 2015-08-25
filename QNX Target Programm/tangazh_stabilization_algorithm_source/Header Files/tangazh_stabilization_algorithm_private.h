
#ifndef RTW_HEADER_tangazh_stabilization_algorithm_private_h_
#define RTW_HEADER_tangazh_stabilization_algorithm_private_h_
#include "rtwtypes.h"

#ifndef rtmIsMajorTimeStep
# define rtmIsMajorTimeStep(rtm)       (((rtm)->Timing.simTimeStep) == MAJOR_TIME_STEP)
#endif

#ifndef rtmIsMinorTimeStep
# define rtmIsMinorTimeStep(rtm)       (((rtm)->Timing.simTimeStep) == MINOR_TIME_STEP)
#endif

#ifndef rtmSetTFinal
# define rtmSetTFinal(rtm, val)        ((rtm)->Timing.tFinal = (val))
#endif

#ifndef rtmGetTPtr
# define rtmGetTPtr(rtm)               ((rtm)->Timing.t)
#endif

#ifndef rtmSetTPtr
# define rtmSetTPtr(rtm, val)          ((rtm)->Timing.t = (val))
#endif

#ifndef __RTWTYPES_H__
#error This file requires rtwtypes.h to be included
#else
#ifdef TMWTYPES_PREVIOUSLY_INCLUDED
#error This file requires rtwtypes.h to be included before tmwtypes.h
#else

#ifndef RTWTYPES_ID_C08S16I32L32N32F1
#error This code was generated with a different "rtwtypes.h" than the file included
#endif 
#endif
#endif

extern void tangazh_stabilization_algorithm_NEGATIVEEdge(real_T rtu_0, boolean_T rtu_INprevious,
  boolean_T rtu_IN, RT_MODEL_tangazh_stabilization_algorithm * const tangazh_stabilization_algorithm_M,
  rtB_NEGATIVEEdge_tangazh_stabilization_algorithm *localB, rtDW_NEGATIVEEdge_tangazh_stabilization_algorithm *localDW);
extern void tangazh_stabilization_algorithm_POSITIVEEdge(real_T rtu_0, boolean_T rtu_IN, boolean_T
  rtu_INprevious, RT_MODEL_tangazh_stabilization_algorithm * const tangazh_stabilization_algorithm_M,
  rtB_POSITIVEEdge_tangazh_stabilization_algorithm *localB, rtDW_POSITIVEEdge_tangazh_stabilization_algorithm *localDW);
extern void tangazh_stabilization_algorithm_OFFDelay_Init(rtDW_OFFDelay_tangazh_stabilization_algorithm *localDW,
  rtP_OFFDelay_tangazh_stabilization_algorithm *localP);
extern void tangazh_stabilization_algorithm_OFFDelay_Start(rtDW_OFFDelay_tangazh_stabilization_algorithm *localDW,
  rtP_OFFDelay_tangazh_stabilization_algorithm *localP);
extern void tangazh_stabilization_algorithm_OFFDelay_Disable(rtDW_OFFDelay_tangazh_stabilization_algorithm *localDW);
extern void tangazh_stabilization_algorithm_OFFDelay_Update(boolean_T rtu_IN, RT_MODEL_tangazh_stabilization_algorithm *
  const tangazh_stabilization_algorithm_M, rtB_OFFDelay_tangazh_stabilization_algorithm *localB, rtDW_OFFDelay_tangazh_stabilization_algorithm
  *localDW);
extern void tangazh_stabilization_algorithm_OFFDelay(boolean_T rtu_0, boolean_T rtu_IN, real_T
  rtu_clock, real_T rtu_DELAY, RT_MODEL_tangazh_stabilization_algorithm * const tangazh_stabilization_algorithm_M,
  rtB_OFFDelay_tangazh_stabilization_algorithm *localB, rtDW_OFFDelay_tangazh_stabilization_algorithm *localDW,
  rtP_OFFDelay_tangazh_stabilization_algorithm *localP);
extern void tangazh_stabilization_algorithm_ONDelay_Init(rtDW_ONDelay_tangazh_stabilization_algorithm *localDW,
  rtP_ONDelay_tangazh_stabilization_algorithm *localP);
extern void tangazh_stabilization_algorithm_ONDelay_Start(rtDW_ONDelay_tangazh_stabilization_algorithm *localDW,
  rtP_ONDelay_tangazh_stabilization_algorithm *localP);
extern void tangazh_stabilization_algorithm_ONDelay_Disable(rtDW_ONDelay_tangazh_stabilization_algorithm *localDW);
extern void tangazh_stabilization_algorithm_ONDelay_Update(boolean_T rtu_IN, RT_MODEL_tangazh_stabilization_algorithm *
  const tangazh_stabilization_algorithm_M, rtB_ONDelay_tangazh_stabilization_algorithm *localB, rtDW_ONDelay_tangazh_stabilization_algorithm
  *localDW);
extern void tangazh_stabilization_algorithm_ONDelay(boolean_T rtu_0, boolean_T rtu_IN, real_T
  rtu_clock, real_T rtu_DELAY, RT_MODEL_tangazh_stabilization_algorithm * const tangazh_stabilization_algorithm_M,
  rtB_ONDelay_tangazh_stabilization_algorithm *localB, rtDW_ONDelay_tangazh_stabilization_algorithm *localDW,
  rtP_ONDelay_tangazh_stabilization_algorithm *localP);

extern void tangazh_stabilization_algorithm_derivatives(void);

#endif        


