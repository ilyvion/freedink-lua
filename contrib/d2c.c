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
#include <assert.h>
#define NB_PAIRS_MAX 128

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

/* Simple run with 3 pairs: */
/*
- Pairs:
0 = 65 ('a'), 65 ('a') // coded as 128
1 = 128, 128           // coded as 129
2 = 129, 129           // coded as 130
- Text body:
130
- Stack (bottom is left):
-> (0)
-> 130
-> 129, 129
-> 129, 128, 128
-> 129, 128, 65, 65
-> 129, 128, 65 [output => 65]
-> 129, 128 [output => 65]
-> 129, 65, 65
-> 129, 65 [output => 65]
-> 129, [output => 65]
-> 129
-> 128, 128
-> 128, 65, 65
-> 128, 65 [output => 65]
-> 128 [output => 65]
-> 65, 65
-> 65 [output => 65]
-> (0) [output => 65]
- Output:
aaaaaaaa
*/

#ifdef DEBUG
#define debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug(...) 
#endif

int decompress(FILE *in, FILE *out)
{
  /* Replacement strings contain at most 2^NB_PAIRS_MAX
     (or 1<<NB_PAIRS_MAX) */
  /* Maximum length of a substitution = 2^128 =~ 2x10^38 => can't be
     cached. Instead we'll stack the pairs references in the body of
     the compressed text and pop them until the stack is
     empty. There's a maximum of 128 chained pairs, so the stack has a
     size of 128 + 1 (last substitution references non-pairs). */
  unsigned char pairs[NB_PAIRS_MAX][2];
  unsigned char stack[NB_PAIRS_MAX+1];
  int stack_top = -1; /* empty stack */

  /* First byte is the number of pairs + 128 */
  int nb_pairs = fgetc(in);
  nb_pairs -= NB_PAIRS_MAX;
  debug("nb_pairs = %d\n", nb_pairs);
  if (nb_pairs < 0)
    return -1; /* Invalid header: negative number of pairs */

  /* Read pairs table */
  for (int i = 0; i < nb_pairs; i++)
    {
      for (int j = 0; j < 2; j++)
	{
	  int c = fgetc(in);
	  if (c == EOF)
	    return -1; /* Invalid header: truncated pair table */
	  if (c > i+128)
	    return -1; /* Invalid header: reference to a pair that is not registered yet */
	  pairs[i][j] = c;
	}
    }

  /* Debug: dump pair table */
  for (int i = 0; i < nb_pairs; i++)
    {
      debug("pairs[%d]: %d . %d\n", i,  pairs[i][0], pairs[i][1]);
    }

  /* Decompress file */
  while (1)
    {
      assert(stack_top < nb_pairs+1);
      if (stack_top < 0) /* empty stack */
	{
	  int c = fgetc(in);
	  if (c == EOF) /* end of file */
	    break;
	  else if (c < 128)
	    fputc(c, out);
	  else
	    stack[++stack_top] = c;
	}
      else
	{
	  if (stack[stack_top] < 128)
	    fputc(stack[stack_top--], out);
	  else
	    {
	      unsigned char cur_pair = stack[stack_top--] - 128;
	      if (cur_pair >= nb_pairs)
		return -1; /* Invalid body: references non-existent pair */
	      stack[++stack_top] = pairs[cur_pair][1];
	      stack[++stack_top] = pairs[cur_pair][0];
	    }
	}
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
      printf("Uncompressing %s... ", infile);
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
