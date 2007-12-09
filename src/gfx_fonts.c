/**
 * Fonts

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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "SDL.h"
#include "SDL_ttf.h"
#include "dinkvar.h"
#include "gfx.h"
#include "io_util.h"
#include "gfx_fonts.h"


/* Default size was 18 in the original game, but it refers to a
   different part of the font glyph (see doc/fonts.txt for
   details). 16 matches that size with SDL_ttf. */
#define FONT_SIZE 16

/* Default fonts: dialog and system */
static TTF_Font *dialog_font = NULL;
static TTF_Font *system_font = NULL;

/* Current font parameters */
static SDL_Color text_color;

static TTF_Font *load_default_font(char *filename);
static void setup_font(TTF_Font *font);


/**
 * Init font subsystem and load the default fonts
 */
void FONTS_init()
{
  if(TTF_Init()==-1) {
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
    exit(1);
  }

  dialog_font = load_default_font("LiberationSans-Regular.ttf");
  system_font = load_default_font("vgasys.fon");
  if (dialog_font == NULL || system_font == NULL)
    {
      fprintf(stderr, "Failed to load the default fonts\n");
      exit(1);
    }
}

/**
 * Quit the font subsystem (and free loaded fonts from memory)
 */
void kill_fonts(void)
{
  if (dialog_font)
    {
      TTF_CloseFont(dialog_font);
      dialog_font = NULL;
    }
  if (system_font)
    {
      TTF_CloseFont(system_font);
      system_font = NULL;
    }

  TTF_Quit();
}


/**
 * Default font from resources and pkgdatadir
 */
static TTF_Font *load_default_font(char *filename) {
  TTF_Font *font_object = NULL;
  char *first_error = NULL;

  /* Try from resources */
  SDL_RWops* rwops;
  rwops = find_resource_as_rwops(filename);
  if (rwops == NULL)
    {
      /* Error comes from ZZIP, keep it for later if everything
	 fails */
      char *error = strerror(errno);
      first_error = malloc(strlen(error) + 1);
      strcpy(first_error, error);
    }
  else
    {
      font_object = TTF_OpenFontRW(rwops, 1, FONT_SIZE);
      if (font_object == NULL)
	{
	  char *error = TTF_GetError();
	  first_error = malloc(strlen(error) + 1);
	  strcpy(first_error, error);
	}
    }
  
  /* Fallback to package data directory */
  if (font_object == NULL)
    {
      char *path = find_data_file(filename);
      if (path != NULL)
	font_object = TTF_OpenFont(path, FONT_SIZE);
  
      if (font_object == NULL)
	{
	  fprintf(stderr, "Could not find %s\n", filename);
	  fprintf(stderr, "- loading from myself failed with: %s\n", first_error);
	  if (path != NULL)
	    fprintf(stderr, "- loading from %s failed with %s\n", path, TTF_GetError());
	  else
	    fprintf(stderr, "- file was not found in the data dir\n");
	  /* TODO: clean-up before exiting */
	  return NULL;
	}

      if (path != NULL)
	free(path);
    }

  if (first_error != NULL)
    free(first_error);

  if (font_object == NULL)
    return NULL;

  setup_font(font_object);

  return font_object;
}

/**
 * Change the current dialog font (DinkC initfont() command)
 */
int initfont(char* fontname) {
  /* TODO: lf.lfWeight = 600; */
  /* TODO: Load from Woe's system font dir if not found in the current
     directory */

  if (dialog_font != NULL) {
    TTF_CloseFont(dialog_font);
    dialog_font = NULL;
  }

  /* Font from DMod directory */
  if (dialog_font == NULL)
    {
      dialog_font = TTF_OpenFont(fontname, FONT_SIZE);
    }

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
  if (dialog_font == NULL)
    {
      /* Look in %WINDIR%\Fonts */
      char *path = malloc(MAX_PATH + 7 + strlen(fontname) + 1);
      GetWindowsDirectory(path, MAX_PATH);
      strcat(path, "\\Fonts\\");
      strcat(path, fontname);
      dialog_font = TTF_OpenFont(path, FONT_SIZE);
    }
#endif

  if (dialog_font == NULL) {
    printf("TTF_OpenFont: %s\n", TTF_GetError());
    return -1;
  }

  setup_font(dialog_font);

  return 0;
}

/**
 * Apply default style to the font
 * Plus some informative output
 */
static void
setup_font(TTF_Font *font)
{
  printf("font=%p\n", font);

  char *familyname = TTF_FontFaceFamilyName(font);
  if(familyname)
    printf("The family name of the face in the font is: %s\n", familyname);
  char *stylename = TTF_FontFaceStyleName(font);
  if(stylename)
    printf("The name of the face in the font is: %s\n", stylename);
  printf("The font max height is: %d\n", TTF_FontHeight(font));
  printf("The font ascent is: %d\n", TTF_FontAscent(font));
  printf("The font descent is: %d\n", TTF_FontDescent(font));
  printf("The font line skip is: %d\n", TTF_FontLineSkip(font));
  if(TTF_FontFaceIsFixedWidth(font))
    printf("The font is fixed width.\n");
  else
    printf("The font is not fixed width.\n");

  TTF_SetFontStyle(font, TTF_STYLE_BOLD);
}




void FONTS_SetTextColor(Uint8 r, Uint8 g, Uint8 b) {
  text_color.r = r;
  text_color.g = g;
  text_color.b = b;
}


static void
print_text (TTF_Font * font, char *str, int x, int y, int w, SDL_Color /*&*/color,
	    /*bool*/int hcenter)
{
  int new_x, text_w, text_h;
  SDL_Surface *tmp;
  SDL_Rect dst;

  if (strlen (str) == 0)
    return;
  
  /* Msg (("printing \"%s\"", str)); */


  /* Rationale: text color is not affected by palette shifts (yellow
     stays yellow even if we're using the negative palette from Lyna's
     story) _but_ text color is the closest one in the final palette
     color (so when after a fade_down(), the available colors are only
     black and white, yellow becomes white, as yellow is just not
     available, and white is closest than black). */
  /* So we get the color index from cur_screen_palette (final color
     even after palette effects), and use the color value from
     GFX_real_pal (reference palette used for all graphics before
     palette effects). */
  /* Previously this was implemented using BlitSurface palette
     conversion (commit 2007-11-01) but this was terribly slow (full
     palette conversion done 5 times per text (for the border effect)
     and per frame). */
  {
    SDL_PixelFormat fmt;
    SDL_Palette pal = {256, cur_screen_palette};
    Uint32 screen_index;
    fmt.palette = &pal;
    screen_index = SDL_MapRGB(&fmt, color.r, color.g, color.b);
    fmt.palette->colors = GFX_real_pal;
    SDL_GetRGB(screen_index, &fmt, &(color.r), &(color.g), &(color.b));
  }

  /* Transparent, low quality - closest to the original engine. */
  /* Besides, we do need a monochrome render, since we're doing nasty
     tricks to set the color appropriately */
  tmp = TTF_RenderText_Solid(font, str, color);

  /* Bigger, with a box background */
  // SDL_Color background = {0, 0, 0};
  // tmp = TTF_RenderText_Shaded(font, str, color, background);
  
  /* High quality, 32bit+alpha, but I can't get the transparency
     OK. Directly applying it on lpDDSBack messed with the colors of
     the graphics behind the text, and trying to pre-convert it by
     blitting it on a transparent surface missed some of the text
     borders in the final result (i.e. multiple calls to print_text
     with a small postponement to make the borders). */
  // tmp = TTF_RenderText_Blended(font, str, color);
  // {
  //   // SDL_Surface *conv = SDL_DisplayFormat(tmp);
  //   SDL_Surface *conv = SDL_CreateRGBSurface(SDL_SWSURFACE, tmp->w, tmp->h, 8,
  // 			     			0, 0, 0, 0);
  //   SDL_SetPalette(conv, SDL_LOGPAL, GFX_real_pal, 0, 256);
  //   SDL_SetColorKey(conv, SDL_SRCCOLORKEY, 0);
  //   SDL_FillRect(conv, NULL, 0);
  //   SDL_BlitSurface(tmp, NULL, conv, NULL);
  //   SDL_FreeSurface(tmp);
  //   tmp = conv;
  // }

  if (tmp == NULL)
    {
      printf("Error rendering text: %s; font is %p\n", TTF_GetError(), font);
    }

  TTF_SizeText (font, str, &text_w, &text_h);
  new_x = x;
  if (hcenter)
    {
      new_x += w / 2;
      new_x -= text_w / 2;
    }
  dst.x = new_x; dst.y = y;
  
  SDL_Rect src;
  src.x = src.y = 0;
  src.w = w; // truncate text if outside the box
  src.h = tmp->h;
  SDL_BlitSurface(tmp, &src, GFX_lpDDSBack, &dst);

  SDL_FreeSurface (tmp);
}

/**
 * Get the size in pixel of 'len' chars starting at 'str'
 */
static int
font_len (TTF_Font *font, char *str, int len)
{
  int text_w;
  char *tmp;

  /* Get the specified string portion and terminate it by \0 */
  tmp = (char *) malloc ((len + 1) * sizeof (char));
  strncpy (tmp, str, len);
  tmp[len] = 0;

  TTF_SizeText (font, tmp, &text_w, NULL);
  free (tmp);

  return text_w;
}

/**
 * Add newlines in the text so that it fit in 'box'
 * (a.k.a. word-wrapping)
 */
static int
process_text_for_wrapping (TTF_Font *font, char *str, rect *box)
{
  //printf("process_text_for_wrapping: %s on %dx%d\n", str, box->right - box->left, box->bottom - box->top);
  int i, start, line, last_fit;

  start = 0;
  i = 0;
  last_fit = -1;
  line = 0;
  while (str[i] != '\0')
    {
      int len;

      /* Skip forward to the end of the word */
      while (str[i] != '\0' && str[i] != ' ' && str[i] != '\n')
	i++;

      /* If the length of the text from start to i is bigger than the
	 box, then draw the text up to the last fitting portion -
	 unless that was the beginning of the string. */
      len = font_len (font, &str[start], i - start);

      if (len > (box->right - box->left))
	{
	  /* String is bigger than the textbox */

	  if (last_fit == -1)
	    {
	      /* Current word is too long by itself already, let's
		 keep it on a single line */
	      if (str[i] != '\0')
		str[i] = '\n';
	      /* continue on a new line */
	      line++;
	      start = i + 1;
	      i++;
	    }
	  else
	    {
	      /* All those words is bigger than the textbox, linebreak
		 at previous space */
	      str[last_fit] = '\n';
	      /* continue on a new line */
	      line++;
	      start = last_fit + 1;
	      i = last_fit + 1;
	    }
	  last_fit = -1;
	}
      else if (str[i] == '\0')
	{
	  line++;
	  i++;
	}
      else if (str[i] == '\n')
	{
	  line++;
	  i++;
	  start = i;
	  last_fit = -1;
	}
      else
	{
	  last_fit = i;
	  i++;
	}
    }

  return line;
}

/**
 * Print text 'str' in 'box', adding newlines if necessary
 * (word-wrapping). Return the text height in pixels.
 * 
 * calc_only: don't actually draw text on screen, but still compute
 * the text height
 */
int
print_text_wrap (char *str, rect* box,
		 /*bool*/int hcenter, int calc_only, FONT_TYPE font_type)
{
  int x, y, res_height;
  char *tmp, *pline, *pc;
  int this_is_last_line = 0;
  //  SDL_Color color = {0, 0, 0};
  SDL_Color color = text_color;
  TTF_Font *font;
  int lineskip = 0;

  if (font_type == DIALOG_FONT)
    font = dialog_font;
  else if (font_type == SYSTEM_FONT)
    font = system_font;
  else
    {
      fprintf(stderr, "Error: unknown font type %d\n", font_type);
      exit(1);
    }

  /* Workaround: with vgasys.fon, lineskip is always 1. We'll use it's
     height instead. */
  lineskip = TTF_FontLineSkip(font);
  if (lineskip == 1)
    lineskip = TTF_FontHeight(font);

  tmp = (char*) malloc(strlen(str) + 1);
  strcpy(tmp, str);
  /*   tmp = strdup (str); */
  process_text_for_wrapping (font, tmp, box);

  x = box->left;
  y = box->top;

  res_height = 0;
  pline = pc = tmp;
  this_is_last_line = 0;
  while (!this_is_last_line)
    {
      while (*pc != '\n' && *pc != '\0')
	pc++;

      if (*pc == '\0')
	this_is_last_line = 1;
      else
	/* Terminate the current line to feed it to print_text */
	*pc= '\0';

      if (!calc_only)
	print_text(font, pline, x, y + res_height, (box->right - box->left), color, hcenter);

      res_height += lineskip;

      /* advance to next line*/
      pc++;
      pline = pc;
    }
  free(tmp);
  return res_height;
}




/* Say, SaySmall: only used by freedinkedit.c */
/**
 * SaySmall: print text in a 40x40 small square; without font border
 * (sprite info boxes when typing 'I', plus something in tile
 * hardness)
 */
void SaySmall(char thing[500], int px, int py, int r, int g, int b)
{
  rect rcRect;
/*   HDC hdc; */
/*   if (lpDDSBack->GetDC(&hdc) == DD_OK) */
/*     {       */
/*       SetBkMode(hdc, TRANSPARENT);  */
      rect_set(&rcRect,px,py,px+40,py+40);
/*       SetTextColor(hdc,RGB(r,g,b)); */
/*       DrawText(hdc,thing,lstrlen(thing),&rcRect,DT_WORDBREAK); */
      // FONTS
      FONTS_SetTextColor(r, g, b);
      print_text_wrap(thing, &rcRect, 0, 0, SYSTEM_FONT);
      
/*       lpDDSBack->ReleaseDC(hdc); */
/*     }    */
}
/**
 * Say: print text until it reaches the border of the screen, with a
 * font border (input dialog boxes)
 */
void Say(char thing[500], int px, int py)
{
  rect rcRect;
/*   HDC hdc; */
  
/*   if (lpDDSBack->GetDC(&hdc) == DD_OK) */
/*     {       */
/*       SetBkMode(hdc, TRANSPARENT);  */
      rect_set(&rcRect,px,py,620,480);
/*       SelectObject (hdc, hfont_small); */

/*       SetTextColor(hdc,RGB(8,14,21)); */
/*       DrawText(hdc,thing,lstrlen(thing),&rcRect,DT_WORDBREAK); */
      // FONTS
      FONTS_SetTextColor(8, 14, 21);
      print_text_wrap(thing, &rcRect, 0, 0, DIALOG_FONT);

      rect_offset(&rcRect,-2,-2);
/*       DrawText(hdc,thing,lstrlen(thing),&rcRect,DT_WORDBREAK); */
      // FONTS
      print_text_wrap(thing, &rcRect, 0, 0, DIALOG_FONT);

      rect_offset(&rcRect,1,1);
/*       SetTextColor(hdc,RGB(255,255,0)); */
/*       DrawText(hdc,thing,lstrlen(thing),&rcRect,DT_WORDBREAK); */
      // FONTS
      FONTS_SetTextColor(255, 255, 0);
      print_text_wrap(thing, &rcRect, 0, 0, DIALOG_FONT);
      
/*       lpDDSBack->ReleaseDC(hdc); */
/*     }    */
}
