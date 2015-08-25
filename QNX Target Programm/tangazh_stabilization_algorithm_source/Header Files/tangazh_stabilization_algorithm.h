
#ifndef RTW_HEADER_tangazh_stabilization_algorithm_h_
#define RTW_HEADER_tangazh_stabilization_algorithm_h_
#ifndef tangazh_stabilization_algorithm_COMMON_INCLUDES_
# define tangazh_stabilization_algorithm_COMMON_INCLUDES_
#include <float.h>
#include <math.h>
#include <string.h>
#include "rtwtypes.h"
#include "rtw_extmode.h"
#include "sysran_types.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "dt_info.h"
#include "ext_work.h"
#include "rt_matrixlib.h"
#endif 

#include "tangazh_stabilization_algorithm_types.h"
#ifndef rtmGetFinalTime
# define rtmGetFinalTime(rtm)          ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
# define rtmSetStopRequested(rtm, val) ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
# define rtmGetStopRequestedPtr(rtm)   (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
# define rtmGetT(rtm)                  (rtmGetTPtr((rtm))[0])
#endif

#ifndef rtmGetTFinal
# define rtmGetTFinal(rtm)             ((rtm)->Timing.tFinal)
#endif
typedef struct {
  boolean_T RelationalOperator1;
} rtB_NEGATIVEEdge_tangazh_stabilization_algorithm;

typedef struct {
  int8_T NEGATIVEEdge_SubsysRanBC;     
} rtDW_NEGATIVEEdge_tangazh_stabilization_algorithm;


typedef struct {
  boolean_T RelationalOperator1;       
} rtB_POSITIVEEdge_tangazh_stabilization_algorithm;


typedef struct {
  int8_T POSITIVEEdge_SubsysRanBC;     
} rtDW_POSITIVEEdge_tangazh_stabilization_algorithm;


typedef struct {
  real_T ICic;  
  real_T Switch;
  real_T Sum;
  boolean_T LogicalOperator1; 
  boolean_T LogicalOperator; 
  boolean_T LogicalOperator2;
  rtB_POSITIVEEdge_tangazh_stabilization_algorithm POSITIVEEdge;
  rtB_NEGATIVEEdge_tangazh_stabilization_algorithm NEGATIVEEdge;
} rtB_OFFDelay_tangazh_stabilization_algorithm;


typedef struct {
  real_T ICic_PreviousInput;
  int8_T OFFDelay_SubsysRanBC;
  boolean_T Memory_PreviousInput;
  boolean_T OFFDelay_MODE;
  rtDW_POSITIVEEdge_tangazh_stabilization_algorithm POSITIVEEdge;
  rtDW_NEGATIVEEdge_tangazh_stabilization_algorithm NEGATIVEEdge;
} rtDW_OFFDelay_tangazh_stabilization_algorithm;

typedef struct {
  real_T ICic;
  real_T Switch;
  real_T Sum; 
  boolean_T LogicalOperator1;
  boolean_T LogicalOperator2;
  rtB_POSITIVEEdge_tangazh_stabilization_algorithm POSITIVEEdge;
  rtB_NEGATIVEEdge_tangazh_stabilization_algorithm NEGATIVEEdge;
} rtB_ONDelay_tangazh_stabilization_algorithm;

typedef struct {
  real_T ICic_PreviousInput;
  int8_T ONDelay_SubsysRanBC;
  boolean_T Memory_PreviousInput;
  boolean_T ONDelay_MODE;
  rtDW_POSITIVEEdge_tangazh_stabilization_algorithm POSITIVEEdge;
  rtDW_NEGATIVEEdge_tangazh_stabilization_algorithm NEGATIVEEdge;
} rtDW_ONDelay_tangazh_stabilization_algorithm;

typedef struct {
  real_T Integrator;
  real_T Divide1; 
  real_T Saturation; 
  real_T SineWave; 
  real_T Switch1; 
  real_T Add2;
  real_T Gain; 
  real_T Switch; 
  boolean_T RelationalOperator1;  
  boolean_T LogicalOperator2;
  boolean_T LogicalOperator;  
  boolean_T RelationalOperator1_h; 
  boolean_T LogicalOperator2_a;
  boolean_T Compare; 
  boolean_T RelationalOperator1_e;
  boolean_T LogicalOperator2_m; 
  boolean_T Memory; 
  boolean_T Logic[2];
  boolean_T LogicalOperator_f; 
  rtB_ONDelay_tangazh_stabilization_algorithm ONDelay;
  rtB_OFFDelay_tangazh_stabilization_algorithm OFFDelay;
  rtB_ONDelay_tangazh_stabilization_algorithm ONDelay_p; 
  rtB_OFFDelay_tangazh_stabilization_algorithm OFFDelay_p; 
  rtB_ONDelay_tangazh_stabilization_algorithm ONDelay_i; 
  rtB_OFFDelay_tangazh_stabilization_algorithm OFFDelay_a; 
} BlockIO_tangazh_stabilization_algorithm;

typedef struct {
  struct {
    void *LoggedData;
  } Scope_PWORK;                      

  boolean_T Memory_PreviousInput; 
  rtDW_ONDelay_tangazh_stabilization_algorithm ONDelay;      
  rtDW_OFFDelay_tangazh_stabilization_algorithm OFFDelay;    
  rtDW_ONDelay_tangazh_stabilization_algorithm ONDelay_p;   
  rtDW_OFFDelay_tangazh_stabilization_algorithm OFFDelay_p; 
  rtDW_ONDelay_tangazh_stabilization_algorithm ONDelay_i;   
  rtDW_OFFDelay_tangazh_stabilization_algorithm OFFDelay_a;
} D_Work_tangazh_stabilization_algorithm;

typedef struct {
  real_T Integrator_CSTATE; 
} ContinuousStates_tangazh_stabilization_algorithm;

typedef struct {
  real_T Integrator_CSTATE;   
} StateDerivatives_tangazh_stabilization_algorithm;

typedef struct {
  boolean_T Integrator_CSTATE;
} StateDisabled_tangazh_stabilization_algorithm;

typedef struct {
  ZCSigState Integrator_Reset_ZCE; 
} PrevZCSigStates_tangazh_stabilization_algorithm;

#ifndef ODE14X_INTG
#define ODE14X_INTG

typedef struct {
  real_T *x0;
  real_T *f0;
  real_T *x1start;
  real_T *f1;
  real_T *Delta;
  real_T *E;
  real_T *fac;
  real_T *DFDX;
  real_T *W;
  int_T *pivots;
  real_T *xtmp;
  real_T *ztmp;
  real_T *M;
  real_T *M1;
  real_T *Edot;
  real_T *xdot;
  real_T *fminusMxdot;
  boolean_T isFirstStep;
} ODE14X_IntgData;

#endif

struct rtP_OFFDelay_tangazh_stabilization_algorithm_ {
  real_T eitheredge_Value[2];
  real_T negedge_Value[2];
  real_T posedge_Value[2]; 
  real_T Constant1_Value; 
  real_T ICic_X0; 
  boolean_T Memory_X0;
};

struct rtP_ONDelay_tangazh_stabilization_algorithm_ {
  real_T eitheredge_Value[2]; 
  real_T negedge_Value[2]; 
  real_T posedge_Value[2]; 
  real_T Constant1_Value;
  real_T ICic_X0;
  boolean_T Memory_X0;
};

struct Parameters_tangazh_stabilization_algorithm_ {
  real_T Constant1_Value;
  real_T Constant2_Value;
  real_T Constant_Value; 
  real_T Constant1_Value_m;
  real_T Constant2_Value_j;
  real_T Constant_Value_p;
  real_T Constant7_Value; 
  real_T Constant_Value_f;
  real_T Constant1_Value_h;
  real_T Constant2_Value_f;
  real_T Constant_Value_g;
  real_T Integrator_IC; 
  real_T Constant3_Value;
  real_T Constant8_Value;
  real_T Constant9_Value;
  real_T Constant2_Value_c; 
  real_T Saturation_UpperSat; 
  real_T Saturation_LowerSat; 
  real_T SineWave_Amp;
  real_T SineWave_Bias; 
  real_T SineWave_Freq;
  real_T SineWave_Phase; 
  real_T Constant_Value_b; 
  real_T Constant1_Value_p; 
  real_T Gain_Gain; 
  boolean_T Constant4_Value; 
  boolean_T Constant_Value_c; 
  boolean_T Constant2_Value_f1; 
  boolean_T Constant5_Value; 
  boolean_T Constant6_Value; 
  boolean_T Memory_X0;
  boolean_T Logic_table[16];
  rtP_ONDelay_tangazh_stabilization_algorithm ONDelay;     
  rtP_OFFDelay_tangazh_stabilization_algorithm OFFDelay; 
  rtP_ONDelay_tangazh_stabilization_algorithm ONDelay_p;   
  rtP_OFFDelay_tangazh_stabilization_algorithm OFFDelay_p; 
  rtP_ONDelay_tangazh_stabilization_algorithm ONDelay_i; 
  rtP_OFFDelay_tangazh_stabilization_algorithm OFFDelay_a;   
};

struct tag_RTM_tangazh_stabilization_algorithm {
  const char_T *errorStatus;
  RTWExtModeInfo *extModeInfo;
  RTWSolverInfo solverInfo;

  struct {
    real_T *contStates;
    real_T *derivs;
    boolean_T *contStateDisabled;
    boolean_T zCCacheNeedsReset;
    boolean_T derivCacheNeedsReset;
    boolean_T blkStateChange;
    real_T odeX0[1];
    real_T odeF0[1];
    real_T odeX1START[1];
    real_T odeF1[1];
    real_T odeDELTA[1];
    real_T odeE[4*1];
    real_T odeFAC[1];
    real_T odeDFDX[1*1];
    real_T odeW[1*1];
    int_T odePIVOTS[1];
    real_T odeXTMP[1];
    real_T odeZTMP[1];
    ODE14X_IntgData intgData;
  } ModelData;

  struct {
    uint32_T checksums[4];
    int_T numContStates;
    int_T numSampTimes;
  } Sizes;


  struct {
    const void *mappingInfo;
  } SpecialInfo;


  struct {
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    uint32_T clockTick1;
    uint32_T clockTickH1;
    time_T tFinal;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

extern Parameters_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_P;
extern BlockIO_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_B;
extern ContinuousStates_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_X;
extern D_Work_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_DWork;
extern PrevZCSigStates_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_PrevZCSigState;
extern void tangazh_stabilization_algorithm_initialize(void);
extern void tangazh_stabilization_algorithm_step(void);
extern RT_MODEL_tangazh_stabilization_algorithm *const tangazh_stabilization_algorithm_M;

#endif  
