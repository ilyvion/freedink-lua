/* compress.c -- Byte Pair Encoding compression */
/* Copyright 1996 Philip Gage */

#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 65535L  /* Input file buffer size */
#define HASHSIZE 8192   /* Hash table size, power of 2 */
#define THRESHOLD   3   /* Increase for speed, min 3 */

void compress (FILE *input, FILE *output)
{
  unsigned char *buffer, *left, *right, *count;
  unsigned char a, b, bestcount=0, pairtable[128][2];
  int i, j, index, bestindex, code=128;
  size_t size;

  /* Dynamically allocate buffers and check for errors */
  buffer = (unsigned char *)malloc(MAXSIZE);
  left = (unsigned char *)malloc(HASHSIZE);
  right = (unsigned char *)malloc(HASHSIZE);
  count = (unsigned char *)malloc(HASHSIZE);
  if (buffer==NULL || left==NULL ||
      right==NULL || count==NULL) {
    printf("Error allocating memory\n");
    exit(1);
  }

  /* Read input file into buffer and check for errors */
  size = fread(buffer,1,MAXSIZE,input);
  if (size == MAXSIZE) {
    printf("Dink.c file too big.  64K is the max, pal.\n");
    exit(1);
  }
  for (i=0; i<size; i++)
    if (buffer[i] > 127) {
      printf("This program cannot compile this.  Only text files!\n");
      exit(1);
    }

  do {  /* Replace frequent pairs with bytes 128..255 */

    /* Enter counts of all byte pairs into hash table */
    memset(count,0,HASHSIZE);
    for (i=0; i<size-1; i++) {
      a = buffer[i];
      b = buffer[i+1];
      index = (a ^ (b << 6)) & (HASHSIZE-1);
      while ((left[index] != a || right[index] != b) &&
             count[index] != 0)
        index = (index + 1) & (HASHSIZE-1);
      left[index] = a;
      right[index] = b;
      if (count[index] < 255)
        ++count[index];
    }

    /* Search hash table for most frequent pair */
    bestcount = THRESHOLD - 1;
    for (i=0; i<HASHSIZE; i++) {
      if (count[i] > bestcount) {
        bestcount = count[i];
        bestindex = i;
      }
    }

    /* Compress if enough occurrences of pair */
    if (bestcount >= THRESHOLD) {

      /* Add pair to table using code as index */
      a = pairtable[code-128][0] = left[bestindex];
      b = pairtable[code-128][1] = right[bestindex];

      /* Replace all pair occurrences with unused byte */
      for (i=0, j=0; i<size; i++, j++)
        if (a == buffer[i] && b == buffer[i+1]) {
          buffer[j] = code;
          ++i;
        }
        else
          buffer[j] = buffer[i];
      size = j;
    }
    else
      break;
  } while (++code < 255);

  /* Write pair count, pair table and compressed data */
  putc(code,output);
  fwrite(pairtable,2,code-128,output);
  fwrite(buffer,1,size,output);
  free(buffer); free(left); free(right); free(count);
}

int main (int argc, char **argv)
{
  FILE *in, *out;
char crap[100];
char crap2[100];

printf("\nDinkC Compiler V1.00 - RTsoft, Inc.  All rights reserved.\n\n");
if (argc > 1)
{
//printf("Char is %c",argv[1][strlen(argv[1])-2]);
	if (argv[1][strlen(argv[1])-2] != '.')
{
	
	strcpy(crap, argv[1]);
strcat(crap, ".c");
strcpy(crap2, argv[1]);
strcat(crap2, ".d");
} else
{

		strcpy(crap, argv[1]);
strcpy(crap2, argv[1]);
crap2[strlen(crap2)-1] = 'd';

}
}


  if (argc != 2)
    printf("Usage: compile crap [.c] (a file called crap.d will be made)\n");
  else
	  if ((in=fopen(crap,"rb"))==NULL)
    printf("Error opening input %s\n",crap);
  else if ((out=fopen(crap2,"wb"))==NULL)
    printf("Error opening output %s\n",crap2);
  else {
  
 printf("Creating %s....", crap2);	  
	  compress(in,out);
    fclose(out);
    fclose(in);
 printf("Done!\n");	  

 
  }
  return 0;
}
