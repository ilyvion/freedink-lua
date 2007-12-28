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
/* fastfile name */
#define FASTFILE_NAME "dir.ff"

#define DEBUG(...) printf(__VA_ARGS__)
/* #define DEBUG(...) */

#define IMIN(a,b) ((a < b) ? a : b)
#define IMAX(a,b) ((a > b) ? a : b)

void ffrextract(void);
void ffextract(char *filename);

int main(int argc, char *argv[])
{
  /* start from current directory */
  if (argc > 1)
    {
      /* We're passed a list of files to extract */
      int i = 1;
      for (i = 1; i < argc; i++)
	ffextract(argv[i]);
    }
  else
    {
      /* Recursive extraction starting with current directory */
      DEBUG("Starting ffrextract from ./\n");
      ffrextract();
    }
  return 0;
}


/* extract dir.ff */
void ffextract(char *filename)
{
  #define FILENAME_SIZE 12
  FILE *fin = NULL, *fout = NULL;
  struct subfile
  {
    long int offset;
    char filename[FILENAME_SIZE + 1];
  } *subfiles;
  /* warning: sizeof(struct subfile) = 20 != 17... */

  int nb_entries = 0;
  DEBUG(" Parsing %s\n", filename);

  /* grab the number of files */
  fin = fopen(filename, "r");
  fread(&nb_entries, sizeof(nb_entries), 1, fin);

  /* allocate some memory to store the {offset/filename}s */
  subfiles = (struct subfile*) malloc(nb_entries * sizeof(struct subfile));
  {
    int i = 0;
    for (i = 0; i < nb_entries; i++)
      {
	fread(&subfiles[i].offset, sizeof(subfiles[i].offset), 1, fin);
	fread(&subfiles[i].filename, FILENAME_SIZE + 1, 1, fin);
	/* Support badly generated dir.ff such as Mystery Island's or
	   inter/Text-box/dir.ff */
	if (subfiles[i].offset <= 0)
	  {
	    /* skip that entry */
	    subfiles[i].filename[FILENAME_SIZE] = '\0';
	    DEBUG(" %s: skipping bad subfile %s (invalid offset %ld)\n",
		  filename, subfiles[i].filename, subfiles[i].offset);
	    i--;
	    nb_entries--;
	  }
      }
  }

  /* grab all the (n-1) files - the n-th 'file' only tells where EOF
     is. This last entry is present in all dir.ff I checked. */
  {
    char *output_file = NULL;
    int i = 0;
    int nb_subfiles = 0;
    char buffer[BUFSIZ];
    nb_subfiles = nb_entries - 1;
    output_file = (char*)malloc(FILENAME_SIZE + 1);
    for (i = 0; i < nb_subfiles; i++)
      {
	int remaining = -1;
	strcpy(output_file, subfiles[i].filename);
	fout = fopen(output_file, "w");
	
	/* read while a full block can be read */
	remaining = subfiles[i+1].offset - subfiles[i].offset;
	while (remaining > 0)
	  {
	    if ((fread(buffer, IMIN(BUFSIZ, remaining), 1, fin)) != 1)
	      if (feof(fin))
		{
		  fprintf(stderr, "%s: subfile offset is beyond end of file.\n", filename);
		  break;
		}
	    fwrite(buffer, IMIN(BUFSIZ, remaining), 1, fout);
	    remaining -= BUFSIZ;
	    /* printf("  pos = %d\n", cur_offset); */
	  }
	fclose(fout);
      }
    free(output_file);
  }

  fclose(fin);
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
      ffextract(FASTFILE_NAME);
  else if (errno != ENOENT) /* file exist but cannot be accessed */
    {
      fprintf(stderr, "Error #%d when accessing %s\n", errno, FASTFILE_NAME);
      exit(1);
    }
  /* else { dir.ff does not exist in this directory, nothing to do } */

  /* do the same thing in the subdirectories */
  n = scandir(".", &namelist, isDir, alphasort);
  DEBUG(" %d subdir%s found; err=%s\n",
	 n, (n>1)?"s":"", (n==-1)?strerror(errno):"no error");
  for(i = 0; i < n; i++)
    {
      DEBUG(" - %s\n", namelist[i]->d_name);
      if (chdir(namelist[i]->d_name) < 0)
	{
	  fprintf(stderr, "ffrextract: Error#%d (%s) when entering %s\n",
		  errno, strerror(errno), namelist[i]->d_name);
	  fprintf(stderr, "Ignoring %s\n", namelist[i]->d_name);
	  continue;
	}

      DEBUG("Entering %s\n", namelist[i]->d_name);
      ffrextract();
      if (chdir("..") < 0)
	{
	  fprintf(stderr, "ffrextract: Error#%d (%s) while quitting dir.\n",
		  errno, strerror(errno));
	  fprintf(stderr, "Stopping extraction process\n");
	  exit(1);
	}
    }
}

/**
 * Local Variables:
 * compile-command: "gcc -Wall ffrextract.c -o ffrextract"
 * End:
 */
