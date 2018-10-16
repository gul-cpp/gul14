DOOCSROOT = ../../..
PKGDIR = gul
BUILDDIR = build.release

include $(DOOCSROOT)/$(DOOCSARCH)/CONFIG

OBJDIR = $(DOOCSROOT)/$(DOOCSARCH)/obj/library/common/$(PKGDIR)
SRCDIR = $(DOOCSROOT)/library/common/$(PKGDIR)/src
INCDIR = $(DOOCSROOT)/library/common/$(PKGDIR)/include
DOCDIR = /web/tesla/doocs/doocs_libs/$(PKGDIR)

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


libs:	$(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR)


# Needed for NetBeans IDE
build-tests:
	@echo $(INTRO) $@ $(OUTRO)

clean:
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR) clean

doc:	$(BUILDDIR)/build.ninja
	@echo $(INTRO) $@ $(OUTRO)
	@ninja -C $(BUILDDIR) resources/docs

install-doc: doc
	@echo $(INTRO) $@ $(OUTRO)
	-cp -r $(BUILDDIR)/resources/doxygenerated/* $(DOCDIR)
	-chmod -R a+r $(DOCDIR) 2>/dev/null
	-chmod -R ug+w $(DOCDIR) 2>/dev/null
	-find $(DOCDIR) -type d -exec chmod a+x {} ';' 2>/dev/null

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
