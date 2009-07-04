/*
 * Recursively pack dir.ff FastFile archives

 * Copyright (C) 2002, 2003, 2007, 2009  Sylvain Beucler

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

/* sortable list of filenames */
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

/* size of read block */
/* fastfile name */
#define FASTFILE_NAME "dir.ff"

#define DEBUG(...) printf(__VA_ARGS__)
/* #define DEBUG(...) */

void ffrpack(void);
void ffpack(const char *filename);

int main(int argc, char *argv[])
{
  /* Recursive packing starting with current directory */
  DEBUG("Starting ffrpack from ./\n");
  ffrpack();
  return 0;
}


/* Pack one dir.ff for all files in the directory */
void ffpack(const char *dir)
{
  #define FILENAME_SIZE 12
  FILE *fin = NULL, *fout = NULL;
  struct subfile
  {
    int offset;
    char filename[FILENAME_SIZE + 1];
  } *subfiles;
  /* warning: sizeof(struct subfile) = 20 != 17... */

  DEBUG(" Parsing %s\n", dir);

  /* Now check if there's a matching entry in the directory */
  DIR *list = opendir(".");
  vector<string> files;
  if (list != NULL)
    {
      struct dirent *entry;
      while ((entry = readdir(list)) != NULL)
	{
	  if (strcmp(entry->d_name, ".") == 0
	      || strcmp(entry->d_name, "..") == 0
	      || strcasecmp(entry->d_name, "dir.ff") == 0)
	    continue;
	  if (entry->d_type != DT_REG)
	    continue;
	  files.push_back(string(entry->d_name));
	}
      closedir(list);
    }
  sort(files.begin(), files.end());

  /* Don't create a dir.ff for empty directories */
  if (files.size() == 0)
    return;

  fout = fopen(FASTFILE_NAME, "w");
  if (fout == NULL)
    {
      perror("ffpack");
      return;
    }
  /* allocate some memory to store the {offset/filename}s */
  int nb_entries = files.size() + 1;
  subfiles = (struct subfile*) calloc(nb_entries, sizeof(struct subfile));

  /* skip space for the offsets&filenames table */
  fseek(fout, 4 + nb_entries * (4 + FILENAME_SIZE + 1), SEEK_CUR);

  /* Copy files contents */
  vector<string>::iterator iter;
  int i = 0;
  for (iter = files.begin(); iter != files.end(); iter++)
    {
      /* Update table of contents */
      const char* filename = iter->c_str(); /* internal pointer, valid until
					 next call to *iter */
      strncpy(subfiles[i].filename, filename, FILENAME_SIZE);
      subfiles[i].offset = ftell(fout);
      i++;

      fin = fopen(filename, "r");
      if (fin == NULL)
	{
	  perror("ffpack");
	  return;
	}

      /* Embed file in the fastfile */
      char buffer[BUFSIZ];
      while (fread(buffer, BUFSIZ, 1, fin) > 0 && !feof(fin))
	fwrite(buffer, BUFSIZ, 1, fout);
      int remaining = ftell(fin) % BUFSIZ;
      fwrite(buffer, remaining, 1, fout);
      
      fclose(fin);
    }
  /* Define last pseudo-entry */
  strcpy(subfiles[i].filename, "");
  subfiles[i].offset = ftell(fout);


  /* Write table of contents */
  {
    fseek(fout, 0, SEEK_SET);
    /* write the number of entries (nb files + 1) */
    fwrite(&nb_entries, sizeof(nb_entries), 1, fout);

    int i = 0;
    for (i = 0; i < nb_entries; i++)
      {
	fwrite(&subfiles[i].offset, sizeof(subfiles[i].offset), 1, fout);
	fwrite(&subfiles[i].filename, FILENAME_SIZE + 1, 1, fout);
      }
  }

  free(subfiles);
  fclose(fout);
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

/* Pack dir.ff's recursively in all subdirectories. It inserts all
   files except for 'dir.ff'. */
void ffrpack(void)
{
  short int n, i;
  struct dirent **namelist;
  
  /* pack current dir.ff file */
  ffpack(FASTFILE_NAME);

  /* do the same thing in the subdirectories */
  n = scandir(".", &namelist, isDir, alphasort);
  DEBUG(" %d subdir%s found; err=%s\n",
	 n, (n>1)?"s":"", (n==-1)?strerror(errno):"no error");
  for (i = 0; i < n; i++)
    {
      DEBUG(" - %s\n", namelist[i]->d_name);
      if (chdir(namelist[i]->d_name) < 0)
	{
	  fprintf(stderr, "ffrpack: Error#%d (%s) when entering %s\n",
		  errno, strerror(errno), namelist[i]->d_name);
	  fprintf(stderr, "Ignoring %s\n", namelist[i]->d_name);
	  continue;
	}

      DEBUG("Entering %s\n", namelist[i]->d_name);
      ffrpack();
      if (chdir("..") < 0)
	{
	  fprintf(stderr, "ffrpack: Error#%d (%s) while quitting dir.\n",
		  errno, strerror(errno));
	  fprintf(stderr, "Stopping packing process\n");
	  exit(1);
	}
    }
}

/**
 * Local Variables:
 * compile-command: "g++ -Wall ffrpack.cpp -o ffrpack"
 * End:
 */
