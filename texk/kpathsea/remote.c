/*
  Handling of remote files with kpathsea.
*/

#include <kpathsea/config.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/hash.h>
#include <geturl.h>

hash_table_type *remote_db;

static char *log_buffer;
int index;

void __cdecl DoDownloadProgress(unsigned long partial, unsigned long total)
{
  sprintf(log_buffer+index," -> %8ld/%8ld\r", partial, total);
  fputs(log_buffer, stdout);
}

void __cdecl DoDownloadLog(char *s, ...)
{
#if 0
  va_list argList;
  fputc('\n', stderr);
  va_start (argList, s);
  vfprintf(stderr, s, argList);
  va_end (argList);
#endif
}

string get_remote_file(const_string filename)
{
  string *lookup = NULL;
  string localname = NULL;
  boolean ret;

  if ((remote_db = hash_exists_p(hashtable_remote)) == NULL) {
    remote_db = hash_create (1007, hashtable_remote);
  }

  lookup = hash_lookup(remote_db, filename);
  /* Either the file has already been downloaded */
  if (lookup && *lookup) {
    return *lookup;
  }
  /* Or it is a new one */
  /* Get some local name */
  localname =_tempnam(getenv("TMP"), "kpse");
  if (localname == NULL) return localname;
  
  index = strlen(filename);
  log_buffer = xmalloc(index + 24);
  strcpy(log_buffer, filename);

  ret = (get_url_to_file(filename, 
			 localname,
			 0, 
			 DoDownloadLog, 
			 DoDownloadProgress,
			 NULL, /* AfxGetInstanceHandle() */
			 NetIOIE5, /*g_uiNetMethod */
			 NULL, /* _proxy_address */
			 80 /* g_uiProxyPort */
			 ) == 0);
  
  if (ret) 
    hash_insert(remote_db, filename, localname);
  else
    localname = NULL;

  puts("\n");
  free(log_buffer);
  return localname;
}

void CDECL unlink_remote_file(const_string key, const_string value)
{
  int fa;
  fa = GetFileAttributes(value);
  if (fa != 0xFFFFFFFF && !(fa & FILE_ATTRIBUTE_DIRECTORY)) {
    if (DeleteFile(value) == 0) {
      fprintf(stderr, "%s: can't delete file %s (error %d)\n", kpse_program_name, value, GetLastError());
    }
  }
}

void cleanup_remote_files()
{
  hash_table_type *remote = hash_exists_p(hashtable_remote);
  hash_iter(remote, unlink_remote_file);
}
