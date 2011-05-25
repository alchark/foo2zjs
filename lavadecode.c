/*
 * $Id: lavadecode.c,v 1.15 2006/12/23 13:40:51 rick Exp $
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

#include "jbig.h"

/*
 * Global option flags
 */
int	Debug = 0;
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
"	xqxdecode [options] < zjs-file\n"
"\n"
"	Decode a XQX stream into human readable form.\n"
"\n"
"	A XQX stream is the printer langauge used by some HP LaserJet\n"
"	printers, such as the HP LaserJet M1005 (MFP).\n"
"\n"
"\n"
"Options:\n"
"       -d basename Basename of .pbm file for saving decompressed planes\n"
"       -o          Print file offsets\n"
"       -h          Print hex file offsets\n"
"       -D lvl      Set Debug level [%d]\n"
    , Debug
    );

    exit(1);
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
proff(int curOff)
{
    if (PrintOffset)
	printf("%d:	", curOff);
    else if (PrintHexOffset)
	printf("%6x:	", curOff);
}

void
decode(FILE *fp)
{
    int		c;
    int		rc;
    FILE	*dfp = NULL;
    int		pageNum = 0;
    int		curOff = 0;
    struct jbg_dec_state	s[5];
    unsigned char	bih[20];
    int			bihlen = 0;
    int			imageCnt[5] = {0,0,0,0,0};
    int         	pn = 0;
    int			nbie = 0;
    int         	incrY = 0;
    int			totSize = 0;
    char		buf[1024];
    char		*strpage[837+1];
    int			i;

    for (i = 0; i < sizeof(strpage)/sizeof(strpage[0]); ++i)
	strpage[i] = "unk";
    strpage[1] = "exec";
    strpage[2] = "letter";
    strpage[3] = "legal";
    strpage[25] = "a5";
    strpage[26] = "a4";
    strpage[45] = "b5jis";
    strpage[65] = "b5iso";
    strpage[80] = "envMonarch";
    strpage[81] = "env#10";
    strpage[90] = "envDL";
    strpage[91] = "envC5";
    strpage[92] = "envC6";
    strpage[835] = "photo4x6";
    strpage[837] = "photo10x15";

    while (fgets(buf, sizeof(buf), fp))
    {
	proff(curOff);
	if (buf[0] == '\033')
	{
	    printf("\\033");
	    fputs(buf+1, stdout);
	}
	else
	    fputs(buf, stdout);
	curOff += strlen(buf);

	if (strcmp(buf, "\033%-12345X@PJL ENTER LANGUAGE=LAVAFLOW\n") == 0)
	{
	    int		state = 0;
	    char	intro = 0, groupc = 0;
	    int		val = 0;
	    size_t		cnt;
	    unsigned char	ch;
	
	    while ( (c = fgetc(fp)) != EOF)
	    {
		curOff++;
		switch (state)
		{
		case 0:
		    if (c == '\033') state = '\033';
		    if (c == '\033') proff(curOff-1);
		    break;
		case '\033':
		    if (c >= 'A' && c <= 'Z')
		    {
			printf("\\033%c", c);
			if (c == 'E')
			    printf("\t\t\tRESET");
			printf("\n");
			state = 0;
		    }
		    else
		    {
			intro = c;
			state = 'i';
		    }
		    break;
		case 'i':
		    if (c >= 'A' && c <= 'Z')
			state = 0;
		    else
		    {
			groupc = c;
			state = 'v';
			val = 0;
		    }
		    break;
		case 'v':
		    if (c >= 'A' && c <= 'Z')
		    {
			#define STRARY(X, A) \
			    ((X) >= 0 && (X) < sizeof(A)/sizeof(A[0])) \
			    ? A[X] : "UNK"
			char *strduplex[] = {
			    "off", "long", "short", "manlong", "manshort"
			    };
			char *strorient[] = { "port", "land" };
			char *strsource[] = {
			    /*00*/ "unk", "tray1", "unk", "unk", "tray2",
			    /*05*/ "unk", "unk", "auto"
			    };
			char *strmedia[] = { 
			    /*00*/ "plain", "unk", "unk", "unk", "transparency",
			    /*05*/ "unk", "unk", "unk", "unk", "unk",
			    /*10*/ "unk", "unk", "unk", "unk", "unk",
			    /*15*/ "unk", "unk", "unk", "unk", "unk",
			    /*20*/ "thick", "unk", "env", "letterhead", "unk",
			    /*25*/ "postcard", "labels", "recycled", "glossy",
			    };

			printf("\\033%c%c%d%c", intro, groupc, val, c);
			state = 0;
			if (intro == '&' && groupc == 'l' && c == 'S')
			    printf("\t\tDUPLEX: [%s]", STRARY(val, strduplex));
			if (intro == '&' && groupc == 'u' && c == 'D')
			    printf("\t\tX RESOLUTION: [%d]", val);
			if (intro == '&' && groupc == 'l' && c == 'X')
			    printf("\t\tCOPIES: [%d]", val);
			if (intro == '&' && groupc == 'x' && c == 'X')
			    printf("\t\tTRANSMIT ONCE COPIES: [%d]", val);
			if (intro == '&' && groupc == 'l' && c == 'O')
			    printf("\t\tORIENTATION: [%s]",
				STRARY(val, strorient));
			if (intro == '&' && groupc == 'l' && c == 'A')
			    printf("\t\tPAGE SIZE: [%s]",
				STRARY(val, strpage));
			if (intro == '&' && groupc == 'l' && c == 'E')
			    printf("\t\tTOP MARGIN: [%d]", val);
			if (intro == '&' && groupc == 'l' && c == 'H')
			    printf("\t\tPAPER SOURCE: [%s]",
				STRARY(val&7, strsource));
			if (intro == '&' && groupc == 'l' && c == 'M')
			    printf("\t\tMEDIA TYPE: [%s]",
				STRARY(val, strmedia));
			if (intro == '*' && groupc == 'r' && c == 'S')
			    printf("\t\tX RASTER: [%d]", val);
			if (intro == '*' && groupc == 'r' && c == 'T')
			    printf("\t\tY RASTER: [%d]", val);
			if (intro == '*' && groupc == 'r' && c == 'U')
			{
			    nbie = val & 7;
			    printf("\t\tNBIE: [%d]", nbie);
			}
			if (intro == '*' && groupc == 'g' && c == 'W')
			    printf("\t\tBW/COLOR: [%d]", val);
			if (intro == '*' && groupc == 'b' && c == 'M')
			    printf("\t\tCOMPRESSION: [%d]", val);
			if (intro == '*' && groupc == 'r' && c == 'C')
			    printf("\t\tEND PAGE");
			if (intro == '*' && groupc == 'p' && c == 'X')
			    printf("\t\tX OFFSET: [%d]", val);
			if (intro == '*' && groupc == 'p' && c == 'Y')
			    printf("\t\tY OFFSET: [%d]", val);
			if (intro == '*' && groupc == 'x' && c == 'Y')
			    printf("\t\tYELLOW DOTS: [%d]", val);
			if (intro == '*' && groupc == 'x' && c == 'M')
			    printf("\t\tMAGENTA DOTS: [%d]", val);
			if (intro == '*' && groupc == 'x' && c == 'C')
			    printf("\t\tCYAN DOTS: [%d]", val);
			if (intro == '*' && groupc == 'x' && c == 'K')
			    printf("\t\tBLACK DOTS: [%d]", val);
			if (intro == '*' && groupc == 'x' && c == 'U')
			    printf("\t\tYELLOW WHITEDOTS: [%d]", val);
			if (intro == '*' && groupc == 'x' && c == 'V')
			    printf("\t\tMAGENTA WHITEDOTS: [%d]", val);
			if (intro == '*' && groupc == 'x' && c == 'Z')
			    printf("\t\tCYAN WHITEDOTS: [%d]", val);
			if (intro == '*' && groupc == 'x' && c == 'W')
			    printf("\t\tBLACK WHITEDOTS: [%d]", val);

			if (intro == '%' && val == 12345 && c == 'X')
			    goto out;
			if (intro == '*' && groupc == 'r' && c == 'A')
			{
			    ++pageNum;
			    printf("\t\t[Page %d]", pageNum); 
			    pn = 0;
			}
			if (intro == '*' && groupc == 'b' && c == 'W')
			{
			    printf("\tJBIG data (end)");
			    state = 'd';
			}
			if (intro == '*' && groupc == 'b' && c == 'V')
			{
			    if (val == 20)
			    {
				char *color[] =
				    { "yellow", "magenta", "cyan", "black" };
				    
				++pn;
				rc = fread(bih, bihlen = sizeof(bih), 1, fp);
				curOff += bihlen;
				if (nbie == 4 && pn >= 1 && pn <= 4)
				    printf("\t\t[%s]\n", color[pn-1]);
				else if (nbie == 1)
				    printf("\t\t[%s]\n", "black");
				else
				    printf("\t\t[%s]\n", "unknown");
				print_bih(bih);
				if (DecFile)
				{
				    size_t	cnt;

				    jbg_dec_init(&s[pn]);
				    rc = jbg_dec_in(&s[pn], bih, bihlen, &cnt);
				    if (rc == JBG_EIMPL)
					error(1, "JBIG uses unimpl feature\n");
				}
			    }
			    else
			    {
				printf("\tJBIG data (first)");
				state = 'd';
			    }
			}
			printf("\n");
		    }
		    else if (c >= '0' && c <= '9')
		    {
			val *= 10; val += c - '0';
		    }
		    break;
		case 'd':
		    --val;
		    if (val == 0)
			state = 0;
		    if (!DecFile)
			break;

		    ch = c;
		    rc = JBG_EAGAIN;
		    rc = jbg_dec_in(&s[pn], &ch, 1, &cnt);
		    if (rc == JBG_EOK)
		    {
			int	h, w, len;
			unsigned char *image;

			//debug(0, "JBG_OK: %d\n", pn);
			h = jbg_dec_getheight(&s[pn]);
			w = jbg_dec_getwidth(&s[pn]);
			image = jbg_dec_getimage(&s[pn], 0);
			len = jbg_dec_getsize(&s[pn]);
			if (image)
			{
			    char	buf[512];
			    sprintf(buf, "%s-%02d-%d.pbm",
				    DecFile, pageNum, pn);
			    dfp = fopen(buf, imageCnt[pn] ? "a" : "w");
			    if (dfp)
			    {
				if (imageCnt[pn] == 0)
				    fprintf(dfp, "P4\n%8d %8d\n", w, h);
				imageCnt[pn] += incrY;
				fwrite(image, 1, len, dfp);
				fclose(dfp);
			    }
			}
			else
			    debug(0, "Missing image %dx%d!\n", h, w);
			jbg_dec_free(&s[pn]);
		    }
		    break;
		}
	    }
	out:
	    ;
	}
    }
    if (feof(fp))
	return;

    printf("Total size: %d bytes\n", totSize);
}

int
main(int argc, char *argv[])
{
	extern int	optind;
	extern char	*optarg;
	int		c;

	while ( (c = getopt(argc, argv, "d:hoD:?h")) != EOF)
		switch (c)
		{
		case 'd': DecFile = optarg; break;
		case 'o': PrintOffset = 1; break;
		case 'h': PrintHexOffset = 1; break;
		case 'D': Debug = atoi(optarg); break;
		default: usage(); exit(1);
		}

	argc -= optind;
	argv += optind;

	for(;;)
	{
	    decode(stdin);
	    c = getc(stdin); ungetc(c, stdin);
	    if (feof(stdin))
		break;
	}
	printf("\n");

	exit(0);
}
