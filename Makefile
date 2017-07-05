include Makefile.cfg

ifndef BINLOC

.DEFAULT_GOAL:=default

$(builds): export BINLOC = $(binarydir)/$($@dir)/$(name)
$(builds): export FLAGS  = $($@flags)
$(builds): export objdir = $(objectdir)/$($@dir)
$(builds): export depdir = $(dependdir)/$($@dir)
$(builds): export bindir = $(binarydir)/$($@dir)
$(builds):
	@$(MAKE)

else

ifeq ($(OS), Windows_NT)
	FLAGS += -DWIN32

	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
#64bit
	else
		ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
#64bit
		endif
		ifeq ($(PROCESSOR_ARCHITECTURE),x86)
#32bit
		endif
	endif
else
	UNAME_S:=$(shell uname -s)
	UNAME_P:=$(shell uname -p)

	ifeq ($(UNAME_S),Linux)
#linux
	endif
	ifeq ($(UNAME_S),Darwin)
#OSX
	endif

	ifeq ($(UNAME_S),x86_64)
#64bit
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
#32bit
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
#arm
	endif
endif

.DEFAULT_GOAL:=$(BINLOC)

SOURCES:=$(shell find $(sourcedir) -name '*$(sourceextension)')
OBJECTS:=$(subst $(sourcedir),$(objdir), $(subst $(sourceextension),.o, $(SOURCES)))
DEPENDS:=$(subst $(sourcedir),$(depdir), $(subst $(sourceextension),.d, $(SOURCES)))
DIRS := $(sourcedir) $(headerdir) $(objectdir) $(objdir) $(dependdir) $(depdir) $(binarydir) $(bindir) $(librarydir)
LIBS:=$(patsubst %,-l%,$(libraries))
INCLUDES:=$(patsubst %,-I./%,$(includedirs))

.SECONDARY:$(OBJECTS)

# link everything together
%/$(name):$(OBJECTS)
	$(CC) $+ $(FLAGS) -o $@ $(LIBS) -L$(librarydir)

-include $(DEPENDS)

# generate directories
$(DIRS):
	@echo Making directory: $@
	@mkdir $@

# more complicated dependency computation, so all prereqs listed
# will also become command-less, prereq-less targets
#   sed:    strip the target (everything before colon)
#   sed:    remove any continuation backslashes
#   fmt -1: list words one per line
#   sed:    strip leading spaces
#   sed:    add trailing colons
$(objdir)/%.o:|$(DIRS)
	$(CC) -c -MMD -MP -MF $(depdir)/$*.d $(sourcedir)/$*$(sourceextension) $(FLAGS) $(INCLUDES) -o $@

endif

clean:
	-@$(RM) -rf $(objectdir) *.o *.exe *.stackdump $(dependdir) $(binarydir) $(name)

#file partially written based on information from:
#http://scottmcpeak.com/autodepend/autodepend.html

.PHONY:clean $(DEPENDS) Makefile Makefile.cfg $(builds)

