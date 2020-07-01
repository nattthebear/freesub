/*
SDL_bdf - renders BDF fonts
Copyright (C) 2002-2003 Andre de Leiradella

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

For information about SDL_bdf contact leiradella@bigfoot.com

Version 1.0: first public release.
Version 1.1: removed SDL dependecies, now SDL_bdf can be used with any graphics
	     library.
Version 1.2: fixed BDF_SizeH and BDF_SizeEntitiesH to return the correct sizes.
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
#include "SDL_bdf.h"

/*
BDF fonts are encoded in ascii. Each line of the file begins with a keyword,
has zero or more arguments which can be integers, numbers (floats), strings and
quoted string, and ends with an eol. Some keywords are optional or
user-defined, unquoted strings can begin with any character which means that it
can start with a minus sign making it hard to distinguish them from negative
integers or numbers (and it do happen in some places). Adobe's spec isn't clear
sometimes so we have to be cautious (eg it doesn't say how eols are encoded so
we have to accept MSDOS (cr lf), Unix (lf) and Mac (cr) styles. I implemented a
*very* relaxed parser which won't stop on some errors. The parser reads the
font line by line and for each one verifies which is the keyword (using a hash
table generated by gperf) and takes the appropriate action through a switch
statement.
*/

/* BDF keywords. */
#define BBX		1
#define BITMAP		2
#define CHARS		3
#define COMMENT 	4
#define CONTENTVERSION	5
#define DWIDTH		6
#define DWIDTH1 	7
#define ENCODING	8
#define ENDCHAR 	9
#define ENDFONT 	10
#define ENDPROPERTIES	11
#define FONT		12
#define FONTBOUNDINGBOX 13
#define METRICSSET	14
#define SIZE		15
#define STARTCHAR	16
#define STARTFONT	17
#define STARTPROPERTIES 18
#define SWIDTH		19
#define SWIDTH1 	20
#define VVECTOR 	21

/* Include GPERF hash function generated from bdf.in. */
/* hash */

/* ANSI-C code produced by gperf version 2.7 */
/* Command-line: gperf -c -C -l -L ANSI-C -f 0 -G -t bdf.in  */
struct bdfword { char *name; int code; };

#define TOTAL_KEYWORDS 21
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 15
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 37
/* maximum key range = 35, duplicates = 0 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash (register const char *str, register unsigned int len)
{
	static const unsigned char asso_values[] =
	{
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38,  5,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38,  0, 20,  5,  0,
		15,  0,  0, 38, 38, 38, 38,  0,  0, 38,
		10, 38, 15,  0,  0, 38, 15, 38,  0, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
		38, 38, 38, 38, 38, 38
	};
	return len + asso_values[(unsigned char)str[len - 1]] + asso_values[(unsigned char)str[0]];
}

static const unsigned char lengthtable[] =
{
0,  0,  0,  3,  4,  0,  6,  7,  8,  9, 10,  6,  7, 13,
0, 15,  6,  7,  0,  4,  0,  0,  7,  0,  9,  5,  0,  7,
0,  0, 15,  0,  0,  0, 14,  0,  0,  7
};

static const struct bdfword wordlist[] =
{
{""}, {""}, {""},
{"BBX",BBX},
{"SIZE",SIZE},
{""},
{"SWIDTH",SWIDTH},
{"ENDFONT",ENDFONT},
{"ENCODING",ENCODING},
{"STARTFONT",STARTFONT},
{"METRICSSET",METRICSSET},
{"DWIDTH",DWIDTH},
{"SWIDTH1",SWIDTH1},
{"ENDPROPERTIES",ENDPROPERTIES},
{""},
{"STARTPROPERTIES",STARTPROPERTIES},
{"BITMAP",BITMAP},
{"DWIDTH1",DWIDTH1},
{""},
{"FONT",FONT},
{""}, {""},
{"ENDCHAR",ENDCHAR},
{""},
{"STARTCHAR",STARTCHAR},
{"CHARS",CHARS},
{""},
{"COMMENT",COMMENT},
{""}, {""},
{"FONTBOUNDINGBOX",FONTBOUNDINGBOX},
{""}, {""}, {""},
{"CONTENTVERSION",CONTENTVERSION},
{""}, {""},
{"VVECTOR",VVECTOR}
};

#ifdef __GNUC__
#if 0
__inline
#endif
#endif
const struct bdfword *
in_word_set (register const char *str, register unsigned int len)
{
	if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
	{
		register int key = hash (str, len);
		
		if (key <= MAX_HASH_VALUE && key >= 0)
			if (len == lengthtable[key])
			{
				register const char *s = wordlist[key].name;
				
				if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
					return &wordlist[key];
			}
    }
	return 0;
}


/* /hash */

/* Reads a line from rwops up to 65536 characters. */
static int readline(BDF_ReadByte getbyte, void *info, char *data) {
	int k, i = 65536;

	for (;;) {
		k = getbyte(info);
		if (k == -1)
			return 0;
		switch (k) {
			default:
				*data++=k;
				if (--i==0) {
			case '\n':
					*data='\0';
					return 1;
				}
			case '\r':
				;
		}
	}
}

/* Parse an integer updating the pointer. */
static int readinteger(char **data, int *integer) {
	char *aux;
	int  i, signal;

	aux = *data;
	signal = 1;
	/* Adobe's spec doesn't say numbers can be preceeded by '+' but we never know. */
	switch (*aux) {
		case '-':
			signal = -1;
		case '+':
			aux++;
			break;
	}
	/* Skip spaces between signal and first digit. */
	while (isspace(*aux)) aux++;
	if (!isdigit(*aux)) return 0;
	/* Now we start reading digits. */
	i = 0;
	while (isdigit(*aux)) i = i * 10 + *aux++ - '0';
	/* We're done, update pointer and return value. */
	*data = aux;
	if (integer != NULL) *integer = signal * i;
	return 1;
}

/* Parse a double updating the pointer. */
static int readnumber(char **data, double *number) {
	register char *aux;
	double	      n, d;
	int	      signal;

	aux = *data;
	signal = 1;
	/* Adobe's spec doesn't say numbers can be preceeded by '+' but we never know. */
	switch (*aux) {
		case '-':
			signal = -1;
		case '+':
			aux++;
			break;
	}
	/* Skip spaces between signal and first digit. */
	while (isspace(*aux)) aux++;
	if (!isdigit(*aux)) return 0;
	/* Now we start reading digits */
	n = 0;
	while (isdigit(*aux)) n = n * 10 + *aux++ - '0';
	d = 10;
	/* If next character is a dot then we have a decimal part. */
	if (*aux == '.') {
		aux++;
		/* Decimal point must be succeeded by one or more digits. */
		if (!isdigit(*aux)) return 0;
		while (isdigit(*aux)) n += (*aux++ - '0') / d, d /= 10;
	}
	/* We're done, update pointer and return value. */
	*data = aux;
	if (number != NULL) *number = signal*n;
	return 1;
}

/* Parse a string updating the pointer. */
static int readstring(char **data, char **string) {
	int len;

	len = strlen(*data);
	if (string != NULL) {
		/* Malloc the required space. */
		*string=(char *)malloc(len + 1);
		if (*string == NULL)
			return 0;
		/* Copy everything. */
		strcpy(*string, *data);
	}
	/* We're done, update pointer. */
	*data += len;
	return 1;
}

/* Scan the line (just after the keyword) for elements listed in format. */
static int scan(char *data, char *format, ...) {
	va_list args;
	int	*i;
	double	*n;
	char	**s;

	/* Keyword already skipped, skip spaces. */
	while (*data != '\0' && isspace(*data)) data++;
	/* Scan the data for the pattern in format. */
	va_start(args, format);
	while (*format != '\0') {
		switch (*format++) {
			case 'i': /* integer. */
				i = va_arg(args, int *);
				if (!readinteger(&data, i)) {
					va_end(args);
					return 0;
				}
				break;
			case 'n': /* number. */
				n = va_arg(args, double *);
				if (!readnumber(&data, n)) {
					va_end(args);
					return 0;
				}
				break;
			case 's': /* string. */
				s = va_arg(args, char **);
				if (!readstring(&data, s)) {
					va_end(args);
					return 0;
				}
				break;
		}
		/* Skip spaces between elements. */
		while (*data != '\0' && isspace(*data)) data++;
	}
	va_end(args);
	return *data == '\0';
}

/* Compare function to sort characters by their names. */
static int compare(const void *c1, const void *c2) {
	return strcmp(((BDF_Char *)c1)->name, ((BDF_Char *)c2)->name);
}

#define XVAL(x) (isdigit((x)) ? (x) - '0' : toupper((x)) - 'A' + 10)

BDF_Font *BDF_OpenFont(BDF_ReadByte getbyte, void *info, int *error) {
	BDF_Font       *font;
	BDF_Char       *chr;
	char	       *data;
	register char  *aux;
	struct bdfword *word;
	unsigned char  *bits;
	double	       n;
	int	       numChars;
	int	       dwx0, dwy0;
	int	       dwx1, dwy1;
	int	       bbw, bbh, bbxoff0x, bbyoff0y, i;

	/* Malloc the font. */
	font = (BDF_Font *)malloc(sizeof(BDF_Font));
	if (font == NULL) {
		if (error != NULL) *error = BDF_MEMORYERROR;
		goto error;
	}
	/* Null array of characters. */
	font->chars = NULL;
	/* Looks ugly but I'm lazy... */
	data = (char *)malloc(65537 * sizeof(char));
	if (data == NULL) {
		if (error != NULL) *error = BDF_MEMORYERROR;
		goto error;
	}
	/* Zero the structure. */
	font->metricsSet = font->numChars = 0;
	dwx0 = dwy0 = 0;
	dwx1 = dwy1 = 0;
	bbw = bbh = 0;
	bbxoff0x = bbyoff0y = 0;
	/* chr holds the current character or NULL if we're not inside a character definition. */
	chr = NULL;
	for (;;) {
		/* Read one line at a time. */
		if (!readline(getbyte, info, data)) {
			if (error != NULL) *error = BDF_READERROR;
			goto error;
		}
		/* Find end of keyword. */
		aux = data;
		while (*aux != '\0' && !isspace(*aux)) aux++;
		/* Find which keyword it is using gperf's hash function. */
		word = (struct bdfword *)in_word_set(data, aux - data);
		switch (word == NULL ? 0 : word->code) {
			case STARTFONT:
				/* Issue an error on versions higher than 2.2. */
				if (!scan(aux, "n", &n)) {
					if (error != NULL) *error = BDF_PARSEERROR;
					goto error;
				}
				if (n > 2.2) {
					if (error != NULL) *error = BDF_WRONGVERSION;
					goto error;
				}
				break;
			case FONTBOUNDINGBOX:
				/* The FONTBOUNDINGBOX values seems to be defaults for BBX values. */
				if (!scan(aux, "iiii", &bbw, &bbh, &bbxoff0x, &bbyoff0y)) {
					if (error != NULL) *error = BDF_PARSEERROR;
					goto error;
				}
				break;
			case METRICSSET:
				/* We only handle horizontal writing by now. */
				if (!scan(aux, "i", &font->metricsSet)) {
					if (error != NULL) *error = BDF_PARSEERROR;
					goto error;
				}
				if (font->metricsSet != 0) {
					if (error != NULL) *error = BDF_CANNOTHANDLEVERTICAL;
					goto error;
				}
				break;
			case DWIDTH:
				/* This is the character's width in pixels. */
				if (chr != NULL) {
					if (!scan(aux, "ii", &chr->dwx0, &chr->dwy0)) {
						if (error != NULL) *error = BDF_PARSEERROR;
						goto error;
					}
                }
				else {
					if (!scan(aux, "ii", &dwx0, &dwy0)) {
						if (error != NULL) *error = BDF_PARSEERROR;
						goto error;
					}
                }
				break;
			case DWIDTH1:
				/* This is the character's width in pixels for vertical writing. */
				if (chr != NULL) {
					if (!scan(aux, "ii", &chr->dwx1, &chr->dwy1)) {
						if (error != NULL) *error = BDF_PARSEERROR;
						goto error;
					}
                }
				else {
					if (!scan(aux, "ii", &dwx1, &dwy1)) {
						if (error != NULL) *error = BDF_PARSEERROR;
						goto error;
					}
                }
				break;
			case CHARS:
				/* We read the number of chars in this font and malloc the required memory. */
				if (!scan(aux, "i", &font->numChars)) {
					if (error != NULL) *error = BDF_PARSEERROR;
					goto error;
				}
				font->chars = (BDF_Char *)malloc(font->numChars * sizeof(BDF_Char));
				if (font->chars == NULL) {
					if (error != NULL) *error = BDF_MEMORYERROR;
					goto error;
				}
				/* Zero all characters' info. */
				for (i = font->numChars, chr = font->chars; i != 0; i--, chr++) {
					chr->name = NULL;
					chr->code = -1;
					chr->dwx0 = chr->dwy0 = 0;
					chr->dwx1 = chr->dwy1 = 0;
					chr->bbw = chr->bbh = 0;
					chr->bbxoff0x = chr->bbyoff0y = 0;
					chr->bits = NULL;
				}
				/* chr points to the current character. */
				chr = font->chars;
				break;
			case STARTCHAR:
				/* If chr is NULL there are more characters in the font then expected. */
				if (chr == NULL) {
					if (error != NULL) *error = BDF_TOOMANYCHARACTERS;
					goto error;
				}
				if (!scan(aux, "s", &chr->name)) {
					if (error != NULL) *error = BDF_PARSEERROR;
					goto error;
				}
				/* Copy default values. */
				chr->code = -1;
				chr->dwx0 = dwx0;
				chr->dwy0 = dwy0;
				chr->dwx1 = dwx1;
				chr->dwy1 = dwy1;
				chr->bbw = bbw;
				chr->bbh = bbh;
				chr->bbxoff0x = bbxoff0x;
				chr->bbyoff0y = bbyoff0y;
				break;
			case ENCODING:
				/* Read character's code, it can be -1. */
				if (chr != NULL)
					if (!scan(aux, "i", &chr->code)) {
						if (error != NULL) *error = BDF_PARSEERROR;
						goto error;
					}
				break;
			case BBX:
				/* The bounding box around the character's black pixels. */
				if (chr != NULL)
					if (!scan(aux, "iiii", &chr->bbw, &chr->bbh, &chr->bbxoff0x, &chr->bbyoff0y)) {
						if (error != NULL) *error = BDF_PARSEERROR;
						goto error;
					}
				break;
			case BITMAP:
				/* BITMAP signals the start of the hex data. */
				if (chr != NULL) {
					/* wbytes is the width of the char in bytes. */
					chr->wbytes = (chr->bbw + 7) / 8;
					/* Malloc the memory for the pixels. */
					chr->bits = (unsigned char *)malloc(chr->wbytes * chr->bbh);
					if (chr->bits == NULL) {
						if (error != NULL) *error = BDF_MEMORYERROR;
						goto error;
					}
					/* Read all pixels from file. */
					for (i = chr->bbh, bits = chr->bits; i != 0; i--) {
						if (!readline(getbyte, info, data)) {
							if (error != NULL) *error = BDF_READERROR;
							goto error;
						}
						aux = data;
						while (aux[0] != '\0' && aux[1] != '\0') {
							*bits++ = XVAL(aux[0]) * 16 + XVAL(aux[1]);
							aux += 2;
						}
					}
				}
				break;
			case ENDCHAR:
				/* Skip to the next character, makes a bound check. */
				chr++;
				if ((chr-font->chars) >= font->numChars)
					chr = NULL;
				break;
			case ENDFONT:
				/* Ends the font, if chr is not NULL then we are short on characters. */
				if (chr != NULL) {
					if (error != NULL) *error = BDF_TOOFEWCHARACTERS;
					goto error;
				}
				/* Sort font by character names, should be an hash table. */
				qsort(font->chars, font->numChars, sizeof(BDF_Char), compare);
				/* Fast pointers to characters encoded between [0..255]. */
				for (i = 0; i < 256; i++)
					font->code[i] = NULL;
				for (i = font->numChars, chr = font->chars; i != 0; i--, chr++)
					if (chr->code >= 0 && chr->code <= 255)
						font->code[chr->code] = chr;
				if (error != NULL) *error = BDF_OK;
				free(data);
				return font;
		}
	}
	error:
	/* Free everything. */
	free(data);
	BDF_CloseFont(font);
	return NULL;
}

void BDF_CloseFont(BDF_Font *font) {
	int	 i;
	BDF_Char *chr;

	/* Free everything. */
	if (font != NULL) {
		if (font->chars != NULL) {
			for (i = font->numChars, chr = font->chars; i != 0; i--, chr++) {
				free(chr->name);
				free(chr->bits);
			}
			free(font->chars);
		}
		free(font);
	}
}

/* Finds a char in the font, if entities is not zero then handle entities. */
static BDF_Char *findchar(BDF_Font *font, const char **text, int entities) {
	char	 *aux;
	BDF_Char key, *chr;

	/* Handle entities. */
	if (entities != 0 && **text == '&') {
		if ((*text)[1] != '&') {
			key.name = *text + 1;
			aux = strchr(*text, ';');
			if (aux == NULL) {
				*text = *text + strlen(*text);
				return NULL;
			}
			*aux = '\0';
			*text = aux + 1;
			chr = (BDF_Char *)bsearch(&key, font->chars, font->numChars, sizeof(BDF_Char), compare);
			*aux = ';';
			return chr;
		} else
			(*text)++;
	}
	/* Return the character in the range [0..255]. */
	return font->code[*(unsigned char *)(*text)++];
}

/* Determines the size of the horizontal text. */
static void sizeh(BDF_Font *font, const char *text, int entities, int *x0, int *y0, int *width, int *height) {
	BDF_Char *chr;
	int	 first, y, h, minh, maxh;

	first = 1;
	minh = *y0 = INT_MAX;
	maxh = INT_MIN;
	y = 0;
	while (*text != '\0') {
		chr = findchar(font, &text, entities);
		if (first != 0) {
			first = 0;
			*x0 = *width = -chr->bbxoff0x;
		}
		if (chr != NULL) {
			h = y - (chr->bbyoff0y + chr->bbh);
			if (h < minh)
				minh = h;
			h += chr->bbh - 1;
			if (h > maxh)
				maxh = h;
			*width += chr->dwx0;
			if (chr->bbyoff0y < *y0)
				*y0 = chr->bbyoff0y;
			y += chr->dwy0;
		}
	}
	*height = maxh - minh + 1;
	*y0 += *height;
}

void BDF_SizeH(BDF_Font *font, const char *text, int *x0, int *y0, int *width, int *height) {
	int _x0, _y0, _width, _height;

	sizeh(font, text, 0, &_x0, &_y0, &_width, &_height);
	if (x0 != NULL) *x0 = _x0;
	if (y0 != NULL) *y0 = _y0;
	if (width != NULL) *width = _width;
	if (height != NULL) *height = _height;
}

void BDF_SizeEntitiesH(BDF_Font *font, const char *text, int *x0, int *y0, int *width, int *height) {
	int _x0, _y0, _width, _height;

	sizeh(font, text, 1, &_x0, &_y0, &_width, &_height);
	if (x0 != NULL) *x0 = _x0;
	if (y0 != NULL) *y0 = _y0;
	if (width != NULL) *width = _width;
	if (height != NULL) *height = _height;
}

/* Draws a char on the surface. */
static void drawchar(void *surface, BDF_PutPixel putpixel, BDF_Char *chr, int x, int y, unsigned int color) {
	int	      xx;
	unsigned char *bits, *endfont, *endline;

	/* Calculate the position of the first pixel. */
	x += chr->bbxoff0x;
	y -= (chr->bbyoff0y + chr->bbh);
	bits = chr->bits;
	/* Put them! */
	for (endfont = bits + chr->wbytes * chr->bbh; bits < endfont; y++)
		for (endline = bits + chr->wbytes, xx = x; bits < endline; xx += 8, bits++) {
			if ((*bits) & 0x80) putpixel(surface, xx, y, color);
			if ((*bits) & 0x40) putpixel(surface, xx + 1, y, color);
			if ((*bits) & 0x20) putpixel(surface, xx + 2, y, color);
			if ((*bits) & 0x10) putpixel(surface, xx + 3, y, color);
			if ((*bits) & 0x08) putpixel(surface, xx + 4, y, color);
			if ((*bits) & 0x04) putpixel(surface, xx + 5, y, color);
			if ((*bits) & 0x02) putpixel(surface, xx + 6, y, color);
			if ((*bits) & 0x01) putpixel(surface, xx + 7, y, color);
		}
}

/* Draws an entire line of text. */
static int drawh(void *surface, BDF_PutPixel putpixel, BDF_Font *font, const char *text, int entities, int x, int y, unsigned int color) {
	BDF_Char *chr;

	/* For each character... */
	while (*text != '\0') {
		chr = findchar(font, &text, entities);
		if (chr != NULL) {
			/* ... draw it. */
			drawchar(surface, putpixel, chr, x, y, color);
			x += chr->dwx0;
			y += chr->dwy0;
		}
	}
	return x;
}

int BDF_DrawH(void *surface, BDF_PutPixel putpixel, BDF_Font *font, const char *text, int x, int y, unsigned int color) {
	return drawh(surface, putpixel, font, text, 0, x, y, color);
}

int BDF_DrawEntitiesH(void *surface, BDF_PutPixel putpixel, BDF_Font *font, const char *text, int x, int y,unsigned int color) {
	return drawh(surface, putpixel, font, text, 1, x, y, color);
}




void BDF_SizeML (BDF_Font *font, const char *text, int *width, int *height, int lsp)
{
  int i;
  int numlines = 1;
  char *text_s = malloc (strlen (text) + 1);
  strcpy (text_s, text);

  for (i = 0; text_s[i]; i++)
  {
    if (text_s[i] == '\\')
    {
      if (text_s[i + 1] == 'n' || text_s[i + 1] == '\\')
      {
        if (text_s[i + 1] == 'n')
        {
          text_s[i] = '\n';
          numlines++;
        }
        int j = i + 1;
        while (text_s[j])
        {
          text_s[j] = text_s[j + 1];
          j++;
        }
      }
    }
  }  
  
  char **text_sub = malloc ((numlines + 2) * sizeof (*text_sub));
  int *x0_sub = malloc ((numlines + 2) * sizeof (*x0_sub));
  int *y0_sub = malloc ((numlines + 2) * sizeof (*y0_sub));
  int *width_sub = malloc ((numlines + 2) * sizeof (*width_sub));
  int *height_sub = malloc ((numlines + 2) * sizeof (*height_sub));
  
  char *textstart = text_s;
  char *textcurr = text_s;
  int pos = 0;
  
  
  
  while (*textcurr)
  {
    if (*textcurr == '\n')
    {
      text_sub[pos++] = textstart;
      *textcurr = 0;
      textstart = textcurr + 1;
      textcurr++;
    }
    textcurr++;
  }
  text_sub[pos++] = textstart;
  text_sub[pos] = NULL;
  
  for (i = 0; text_sub[i]; i++)
    BDF_SizeH (font, text_sub[i], x0_sub + i, y0_sub + i, width_sub + i, height_sub + i);

  // find width, height
  int finalwidth = 0;
  int finalheight = 0;
  for (i = 0; text_sub[i]; i++)
  {
    if (width_sub[i] > finalwidth)
      finalwidth = width_sub[i];
    finalheight += height_sub[i];
    if (i < numlines - 1)
      finalheight += lsp;
  }
  
  free (text_s);
  free (x0_sub);
  free (y0_sub);
  free (width_sub);
  free (height_sub);
  if (width)
    *width = finalwidth;
  if (height)
    *height = finalheight;
    
  //char buff[512];
  //sprintf (buff, "nlines %i pos %i width %i height %i", numlines, pos, finalwidth, finalheight);
  //MessageBox (0, buff, "foo", 0);  
    
}
  


void BDF_DrawML (void *surface, BDF_PutPixel putpixel, BDF_Font *font, const char *text, unsigned int color, int align, int lsp)
{
  int i;
  int numlines = 1;
  char *text_s = malloc (strlen (text) + 1);
  strcpy (text_s, text);

  for (i = 0; text_s[i]; i++)
  {
    if (text_s[i] == '\\')
    {
      if (text_s[i + 1] == 'n' || text_s[i + 1] == '\\')
      {
        if (text_s[i + 1] == 'n')
        {
          text_s[i] = '\n';
          numlines++;
        }
        int j = i + 1;
        while (text_s[j])
        {
          text_s[j] = text_s[j + 1];
          j++;
        }
      }
    }
  }  
  
  
  char **text_sub = malloc ((numlines + 2) * sizeof (*text_sub));
  int *x0_sub = malloc ((numlines + 2) * sizeof (*x0_sub));
  int *y0_sub = malloc ((numlines + 2) * sizeof (*y0_sub));
  int *width_sub = malloc ((numlines + 2) * sizeof (*width_sub));
  int *height_sub = malloc ((numlines + 2) * sizeof (*height_sub));
  
  char *textstart = text_s;
  char *textcurr = text_s;
  int pos = 0;
  
  
  
  while (*textcurr)
  {
    if (*textcurr == '\n')
    {
      text_sub[pos++] = textstart;
      *textcurr = 0;
      textstart = textcurr + 1;
      textcurr++;
    }
    textcurr++;
  }
  text_sub[pos++] = textstart;
  text_sub[pos] = NULL;
  
  for (i = 0; text_sub[i]; i++)
    BDF_SizeH (font, text_sub[i], x0_sub + i, y0_sub + i, width_sub + i, height_sub + i);
    
  // find width, height
  int finalwidth = 0;
  int finalheight = 0;
  for (i = 0; text_sub[i]; i++)
  {
    if (width_sub[i] > finalwidth)
      finalwidth = width_sub[i];
    finalheight += height_sub[i];
    if (i < numlines - 1)
      finalheight += lsp;
  }
  
  int currentheightpos = 0;
  // center or right justify as appropriate
  for (i = 0; text_sub[i]; i++)
  {
    switch (align)
    {
      case 1:
      case 4:
      case 7:
        width_sub[i] = 0;
        break;
      case 2:
      case 5:
      case 8:
        width_sub[i] = (finalwidth - width_sub[i]) / 2;
        break;
      case 3:
      case 6:
      case 9:
        width_sub[i] = finalwidth - width_sub[i];
        break;
    }
    currentheightpos += (height_sub[i] + lsp);
    height_sub[i] = currentheightpos - (height_sub[i] + lsp);
  }
  for (i = 0; text_sub[i]; i++)
  {
    BDF_DrawH (surface, putpixel, font, text_sub[i], width_sub[i] + x0_sub[i], height_sub[i] + y0_sub[i], color);
  }
        

  
  free (text_s);
  free (x0_sub);
  free (y0_sub);
  free (width_sub);
  free (height_sub);

}



