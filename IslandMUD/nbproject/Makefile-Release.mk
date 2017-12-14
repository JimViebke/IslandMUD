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
CC=gcc-4.9
CCC=g++-4.9
CXX=g++-4.9
FC=gfortran
AS=as

# Macros
CND_PLATFORM=None-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/NPC/non_player_character.o \
	${OBJECTDIR}/NPC/npc_enemy.o \
	${OBJECTDIR}/NPC/npc_enemy_bodyguard.o \
	${OBJECTDIR}/NPC/npc_enemy_corporal.o \
	${OBJECTDIR}/NPC/npc_enemy_fighter.o \
	${OBJECTDIR}/NPC/npc_enemy_worker.o \
	${OBJECTDIR}/NPC/npc_neutral.o \
	${OBJECTDIR}/XML/pugixml.o \
	${OBJECTDIR}/coordinate.o \
	${OBJECTDIR}/item/bloomery.o \
	${OBJECTDIR}/item/chest.o \
	${OBJECTDIR}/item/container.o \
	${OBJECTDIR}/item/equipment.o \
	${OBJECTDIR}/item/table.o \
	${OBJECTDIR}/network/connection.o \
	${OBJECTDIR}/network/network.o \
	${OBJECTDIR}/network/port.o \
	${OBJECTDIR}/server/server.o \
	${OBJECTDIR}/threadsafe/threadsafe_connection_lookup.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-pthread -std=c++14
CXXFLAGS=-pthread -std=c++14

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/islandmud

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/islandmud: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/islandmud ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/NPC/non_player_character.o: NPC/non_player_character.cpp 
	${MKDIR} -p ${OBJECTDIR}/NPC
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NPC/non_player_character.o NPC/non_player_character.cpp

${OBJECTDIR}/NPC/npc_enemy.o: NPC/npc_enemy.cpp 
	${MKDIR} -p ${OBJECTDIR}/NPC
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NPC/npc_enemy.o NPC/npc_enemy.cpp

${OBJECTDIR}/NPC/npc_enemy_bodyguard.o: NPC/npc_enemy_bodyguard.cpp 
	${MKDIR} -p ${OBJECTDIR}/NPC
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NPC/npc_enemy_bodyguard.o NPC/npc_enemy_bodyguard.cpp

${OBJECTDIR}/NPC/npc_enemy_corporal.o: NPC/npc_enemy_corporal.cpp 
	${MKDIR} -p ${OBJECTDIR}/NPC
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NPC/npc_enemy_corporal.o NPC/npc_enemy_corporal.cpp

${OBJECTDIR}/NPC/npc_enemy_fighter.o: NPC/npc_enemy_fighter.cpp 
	${MKDIR} -p ${OBJECTDIR}/NPC
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NPC/npc_enemy_fighter.o NPC/npc_enemy_fighter.cpp

${OBJECTDIR}/NPC/npc_enemy_worker.o: NPC/npc_enemy_worker.cpp 
	${MKDIR} -p ${OBJECTDIR}/NPC
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NPC/npc_enemy_worker.o NPC/npc_enemy_worker.cpp

${OBJECTDIR}/NPC/npc_neutral.o: NPC/npc_neutral.cpp 
	${MKDIR} -p ${OBJECTDIR}/NPC
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NPC/npc_neutral.o NPC/npc_neutral.cpp

${OBJECTDIR}/XML/pugixml.o: XML/pugixml.cpp 
	${MKDIR} -p ${OBJECTDIR}/XML
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/XML/pugixml.o XML/pugixml.cpp

${OBJECTDIR}/coordinate.o: coordinate.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/coordinate.o coordinate.cpp

${OBJECTDIR}/item/bloomery.o: item/bloomery.cpp 
	${MKDIR} -p ${OBJECTDIR}/item
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/item/bloomery.o item/bloomery.cpp

${OBJECTDIR}/item/chest.o: item/chest.cpp 
	${MKDIR} -p ${OBJECTDIR}/item
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/item/chest.o item/chest.cpp

${OBJECTDIR}/item/container.o: item/container.cpp 
	${MKDIR} -p ${OBJECTDIR}/item
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/item/container.o item/container.cpp

${OBJECTDIR}/item/equipment.o: item/equipment.cpp 
	${MKDIR} -p ${OBJECTDIR}/item
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/item/equipment.o item/equipment.cpp

${OBJECTDIR}/item/table.o: item/table.cpp 
	${MKDIR} -p ${OBJECTDIR}/item
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/item/table.o item/table.cpp

${OBJECTDIR}/network/connection.o: network/connection.cpp 
	${MKDIR} -p ${OBJECTDIR}/network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/network/connection.o network/connection.cpp

${OBJECTDIR}/network/network.o: network/network.cpp 
	${MKDIR} -p ${OBJECTDIR}/network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/network/network.o network/network.cpp

${OBJECTDIR}/network/port.o: network/port.cpp 
	${MKDIR} -p ${OBJECTDIR}/network
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/network/port.o network/port.cpp

${OBJECTDIR}/server/server.o: server/server.cpp 
	${MKDIR} -p ${OBJECTDIR}/server
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/server/server.o server/server.cpp

${OBJECTDIR}/threadsafe/threadsafe_connection_lookup.o: threadsafe/threadsafe_connection_lookup.cpp 
	${MKDIR} -p ${OBJECTDIR}/threadsafe
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/threadsafe/threadsafe_connection_lookup.o threadsafe/threadsafe_connection_lookup.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/islandmud

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
