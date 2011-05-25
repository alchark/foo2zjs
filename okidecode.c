/*
 * $Id: okidecode.c,v 1.9 2006/12/07 13:33:50 rick Exp $
 */

/*b
 * Copyright (C) 2003-2006  Rick Richardson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Author: Rick Richardson <rick.richardson@comcast.net>
b*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "oki.h"
#include "jbig.h"

/*
 * Global option flags
 */
int	Debug = 0;
char	*RawFile;
char	*DecFile;
int	PrintOffset = 0;
int	PrintHexOffset = 0;

void
debug(int level, char *fmt, ...)
{
    va_list ap;

    if (Debug < level)
	return;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

int
error(int fatal, char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, fatal ? "Error: " : "Warning: ");
	if (errno)
	    fprintf(stderr, "%s: ", strerror(errno));
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	if (fatal > 0)
	    exit(fatal);
	else
	{
	    errno = 0;
	    return (fatal);
	}
}

void
usage(void)
{
    fprintf(stderr,
"Usage:\n"
"	okidecode [options] < oki-file\n"
"\n"
"	Decode a ZjStream into human readable form.\n"
"\n"
"	A ZjStream is the printer langauge used by some Minolta/QMS and\n"
"	HP printers, such as the 2300DL and LJ-1000.\n"
"\n"
"	More information on Zenographics ZjStream can be found at:\n"
"\n"
"		http://ddk.zeno.com\n"
"\n"
"Options:\n"
"       -d basename Basename of .pbm file for saving decompressed planes\n"
"       -r basename Basename of .jbg file for saving raw planes\n"
"       -o          Print file offsets\n"
"       -h          Print hex file offsets\n"
"       -D lvl      Set Debug level [%d]\n"
    , Debug
    );

    exit(1);
}

static int
getdword(unsigned char buf[4])
{
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3] << 0);
}

void
print_bih(unsigned char bih[20])
{
    unsigned int xd, yd, l0;

    xd = (bih[4] << 24) | (bih[5] << 16) | (bih[6] << 8) | (bih[7] << 0);
    yd = (bih[8] << 24) | (bih[9] << 16) | (bih[10] << 8) | (bih[11] << 0);
    l0 = (bih[12] << 24) | (bih[13] << 16) | (bih[14] << 8) | (bih[15] << 0);

    printf("		DL = %d, D = %d, P = %d, - = %d, XY = %d x %d\n",
	 bih[0], bih[1], bih[2], bih[3], xd, yd);

    printf("		L0 = %d, MX = %d, MY = %d\n",
	 l0, bih[16], bih[17]);

    printf("		Order   = %d %s%s%s%s%s\n", bih[18],
	bih[18] & JBG_HITOLO ? " HITOLO" : "",
	bih[18] & JBG_SEQ ? " SEQ" : "",
	bih[18] & JBG_ILEAVE ? " ILEAVE" : "",
	bih[18] & JBG_SMID ? " SMID" : "",
	bih[18] & 0xf0 ? " other" : "");

    printf("		Options = %d %s%s%s%s%s%s%s%s\n", bih[19],
	bih[19] & JBG_LRLTWO ? " LRLTWO" : "",
	bih[19] & JBG_VLENGTH ? " VLENGTH" : "",
	bih[19] & JBG_TPDON ? " TPDON" : "",
	bih[19] & JBG_TPBON ? " TPBON" : "",
	bih[19] & JBG_DPON ? " DPON" : "",
	bih[19] & JBG_DPPRIV ? " DPPRIV" : "",
	bih[19] & JBG_DPLAST ? " DPLAST" : "",
	bih[19] & 0x80 ? " other" : "");
    printf("		%u stripes, %d layers, %d planes\n",
	((yd >> bih[1]) +  ((((1UL << bih[1]) - 1) & xd) != 0) + l0 - 1) / l0,
	bih[1] - bih[0], bih[2]);
}

void
decode(FILE *fp)
{
    int		c;
    int		rc;
    FILE	*dfp = NULL;
    int		pageNum = 1;
    int		len;
    int		curOff = 0;
    struct jbg_dec_state	s[5];
    unsigned char	bih[4][20];
    int			imageCnt[4] = {0,0,0,0};
    int         	pn = 0;
    char		buf[1024];

    c = getc(fp);
    if (c == EOF)
    {
	printf("EOF on file\n");
	return;
    }
    ungetc(c, fp);
    if (c == '\033')
    {
	while (fgets(buf, sizeof(buf), fp))
	{
	    if (PrintOffset)
		printf("%d:	", curOff);
	    else if (PrintHexOffset)
		printf("%6x:	", curOff);
	    fputs(buf, stdout);
	    curOff += strlen(buf);
	    if (strcmp(buf, "@PJL ENTER LANGUAGE=HIPERC\n") == 0)
		break;
	}
    }

    for (;;)
    {
	unsigned int	reclen;
	unsigned int	rectype;
	unsigned int	blknum;

	rc = fread(&reclen, 4, 1, fp);
	if (rc != 1) break;
	rc = fread(&rectype, 4, 1, fp);
	if (rc != 1) break;

	reclen = be32(reclen);
	rectype = be32(rectype);
	if (PrintOffset) printf("%d:	", curOff);
	else if (PrintHexOffset) printf("%6x:	", curOff);
	printf("RECTYPE %d (len=%d)\n", rectype, reclen);
	curOff += 8;

	blknum = 0;
	reclen -= 8;
	while (reclen)
	{
	    int	blklen;
	    int	i;
	    unsigned char blk[200*1024];

	    if (PrintOffset) printf("%d:	", curOff);
	    else if (PrintHexOffset) printf("%6x:	", curOff);

	    rc = fread(&blklen, 4, 1, fp);
	    if (rc != 1) break;
	    curOff += 4;
	    reclen -= 4;
	    
	    blklen = be32(blklen);
	    rc = fread(&blk, blklen, 1, fp);
	    if (rc != 1) return;
	    curOff += blklen;
	    reclen -= blklen;


	    switch (rectype)
	    {
	    case 0:
		switch (blknum)
		{
		case 0:
		    printf("\tBLKNUM %d, nbie=%d, pn=%d",
			blknum, blk[0], blk[1]);
		    switch (pn = blk[1])
		    {
		    case 0:		printf(" [cyan] "); break;
		    case 1:		printf(" [yello]"); break;
		    case 2:		printf(" [magen]"); break;
		    case 3:		printf(" [black]"); break;
		    }
		    printf(" uc=%d,%d, wid=%d ud=%d,%x\n",
			blk[2], blk[3],
			getdword(blk+4), getdword(blk+8), getdword(blk+12));
		    if (pn < 0 || pn > 3)
			error(1, "Plane %d is not 0-3.\n", pn);
		    break;
		case 1:	/* BIH */
		    printf("\tBLKNUM %d (len=%d), bie:\n",
			blknum, blklen);
		    memcpy(bih[pn], blk, 20);
		    // bih[18] |= JBG_ILEAVE | JBG_SMID;
		    // bih[19] |= JBG_TPDON | JBG_DPON;
		    // bih[19] |= JBG_DELAY_AT;
		    print_bih(bih[pn]);
		    imageCnt[pn] = 0;
		    break;
		}
		break;
	    case 1:
		switch (blknum)
		{
		case 0:	/* PLANE */
		    printf("\tBLKNUM %d (len=%d), plane=%d, uc=%d,%d,%d\n",
			blknum, blklen, pn = blk[0], blk[1], blk[2], blk[3]);
		    break;
		case 1:	/* DATA */
		    printf("\tBLKNUM %d (len=%d), Data=", blknum, blklen);
		    for (i = 0; i < blklen && i < 11; ++i)
			printf("%02x ", blk[i]);
		    if (blklen >= 11)
			printf("...");
		    printf("\n");
		    if (DecFile)
		    {
			size_t		cnt;
			unsigned char	*image;

			if (imageCnt[pn] == 0)
			{
			    jbg_dec_init(&s[pn]);
			    rc = jbg_dec_in(&s[pn], bih[pn], 20, NULL);
			    if (rc == JBG_EIMPL)
				error(1, "JBIG uses unimplemented feature\n");
			    // fwrite(bih, 20, 1, dfp);
			    imageCnt[pn] = 20;
			    len = jbg_dec_getsize(&s[pn]);
			    image = jbg_dec_getimage(&s[pn], 0);
			    if (image && len)
				memset(image, 0, len);
			}
			for (i = 0; i < blklen; ++i)
			{
			    rc = jbg_dec_in(&s[pn], &blk[i], 1, &cnt);
			    if (rc == JBG_EAGAIN && rc == JBG_OK)
				error(1, "jbg_dec_in rc=%d (%s)\n",
				    rc, jbg_strerror(rc, 0));
			}
			if (rc == JBG_OK)
			{
			    int	w, h;

			    h = jbg_dec_getheight(&s[pn]);
			    w = jbg_dec_getwidth(&s[pn]);
			    len = jbg_dec_getsize(&s[pn]);
			    image = jbg_dec_getimage(&s[pn], 0);
			    printf("\trc=%d(%s) %dx%d len=%d\n",
				rc, jbg_strerror(rc, 0), w, h, len);

			    sprintf(buf, "%s-%02d-%d.pbm",
					DecFile, pageNum, pn);
			    dfp = fopen(buf, "w");
			    if (!dfp)
				error(1, "Couldn't open '%s'.\n", buf);
			    fprintf(dfp, "P4\n%8d %8d\n", w, h);
			    fwrite(image, 1, len, dfp);
			    fclose(dfp);

			    jbg_dec_free(&s[pn]);
			    imageCnt[pn] = 0;
			}
		    }
		    break;
		}
		break;
	    default:
		printf("\t");
		for (i = 0; i < blklen && i < 20; ++i)
		    printf("%02x ", blk[i]);
		if (blklen >= 22)
		    printf("...");
		printf("\n");
		break;
	    }

	    ++blknum;
	}
	if (rectype == 255)
	{
	    c = getc(fp);
	    if (c == EOF) break;
	    ungetc(c, fp);
	    if (c == '\033') break;
	    ++pageNum;
	}
    }

    while (fgets(buf, sizeof(buf), fp))
    {
	if (PrintOffset)
	    printf("%d:	", curOff);
	else if (PrintHexOffset)
	    printf("%6x:	", curOff);
	fputs(buf, stdout);
	curOff += strlen(buf);
	if (strcmp(buf, "@PJL ENTER LANGUAGE=HIPERC\n") == 0)
	    break;
    }
    printf("\n");
}

int
main(int argc, char *argv[])
{
	extern int	optind;
	extern char	*optarg;
	int		c;

	while ( (c = getopt(argc, argv, "d:hor:D:?h")) != EOF)
		switch (c)
		{
		case 'd': DecFile = optarg; break;
		case 'r': RawFile = optarg; break;
		case 'o': PrintOffset = 1; break;
		case 'h': PrintHexOffset = 1; break;
		case 'D': Debug = atoi(optarg); break;
		default: usage(); exit(1);
		}

	argc -= optind;
	argv += optind;

	decode(stdin);

	exit(0);
}
