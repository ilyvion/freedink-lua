  int tmalloc = 0;
  while (malloc(1024) != NULL)
    tmalloc += 1024;
/*   while (malloc(1024*1024) != NULL) */
/*     tmalloc += 1024*1024; */
  printf("tmalloc = %d\n", tmalloc);
  /* FW 1.50 => 10910720 / 10485760 (normally 24000kB) */
  /* FW 5.00 => 39819264 / 39845888 (normally 52500kB) */ 
  exit(0);
