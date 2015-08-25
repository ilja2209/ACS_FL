
#include "tangazh_stabilization_algorithm.h"
#include "tangazh_stabilization_algorithm_private.h"
#include "tangazh_stabilization_algorithm_dt.h"

BlockIO_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_B;
ContinuousStates_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_X;

D_Work_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_DWork;
PrevZCSigStates_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_PrevZCSigState;


RT_MODEL_tangazh_stabilization_algorithm tangazh_stabilization_algorithm_M_;
RT_MODEL_tangazh_stabilization_algorithm *const tangazh_stabilization_algorithm_M = &tangazh_stabilization_algorithm_M_;

void local_numjac( RTWSolverInfo *si, real_T *y, const real_T *Fty, real_T *fac,
                  real_T *dFdy )
{
 
  real_T THRESH = 1e-6;
  real_T EPS = 2.2e-16;
  real_T BL = pow(EPS, 0.75);
  real_T BU = pow(EPS, 0.25);
  real_T FACMIN = pow(EPS, 0.78);
  real_T FACMAX = 0.1;
  int_T nx = 1;
  real_T *x = rtsiGetContStates(si);
  real_T del;
  real_T difmax;
  real_T FdelRowmax;
  real_T temp;
  real_T Fdiff;
  real_T maybe;
  real_T xscale;
  real_T fscale;
  real_T *p;
  int_T rowmax;
  int_T i,j;
  if (x != y)
    memcpy(x, y, (uint_T)nx*sizeof(real_T));
  rtsiSetSolverComputingJacobian(si,TRUE);
  for (p = dFdy, j = 0; j < nx; j++, p += nx) {
    xscale = fabs(x[j]);
    if (xscale < THRESH)
      xscale = THRESH;
    temp = (x[j] + fac[j]*xscale);
    del = temp - y[j];
    while (del == 0.0) {
      if (fac[j] < FACMAX) {
        fac[j] *= 100.0;
        if (fac[j] > FACMAX)
          fac[j] = FACMAX;
        temp = (x[j] + fac[j]*xscale);
        del = temp - x[j];
      } else {
        del = THRESH;             
        break;
      }
    }

    if (Fty[j] >= 0.0)
      del = fabs(del);
    else
      del = -fabs(del);

    temp = x[j];
    x[j] += del;
    tangazh_stabilization_algorithm_step();
    rtsiSetdX(si,p);
    tangazh_stabilization_algorithm_derivatives();
    x[j] = temp;
    difmax = 0.0;
    rowmax = 0;
    FdelRowmax = p[0];
    temp = 1.0 / del;
    for (i = 0; i < nx; i++) {
      Fdiff = p[i] - Fty[i];
      maybe = fabs(Fdiff);
      if (maybe > difmax) {
        difmax = maybe;
        rowmax = i;
        FdelRowmax = p[i];
      }

      p[i] = temp * Fdiff;
    }

    if (((FdelRowmax != 0.0) && (Fty[rowmax] != 0.0)) || (difmax == 0.0)) {
      fscale = fabs(FdelRowmax);
      if (fscale < fabs(Fty[rowmax]))
        fscale = fabs(Fty[rowmax]);
      if (difmax <= BL*fscale) {

        fac[j] *= 10.0;
        if (fac[j] > FACMAX)
          fac[j] = FACMAX;
      } else if (difmax > BU*fscale) {

        fac[j] *= 0.1;
        if (fac[j] < FACMIN)
          fac[j] = FACMIN;
      }
    }
  }

  rtsiSetSolverComputingJacobian(si,FALSE);
}                                      

static void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{

  static int_T rt_ODE14x_N[4] = { 12, 8, 6, 4 };

  time_T t0 = rtsiGetT(si);
  time_T t1 = t0;
  time_T h = rtsiGetStepSize(si);
  real_T *x1 = rtsiGetContStates(si);
  int_T order = rtsiGetSolverExtrapolationOrder(si);
  int_T numIter = rtsiGetSolverNumberNewtonIterations(si);
  ODE14X_IntgData *id = (ODE14X_IntgData *)rtsiGetSolverData(si);
  real_T *x0 = id->x0;
  real_T *f0 = id->f0;
  real_T *x1start = id->x1start;
  real_T *f1 = id->f1;
  real_T *Delta = id->Delta;
  real_T *E = id->E;
  real_T *fac = id->fac;
  real_T *dfdx = id->DFDX;
  real_T *W = id->W;
  int_T *pivots = id->pivots;
  real_T *xtmp = id->xtmp;
  real_T *ztmp = id->ztmp;
  int_T *N = &(rt_ODE14x_N[0]);
  int_T i,j,k,iter;
  int_T nx = 1;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  memcpy(x0, x1,(uint_T)nx*sizeof(real_T));

  rtsiSetdX(si, f0);
  tangazh_stabilization_algorithm_derivatives();
  local_numjac(si,x0,f0,fac,dfdx );
  for (j = 0; j < order; j++) {
    real_T *p;
    real_T hN = h/N[j];

    

    //[L,U] = lu(M - hN*J)
    memcpy(W, dfdx,(uint_T)nx*nx*sizeof(real_T));
    for (p = W, i = 0; i < nx*nx; i++, p++) {
      *p *= (-hN);
    }

    for (p = W, i = 0; i < nx; i++, p += (nx+1)) {
      *p += 1.0;
    }

    rt_lu_real(W, nx, pivots);

    for (i = 0; i < nx; i++) {
      Delta[i] = hN*f0[i];
    }

    /* Delta = (U \ (L \ rhs)) */
    rt_ForwardSubstitutionRR_Dbl(W, Delta, f1, nx, 1, pivots, 1);
    rt_BackwardSubstitutionRR_Dbl(W+nx*nx-1, f1+nx-1, Delta, nx, 1, 0);

    /* ytmp = y0 + Delta
       ztmp = (ytmp-y0)/h
     */
    memcpy(x1, x0,(uint_T)nx*sizeof(real_T));
    for (i = 0; i < nx; i++) {
      x1[i] += Delta[i];
      ztmp[i] = Delta[i]/hN;
    }
    rtsiSetT(si, t0);
    rtsiSetdX(si, f1);
    for (iter = 1; iter < numIter; iter++) {
      tangazh_stabilization_algorithm_step();
      tangazh_stabilization_algorithm_derivatives();
      for (i = 0; i < nx; i++) {
        Delta[i] = hN*f1[i];
        xtmp[i] = x1[i] - x0[i];
      }

      /* rhs = hN*f(tn,ytmp) - (ytmp-yn) */
      for (i = 0; i < nx; i++) {
        Delta[i] -= xtmp[i];
      }

      rt_ForwardSubstitutionRR_Dbl(W, Delta,
        f1, nx,
        1, pivots,
        1);
      rt_BackwardSubstitutionRR_Dbl(W+nx*nx-1, f1+nx-1,
        Delta, nx,
        1, 0);

      /* ytmp = ytmp + delta
         ztmp = (ytmp - yn)/h
       */
      for (i = 0; i < nx; i++) {
        x1[i] += Delta[i];
        ztmp[i] = (x1[i] - x0[i])/hN;
      }
    }

    for (k = 1; k < N[j]; k++) {
      t1 = t0 + k*hN;
      memcpy(x1start, x1,
                    (uint_T)nx*sizeof(real_T));
      rtsiSetT(si, t1);
      rtsiSetdX(si, f1);
      for (iter = 0; iter < numIter; iter++) {
        tangazh_stabilization_algorithm_step();
        tangazh_stabilization_algorithm_derivatives();
        if (iter == 0) {
          for (i = 0; i < nx; i++) {
            Delta[i] = hN*f1[i];
          }
        } else {
          for (i = 0; i < nx; i++) {
            Delta[i] = hN*f1[i];
            xtmp[i] = (x1[i]-x1start[i]);
          }

          /* rhs = hN*f(tn,ytmp) - M*(ytmp-yn) */
          for (i = 0; i < nx; i++) {
            Delta[i] -= xtmp[i];
          }
        }

        rt_ForwardSubstitutionRR_Dbl(W, Delta,
          f1, nx,
          1, pivots,
          1);
        rt_BackwardSubstitutionRR_Dbl(W+nx*nx-1, f1+nx-1,
          Delta, nx,
          1, 0);

        /* ytmp = ytmp + Delta
           ztmp = (ytmp - ytmp0)/h
         */
        for (i = 0; i < nx; i++) {
          x1[i] += Delta[i];
          ztmp[i] = (x1[i] - x1start[i])/hN;
        }
      }
    }
	
    memcpy(&(E[nx*j]), x1, (uint_T)nx*sizeof(real_T));
    for (k = j; k > 0; k--) {
      real_T coef = (real_T)(N[k-1]) / (N[j]-N[k-1]);
      for (i = 0; i < nx; i++) {
        x1[i] = E[nx*k+i] + coef*(E[nx*k+i] - E[nx*(k-1)+i]);
      }

      memcpy(&(E[nx*(k-1)]), x1,(uint_T)nx*sizeof(real_T));
    }
  }

  memcpy(x1, E,  (uint_T)nx*sizeof(real_T));

  /* t1 = t0 + h; */
  rtsiSetT(si,rtsiGetSolverStopTime(si));
  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}


void tangazh_stabilization_algorithm_NEGATIVEEdge(real_T rtu_0, boolean_T rtu_INprevious, boolean_T
  rtu_IN, RT_MODEL_tangazh_stabilization_algorithm * const tangazh_stabilization_algorithm_M, rtB_NEGATIVEEdge_tangazh_stabilization_algorithm
  *localB, rtDW_NEGATIVEEdge_tangazh_stabilization_algorithm *localDW)
{

  if (rtu_0 > 0.0) {

    localB->RelationalOperator1 = ((int32_T)rtu_INprevious > (int32_T)rtu_IN);
    if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
      srUpdateBC(localDW->NEGATIVEEdge_SubsysRanBC);
    }
  }


}

void tangazh_stabilization_algorithm_POSITIVEEdge(real_T rtu_0, boolean_T rtu_IN, boolean_T
  rtu_INprevious, RT_MODEL_tangazh_stabilization_algorithm * const tangazh_stabilization_algorithm_M,
  rtB_POSITIVEEdge_tangazh_stabilization_algorithm *localB, rtDW_POSITIVEEdge_tangazh_stabilization_algorithm *localDW)
{

  if (rtu_0 > 0.0) {
    
    localB->RelationalOperator1 = ((int32_T)rtu_IN > (int32_T)rtu_INprevious);
    if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
      srUpdateBC(localDW->POSITIVEEdge_SubsysRanBC);
    }
  }

}

void tangazh_stabilization_algorithm_OFFDelay_Init(rtDW_OFFDelay_tangazh_stabilization_algorithm *localDW,
  rtP_OFFDelay_tangazh_stabilization_algorithm *localP)
{

  localDW->Memory_PreviousInput = localP->Memory_X0;


  localDW->ICic_PreviousInput = localP->ICic_X0;
}


void tangazh_stabilization_algorithm_OFFDelay_Disable(rtDW_OFFDelay_tangazh_stabilization_algorithm *localDW)
{
  localDW->OFFDelay_MODE = FALSE;
}


void tangazh_stabilization_algorithm_OFFDelay_Start(rtDW_OFFDelay_tangazh_stabilization_algorithm *localDW,
  rtP_OFFDelay_tangazh_stabilization_algorithm *localP)
{
  tangazh_stabilization_algorithm_OFFDelay_Init(localDW, localP);

}

void tangazh_stabilization_algorithm_OFFDelay(boolean_T rtu_0, boolean_T rtu_IN, real_T rtu_clock,
  real_T rtu_DELAY, RT_MODEL_tangazh_stabilization_algorithm * const tangazh_stabilization_algorithm_M,
  rtB_OFFDelay_tangazh_stabilization_algorithm *localB, rtDW_OFFDelay_tangazh_stabilization_algorithm *localDW,
  rtP_OFFDelay_tangazh_stabilization_algorithm *localP)
{

  real_T rtb_MultiportSwitch[2];
  boolean_T rtb_Memory;

  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M) && rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    if (rtu_0) {
      if (!localDW->OFFDelay_MODE) {
        localDW->OFFDelay_MODE = TRUE;
      }
    } else {
      if (localDW->OFFDelay_MODE) {
        tangazh_stabilization_algorithm_OFFDelay_Disable(localDW);
      }
    }
  }

  if (localDW->OFFDelay_MODE) {
    if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {

      rtb_Memory = localDW->Memory_PreviousInput;

      switch ((int32_T)localP->Constant1_Value) {
       case 1:
        rtb_MultiportSwitch[0] = localP->posedge_Value[0];
        rtb_MultiportSwitch[1] = localP->posedge_Value[1];
        break;

       case 2:
        rtb_MultiportSwitch[0] = localP->negedge_Value[0];
        rtb_MultiportSwitch[1] = localP->negedge_Value[1];
        break;

       default:
        rtb_MultiportSwitch[0] = localP->eitheredge_Value[0];
        rtb_MultiportSwitch[1] = localP->eitheredge_Value[1];
        break;
      }

      tangazh_stabilization_algorithm_POSITIVEEdge(rtb_MultiportSwitch[0], rtu_IN, rtb_Memory,
        tangazh_stabilization_algorithm_M, &localB->POSITIVEEdge, &localDW->POSITIVEEdge);

      tangazh_stabilization_algorithm_NEGATIVEEdge(rtb_MultiportSwitch[1], rtb_Memory, rtu_IN,
        tangazh_stabilization_algorithm_M, &localB->NEGATIVEEdge, &localDW->NEGATIVEEdge);

      localB->LogicalOperator1 = (localB->POSITIVEEdge.RelationalOperator1 ||
        localB->NEGATIVEEdge.RelationalOperator1);

      localB->LogicalOperator = !rtu_IN;

      localB->ICic = localDW->ICic_PreviousInput;
    }

    if (localB->LogicalOperator1) {
      localB->Switch = rtu_clock;
    } else {
      localB->Switch = localB->ICic;
    }

    localB->Sum = localB->Switch + rtu_DELAY;
    localB->LogicalOperator2 = !(localB->LogicalOperator && (rtu_clock >=
      localB->Sum));
    if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
      srUpdateBC(localDW->OFFDelay_SubsysRanBC);
    }
  }
}

void tangazh_stabilization_algorithm_OFFDelay_Update(boolean_T rtu_IN, RT_MODEL_tangazh_stabilization_algorithm * const
  tangazh_stabilization_algorithm_M, rtB_OFFDelay_tangazh_stabilization_algorithm *localB, rtDW_OFFDelay_tangazh_stabilization_algorithm *localDW)
{

  if (localDW->OFFDelay_MODE && rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    localDW->Memory_PreviousInput = rtu_IN;
    localDW->ICic_PreviousInput = localB->Switch;
  }
}

void tangazh_stabilization_algorithm_ONDelay_Init(rtDW_ONDelay_tangazh_stabilization_algorithm *localDW,
  rtP_ONDelay_tangazh_stabilization_algorithm *localP)
{
  localDW->Memory_PreviousInput = localP->Memory_X0;
  localDW->ICic_PreviousInput = localP->ICic_X0;
}


void tangazh_stabilization_algorithm_ONDelay_Disable(rtDW_ONDelay_tangazh_stabilization_algorithm *localDW)
{
  localDW->ONDelay_MODE = FALSE;
}


void tangazh_stabilization_algorithm_ONDelay_Start(rtDW_ONDelay_tangazh_stabilization_algorithm *localDW,
  rtP_ONDelay_tangazh_stabilization_algorithm *localP)
{
  tangazh_stabilization_algorithm_ONDelay_Init(localDW, localP);
}

void tangazh_stabilization_algorithm_ONDelay(boolean_T rtu_0, boolean_T rtu_IN, real_T rtu_clock,
  real_T rtu_DELAY, RT_MODEL_tangazh_stabilization_algorithm * const tangazh_stabilization_algorithm_M,
  rtB_ONDelay_tangazh_stabilization_algorithm *localB, rtDW_ONDelay_tangazh_stabilization_algorithm *localDW,
  rtP_ONDelay_tangazh_stabilization_algorithm *localP)
{

  real_T rtb_MultiportSwitch_d[2];
  boolean_T rtb_Memory_e;

  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M) && rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    if (rtu_0) {
      if (!localDW->ONDelay_MODE) {
        localDW->ONDelay_MODE = TRUE;
      }
    } else {
      if (localDW->ONDelay_MODE) {
        tangazh_stabilization_algorithm_ONDelay_Disable(localDW);
      }
    }
  }

  if (localDW->ONDelay_MODE) {
    if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
      rtb_Memory_e = localDW->Memory_PreviousInput;
      switch ((int32_T)localP->Constant1_Value) {
       case 1:
        rtb_MultiportSwitch_d[0] = localP->posedge_Value[0];
        rtb_MultiportSwitch_d[1] = localP->posedge_Value[1];
        break;

       case 2:
        rtb_MultiportSwitch_d[0] = localP->negedge_Value[0];
        rtb_MultiportSwitch_d[1] = localP->negedge_Value[1];
        break;

       default:
        rtb_MultiportSwitch_d[0] = localP->eitheredge_Value[0];
        rtb_MultiportSwitch_d[1] = localP->eitheredge_Value[1];
        break;
      }

      tangazh_stabilization_algorithm_POSITIVEEdge(rtb_MultiportSwitch_d[0], rtu_IN, rtb_Memory_e,
        tangazh_stabilization_algorithm_M, &localB->POSITIVEEdge, &localDW->POSITIVEEdge);

      tangazh_stabilization_algorithm_NEGATIVEEdge(rtb_MultiportSwitch_d[1], rtb_Memory_e, rtu_IN,
        tangazh_stabilization_algorithm_M, &localB->NEGATIVEEdge, &localDW->NEGATIVEEdge);
      localB->LogicalOperator1 = (localB->POSITIVEEdge.RelationalOperator1 ||
        localB->NEGATIVEEdge.RelationalOperator1);
      localB->ICic = localDW->ICic_PreviousInput;
    }

    if (localB->LogicalOperator1) {
      localB->Switch = rtu_clock;
    } else {
      localB->Switch = localB->ICic;
    }

    localB->Sum = localB->Switch + rtu_DELAY;

    localB->LogicalOperator2 = ((rtu_clock >= localB->Sum) && rtu_IN);
    if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
      srUpdateBC(localDW->ONDelay_SubsysRanBC);
    }
  }

}

void tangazh_stabilization_algorithm_ONDelay_Update(boolean_T rtu_IN, RT_MODEL_tangazh_stabilization_algorithm * const
  tangazh_stabilization_algorithm_M, rtB_ONDelay_tangazh_stabilization_algorithm *localB, rtDW_ONDelay_tangazh_stabilization_algorithm *localDW)
{

  if (localDW->ONDelay_MODE && rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {

    localDW->Memory_PreviousInput = rtu_IN;
    localDW->ICic_PreviousInput = localB->Switch;
  }
}

void tangazh_stabilization_algorithm_step(void)
{

  real_T rtb_Clock;
  real_T rtb_Clock_d;
  real_T rtb_Clock_e;
  int32_T rowIdx;
  boolean_T rtb_LogicalOperator1;
  boolean_T rtb_LogicalOperator2;
  boolean_T rtb_LogicalOperator1_b;
  real_T u;
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {

    if (!(tangazh_stabilization_algorithm_M->Timing.clockTick0+1)) {
      rtsiSetSolverStopTime(&tangazh_stabilization_algorithm_M->solverInfo,
                            ((tangazh_stabilization_algorithm_M->Timing.clockTickH0 + 1) *
        tangazh_stabilization_algorithm_M->Timing.stepSize0 * 4294967296.0));
    } else {
      rtsiSetSolverStopTime(&tangazh_stabilization_algorithm_M->solverInfo,
                            ((tangazh_stabilization_algorithm_M->Timing.clockTick0 + 1) *
        tangazh_stabilization_algorithm_M->Timing.stepSize0 + tangazh_stabilization_algorithm_M->Timing.clockTickH0 *
        tangazh_stabilization_algorithm_M->Timing.stepSize0 * 4294967296.0));
    }
  }                                    
  if (rtmIsMinorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_M->Timing.t[0] = rtsiGetT(&tangazh_stabilization_algorithm_M->solverInfo);
  }

  srClearBC(tangazh_stabilization_algorithm_DWork.OFFDelay_a.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC);
  srClearBC(tangazh_stabilization_algorithm_DWork.OFFDelay_a.POSITIVEEdge.POSITIVEEdge_SubsysRanBC);
  srClearBC(tangazh_stabilization_algorithm_DWork.OFFDelay_a.OFFDelay_SubsysRanBC);
  srClearBC(tangazh_stabilization_algorithm_DWork.ONDelay_i.ONDelay_SubsysRanBC);
  rtb_Clock = tangazh_stabilization_algorithm_M->Timing.t[0];
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_B.RelationalOperator1 = (tangazh_stabilization_algorithm_P.Constant2_Value ==
      tangazh_stabilization_algorithm_P.Constant_Value);
  }
  tangazh_stabilization_algorithm_ONDelay(tangazh_stabilization_algorithm_B.RelationalOperator1, tangazh_stabilization_algorithm_P.Constant4_Value,
                    rtb_Clock, tangazh_stabilization_algorithm_P.Constant1_Value, tangazh_stabilization_algorithm_M,
                    &tangazh_stabilization_algorithm_B.ONDelay, &tangazh_stabilization_algorithm_DWork.ONDelay,
                    (rtP_ONDelay_tangazh_stabilization_algorithm *)&tangazh_stabilization_algorithm_P.ONDelay);
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_B.LogicalOperator2 = !tangazh_stabilization_algorithm_B.RelationalOperator1;
  }
  tangazh_stabilization_algorithm_OFFDelay(tangazh_stabilization_algorithm_B.LogicalOperator2, tangazh_stabilization_algorithm_P.Constant4_Value,
                     rtb_Clock, tangazh_stabilization_algorithm_P.Constant1_Value, tangazh_stabilization_algorithm_M,
                     &tangazh_stabilization_algorithm_B.OFFDelay, &tangazh_stabilization_algorithm_DWork.OFFDelay,
                     (rtP_OFFDelay_tangazh_stabilization_algorithm *)&tangazh_stabilization_algorithm_P.OFFDelay);
  rtb_LogicalOperator1 = (tangazh_stabilization_algorithm_B.ONDelay.LogicalOperator2 ||
    tangazh_stabilization_algorithm_B.OFFDelay.LogicalOperator2);
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_B.LogicalOperator = (tangazh_stabilization_algorithm_P.Constant_Value_c &&
      tangazh_stabilization_algorithm_P.Constant2_Value_f1);
  }

  rtb_Clock_d = tangazh_stabilization_algorithm_M->Timing.t[0];
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {

    tangazh_stabilization_algorithm_B.RelationalOperator1_h = (tangazh_stabilization_algorithm_P.Constant2_Value_j ==
      tangazh_stabilization_algorithm_P.Constant_Value_p);
  }
  tangazh_stabilization_algorithm_ONDelay(tangazh_stabilization_algorithm_B.RelationalOperator1_h,
                    tangazh_stabilization_algorithm_B.LogicalOperator, rtb_Clock_d,
                    tangazh_stabilization_algorithm_P.Constant1_Value_m, tangazh_stabilization_algorithm_M,
                    &tangazh_stabilization_algorithm_B.ONDelay_p, &tangazh_stabilization_algorithm_DWork.ONDelay_p,
                    (rtP_ONDelay_tangazh_stabilization_algorithm *)&tangazh_stabilization_algorithm_P.ONDelay_p);

  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_B.LogicalOperator2_a = !tangazh_stabilization_algorithm_B.RelationalOperator1_h;
  }
  tangazh_stabilization_algorithm_OFFDelay(tangazh_stabilization_algorithm_B.LogicalOperator2_a, tangazh_stabilization_algorithm_B.LogicalOperator,
                     rtb_Clock_d, tangazh_stabilization_algorithm_P.Constant1_Value_m, tangazh_stabilization_algorithm_M,
                     &tangazh_stabilization_algorithm_B.OFFDelay_p, &tangazh_stabilization_algorithm_DWork.OFFDelay_p,
                     (rtP_OFFDelay_tangazh_stabilization_algorithm *)&tangazh_stabilization_algorithm_P.OFFDelay_p);
  rtb_LogicalOperator2 = !(tangazh_stabilization_algorithm_B.ONDelay_p.LogicalOperator2 ||
    tangazh_stabilization_algorithm_B.OFFDelay_p.LogicalOperator2);
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_B.Compare = (fabs(tangazh_stabilization_algorithm_P.Constant7_Value) >
      tangazh_stabilization_algorithm_P.Constant_Value_f);
  }
  rtb_Clock_e = tangazh_stabilization_algorithm_M->Timing.t[0];
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_B.RelationalOperator1_e = (tangazh_stabilization_algorithm_P.Constant2_Value_f ==
      tangazh_stabilization_algorithm_P.Constant_Value_g);
  }
  tangazh_stabilization_algorithm_ONDelay(tangazh_stabilization_algorithm_B.RelationalOperator1_e, tangazh_stabilization_algorithm_B.Compare,
                    rtb_Clock_e, tangazh_stabilization_algorithm_P.Constant1_Value_h, tangazh_stabilization_algorithm_M,
                    &tangazh_stabilization_algorithm_B.ONDelay_i, &tangazh_stabilization_algorithm_DWork.ONDelay_i,
                    (rtP_ONDelay_tangazh_stabilization_algorithm *)&tangazh_stabilization_algorithm_P.ONDelay_i);

  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_B.LogicalOperator2_m = !tangazh_stabilization_algorithm_B.RelationalOperator1_e;
  }

  tangazh_stabilization_algorithm_OFFDelay(tangazh_stabilization_algorithm_B.LogicalOperator2_m, tangazh_stabilization_algorithm_B.Compare,
                     rtb_Clock_e, tangazh_stabilization_algorithm_P.Constant1_Value_h, tangazh_stabilization_algorithm_M,
                     &tangazh_stabilization_algorithm_B.OFFDelay_a, &tangazh_stabilization_algorithm_DWork.OFFDelay_a,
                     (rtP_OFFDelay_tangazh_stabilization_algorithm *)&tangazh_stabilization_algorithm_P.OFFDelay_a);
  rtb_LogicalOperator1_b = (tangazh_stabilization_algorithm_B.ONDelay_i.LogicalOperator2 ||
    tangazh_stabilization_algorithm_B.OFFDelay_a.LogicalOperator2);
  rtb_LogicalOperator1 = (rtb_LogicalOperator1 && rtb_LogicalOperator2 &&
    tangazh_stabilization_algorithm_P.Constant5_Value && rtb_LogicalOperator1_b);
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_B.Memory = tangazh_stabilization_algorithm_DWork.Memory_PreviousInput;
  }
  rowIdx = (int32_T)(((uint32_T)(int32_T)(((uint32_T)(int32_T)
    rtb_LogicalOperator1 << 1) + (uint32_T)tangazh_stabilization_algorithm_P.Constant6_Value) << 1) +
                     (uint32_T)tangazh_stabilization_algorithm_B.Memory);
  tangazh_stabilization_algorithm_B.Logic[0U] = tangazh_stabilization_algorithm_P.Logic_table[(uint32_T)rowIdx];
  tangazh_stabilization_algorithm_B.Logic[1U] = tangazh_stabilization_algorithm_P.Logic_table[(uint32_T)rowIdx + 8U];
  tangazh_stabilization_algorithm_B.LogicalOperator_f = !tangazh_stabilization_algorithm_B.Logic[0];
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    ZCEventType zcEvent;
    zcEvent = ((ZCEventType) ((tangazh_stabilization_algorithm_B.LogicalOperator_f !=
      (tangazh_stabilization_algorithm_PrevZCSigState.Integrator_Reset_ZCE == POS_ZCSIG)) &&
                (tangazh_stabilization_algorithm_PrevZCSigState.Integrator_Reset_ZCE !=
                 UNINITIALIZED_ZCSIG)));
    tangazh_stabilization_algorithm_PrevZCSigState.Integrator_Reset_ZCE = (ZCSigState)
      tangazh_stabilization_algorithm_B.LogicalOperator_f;
    if (zcEvent || tangazh_stabilization_algorithm_B.LogicalOperator_f) {
      tangazh_stabilization_algorithm_X.Integrator_CSTATE = tangazh_stabilization_algorithm_P.Integrator_IC;
    }
  }

  tangazh_stabilization_algorithm_B.Integrator = tangazh_stabilization_algorithm_X.Integrator_CSTATE;
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_B.Divide1 = (tangazh_stabilization_algorithm_P.Constant8_Value -
      tangazh_stabilization_algorithm_P.Constant9_Value) * tangazh_stabilization_algorithm_P.Constant2_Value_c;
  }
  u = tangazh_stabilization_algorithm_B.Integrator * tangazh_stabilization_algorithm_P.Constant3_Value + tangazh_stabilization_algorithm_B.Divide1;
  if (u >= tangazh_stabilization_algorithm_P.Saturation_UpperSat) {
    tangazh_stabilization_algorithm_B.Saturation = tangazh_stabilization_algorithm_P.Saturation_UpperSat;
  } else if (u <= tangazh_stabilization_algorithm_P.Saturation_LowerSat) {
    tangazh_stabilization_algorithm_B.Saturation = tangazh_stabilization_algorithm_P.Saturation_LowerSat;
  } else {
    tangazh_stabilization_algorithm_B.Saturation = u;
  }
  tangazh_stabilization_algorithm_B.SineWave = sin(tangazh_stabilization_algorithm_P.SineWave_Freq * tangazh_stabilization_algorithm_M->Timing.t[0]
    + tangazh_stabilization_algorithm_P.SineWave_Phase) * tangazh_stabilization_algorithm_P.SineWave_Amp +
    tangazh_stabilization_algorithm_P.SineWave_Bias;
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
  }
  if (rtb_LogicalOperator1_b) {
    if (tangazh_stabilization_algorithm_B.Logic[0]) {
      tangazh_stabilization_algorithm_B.Switch = tangazh_stabilization_algorithm_B.SineWave;
    } else {
      tangazh_stabilization_algorithm_B.Switch = tangazh_stabilization_algorithm_P.Constant_Value_b;
    }
    tangazh_stabilization_algorithm_B.Switch1 = tangazh_stabilization_algorithm_B.Switch;
  } else {
    tangazh_stabilization_algorithm_B.Switch1 = tangazh_stabilization_algorithm_P.Constant1_Value_p;
  }

  tangazh_stabilization_algorithm_B.Add2 = tangazh_stabilization_algorithm_B.Switch1 - tangazh_stabilization_algorithm_B.Integrator;
  tangazh_stabilization_algorithm_B.Gain = tangazh_stabilization_algorithm_P.Gain_Gain * tangazh_stabilization_algorithm_B.Add2;
  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    tangazh_stabilization_algorithm_ONDelay_Update(tangazh_stabilization_algorithm_P.Constant4_Value, tangazh_stabilization_algorithm_M,
      &tangazh_stabilization_algorithm_B.ONDelay, &tangazh_stabilization_algorithm_DWork.ONDelay);
    tangazh_stabilization_algorithm_OFFDelay_Update(tangazh_stabilization_algorithm_P.Constant4_Value, tangazh_stabilization_algorithm_M,
      &tangazh_stabilization_algorithm_B.OFFDelay, &tangazh_stabilization_algorithm_DWork.OFFDelay);
    tangazh_stabilization_algorithm_ONDelay_Update(tangazh_stabilization_algorithm_B.LogicalOperator, tangazh_stabilization_algorithm_M,
      &tangazh_stabilization_algorithm_B.ONDelay_p, &tangazh_stabilization_algorithm_DWork.ONDelay_p);
    tangazh_stabilization_algorithm_OFFDelay_Update(tangazh_stabilization_algorithm_B.LogicalOperator, tangazh_stabilization_algorithm_M,
      &tangazh_stabilization_algorithm_B.OFFDelay_p, &tangazh_stabilization_algorithm_DWork.OFFDelay_p);
    tangazh_stabilization_algorithm_ONDelay_Update(tangazh_stabilization_algorithm_B.Compare, tangazh_stabilization_algorithm_M,
      &tangazh_stabilization_algorithm_B.ONDelay_i, &tangazh_stabilization_algorithm_DWork.ONDelay_i);
    tangazh_stabilization_algorithm_OFFDelay_Update(tangazh_stabilization_algorithm_B.Compare, tangazh_stabilization_algorithm_M,
      &tangazh_stabilization_algorithm_B.OFFDelay_a, &tangazh_stabilization_algorithm_DWork.OFFDelay_a);
    if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
      tangazh_stabilization_algorithm_DWork.Memory_PreviousInput = tangazh_stabilization_algorithm_B.Logic[0];
    }
    rtExtModeUploadCheckTrigger(2);     
    rtExtModeUpload(0, tangazh_stabilization_algorithm_M->Timing.t[0]);

    if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
      rtExtModeUpload(1, (((tangazh_stabilization_algorithm_M->Timing.clockTick1+
                            tangazh_stabilization_algorithm_M->Timing.clockTickH1* 4294967296.0)) *
                          0.2));
    }
  }                                    

  if (rtmIsMajorTimeStep(tangazh_stabilization_algorithm_M)) {
    {                                  
      if ((rtmGetTFinal(tangazh_stabilization_algorithm_M)!=-1) &&
          !((rtmGetTFinal(tangazh_stabilization_algorithm_M)-(((tangazh_stabilization_algorithm_M->Timing.clockTick1+
               tangazh_stabilization_algorithm_M->Timing.clockTickH1* 4294967296.0)) * 0.2)) >
            (((tangazh_stabilization_algorithm_M->Timing.clockTick1+tangazh_stabilization_algorithm_M->Timing.clockTickH1*
               4294967296.0)) * 0.2) * (DBL_EPSILON))) {
        rtmSetErrorStatus(tangazh_stabilization_algorithm_M, "Simulation finished");
      }

      if (rtmGetStopRequested(tangazh_stabilization_algorithm_M)) {
        rtmSetErrorStatus(tangazh_stabilization_algorithm_M, "Simulation finished");
      }
    }
    rt_ertODEUpdateContinuousStates(&tangazh_stabilization_algorithm_M->solverInfo);
    
    if (!(++tangazh_stabilization_algorithm_M->Timing.clockTick0)) {
      ++tangazh_stabilization_algorithm_M->Timing.clockTickH0;
    }

    tangazh_stabilization_algorithm_M->Timing.t[0] = rtsiGetSolverStopTime(&tangazh_stabilization_algorithm_M->solverInfo);
	tangazh_stabilization_algorithm_M->Timing.clockTick1++;
      if (!tangazh_stabilization_algorithm_M->Timing.clockTick1) {
        tangazh_stabilization_algorithm_M->Timing.clockTickH1++;
      }
    }
  }                                  



void tangazh_stabilization_algorithm_derivatives(void)
{
  {
    if (!tangazh_stabilization_algorithm_B.LogicalOperator_f) {
      ((StateDerivatives_tangazh_stabilization_algorithm *) tangazh_stabilization_algorithm_M->ModelData.derivs)
        ->Integrator_CSTATE = tangazh_stabilization_algorithm_B.Gain;
    } else {
      ((StateDerivatives_tangazh_stabilization_algorithm *) tangazh_stabilization_algorithm_M->ModelData.derivs)
        ->Integrator_CSTATE = 0.0;
    }
  }
}

void tangazh_stabilization_algorithm_initialize(void)
{
  {
    rtsiSetSimTimeStepPtr(&tangazh_stabilization_algorithm_M->solverInfo,
                          &tangazh_stabilization_algorithm_M->Timing.simTimeStep);
    rtsiSetTPtr(&tangazh_stabilization_algorithm_M->solverInfo, &rtmGetTPtr(tangazh_stabilization_algorithm_M));
    rtsiSetStepSizePtr(&tangazh_stabilization_algorithm_M->solverInfo, &tangazh_stabilization_algorithm_M->Timing.stepSize0);
    rtsiSetdXPtr(&tangazh_stabilization_algorithm_M->solverInfo, &tangazh_stabilization_algorithm_M->ModelData.derivs);
    rtsiSetContStatesPtr(&tangazh_stabilization_algorithm_M->solverInfo,
                         &tangazh_stabilization_algorithm_M->ModelData.contStates);
    rtsiSetNumContStatesPtr(&tangazh_stabilization_algorithm_M->solverInfo,
      &tangazh_stabilization_algorithm_M->Sizes.numContStates);
    rtsiSetErrorStatusPtr(&tangazh_stabilization_algorithm_M->solverInfo, (&rtmGetErrorStatus
      (tangazh_stabilization_algorithm_M)));
    rtsiSetRTModelPtr(&tangazh_stabilization_algorithm_M->solverInfo, tangazh_stabilization_algorithm_M);
  }

  rtsiSetSimTimeStep(&tangazh_stabilization_algorithm_M->solverInfo, MAJOR_TIME_STEP);
  tangazh_stabilization_algorithm_M->ModelData.intgData.x0 = tangazh_stabilization_algorithm_M->ModelData.odeX0;
  tangazh_stabilization_algorithm_M->ModelData.intgData.f0 = tangazh_stabilization_algorithm_M->ModelData.odeF0;
  tangazh_stabilization_algorithm_M->ModelData.intgData.x1start = tangazh_stabilization_algorithm_M->ModelData.odeX1START;
  tangazh_stabilization_algorithm_M->ModelData.intgData.f1 = tangazh_stabilization_algorithm_M->ModelData.odeF1;
  tangazh_stabilization_algorithm_M->ModelData.intgData.Delta = tangazh_stabilization_algorithm_M->ModelData.odeDELTA;
  tangazh_stabilization_algorithm_M->ModelData.intgData.E = tangazh_stabilization_algorithm_M->ModelData.odeE;
  tangazh_stabilization_algorithm_M->ModelData.intgData.fac = tangazh_stabilization_algorithm_M->ModelData.odeFAC;


  int_T i;
  real_T *f = tangazh_stabilization_algorithm_M->ModelData.intgData.fac;
  for (i = 0; i < (int_T)(sizeof(tangazh_stabilization_algorithm_M->ModelData.odeFAC)/sizeof(real_T));i++) 
      f[i] = 1.5e-8;



  tangazh_stabilization_algorithm_M->ModelData.intgData.DFDX = tangazh_stabilization_algorithm_M->ModelData.odeDFDX;
  tangazh_stabilization_algorithm_M->ModelData.intgData.W = tangazh_stabilization_algorithm_M->ModelData.odeW;
  tangazh_stabilization_algorithm_M->ModelData.intgData.pivots = tangazh_stabilization_algorithm_M->ModelData.odePIVOTS;
  tangazh_stabilization_algorithm_M->ModelData.intgData.xtmp = tangazh_stabilization_algorithm_M->ModelData.odeXTMP;
  tangazh_stabilization_algorithm_M->ModelData.intgData.ztmp = tangazh_stabilization_algorithm_M->ModelData.odeZTMP;
  tangazh_stabilization_algorithm_M->ModelData.intgData.isFirstStep = TRUE;
  rtsiSetSolverExtrapolationOrder(&tangazh_stabilization_algorithm_M->solverInfo, 2);
  rtsiSetSolverNumberNewtonIterations(&tangazh_stabilization_algorithm_M->solverInfo, 1);
  tangazh_stabilization_algorithm_M->ModelData.contStates = ((real_T *) &tangazh_stabilization_algorithm_X);
  rtsiSetSolverData(&tangazh_stabilization_algorithm_M->solverInfo, (void *)
                    &tangazh_stabilization_algorithm_M->ModelData.intgData);
  rtsiSetSolverName(&tangazh_stabilization_algorithm_M->solverInfo,"ode14x");
  rtmSetTPtr(tangazh_stabilization_algorithm_M, &tangazh_stabilization_algorithm_M->Timing.tArray[0]);
  rtmSetTFinal(tangazh_stabilization_algorithm_M, -1);
  tangazh_stabilization_algorithm_M->Timing.stepSize0 = 0.2;

  tangazh_stabilization_algorithm_M->Sizes.checksums[0] = (1741543849U);
  tangazh_stabilization_algorithm_M->Sizes.checksums[1] = (3280251608U);
  tangazh_stabilization_algorithm_M->Sizes.checksums[2] = (679210516U);
  tangazh_stabilization_algorithm_M->Sizes.checksums[3] = (3610629248U);


  static const sysRanDType rtAlwaysEnabled = SUBSYS_RAN_BC_ENABLE;
  static RTWExtModeInfo rt_ExtModeInfo;
  static const sysRanDType *systemRan[38];
  tangazh_stabilization_algorithm_M->extModeInfo = (&rt_ExtModeInfo);
  rteiSetSubSystemActiveVectorAddresses(&rt_ExtModeInfo, systemRan);
  systemRan[0] = &rtAlwaysEnabled;
  systemRan[1] = &rtAlwaysEnabled;
  systemRan[2] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_a.OFFDelay_SubsysRanBC;
  systemRan[3] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_a.OFFDelay_SubsysRanBC;
  systemRan[4] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_a.OFFDelay_SubsysRanBC;
  systemRan[5] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_a.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC;
  systemRan[6] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_a.POSITIVEEdge.POSITIVEEdge_SubsysRanBC;
  systemRan[7] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_a.OFFDelay_SubsysRanBC;
  systemRan[8] = (sysRanDType *)&tangazh_stabilization_algorithm_DWork.ONDelay_i.ONDelay_SubsysRanBC;
  systemRan[9] = (sysRanDType *)&tangazh_stabilization_algorithm_DWork.ONDelay_i.ONDelay_SubsysRanBC;
  systemRan[10] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_i.ONDelay_SubsysRanBC;
  systemRan[11] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_i.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC;
  systemRan[12] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_i.POSITIVEEdge.POSITIVEEdge_SubsysRanBC;
  systemRan[13] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_i.ONDelay_SubsysRanBC;
  systemRan[14] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_p.OFFDelay_SubsysRanBC;
  systemRan[15] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_p.OFFDelay_SubsysRanBC;
  systemRan[16] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_p.OFFDelay_SubsysRanBC;
  systemRan[17] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_p.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC;
  systemRan[18] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_p.POSITIVEEdge.POSITIVEEdge_SubsysRanBC;
  systemRan[19] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay_p.OFFDelay_SubsysRanBC;
  systemRan[20] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_p.ONDelay_SubsysRanBC;
  systemRan[21] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_p.ONDelay_SubsysRanBC;
  systemRan[22] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_p.ONDelay_SubsysRanBC;
  systemRan[23] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_p.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC;
  systemRan[24] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_p.POSITIVEEdge.POSITIVEEdge_SubsysRanBC;
  systemRan[25] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay_p.ONDelay_SubsysRanBC;
  systemRan[26] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay.OFFDelay_SubsysRanBC;
  systemRan[27] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay.OFFDelay_SubsysRanBC;
  systemRan[28] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay.OFFDelay_SubsysRanBC;
  systemRan[29] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC;
  systemRan[30] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay.POSITIVEEdge.POSITIVEEdge_SubsysRanBC;
  systemRan[31] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.OFFDelay.OFFDelay_SubsysRanBC;
  systemRan[32] = (sysRanDType *)&tangazh_stabilization_algorithm_DWork.ONDelay.ONDelay_SubsysRanBC;
  systemRan[33] = (sysRanDType *)&tangazh_stabilization_algorithm_DWork.ONDelay.ONDelay_SubsysRanBC;
  systemRan[34] = (sysRanDType *)&tangazh_stabilization_algorithm_DWork.ONDelay.ONDelay_SubsysRanBC;
  systemRan[35] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC;
  systemRan[36] = (sysRanDType *)
    &tangazh_stabilization_algorithm_DWork.ONDelay.POSITIVEEdge.POSITIVEEdge_SubsysRanBC;
  systemRan[37] = (sysRanDType *)&tangazh_stabilization_algorithm_DWork.ONDelay.ONDelay_SubsysRanBC;
  rteiSetModelMappingInfoPtr(tangazh_stabilization_algorithm_M->extModeInfo,
    &tangazh_stabilization_algorithm_M->SpecialInfo.mappingInfo);
  rteiSetChecksumsPtr(tangazh_stabilization_algorithm_M->extModeInfo, tangazh_stabilization_algorithm_M->Sizes.checksums);
  rteiSetTPtr(tangazh_stabilization_algorithm_M->extModeInfo, rtmGetTPtr(tangazh_stabilization_algorithm_M));


  static DataTypeTransInfo dtInfo;
  memset((char_T *) &dtInfo, 0, sizeof(dtInfo));
  tangazh_stabilization_algorithm_M->SpecialInfo.mappingInfo = (&dtInfo);
  dtInfo.numDataTypes = 14;
  dtInfo.dataTypeSizes = &rtDataTypeSizes[0];
  dtInfo.dataTypeNames = &rtDataTypeNames[0];

  dtInfo.B = &rtBTransTable;
  dtInfo.P = &rtPTransTable;

  tangazh_stabilization_algorithm_ONDelay_Start(&tangazh_stabilization_algorithm_DWork.ONDelay, (rtP_ONDelay_tangazh_stabilization_algorithm *)
    &tangazh_stabilization_algorithm_P.ONDelay);

  tangazh_stabilization_algorithm_OFFDelay_Start(&tangazh_stabilization_algorithm_DWork.OFFDelay, (rtP_OFFDelay_tangazh_stabilization_algorithm *)
    &tangazh_stabilization_algorithm_P.OFFDelay);

  tangazh_stabilization_algorithm_ONDelay_Start(&tangazh_stabilization_algorithm_DWork.ONDelay_p, (rtP_ONDelay_tangazh_stabilization_algorithm *)
    &tangazh_stabilization_algorithm_P.ONDelay_p);

  tangazh_stabilization_algorithm_OFFDelay_Start(&tangazh_stabilization_algorithm_DWork.OFFDelay_p, (rtP_OFFDelay_tangazh_stabilization_algorithm *)
    &tangazh_stabilization_algorithm_P.OFFDelay_p);

  tangazh_stabilization_algorithm_ONDelay_Start(&tangazh_stabilization_algorithm_DWork.ONDelay_i, (rtP_ONDelay_tangazh_stabilization_algorithm *)
    &tangazh_stabilization_algorithm_P.ONDelay_i);

  tangazh_stabilization_algorithm_OFFDelay_Start(&tangazh_stabilization_algorithm_DWork.OFFDelay_a, (rtP_OFFDelay_tangazh_stabilization_algorithm *)
    &tangazh_stabilization_algorithm_P.OFFDelay_a);

  tangazh_stabilization_algorithm_PrevZCSigState.Integrator_Reset_ZCE = UNINITIALIZED_ZCSIG;

  tangazh_stabilization_algorithm_DWork.Memory_PreviousInput = tangazh_stabilization_algorithm_P.Memory_X0;


  tangazh_stabilization_algorithm_X.Integrator_CSTATE = tangazh_stabilization_algorithm_P.Integrator_IC;
}