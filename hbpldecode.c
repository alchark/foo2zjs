/*
 * $Id: hbpldecode.c,v 1.13 2011/06/02 20:57:24 rick Exp $
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

// #include "hiperc.h"
// #include <zlib.h>

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
"	splcdecode [options] < oki-file\n"
"\n"
"	Decode a SPL-C into human readable form.\n"
"\n"
"	A SPL-C is the printer langauge used by some Samsung\n"
"	printers, such as the CLT-510n.\n"
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
getLEdword(unsigned char buf[4])
{
    return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0] << 0);
}

static int
getLEword(unsigned char buf[2])
{
    return (buf[1] << 8) | (buf[0] << 0);
}

#if 0
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
#endif

void
proff(int curOff)
{
    if (PrintOffset)
        printf("%d:     ", curOff);
    else if (PrintHexOffset)
        printf("%6x:    ", curOff);
}

int
payload(FILE *fp, int *curoffp, unsigned char *data, int *vp)
{
    int	type;
    int subtype;
    int len = 2;
    int val2 = 0;

    type = fgetc(fp);
again:
    subtype = fgetc(fp);
    switch (subtype)
    {
    case 0xa1:
	data[0] = fgetc(fp);
	len += 1;
	*vp = data[0];
	break;
    case 0xa2:
	data[0] = fgetc(fp);
	data[1] = fgetc(fp);
	len += 2;
	*vp = getLEword(data);
	break;
    case 0xa4:
	data[0] = fgetc(fp);
	data[1] = fgetc(fp);
	data[2] = fgetc(fp);
	data[3] = fgetc(fp);
	len += 4;
	*vp = getLEdword(data);
	break;
    case 0xc2: case 0xc3:
	data[0] = fgetc(fp);
	data[1] = fgetc(fp);
	data[2] = fgetc(fp);
	data[3] = fgetc(fp);
	len += 4;
	*vp = getLEdword(data);
	break;
    case 0xc4:
	data[0] = fgetc(fp);
	data[1] = fgetc(fp);
	data[2] = fgetc(fp);
	data[3] = fgetc(fp);
	data[4] = fgetc(fp);
	data[5] = fgetc(fp);
	data[6] = fgetc(fp);
	data[7] = fgetc(fp);
	len += 8;
	*vp = getLEdword(data);
	val2 = getLEdword(data + 4);
	break;
    case 0xb1:
	len += 1;
	goto again;
	break;
    default:
	error(1, "Unknown subtype 0x%02x\n", subtype);
	break;
    }
    proff(*curoffp);
    switch (type)
    {
    case 0x99:
	printf("	%x %x: %d [PAGECNT]\n", type, subtype, *vp);
	break;
    case 0x9a:
	printf("	%x %x: %dx%d [WxH]\n", type, subtype, *vp, val2);
	break;
    case 0x9d:
	printf("	%x %x: 0x%x [%s]\n", type, subtype, *vp,
			    *vp == 9 ? "Mono" : "Color");
	break;
    case 0xa2:
	printf("	%x %x: %dx%d [WxH]\n", type, subtype, *vp, val2);
	break;
    case 0xa4:
	printf("	%x %x: %d (0x%x) bytes of data...\n",
	    type, subtype, *vp, *vp);
	break;
    default:
	printf("	%x %x: 0x%x\n", type, subtype, *vp);
	break;
    }
    *curoffp += len;

    return type;
}

void
decode(FILE *fp)
{
    int		c;
    // int		rc;
    // FILE	*dfp = NULL;
    // int		pageNum = 1;
    // int		len;
    int		curOff = 0;
    //struct jbg_dec_state	s[5];
    // unsigned char	bih[4][20];
    // int			imageCnt[4] = {0,0,0,0};
    // int         	pn = 0;
    char		buf[70000];

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
	    proff(curOff);
            if (buf[0] == '\033')
            {
		printf("\\033");
		fputs(buf+1, stdout);
            }
            else
		fputs(buf, stdout);
	    curOff += strlen(buf);
	    if (strcmp(buf, "@PJL ENTER LANGUAGE=HBPL\n") == 0)
		break;
	}
    }

/*
00000000: 41 81 a1 00 82 a2 07 00  83 a2 01 00 43 91 a1 00 | A...........C... |
             -------- -----------  -----------    --------
00000010: 92 a1 01 93 a1 01 94 a1  00 95 c2 00 00 00 00 96 | ................ |
          -------- -------- --------- ----------------- --
00000020: a1 00 97 c3 00 00 00 00  98 a1 00 99 a4 01 00 00 | ................ |
          ----- -----------------  -------- --------------
00000030: 00 9a c4 f0 13 00 00 c8  19 00 00 9b a1 00 9c a1 | ................ |
          -- ------------------------------ -------- -----
00000040: 01 9d a1 8b 9e a1 02 9f  a1 05 a0 a1 08 a1 a1 00 | ................ |
          -- -------- -------- --------- -------- --------
00000050: a2 c4 f0 13 00 00 c8 19  00 00 51 52 a3 a1 00 a4 | ..........QR.... |
          -------------------------------      -------- --
00000060: b1 a4 ff 2a 01 00 20 00  00 00 00 01 01 00 10 32 | ...*.. ........2 |
          -----------------
                LEN??
00000060: b1 a2 39 19 20 00 00 00  08 01 00 00 10 32 04 00 | ..9. ........2.. |

00000070: 04 00 a1 42 00 00 00 00  ff 00 00 00 00 00 00 00 | ...B............ |
00000080: 00 00 00 00 00 00 18 fc  00 00 01 0b 00 00 5f 0c | .............._. |
00000090: 00 00 a7 0b 00 00 a7 e8  a6 3f ff fd ff ff ef ff | .........?...... |
000000a0: ff 7f ff fb ff ff df ff  fe ff ff f7 ff ff bf ff | ................ |
000000b0: fd ff ff ef ff ff 7f ff  fb ff ff df ff fe ff ff | ................ |
000000c0: f7 ff ff bf ff fd ff ff  ef ff ff 7f ff fb ff ff | ................ |
000000d0: df ff fe ff ff f7 ff ff  bf ff fd ff ff ef ff ff | ................ |
000000e0: 7f ff fb ff ff df ff fe  ff ff f7 ff ff bf ff fd | ................ |
000000f0: ff ff ef ff ff 7f ff fb  ff ff df ff fe ff ff f7 | ................ |
*/

    for (;;)
    {
	// int	reclen;
	int	rectype;
	// int	w, h, comp, stripe;
	int	datatype;
	unsigned char	data[512];
	int	val;

	rectype = fgetc(fp);
	if (rectype == EOF)
	    break;

	proff(curOff);
	printf("RECTYPE '%c' [0x%x]:\n", rectype, rectype);
	curOff++;

	switch (rectype)
	{
	case 0x41:
	    for (;;)
	    {
		datatype = payload(fp, &curOff, data, &val);
		if (datatype == 0x83)
		    break;
	    }
	    break;
	case 0x43:
	    for (;;)
	    {
		datatype = payload(fp, &curOff, data, &val);
		if (datatype == 0xA2)
		    break;
	    }
	    break;
	case 0x51:
	    break;
	case 0x52:
	    for (;;)
	    {
		datatype = payload(fp, &curOff, data, &val);
		if (datatype == 0xA4)
		{
		    FILE *ofp = NULL;

		    if (Debug > 1)
			ofp = fopen("/tmp/hblp.out", "w");
		    while (val--)
		    {
			int c;

			c = fgetc(fp); curOff++;
			if (ofp)
			    putc(c, ofp);
		    }
		    if (ofp)
			fclose(ofp);
		    break;
		}
	    }
	    break;
	case 0x20:
	    break;
	case 0x53:
	    break;
	case 0x42:
	    goto done;
	case 0x44:
	    break;
	default:
	    error(1, "Unknown rectype 0x%x at 0x%x(%d)\n",
			rectype, curOff, curOff);
	    break;
	}

    }

done:
    c = fgetc(fp);
    if (c != 033)
	return;
    ungetc(c, fp);

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
	if (strcmp(buf, "@PJL ENTER LANGUAGE=HBPL\n") == 0)
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

    if (argc > 0)
    {
        FILE	*fp;

        fp = fopen(argv[0], "r");
        if (!fp)
	error(1, "file '%s' doesn't exist\n", argv[0]);
        decode(fp);
        fclose(fp);
    }
    else
	decode(stdin);

    exit(0);
}
