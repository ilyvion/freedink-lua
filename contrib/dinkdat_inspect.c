#include <stdio.h>
#include <stdlib.h>

/**
 * Some info on the subject:
 * http://www.ioplex.com/~miallen/encdec/
 */

/**
 * Read integer portably (same result with MSB and LSB endianness)
 */
int read_lsb_int(FILE *f)
{
  unsigned char buf[4];
  fread(buf, 4, 1, f);
  return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
}

int main(int argc, char *argv[])
{
  if (argc != 2)
    {
      printf("Usage: %s dink.dat\n", argv[0]);
      exit(1);
    }

  /* dink.dat contains the screen indices in map.dat, for each of the
     768 screens sequencially. It may go behond index 768 (e.g. in the
     Quest for Dorinthia 2 which goes up to 2576 - maybe after using
     tools such as MapNuke?) */
  /* Format: */
  /* - 20 chars */
  /* - A first dummy integer (to ease programming in Dinkedit because screen number counts from 1, not 0 */
  /*  - 768 signed integers on 4 bytes, describing screen index in map.dat */
  /* - 1 dummy integer */
  /*  - 768 signed integers, describing the screen music number */
  /* - 1 dummy integer */
  /*  - 768 signed integers, describing the screen indoor status (yes/no) */
  /* - And then, unused blank data (2240 bytes) */
  FILE *f = fopen(argv[1], "rb");
  if (f == NULL)
    {
      perror("fopen");
      exit(2);
    }
  fseek(f, 24, SEEK_SET);
  for (int i = 0; i < 768; i++)
    {
      printf("Screen #%03d: %d\n", (i+1), read_lsb_int(f));
    }
}

/* Size of 1 screen in MAP.DAT: 31280 bytes. 768 screens = 22MB. */

/**
 * Local Variables:
 * compile-command: "gcc -Wall -std=c99 dinkdat_inspect.c -o dinkdat_inspect"
 * End:
 */
