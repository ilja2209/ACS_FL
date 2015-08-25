
#include "ext_types.h"

static uint_T rtDataTypeSizes[] = {
  sizeof(real_T),
  sizeof(real32_T),
  sizeof(int8_T),
  sizeof(uint8_T),
  sizeof(int16_T),
  sizeof(uint16_T),
  sizeof(int32_T),
  sizeof(uint32_T),
  sizeof(boolean_T),
  sizeof(fcn_call_T),
  sizeof(int_T),
  sizeof(pointer_T),
  sizeof(action_T),
  2*sizeof(uint32_T)
};

static const char_T * rtDataTypeNames[] = {
  "real_T",
  "real32_T",
  "int8_T",
  "uint8_T",
  "int16_T",
  "uint16_T",
  "int32_T",
  "uint32_T",
  "boolean_T",
  "fcn_call_T",
  "int_T",
  "pointer_T",
  "action_T",
  "timer_uint32_pair_T"
};

static DataTypeTransition rtBTransitions[] = {
  { (char_T *)(&tangazh_stabilization_algorithm_B.Integrator), 0, 0, 8 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.RelationalOperator1), 8, 0, 12 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay.ICic), 0, 0, 3 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay.LogicalOperator1), 8, 0, 2 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay.POSITIVEEdge.RelationalOperator1), 8, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay.NEGATIVEEdge.RelationalOperator1), 8, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay.ICic), 0, 0, 3 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay.LogicalOperator1), 8, 0, 3 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay.POSITIVEEdge.RelationalOperator1), 8, 0, 1
  },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay.NEGATIVEEdge.RelationalOperator1), 8, 0, 1
  },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay_p.ICic), 0, 0, 3 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay_p.LogicalOperator1), 8, 0, 2 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay_p.POSITIVEEdge.RelationalOperator1), 8, 0, 1
  },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay_p.NEGATIVEEdge.RelationalOperator1), 8, 0, 1
  },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay_p.ICic), 0, 0, 3 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay_p.LogicalOperator1), 8, 0, 3 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay_p.POSITIVEEdge.RelationalOperator1), 8, 0,
    1 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay_p.NEGATIVEEdge.RelationalOperator1), 8, 0,
    1 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay_i.ICic), 0, 0, 3 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay_i.LogicalOperator1), 8, 0, 2 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay_i.POSITIVEEdge.RelationalOperator1), 8, 0, 1
  },

  { (char_T *)(&tangazh_stabilization_algorithm_B.ONDelay_i.NEGATIVEEdge.RelationalOperator1), 8, 0, 1
  },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay_a.ICic), 0, 0, 3 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay_a.LogicalOperator1), 8, 0, 3 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay_a.POSITIVEEdge.RelationalOperator1), 8, 0,
    1 },

  { (char_T *)(&tangazh_stabilization_algorithm_B.OFFDelay_a.NEGATIVEEdge.RelationalOperator1), 8, 0,
    1 }
  ,

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.Scope_PWORK.LoggedData), 11, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.Memory_PreviousInput), 8, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay.ICic_PreviousInput), 0, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay.ONDelay_SubsysRanBC), 2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay.Memory_PreviousInput), 8, 0, 2 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay.POSITIVEEdge.POSITIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay.ICic_PreviousInput), 0, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay.OFFDelay_SubsysRanBC), 2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay.Memory_PreviousInput), 8, 0, 2 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay.POSITIVEEdge.POSITIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_p.ICic_PreviousInput), 0, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_p.ONDelay_SubsysRanBC), 2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_p.Memory_PreviousInput), 8, 0, 2 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_p.POSITIVEEdge.POSITIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_p.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_p.ICic_PreviousInput), 0, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_p.OFFDelay_SubsysRanBC), 2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_p.Memory_PreviousInput), 8, 0, 2 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_p.POSITIVEEdge.POSITIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_p.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_i.ICic_PreviousInput), 0, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_i.ONDelay_SubsysRanBC), 2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_i.Memory_PreviousInput), 8, 0, 2 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_i.POSITIVEEdge.POSITIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.ONDelay_i.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_a.ICic_PreviousInput), 0, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_a.OFFDelay_SubsysRanBC), 2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_a.Memory_PreviousInput), 8, 0, 2 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_a.POSITIVEEdge.POSITIVEEdge_SubsysRanBC),
    2, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_DWork.OFFDelay_a.NEGATIVEEdge.NEGATIVEEdge_SubsysRanBC),
    2, 0, 1 }
};

static DataTypeTransitionTable rtBTransTable = {
  58U,
  rtBTransitions
};

static DataTypeTransition rtPTransitions[] = {
  { (char_T *)(&tangazh_stabilization_algorithm_P.Constant1_Value), 0, 0, 25 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.Constant4_Value), 8, 0, 22 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.ONDelay.eitheredge_Value[0]), 0, 0, 8 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.ONDelay.Memory_X0), 8, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.OFFDelay.eitheredge_Value[0]), 0, 0, 8 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.OFFDelay.Memory_X0), 8, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.ONDelay_p.eitheredge_Value[0]), 0, 0, 8 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.ONDelay_p.Memory_X0), 8, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.OFFDelay_p.eitheredge_Value[0]), 0, 0, 8 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.OFFDelay_p.Memory_X0), 8, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.ONDelay_i.eitheredge_Value[0]), 0, 0, 8 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.ONDelay_i.Memory_X0), 8, 0, 1 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.OFFDelay_a.eitheredge_Value[0]), 0, 0, 8 },

  { (char_T *)(&tangazh_stabilization_algorithm_P.OFFDelay_a.Memory_X0), 8, 0, 1 }
};

static DataTypeTransitionTable rtPTransTable = {
  14U,
  rtPTransitions
};

