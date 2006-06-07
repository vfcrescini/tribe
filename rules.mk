# This file is part of Tribe.
#
# Copyright (C) 2005, 2006 University of Western Sydney
# by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
#
# Tribe is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Tribe is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Tribe; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

define EXEC
  $(1)

endef

.PHONY : \
  copy-includes \
  copy-static-libs \
  copy-shared-libs \
  copy-bins \
  install-includes \
  install-static-libs \
  install-shared-libs \
  install-bins \
  uninstall-includes \
  uninstall-static-libs \
  uninstall-shared-libs \
  uninstall-bins \
  all \
  clean \
  distclean \
  $(NULL)

all : \
  copy-includes \
  copy-static-libs \
  copy-shared-libs \
  copy-bins \
  $(NULL)

install : \
  all \
  install-includes \
  install-static-libs \
  install-shared-libs \
  install-bins \
  $(NULL)

uninstall : \
  uninstall-includes \
  uninstall-static-libs \
  uninstall-shared-libs \
  uninstall-bins \
  $(NULL)

copy-includes : $(INCLUDES)
	mkdir -p $(DISTDIR)/include/tribe
	$(foreach i,$(INCLUDES),$(call EXEC,$(LN_S) -f ../../../$(CURDIR)/$(i) $(DISTDIR)/include/tribe/$(i)))

copy-static-libs : $(STATIC_LIBS)
	mkdir -p $(DISTDIR)/lib
	$(foreach i,$(STATIC_LIBS),$(call EXEC,$(LN_S) -f ../../$(CURDIR)/$(i) $(DISTDIR)/lib/$(i)))

ifdef BUILDSHARED
copy-shared-libs : $(SHARED_LIBS)
	mkdir -p $(DISTDIR)/lib
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(LN_S) -f ../../$(CURDIR)/$(i) $(DISTDIR)/lib/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(LN_S) -f $(i) $(DISTDIR)/lib/$(shell echo $(i) | cut -d. -f1,2,3)))
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(LN_S) -f $(i) $(DISTDIR)/lib/$(shell echo $(i) | cut -d. -f1,2)))
endif

copy-bins : $(BINS)
	mkdir -p $(DISTDIR)/bin
	$(foreach i,$(BINS),$(call EXEC,$(LN_S) -f ../../$(CURDIR)/$(i) $(DISTDIR)/bin/$(i)))

install-includes : $(INCLUDES)
	mkdir -p $(includedir)/tribe
	$(foreach i,$(INCLUDES),$(call EXEC,cp -f $(i) $(includedir)/tribe/$(i)))

install-static-libs : $(STATIC_LIBS)
	mkdir -p $(libdir)
	$(foreach i,$(STATIC_LIBS),$(call EXEC,cp -f $(i) $(libdir)/$(i)))

ifdef BUILDSHARED
install-shared-libs : $(SHARED_LIBS)
	mkdir -p $(libdir)
	$(foreach i,$(SHARED_LIBS),$(call EXEC,cp -f $(i) $(libdir)/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(LN_S) -f $(i) $(libdir)/$(shell echo $(i) | cut -d. -f1,2,3)))
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(LN_S) -f $(i) $(libdir)/$(shell echo $(i) | cut -d. -f1,2)))
endif

install-bins : $(BINS)
	mkdir -p $(bindir)
	$(foreach i,$(BINS),$(call EXEC,cp -f $(i) $(bindir)/$(i)))

uninstall-includes :
	$(foreach i,$(INCLUDES),$(call EXEC,$(RM) $(includedir)/tribe/$(i)))
	$(RM) -r $(includedir)/tribe

uninstall-static-libs :
	$(foreach i,$(STATIC_LIBS),$(call EXEC,$(RM) $(libdir)/$(i)))

ifdef BUILDSHARED
uninstall-shared-libs :
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(RM) $(libdir)/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(RM) $(libdir)/$(shell echo $(i) | cut -d. -f1,2,3)))
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(RM) $(libdir)/$(shell echo $(i) | cut -d. -f1,2)))
endif

uninstall-bins :
	$(foreach i,$(BINS),$(call EXEC,$(RM) $(bindir)/$(i)))

clean :
	$(foreach i,$(TEMPS1) $(OBJECTS),$(call EXEC,$(RM) $(i)))

distclean : clean
	$(foreach i,$(INCLUDES),$(call EXEC,$(RM) $(DISTDIR)/include/tribe/$(i)))
	$(foreach i,$(STATIC_LIBS),$(call EXEC,$(RM) $(DISTDIR)/lib/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(RM) $(DISTDIR)/lib/$(i)))
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(RM) $(DISTDIR)/lib/$(shell echo $(i) | cut -d. -f1,2,3)))
	$(foreach i,$(SHARED_LIBS),$(call EXEC,$(RM) $(DISTDIR)/lib/$(shell echo $(i) | cut -d. -f1,2)))
	$(foreach i,$(BINS),$(call EXEC,$(RM) $(DISTDIR)/bin/$(i)))
	$(RM) -r $(DISTDIR)/include/tribe
	$(RM) $(TEMPS2) $(STATIC_LIBS) $(SHARED_LIBS) $(BINS) Makefile
