#include <malloc.h>
void mymemstat()
{
  struct mallinfo mi;
  mi = mallinfo();
  printf("arena: %d\n", mi.arena);
  printf("uordblks: %d\n", mi.uordblks);
  printf("fordblks: %d\n", mi.fordblks);
  printf("\n");
}
