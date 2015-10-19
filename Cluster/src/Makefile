#
# OMNeT++/OMNEST Makefile for Cluster
#
# This file was generated with the command:
#  opp_makemake -f --deep
#

# Name of target to be created (-o option)
TARGET = Cluster$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -Iarrivals \
    -Idistr \
    -Igraphs \
    -Igraphs/graphsavg \
    -Ijob \
    -Imachine \
    -Imessages \
    -Inet \
    -Ischeduler \
    -Itask

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/arrivals/JobArrival.o \
    $O/arrivals/MachineArrival.o \
    $O/distr/MachineAttributesDistr.o \
    $O/distr/TaskConstraintsDistr.o \
    $O/job/Job.o \
    $O/machine/MachineState.o \
    $O/machine/MachineUsage.o \
    $O/machine/Machine.o \
    $O/machine/MachineAttributes.o \
    $O/machine/MachineInitializer.o \
    $O/net/Net.o \
    $O/scheduler/Scheduler.o \
    $O/task/Task.o \
    $O/task/TaskConstraints.o \
    $O/messages/basemsg_m.o

# Message files
MSGFILES = \
    messages/basemsg.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f Cluster Cluster.exe libCluster.so libCluster.a libCluster.dll libCluster.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f arrivals/*_m.cc arrivals/*_m.h
	-rm -f distr/*_m.cc distr/*_m.h
	-rm -f graphs/*_m.cc graphs/*_m.h
	-rm -f graphs/graphsavg/*_m.cc graphs/graphsavg/*_m.h
	-rm -f job/*_m.cc job/*_m.h
	-rm -f machine/*_m.cc machine/*_m.h
	-rm -f messages/*_m.cc messages/*_m.h
	-rm -f net/*_m.cc net/*_m.h
	-rm -f scheduler/*_m.cc scheduler/*_m.h
	-rm -f task/*_m.cc task/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc arrivals/*.cc distr/*.cc graphs/*.cc graphs/graphsavg/*.cc job/*.cc machine/*.cc messages/*.cc net/*.cc scheduler/*.cc task/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/arrivals/JobArrival.o: arrivals/JobArrival.cc \
	arrivals/JobArrival.h \
	distr/TaskConstraintsDistr.h \
	job/Job.h \
	messages/basemsg_m.h \
	net/Net.h \
	task/Task.h \
	task/TaskConstraints.h
$O/arrivals/MachineArrival.o: arrivals/MachineArrival.cc \
	arrivals/MachineArrival.h \
	messages/basemsg_m.h \
	net/Net.h
$O/distr/MachineAttributesDistr.o: distr/MachineAttributesDistr.cc \
	distr/MachineAttributesDistr.h \
	machine/MachineAttributes.h \
	task/Task.h \
	task/TaskConstraints.h
$O/distr/TaskConstraintsDistr.o: distr/TaskConstraintsDistr.cc \
	distr/TaskConstraintsDistr.h \
	task/TaskConstraints.h
$O/job/Job.o: job/Job.cc \
	job/Job.h \
	task/Task.h \
	task/TaskConstraints.h
$O/machine/Machine.o: machine/Machine.cc \
	distr/MachineAttributesDistr.h \
	job/Job.h \
	machine/Machine.h \
	machine/MachineAttributes.h \
	machine/MachineInitializer.h \
	machine/MachineState.h \
	messages/basemsg_m.h \
	net/Net.h \
	task/Task.h \
	task/TaskConstraints.h
$O/machine/MachineAttributes.o: machine/MachineAttributes.cc \
	machine/MachineAttributes.h \
	task/Task.h \
	task/TaskConstraints.h
$O/machine/MachineInitializer.o: machine/MachineInitializer.cc \
	distr/MachineAttributesDistr.h \
	machine/MachineAttributes.h \
	machine/MachineInitializer.h \
	task/Task.h \
	task/TaskConstraints.h
$O/machine/MachineState.o: machine/MachineState.cc \
	machine/MachineState.h \
	task/Task.h \
	task/TaskConstraints.h
$O/machine/MachineUsage.o: machine/MachineUsage.cc \
	machine/MachineUsage.h
$O/messages/basemsg_m.o: messages/basemsg_m.cc \
	messages/basemsg_m.h
$O/net/Net.o: net/Net.cc \
	messages/basemsg_m.h \
	net/Net.h
$O/scheduler/Scheduler.o: scheduler/Scheduler.cc \
	job/Job.h \
	machine/Machine.h \
	machine/MachineAttributes.h \
	machine/MachineState.h \
	machine/MachineUsage.h \
	messages/basemsg_m.h \
	net/Net.h \
	scheduler/Scheduler.h \
	task/Task.h \
	task/TaskConstraints.h
$O/task/Task.o: task/Task.cc \
	task/Task.h \
	task/TaskConstraints.h
$O/task/TaskConstraints.o: task/TaskConstraints.cc \
	task/TaskConstraints.h

