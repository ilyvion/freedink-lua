/*
 * Decomp-ile^Wress .d scripts

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
#include <string.h>

/* Dink's .d files are compressed using the Binary Pair Encoding
   algorithm, using one single block, check
   http://www.csse.monash.edu.au/cluster/RJK/Compress/problem.html for
   details. */

/* Format:
   - <nb_pairs + 128> (1 byte)
   - nb_pairs * <2 bytes> = pairs table, starting at 128
     <pair #128><pair #129><pair #130>...
   - <compressed text>, where all chars > 127 are to be replaced by the
     matching pair
*/

/* #define debug(...) fprintf(stderr, __VA_ARGS__) */
#define debug(...) 

int decompress(FILE *in, FILE *out)
{
  /* Replacement strings contain at most 2^nb_pairs_max */
  /* Memory footprint = 2MB = acceptable :) */
  char repl[128][2^128];
  memset(repl, 0, 128 * 2^128);

  /* First byte is the number of pairs + 128 */
  int nb_pairs = fgetc(in);
  nb_pairs -= 128;
  debug("nb_pairs = %d\n", nb_pairs);
  if (nb_pairs < 0)
    return -1;

  /* Read pairs table */
  for (int i = 0; i < nb_pairs; i++)
    {
      char *pc = repl[i];
      for (int n = 0; n < 2; n++)
	{
	  int c = fgetc(in);

	  if (c > i+128)
	    {
	      /* reference to a pair that is not registered yet! */
	      return -1;
	    }

	  if (c < 128)
	    {
	      /* Copy this char */
	      *pc = c;
	      pc++;
	    }
	  else
	    {
	      /* Copy the matching replacement string */
	      char *pc2 = repl[c-128];
	      while(*pc2 != '\0')
		{
		  *pc = *pc2;
		  pc++;
		  pc2++;
		}
	    }
	  *pc = '\0';
	}
    }

  /* Debug: dump replacement strings table */
  for (int i = 0; i < 128; i++)
    {
      debug("%d(%d): %s\n", i, strlen(repl[i]), repl[i]);
    }

  /* Decompress file */
  int c;
  while((c = fgetc(in)) != EOF)
    {
      debug("%d/%d ", c, c-128);
      if (c < 128)
	fputc(c, out);
      else
	fputs(repl[c-128], out);
    }
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
    {
      printf("Usage: %s script.d ...\n", argv[0]);
      exit(1);
    }

  while(*(++argv))
    {
      char *infile = *argv;
      char *outfile;
      int infile_len = strlen(infile);
      int success = -1;
      printf("Uncompressing %s...", infile);
      FILE *in;
      if ((in = fopen(*argv, "rb")) == NULL)
	{
	  perror("");
	}
      else
	{
	  FILE *out;
	  outfile = malloc(infile_len + 1);
	  strcpy(outfile, infile);
	  outfile[infile_len - 1]--; /* D->C */
	  debug("outfile=%s\n", outfile);
	  if ((out = fopen(outfile, "wb")) == NULL)
	    {
	      perror("");
	    }
	  else
	    {
	      success = decompress(in, out);
	      fclose(out);
	    }
	  free(outfile);
	  fclose(in);
	}
      if (success != 0)
	printf("failed!");
      else
	printf("done!");
      printf("\n");
    }
  return 0;
}

/**
 * Local Variables:
 * compile-command: "gcc -Wall -std=c99 -pedantic -g d2c.c -o d2c"
 * End:
 */
