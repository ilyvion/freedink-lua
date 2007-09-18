/**
 * Timer tests

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

/* 3 ways to test, using SDL, nanosleep() or Woe - same results. */
/* Under GNU/Linux, the timer resolution will be limited by the kernel
   config HZ parameter (100, 250 or 1000Hz, so 10ms, 4ms or 1ms). */
/* Under Woe, the timer resolution can be changed at run time. It's
   10ms by default but functions such as timeGetDevCaps can reduce it
   to 1ms. That's automatically done by SDL when you start your
   app. The timer resolution resets to 10ms when SDL quits. */
/* Conclusion: waiting for 1ms is not reliable in a portable app. */

/* #define SDL_MODE 1 */
#define NANOSLEEP_MODE 1
/* #define WOE_MODE 1 */

#ifdef SDL_MODE
#include "SDL.h"
#endif
#ifdef NANOSLEEP_MODE
#include <sys/time.h>
#include <time.h>
#endif
#ifdef WOE_MODE
#include <windows.h>
#endif

#include <stdio.h>

int main(int argc, char *argv[])
{
  long last_ticks;
  long cur_ticks;
  unsigned long max = 0;
  unsigned long min = -1; /* unsigned, so it's the max value */
  double average = 0;
  int count = 0;

#ifdef SDL_MODE
  SDL_Init(SDL_INIT_TIMER);
#endif

  while(1)
    {
      long duration;
      count++;

#ifdef SDL_MODE
      last_ticks = SDL_GetTicks();
      SDL_Delay(1);
      cur_ticks = SDL_GetTicks();
#endif
#ifdef NANOSLEEP_MODE
      {
	struct timespec t = {0, 1e6};
	struct timeval tv;
	gettimeofday(&tv, NULL);
	last_ticks = tv.tv_usec / 1000;
	nanosleep(&t, NULL);
	gettimeofday(&tv, NULL);
	cur_ticks = tv.tv_usec / 1000;
	if (cur_ticks < last_ticks)
	  cur_ticks += 1000;
      }
#endif
#ifdef WOE_MODE
      last_ticks = GetTickCount();
      Sleep(1);
      cur_ticks = GetTickCount();
#endif

      duration = cur_ticks - last_ticks;
      if (duration > max) max = duration;
      if (duration < min) min = duration;
      average = (average * (count-1) + duration) / count;
      printf("%ld %ld %ld %f\n", duration, min, max, average);
    }
  
#ifdef SDL_MODE
  SDL_Quit();
#endif

  return 0;
}
