/**
 * Fonts

 * Copyright (C) 2007, 2008, 2009  Sylvain Beucler

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
#define WIN32_LEAN_AND_MEAN
#define _WIN32_IE 0x0401
#include <windows.h>
#include <shlobj.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "SDL.h"
#include "SDL_ttf.h"
#include "dinkvar.h"
#include "gfx.h"
#include "io_util.h"
#include "paths.h"
#include "gfx_fonts.h"
#include "gfx_palette.h"
#include "vgasys_fon.h"
#include "init.h"
#include "log.h"


/* Default size was 18 in the original game, but it refers to a
   different part of the font glyph (see doc/fonts.txt for
   details). 16 matches that size with SDL_ttf (possibly only for
   LiberationSans). */
#define FONT_SIZE 16

/* Default fonts: dialog and system */
static TTF_Font *dialog_font = NULL;
static TTF_Font *system_font = NULL;

/* Current font parameters */
static SDL_Color text_color;

static TTF_Font *load_default_font();
static void setup_font(TTF_Font *font);

// D-Mod-defined font colors
struct font_color
{
  int red;
  int green;
  int blue;
};
static struct font_color font_colors[16];

#ifdef HAVE_FONTCONFIG
#include <fontconfig/fontconfig.h>
/* Get filename for canonical font name 'fontname'. Return NULL if the
   font cannot be found (for correctness, no alternate font will be
   provided). */
char* get_fontconfig_path(char* fontname)
{
  char* filename = NULL;
  FcPattern* p = NULL;
  FcChar8* strval = NULL;
  FcObjectSet *attr = NULL;

  if (!FcInit())
    {
      log_error("get_fontconfig_path: cannot initialize fontconfig");
      return NULL;
    }

  p = FcNameParse((FcChar8*)fontname);
  if (p == NULL)
    {
      log_error("get_fontconfig_path: invalid font pattern: %s", fontname);
      return NULL;
    }
  /* Grab filename attribute */
  attr = FcObjectSetBuild (FC_FILE, (char *) 0);

  FcFontSet *fs = FcFontList (0, p, attr);
  if (fs->nfont == 0)
    {
      log_error("get_fontconfig_path: no matching font for %s", fontname);
      return NULL;
    }
  if (FcPatternGetString(fs->fonts[0], FC_FILE, 0, &strval) == FcResultTypeMismatch
      || strval == NULL)
    {
      log_error("get_fontconfig_path: cannot find font filename for %s", fontname);
      return NULL;
    }

  filename = strdup((char*)strval);

  FcFontSetDestroy(fs);
  FcObjectSetDestroy(attr);
  FcPatternDestroy(p);
  FcFini();

  return filename;
}
#endif

/**
 * Init font subsystem and one built-in font, so we can display error
 * messages in emergency (init error) situations
 */
int gfx_fonts_init_failsafe()
{
  if (!TTF_WasInit() && TTF_Init() == -1)
    return -1;

  /* Load system font from compiled data */
  if (system_font == NULL)
    {
      system_font = TTF_OpenFontRW(SDL_RWFromConstMem(vgasys_fon, sizeof(vgasys_fon)),
				   1, FONT_SIZE);
      if (system_font == NULL)
	return -1;
      setup_font(system_font);
    }

  return 0;
}

/**
 * Init font subsystem and load the default fonts
 */
int gfx_fonts_init()
{
  if (TTF_Init() == -1) {
    init_set_error_msg(TTF_GetError());
    return -1;
  }

  /* Load system font from compiled data */
  system_font = TTF_OpenFontRW(SDL_RWFromConstMem(vgasys_fon, sizeof(vgasys_fon)),
			       1, FONT_SIZE);
  if (system_font == NULL)
    {
      init_set_error_msg("Failed to load builtin 'vgasys.fon' font.");
      return -1;
    }
  setup_font(system_font);

  /* Load dialog font from built-in resources */
  dialog_font = load_default_font();
  if (dialog_font == NULL)
    return -1; /* error message set by load_default_font */
  setup_font(dialog_font);

  gfx_fonts_init_colors();

  return 0;
}

/**
 * Quit the font subsystem (and free loaded fonts from memory)
 */
void gfx_fonts_quit(void)
{
  if (dialog_font != NULL)
    {
      TTF_CloseFont(dialog_font);
      dialog_font = NULL;
    }
  if (system_font != NULL)
    {
      // Uncomment when FreeType 2.5.3 is widespread
      // https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=747002
      //TTF_CloseFont(system_font);
      system_font = NULL;
    }

  TTF_Quit();
}


/**
 * Default font from resources and pkgdatadir
 */
static TTF_Font *load_default_font() {
  TTF_Font *font_object = NULL;
  SDL_RWops* rwops = NULL;

  /* Try from resources */
  if (rwops == NULL)
    {
      rwops = find_resource_as_rwops("LiberationSans-Regular.ttf");
    }
#ifdef HAVE_FONTCONFIG
  if (rwops == NULL)
    {
      char *path = get_fontconfig_path("Liberation Sans:style=Regular");
      rwops = SDL_RWFromFile(path, "rb");
      free(path);
    }
#endif
  if (rwops == NULL)
    {
      init_set_error_msg("Could not open font 'LiberationSans-Regular.ttf'. I tried:\n"
			 "- loading from '%s'\n"
			 "- loading from '%s'\n"
			 "- loading from '%s'\n"
			 "- loading from executable's resources\n"
#ifdef HAVE_FONTCONFIG
			 "- querying fontconfig"
#endif
			 ,
			 paths_getpkgdatadir(), paths_getdefaultpkgdatadir(), paths_getexedir());
      return NULL;
    }

  font_object = TTF_OpenFontRW(rwops, 1, FONT_SIZE);
  if (font_object == NULL)
    {
      init_set_error_msg("Could not open font 'LiberationSans-Regular.ttf': %s", TTF_GetError());
      return NULL;
    }

  return font_object;
}

/**
 * Change the current dialog font (DinkC initfont() command)
 */
int initfont(char* fontname) {
  TTF_Font *new_font = NULL;
  char* ext = ".ttf";
  char* filename = malloc(strlen(fontname) + strlen(ext) + 1);
  strcpy(filename, fontname);
  strcat(filename, ext);

  if (new_font == NULL)
    {
      char *path = NULL;
#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__
      /* Look in system fonts dir */
      path = malloc(MAX_PATH + 1 + strlen(filename) + 1);
      /* C:\WINNT\Fonts */
      SHGetSpecialFolderPath(NULL, path, CSIDL_FONTS, 0);
      strcat(path, "\\");
      strcat(path, filename);
#else
#  ifdef HAVE_FONTCONFIG
      path = get_fontconfig_path(fontname);
#  else
      path = NULL;
#  endif
#endif
      if (path == NULL)
	{
	  log_error("initfont: cannot find '%s'", fontname);
	}
      else
	{
	  new_font = TTF_OpenFont(path, FONT_SIZE);
	  if (new_font == NULL)
	    log_error("TTF_OpenFont: %s", TTF_GetError());
	  free(path);
	}
    }

  if (new_font == NULL)
    return -1;


  /* new_font could be loaded - we can free the previous one */
  TTF_CloseFont(dialog_font);
  dialog_font = new_font;

  setup_font(dialog_font);

  return 0;
}

/**
 * Change a font color (DinkC set_font_color() command)
 */
void set_font_color(int no, int red, int green, int blue)
{
  if (no >= 1 && no <= 15
      && red   >= 0 && red   <= 255
      && green >= 0 && green <= 255
      && blue  >= 0 && blue  <= 255)
    {
      font_colors[no].red = red;
      font_colors[no].green = green;
      font_colors[no].blue = blue;
    }
}


/**
 * Apply default style to the font
 * Plus some informative output
 */
static void
setup_font(TTF_Font *font)
{
  char *familyname = TTF_FontFaceFamilyName(font);
  if(familyname)
    log_info("The family name of the face in the font is: %s", familyname);
  char *stylename = TTF_FontFaceStyleName(font);
  if(stylename)
    log_info("The name of the face in the font is: %s", stylename);
  log_info("The font max height is: %d", TTF_FontHeight(font));
  log_info("The font ascent is: %d", TTF_FontAscent(font));
  log_info("The font descent is: %d", TTF_FontDescent(font));
  log_info("The font line skip is: %d", TTF_FontLineSkip(font));
  if(TTF_FontFaceIsFixedWidth(font))
    log_info("The font is fixed width.");
  else
    log_info("The font is not fixed width.");

  TTF_SetFontStyle(font, TTF_STYLE_BOLD);
}




void FONTS_SetTextColor(Uint8 r, Uint8 g, Uint8 b) {
  text_color.r = r;
  text_color.g = g;
  text_color.b = b;
}
void FONTS_SetTextColorIndex(int no) {
  text_color.r = font_colors[no].red;
  text_color.g = font_colors[no].green;
  text_color.b = font_colors[no].blue;
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
  if (!truecolor)
    {
      SDL_PixelFormat fmt;
      SDL_Palette pal;
        pal.ncolors = 256;
        SDL_Color tmppal[256];
        gfx_palette_get_phys(tmppal);
        pal.colors = tmppal;
      fmt.palette = &pal;
      Uint32 phys_index = SDL_MapRGB(&fmt, color.r, color.g, color.b);
      SDL_GetRGB(phys_index, GFX_lpDDSBack->format, &(color.r), &(color.g), &(color.b));
    }

  /* Transparent, low quality - closest to the original engine. */
  /* Besides, we do need a monochrome render, since we're doing nasty
     tricks to set the color appropriately */
  tmp = TTF_RenderUTF8_Solid(font, str, color);

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
      log_error("Error rendering text: %s; font is %p", TTF_GetError(), font);
      return;
    }

  TTF_SizeUTF8 (font, str, &text_w, &text_h);
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

  TTF_SizeUTF8 (font, tmp, &text_w, NULL);
  free (tmp);

  return text_w;
}

/**
 * Add newlines in the text so that it fit in 'box'
 * (a.k.a. word-wrapping)
 */
static int
process_text_for_wrapping (TTF_Font *font, char *str, int max_len)
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

      if (len > max_len)
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
	      if (str[i] != '\0')
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

  if (font_type == FONT_DIALOG)
    font = dialog_font;
  else if (font_type == FONT_SYSTEM)
    font = system_font;
  else
    {
      log_error("Error: unknown font type %d", font_type);
      exit(1);
    }

  /* Workaround: with vgasys.fon, lineskip is always 1. We'll use it's
     height instead. */
  lineskip = TTF_FontLineSkip(font);
  if (lineskip == 1)
    lineskip = TTF_FontHeight(font);

  tmp = strdup(str);
  process_text_for_wrapping(font, tmp, box->right - box->left);

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


/**
 * Display text for debug mode (with a white background)
 */
void
print_text_wrap_debug(char *text, int x, int y)
{
  char *tmp, *pline, *pc;
  int this_is_last_line = 0;
  int res_height = 0;
  SDL_Color bgcolor = {255, 255, 255};
  int max_len = 640;

  /* Workaround: with vgasys.fon, lineskip is always 1. We'll use it's
     height instead. */
  int lineskip = TTF_FontHeight(system_font);

  int textlen = strlen(text);
  tmp = malloc(strlen(text) + 1);
  /* drop '\r' */
  pc = tmp;
  int i;
  for (i = 0; i < textlen; i++)
    if (text[i] == '\r' && text[i+1] == '\n')
      continue;
    else
      *(pc++) = text[i];
  *pc = '\0';
  
  process_text_for_wrapping(system_font, tmp, max_len);

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

      SDL_Rect dst = {x, y + res_height, -1, -1};
      SDL_Surface *rendered_text = TTF_RenderUTF8_Shaded(system_font, pline, text_color, bgcolor);
      SDL_BlitSurface(rendered_text, NULL, GFX_lpDDSBack, &dst);
      SDL_FreeSurface(rendered_text);

      res_height += lineskip;

      /* advance to next line*/
      pc++;
      pline = pc;
    }
  free(tmp);
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
      print_text_wrap(thing, &rcRect, 0, 0, FONT_SYSTEM);
      
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
      print_text_wrap(thing, &rcRect, 0, 0, FONT_DIALOG);

      rect_offset(&rcRect,-2,-2);
/*       DrawText(hdc,thing,lstrlen(thing),&rcRect,DT_WORDBREAK); */
      // FONTS
      print_text_wrap(thing, &rcRect, 0, 0, FONT_DIALOG);

      rect_offset(&rcRect,1,1);
/*       SetTextColor(hdc,RGB(255,255,0)); */
/*       DrawText(hdc,thing,lstrlen(thing),&rcRect,DT_WORDBREAK); */
      // FONTS
      FONTS_SetTextColor(255, 255, 0);
      print_text_wrap(thing, &rcRect, 0, 0, FONT_DIALOG);
      
/*       lpDDSBack->ReleaseDC(hdc); */
/*     }    */
}


void gfx_fonts_init_colors()
{
  //Light Magenta
  font_colors[1].red = 255;
  font_colors[1].green = 198;
  font_colors[1].blue = 255;

  //Dark Green
  font_colors[2].red = 131;
  font_colors[2].green = 181;
  font_colors[2].blue = 74;

  //Bold Cyan
  font_colors[3].red = 99;
  font_colors[3].green = 242;
  font_colors[3].blue = 247;

  //Orange
  font_colors[4].red = 255;
  font_colors[4].green = 156;
  font_colors[4].blue = 74;

  //Magenta
  font_colors[5].red = 222;
  font_colors[5].green = 173;
  font_colors[5].blue = 255;

  //Brown Orange
  font_colors[6].red = 244;
  font_colors[6].green = 188;
  font_colors[6].blue = 73;

  //Light Gray
  font_colors[7].red = 173;
  font_colors[7].green = 173;
  font_colors[7].blue = 173;

  //Dark Gray
  font_colors[8].red = 85;
  font_colors[8].green = 85;
  font_colors[8].blue = 85;

  //Sky Blue
  font_colors[9].red = 148;
  font_colors[9].green = 198;
  font_colors[9].blue = 255;

  //Bright Green
  font_colors[10].red = 0;
  font_colors[10].green = 255;
  font_colors[10].blue = 0;

  //Yellow
  font_colors[11].red = 255;
  font_colors[11].green = 255;
  font_colors[11].blue = 2;

  //Yellow
  font_colors[12].red = 255;
  font_colors[12].green = 255;
  font_colors[12].blue = 2;

  //Hot Pink
  font_colors[13].red = 255;
  font_colors[13].green = 132;
  font_colors[13].blue = 132;

  //Yellow
  font_colors[14].red = 255;
  font_colors[14].green = 255;
  font_colors[14].blue = 2;

  //White
  font_colors[15].red = 255;
  font_colors[15].green = 255;
  font_colors[15].blue = 255;
}
