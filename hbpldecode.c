/*
 * $Id: hbpldecode.c,v 1.48 2014/01/24 19:36:32 rick Exp $
 */

/*b
 * Copyright (C) 2011-2012  Rick Richardson, peter
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
 * Authors: Rick Richardson <rick.richardson@comcast.net>
 * 	    Peter Korf <peter@niendo.de>
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
"	hbpldecode [options] < hbpl-file\n"
"\n"
"	Decode a HBPL stream into human readable form.  HBPL is\n"
"	Host Based Printer Language.\n"
"\n"
"	There are two versions of HBPL in existence.\n"
"\n"
"	Version one is an HBPL stream with Huffman RLE data. This data is\n"
"	used by the Dell 1250c, Dell C1660w, Epson AcuLaser C1700, Fuji-Xerox\n"
"	cp105b, and similar printers. These printers are unsupported.\n"
"\n"
"	Version two is an HBPL stream with JBIG encoded data. This data\n"
"	is used by the Xerox WorkCentre 6015, Fuji Xerox Docuprint CM205\n"
"	and the Dell 1355c. These printers are supported by foo2hbpl2-wrapper\n"
"	et al.\n"
"\n"
"	Both versions can be decoded by hbpldecode.\n"
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

/*
 * Hexdump stream data
 */
void
hexdump(FILE *fp, int decmode, char *lbl1, char *lbln, \
	    const void *vdata, int length)
{
	int			s;
	int			i;
	int			n;
	unsigned char		c;
	unsigned char		buf[16];
	const unsigned char	*data = vdata;

	if (length == 0)
	{
		fprintf(fp, "%s [length 0]\n", lbl1);
		return;
	}
	for (s = 0; s < length; s += 16)
	{
		fprintf(fp, "%s", s ? lbln : lbl1);
		fprintf(fp, decmode ? "%8d:" : "%08x:", s);
		n = length - s; if (n > 16) n = 16;
		for (i = 0; i < 16; ++i)
		{
			if (i == 8)
				fprintf(fp, " ");
			if (i < n)
				fprintf(fp, decmode ? " %3d" : " %02x",
				    buf[i] = data[s+i]);
			else
				fprintf(fp, "   ");
		}
		if (!decmode)
		{
		    fprintf(fp, "  ");
		    for (i = 0; i < n; ++i)
		    {
			    if (i == 8)
				    fprintf(fp, " ");
			    c = buf[i];
			    if (c >= ' ' && c < 0x7f)
				    fprintf(fp, "%c", c);
			    else
				    fprintf(fp, ".");
		    }
		}
		fprintf(fp, "\n");
	}
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
        printf("%d:     ", curOff);
    else if (PrintHexOffset)
        printf("%6x:    ", curOff);
}

/*
 * Version 2 stuff
 */
void
decode_image(char *filename, int pagenum, int planenum,
		unsigned char *bih, unsigned char *jbig, int jbiglen)
{
    FILE			*dfp;
    struct jbg_dec_state        s;
    size_t			cnt;
    unsigned char		*image;
    char			buf[512];
    int				rc;

    jbg_dec_init(&s);
    rc = jbg_dec_in(&s, bih, 20, &cnt);
    if (rc == JBG_EIMPL)
	error(1, "JBIG uses unimplemented feature\n");
    rc = jbg_dec_in(&s, jbig, jbiglen, &cnt);
    if (rc == JBG_EOK)
    {
	int	h, w, len;
	h = jbg_dec_getheight(&s);
	w = jbg_dec_getwidth(&s);
	image = jbg_dec_getimage(&s, 0);
	len = jbg_dec_getsize(&s);
	if (image)
	{
	    sprintf(buf, "%s-%02d-%d.pbm",
		    filename, pagenum, planenum);
	    dfp = fopen(buf, "w");
	    if (dfp)
	    {
		fprintf(dfp, "P4\n%8d %8d\n", w, h);
		rc = fwrite(image, 1, len, dfp);
		fclose(dfp);
		dfp = NULL;
	    }
	}
	else
	    debug(0, "Missing image %dx%d!\n", h, w);
    }
    jbg_dec_free(&s);
}

void
decode2(FILE *fp, int curOff)
{
    // int		c;
    int		rc;
    // FILE	*dfp = NULL;
    int		pageNum = 1;
    int		len;
    // int		curOff = 0;
    // struct jbg_dec_state	s[5];
    // unsigned char	bih[4][20];
    // int			imageCnt[4] = {0,0,0,0};
    // int         	pn = 0;
    unsigned char	header[4];
    unsigned char	buf[512];
    int		w, h, wh_total, res, color, mediatype, papersize;
    int		p, offbih[4];
    #define STRARY(X, A) \
	((X) >= 0 && (X) < sizeof(A)/sizeof(A[0])) \
	? A[X] : "UNK"
    char *strsize[] = {
	/*00*/	"unk", "A4", "B5", "unk", "Letter",
	/*05*/	"Executive", "FanFoldGermanLegal", "Legal", "unk", "env#10",
	/*10*/	"envMonarch", "envC5", "envDL", "unk", "unk",
	};

    char *strtype[] = {
	/*00*/	"unk", "Plain", "Bond", "LwCard", "LwGCard",
	/*05*/	"Labels", "Envelope", "Recycled", "Plain-side2", "Bond-side2",
	/*10*/	"LwCard-side2", "LwGCard-side2", "Recycled-side2",
	};

    for (;;)
    {
	len = 4;
	rc = fread(header, 1, len, fp);
	if (rc != len)
	{
	    error(1, "len=%d, but EOF on file\n", len);
	    return;
	}

	proff(curOff);
	
	if (header[1] == '%' && header[2] == '-') //end of file
	  len = 15;
	else
	{
	    if (header[1] == 'J' && header[2] == 'P')
		len = 60;	// JP doesn't have len
	    else 
		len = header[3];
	    printf("RECTYPE %c%c - size=%d ", header[1], header[2], len);
	}
	
	curOff += len+4;
	rc = fread(buf, 1, len, fp);
	if (rc != len)
	{
	    error(1, "len=%d, but EOF on file\n", len);
	    return;
	}

	if (0) {}
	else if (header[1] == '%' && header[2] == '-')
	{ 
	    buf[len] = 0x00;
	    printf("\\033%%-%c%s", header[3], buf);
	    return;
	} 
	else if (header[1] == 'J' && header[2] == 'P')
	{
	    printf("[Job Parameters]\n");

	    hexdump(stdout, 0, "", "", buf, len);
	    hexdump(stdout, 1, "", "", buf, len);
	    printf("\t\tsize/source(?) = %d(0x%02x)\n", buf[11], buf[11]);
	}
	else if (header[1] == 'D' && header[2] == 'M')
	{
	    printf("[DM]\n");
	    hexdump(stdout, 0, "", "", buf, len);
	}
	
	else if (header[1] == 'P' && header[2] == 'S')
	{
	    unsigned char	*mbuf;

	    printf("[Page Start]\n");
	    if (Debug)
		hexdump(stdout, 0, "", "", buf, len);
	    w = getLEdword(&buf[0]);
	    h = getLEdword(&buf[4]);
	    wh_total = getLEdword(&buf[8]);
	    res = getLEword(&buf[20]);
	    papersize = buf[16];
	    mediatype = buf[17];
	    color = buf[18];
	    printf("\t\tw,h = %dx%d, wh_total = %d, res = %d, color = %d\n",
		w, h, wh_total, res, color);
	    printf("\t\tmediatype = %s(%d), papersize = %s(%d)\n",
		STRARY(mediatype, strtype), mediatype,
		STRARY(papersize, strsize), papersize);

	    for (p = 0; p < 4; ++p)
	    {
		// offsets at 26, 30, 34, 38
		offbih[p] = getLEdword(&buf[22 + p*4]);
		printf("\t\toffbih[%d] = %d (0x%x)\n", p, offbih[p], offbih[p]);
	    }

	    len = getLEdword(&buf[12]);
	    mbuf = malloc(len);
	    if (!mbuf)
		error(1, "malloc on mbuf, size=%d failed\n", len);
	    rc = fread(mbuf, 1, len, fp);
	    if (rc != len)
	    {
		error(1, "len=%d, but EOF on file\n");
	    }
	    if (Debug > 2) hexdump(stdout, 0, "", "", mbuf, len);
	    if (color == 1)
	    {
		proff(curOff);
		printf("Yellow BIH:\n");
		print_bih(mbuf);
		printf("\t\t...yellow data skipped...\n");
		decode_image(DecFile, pageNum, 3,
		    mbuf, mbuf+20, offbih[0]-20);

		proff(curOff + offbih[0]);
		printf("Magenta BIH:\n");
		print_bih(mbuf + offbih[0]);
		printf("\t\t...magenta data skipped...\n");
		decode_image(DecFile, pageNum, 2,
		    mbuf, mbuf+20+offbih[0], offbih[1]-20);

		proff(curOff + offbih[0] + offbih[1]);
		printf("Cyan BIH:\n");
		print_bih(mbuf + offbih[0] + offbih[1]);
		printf("\t\t...cyan data skipped...\n");
		decode_image(DecFile, pageNum, 1,
		    mbuf, mbuf+20+offbih[0]+offbih[1], offbih[2]-20);

		proff(curOff + offbih[0] + offbih[1] + offbih[2]);
		printf("Black BIH:\n");
		print_bih(mbuf + offbih[0] + offbih[1] + offbih[2]);
		printf("\t\t...black data skipped...\n");
		decode_image(DecFile, pageNum, 4,
		    mbuf, mbuf+20+offbih[0]+offbih[1]+offbih[2], offbih[3]-20);
	    }
	    else
	    {
		proff(curOff);
		printf("Black BIH:\n");
		// hexdump(stdout, 0, "", "", &bih[0], 20);
		print_bih(mbuf);
		printf("\t\t...black data skipped...\n");
		decode_image(DecFile, pageNum, 0, mbuf, mbuf+20, offbih[3]-20);
	    }
	    free(mbuf);
	    curOff += len;
	    ++pageNum;
	}
	else if (header[1] == 'P' && header[2] == 'E')
	{
	    printf("[Page End]\n");
	    hexdump(stdout, 0, "", "", buf, len);
	}
	else
	{
	    printf("[Unknown]\n");
	    hexdump(stdout, 0, "", "", buf, len);
	}
    }
}

/*
 * Version 1 stuff
 */
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
	printf("	%x %x: %dx%d (0x%x x 0x%x) [WxH]\n",
	    type, subtype, *vp, val2, *vp, val2);
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
    int		len;
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
	    if ((strcmp(buf, "@PJL ENTER LANGUAGE=HBPL\r\n") == 0)
		|| (strcmp(buf, "@PJL ENTER LANGUAGE=HBPL\n") == 0))
		break;
	}
    }

    c = getc(fp);
    ungetc(c, fp);
    if (c == 0x1b)
    {
	// Decode version 2, ESC based
	decode2(fp, curOff);
	goto done;
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
	    {
		printf("Unknown rectype 0x%x at 0x%x(%d)\n",
			    rectype, curOff, curOff);
		printf("Continuing with hexdump...\n");
		ungetc(rectype, fp);
		if ( (len = fread(buf, 1, sizeof(buf), fp)) )
		hexdump(stdout, 0, "", "", buf, len);
		exit(1);
	    }
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
