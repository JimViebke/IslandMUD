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
CND_PLATFORM=GNU-Linux-x86
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
	${OBJECTDIR}/NPC/npc_unaffiliated.o \
	${OBJECTDIR}/XML/pugixml.o \
	${OBJECTDIR}/character.o \
	${OBJECTDIR}/constants.o \
	${OBJECTDIR}/craft.o \
	${OBJECTDIR}/door.o \
	${OBJECTDIR}/game.o \
	${OBJECTDIR}/generator.o \
	${OBJECTDIR}/item.o \
	${OBJECTDIR}/item/bloomery.o \
	${OBJECTDIR}/item/chest.o \
	${OBJECTDIR}/item/container.o \
	${OBJECTDIR}/item/table.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/message.o \
	${OBJECTDIR}/parse.o \
	${OBJECTDIR}/player_character.o \
	${OBJECTDIR}/recipes.o \
	${OBJECTDIR}/room.o \
	${OBJECTDIR}/room_side.o \
	${OBJECTDIR}/utilities.o \
	${OBJECTDIR}/world.o


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

${OBJECTDIR}/NPC/npc_unaffiliated.o: NPC/npc_unaffiliated.cpp 
	${MKDIR} -p ${OBJECTDIR}/NPC
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NPC/npc_unaffiliated.o NPC/npc_unaffiliated.cpp

${OBJECTDIR}/XML/pugixml.o: XML/pugixml.cpp 
	${MKDIR} -p ${OBJECTDIR}/XML
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/XML/pugixml.o XML/pugixml.cpp

${OBJECTDIR}/character.o: character.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/character.o character.cpp

${OBJECTDIR}/constants.o: constants.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/constants.o constants.cpp

${OBJECTDIR}/craft.o: craft.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/craft.o craft.cpp

${OBJECTDIR}/door.o: door.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/door.o door.cpp

${OBJECTDIR}/game.o: game.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/game.o game.cpp

${OBJECTDIR}/generator.o: generator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/generator.o generator.cpp

${OBJECTDIR}/item.o: item.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/item.o item.cpp

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

${OBJECTDIR}/item/table.o: item/table.cpp 
	${MKDIR} -p ${OBJECTDIR}/item
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/item/table.o item/table.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/message.o: message.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/message.o message.cpp

${OBJECTDIR}/parse.o: parse.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/parse.o parse.cpp

${OBJECTDIR}/player_character.o: player_character.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/player_character.o player_character.cpp

${OBJECTDIR}/recipes.o: recipes.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/recipes.o recipes.cpp

${OBJECTDIR}/room.o: room.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/room.o room.cpp

${OBJECTDIR}/room_side.o: room_side.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/room_side.o room_side.cpp

${OBJECTDIR}/utilities.o: utilities.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utilities.o utilities.cpp

${OBJECTDIR}/world.o: world.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Werror -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/world.o world.cpp

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
