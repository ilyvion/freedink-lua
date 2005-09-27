#include <stdio.h>

enum youp {a=5000000000, b, c};
int main(void) {
  youp test_enum = a;
  printf("%d\n", test_enum);
  printf("%d\n", sizeof(test_enum));
  return 0;
}
