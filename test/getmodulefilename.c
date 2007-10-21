#include <windows.h>
#include <stdio.h>

int main(void)
{
  char buf[MAX_PATH];
  int ok = GetModuleFileName(NULL, buf, MAX_PATH);
  if (ok)
    printf("%s\n", buf);
}
