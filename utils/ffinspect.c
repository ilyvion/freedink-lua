/**
 * Dumps a dir.ff file, checking for common errors.

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

int main(int argc, char* argv[]) {
  FILE *file;
  int fsize;
  int nb_entries;
  int nb_errors = 0;
  int i;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s dir.ff\n", argv[0]);
    exit(1);
  }

  file = fopen(argv[1], "rb");

  /* Check file size */
  fseek(file, 0, SEEK_END);
  fsize = ftell(file);

  /* Read how the number of files in the archive */
  fseek(file, 0, SEEK_SET);
  fread(&nb_entries, sizeof(int), 1, file);
  printf("Nb entries: %d\n", nb_entries);

  if (nb_entries < 0) {
    fprintf(stderr, "Negative number of files\n");
    exit(1);
  }

  for (i = 0; i < nb_entries - 1; i++) {
    int offset;
    char filename[12+1]; /* 8.3\0 */
    fread(&offset, sizeof(int), 1, file);
    fread(filename, 13*sizeof(char), 1, file);
    if (filename[12] != '\0')
      {
	filename[12] = '\0';
	printf("\t// Error: filename string does not end with \\0");
	nb_errors++;
      }

    printf("%d %s", offset, filename);
    if (offset <= 0 || offset > fsize) {
      printf("\t// Error: invalid index");
      nb_errors++;
    }
    if (i == 0 && offset != (sizeof(int) + (sizeof(int) + (12+1) * sizeof(char)) * nb_entries))
      {
	printf("\t// Error: first index does not match the start of the data area");
	nb_errors++;
      }
    printf("\n");
  }
  {
    int last_offset;
    fread(&last_offset, sizeof(int), 1, file);
    printf("Last offset: %d\n", last_offset);
    printf("File size  : %d\n", fsize);
    if (last_offset != fsize) {
      printf("// Error: file size differs from last offset\n");
      nb_errors++;
    }
  }
  if (nb_errors > 0)
    printf("Warning: %d error(s) detected\n", nb_errors);
  return 0;
}

/**
 * Local Variables:
 * compile-command: "gcc -Wall ffinspect.c -o ffinspect"
 * End:
 */
