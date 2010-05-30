/**
 * Dink.ini - loading graphics

 * Copyright (C) 1997, 1998, 1999, 2002, 2003  Seth A. Robinson
 * Copyright (C) 2008, 2009  Sylvain Beucler

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

#include <stdlib.h>
#include "game_engine.h"
#include "paths.h"
#include "dinkvar.h"
#include "dinkini.h"
#include "log.h"

#include "gfx_sprites.h"

static int nb_idata = 0;
struct idata
{
  enum idata_type type;
  int seq;
  int frame;
  int xoffset, yoffset;
  rect hardbox;
};
static struct idata *id;

/**
 * Allocate idata
 */
void dinkini_init()
{
  nb_idata = 600;
  if (dversion >= 108)
    nb_idata = 1000;
  id = calloc(sizeof(struct idata), nb_idata);
}

/**
 * Deallocate idata
 */
void dinkini_quit()
{
  if (id != NULL)
    free(id);
  id = NULL;
}

/**
 * Store sprite instructions from dink.ini
 */
void make_idata(enum idata_type type, int myseq, int myframe, int xoffset, int yoffset, rect crect)
{
  int i;
  for (i = 1; i < nb_idata; i++)
    {
      if (id[i].type == IDATA_EMPTY)
	{
	  //found empty one
	  id[i].type = type;
	  id[i].seq = myseq;
	  id[i].frame = myframe;
	  id[i].xoffset = xoffset;
	  id[i].yoffset = yoffset;
	  rect_copy(&id[i].hardbox, &crect);
	  
	  return;
	}
    }
  
  log_error("Out of idata spots (max is %d), no more sprite corrections can be allowed.", nb_idata);
}

/**
 * Interpret/execute sprite instructions
 */
void program_idata(void)
{
  int i;
  for (i = 1; i < nb_idata; i++)
    {
      if (id[i].type == IDATA_EMPTY)
	return;

      if (id[i].type == IDATA_SPRITE_INFO)
	{
	  k[seq[id[i].seq].frame[id[i].frame]].xoffset = id[i].xoffset;
	  k[seq[id[i].seq].frame[id[i].frame]].yoffset = id[i].yoffset;
	  rect_copy(&k[seq[id[i].seq].frame[id[i].frame]].hardbox, &id[i].hardbox);
	  
	  // Msg("Programming idata type %d in %d...Seq %d Frame %d (Hardbox is %d %d %d %d)", id[i].type, i,
	  //     id[i].seq, id[i].frame, id[i].hardbox.left,id[i].hardbox.right, id[i].hardbox.top, id[i].hardbox.bottom);
	}
      
      if (id[i].type == IDATA_FRAME_SPECIAL)
	{
	  //set special
	  seq[id[i].seq].special[id[i].frame] = id[i].xoffset;
	}
      if (id[i].type == IDATA_FRAME_DELAY)
	{
	  //set delay
	  seq[id[i].seq].delay[id[i].frame] = id[i].xoffset;
	}
      
      if (id[i].type == IDATA_FRAME_FRAME)
	{
	  if (id[i].xoffset == -1)
	    seq[id[i].seq].frame[id[i].frame] = -1;
	  else
	    seq[id[i].seq].frame[id[i].frame] = seq[id[i].xoffset].frame[id[i].yoffset];
	}
    }
}


/* Parse dink.ini */
void load_batch(void)
{
  FILE *in = NULL;
  char line[255];
  
  log_info("Loading dink.ini");
  /* Open the text file in binary mode, so it's read the same way
     under different OSes (Unix has no text mode) */
  if ((in = paths_dmodfile_fopen("dink.ini", "rb")) == NULL)
    log_error("Error opening dink.ini for reading.");
  else
    {
      while(fgets(line, 255, in) != NULL) 
	{
	  pre_figure_out(line);
	  /* printf("[pre_figure_out] %s", line); */
	}
      fclose(in);
    }

  program_idata();
}
