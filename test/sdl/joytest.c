/**
 * SDL joystick test

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

/* Moves the heart surface using the joystick. Button 2 quits, button
   4 goes full screen, button 6 triples the speed. */

#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"

#define IMIN(a,b) ((a < b) ? a : b)
#define IMAX(a,b) ((a > b) ? a : b)

int main(int argc, char *argv[]) {
  SDL_Joystick *joystick;
  int i;
  int quit = 0;
  SDL_Surface *screen, *pic;
  Uint32 video_flags = SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF | SDL_ANYFORMAT | SDL_RESIZABLE;
  SDL_Rect dst = {320, 240};

  int default_screen_w = 640, default_screen_h = 480;
  double
    px = 320,
    py = 240,
    cur_joyx = 0,
    cur_joyy = 0;
  Uint32 last_update = 0;

  /* pixels per second */
  int full_speed = 200;

  if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0)
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
      exit(1);
    }

  screen = SDL_SetVideoMode(0, 0, 0, video_flags); /* SDL >= 1.10 */
  if (screen == NULL)
    {
      printf("Trying %dx%d...\n", default_screen_w, default_screen_h);
      screen = SDL_SetVideoMode(default_screen_w, default_screen_h, 0, video_flags);
      if (screen == NULL)
	{
	  fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
	  exit(1);
	}
    }
  SDL_WM_SetCaption("JoyTest", NULL);
  SDL_ShowCursor(SDL_DISABLE);

  pic = SDL_LoadBMP("pic.bmp");
  if (pic == NULL)
    {
      fprintf(stderr, "Failed to load pic.bmp: %s\n", SDL_GetError());
      exit(1);
    }
  SDL_SetColorKey(pic, SDL_SRCCOLORKEY, SDL_MapRGB(pic->format, 255, 255, 255));

  printf("%i joysticks were found.\n\n", SDL_NumJoysticks() );
  printf("The names of the joysticks are:\n");

  for (i=0; i < SDL_NumJoysticks(); i++)
    {
      printf("    %s\n", SDL_JoystickName(i));
    }

  SDL_JoystickEventState(SDL_ENABLE);
  joystick = SDL_JoystickOpen(0);
  printf("Number of axes: %d\n", SDL_JoystickNumAxes(joystick));
  printf("Number of buttons: %d\n", SDL_JoystickNumButtons(joystick));
  printf("Number of balls: %d\n", SDL_JoystickNumBalls(joystick));
  printf("Number of hats: %d\n", SDL_JoystickNumHats(joystick));

  /* Flush stacked joystick events */
  {
    SDL_Event event;
    while (SDL_PollEvent(&event));
  }

  /* Fill screen in blue */
  SDL_FillRect(screen, NULL,
	       SDL_MapRGB(screen->format, 0, 0, 255));
  SDL_Flip(screen);

  last_update = SDL_GetTicks();

  /* Main game loop */
  while(!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      {
	switch(event.type)
	  {
	  case SDL_KEYDOWN:
	    /* handle keyboard stuff here */
	    printf("Key pressed\n");
	    if (event.key.keysym.sym == 'q' || event.key.keysym.sym == SDLK_ESCAPE)
	      quit = 1;
	    break;
	    
	  case SDL_VIDEORESIZE:
	    {
	      SDL_ResizeEvent *resize = (SDL_ResizeEvent*)&event;
	      screen = SDL_SetVideoMode(resize->w, resize->h, 0, video_flags);
	      printf("Screen resized to %dx%d\n", resize->w, resize->h);
	    }
	    break;
	    
	  case SDL_QUIT:
	    quit = 1;
	  
	    break;
	    
	  case SDL_JOYAXISMOTION:  /* Joystick pad moved */
	    if (event.jaxis.axis == 0)
	      {
		/* Ignore noise from joystick */
		if ((event.jaxis.value < -3276 ) || (event.jaxis.value > 3276))
		  {
		    cur_joyx = event.jaxis.value;
		  }
		else
		  cur_joyx = 0;
	      }
	    
	    if (event.jaxis.axis == 1)
	      {
		/* Ignore noise from joystick */
		if ((event.jaxis.value < -3276 ) || (event.jaxis.value > 3276))
		  {
		    cur_joyy = event.jaxis.value;
		  }
		else
		  cur_joyy = 0;
	      }
	    
 	    break;
	    
	  case SDL_JOYBUTTONDOWN:  /* Joystick button pressed */
	    printf("Button %d pressed\n", event.jbutton.button);
	    if (event.jbutton.button == 3)
	    {
 	      SDL_WM_ToggleFullScreen(screen);
	    }
	    else if (event.jbutton.button == 1)
	      {
		quit = 1;
	      }
	    else if (event.jbutton.button == 5)
	      {
		full_speed *= 3;
	      }
	    break;
	    
	  case SDL_JOYBUTTONUP:  /* Joystick button pressed */
	    if (event.jbutton.button == 5)
	      {
		full_speed /= 3;
	      }
	    break;
	  }
      }
    
    {
      SDL_Rect erase, update;
      int prev_x, prev_y;
      int update_p1_x, update_p1_y;
      int update_p2_x, update_p2_y;
      double dx = 0, dy = 0;
      Uint32 dt;

      /* Check elapsed time since last frame */
      dt = SDL_GetTicks() - last_update;
      last_update = SDL_GetTicks();
      

      /* Erase last position */
      prev_x = dst.x;
      prev_y = dst.y;
      erase.x = prev_x;
      erase.y = prev_y;
      erase.w = pic->w;
      erase.h = pic->h;
      SDL_FillRect(screen, &erase,
		   SDL_MapRGB(screen->format, 0, 0, 255));


      /* Update position */
      /* dx/dy is the speed in pixels per second */
      /* In one second, with full push (32767), I'll move full_speed (200) pixels */
      dx = cur_joyx * (1.0 * full_speed / 32767) * dt/1000;
      dy = cur_joyy * (1.0 * full_speed / 32767) * dt/1000;
      px += dx;
      py += dy;
      dst.x = px;
      dst.y = py;
      

      /* Display new position */
      SDL_BlitSurface(pic, NULL, screen, &dst);


      /* What to refresh */
      update_p1_x = IMIN(prev_x, dst.x);
      update_p1_y = IMIN(prev_y, dst.y);
      update_p2_x = IMAX(prev_x, dst.x) + pic->w;
      update_p2_y = IMAX(prev_y, dst.y) + pic->h;
  
      /* Clipping */
      update_p1_x = IMIN(IMAX(update_p1_x, 0), screen->w);
      update_p1_y = IMIN(IMAX(update_p1_y, 0), screen->h);
      update_p2_x = IMIN(IMAX(update_p2_x, 0), screen->w);
      update_p2_y = IMIN(IMAX(update_p2_y, 0), screen->h);

      /* Refresh */
      update.x = update_p1_x;
      update.y = update_p1_y;
      update.w = update_p2_x - update_p1_x;
      update.h = update_p2_y - update_p1_y;
      SDL_UpdateRects(screen, 1, &update);
      /* printf("pos %f,%f\n", px, py); */
    }
  }
  SDL_JoystickClose(joystick);
  SDL_Quit();
  return 0;
}
