DOOCSROOT = ../../..
PKGDIR = gul
BUILDDIR = build.release

# LOCALINSTDIR The build dir used for local install
LOCALINSTDIR = local_build
DOOCSARCH ?= unknown_doocsarch
LOCALINSTPRE = $(abspath ../../../${DOOCSARCH})
LOCALSECTION := $(notdir $(abspath ../..))/$(notdir $(abspath ..))

PREFIX ?= /export/doocs
DOOCS_PATHS = --libdir 'lib' --includedir 'lib/include'

# Colorful toys
INTRO = "\033[1;34m------------"
OUTRO = "------------\033[0m"


all:
	@echo This makefile is a wrapper for meson/ninja.
	@echo You are encouraged to use meson and ninja directly to build the library.
	@echo
	@echo You can \'make lib\' to build the library under build.release
	@echo You can \'make BUILDDIR=build.debug lib\' to build a debug version of the library under build.debug
	@echo You can \'make BUILDDIR=mybuilddir test\' to run unit tests in the specified build directory
	@echo You can \'make BUILDDIR=mybuilddir doc\' to build documentation under mybuilddir/resources/doxygenerated (needs Doxygen)
	@echo You can \'make localinstall\' to make a local installation of the library under $(LOCALINSTPRE)/lib

# Needed for NetBeans IDE
build-tests:
	@echo $(INTRO) $@ $(OUTRO)

clean: $(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR) clean

doc:	$(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR) resources/docs

lib:	$(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR)

localinstall: $(LOCALINSTDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(LOCALINSTDIR) install

test: $(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR) test

$(BUILDDIR)/build.ninja:
	@if [ ! -f $(BUILDDIR)/build.ninja ] ; then \
	    echo Use Meson to create build configuration under $(BUILDDIR) ; \
	    if [ $(BUILDDIR) = build.release ] ; then \
		meson $(BUILDDIR) --buildtype=release ; \
	    elif [ $(BUILDDIR) = build.debug ] ; then \
		meson $(BUILDDIR) --buildtype=debug ; \
	    else \
	        meson $(BUILDDIR) ; \
	    fi ; \
	fi

$(LOCALINSTDIR)/build.ninja:
	@echo $(INTRO) $@ $(OUTRO)
	@meson --prefix ${LOCALINSTPRE} --bindir 'obj/${LOCALSECTION}' ${DOOCS_PATHS} \
               --buildtype=release ${LOCALINSTDIR}

.PHONY: build-tests clean doc install-doc libs localinstall test
