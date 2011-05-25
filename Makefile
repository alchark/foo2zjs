UNAME := $(shell uname)
MACH := $(shell uname -m | sed 's/i.86/x86_32/')
EUID := $(shell id -u)
SYSNAME := $(shell uname -n)

# No version number yet...
VERSION=0.0

# Installation prefix...
PREFIX=/usr/local
PREFIX=/usr

# Pathnames for this package...
BIN=$(PREFIX)/bin
SHAREZJS=$(PREFIX)/share/foo2zjs
SHAREOAK=$(PREFIX)/share/foo2oak
SHAREHP=$(PREFIX)/share/foo2hp
SHAREXQX=$(PREFIX)/share/foo2xqx
SHARELAVA=$(PREFIX)/share/foo2lava
SHAREQPDL=$(PREFIX)/share/foo2qpdl
MANDIR=$(PREFIX)/share/man
DOCDIR=$(PREFIX)/share/doc/foo2zjs/

# Pathnames for referenced packages...
FOODB=/usr/share/foomatic/db/source

# User ID's
LPuid=-olp
LPgid=-glp
ifeq ($(UNAME),Darwin)
    LPuid=-oroot
    LPgid=-gwheel
endif
ifeq ($(UNAME),FreeBSD)
    LPuid=-oroot
    LPgid=-gwheel
endif
# If we aren't root, don't try to set ownership
ifneq ($(EUID),0)
    LPuid=
    LPgid=
endif

# Definition of modtime()
MODTIME= date -d "1/1/1970 utc + `stat -t $$1 | cut -f14 -d' '` seconds" "+%a %b %d %T %Y"
ifeq ($(UNAME),FreeBSD)
    MODTIME= stat -f "%Sm" -t "%a %b %d %T %Y" $$1
endif
ifeq ($(UNAME),Darwin)
    MODTIME= stat -f "%Sm" -t "%a %b %d %T %Y" $$1
endif

#
# Files for tarball
#
NULL=
WEBFILES=	\
		foo2zjs.html.in \
		style.css \
		archzjs.fig \
		2300.png \
		2430.png \
		1020.png \
		foo2oak.html.in \
		archoak.fig \
		1500.gif \
		foo2hp.html.in \
		archhp.fig \
		2600.gif \
		foo2xqx.html.in \
		archxqx.fig \
		m1005.gif \
		foo2lava.html.in \
		archlava.fig \
		2530.gif \
		foo2qpdl.html.in \
		archqplp.fig \
		$(NULL)
	
FILES	=	\
		README \
		README.in \
		INSTALL \
		INSTALL.in \
		INSTALL.osx \
		INSTALL.usb \
		COPYING \
		ChangeLog \
		Makefile \
		foo2zjs.c \
		foo2zjs.1in \
		jbig.c \
		jbig.h \
		jbig_tab.c \
		zjsdecode.c \
		zjsdecode.1in \
		zjs.h \
		foo2hp.c \
		foo2hp.1in \
		foo2xqx.c \
		foo2xqx.1in \
		foo2lava.c \
		foo2lava.1in \
		foo2qpdl.c \
		foo2qpdl.1in \
		cups.h \
		xqx.h \
		xqxdecode.c \
		xqxdecode.1in \
		lavadecode.c \
		lavadecode.1in \
		qpdl.h \
		qpdldecode.c \
		qpdldecode.1in \
		opldecode.c \
		opldecode.1in \
		foo2zjs-wrapper.in \
		foo2zjs-wrapper.1in \
		foo2hp2600-wrapper.in \
		foo2hp2600-wrapper.1in \
		foo2xqx-wrapper.in \
		foo2xqx-wrapper.1in \
		foo2lava-wrapper.in \
		foo2lava-wrapper.1in \
		foo2qpdl-wrapper.in \
		foo2qpdl-wrapper.1in \
		gamma.ps \
		gamma-lookup.ps \
		align.ps \
		testpage.ps \
		foomatic-db/*/*.xml \
		foomatic-test \
		getweb.in \
		icc2ps/*.[ch] \
		icc2ps/Makefile \
		icc2ps/AUTHORS \
		icc2ps/COPYING \
		icc2ps/README \
		icc2ps/README.foo2zjs \
		PPD/*.ppd \
		crd/zjs/*.crd \
		crd/zjs/*.ps \
		crd/qpdl/*cms* \
		crd/qpdl/*.ps \
		arm2hpdl.c \
		usb_printerid.c \
		hplj1000 \
		hplj10xx.rules \
		msexpand \
		foo2oak.1in \
		foo2oak-wrapper.in \
		foo2oak-wrapper.1in \
		oakdecode.1in \
		okidecode.c \
		oki.h \
		c5200mono.prn \
		$(NULL)

FILESOAK= \
		foo2oak_x86_32.o \
		foo2oak_x86_64.o \
		oakdecode_x86_32.o \
		oakdecode_x86_64.o \
		$(NULL)

# hpclj2600n-0.icm km2430_0.icm km2430_1.icm km2430_2.icm samclp300-0.icm
# sihp1000.img sihp1005.img sihp1020.img sihp1018.img

# Programs and libraries
PROGS=		foo2zjs zjsdecode arm2hpdl foo2hp foo2xqx xqxdecode
PROGS+=		foo2lava lavadecode foo2qpdl qpdldecode opldecode
ifeq ($(SYSNAME),amd.rkkda.org)
    PROGS+=	okidecode
endif
PROGSOAK=	okidecode
SHELLS=		foo2zjs-wrapper foo2oak-wrapper foo2hp2600-wrapper \
		foo2xqx-wrapper foo2lava-wrapper foo2qpdl-wrapper
MANPAGES=	foo2zjs-wrapper.1 foo2zjs.1 zjsdecode.1
MANPAGES+=	foo2oak-wrapper.1 foo2oak.1 oakdecode.1
MANPAGES+=	foo2hp2600-wrapper.1 foo2hp.1
MANPAGES+=	foo2xqx-wrapper.1 foo2xqx.1 xqxdecode.1
MANPAGES+=	foo2lava-wrapper.1 foo2lava.1 lavadecode.1
MANPAGES+=	foo2qpdl-wrapper.1 foo2qpdl.1 qpdldecode.1
MANPAGES+=	opldecode.1
LIBJBG	=	jbig.o jbig_tab.o
BINPROGS=

ifeq ($(UNAME),Linux)
	BINPROGS += usb_printerid
#	ifeq ($(MACH),x86_32)
#	    PROGS += foo2oak oakdecode
#	endif
#	ifeq ($(MACH),x86_64)
#	    PROGS += foo2oak oakdecode
#	endif
endif

# Compiler flags
CFLAGS +=	-O2 -Wall
#CFLAGS +=	-g

#
# Rules to create test documents
#
GX=10200
GY=6600
GXR=1200
GYR=600
GSOPTS=	-q -dBATCH -dSAFER -dQUIET -dNOPAUSE -sPAPERSIZE=letter -r$(GXR)x$(GYR)
JBGOPTS=-m 16 -d 0 -p 92	# Equivalent options for pbmtojbg

.SUFFIXES: .ps .pbm .pgm .pgm2 .ppm .ppm2 .zjs .cmyk .pksm .zc .zm .jbg \
	   .cups .cupm .1 .1in .fig .gif .xqx .lava .qpdl

.fig.gif:
	fig2dev -L gif $*.fig | giftrans -t "#ffffff" -o $*.gif
	# fig2dev -L ppm  $*.fig | pnmquant -fs 256 \
	    | ppmtogif -transparent rgb:ff/ff/ff >$*.gif

.ps.cups:
	gs $(GSOPTS) -r600x600 \
	    -dcupsColorSpace=6 -dcupsBitsPerColor=2 -dcupsColorOrder=2 \
	    -sDEVICE=cups -sOutputFile=$*.cups $*.ps

.ps.cupm:
	gs $(GSOPTS) -r600x600 \
	    -dcupsColorSpace=3 -dcupsBitsPerColor=2 -dcupsColorOrder=2 \
	    -sDEVICE=cups -sOutputFile=$*.cupm $*.ps

.ps.pbm:
	gs $(GSOPTS) -sDEVICE=pbmraw -sOutputFile=$*.pbm $*.ps

.ps.ppm:
	gs $(GSOPTS) -sDEVICE=ppmraw -sOutputFile=$*.ppm $*.ps

.ps.pgm:
	gs $(GSOPTS) -sDEVICE=pgmraw -sOutputFile=- $*.ps \
	| pnmdepth 3 > $*.pgm

.ps.pgm2:
	gs.rick $(GSOPTS) -sDEVICE=pgmraw2 -sOutputFile=$*.pgm2 $*.ps

.ps.cmyk:
	gs $(GSOPTS) -sDEVICE=bitcmyk -sOutputFile=$*.cmyk $*.ps

.ps.pksm:
	gs $(GSOPTS) -sDEVICE=pksmraw -sOutputFile=$*.pksm $*.ps

.ps.zc:
	gs $(GSOPTS) -sDEVICE=bitcmyk -sOutputFile=- - < $*.ps \
	| ./foo2zjs -r1200x600 -g10200x6600 -p1 >$*.zc

.ps.zm:
	gs $(GSOPTS) -sDEVICE=pbmraw -sOutputFile=- - < $*.ps \
	| ./foo2zjs -r1200x600 -g10200x6600 -p1 >$*.zm

.pbm.zjs:
	./foo2zjs < $*.pbm > $*.zjs

.cmyk.zjs:
	./foo2zjs < $*.cmyk > $*.zjs

.pksm.zjs:
	./foo2zjs < $*.pksm > $*.zjs

.pbm.xqx:
	./foo2xqx < $*.pbm > $*.xqx

.pbm.qpdl:
	./foo2qpdl < $*.pbm > $*.qpdl

#
# The usual build rules
#
all:	all-test $(PROGS) $(BINPROGS) $(SHELLS) getweb all-icc2ps man doc

all-test:
	#
	# Dependencies...
	#
	@if ! type $(CC) >/dev/null 2>&1; then \
	    echo "      ***"; \
	    echo "      *** Error: $(CC) is not installed!"; \
	    echo "      ***"; \
	    echo "      *** Install Software Development (gcc) package"; \
	    echo "      ***"; \
	    exit 1; \
	fi
	@if ! test -f /usr/include/stdio.h; then \
	    echo "      ***"; \
	    echo "      *** Error: /usr/include/stdio.h is not installed!"; \
	    echo "      ***"; \
	    echo "      *** Install Software Development (gcc) package"; \
	    echo "      ***"; \
	    exit 1; \
	fi
	@if ! type gs >/dev/null 2>&1; then \
	    echo "      ***"; \
	    echo "      *** Error: gs is not installed!"; \
	    echo "      ***"; \
	    echo "      *** Install ghostscript (gs) package"; \
	    echo "      ***"; \
	    exit 1; \
	fi
	# ... OK!
	#

foo2zjs: foo2zjs.o $(LIBJBG)
	$(CC) $(CFLAGS) -o $@ foo2zjs.o $(LIBJBG)

foo2hp: foo2hp.o $(LIBJBG)
	# $(CC) $(CFLAGS) -o $@ foo2hp.o $(LIBJBG) /usr/local/lib/libdmalloc.a
	$(CC) $(CFLAGS) -o $@ foo2hp.o $(LIBJBG)

foo2xqx: foo2xqx.o $(LIBJBG)
	$(CC) $(CFLAGS) -o $@ foo2xqx.o $(LIBJBG)

foo2lava: foo2lava.o $(LIBJBG)
	$(CC) $(CFLAGS) -o $@ foo2lava.o $(LIBJBG)

foo2qpdl: foo2qpdl.o $(LIBJBG)
	$(CC) $(CFLAGS) -o $@ foo2qpdl.o $(LIBJBG)

zjsdecode: zjsdecode.o $(LIBJBG)
	$(CC) $(CFLAGS) zjsdecode.o $(LIBJBG) -o $@

foo2zjs-wrapper: foo2zjs-wrapper.in Makefile
	[ ! -f $@ ] || chmod +w $@
	sed < $@.in > $@ \
	    -e 's@^PREFIX=.*@PREFIX=$(PREFIX)@' || (rm -f $@ && exit 1)
	chmod 555 $@

foo2hp2600-wrapper: foo2hp2600-wrapper.in Makefile
	[ ! -f $@ ] || chmod +w $@
	sed < $@.in > $@ \
	    -e 's@^PREFIX=.*@PREFIX=$(PREFIX)@' || (rm -f $@ && exit 1)
	chmod 555 $@

foo2xqx-wrapper: foo2xqx-wrapper.in Makefile
	[ ! -f $@ ] || chmod +w $@
	sed < $@.in > $@ \
	    -e 's@^PREFIX=.*@PREFIX=$(PREFIX)@' || (rm -f $@ && exit 1)
	chmod 555 $@

foo2lava-wrapper: foo2lava-wrapper.in Makefile
	[ ! -f $@ ] || chmod +w $@
	sed < $@.in > $@ \
	    -e 's@^PREFIX=.*@PREFIX=$(PREFIX)@' || (rm -f $@ && exit 1)
	chmod 555 $@

foo2qpdl-wrapper: foo2qpdl-wrapper.in Makefile
	[ ! -f $@ ] || chmod +w $@
	sed < $@.in > $@ \
	    -e 's@^PREFIX=.*@PREFIX=$(PREFIX)@' || (rm -f $@ && exit 1)
	chmod 555 $@

getweb: getweb.in Makefile
	[ ! -f $@ ] || chmod +w $@
	sed < $@.in > $@ \
	    -e "s@\$${URLZJS}@$(URLZJS)@" \
	    -e 's@^PREFIX=.*@PREFIX=$(PREFIX)@' || (rm -f $@ && exit 1)
	chmod 555 $@

all-icc2ps:
	cd icc2ps; $(MAKE) all

okidecode: okidecode.o $(LIBJBG)
	$(CC) $(CFLAGS) okidecode.o $(LIBJBG) -o $@

ok: ok.o $(LIBJBG)
	$(CC) $(CFLAGS) ok.o $(LIBJBG) -o $@

splcdecode: splcdecode.o $(LIBJBG)
	$(CC) $(CFLAGS) splcdecode.o $(LIBJBG) -lz -o $@

xqxdecode: xqxdecode.o $(LIBJBG)
	$(CC) $(CFLAGS) xqxdecode.o $(LIBJBG) -o $@

lavadecode: lavadecode.o $(LIBJBG)
	$(CC) $(CFLAGS) lavadecode.o $(LIBJBG) -o $@

qpdldecode: qpdldecode.o $(LIBJBG)
	$(CC) $(CFLAGS) qpdldecode.o $(LIBJBG) -o $@

opldecode: opldecode.o $(LIBJBG)
	$(CC) $(CFLAGS) -g opldecode.o $(LIBJBG) -o $@

#
# Installation rules
#
install: all install-test install-prog install-icc2ps install-extra \
	    install-crd install-foo install-ppd install-man install-doc
	#
	# If you use CUPS, then restart the spooler:
	#	make cups
	#
	# Now use your printer configuration GUI to create a new printer.
	#
	# On Redhat 7.2/7.3/8.0/9.0 and Fedora Core 1-5, run "printconf-gui".
	# On Fedora Core 6 and Fedora 7, run "system-config-printer".
	# On Mandrake, run "printerdrake"
	# On Suse 9.0, run "yast"
	# On Ubuntu 5.10/6.06/6.10/7.04, run "gnome-cups-manager"

install-test:
	#
	# Installation Dependencies...
	#
	@if ! type foomatic-rip >/dev/null 2>&1; then \
	    echo "      ***"; \
	    echo "      *** Error: foomatic-rip is not installed!"; \
	    echo "      ***"; \
	    echo "      *** Install foomatic package(s) for your OS"; \
	    echo "      ***"; \
	    exit 1; \
	fi
	# ... OK!
	#
    

install-prog:
	#
	# Install driver, wrapper, and development tools
	#
	install -c $(PROGS) $(SHELLS) $(BIN)/
	if [ "$(BINPROGS)" != "" ]; then \
	    install -c $(BINPROGS) /bin/; \
	fi
	#
	# Install gamma correction files.  These are just templates,
	# and don't actually do anything right now.  If anybody wants
	# to tune them or point me at a process for doing that, please...
	#
	install -d $(SHAREZJS)/
	install -c -m 644 gamma.ps $(SHAREZJS)/
	install -c -m 644 gamma-lookup.ps $(SHAREZJS)/
	install -d $(SHAREOAK)/
	install -d $(SHAREHP)/
	install -d $(SHAREXQX)/
	install -d $(SHARELAVA)/

install-foo:
	#
	# Remove obsolete foomatic database files from previous versions
	#
	rm -f $(FOODB)/opt/foo2zjs-Media.xml
	rm -f $(FOODB)/opt/foo2zjs-PaperSize.xml
	rm -f $(FOODB)/opt/foo2zjs-Source.xml
	rm -f $(FOODB)/opt/foo2zjs-DitherPPI.xml
	#
	# Install current database files
	#
	@if [ -d $(FOODB) ]; then \
	    for dir in driver printer opt; do \
		echo install -m 644 foomatic-db/$$dir/*.xml $(FOODB)/$$dir/; \
		install -c -m 644 foomatic-db/$$dir/*.xml $(FOODB)/$$dir/; \
	    done \
	else \
	    echo "***"; \
	    echo "*** WARNING! You don't have directory $(FOODB)/"; \
	    echo "*** If you want support for foomatic printer configuration,";\
	    echo "*** then you will have to manually install these files..."; \
	    echo "***"; \
	    ls foomatic-db/*/*.xml | sed 's/^/	/'; \
	    echo "***"; \
	    echo "*** ... wherever foomatic is stashed on your machine."; \
	    echo "***"; \
	fi
	#
	# Clear foomatic cache and rebuild database if needed
	#
	rm -rf /var/cache/foomatic/*/*
	rm -f /var/cache/foomatic/printconf.pickle
	if [ -d /var/cache/foomatic/compiled ]; then \
	    cd /var/cache/foomatic/compiled; \
	    foomatic-combo-xml -O >overview.xml; \
	fi

install-icc2ps:
	#
	# Install ICM to Postscript file conversion utility
	#
	cd icc2ps; $(MAKE) PREFIX=$(PREFIX) install

install-crd:
	#
	# Install prebuilt CRD files (from m2300w project)
	#
	install -d $(SHAREZJS)/
	install $(LPuid) $(LPgid) -m 775 -d $(SHAREZJS)/crd/
	for i in crd/zjs/*.*; do \
	    install -c -m 644 $$i $(SHAREZJS)/crd/; \
	done
	#
	# Install prebuilt CRD files for CLP-300/CLP-600
	#
	install -d $(SHAREQPDL)/
	install $(LPuid) $(LPgid) -m 775 -d $(SHAREQPDL)/crd/
	for i in crd/qpdl/*cms* crd/qpdl/*.ps; do \
	    install -c -m 644 $$i $(SHAREQPDL)/crd/; \
	done

install-psfiles:
	#
	# Install prebuilt psfiles files (from m2300w project)
	#
	install -d $(SHAREHP)/
	install $(LPuid) $(LPgid) -m 775 -d $(SHAREHP)/psfiles/
	for i in psfiles/*.*; do \
	    install -c -m 644 $$i $(SHAREHP)/psfiles/; \
	done

install-extra:
	#
	# Install extra files (ICM and firmware), if any exist here.
	#
	# Get files from the printer manufacturer, i.e. www.minolta-qms.com,
	# or use the "./getweb" convenience script.
	#
	install -d $(SHAREZJS)/
	# foo2zjs ICM files (if any)
	install $(LPuid) $(LPgid) -m 775 -d $(SHAREZJS)/icm/
	for i in DL*.icm CP*.icm km2430*.icm; do \
	    if [ -f $$i ]; then \
		install -c -m 644 $$i $(SHAREZJS)/icm/; \
	    fi; \
	done
	# foo2zjs Firmware files (if any)
	install $(LPuid) $(LPgid) -m 775 -d $(SHAREZJS)/firmware/
	for i in sihp*.img; do \
	    if [ -f $$i ]; then \
		base=`basename $$i .img`; \
		./arm2hpdl $$i >$$base.dl; \
		install -c -m 644 $$base.dl $(SHAREZJS)/firmware/; \
	    fi; \
	done
	# foo2oak ICM files (if any)
	install $(LPuid) $(LPgid) -m 775 -d $(SHAREOAK)/icm/
	# foo2hp ICM files (if any)
	install $(LPuid) $(LPgid) -m 775 -d $(SHAREHP)/icm/
	for i in hpclj26*.icm km2430*.icm; do \
	    if [ -f $$i ]; then \
		install -c -m 644 $$i $(SHAREHP)/icm/; \
	    fi; \
	done
	# foo2lava ICM files (if any)
	install $(LPuid) $(LPgid) -m 775 -d $(SHARELAVA)/icm/
	for i in km2530*.icm; do \
	    if [ -f $$i ]; then \
		install -c -m 644 $$i $(SHARELAVA)/icm/; \
	    fi; \
	done
	# foo2qpdl ICM files (if any)
	install $(LPuid) $(LPgid) -m 775 -d $(SHAREQPDL)/icm/
	for i in samclp300*.icm; do \
	    if [ -f $$i ]; then \
		install -c -m 644 $$i $(SHAREQPDL)/icm/; \
	    fi; \
	done

MODEL=/usr/share/cups/model
LOCALMODEL=/usr/local/share/cups/model
PPD=/usr/share/ppd
install-ppd:
	#
	# Install PPD files for CUPS
	#
	if [ -d $(PPD) ]; then \
	    find $(PPD) -name '*foo2zjs*' | xargs rm -f; \
	    find $(PPD) -name '*foo2hp*' | xargs rm -f; \
	    find $(PPD) -name '*foo2xqx*' | xargs rm -f; \
	    find $(PPD) -name '*foo2lava*' | xargs rm -f; \
	    find $(PPD) -name '*foo2qpdl*' | xargs rm -f; \
            [ -d $(PPD)/foo2zjs ] || mkdir $(PPD)/foo2zjs; \
	    cd PPD; \
	    for ppd in *.ppd; do \
		gzip < $$ppd > $(PPD)/foo2zjs/$$ppd.gz; \
	    done; \
	fi
	if [ -d $(MODEL) ]; then \
	    cd PPD; \
	    for ppd in *.ppd; do \
		gzip < $$ppd > $(MODEL)/$$ppd.gz; \
	    done; \
	elif [ -d $(LOCALMODEL) ]; then \
	    cd PPD; \
	    for ppd in *.ppd; do \
		gzip < $$ppd > $(LOCALMODEL)/$$ppd.gz; \
	    done; \
	fi

USBDIR=/etc/hotplug/usb
UDEVDIR=/etc/udev/rules.d
RULES=hplj10xx.rules
install-hotplug: install-hotplug-test install-hotplug-prog

install-hotplug-test:
	#
	# Hotplug Installation Dependencies...
	#
	@if ! type ex >/dev/null 2>&1; then \
	    echo "      ***"; \
	    echo "      *** Error: "ex" is not installed!"; \
	    echo "      ***"; \
	    echo "      *** Install "vim" package(s) for your OS"; \
	    echo "      ***"; \
	    exit 1; \
	fi
	# ... OK!
	#

install-hotplug-prog:
	if [ -d $(UDEVDIR) ]; then \
	    install -c -m 644 $(RULES) $(UDEVDIR)/11-$(RULES); \
	fi
	[ -d $(USBDIR) ] || install -d -m 755 $(USBDIR)/
	install -c -m 755 hplj1000 $(USBDIR)/
	ln -sf $(USBDIR)/hplj1000 $(USBDIR)/hplj1005
	ln -sf $(USBDIR)/hplj1000 $(USBDIR)/hplj1018
	ln -sf $(USBDIR)/hplj1000 $(USBDIR)/hplj1020
	$(USBDIR)/hplj1000 install-usermap
	$(USBDIR)/hplj1005 install-usermap
	$(USBDIR)/hplj1018 install-usermap
	$(USBDIR)/hplj1020 install-usermap

cups:	FRC
	if [ -x /etc/init.d/cups ]; then \
	    /etc/init.d/cups restart; \
	elif [ -x /etc/rc.d/rc.cups ]; then \
	    /etc/rc.d/rc.cups restart; \
	elif [ -x /etc/init.d/cupsys ]; then \
	    /etc/init.d/cupsys restart; \
	elif [ -x /etc/init.d/cupsd ]; then \
	    /etc/init.d/cupsd restart; \
	elif [ -x /usr/local/etc/rc.d/cups.sh ]; then \
	    /usr/local/etc/rc.d/cups.sh restart; \
	elif [ -x /usr/local/etc/rc.d/cups.sh.sample ]; then \
	    cp /usr/local/etc/rc.d/cups.sh.sample /usr/local/etc/rc.d/cups.sh; \
	    /usr/local/etc/rc.d/cups.sh restart; \
	fi

#
# Uninstall
#
uninstall:
	-rm -f /etc/hotplug/usb/hplj1000
	-rm -f /etc/hotplug/usb/hplj1005
	-rm -f /etc/hotplug/usb/hplj1018
	-rm -f /etc/hotplug/usb/hplj1020
	-rm -f /etc/hotplug/usb/foo2zjs.usermap
	-(echo "g/^hplj10[02][05]/d"; echo "w") | ex /etc/hotplug/usb.usermap
	-rm -f /etc/udev/rules.d/11-hplj10xx.rules
	-rm -f /usr/bin/usb_printerid /bin/usb_printerid /sbin/usb_printerid
	-rm -f /etc/hotplug/usb/hplj.usermap #
	-rm -f /etc/udev/rules.d/58-foo2zjs.rules #
	-rm -f /sbin/foo2zjs-loadfw #
	-rm -rf /usr/share/doc/foo2zjs/
	-rm -rf /usr/share/foo2zjs/
	-rm -rf /usr/share/foo2hp/
	-rm -rf /usr/share/foo2oak/
	-rm -rf /usr/share/foo2xqx/
	-rm -rf /usr/share/foo2lava/
	-rm -rf /usr/share/foo2qpdl/
	-rm -f /usr/bin/arm2hpdl
	-rm -f /usr/bin/foo2zjs-wrapper /usr/bin/foo2zjs /usr/bin/zjsdecode
	-rm -f /usr/bin/foo2oak-wrapper /usr/bin/foo2oak /usr/bin/oakdecode
	-rm -f /usr/bin/foo2hp2600-wrapper /usr/bin/foo2hp
	-rm -f /usr/bin/foo2xqx-wrapper /usr/bin/foo2xqx /usr/bin/xqxdecode
	-rm -f /usr/bin/foo2lava-wrapper /usr/bin/foo2lava /usr/bin/lavadecode
	-rm -f /usr/bin/foo2qpdl-wrapper /usr/bin/foo2qpdl /usr/bin/qpdldecode
	-rm -f /usr/bin/opldecode
	-rm -f /usr/bin/foo2zjs-icc2ps
	-cd foomatic-db; for i in `find driver opt printer -name "*.xml"`; do \
		rm -f $(FOODB)/$$i; \
	done
	cd PPD; for ppd in *.ppd; do \
	    rm -f $(MODEL)/$$ppd.gz; \
	done;
	-rm -f /var/cache/foomatic/printconf.pickle

#
# Clean
#
clean:
	-rm -f $(PROGS) $(BINPROGS) $(SHELLS)
	-rm -f *.zc *.zm
	-rm -f xxx.* xxxomatic
	-rm -f foo2zjs.o jbig.o jbig_tab.o zjsdecode.o foo2hp.o
	-rm -f foo2xqx.o xqxdecode.o
	-rm -f foo2lava.o lavadecode.o
	-rm -f foo2qpdl.o qpdldecode.o
	-rm -f opldecode.o
	-rm -f foo2oak.html foo2zjs.html foo2hp.html foo2xqx.html foo2lava.html
	-rm -f foo2qpdl.html
	-rm -f index.html
	-rm -f arch*.gif
	-rm -f sihp*.dl
	-rm -f *.tar.gz
	-rm -f getweb
	-rm -f patch.db
	-rm -f $(MANPAGES) manual.pdf
	-rm -f *.zjs *.zm *.zc *.zc? *.zc?? *.oak *.pbm *.pksm *.cmyk
	-rm -f pksm2bitcmyk
	-rm -f *.icm.*.ps
	-rm -f okidecode.o
	cd icc2ps; $(MAKE) $@

#
# Header dependencies
#
zjsdecode.o: zjs.h jbig.h
foo2zjs.o: zjs.h jbig.h
jbig.o: jbig.h
foo2hp.o: zjs.h jbig.h cups.h
xqxdecode.o: xqx.h jbig.h
foo2xqx.o: xqx.h jbig.h
lavadecode.o: jbig.h
foo2lava.o: jbig.h
qpdldecode.o: jbig.h
foo2qpdl.o: jbig.h qpdl.h
opldecode.o: jbig.h

#
# foo2* Regression tests
#
test:		testzjs testhp
	#
	# All regression tests passed.
	#
	# Send the appropriate test page .zm/.zc ZjStream file(s) to
	# your printer using a *RAW* printer queue.

#
# foo2zjs Regression tests
#
testzjs:	testpage.zm \
		testpage.zc10 testpage.zc1 testpage.zc2 testpage.zc3 \
		lj1000.zm lj1020.zm

testpage.zm: testpage.ps foo2zjs-wrapper foo2zjs Makefile FRC
	#
	# Tests will pass only if you are using ghostscript-7.05-24.7
	# or ghostscript-8.15.2-1.1
	#
	# Monochrome test page for Minolta 2200/2300 DL
	PATH=.:$$PATH time -p foo2zjs-wrapper -b gs testpage.ps > $@
	@want1="c4cf1940d6fb854cc3efdd6283388ea4  $@"; \
	want2="0900df1fe16dd6bb96958bcb5e8e2550  $@"; \
	got=`md5sum $@`; [ "$$want1" = "$$got" -o "$$want2" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

testpage.zc10: testpage.ps foo2zjs-wrapper foo2zjs Makefile FRC
	#
	# Color test page for Minolta 2200/2300 DL
	PATH=.:$$PATH time -p foo2zjs-wrapper -b gs -c -C10 testpage.ps > $@
	@want1="3d3b6fc08d9a1c9f80a99fec867596df  $@"; \
	want2="12991b79ed11b18a639d6fd72e92477b  $@"; \
	got=`md5sum $@`; [ "$$want1" = "$$got" -o "$$want2" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

testpage.zc1: testpage.ps foo2zjs-wrapper foo2zjs Makefile FRC
	PATH=.:$$PATH time -p foo2zjs-wrapper -c -C1 testpage.ps > $@
	@want="3d3b6fc08d9a1c9f80a99fec867596df  $@"; got=`md5sum $@`; \
	#[ "$$want" = "$$got" ] || \
	#    { echo "*** Test failure, got $$got"; exit 1; }

testpage.zc2: testpage.ps foo2zjs-wrapper foo2zjs Makefile FRC
	PATH=.:$$PATH time -p foo2zjs-wrapper -c -C2 testpage.ps > $@
	@want="3d3b6fc08d9a1c9f80a99fec867596df  $@"; got=`md5sum $@`; \
	#[ "$$want" = "$$got" ] || \
	#    { echo "*** Test failure, got $$got"; exit 1; }

testpage.zc3: testpage.ps foo2zjs-wrapper foo2zjs Makefile FRC
	PATH=.:$$PATH time -p foo2zjs-wrapper -c -C3 testpage.ps > $@
	@want="3d3b6fc08d9a1c9f80a99fec867596df  $@"; got=`md5sum $@`; \
	#[ "$$want" = "$$got" ] || \

lj1000.zm: testpage.ps foo2zjs-wrapper foo2zjs Makefile FRC
	#
	# Monochrome test page for HP LJ1000
	PATH=.:$$PATH time -p foo2zjs-wrapper -b gs -r600x600 -P testpage.ps >$@
	@want1="e458bacac57331ec5206cacd0181fe8a  $@"; \
	want2="ee7ff564b881ce9ee44e47f1dbede560  $@"; \
	got=`md5sum $@`; [ "$$want1" = "$$got" -o "$$want2" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

lj1020.zm: testpage.ps foo2zjs-wrapper foo2zjs Makefile FRC
	#
	# Monochrome test page for HP LJ1020
	PATH=.:$$PATH time -p foo2zjs-wrapper -b gs -r600x600 -P -z1 \
	    testpage.ps >$@
	@want1="391f91cd50781f733cfa7df44272da09  $@"; \
	want2="1a33dfc460a5725bb7dbc34a7342b092  $@"; \
	got=`md5sum $@`; [ "$$want1" = "$$got" -o "$$want2" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

#
# foo2hp Regression tests
#
testhp: lj2600.zm1 lj2600.zc1

lj2600.zm1: testpage.ps foo2hp2600-wrapper foo2hp Makefile FRC
	#
	# Monochrome test page for HP 2600n (1-bit)
	PATH=.:$$PATH time -p foo2hp2600-wrapper testpage.ps > $@
	@want1="11888137a72c4fd0ab1f791d1600fba0  $@"; \
	want2="a768fc2854434276dd8a274896af5d4f  $@"; \
	got=`md5sum $@`; [ "$$want1" = "$$got" -o "$$want2" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

lj2600.zc1: testpage.ps foo2hp2600-wrapper foo2hp Makefile FRC
	#
	# Color test page for HP 2600n (1-bit)
	PATH=.:$$PATH time -p foo2hp2600-wrapper -c testpage.ps > $@
	@want1="a20f0d33017200d64ce9aec9fc0111af  $@"; \
	want2="88e77799c6e10b15586e9584afa50c09  $@"; \
	got=`md5sum $@`; [ "$$want1" = "$$got" -o "$$want2" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

#
# foo2oak Regression tests
#
testoak: pprtest-0.oak pprtest-1.oak pprtest-2.oak pprtest-3.oak

pprtest-0.oak: FRC
	#
	# 1-bit Monochrome test page for OAKT
	PATH=.:$$PATH foo2oak-wrapper -b1 -D12345678 pprtest.ps > $@
	@want="fbd4c1a560985a6ad47ff23b018c7ce8  $@"; got=`md5sum $@`; \
	[ "$$want" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

pprtest-1.oak: FRC
	#
	# 2-bit Monochrome test page for OAKT
	PATH=.:$$PATH foo2oak-wrapper -b2 -D12345678 pprtest.ps > $@
	@want="bec9a24ee1ce0d388b773f83609a4d01  $@"; got=`md5sum $@`; \
	[ "$$want" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

pprtest-2.oak: FRC
	#
	# 1-bit color test page for OAKT
	PATH=.:$$PATH foo2oak-wrapper -c -b1 -D12345678 pprtest.ps > $@
	@want="c714bcd69fe5f3b2b257d7435eb938d1  $@"; got=`md5sum $@`; \
	[ "$$want" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

pprtest-3.oak: FRC
	#
	# 2-bit color test page for OAKT
	PATH=.:$$PATH foo2oak-wrapper -c -b2 -D12345678 pprtest.ps > $@
	@want="ed89abcd873979bc9337e02263511964  $@"; got=`md5sum $@`; \
	[ "$$want" = "$$got" ] || \
	    { echo "*** Test failure, got $$got"; exit 1; }

#
#	icc2ps regression tests
#
ICC2PS=./icc2ps/foo2zjs-icc2ps
icctest:
	for g in *.icm; do \
	    for i in 0 1 2 3; do \
		$(ICC2PS) -o $$g -t$$i \
		| sed '/Created:/d' > $$g.$$i.ps; \
	    done; \
	done


#
# Make phony print devices for testing full spooler interface without printing
#
tmpdev:
	DEV=/tmp/OAK; > $$DEV; chmod 666 $$DEV
	DEV=/tmp/OAKCM; > $$DEV; chmod 666 $$DEV
	DEV=/tmp/testfile; > $$DEV; chmod 666 $$DEV

#
# Test files for debugging
#
testpage.pbm: testpage.ps Makefile
xxx.zc: FRC
xxx.zm: FRC

#
#	PPD files
#
ppd:
	# Did you do a "make install"????
	./getweb ppd

#
# Manpage generation.  No, I am not interested in "info" files or
# HTML documentation.
#
man: $(MANPAGES)

.1in.1:
	-rm -f $*.1
	modtime() { $(MODTIME); }; \
	MODpage=`modtime $*.1in`; \
	MODver=$(VERSION); \
	sed < $*.1in > $*.1 \
	    -e "s@\$${URLOAK}@$(URLOAK)@" \
	    -e "s@\$${URLZJS}@$(URLZJS)@" \
	    -e "s@\$${URLHP}@$(URLHP)@" \
	    -e "s@\$${URLXQX}@$(URLXQX)@" \
	    -e "s@\$${URLLAVA}@$(URLLAVA)@" \
	    -e "s@\$${URLQPDL}@$(URLQPDL)@" \
	    -e "s/\$${MODpage}/$$MODpage/" \
	    -e "s/\$${MODver}/$$MODver/"
	chmod -w $*.1

install-man: man
	#
	# Install manual pages
	#
	install -d -m 755 $(MANDIR)
	install -d -m 755 $(MANDIR)/man1/
	install -c -m 644 foo2zjs.1 $(MANDIR)/man1/
	install -c -m 644 foo2zjs-wrapper.1 $(MANDIR)/man1/
	install -c -m 644 zjsdecode.1 $(MANDIR)/man1/
	install -c -m 644 foo2oak.1 $(MANDIR)/man1/
	install -c -m 644 foo2oak-wrapper.1 $(MANDIR)/man1/
	install -c -m 644 oakdecode.1 $(MANDIR)/man1/
	install -c -m 644 foo2hp.1 $(MANDIR)/man1/
	install -c -m 644 foo2hp2600-wrapper.1 $(MANDIR)/man1/
	install -c -m 644 xqxdecode.1 $(MANDIR)/man1/
	install -c -m 644 foo2xqx.1 $(MANDIR)/man1/
	install -c -m 644 foo2xqx-wrapper.1 $(MANDIR)/man1/
	install -c -m 644 lavadecode.1 $(MANDIR)/man1/
	install -c -m 644 foo2lava.1 $(MANDIR)/man1/
	install -c -m 644 foo2lava-wrapper.1 $(MANDIR)/man1/
	install -c -m 644 qpdldecode.1 $(MANDIR)/man1/
	install -c -m 644 foo2qpdl.1 $(MANDIR)/man1/
	install -c -m 644 foo2qpdl-wrapper.1 $(MANDIR)/man1/
	install -c -m 644 opldecode.1 $(MANDIR)/man1/

doc: README INSTALL manual.pdf

install-doc: doc
	#
	# Install documentation
	#
	install -d -m 755 $(DOCDIR)
	install -c -m 644 manual.pdf $(DOCDIR)
	install -c -m 644 COPYING $(DOCDIR)
	install -c -m 644 INSTALL $(DOCDIR)
	install -c -m 644 INSTALL.osx $(DOCDIR)
	install -c -m 644 README $(DOCDIR)
	install -c -m 644 ChangeLog $(DOCDIR)

GROFF=/usr/local/test/bin/groff
GROFF=groff
manual.pdf: $(MANPAGES)
	-$(GROFF) -t -man $(MANPAGES) | ps2pdf - $@

README: README.in
	rm -f $@
	sed < $@.in > $@ \
	    -e "s@\$${URLOAK}@$(URLOAK)@" \
	    -e "s@\$${URLZJS}@$(URLZJS)@"
	chmod -w $@

INSTALL: INSTALL.in
	rm -f $@
	sed < $@.in > $@ \
	    -e "s@\$${URLOAK}@$(URLOAK)@" \
	    -e "s@\$${URLZJS}@$(URLZJS)@"
	chmod -w $@

#
#	Check db files against current foomatic to see if any changes
#	need to be made or reported.
#
MYFOODB=../foomatic/foomatic-db/db/source
checkdb:
	@for dir in driver printer opt; do \
	    for file in foomatic-db/$$dir/*.xml ; do \
		ofile=$(MYFOODB)/$$dir/`basename $$file`; \
		: echo diff -N -u $$ofile $$file; \
		if [ ! -f  $$ofile ]; then \
		    ofile=/dev/null; \
		fi; \
		diff -N -u $$ofile $$file; \
	    done \
	done

#
#	Mail my latest foomatic-db entries to Till.
#
WHO=rick.richardson@comcast.net
WHO=till.kamppeter@gmx.net
maildb:
	$(MAKE) -s checkdb > patch.db
	echo "Here is a patch for the foomatic-db foo2zjs/foo2oak entries." | \
	mutt -a patch.db \
	    -s "foo2zjs/foo2oak - patch for foomatic database" $(WHO)

#
# Create tarball
#
tar:	
	HERE=`basename $$PWD`; \
	/bin/ls $(FILES) | \
	sed -e "s?^?$$HERE/?" | \
	(cd ..; tar -c -z -f $$HERE/$$HERE.tar.gz -T-)

#
# Populate the web site
#	make web
#	make webworld
#	make webextra
#
URLOAK=http://foo2oak.rkkda.com
URLZJS=http://foo2zjs.rkkda.com
URLHP=http://foo2hp.rkkda.com
URLXQX=http://foo2xqx.rkkda.com
URLLAVA=http://foo2lava.rkkda.com
URLQPDL=http://foo2qpdl.rkkda.com
FTPSITE=~/.ncftp-website

foo2zjs.html foo2oak.html foo2hp.html \
    foo2xqx.html foo2lava.html foo2qpdl.html: thermometer.gif FRC
	rm -f $@
	HERE=`basename $$PWD`; \
	TZ=`date | cut -c 21-24`; \
	modtime() { $(MODTIME); }; \
	MODindex=`modtime $@.in`; \
	MODtarball=`modtime $$HERE.tar.gz`; \
	PRODUCT=`basename $@ .html`; \
	./includer-html $@.in | sed > $@ \
	    -e "s@\$${URLOAK}@$(URLOAK)@g" \
	    -e "s@\$${URLZJS}@$(URLZJS)@g" \
	    -e "s@\$${URLHP}@$(URLHP)@g" \
	    -e "s@\$${URLXQX}@$(URLXQX)@g" \
	    -e "s@\$${URLLAVA}@$(URLLAVA)@g" \
	    -e "s@\$${URLQPDL}@$(URLQPDL)@g" \
	    -e "s@\$${PRODUCT}@$$PRODUCT@g" \
	    -e "s/\$${MODindex}/$$MODindex $$TZ/" \
	    -e "s/\$${MODtarball}/$$MODtarball $$TZ/"
	chmod -w $@

web: test tar manual.pdf webindex
	ncftpput -m -f $(FTPSITE) foo2zjs \
	    ChangeLog INSTALL manual.pdf *.tar.gz;

webt: tar manual.pdf webindex
	ncftpput -m -f $(FTPSITE) foo2zjs \
	    ChangeLog INSTALL manual.pdf *.tar.gz;

webworld: web webpics

webindex: INSTALL zjsindex oakindex hpindex xqxindex lavaindex qpdlindex

webpics: redhat suse ubuntu mandriva

zjsindex: foo2zjs.html archzjs.gif thermometer.gif
	ln -sf foo2zjs.html index.html
	ncftpput -m -f $(FTPSITE) foo2zjs \
	    index.html style.css archzjs.gif thermometer.gif \
	    INSTALL INSTALL.osx zjsfavicon.png;

oakindex: foo2oak.html archoak.gif thermometer.gif
	ln -sf foo2oak.html index.html
	ncftpput -m -f $(FTPSITE) foo2oak \
	    index.html style.css archoak.gif thermometer.gif \
	    INSTALL;

hpindex: foo2hp.html archhp.gif thermometer.gif
	ln -sf foo2hp.html index.html
	ncftpput -m -f $(FTPSITE) foo2hp \
	    index.html style.css archhp.gif thermometer.gif \
	    INSTALL hpfavicon.png;

xqxindex: foo2xqx.html archxqx.gif thermometer.gif
	ln -sf foo2xqx.html index.html
	ncftpput -m -f $(FTPSITE) foo2xqx \
	    index.html style.css archxqx.gif thermometer.gif \
	    INSTALL xqxfavicon.png;

lavaindex: foo2lava.html archlava.gif thermometer.gif
	ln -sf foo2lava.html index.html
	ncftpput -m -f $(FTPSITE) foo2lava \
	    index.html style.css archlava.gif thermometer.gif \
	    INSTALL lavafavicon.png;

qpdlindex: foo2qpdl.html archqpdl.gif thermometer.gif
	ln -sf foo2qpdl.html index.html
	ncftpput -m -f $(FTPSITE) foo2qpdl \
	    index.html style.css archqpdl.gif thermometer.gif \
	    INSTALL qpdlfavicon.png;

# RedHat
redhat: FRC
	cd redhat; $(MAKE) web FTPSITE=$(FTPSITE)

# Fedora Core 6+
fedora: FRC
	cd fedora; $(MAKE) web FTPSITE=$(FTPSITE)

suse: FRC
	cd suse; $(MAKE) web FTPSITE=$(FTPSITE)

ubuntu: FRC
	cd ubuntu; $(MAKE) web FTPSITE=$(FTPSITE)

mandriva: FRC
	cd mandriva; $(MAKE) web FTPSITE=$(FTPSITE)

#
#	Extra files from web
#
webextra: webicm webfw

webicm: icm/km2430.tar.gz icm/hpclj2600n.tar.gz icm/km2530.tar.gz \
	icm/samclp300.tar.gz
	ncftpput -m -f $(FTPSITE) foo2zjs icm/km2430.tar.gz;
	ncftpput -m -f $(FTPSITE) foo2hp icm/hpclj2600n.tar.gz;
	ncftpput -m -f $(FTPSITE) foo2lava icm/km2530.tar.gz;
	ncftpput -m -f $(FTPSITE) foo2qpdl icm/samclp300.tar.gz;

icm/km2430.tar.gz: FRC
	cd icm; tar -c -z -f ../$@ km2430*.icm
icm/hpclj2600n.tar.gz: FRC
	cd icm; tar -c -z -f ../$@ hpclj2600*.icm
icm/km2530.tar.gz: FRC
	cd icm; tar -c -z -f ../$@ km2530*.icm
icm/samclp300.tar.gz: FRC
	cd icm; tar -c -z -f ../$@ samclp300*.icm

webfw:	firmware/sihp1000.tar.gz \
	firmware/sihp1005.tar.gz \
	firmware/sihp1018.tar.gz \
	firmware/sihp1020.tar.gz \
	$(NULL)
	ncftpput -m -f $(FTPSITE) foo2zjs firmware/*.tar.gz;

firmware/sihp1000.tar.gz: FRC
	cd firmware; tar -c -z -f ../$@ sihp1000.img
firmware/sihp1005.tar.gz: FRC
	cd firmware; tar -c -z -f ../$@ sihp1005.img
firmware/sihp1018.tar.gz: FRC
	cd firmware; tar -c -z -f ../$@ sihp1018.img
firmware/sihp1020.tar.gz: FRC
	cd firmware; tar -c -z -f ../$@ sihp1020.img

FRC:

#
#	Target for building Oak Technologies encoder/decoder (for HP1500)
#
#	This source code will not be made available unless someone sponsors
#	me to finish the work on it.
#
LIBJBGOAK	=	jbig.o jbig_tab.o
o: oakdecode foo2oak foo2oak-wrapper
	root install oakdecode foo2oak foo2oak-wrapper $(BIN)/
oakdecode: oakdecode_$(MACH).o $(LIBJBGOAK)
	$(CC) $(CFLAGS) oakdecode_$(MACH).o $(LIBJBGOAK) -o $@
foo2oak: foo2oak_$(MACH).o $(LIBJBGOAK)
	$(CC) $(CFLAGS) -o $@ foo2oak_$(MACH).o $(LIBJBGOAK)
foo2oak-wrapper: foo2oak-wrapper.in Makefile
	[ ! -f $@ ] || chmod +w $@
	sed < $@.in > $@ \
	    -e 's@^PREFIX=.*@PREFIX=$(PREFIX)@' || (rm -f $@ && exit 1)
	chmod 555 $@

SYSNAME := $(shell uname -n)
ifeq ($(SYSNAME),ipcroe.rkkda.com)
foo2oak_$(MACH).o: oak.h jbig.h foo2oak.c
	cp -a foo2oak.c foo2oak_$(MACH).c
	$(CC) $(CFLAGS) -c foo2oak_$(MACH).c
oakdecode_$(MACH).o: oak.h oakdecode.c
	cp -a oakdecode.c oakdecode_$(MACH).c
	$(CC) $(CFLAGS) -c oakdecode_$(MACH).c
endif
ifeq ($(SYSNAME),amd.rkkda.org)
foo2oak_$(MACH).o: oak.h jbig.h foo2oak.c
	cp -a foo2oak.c foo2oak_$(MACH).c
	$(CC) $(CFLAGS) -c foo2oak_$(MACH).c
oakdecode_$(MACH).o: oak.h oakdecode.c
	cp -a oakdecode.c oakdecode_$(MACH).c
	$(CC) $(CFLAGS) -c oakdecode_$(MACH).c
endif

#
# Misc
#
misc: pksm2bitcmyk

pksm2bitcmyk: pksm2bitcmyk.c
	$(CC) $(CFLAGS) pksm2bitcmyk.c -lpbm -o $@
