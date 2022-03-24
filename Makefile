#
# Makefile
#
# Creates a password generator from c++ source
#

ifeq ($(TARGET), win32)
Configuration = win32
else
Configuration = Release
endif

BINDIR = bin/$(Configuration)
OBJDIR = obj/$(Configuration)
DEPDIR := .deps

DIRTY = $(shell git diff-index --quiet HEAD || echo -dirty)
VERSION = $(shell git rev-parse --short HEAD)$(DIRTY)
DATE = $(shell date '+%y/%m/%d')

# auto-dependency files
DEPFLAGS = -MMD -MP -MF $(DEPDIR)/$(@F:.o=.d)

CFLAGS =
CPPFLAGS = -Werror -Wall ${DEPFLAGS} -I./${RESDIR}
CXXFLAGS = -std=c++11
LDLIBS = -lstdc++

# CSDEFINES = -define:TRACE 
# CSDEBUG = -debug:pdbonly -optimize+ 

ifeq ($(TARGET), win32)
CROSS_COMPILE = x86_64-w64-mingw32-
TARGET_ARCH = -m64 -mwin32
EXEEXT = .exe
LDFLAGS += -mconsole
endif
CXX = ${CROSS_COMPILE}g++
CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}ld

SRC = passphrase.cpp
DICS = en_CA.dic fr_CA.dic sw_TZ.dic

RESDIR = Resources
# RESOURCES = $(addprefix ${RESDIR}/, ${DICS})
RESOURCES = \
	$(addprefix ${RESDIR}/, ${DICS:.dic=_dict.cpp})

bin : $(BINDIR)/passphrase${EXEEXT}

OBJ = $(addprefix ${OBJDIR}/,$(SRC:.cpp=.o)) \
	$(addprefix ${OBJDIR}/,$(DICS:.dic=_dict.o))

$(BINDIR)/passphrase${EXEEXT} : $(OBJ) | $(BINDIR)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)


$(OBJDIR)/passphrase.o: CXXFLAGS += -DVERSION=\"${VERSION}\" -DDATE=\"${DATE}\"
$(OBJDIR)/passphrase.o: ${RESOURCES}

$(OBJDIR)/%.o : %.cpp | $(OBJDIR) ${DEPDIR}
	$(COMPILE.cpp) $< $(OUTPUT_OPTION)

$(OBJDIR)/%.o : ${RESDIR}/%.cpp | $(OBJDIR) ${DEPDIR}
	$(COMPILE.cpp) $< $(OUTPUT_OPTION)

$(OBJDIR)/%.o : $(RESDIR)/%.dic | $(OBJDIR)
	cd $(dir $<) ;\
	$(LD) -r -b binary $(notdir $<) -o ../$@

$(RESDIR)/%_dict.cpp : 

$(BINDIR) ${RESDIR} ${OBJDIR} ${DEPDIR} : ; mkdir -p $@

.PHONY: clean bin all install resources

clean :
	$(RM) -r bin obj ${DEPDIR} ${RESDIR}

resources : ${RESOURCES}

DICTDIRS = /usr/share/myspell /usr/share/hunspell
DICT_BIG_LIST = $(foreach d,${DICTDIRS},$(addprefix $d/, ${DICS}))
DICT_SRC = $(wildcard $(DICT_BIG_LIST))

${RESOURCES} : ${DICT_SRC} | ${RESDIR} process_dict.py
	@echo "Processing $(filter %$(notdir ${@:_dict.cpp=}).dic,${DICT_SRC}) --> $@"
	@python3 process_dict.py $(filter %$(notdir ${@:_dict.cpp=.dic}),$^) $@ 

# ${RESDIR}/%_dict.cpp : $(filter %.dic,$(DICT_SRC)) | ${RESDIR} process_dict.py
# 	python3 process_dict.py $< $@

# Check if we're making the clean target; only include
# dependencies if we AREN'T
ifneq ($(MAKECMDGOALS), clean)
# Include dependency information
include $(wildcard ${DEP_DIR}/*.d)
endif
