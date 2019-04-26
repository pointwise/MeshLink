########################################################################
# Makefile (linux_x86_64)
########################################################################
# Check for valid machine types
#
KNOWN_MACHINES := linux_x86_64 macosx
ifeq ($(machine),)
    $(error empty machine type is not valid ($(KNOWN_MACHINES)))
endif
ifeq ($(findstring $(machine),$(KNOWN_MACHINES)),)
    $(error machine type ($(machine)) is not valid ($(KNOWN_MACHINES)))
endif

########################################################################
# Directories
#
PLATFORM = $(machine)
DIST_DIR = dist/$(PLATFORM)/bin
TEST_DIR = ../test
MESHLINK_DIR  = meshlink
MESHLINK_MODULE_NAME = meshlink
MLKERNEL_GEODE_DIR = mlkernel_geode
MLKERNEL_GEODE_MODULE_NAME = mlkernel_geode
MLPARSER_XERCES_DIR = mlparser_xerces
MLPARSER_XERCES_MODULE_NAME = mlparser_xerces
HARNESS_CPP_DIR = ../app/harness_cpp
HARNESS_CPP_NAME = harness_cpp
HARNESS_C_DIR = ../app/harness_c
HARNESS_C_NAME = harness_c
HARNESS_F_DIR = ../app/harness_f
HARNESS_F_NAME = harness_f
REFINE_STR_DIR = ../app/refine_str_cpp
REFINE_STR_NAME = refine_str
REFINE_UNS_DIR = ../app/refine_uns_cpp
REFINE_UNS_NAME = refine_uns

HAVE_XERCES := 1
#HAVE_GEODE := 1


#
# Check for a valid build type.  If none is specified, use Debug
#
ifndef BUILD
    BUILD := Debug
endif

KNOWN_BUILD_TYPES := Release Debug

ifeq ($(findstring $(BUILD),$(KNOWN_BUILD_TYPES)),)
    $(error BUILD type ($(BUILD)) is not valid ($(KNOWN_BUILD_TYPES)))
endif

#
# Common defines
#
LIB_SUFFIX := .a
ifeq ($(BUILD),Debug)
	  DBG_SUFFIX := d
	  DBG_EXE_SUFFIX := _dbg
else
	  DBG_SUFFIX :=
	  DBG_EXE_SUFFIX :=
endif



HARNESS_CPP_FULLNAME := ${HARNESS_CPP_NAME}${DBG_EXE_SUFFIX}
HARNESS_C_FULLNAME := ${HARNESS_C_NAME}${DBG_EXE_SUFFIX}
HARNESS_F_FULLNAME := ${HARNESS_F_NAME}${DBG_EXE_SUFFIX}
REFINE_STR_FULLNAME := ${REFINE_STR_NAME}${DBG_EXE_SUFFIX}
REFINE_UNS_FULLNAME := ${REFINE_UNS_NAME}${DBG_EXE_SUFFIX}

MESHLINK_FULLPATH := $(patsubst %,$(MESHLINK_DIR)/$(DIST_DIR)/%,$(MESHLINK_MODULE_NAME)$(DBG_SUFFIX)$(LIB_SUFFIX))
ML_MODULES_FULLPATH := $(MESHLINK_FULLPATH)
ifdef HAVE_GEODE
MLKERNEL_GEODE_FULLPATH := $(patsubst %,$(MLKERNEL_GEODE_DIR)/$(DIST_DIR)/%,$(MLKERNEL_GEODE_MODULE_NAME)$(DBG_SUFFIX)$(LIB_SUFFIX))
ML_MODULES_FULLPATH := $(ML_MODULES_FULLPATH) $(MLKERNEL_GEODE_FULLPATH)
endif
ifdef HAVE_XERCES
MLPARSER_XERCES_FULLPATH := $(patsubst %,$(MLPARSER_XERCES_DIR)/$(DIST_DIR)/%,$(MLPARSER_XERCES_MODULE_NAME)$(DBG_SUFFIX)$(LIB_SUFFIX))
ML_MODULES_FULLPATH := $(ML_MODULES_FULLPATH) $(MLPARSER_XERCES_FULLPATH)
endif

$(info $$ML_MODULES_FULLPATH is [${ML_MODULES_FULLPATH}])

########################################################################
# Tool Macros
#
MKDIR = mkdir -p
CP = cp -f


########################################################################
# Rules
#
.PHONY: dist

# Example executables
${HARNESS_CPP_NAME}: $(HARNESS_CPP_DIR)/$(DIST_DIR)/${HARNESS_CPP_FULLNAME}

$(HARNESS_CPP_DIR)/$(DIST_DIR)/${HARNESS_CPP_FULLNAME}: $(ML_MODULES_FULLPATH) 
	@echo ""
	@echo "***"
	@echo "*** $(HARNESS_CPP_NAME)"
	@echo "***"
	cd $(HARNESS_CPP_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) HAVE_XERCES=$(HAVE_XERCES) HAVE_GEODE=$(HAVE_GEODE)

${HARNESS_C_NAME}: $(HARNESS_C_DIR)/$(DIST_DIR)/${HARNESS_C_FULLNAME}

$(HARNESS_C_DIR)/$(DIST_DIR)/${HARNESS_C_FULLNAME}: $(ML_MODULES_FULLPATH) 
	@echo ""
	@echo "***"
	@echo "*** $(HARNESS_C_NAME)"
	@echo "***"
	cd $(HARNESS_C_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) HAVE_XERCES=$(HAVE_XERCES) HAVE_GEODE=$(HAVE_GEODE)

${HARNESS_F_NAME}: $(HARNESS_F_DIR)/$(DIST_DIR)/${HARNESS_F_FULLNAME}

$(HARNESS_F_DIR)/$(DIST_DIR)/${HARNESS_F_FULLNAME}: $(ML_MODULES_FULLPATH) 
	@echo ""
	@echo "***"
	@echo "*** $(HARNESS_F_NAME)"
	@echo "***"
	cd $(HARNESS_F_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) HAVE_XERCES=$(HAVE_XERCES) HAVE_GEODE=$(HAVE_GEODE)


${REFINE_STR_NAME}: $(REFINE_STR_DIR)/$(DIST_DIR)/${REFINE_STR_FULLNAME}

$(REFINE_STR_DIR)/$(DIST_DIR)/${REFINE_STR_FULLNAME}: $(ML_MODULES_FULLPATH) 
	@echo ""
	@echo "***"
	@echo "*** $(REFINE_STR_NAME)"
	@echo "***"
	cd $(REFINE_STR_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD)


${REFINE_UNS_NAME}: $(REFINE_UNS_DIR)/$(DIST_DIR)/${REFINE_UNS_FULLNAME}

$(REFINE_UNS_DIR)/$(DIST_DIR)/${REFINE_UNS_FULLNAME}: $(ML_MODULES_FULLPATH) 
	@echo ""
	@echo "***"
	@echo "*** $(REFINE_UNS_NAME)"
	@echo "***"
	cd $(REFINE_UNS_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD)

# MeshLink Libraries
${MESHLINK_FULLPATH}:
	@echo ""
	@echo "***"
	@echo "*** $(MESHLINK_FULLPATH)"
	@echo "***"
	cd $(MESHLINK_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD)

ifdef HAVE_GEODE
${MLKERNEL_GEODE_FULLPATH}:
	@echo ""
	@echo "***"
	@echo "*** $(MLKERNEL_GEODE_FULLPATH)"
	@echo "***"
	cd $(MLKERNEL_GEODE_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD)
endif

ifdef HAVE_XERCES
${MLPARSER_XERCES_FULLPATH}:
	@echo ""
	@echo "***"
	@echo "*** $(MLPARSER_XERCES_FULLPATH)"
	@echo "***"
	cd $(MLPARSER_XERCES_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD)
endif

# Tests
test_harness_cpp: ${HARNESS_CPP_NAME}
	@echo ""
	@echo "***"
	@echo "*** test_harness_cpp"
	@echo "***"
	cd $(TEST_DIR) && $(HARNESS_CPP_DIR)/$(DIST_DIR)/${HARNESS_CPP_FULLNAME} sphere_ml.xml

test_harness_c: ${HARNESS_C_NAME}
	@echo ""
	@echo "***"
	@echo "*** test_harness_c"
	@echo "***"
	cd $(TEST_DIR) && $(HARNESS_C_DIR)/$(DIST_DIR)/${HARNESS_C_FULLNAME} sphere_ml.xml

test_harness_f: ${HARNESS_F_NAME}
	@echo ""
	@echo "***"
	@echo "*** test_harness_f"
	@echo "***"
	cd $(TEST_DIR) && $(HARNESS_F_DIR)/$(DIST_DIR)/${HARNESS_F_FULLNAME} sphere_ml.xml

test_refine_str: ${REFINE_STR_NAME}
	@echo ""
	@echo "***"
	@echo "*** test_refine_str"
	@echo "***"
	cd $(TEST_DIR) && $(REFINE_STR_DIR)/$(DIST_DIR)/${REFINE_STR_FULLNAME} sphere_str_refine.xml


test_refine_uns: ${REFINE_UNS_NAME}
	@echo ""
	@echo "***"
	@echo "*** test_refine_uns"
	@echo "***"
	cd $(TEST_DIR) && $(REFINE_UNS_DIR)/$(DIST_DIR)/${REFINE_UNS_FULLNAME} oneraM6.xml

clean:
	cd $(MESHLINK_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) clean
	cd $(MLKERNEL_GEODE_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) clean
	cd $(MLPARSER_XERCES_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) clean
	cd $(HARNESS_CPP_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) clean
	cd $(HARNESS_C_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) clean
	cd $(HARNESS_F_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) clean
	cd $(REFINE_STR_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) clean
	cd $(REFINE_UNS_DIR) && make -f Makefile.$(PLATFORM) BUILD=$(BUILD) clean


