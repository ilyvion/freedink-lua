#include <fontconfig/fontconfig.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Get filename for canonical font name 'fontname'. Return NULL if the
   font cannot be found (for correctness, no replacement font will be
   provided) */
char* get_fontconfig_path(char* fontname)
{
  char* filename = NULL;
  FcPattern* p = NULL;
  FcChar8* strval = NULL;
  FcObjectSet *attr = NULL;

  if (!FcInit())
    {
      fprintf(stderr, "get_fontconfig_path: cannot initialize fontconfig\n");
      return NULL;
    }

  p = FcNameParse((FcChar8*)fontname);
  if (p == NULL)
    {
      fprintf(stderr, "get_fontconfig_path: invalid font pattern\n");
      return NULL;
    }    
  /* Grab filename attribute */
  attr = FcObjectSetBuild (FC_FILE, (char *) 0);

  FcFontSet *fs = FcFontList (0, p, attr);
  if (fs->nfont == 0)
    {
      fprintf(stderr, "get_fontconfig_path: no matching font\n");
      return NULL;
    }
  if (FcPatternGetString(fs->fonts[0], FC_FILE, 0, &strval) == FcResultTypeMismatch
      || strval == NULL)
    {
      fprintf(stderr, "get_fontconfig_path: cannot find font filename\n");
      return NULL;
    }

  filename = strdup((char*)strval);

  FcFontSetDestroy(fs);
  FcObjectSetDestroy(attr);
  FcPatternDestroy(p);
  FcFini();

  return filename;
}

int main(void)
{

  printf("\n");
  printf("Default search paths\n");
  printf("--------------------\n");
  FcInit();
  FcStrList* paths = FcConfigGetFontDirs(NULL);
  FcChar8* cur_path = NULL;
  while ((cur_path = FcStrListNext(paths)) != NULL)
    {
      printf("%s\n", cur_path);
    }
  FcStrListDone(paths);
  FcFini();

  printf("\n");
  printf("Searching 'Liberation Sans:style=Regular'\n");
  printf("-----------------------------------------\n");

  char* path = get_fontconfig_path("Liberation Sans:style=Regular");
  if (path != NULL)
    printf("%s\n", path);
  else
    printf("<not found>\n", path);

  if (path != NULL)
    free(path);

  return 0;
}
