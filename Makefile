DOOCSROOT = ../../..

include $(DOOCSROOT)/$(DOOCSARCH)/DEFINEDOOCSROOT
include ./LIBNO
include $(DOOCSROOT)/$(DOOCSARCH)/CONFIG

OBJDIR = $(DOOCSROOT)/$(DOOCSARCH)/obj/library/common/$(PKGDIR)
SRCDIR = $(DOOCSROOT)/library/common/$(PKGDIR)/source
INCDIR = $(DOOCSROOT)/library/common/$(PKGDIR)/include
DOCDIR = /web/tesla/doocs/doocs_libs/$(PKGDIR)

LIBRARYOBJ = 
#\
#	$(OBJDIR)/string_util.o

# All headers to be exported by the library, relative to $(INCDIR)
LIBRARYHFILES = \
	gul.h \
	gul/string_view.h

NEEDED_LIBS =

PKGCONFIG =
# \
#	$(OBJDIR)/gul.pc

ALLLIBS = \
	$(OBJDIR)/libgul.$(EXT) \
	$(OBJDIR)/libgul.a

CPPFLAGS := -I $(INCDIR) ${CPPFLAGS}

#CXXFLAGS += -std=c++14
#CPPFLAGS += -std=c++14

# Link against Clang's C++ lib instead of the system one
ifeq  '$(shell uname)' 'Darwin'
    CXXFLAGS += -stdlib=libc++
endif

# Colorful toys
INTRO = "\033[1;34m------------"
OUTRO = "------------\033[0m"


all:	$(ALLLIBS) $(PKGCONFIG)
	@echo Empty command for stubborn make versions. >/dev/null

$(OBJDIR)/.depend depend:
	@echo $(INTRO) $@ $(OUTRO)
	@if [ ! -f $(OBJDIR) ] ; then \
	    echo ---------- create dir $(OBJDIR) --------------; \
	    mkdir -p $(OBJDIR) ; \
	fi
#	for i in $(SRCDIR)/*.cc ;do \
#	    $(CCDEP) $$i ; \
#	done > $(OBJDIR)/.depend_temp
	cat $(OBJDIR)/.depend_temp | sed -e "/:/s/^/\$$\(OBJDIR\)\//g" > $(OBJDIR)/.depend
	chmod g+w $(OBJDIR)/.depend*

ifneq ($(MAKECMDGOALS),clean)
    include $(OBJDIR)/.depend
endif

$(OBJDIR)/libgul.a: $(LIBRARYOBJ)
	@echo $(INTRO) $@ $(OUTRO)
	$(LINK.a) $(LIBRARYOBJ)
	@-ranlib $(OBJDIR)/libgul.a
	@-chmod g+w $(OBJDIR)/libgul.a
	@echo "---------- $(OBJDIR)/libgul.a done---------------"

$(OBJDIR)/libgul.so.$(LIBNO): $(LIBRARYOBJ)
	@echo $(INTRO) $@ $(OUTRO)
	$(LINK.so) $(LIBRARYOBJ)
	@-chmod g+w $(OBJDIR)/libgul.s*.$(LIBNO)
	@rm -f $(OBJDIR)/libgul.so
	@ln -s $(OBJDIR)/libgul.so.$(LIBNO) $(OBJDIR)/libgul.so
	@echo "------ $(OBJDIR)/libgul.so.$(LIBNO) done---------------"

$(OBJDIR)/libgul.$(LIBNO).dylib: $(LIBRARYOBJ)
	@echo $(INTRO) $@ $(OUTRO)
	$(LINK.dylib) $(LIBRARYOBJ) -undefined suppress
	@-chmod g+w $(OBJDIR)/libgul.$(LIBNO).d*
	@rm -f $(OBJDIR)/libgul.dylib
	@ln -s $(OBJDIR)/libgul.$(LIBNO).dylib $(OBJDIR)/libgul.dylib
	@echo "------------ $(OBJDIR)/libgul.$(LIBNO).dylib done ---------------"
	@echo

# Insert the current release version, by extracting it from release tag
$(OBJDIR)/%.pc: $(SRCDIR)/../%.pc.in
	@echo $(INTRO) $@ $(OUTRO)
	@echo "GEN   $(subst $(OBJDIR)/,,$@)"
	@cat $< | sed -e "s/@VERSION@/$(shell git describe --tags | sed -e 's/[^_]*_//' -e 's/-.*//' -e 's/_/./g')/g" > $@

localinstall: $(ALLLIBS)
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
	-find $(DOOCSLIBS) -name '*'$(PKGDIR)'*' -print -delete

clean: rmlocalinstall
	rm -f $(LIBRARYOBJ) \
	      $(OBJDIR)/libgul.* $(OBJDIR)/.depend*
	rm -f $(PKGCONFIG)
	rm -rf $(OBJDIR)

doc:	doxygen
	@echo Done.

doxygen:
	-( cat Doxyfile | sed "s/PROJECT_NUMBER         =.*/PROJECT_NUMBER         = `cut -f2 -d= LIBNO`/" ) | doxygen -
	-chmod -R a+r $(DOCDIR) 2>/dev/null
	-chmod -R ug+w $(DOCDIR) 2>/dev/null
	-find $(DOCDIR) -type d -exec chmod a+x {} ';' 2>/dev/null

dirs:
	@echo $(INTRO) Create dir ${OBJDIR}/tests $(OUTRO)
	-mkdir -p ${OBJDIR}/tests

# TODO: Improve building of tests
TEST_OBJ = $(OBJDIR)/tests/test_main.o $(OBJDIR)/tests/test_backports.o $(SOURCEOBJ)

$(OBJDIR)/tests/test_main.o: $(SRCDIR)/../tests/test_main.cc
	@echo $(INTRO) $@ $(OUTRO)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o ${OBJDIR}/tests/test_main.o \
	    $(SRCDIR)/../tests/test_main.cc

$(OBJDIR)/tests/test_backports.o: $(SRCDIR)/../tests/test_backports.cc
	@echo $(INTRO) $@ $(OUTRO)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o ${OBJDIR}/tests/test_backports.o \
	    $(SRCDIR)/../tests/test_backports.cc

tests/test_main: dirs $(TEST_OBJ)
	@echo $(INTRO) $@ $(OUTRO)
	$(LINK.cc) $(TEST_OBJ) $(NEEDED_LIBS) -o tests/test_main

build-tests: tests/test_main
	@echo Done.

test: export LD_LIBRARY_PATH=$(DOOCSLIBS):$LD_LIBRARY_PATH
test: build-tests
	tests/test_main

.PHONY: build-tests dirs doc doxygen rmlocalinstall test