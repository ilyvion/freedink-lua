/**
 * Dumps a BMP file header

 * Copyright (C) 2007  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

/* Format info:
   http://en.wikipedia.org/wiki/Windows_and_OS/2_bitmap */
/* TODO: endianness support for portability */

struct bmp_file_header {
  char signature[2]; /* BM|BA|CI|CP|IC|PT */
  unsigned int file_size;
  unsigned short reserved1;
  unsigned short reserved2;
  unsigned int image_offset;
};

struct bmp_image_header_v3 {
  unsigned int header_size;
  /* 40 	Windows V3 [x]
     12 	OS/2 V1
     64 	OS/2 V2
     108 	Windows V4
     124 	Windows V5 */
  unsigned int width;
  unsigned int height;
  unsigned short nb_color_planes; /* always 1? */
  unsigned short bits_per_pixel; /* 1|4|8|16|24|32 */
  unsigned int compression_method;
  /* 0 	BI_RGB  "none"
     1 	BI_RLE8 "RLE 8-bit/pixel"
     2 	BI_RLE4	"RLE 4-bit/pixel"
     3 	BI_BITFIELDS "Bit field"
     4 	BI_JPEG "JPEG"
     5 	BI_PNG 	"PNG" */
  unsigned int size;
  unsigned int hres;
  unsigned int vres;
  unsigned int nb_colors;
  unsigned int nb_important_colors; /* 0 when all colors are significant */
};

/* only used for BMPs with 256 colors */
struct bmp_palette_entry_v3 {
  unsigned char b;
  unsigned char g;
  unsigned char r;
  unsigned char unused;
};

/* Pixels: stored lines by lines (left->right) from bottom to top; the
   number of bytes in one line is a multiple of 4, padded with \0 is
   necessary. */

int main(int argc, char* argv[]) {
  FILE *file;

  struct bmp_file_header fheader;
  struct bmp_image_header_v3 iheader;

  struct bmp_palette_entry_v3 *palette;
  int i;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s dir.ff\n", argv[0]);
    exit(1);
  }

  file = fopen(argv[1], "rb");

  /* File header */
  /* Problem: there may be padding, that's not portable (and that
     doesn't work for me anyway) */
  /* nb_read = fread(&fheader, sizeof(fheader), 1, file); */
  printf("File header:\n");
  printf("============\n");
  fread(&fheader.signature, sizeof(fheader.signature), 1, file);
  printf("Signature: %c%c\n", fheader.signature[0], fheader.signature[1]);
  fread(&fheader.file_size, sizeof(fheader.file_size), 1, file);
  printf("File size: %u %x\n", fheader.file_size, fheader.file_size);
  fread(&fheader.reserved1, sizeof(fheader.reserved1), 1, file);
  fread(&fheader.reserved2, sizeof(fheader.reserved2), 1, file);
  fread(&fheader.image_offset, sizeof(fheader.image_offset), 1, file);
  printf("Image offset: %d\n", fheader.image_offset);
  printf("\n");

  /* Image header */
  printf("Image header:\n");
  printf("=============\n");
  fread(&iheader.header_size, sizeof(iheader.header_size), 1, file);
  if (iheader.header_size != 0x28)
    {
      fprintf(stderr, "Unsupported header size: %x\n", iheader.header_size);
      exit(1);
    }
  fread(&iheader.width, sizeof(iheader.width), 1, file);
  printf("width = %d\n", iheader.width);
  fread(&iheader.height, sizeof(iheader.height), 1, file);
  printf("height = %d\n", iheader.height);
  fread(&iheader.nb_color_planes, sizeof(iheader.nb_color_planes), 1, file);
  printf("nb_color_planes = %hd\n", iheader.nb_color_planes);
  fread(&iheader.bits_per_pixel, sizeof(iheader.bits_per_pixel), 1, file);
  printf("bits_per_pixel = %hd\n", iheader.bits_per_pixel);
  fread(&iheader.compression_method, sizeof(iheader.compression_method), 1, file);
  printf("compression_method = %d\n", iheader.compression_method);
  fread(&iheader.size, sizeof(iheader.size), 1, file);
  printf("size = %d\n", iheader.size);
  fread(&iheader.hres, sizeof(iheader.hres), 1, file);
  printf("hres = %d\n", iheader.hres);
  fread(&iheader.vres, sizeof(iheader.vres), 1, file);
  printf("vres = %d\n", iheader.vres);
  fread(&iheader.nb_colors, sizeof(iheader.nb_colors), 1, file);
  printf("nb_colors = %d\n", iheader.nb_colors);
  fread(&iheader.nb_important_colors, sizeof(iheader.nb_important_colors), 1, file);
  printf("nb_important_colors = %d\n", iheader.nb_important_colors);
  printf("\n");

  /* Palette */
  printf("Palette:\n");
  printf("========\n");
  printf("Palette location: %ld\n", ftell(file));
  if (iheader.bits_per_pixel == 8)
    {
      palette = malloc(256 * sizeof(struct bmp_palette_entry_v3));
      
      for (i = 0; i < 256; i++)
	{
	  fread(&(palette[i].b), 1, 1, file);
	  fread(&(palette[i].g), 1, 1, file);
	  fread(&(palette[i].r), 1, 1, file);
	  fread(&(palette[i].unused), 1, 1, file);
	  printf("%d - (%d, %d, %d)\n", i, palette[i].r, palette[i].g, palette[i].b);
	}
    }
  return 0;
}
