#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/libtrudp/packet.o \
	${OBJECTDIR}/libtrudp/queue.o \
	${OBJECTDIR}/libtrudp/send_queue.o \
	${OBJECTDIR}/main.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/libtrudp/tests/packet_t.o \
	${TESTDIR}/libtrudp/tests/queue_t.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tr-udp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tr-udp: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tr-udp ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/libtrudp/packet.o: libtrudp/packet.c 
	${MKDIR} -p ${OBJECTDIR}/libtrudp
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libtrudp/packet.o libtrudp/packet.c

${OBJECTDIR}/libtrudp/queue.o: libtrudp/queue.c 
	${MKDIR} -p ${OBJECTDIR}/libtrudp
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libtrudp/queue.o libtrudp/queue.c

${OBJECTDIR}/libtrudp/send_queue.o: libtrudp/send_queue.c 
	${MKDIR} -p ${OBJECTDIR}/libtrudp
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libtrudp/send_queue.o libtrudp/send_queue.c

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f1: ${TESTDIR}/libtrudp/tests/packet_t.o ${TESTDIR}/libtrudp/tests/queue_t.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc}   -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} -lcunit 


${TESTDIR}/libtrudp/tests/packet_t.o: libtrudp/tests/packet_t.c 
	${MKDIR} -p ${TESTDIR}/libtrudp/tests
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${TESTDIR}/libtrudp/tests/packet_t.o libtrudp/tests/packet_t.c


${TESTDIR}/libtrudp/tests/queue_t.o: libtrudp/tests/queue_t.c 
	${MKDIR} -p ${TESTDIR}/libtrudp/tests
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${TESTDIR}/libtrudp/tests/queue_t.o libtrudp/tests/queue_t.c


${OBJECTDIR}/libtrudp/packet_nomain.o: ${OBJECTDIR}/libtrudp/packet.o libtrudp/packet.c 
	${MKDIR} -p ${OBJECTDIR}/libtrudp
	@NMOUTPUT=`${NM} ${OBJECTDIR}/libtrudp/packet.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libtrudp/packet_nomain.o libtrudp/packet.c;\
	else  \
	    ${CP} ${OBJECTDIR}/libtrudp/packet.o ${OBJECTDIR}/libtrudp/packet_nomain.o;\
	fi

${OBJECTDIR}/libtrudp/queue_nomain.o: ${OBJECTDIR}/libtrudp/queue.o libtrudp/queue.c 
	${MKDIR} -p ${OBJECTDIR}/libtrudp
	@NMOUTPUT=`${NM} ${OBJECTDIR}/libtrudp/queue.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libtrudp/queue_nomain.o libtrudp/queue.c;\
	else  \
	    ${CP} ${OBJECTDIR}/libtrudp/queue.o ${OBJECTDIR}/libtrudp/queue_nomain.o;\
	fi

${OBJECTDIR}/libtrudp/send_queue_nomain.o: ${OBJECTDIR}/libtrudp/send_queue.o libtrudp/send_queue.c 
	${MKDIR} -p ${OBJECTDIR}/libtrudp
	@NMOUTPUT=`${NM} ${OBJECTDIR}/libtrudp/send_queue.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/libtrudp/send_queue_nomain.o libtrudp/send_queue.c;\
	else  \
	    ${CP} ${OBJECTDIR}/libtrudp/send_queue.o ${OBJECTDIR}/libtrudp/send_queue_nomain.o;\
	fi

${OBJECTDIR}/main_nomain.o: ${OBJECTDIR}/main.o main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/main.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main_nomain.o main.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/main.o ${OBJECTDIR}/main_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/tr-udp

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc