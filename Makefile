# Generate an architecture descriptor like x86_64-linux-gnu
ARCH := $(shell uname -mo | sed -e 's/\([^ ]*\) \([^/]*\)\/\?\(.*\)/\L\1-\3-\2/')

# Hack: Limit ARM targets to sequential compilation to avoid out-of-memory problems
ifneq "$(findstring arm,$(shell uname -m))" ""
NINJA_ARGS += -j 1
endif

BUILDTYPE = release
BUILDDIR = build/$(ARCH)/$(BUILDTYPE)

# LOCALINSTDIR The build dir used for local install
LOCALINSTDIR = build/$(ARCH)/localinstall-$(BUILDTYPE)
DOOCSARCH ?= unknown_doocsarch
LOCALINSTPRE = $(abspath ../../../${DOOCSARCH})
LOCALSECTION := $(notdir $(abspath ../..))/$(notdir $(abspath ..))

PREFIX ?= /export/doocs
DOOCS_PATHS = --libdir 'lib' --includedir 'lib/include'

JUNIT_XML_FILE ?= $(BUILDDIR)/test.xml

# Colorful toys
INTRO = "\033[1;34m------------"
OUTRO = "------------\033[0m"


help:
	@echo This makefile is a wrapper for meson/ninja.
	@echo You are encouraged to use meson and ninja directly to build the library.
	@echo
	@echo \'make release\' builds the release version of the library under build/$(ARCH)/release
	@echo \'make debug\' builds the debug version of the library under build/$(ARCH)/debug
	@echo \'make doocs-release\' builds a DOOCS release version that can be packaged with makeDdeb under build/$(ARCH)/doocs-release
	@echo
	@echo \'make test\' runs unit tests on the release version
	@echo \'make BUILDTYPE=debug test\' runs unit tests on the debug version
	@echo \'make test-junit\' runs unit tests and generates output in JUnit XML format under build/$(ARCH)/release/test.xml
	@echo \'make JUNIT_XML_FILE=a.xml test-junit\' runs unit tests and generates output in JUnit XML format in the specified file
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
	ninja $(NINJA_ARGS) -C $(BUILDDIR) clean

debug: build/$(ARCH)/debug/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	ninja $(NINJA_ARGS) -C build/$(ARCH)/debug

doc:	$(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	ninja $(NINJA_ARGS) -C $(BUILDDIR) resources/docs

doocs-release: build/$(ARCH)/doocs-release/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	ninja $(NINJA_ARGS) -C build/$(ARCH)/doocs-release

localinstall: $(LOCALINSTDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	ninja $(NINJA_ARGS) -C $(LOCALINSTDIR) install

mrproper:
	@echo $(INTRO) $@ $(OUTRO)
	rm -rf build

release: build/$(ARCH)/release/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	ninja $(NINJA_ARGS) -C build/$(ARCH)/release

test: $(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	ninja $(NINJA_ARGS) -C $(BUILDDIR) test

test-junit: $(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	ninja $(NINJA_ARGS) -C $(BUILDDIR) tests/libgul-test
	@$(BUILDDIR)/tests/libgul-test -r junit >$(JUNIT_XML_FILE)
	@echo Test results in JUnit format written to $(JUNIT_XML_FILE)

build/$(ARCH)/debug/build.ninja:
	@echo $(INTRO) $@ $(OUTRO)
	meson build/$(ARCH)/debug --buildtype=debug

build/$(ARCH)/doocs-release/build.ninja:
	@echo $(INTRO) $@ $(OUTRO)
	meson build/$(ARCH)/doocs-release --buildtype=release --prefix=/export/doocs \
	      --libdir=lib --includedir=lib/include -D deb-vers-ext=true \
              -D deb-name=doocs-@0@ -D deb-dev-name=dev-doocs-@0@

build/$(ARCH)/release/build.ninja:
	@echo $(INTRO) $@ $(OUTRO)
	meson build/$(ARCH)/release --buildtype=release

$(LOCALINSTDIR)/build.ninja:
	@echo $(INTRO) $@ $(OUTRO)
	meson --prefix ${LOCALINSTPRE} --bindir 'obj/${LOCALSECTION}' ${DOOCS_PATHS} \
              --buildtype=release ${LOCALINSTDIR}

.PHONY: build-tests debug clean doc doocs-release help install-doc libs localinstall mrproper release test
