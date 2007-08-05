/**
 * Quick'n dirty palette inversion (filter negative)

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
#define PAL_OFFSET 54
#define PAL_NB_ENTRIES 256
#define PAL_ENTRY_SIZE 4


int main(int argc, char* argv[]) {
  FILE *file;
  unsigned char buf[PAL_NB_ENTRIES][PAL_ENTRY_SIZE];
  int i;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s dir.ff\n", argv[0]);
    exit(1);
  }

  file = fopen(argv[1], "r+b");
  fseek(file, PAL_OFFSET, SEEK_SET);
  fread(buf, PAL_ENTRY_SIZE, PAL_NB_ENTRIES, file);
  
  for (i = 0; i < PAL_NB_ENTRIES; i++)
    {
      int j;
      for (j = 0; j < PAL_ENTRY_SIZE; j++)
	{
	  /* buf[i] = 0; */
	  buf[i][j] = 255 - buf[i][j];
	}
    }
  
  fseek(file, PAL_OFFSET, SEEK_SET);
  fwrite(buf, PAL_ENTRY_SIZE, PAL_NB_ENTRIES, file);

  fclose(file);
  return 0;
}
