DOOCSROOT = ../../..
PKGDIR = gul

# Generate an architecture descriptor like x86_64-linux-gnu
ARCH := $(shell uname -mo | sed -e 's/\([^ ]*\) \([^/]*\)\/\?\(.*\)/\L\1-\3-\2/')

BUILDTYPE = release
BUILDDIR = build/$(ARCH)/$(BUILDTYPE)

# LOCALINSTDIR The build dir used for local install
LOCALINSTDIR = build/$(ARCH)/localinstall-$(BUILDTYPE)
DOOCSARCH ?= unknown_doocsarch
LOCALINSTPRE = $(abspath ../../../${DOOCSARCH})
LOCALSECTION := $(notdir $(abspath ../..))/$(notdir $(abspath ..))

PREFIX ?= /export/doocs
DOOCS_PATHS = --libdir 'lib' --includedir 'lib/include'

# Colorful toys
INTRO = "\033[1;34m------------"
OUTRO = "------------\033[0m"


help:
	@echo This makefile is a wrapper for meson/ninja.
	@echo You are encouraged to use meson and ninja directly to build the library.
	@echo
	@echo \'make release\' builds the release version of the library under build/$(ARCH)/release
	@echo \'make debug\' builds the debug version of the library under build/$(ARCH)/debug
	@echo
	@echo \'make test\' runs unit tests on the release version
	@echo \'make BUILDTYPE=debug test\' runs unit tests on the debug version
	@echo
	@echo \'make clean\' cleans up the build files of the release version
	@echo \'make BUILDTYPE=debug clean\' cleans up the build files of the debug version
	@echo \'make mrproper\' removes all build directories completely
	@echo
	@echo \'make doc\' builds documentation under build/$(ARCH)/release/resources/doxygenerated \(needs Doxygen\)
	@echo
	@echo \'make localinstall\' makes a local installation of the library under $(LOCALINSTPRE)/lib


# Needed for NetBeans IDE
build-tests:
	@echo $(INTRO) $@ $(OUTRO)

clean: $(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR) clean

debug: build/$(ARCH)/debug/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR)

doc:	$(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR) resources/docs

localinstall: $(LOCALINSTDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(LOCALINSTDIR) install

mrproper:
	@echo $(INTRO) $@ $(OUTRO)
	rm -rf build

release: build/$(ARCH)/release/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR)

test: $(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR) test


build/$(ARCH)/debug/build.ninja:
	@echo $(INTRO) $@ $(OUTRO)
	meson build/$(ARCH)/debug --buildtype=debug

build/$(ARCH)/release/build.ninja:
	@echo $(INTRO) $@ $(OUTRO)
	meson build/$(ARCH)/release --buildtype=release

$(LOCALINSTDIR)/build.ninja:
	@echo $(INTRO) $@ $(OUTRO)
	@meson --prefix ${LOCALINSTPRE} --bindir 'obj/${LOCALSECTION}' ${DOOCS_PATHS} \
               --buildtype=release ${LOCALINSTDIR}

.PHONY: build-tests debug clean doc install-doc libs localinstall mrproper release test
