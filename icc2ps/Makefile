UNAME := $(shell uname)
PREFIX=	/usr
BIN=	$(PREFIX)/bin
SRC=	icc2ps.c xgetopt.c
LIB=	cmscam97.c cmscnvrt.c cmserr.c cmsgamma.c cmsgmt.c cmsintrp.c cmsio1.c \
	cmslut.c cmsmatsh.c cmsmtrx.c cmsnamed.c cmspack.c cmspcs.c cmsps2.c \
	cmssamp.c cmswtpnt.c cmsxform.c cmsio0.c cmsvirt.c
CFLAGS=	-O3
INSTALL=install
ifeq ($(UNAME),SunOS)
    INSTALL=/usr/ucb/install
    CC=gcc
endif

all: foo2zjs-icc2ps

foo2zjs-icc2ps: $(SRC) $(LIB)
	$(CC) $(CFLAGS) $(SRC) $(LIB) -lm -o $@

install: all
	$(INSTALL) -c foo2zjs-icc2ps $(BIN)

clean:
	rm -f *.o foo2zjs-icc2ps

foo2zjs-icc2ps: icc34.h lcms.h

regress: all
	@./foo2zjs-icc2ps a b 2>&1 | grep "little cms P"
	@for i in ../icm/*.icm; do \
	    b=`basename $$i`; \
	    ./foo2zjs-icc2ps -o $$i -t0 | grep -v Created >$$b.ps; \
	    md5sum $$b.ps; \
	done
	@rm -f *icm.ps
