/**
 * Graphics utilities: palettes...

 * Copyright (C) 2003  Shawn Betts
 * Copyright (C) 2003, 2004, 2007  Sylvain Beucler

 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with program; see the file COPYING. If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "SDL.h"
#include "SDL_image.h"
#include "io_util.h"
#include "paths.h"
#include "log.h"
#include "dinkvar.h"
#include "dinkini.h"

/* Parse dink.ini */
void load_batch(void)
{
  FILE *in = NULL;
  char line[255];
  
  printf("Loading dink.ini\n");

  /* Open the text file in binary mode, so it's read the same way
     under different OSes (Unix has no text mode) */
  if ((in = paths_dmodfile_fopen("dink.ini", "rb")) == NULL)
    fprintf(stderr, "Error opening dink.ini for reading.\n");
  else
    {
      while(fgets(line, 255, in) != NULL) 
	pre_figure_out(line);
      fclose(in);
    }

  program_idata();
}

/* Get a colors palette from the specified image */
int
load_palette_from_surface (SDL_Surface *bmp, SDL_Color *palette)
{
  int i;

  if (bmp == NULL || bmp->format->palette == NULL)
    return -1;

  for (i = 0; i < bmp->format->palette->ncolors; i++)
    {
      palette[i].r = bmp->format->palette->colors[i].r;
      palette[i].g = bmp->format->palette->colors[i].g;
      palette[i].b = bmp->format->palette->colors[i].b;
    }

  /* Reproduce DX/Woe limitation (see change_screen_palette) */
  palette[0].r = 0;
  palette[0].g = 0;
  palette[0].b = 0;
  palette[255].r = 255;
  palette[255].g = 255;
  palette[255].b = 255;

  return 0;
}

/* Get a colors palette from the specified image */
int
load_palette_from_bmp (char *filename, SDL_Color *palette)
{
  SDL_Surface *bmp;
  int success = -1;
  char *fullpath = NULL;

  fullpath = paths_dmodfile(filename);
  bmp = IMG_Load(fullpath);
  free(fullpath);
  if (bmp == NULL)
    {
      fullpath = paths_fallbackfile(filename);
      bmp = IMG_Load(fullpath);
      free(fullpath);
      if (bmp == NULL)
	{
	  fprintf(stderr, "load_palette_from_bmp: couldn't open %s\n", filename);
	  return -1;
	}
    }

  success = load_palette_from_surface(bmp, palette);
  SDL_FreeSurface(bmp);
  return success;
}


/* Generate a default/fallback 216 colors palette */
void
setup_palette(SDL_Color *palette)
{
  int i = 0;
  int r, g, b;

  /* Set a 3,3,2 color cube */
  for (r = 0; r < 256; r += 0x33)
    {
      for (g = 0; g < 256; g += 0x33)
	{
	  for (b = 0; b < 256; b += 0x33)
	    {
	      palette[i].r = r;
	      palette[i].g = g;
	      palette[i].b = b;
	      i++;
	    }
	}
    }

  /* Set the rest of the colors to black */
  for (; i < 256; i++)
    palette[i].r
      = palette[i].g
      = palette[i].b
      = 0;

  /* Reproduce DX/Woe limitation (see change_screen_palette) */
  palette[0].r = 0;
  palette[0].g = 0;
  palette[0].b = 0;
  palette[255].r = 255;
  palette[255].g = 255;
  palette[255].b = 255;
}
