PREFIX=	/usr
BIN=	$(PREFIX)/bin
SRC=	icc2ps.c xgetopt.c
LIB=	cmscam97.c cmscnvrt.c cmserr.c cmsgamma.c cmsgmt.c cmsintrp.c cmsio1.c \
	cmslut.c cmsmatsh.c cmsmtrx.c cmsnamed.c cmspack.c cmspcs.c cmsps2.c \
	cmssamp.c cmswtpnt.c cmsxform.c cmsio0.c cmsvirt.c
CFLAGS=	-O3

all: foo2zjs-icc2ps

foo2zjs-icc2ps: $(SRC) $(LIB)
	$(CC) $(CFLAGS) $(SRC) $(LIB) -lm -o $@

install: all
	install -c foo2zjs-icc2ps $(BIN)

clean:
	rm -f *.o foo2zjs-icc2ps

foo2zjs-icc2ps: icc34.h lcms.h
