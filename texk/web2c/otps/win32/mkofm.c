/* currently script mkofm only calls mktextfm */
#include <stdio.h>
#include <string.h>
#include <process.h>
int main(int argc, char *argv[])
{
  strcpy(argv[0], "mkofm");
  return spawnvp(_P_WAIT, "mktextfm.exe", (const char* const*)argv);
}
