#define WIN32_LEAN_AND_MEAN
#define _WIN32_IE 0x0401
#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
int main(void)
{
  char buf[MAX_PATH];
  SHGetSpecialFolderPath(NULL, buf, CSIDL_PERSONAL, 1);
  printf("%s\n", buf); /* C:\Documents and Settings\name\Mes documents */
  SHGetSpecialFolderPath(NULL, buf, CSIDL_APPDATA, 1);
  printf("%s\n", buf); /* C:\Documents and Settings\name\Application Data */
  SHGetSpecialFolderPath(NULL, buf, CSIDL_FONTS, 1);
  printf("%s\n", buf);/* C:\WINNT\Fonts */
  SHGetSpecialFolderPath(NULL, buf, CSIDL_DESKTOP, 1);
  printf("%s\n", buf); /* C:\Documents and Settings\name\Bureau */
  SHGetSpecialFolderPath(NULL, buf, CSIDL_DESKTOPDIRECTORY, 1);
  printf("%s\n", buf); /* C:\Documents and Settings\name\Bureau */
}
