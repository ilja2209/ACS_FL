
MAKECMD         = "C:/qnx660/host/win32/x86/usr/bin/gmake"
HOST            = ANY
BUILD           = yes
SYS_TARGET_FILE = any
COMPILER_TOOL_CHAIN = default

MAKEFILE_FILESEP = /


MODEL                = tangazh_stabilization_algorithm
MODULES              = tangazh_stabilization_algorithm_data.c rt_backsubrr_dbl.c rt_forwardsubrr_dbl.c rt_lu_real.c rt_matrixlib_dbl.c 
MATLAB_ROOT          = C:/MATLAB/MATLABProductionServer/R2012b
ALT_MATLAB_ROOT      = C:/MATLAB/MATLAB~1/R2012b
MATLAB_BIN           = C:/MATLAB/MATLABProductionServer/R2012b/bin
ALT_MATLAB_BIN       = C:/MATLAB/MATLAB~1/R2012b/bin
MASTER_ANCHOR_DIR    = 
START_DIR            = C:/MATLAB
S_FUNCTIONS          = 
S_FUNCTIONS_LIB      = 
NUMST                = 2
NCSTATES             = 1
BUILDARGS            =  GENERATE_REPORT=0PORTABLE_WORDSIZES=0 GENERATE_ASAP2=0 OPTS="-DTID01EQ=1"
MULTITASKING         = 0
INTEGER_CODE         = 0
MAT_FILE             = 0
ONESTEPFCN           = 1
TERMFCN              = 0
B_ERTSFCN            = 0
MEXEXT               = mexw64
EXT_MODE             = 1
TMW_EXTMODE_TESTING  = 0
EXTMODE_TRANSPORT    = 0
EXTMODE_STATIC       = 0
EXTMODE_STATIC_SIZE  = 1000000
MULTI_INSTANCE_CODE  = 0
MODELREFS            = 
SHARED_SRC           = 
SHARED_SRC_DIR       = 
SHARED_BIN_DIR       = 
SHARED_LIB           = 
PORTABLE_WORDSIZES   = 0
SHRLIBTARGET         = 0
MAKEFILEBUILDER_TGT  = 0
STANDALONE_SUPPRESS_EXE = 0
OPTIMIZATION_FLAGS      = 
ADDITIONAL_LDFLAGS      = 
TARGET_LANG_EXT      = c

#--------------------------- Model and reference models -----------------------
MODELREFS                 = 
MODELLIB                  = tangazh_stabilization_algorithmlib.a
MODELREF_LINK_LIBS        = 
MODELREF_LINK_RSPFILE     = tangazh_stabilization_algorithm_ref.rsp
MODELREF_INC_PATH         = 
RELATIVE_PATH_TO_ANCHOR   = ..

MODELREF_TARGET_TYPE       = NONE
MODELREF_SFCN_SUFFIX       = _msf

#-------------------------------------------------------------------------------------
QNX_MW_ROOT                =  C:/MATLAB/Work/qnx

QNX_DOWNLOAD               = 1

#-- In the case when directory name contains space ---
ifneq ($(MATLAB_ROOT),$(ALT_MATLAB_ROOT))
MATLAB_ROOT := $(ALT_MATLAB_ROOT)
endif
ifneq ($(MATLAB_BIN),$(ALT_MATLAB_BIN))
MATLAB_BIN := $(ALT_MATLAB_BIN)
endif

SYSLIB = -llibc.a -llibm.a




#----------------------------- External mode -----------------------------------
# Uncomment -DVERBOSE to have information printed to stdout
# To add a new transport layer, see the comments in
#   <matlabroot>/toolbox/simulink/simulink/extmode_transports.m
ifeq ($(EXT_MODE),1)
  EXT_CC_OPTS = -DEXT_MODE -DWIN32  -DVERBOSE -DQNX_OS
 ifeq ($(EXTMODE_TRANSPORT),0) #tcpip
    EXT_SRC = ext_qnx_work.c \
          ext_qnx_svr.c \
          updown_qnx.c \
          rtiostream_tcpip_qnx.c \
          rtiostream_interface_qnx.c
    
  endif
  ifeq ($(EXTMODE_TRANSPORT),1) #serial_win32
    EXT_SRC  = ext_svr.c updown.c ext_work.c ext_svr_serial_transport.c
    EXT_SRC += ext_serial_pkt.c ext_serial_win32_port.c
  endif
  ifeq ($(TMW_EXTMODE_TESTING),1)
    EXT_SRC     += ext_test.c
    EXT_CC_OPTS += -DTMW_EXTMODE_TESTING
  endif
  ifeq ($(EXTMODE_STATIC),1)
    EXT_SRC     += mem_mgr.c
    EXT_CC_OPTS += -DEXTMODE_STATIC -DEXTMODE_STATIC_SIZE=$(EXTMODE_STATIC_SIZE)
  endif
endif


SFCN = 0
ifeq ($(MODELREF_TARGET_TYPE),SIM)
  SFCN = 1
endif
ifeq ($(B_ERTSFCN),1)
  SFCN = 1
endif

#------------------------------ Include Path -----------------------------------


ADD_INCLUDES = \
	-I$(START_DIR)/tangazh_stabilization_algorithm_qnx_rtw \
	-I$(START_DIR) \
	-I$(START_DIR)/Work/qnx/ModelKraynov \



SHARED_INCLUDES =
USER_INCLUDES = 

SHARED_INCLUDES =
ifneq ($(SHARED_SRC_DIR),)
SHARED_INCLUDES = -I$(SHARED_SRC_DIR) 
endif

ifeq ($(TARGET_LANG_EXT),cpp)
OPT_OPTS = -lang-c++
else
OPT_OPTS =
endif
OPT_OPTS += $(DEFAULT_OPT_OPTS) -g   -Bstatic -static


OPTS = #-V4.4.2,gcc_ntoarmv7le -Bstatic -static


ifneq ($(OPTIMIZATION_FLAGS),)
CC_OPTS =  -$(OPTS) $(ANSI_OPTS) $(EXT_CC_OPTS) $(OPTIMIZATION_FLAGS)
MEX_OPT_OPTS = OPTIMFLAGS="$(OPTIMIZATION_FLAGS)"
else
CC_OPTS =  $(OPT_OPTS) $(OPTS) $(ANSI_OPTS) $(EXT_CC_OPTS)
MEX_OPT_OPTS = $(OPT_OPTS)
endif

CPP_REQ_DEFINES = -DMODEL=$(MODEL) -DNUMST=$(NUMST) -DNCSTATES=$(NCSTATES) \
		  -DMAT_FILE=$(MAT_FILE) -DINTEGER_CODE=$(INTEGER_CODE) \
		  -DONESTEPFCN=$(ONESTEPFCN) -DTERMFCN=$(TERMFCN) \
		  -DHAVESTDIO -DMULTI_INSTANCE_CODE=$(MULTI_INSTANCE_CODE)

ifeq ($(MODELREF_TARGET_TYPE),SIM)
CPP_REQ_DEFINES += -DMDL_REF_SIM_TGT=1
else
CPP_REQ_DEFINES += -DMT=$(MULTITASKING)
endif

ifeq ($(PORTABLE_WORDSIZES),1)
CPP_REQ_DEFINES += -DPORTABLE_WORDSIZES
endif

CFLAGS = $(CC_OPTS) $(CPP_REQ_DEFINES) $(INCLUDES) 
CPPFLAGS = $(CFLAGS)

ifeq ($(OPT_OPTS),$(DEFAULT_OPT_OPTS))

LDFLAGS = 
else

LDFLAGS = 
endif

ifeq ($(TARGET_LANG_EXT),cpp)
LDFLAGS += -lstdc++
endif

ifneq ($(ADDITIONAL_LDFLAGS),)
MEX_LDFLAGS = LINKFLAGS="$$LINKFLAGS $(ADDITIONAL_LDFLAGS)"
else
MEX_LDFLAGS =
endif

#-------------------------- Additional Libraries ------------------------------

LIBS =


LIBS += $(EXT_LIB) $(S_FUNCTIONS_LIB)

ifeq ($(MODELREF_TARGET_TYPE),SIM)
LIBMWIPP = $(MATLAB_ROOT)\lib\win32\libippmwipt.lib
LIBS += $(LIBMWIPP)
endif

ifeq ($(SFCN),1)
LIBFIXPT = $(MATLAB_ROOT)\extern\lib\win32\lcc\libfixedpoint.lib
LIBS += $(LIBFIXPT)
endif

ifeq ($(MODELREF_TARGET_TYPE),SIM)
LIBMWMATHUTIL = $(MATLAB_ROOT)\extern\lib\win32\lcc\libmwmathutil.lib
LIBS += $(LIBMWMATHUTIL)
endif

ifeq ($(MODELREF_TARGET_TYPE),SIM)
LIBMWSL_FILEIO = $(MATLAB_ROOT)\extern\lib\win32\lcc\libmwsl_fileio.lib
LIBS += $(LIBMWSL_FILEIO)
endif



#--------------------------- Tool Specifications -------------------------------

include '$(QNX_MW_ROOT)/qnx/qnxtools.mak'

#----------------------------- Source Files ------------------------------------
ADD_SRCS = ert_qnx_utils.c

ifeq ($(SFCN),0)
  SRCS = $(ADD_SRCS) $(S_FUNCTIONS)
  SRC_DEP =
  ifeq ($(MODELREF_TARGET_TYPE), NONE)
    ifeq ($(SHRLIBTARGET),1)

      PRODUCT            = $(RELATIVE_PATH_TO_ANCHOR)/$(MODEL)_win32.dll
      BIN_SETTING        = $(LD) $(LDFLAGS) $(ADDITIONAL_LDFLAGS) -dll -o $(PRODUCT) $(SYSLIBS) 
      BUILD_PRODUCT_TYPE = dynamically linked library
      SRCS               += $(MODULES) $(MODEL).c lcc_dll_main.c $(EXT_SRC) 
    else
      ifeq ($(MAKEFILEBUILDER_TGT),1)
        PRODUCT            = $(MODEL).exe
        BIN_SETTING        = $(LD) $(LDFLAGS) $(ADDITIONAL_LDFLAGS) -o $(PRODUCT) $(SYSLIBS) 
        BUILD_PRODUCT_TYPE = executable
        PREBUILT_OBJS       = $(MODULES:.c=.obj)
      else
        ifeq ($(STANDALONE_SUPPRESS_EXE),1)

          PRODUCT            = ObjectModulesOnly
          BUILD_PRODUCT_TYPE = object modules
          SRCS               += $(MODULES) $(MODEL).c $(EXT_SRC)
        else

          PRODUCT            = $(RELATIVE_PATH_TO_ANCHOR)/$(MODEL)
          BIN_SETTING        = $(LD) $(LDFLAGS) $(ADDITIONAL_LDFLAGS) -o $(PRODUCT) $(SYSLIBS)
          BUILD_PRODUCT_TYPE = executable
          SRCS               += $(MODULES) $(MODEL).$(TARGET_LANG_EXT) ert_main.$(TARGET_LANG_EXT) $(EXT_SRC)
        endif
      endif
    endif
  else

    SRCS               += $(MODULES) 
    PRODUCT            = $(MODELLIB)
    BUILD_PRODUCT_TYPE = library
  endif
else
  ifeq ($(MODELREF_TARGET_TYPE), SIM)
  PRODUCT            = $(RELATIVE_PATH_TO_ANCHOR)/$(MODEL)$(MODELREF_SFCN_SUFFIX).$(MEXEXT)
  RTW_SFUN_SRC       = $(MODEL)$(MODELREF_SFCN_SUFFIX).c
  SRCS               = $(MODULES) $(ADD_SRCS)
  else
  PRODUCT            = $(RELATIVE_PATH_TO_ANCHOR)/$(MODEL)_sf.$(MEXEXT)
  RTW_SFUN_SRC       = $(MODEL)_sf.c
  SRCS               = $(MODULES) $(ADD_SRCS) $(S_FUNCTIONS)
  BIN_SETTING        = $(MATLAB_BIN)\mex.bat $(MEX_OPT_OPTS) -win32 $(MEX_LDFLAGS) $(RTW_SFUN_SRC) $(INCLUDES) -outdir $(RELATIVE_PATH_TO_ANCHOR) 
  endif  
  BUILD_PRODUCT_TYPE = mex file
  ifeq ($(B_ERTSFCN),1)
    SRCS              += $(MODEL).c    
  endif
  SRC_DEP            = $(RTW_SFUN_SRC)
endif

USER_SRCS =

USER_OBJS       = $(USER_SRCS:.c=.o)
LOCAL_USER_OBJS = $(notdir $(USER_OBJS))

EXT_OBJS = $(EXT_SRC:.c=.o)

OBJS_ = $(SRCS:.c=.o) $(USER_OBJS) $(ERT_MAIN) $(EXT_OBJS) rtiostream
OBJS  = $(OBJS_:.cpp=.o)
LINK_OBJS_ = $(SRCS:.c=.o) $(LOCAL_USER_OBJS)
LINK_OBJS = $(LINK_OBJS_:.cpp=.o)

SHARED_OBJS := $(addsuffix .o, $(basename $(wildcard $(SHARED_SRC))))
FMT_SHARED_OBJS = $(subst /,\,$(SHARED_OBJS))

#--------------------------------- Rules ---------------------------------------
ifeq ($(MODELREF_TARGET_TYPE),NONE)
  ifeq ($(SHRLIBTARGET),1)
    $(PRODUCT) : $(OBJS) $(SHARED_LIB) $(LIBS) $(MODELREF_LINK_LIBS)
	$(BIN_SETTING) $(LINK_OBJS) @$(MODELREF_LINK_RSPFILE) $(SHARED_LIB) $(LIBS)  $(MODEL).def

	@del $(RELATIVE_PATH_TO_ANCHOR)\$(MODEL)_win32.lib $(RELATIVE_PATH_TO_ANCHOR)\$(MODEL)_win32.exp
	@echo "### Created $(BUILD_PRODUCT_TYPE): $@"
  else
    ifeq ($(MAKEFILEBUILDER_TGT),1)
      $(PRODUCT) : $(PREBUILT_OBJS) $(OBJS) $(MODELLIB) $(SHARED_LIB) $(LIBS) $(SRC_DEP) $(MODELREF_LINK_LIBS)
	$(BIN_SETTING) $(PREBUILT_OBJS) $(LINK_OBJS) $(MODELLIB) @$(MODELREF_LINK_RSPFILE) $(SHARED_LIB) $(LIBS)
	@echo "### Created $(BUILD_PRODUCT_TYPE): $@"
    else
      ifeq ($(STANDALONE_SUPPRESS_EXE), 1)
        .PHONY: $(PRODUCT)
        $(PRODUCT) : $(OBJS) $(SHARED_LIB) $(LIBS) $(SRC_DEP) $(MODELREF_LINK_LIBS)
	@echo "### Created $(BUILD_PRODUCT_TYPE): $@"
      else
        $(PRODUCT) : $(OBJS) $(SHARED_LIB) $(LIBS) $(SRC_DEP) $(MODELREF_LINK_LIBS)
	$(BIN_SETTING) $(LINK_OBJS) @$(MODELREF_LINK_RSPFILE) $(SHARED_LIB) $(LIBS) -lm -lsocket -lc
	@echo "### Created $(BUILD_PRODUCT_TYPE): $@"
      endif
    endif
  endif
else
 ifeq ($(MODELREF_TARGET_TYPE),SIM)
  $(PRODUCT) : $(OBJS) $(SHARED_LIB) $(LIBS) $(SRC_DEP)
	@if exist $(MODELLIB) del $(MODELLIB)
	$(LIBCMD) -r $(MODELLIB) $(LINK_OBJS)
	@echo "### Created $(MODELLIB)"
	$(MATLAB_BIN)\mex.bat -win32 $(MEX_LDFLAGS) -c $(RTW_SFUN_SRC) $(INCLUDES) 
	$(MATLAB_BIN)\mex.bat -win32 $(MEX_LDFLAGS) -outdir $(RELATIVE_PATH_TO_ANCHOR) $(MODEL)$(MODELREF_SFCN_SUFFIX).obj $(MODELLIB) @$(MODELREF_LINK_RSPFILE) $(SHARED_LIB) $(LIBS) 
	@echo "### Created $(BUILD_PRODUCT_TYPE): $@"
 else
  $(PRODUCT) : $(OBJS) $(SHARED_LIB) $(SRC_DEP)
	@if exist $(MODELLIB) del $(MODELLIB)
	$(LIBCMD) -r $(MODELLIB) $(LINK_OBJS)
	@echo "### Created $(MODELLIB)"
	@echo "### Created $(BUILD_PRODUCT_TYPE): $@"
 endif
endif

%.o : %.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : %.C
	$(CC) -c -o $@ $(CFLAGS) $<
	
%.o : %.cpp
	$(CC) -c -o $@ $(CPPFLAGS) $<

%.o : $(RELATIVE_PATH_TO_ANCHOR)/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(RELATIVE_PATH_TO_ANCHOR)/%.C
	$(CC) -c -o $@ $(CFLAGS) $<
	
%.o : $(RELATIVE_PATH_TO_ANCHOR)/%.cpp
	$(CC) -c -o $@ $(CPPFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/ert/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/ert/%.C
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/%.C
	$(CC) -c -o $@  $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/common/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/common/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/rtiostream/rtiostreamtcpip/%.c
	$(CC) -c -o $(@) $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/serial/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/serial/%.C
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/custom/%.c
	$(CC) -c -o $@  $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/custom/%.C
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(QNX_MW_ROOT)/qnx/%.c
	$(CC) -c -o $(@F)  $(CFLAGS) $<

%.o : $(QNX_MW_ROOT)/ext_mode/target/%.c
	$(CC) -c -o $(@F)  $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/common/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/rtiostream/rtiostreamtcpip/%.c
	$(CC) -c -o $@ $(CFLAGS) $<



%.o : $(MATLAB_ROOT)/rtw/c/src/%.C
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/common/%.C
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/rtiostream/rtiostreamtcpip/%.C
	$(CC) -c -o $@ $(CFLAGS) $<



%.o : $(MATLAB_ROOT)/rtw/c/src/%.cpp
	$(CC) -c -o $@ $(CPPFLAGS) $<
%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/common/%.cpp
	$(CC) -c -o $@ $(CPPFLAGS) $<
%.o : $(MATLAB_ROOT)/rtw/c/src/rtiostream/rtiostreamtcpip/%.cpp
	$(CC) -c -o $@ $(CPPFLAGS) $<


%.o : $(MATLAB_ROOT)/simulink/src/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/simulink/src/%.C
	$(CC) -c -o $@ $(CFLAGS) $<

# Libraries:





#----------------------------- Dependencies ------------------------------------

$(OBJS) : $(MAKEFILE) rtw_proj.tmw

$(SHARED_OBJS) : $(SHARED_BIN_DIR)/%.o : $(SHARED_SRC_DIR)/%.c  
	$(CC) -c -o $@ $(CFLAGS) $<

$(SHARED_LIB) : $(SHARED_OBJS)
	@echo "### Creating $@"
	@if exist $@ del $@
	$(LIBCMD) -r $@ $(FMT_SHARED_OBJS)
	@echo "### $@ Created"

#--------- Miscellaneous rules to purge, clean and lint  ------------

purge : clean
	@echo "### Deleting the generated source code for $(MODEL)"
	@del $(MODEL).c $(MODEL).h $(MODEL)_types.h $(MODEL)_data.c $(MODEL).rtw \
	        $(MODEL)_private.h $(MODULES) rtw_proj.tmw $(MAKEFILE)

clean :
	@echo "### Deleting the objects and $(PROGRAM)"
	@del $(LINK_OBJS) ..\$(MODEL)

# EOF: tangazh_stabilization_algorithm.mk
