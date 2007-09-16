/*
 * Recursively extract dir.ff FastFile archives

 * Copyright (C) 2002, 2003, 2007  Sylvain Beucler

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

/*
  This is my first C program, so be indulgent :)
  TODO:
  Replace scandir, which is only BSD compliant, by something at least
    POSIX compliant. And then, add some Windows portability, using
    #ifdefs or wx's lib
  Try to simplify
  Cleanup globals
  Comment the code and find some better variable names
  I think there is a crash on the Mystery Island's only dir.ff, due to
    incorrect offset in the header. So the code should check whether
    the offset is a valid one (between the last offset and the next
    one, for example), and if not, ignore the entry (with some nice
    stderr output)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* for errno */
#include <errno.h>

/* for stat() */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* for scandir() */
#include <dirent.h>


/* size of read block */
#define BLOCK_SIZE (512)

/* fastfile name */
#define FASTFILE_NAME "dir.ff"

void ffrextract(void);

int main(void)
{
  /* start from current directory */
  printf("Starting ffrextract from ./\n");
  ffrextract();
  return 0;
}


/* extract dir.ff */
void ffextract(void)
{
  #define FILENAME_SIZE 12
  #define FILECHAR_SIZE 1
  /* that is a lame attempt to make the app runnable
     in a Unicode(16bit/char) system.
     it won't work, but maybe I'll remember to correct that a day... */
  FILE *fin, *fout;
  struct subfile
  {
    long int offset;
    char filename[FILENAME_SIZE + 1];
  } *cur_file, *beginning;
  /* warning: sizeof(struct subfile) = 20 != 17... */

  long int n, i;
  long int cur_offset;
  char *buffer, *ptr, *output_file;

  printf(" Parsing %s\n", FASTFILE_NAME);

  /* grab the number of files */
  fin = fopen(FASTFILE_NAME, "r");
  fread(&n, sizeof(n), 1, fin);

  /* allocate some memory to store the {offset/filename}s */
  beginning = (struct subfile*) malloc(n * sizeof(struct subfile));
  cur_file = beginning;
  ptr = (char*) beginning;
  for (i = 0; i < n; i++)
    {
      fread(&cur_file[i].offset, sizeof(cur_file->offset), 1, fin);
      fread(&cur_file[i].filename, FILECHAR_SIZE, FILENAME_SIZE + 1, fin);
    }

  /* grab all the (n-1) files
     the n-th 'file' only tells where EOF is */
  buffer = (char*) malloc(BLOCK_SIZE * sizeof(char));
  output_file = (char*) malloc((FILENAME_SIZE + 1) * sizeof(char));

  for(i = 0; i < (n-1); i++, cur_file++)
    {
      strcpy(output_file, cur_file->filename);
      fout = fopen(output_file, "w");

      /* read while a full block can be read */
      cur_offset = cur_file->offset;
      /* printf(" Read from %d to %d\n", cur_offset, (cur_file+1)->offset); */
      while( ((cur_file + 1)->offset - cur_offset) > BLOCK_SIZE )
	{
	  fread(buffer, BLOCK_SIZE * sizeof(char), 1, fin);
	  fwrite(buffer, BLOCK_SIZE * sizeof(char), 1, fout);
	  cur_offset += BLOCK_SIZE;
	  /* printf("  pos = %d\n", cur_offset); */
	}
      fread(buffer, ((cur_file + 1)->offset - cur_offset) * sizeof(char), 1, fin);
      fwrite(buffer, ((cur_file + 1)->offset - cur_offset) * sizeof(char),
	     1, fout);
      fclose(fout);
    }
  free(output_file);
  free(buffer);
  fclose(fin);
  free(beginning);
}

/* return whether the dirent structure points to a directory */
int isDir(const struct dirent *entry)
{
  struct stat buf;
  const char *filename = entry->d_name;
  if (strcmp(filename, ".") == 0
      || strcmp(filename, "..") == 0)
    return 0; /* do not take them */
  else
    {
      if (stat(entry->d_name, &buf) == 0)
	return (S_ISDIR(buf.st_mode));
      else
	{
	  fprintf(stderr, "isDir: %s while stating %s\n",
		  strerror(errno), entry->d_name);
	  exit(1);
	}
    }
}

/* extract dir.ff's recursively in all (FAT) subdirectories.
   FAT => links not supported here (yet) */
void ffrextract(void)
{
  struct stat cur_fastfile;
  short int n, i;
  struct dirent **namelist;
  
  /* extract current dir.ff file if it exists */
  if (stat(FASTFILE_NAME, &cur_fastfile) == 0) /* file exists */
      ffextract();
  else if (errno != ENOENT) /* file exist but cannot be accessed */
    {
      fprintf(stderr, "Error #%d when accessing %s\n", errno, FASTFILE_NAME);
      exit(1);
    }
  /* else { dir.ff does not exist in this directory, nothing to do } */

  /* do the same thing in the subdirectories */
  n = scandir(".", &namelist, isDir, alphasort);
  printf(" %d subdir%s found; err=%s\n",
	 n, (n>1)?"s":"", (n==-1)?strerror(errno):"no error");
  for(i = 0; i < n; i++)
    {
      printf(" - %s\n", namelist[i]->d_name);
      if (chdir(namelist[i]->d_name))
	{
	  fprintf(stderr, "ffrextract: Error#%d (%s) when entering %s\n",
		  errno, strerror(errno), namelist[i]->d_name);
	  fprintf(stderr, "Ignoring %s\n", namelist[i]->d_name);
	}
      else
	{
	  printf("Entering %s\n", namelist[i]->d_name);
	  ffrextract();
	  if (chdir(".."))
	    {
	      fprintf(stderr, "ffrextract: Error#%d (%s) while quitting dir.\n",
		      errno, strerror(errno));
	      fprintf(stderr, "Stopping extraction process\n");
	      exit(1);
	    }
	  
	}
    }
}
