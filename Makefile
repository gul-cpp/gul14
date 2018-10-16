DOOCSROOT = ../../..
PKGDIR = gul

include $(DOOCSROOT)/$(DOOCSARCH)/CONFIG

OBJDIR = $(DOOCSROOT)/$(DOOCSARCH)/obj/library/common/$(PKGDIR)
SRCDIR = $(DOOCSROOT)/library/common/$(PKGDIR)/src
INCDIR = $(DOOCSROOT)/library/common/$(PKGDIR)/include
DOCDIR = /web/tesla/doocs/doocs_libs/$(PKGDIR)

LIBRARYOBJ = \
	$(OBJDIR)/cat.o \
	$(OBJDIR)/join_split.o \
	$(OBJDIR)/string_util.o \
	$(OBJDIR)/time_util.o \
	$(OBJDIR)/tokenize.o \
	$(OBJDIR)/Trigger.o \
	$(OBJDIR)/trim.o

# All headers to be exported by the library, relative to $(INCDIR)
LIBRARYHFILES = \
	gul.h \
	gul/cat.h \
	gul/hexdump.h \
	gul/join_split.h \
	gul/string_util.h \
	gul/string_view.h \
	gul/time_util.h \
	gul/tokenize.h \
	gul/Trigger.h \
	gul/trim.h

# Colorful toys
INTRO = "\033[1;34m------------"
OUTRO = "------------\033[0m"


all:	build libs
	@echo Empty command for stubborn make versions. >/dev/null

build:
	@echo $(INTRO) $@ $(OUTRO)
	@if [ ! -d build ] ; then \
	    echo Create build/ dir ; \
	    mkdir -p build ; \
	fi
	@if [ ! -f build/build.ninja ] ; then \
	    echo Use Meson to create build configuration under build/ ; \
	    meson build ; \
	fi

libs:
	@echo $(INTRO) $@ $(OUTRO)
	@cd build; ninja

localinstall: libs
	@echo $(INTRO) $@ $(OUTRO)
	# Install header files
	@if [ ! -d $(DOOCSINC) ] ; then \
	    echo "- INSTALL: create dir $(DOOCSINC) " ; \
	    mkdir -p $(DOOCSINC) ; \
	fi
	@cd $(INCDIR) ; \
	echo "- Source include directory: $(INCDIR)" ; \
	for i in $(LIBRARYHFILES); do \
	    if [ -f $(DOOCSINC)/$$i ] ; then \
	        echo "- INSTALL: move $(DOOCSINC)/$$i to " ; \
	        echo "  $(DOOCSINC)/$$i.OLD" ; \
	        mv -f $(DOOCSINC)/$$i $(DOOCSINC)/$$i.OLD ; \
	    fi ; \
	    echo "- INSTALL: $$i in" ; \
	    echo "  $(DOOCSINC)/`basename $$i` " ; \
	    cp --parents $$i $(DOOCSINC) ;\
	done
	@chmod -f -R g+w $(DOOCSINC)
	# Install libraries in the main arch lib dir
	@if [ ! -d $(DOOCSLIBS) ] ; then \
	    echo "- INSTALL: create dir $(DOOCSLIBS) " ; \
	    mkdir -p $(DOOCSLIBS) ; \
	    chmod -f g+w $(DOOCSLIBS) ; \
	fi
	@for p in $(ALLLIBS); do \
	    echo "- INSTALL: $$p in \n  $(DOOCSLIBS)/`basename $$p` " ; \
	    cp $$p $(DOOCSLIBS) ; \
	    chmod -f g+w $(DOOCSLIBS)/`basename $$p` ; \
	    if [ "XSO" = X`echo $$p | awk '/\.so/ {print "SO"} '` ] ; then \
		SONAME=`echo $$p | sed -e "s/\.so.\{0,15\}/.so/g"` ; \
		echo "- INSTALL: ln -s $(DOOCSLIBS)/`basename $$p` $(DOOCSLIBS)/`basename $$SONAME`" ; \
		rm -f $(DOOCSLIBS)/`basename $$SONAME` ; \
		ln -s $(DOOCSLIBS)/`basename $$p` $(DOOCSLIBS)/`basename $$SONAME` ; \
		SAFILE=`echo $$p  | sed -e "s/\.so\./\.sa\./g"` ; \
		if [ -f $$SAFILE ] ; then \
		   echo "- INSTALL: $$SAFILE in \n  $(DOOCSLIBS)/`basename $$SAFILE` " ; \
		   cp $$SAFILE $(DOOCSLIBS) ; \
		   chmod -f g+w $(DOOCSLIBS)/`basename $$SAFILE` ; \
		fi ; \
	    elif [ "XDYLIB" = X`echo $$p | awk '/\.dylib/ {print "DYLIB"} '` ] ; then \
		DYLIBNAME=`echo $$p | sed -e "s/\.[0-9]\{1,2\}//g"` ; \
		echo "- INSTALL: ln -s $(DOOCSLIBS)/`basename $$p` $(DOOCSLIBS)/`basename $$DYLIBNAME`"  ; \
		rm -f $(DOOCSLIBS)/`basename $$DYLIBNAME` ; \
		ln -s $(DOOCSLIBS)/`basename $$p` $(DOOCSLIBS)/`basename $$DYLIBNAME` ; \
		SAFILE=`echo $$p  | sed -e "s/\.dylib\./\.sa\./g"` ; \
		if [ -f $$SAFILE ] ; then \
		    echo "- INSTALL: $$SAFILE in \n  $(DOOCSLIBS)/`basename $$SAFILE` " ; \
		    cp $$SAFILE $(DOOCSLIBS) ; \
		    chmod -f g+w $(DOOCSLIBS)/`basename $$SAFILE` ; \
	        fi ; \
	    fi ; \
	done

rmlocalinstall:
	@echo $(INTRO) $@ $(OUTRO)
	-find $(DOOCSLIBS) -name '*'$(PKGDIR)'*' -print -delete

clean:
	@echo $(INTRO) $@ $(OUTRO)
	@cd build; ninja clean

doc:	doxygen
	@echo $(INTRO) $@ $(OUTRO)
	@echo Done.

doxygen:
	@echo $(INTRO) $@ $(OUTRO)
	-( cat data/Doxyfile.in | sed "s/PROJECT_NUMBER         =.*/PROJECT_NUMBER         = `cut -f2 -d= LIBNO`/" ) | doxygen -
	-chmod -R a+r $(DOCDIR) 2>/dev/null
	-chmod -R ug+w $(DOCDIR) 2>/dev/null
	-find $(DOCDIR) -type d -exec chmod a+x {} ';' 2>/dev/null

test: build
	@echo $(INTRO) $@ $(OUTRO)
	@cd build; ninja test

# Needed for NetBeans IDE
build-tests:
	@echo $(INTRO) $@ $(OUTRO)

.PHONY: clean doc doxygen libs rmlocalinstall test
