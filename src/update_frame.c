/**
 * FreeDink (not FreeDinkEdit) screen update

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2005, 2006  Dan Walma
 * Copyright (C) 2005, 2007, 2008, 2009, 2010, 2011, 2012  Sylvain Beucler

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* #include <windows.h> */
/* #include <ddraw.h> */
#include "SDL.h"
#include "SDL_framerate.h"

#include "dinkvar.h"
#include "game_engine.h"
#include "screen.h"
#include "freedink.h"
#include "gfx.h"
/* For printing strings in debug mode */
#include "gfx_fonts.h"
#include "gfx_sprites.h"
#include "gfx_tiles.h"
#include "bgm.h"
#include "log.h"
#include "rect.h"
#include "dinkc_console.h"
#include "input.h"
#include "sfx.h"

#include "update_frame.h"

enum speed_type { v107, v108 };
static Uint32 fps_lasttick = 0;
static int frames = 0;
static int fps = 0;
static int drawthistime = /*true*/1;
static /*bool*/int turn_on_plane = /*FALSE*/0;
static /*bool*/int plane_process = /*TRUE*/1;

void updateFrame( void )
{
  /* Refresh frame counter twice per second */
  if ((SDL_GetTicks() - fps_lasttick) > 500)
    {
      fps = frames * (1000/500);
      frames = 0;
      fps_lasttick = SDL_GetTicks();
    }
  frames++;
  
/*     RECT                box_crap,box_real; */
    SDL_Rect GFX_box_crap;

/* 	DDBLTFX     ddbltfx; */
    char msg[100];
	
/* 	HDC         hdc; */
/*     HRESULT             ddrval; */

    int move_result ;
    
    /* Probably a debug variable, only set in a "if (5 > 9)" block. I
       don't know what this allows to debug though; this involves
       copying the screen to lpDDSTrick2 */
    /*bool*/int get_frame = /*false*/0;

	int max_s;
	int rank[MAX_SPRITES_AT_ONCE];
	
	abort_this_flip = /*false*/0;
	
	if (5 > 9)
	{
trigger_start:
	trig_man = /*false*/0;
    get_frame = /*true*/1;    
	}
	
	check_joystick();
	
	if (GetKeyboard('m') && (GetKeyboard(SDLK_LALT) || GetKeyboard(SDLK_RALT)))
	{
		//shutdown music
		StopMidi();
		return;
	}
	
	if (GetKeyboard('d') && (GetKeyboard(SDLK_LALT) || GetKeyboard(SDLK_RALT)))
	  {	
	    if (debug_mode)
	      {
		log_debug_off();
	      }
	    else
	      {
		log_debug_on();
	      }
	}
		  
	
	
	if (GetKeyboard('q') && (GetKeyboard(SDLK_LALT) || GetKeyboard(SDLK_RALT)))
	{
	  //shutdown game
	  //	PostMessage(hWndMain, WM_CLOSE, 0, 0);
	  SDL_Event ev;
	  ev.type = SDL_QUIT;
	  SDL_PushEvent(&ev);
	  return;
	}
	
	if (mode == 1) Scrawl_OnMouseInput(); else
	{
		if (keep_mouse)
		{
			if ((talk.active) || (spr[1].brain == 13))
				Scrawl_OnMouseInput();
		}
		
	}
	
/* demon: */
	
/*	while(GetTickCount() < (lastTickCount+ 20))
{
//wait
}
	*/
	
	lastTickCount = thisTickCount;
	thisTickCount = game_GetTicks();
	
	
      {
	//Use to test at 30 fps
	//Sleep(66);
	
	//redink1 - 'lock the framerate to 83 FPS'... Seth told me to.
	/* Beuc: that doesn't work. Waiting for 1ms is not guaranteed
	   to work accurately, most often computer will wait for
	   delays such as 10ms or 15ms. My tests give framerates of
	   respectively 50 and 60 FPS. */
/* Woe: */
/* 	while (thisTickCount - lastTickCount < 12) */
/* 	  { */
/* 	    Sleep (1); */
/* 	    thisTickCount = GetTickCount (); */
/* 	  } */

/* SDL */
/* 	while (thisTickCount - lastTickCount < 12) */
/* 	  { */
/* 	    SDL_Delay (1); */
/* 	    thisTickCount = SDL_GetTicks (); */
/* 	  } */

	/* SDL_gfx has a more clever algorithm, which accurately sets
	   the framerate to a fixed value. */
	SDL_framerateDelay(&framerate_manager);


	thisTickCount = game_GetTicks();

	fps_final = thisTickCount - lastTickCount;
	
	//redink1 changed to 12-12 from 10-15... maybe work better on faster computers?
	if (fps_final < 12)
	  fps_final = 12;
	if (fps_final > 68)
	  fps_final = 68;
	base_timing = fps_final / 3;
	if (base_timing < 4)
	  base_timing = 4;
	int junk3;
	
	//redink1 added these changes to set Dink's speed correctly, even on fast machines.
	//junk3 = (fps_average / dinkspeed) -  (fps_average / 8);
	if (dinkspeed <= 0)
	  junk3 = 0;
	
	else if (dinkspeed == 1)
	  junk3 = 12;
	
	else if (dinkspeed == 2)
	  junk3 = 6;
	
	else if (dinkspeed == 3)
	  junk3 = 3;
	
	else
	  junk3 = 1;
	
	//redink1... weird bug, only do this for normal mode, as it breaks mouse support
	//if (mode == 2)
	{
	  junk3 *= (base_timing / 4);
	}
	spr[1].speed = junk3;
      }
	
	
	if (showb.active)
	{
		process_show_bmp();
		if (!abort_this_flip)
		  flip_it();
		
		return;
	}
	
	mbase_count++;
	
	if (thisTickCount > mold+100)
	{
		mold = thisTickCount;
		if (bow.active) bow.hitme = /*true*/1;
		if (*pupdate_status == 1) update_status_all();
		
		
		update_sound();
		process_animated_tiles();
	}
	
	
	
	//figure out frame rate
	drawthistime = /*true*/1;
	
	if (show_inventory)
	{
		process_item();
		return;
	}
	
	
	if (total_trigger) 
		
	{
		if (transition()) goto flip;   else return;
	}

	
	/* Fade to black, etc. */
	if (process_upcycle) up_cycle();
	if (process_warp > 0) process_warp_man();
	if (process_downcycle) CyclePalette();
	
	
	if (plane_process)
	  {
	    max_s = last_sprite_created;
	    screen_rank_game_sprites(rank);
	  }
	else
	  {
	    //not processing planes
	    max_s = MAX_SPRITES_AT_ONCE;
	  }
	
	//Blit from Two, which holds the base scene.
	SDL_BlitSurface(GFX_lpDDSTwo, NULL, GFX_lpDDSBack, NULL);
	
	
	if (stop_entire_game == 1)
	{
		if (talk.active) process_talk(); 
		
		else
		{
			stop_entire_game = 0;
			
			draw_map_game_background();
			draw_status_all();
			
		}
		goto flip;
		
	}
	
	
	
	
	int j = 0;
	for (; j <= max_s; j++)
	{
		//h  = 1;
		int h = 0;
		if (plane_process)
		  h = rank[j];
		else
		  h = j;
		//Msg( "Ok, rank %d is %d", j,h);
		
		if (h > 0) 
			if (spr[h].active && spr[h].disabled == 0)
			{
				
				//check_sprite_status_full(h);
				
				spr[h].moveman = 0; //init thing that keeps track of moving path	
				spr[h].lpx[0] = spr[h].x;
				spr[h].lpy[0] = spr[h].y; //last known legal cords
				
				spr[h].skiptimer++;
				//inc delay, used by "skip" by all sprites
/* 				box_crap = k[getpic(h)].box; */
				if (spr[h].kill > 0)
				{
					if (spr[h].kill_timer == 0) spr[h].kill_timer = thisTickCount;
					if (spr[h].kill_timer + spr[h].kill < thisTickCount)
					{
						
						spr[h].active = /*false*/0;
						//          Msg("Killing sprite %d.", h);
						
						get_last_sprite();
						if (spr[h].callback > 0) 
						{
							//	Msg("Callback running script %d.", spr[h].script);
							
							run_script(spr[h].callback);
							
							
						}
						
						
					}
					
				}
				
				if (spr[h].timer > 0)
				{
					if (thisTickCount > spr[h].wait)
					{
						spr[h].wait = thisTickCount + spr[h].timer;
						
					}else
					{
						goto animate;
					}
					
				}
				
				
				//brains - predefined bahavior patterns available to any sprite
				
				if (spr[h].notouch) if (thisTickCount > spr[h].notouch_timer) spr[h].notouch = /*false*/0;
				if (get_frame == /*false*/0)
				{
					if (   (spr[h].brain == 1)/* || (spr[h].brain == 9) || (spr[h].brain == 3) */ )
					{
						
						run_through_touch_damage_list(h);
						
						
					}		
					
					if (spr[h].brain == 1)
					{
						if (process_warp == 0)
							human_brain(h);	
					}
					
					if (spr[h].brain == 2) bounce_brain(h);
					if (spr[h].brain == 0) no_brain(h);
					if (spr[h].brain == 3) duck_brain(h);
					if (spr[h].brain == 4) pig_brain(h);
					if (spr[h].brain == 5) one_time_brain(h);
					if (spr[h].brain == 6) repeat_brain(h);
					if (spr[h].brain == 7) one_time_brain_for_real(h);
					if (spr[h].brain == 8) text_brain(h);
					if (spr[h].brain == 9) pill_brain(h);
					if (spr[h].brain == 10) dragon_brain(h);
					if (spr[h].brain == 11) missile_brain(h, /*true*/1);
					if (spr[h].brain == 12) scale_brain(h);
					if (spr[h].brain == 13) mouse_brain(h);
					if (spr[h].brain == 14) button_brain(h);
					if (spr[h].brain == 15) shadow_brain(h);
					if (spr[h].brain == 16) people_brain(h);
					if (spr[h].brain == 17) missile_brain_expire(h);
				} else
				{
					goto past;
				}
				if (g_b_kill_app) return;
animate:
				
				move_result = check_if_move_is_legal(h);
				
				if (flub_mode != -500)
				{
					log_debug("move result is %d", flub_mode);
					move_result = flub_mode;
					flub_mode = -500;
				}
				
				if (spr[h].brain == 1) if (move_result > 100)
				{
					if (pam.sprite[move_result-100].is_warp == 1)
						special_block(move_result - 100);
				}
				
				
				if (spr[h].reverse)
				{
					
					//reverse instructions
					if (spr[h].seq > 0)
					{
						if (spr[h].frame < 1)
						{
							// new anim
							spr[h].pseq = spr[h].seq;
							spr[h].pframe = seq[spr[h].seq].len;
							spr[h].frame = seq[spr[h].seq].len;
							if (spr[h].frame_delay != 0) spr[h].delay = (thisTickCount+ spr[h].frame_delay); else
								spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[seq[spr[h].seq].len]);
						}   else
						{
							// not new anim
							
							//is it time?
							
							if (thisTickCount > spr[h].delay)
							{
								
								
								spr[h].frame--;
								
								
								if (spr[h].frame_delay != 0) spr[h].delay = (thisTickCount + spr[h].frame_delay); else
									
									spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);
								
								spr[h].pseq = spr[h].seq;
								spr[h].pframe = spr[h].frame;
								
								
								if (seq[spr[h].seq].frame[spr[h].frame]  < 2)
								{
									
									spr[h].pseq = spr[h].seq;
									spr[h].pframe = spr[h].frame+1;
									
									spr[h].frame = 0;
									spr[h].seq_orig = spr[h].seq;
									spr[h].seq = 0;
									spr[h].nocontrol = /*false*/0;
									
									
									if (h == 1) if (in_this_base(spr[h].seq_orig, dink_base_push))
										
									{
										
										
										play.push_active = /*false*/0;
										if (play.push_dir == 2) if (sjoy.down) play.push_active = /*true*/1;
										if (play.push_dir == 4) if (sjoy.left) play.push_active = /*true*/1;
										if (play.push_dir == 6) if (sjoy.right) play.push_active = /*true*/1;
										if (play.push_dir == 8) if (sjoy.up) play.push_active = /*true*/1;
										
										
										goto past;
										
									}
								}
								if (spr[h].seq > 0) if (seq[spr[h].seq].special[spr[h].frame] == 1)
								{
									//this sprite can damage others right now!
									//lets run through the list and tag sprites who were hit with their damage
									
									run_through_tag_list(h, spr[h].strength);
									
								}
								
								
								
								
							}
						}
					}
					
					
				} else
				{
					
					if (spr[h].seq > 0) if (spr[h].picfreeze == 0)
					{
						if (spr[h].frame < 1)
						{
							// new anim
							spr[h].pseq = spr[h].seq;
							spr[h].pframe = 1;
							spr[h].frame = 1;
							if (spr[h].frame_delay != 0)
							  spr[h].delay = thisTickCount + spr[h].frame_delay;
							else
							  spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[1]);
						}   else
						{
							// not new anim
							
							//is it time?
							
							if (thisTickCount > spr[h].delay)
							{
								
								
								spr[h].frame++;
								if (spr[h].frame_delay != 0)
								  spr[h].delay = thisTickCount + spr[h].frame_delay;
								else
								  spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);
								
								spr[h].pseq = spr[h].seq;
								spr[h].pframe = spr[h].frame;
								
								if (seq[spr[h].seq].frame[spr[h].frame] == -1)
								{
									spr[h].frame = 1;
									spr[h].pseq = spr[h].seq;
									spr[h].pframe = spr[h].frame;
									if (spr[h].frame_delay != 0) spr[h].delay = thisTickCount + spr[h].frame_delay; else
										
										spr[h].delay = (thisTickCount + seq[spr[h].seq].delay[spr[h].frame]);
									
								}
								
								if (seq[spr[h].seq].frame[spr[h].frame]  < 1)
								{
									
									spr[h].pseq = spr[h].seq;
									spr[h].pframe = spr[h].frame-1;
									
									spr[h].frame = 0;
									spr[h].seq_orig = spr[h].seq;
									spr[h].seq = 0;
									spr[h].nocontrol = /*false*/0;
									
									
									if (h == 1) if (in_this_base(spr[h].seq_orig, dink_base_push))
										
									{
										
										
										play.push_active = /*false*/0;
										if (play.push_dir == 2) if (sjoy.down) play.push_active = /*true*/1;
										if (play.push_dir == 4) if (sjoy.left) play.push_active = /*true*/1;
										if (play.push_dir == 6) if (sjoy.right) play.push_active = /*true*/1;
										if (play.push_dir == 8) if (sjoy.up) play.push_active = /*true*/1;
										
										
										goto past;
										
									}
								}
								if (spr[h].seq > 0) if (seq[spr[h].seq].special[spr[h].frame] == 1)
								{
									//this sprite can damage others right now!
									//lets run through the list and tag sprites who were hit with their damage
									
									run_through_tag_list(h, spr[h].strength);
									
								}
								
								
								
								
							}
						}
					}
					
					
				}
				
				
				if (spr[h].active) if (spr[h].brain == 1)
				{
					did_player_cross_screen(/*true*/1, h);
				}		
				
past: 
				check_seq_status(spr[h].seq);
				

				// TODO: this seems to be copy
				// protection. What does it do if we
				// remove the draw_sprite_game line?

				// if (mcc == sound_support)
				draw_sprite_game(GFX_lpDDSBack, h);
				
				
				//draw a dot to show where the computer is guessing the start of the shadow is	 
				/* Note: show_dot is never set to
				   true; that's a manual debugging
				   tool; maybe we could introduce a
				   command line option, or activate it
				   along with -debug */
				/* TODO: test me! */
				if (show_dot)
				{
					
					
/* 					ddbltfx.dwSize = sizeof(ddbltfx); */
					if (drawthistime)
					{	
					  int oo;
/* 						ddbltfx.dwFillColor = 1; */
						
/* 						box_crap = k[getpic(h)].hardbox; */
						//box_crap.bottom = spr[h].y + k[spr[h].pic].hardbox.bottom;
						//box_crap.left = spr[h].x + k[spr[h].pic].hardbox.left;
						//box_crap.right = spr[h].x + k[spr[h].pic].hardbox.right;
						
						//OffsetRect(&box_crap, spr[h].x, spr[h].y);
						
						//	ddrval = lpDDSBack->Blt(&box_crap ,NULL, &box_real, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx);
						
						
						//to show center pixel
/* 						ddbltfx.dwFillColor = 100; */
						
/* 						box_crap.top = spr[h].y; */
/* 						box_crap.bottom = spr[h].y+1; */
/* 						box_crap.left = spr[h].x ; */
/* 						box_crap.right = spr[h].x + 1; */
						// GFX
						GFX_box_crap.x = spr[h].x;
						GFX_box_crap.y = spr[h].y;
						GFX_box_crap.w = 1;
						GFX_box_crap.h = 1;
						
/* 						ddrval = lpDDSBack->Blt(&box_crap ,NULL, &box_real, DDBLT_COLORFILL| DDBLT_WAIT, &ddbltfx); */
						// GFX
						SDL_FillRect(GFX_lpDDSBack, &GFX_box_crap,
							     SDL_MapRGB(GFX_lpDDSBack->format,
									GFX_real_pal[100].r,
									GFX_real_pal[100].g,
									GFX_real_pal[100].b));

						
						for (oo=0; oo <  spr[h].moveman+1; oo++)
						{
/* 							ddbltfx.dwFillColor = 50; */
							
/* 							box_crap.top = spr[h].lpy[oo]; */
/* 							box_crap.bottom = box_crap.top+1; */
/* 							box_crap.left = spr[h].lpx[oo]; */
/* 							box_crap.right = box_crap.left+1; */
							// GFX
							GFX_box_crap.x = spr[h].lpx[oo];
							GFX_box_crap.y = spr[h].lpy[oo];
							GFX_box_crap.w = 1;
							GFX_box_crap.h = 1;
							
/* 							ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx); */
							// GFX
							SDL_FillRect(GFX_lpDDSBack, &GFX_box_crap,
								     SDL_MapRGB(GFX_lpDDSBack->format,
										GFX_real_pal[50].r,
										GFX_real_pal[50].g,
										GFX_real_pal[50].b));
							
						}
/* 						ddbltfx.dwFillColor = 1; */
						
/* 						box_crap.top = spr[h].lpy[0]; */
/* 						box_crap.bottom = box_crap.top+1; */
/* 						box_crap.left = spr[h].lpx[0]; */
/* 						box_crap.right = box_crap.left+1; */
						// GFX
						GFX_box_crap.x = spr[h].lpx[0];
						GFX_box_crap.y = spr[h].lpy[0];
						GFX_box_crap.w = 1;
						GFX_box_crap.h = 1;
						
/* 						ddrval = lpDDSBack->Blt(&box_crap ,NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx); */
						// GFX
						SDL_FillRect(GFX_lpDDSBack, &GFX_box_crap,
							     SDL_MapRGB(GFX_lpDDSBack->format,
									GFX_real_pal[1].r,
									GFX_real_pal[1].g,
									GFX_real_pal[1].b));
					}
				}         
}
} /* for 0->max_s */

 
	if (mode == 0)
	{
		
	  memset(&spr[1], 0, sizeof(spr[1]));
		
		spr[1].speed = 3;
		/* init_mouse(hWndMain); */
		/* g_pMouse->Acquire(); */
		
		spr[1].timer = 0;
		spr[1].brain = 1;
		spr[1].hard = 1;
		spr[1].pseq = 2;
		spr[1].pframe = 1;
		spr[1].seq = 2;
		spr[1].dir = 2;
		spr[1].damage = 0;
		spr[1].strength = 10;
		spr[1].defense = 0;
		spr[1].skip = 0;
		rect_set(&spr[1].alt,0,0,0,0);
		spr[1].base_idle = 10;
		spr[1].base_walk = -1;
		spr[1].size = 100;		 
		spr[1].base_hit = 100;
		spr[1].active = /*TRUE*/1;
		
		int crap2 = add_sprite(0,450,8,0,0);
		
		
		spr[crap2].hard = 1;
		spr[crap2].noclip = 1;
		strcpy(spr[crap2].text, dversion_string);
		
		spr[crap2].damage = -1;
		spr[crap2].owner = 1000;
		
		
		
		int scr = load_script("START",1000, /*true*/1);
		if (locate(scr, "MAIN") == /*false*/0)
		{
			log_error("Can't locate MAIN in script START!");
		}
		run_script(scr);
		mode = 1;
		
	}
	
	
	
	if (mode == 2)
	{
		
		//	 if (  (keypressed()) | (jinfo.dwButtons) | (sjoy.right) | (sjoy.left) 
		//		 | (sjoy.up)   | (sjoy.down))
		{
			mode = 3;
			load_map(map.loc[*pmap]);
			draw_map_game();
			flife = *plife;
			
			if (keep_mouse == 0)
			{
				
				
/* 				if (g_pMouse)  */
/* 				{  */
/* 					g_pMouse->Release(); */
/* 					g_pMouse = NULL; */
/* 				} */
				
/* 				if (g_hevtMouse) */
/* 				{ */
/* 					CloseHandle(g_hevtMouse); */
/* 					g_hevtMouse = NULL; */
/* 				} */
/* 				if (g_pdi)      */
/* 				{ */
/* 					g_pdi->Release(); */
/* 					g_pdi    = NULL; */
/* 				} */
				
			}
			
			// draw_status_all();
		}
	}
	
	
	
	if (spr[1].active && spr[1].brain == 1)
	  did_player_cross_screen(/*false*/0, 1);
	
	if (trig_man)
	{
		goto trigger_start;
		
	}
	
	if (get_frame)
	{
/* 		RECT rcRect1; */
		
		total_trigger = /*true*/1;
		get_frame = /*false*/0;
/* 		rcRect1.left = playl; */
/* 		rcRect1.top = 0; */
/* 		rcRect1.right = 620; */
/* 		rcRect1.bottom = 400; */
		//return;
		
/* 		ddrval = lpDDSTrick2->BltFast( 0, 0, lpDDSBack, */
/* 			&rcRect1, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT); */
/* 		if (ddrval != DD_OK) dderror(ddrval); */
		// GFX
		{
		  SDL_Rect src, dst;
		  src.x = playl;
		  src.y = 0;
		  src.w = 620 - playl;
		  src.h = 400;
		  dst.x = dst.y = 0;
		  SDL_BlitSurface(GFX_lpDDSBack, &src, GFX_lpDDSTrick2, &dst);
		}
		
		return;
	}
	
	
	
	if (screenlock == 1)
	{
		//Msg("Drawing screenlock.");
		drawscreenlock();
		
	}
	
	
	if (debug_mode)
	  {
	    FONTS_SetTextColor(200, 200, 200);
	    if (mode == 0) strcpy(msg,"");
	    if (mode == 1)
	      {
		int x;
		sprintf(msg,"X is %d y is %d  FPS: %d", spr[1].x, spr[1].y, fps);
		//let's add the key info to it.
		for (x = 0; x < 256; x++)
		  if (GetKeyboard(x))
		    sprintf(msg + strlen(msg), " (Key %i)", x);
	      }
	    if (mode == 3)
	      {
		sprintf(msg, "Sprites: %d  FPS: %d  Show_dot: %d Plane_process: %d"
			" Moveman X%d X%d: %d Y%d Y%d Map %d",
			last_sprite_created, fps/*_show*/, show_dot, plane_process,
			spr[1].lpx[0], spr[1].lpy[0], spr[1].moveman,
			spr[1].lpx[1], spr[1].lpy[1], *pmap);
	      }
	    
	    print_text_wrap_debug(msg, 0, 0);
	    if (strlen(last_debug) > 0)
	      {
		//let's also draw this...
		strcpy(msg, last_debug);
		print_text_wrap_debug(msg, 0, 20);
	      }
	  }

        /* Console */
        if (console_active == 1)
        {
	  char* line = dinkc_console_get_cur_line();
	  FONTS_SetTextColor(0, 0, 0);
	  print_text_wrap_debug(line, 20, 380);

	  char retval[20+1];
	  sprintf(retval, "%d", dinkc_console_get_last_retval());
	  FONTS_SetTextColor(255, 0, 0);
	  print_text_wrap_debug(retval, 20, 360);
	}

	int j2 = 0;
	for (; j2 <= max_s; j2++)
	  {
	    int h = 0;
	    if (plane_process)
	      h = rank[j2];
	    else
	      h = j2;
	    if (h > 0 && spr[h].active && spr[h].brain == 8)
	      text_draw(h);
	  }
    
    
	if (talk.active) process_talk();
	
	
	process_callbacks();
	
flip:
	if (g_b_kill_app) return;	
	if (!abort_this_flip)
		flip_it(); 
	
	if (turn_on_plane) plane_process = /*TRUE*/1;
	
} /* updateFrame */
