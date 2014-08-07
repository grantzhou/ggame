# ----------------------------------------------
# Makefile for ggame project
#
# History
# Aug 02, 2014     Grant Zhou   Initial Version
# ----------------------------------------------- 

# Global necessary definitions
.SUFFIXES: .c .o .cpp .a
CC=gcc
CEXT   = .c
CPPEXT = .cpp
OBJEXT = .o
HEXT   = .h
INSTALL = /usr/bin/install
TARGET_HOME=/usr/local/ggame
PRODUCTION = $(abspath .)/production
PACKAGES = $(abspath .)/packages
PRODUCTION_HOME = $(PRODUCTION)/$(TARGET_HOME)
PRODUCTION_EXECUTABLE = $(PRODUCTION_HOME)/bin
PRODUCTION_LIB = $(PRODUCTION_HOME)/lib
PKG_CONTROL_DIR = CONTROL
BIN_DIR=bin
LIB_DIR=lib
BIN_SUFFIX=release
CPPFLAGS += -Wall
# -std=c99  -D_POSIX_C_SOURCE=199309

# Use RELEASE=1 for release build
ifndef RELEASE
    CPPFLAGS += -g
    BIN_SUFFIX=debug
endif

# Optimization
ifdef OPTIMIZE
    CPPFLAGS += -O3
endif

# Version#
ifdef VERSION
    CPPFLAGS       += -DVERSION_STR=\"${VERSION}\"
else
    CPPFLAGS       += -DVERSION_STR=\"1.0.1\"
endif

# GIT VERSION
CPPFLAGS           += -D_GIT_VERSION=\"$(shell git describe --abbrev=4 --dirty --always 2>/dev/null)\"

# BUILDER INFO
ifdef _BUILDER
    CPPFLAGS       += -D_BUILDER=\"${_BUILDER}\"
else
    CPPFLAGS       += -D_BUILDER=\"$(shell whoami)@$(shell uname -n)\"
endif

# BUILD Date
ifdef _BUILDDATE
    CPPFLAGS       += -D_BUILDDATE=\"${_BUILDDATE}\"
else
    CPPFLAGS       += -D_BUILDDATE=\"$(shell date +"%c" | tr '[:lower:]' '[:upper:]' | tr ' :' '__')\"
endif


ARCH=i386
# ARM Compile
ifdef ARM
    CPPFLAGS       += -DARM=\"${ARM}\"
    ARCH=arm
endif

OBJ_DIR_REL=./$(ARCH)/$(BIN_SUFFIX)/$(BIN_DIR)/
OBJ_DIR=$(OBJ_DIR_REL)
BUILD_LIB_DIR=./$(ARCH)/$(BIN_SUFFIX)/$(LIB_DIR)/
BUILD_BIN_DIR=./$(ARCH)/$(BIN_SUFFIX)/$(BIN_DIR)/
BUILD_PREFIX_DIR= ./$(ARCH)/$(BIN_SUFFIX)/
EXT_LIB_DIR = ./3rdParty/lib/$(ARCH)
CSOURCES = $(filter %$(CEXT), $(SOURCES))
HEADERS  = $(filter %$(HEXT), $(SOURCES))
CPPSOURCES = $(filter %$(CPPEXT), $(SOURCES))
MOCHEADERS = $(addprefix moc, $(HEADERS))
MOCSOURCES = $(subst $(HEXT),$(CPPEXT),$(MOCHEADERS))

OBJECTS = $(subst $(CEXT),$(OBJEXT),$(CSOURCES)) \
$(subst $(CPPEXT),$(OBJEXT),$(MOCSOURCES)) \
$(subst $(CPPEXT),$(OBJEXT),$(CPPSOURCES))

BIN_OBJECTS= $(addprefix $(OBJ_DIR),$(notdir $(OBJECTS)))    
OTHER_DEP =  $(filter-out %$(CPPEXT) %(CEXT), $(SOURCES))      
CPPFLAGS += $(MACHINE_FLAGS)
CPPFLAGS += $(EXFLAGS)
CPPFLAGS += $(addprefix -I,$(MY_INCLUDES))

# external liraries linked
MY_LIBS   = -lrt

# ar flags: c - create r - insert file members into archive
ARFLAGS=rc

$(OBJ_DIR)%.o : %.c
	@mkdir -p $(dir $@)
	@echo "Building "$@
	$(CC) -fPIC -o $@ $(CPPFLAGS) -c $<

$(OBJ_DIR)% : $(OBJ_DIR)%.o
	$(CC) -o $@ $(CPPFLAGS) $(LINKFLAGS) $(BIN_OBJECTS) $(MY_LIBS) $(EXT_LIBS)

$(OBJ_DIR)lib%.a: $(BIN_OBJECTS)%.o
	$(AR) $(ARFLAGS) $@ $(BIN_OBJECTS)

$(PRODUCTION_EXECUTABLE)/% : $(OBJ_DIR)%
	$(INSTALL) -d $(@D)
	$(INSTALL) -D -m 644 $< $@	

$(PRODUCTION_LIB)/% : $(LIB_DIR)%
	$(INSTALL) -d $(@D)


# global dependencies (must stay here)

# ----------------------------------------
#  Include other modules that we need here
# ----------------------------------------
MY_INCLUDES=../include 				\
	    	include 				\
            ../3rdParty/include/ 	\
            ../3rdParty/uthash

# ----------------------------------------
# Other module libraries that we need here
# ----------------------------------------
LINKFLAGS += -L$(BUILD_LIB_DIR)

# ----------------------------------------
# Source files for this module
# ----------------------------------------
VPATH = $(OBJ_DIR):src/

SOURCES=src/SysLogging.c \
	src/CommonInc.c \
	src/CommonFsm.c \
	src/GGameMainLEDView.c \
	src/GGameMainModel.c \
	src/GGameMainController.c

# ----------------------------------------
# main executable target made by this make
# ----------------------------------------
EXEC       = ggame

all: $(BUILD_BIN_DIR)$(EXEC)

$(BUILD_BIN_DIR)$(EXEC):  $(BIN_OBJECTS)  $(MY_LIBS_DEP) $(GLB_DEP) $(OTHER_DEP)
	@mkdir -p `dirname $@`
	rm -f $@
	$(CC) $(MACHINE_FLAGS) $(LINKFLAGS) $(MY_LIBS) $(OBJ_DIR)*.o  $(MY_LIBS) -o $@
	@echo

.PHONY: clean
clean:
	@if [ -d $(OBJ_DIR) ] ; then \
		(rm -f $(OBJ_DIR)*)\
	fi
	rm -rf $(BUILD_BIN_DIR)$(EXEC)
	rm -rf $(OBJ_DIR)

.PHONY: install
install: $(PRODUCTION_EXECUTABLE)/$(EXEC)
