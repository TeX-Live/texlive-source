/* Public domain.
 * Originally written by Akira Kakuto  <kakuto@fuk.kindai.ac.jp>
 *
 * DLL calling main program for TeX & Co
 * DLLPROC must be defined in the Makefile as,
 * e.g., -DDLLPROC=dlltexmain.
 */

__declspec(dllimport) DLLPROC(int ac, char **av);
int main(int ac, char **av)
{
  return (DLLPROC(ac, av));
}
