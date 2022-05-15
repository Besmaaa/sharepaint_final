#include <stdio.h>
#include <string.h>
 
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
 
/* <DESC>
 * Performs an FTP upload and renames the file just after a successful
 * transfer.
 * </DESC>
 */
 
#define LOCAL_FILE      "test"
#define UPLOAD_FILE_AS  "while-uploading"
//#define REMOTE_URL      "ftp://192.168.1.23/ftp/"  UPLOAD_FILE_AS
#define RENAME_FILE_TO  "renamed-and-fine"
#define REMOTE_URL      "ftp://195.132.24.123/ftp/"  UPLOAD_FILE_AS
 
/* NOTE: if you want this example to work on Windows with libcurl as a
   DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
   Failing to do so will give you a crash since a DLL may not use the
   variable's memory when passed in to it from an app like this. */
size_t read_callback(char *ptr, size_t size, size_t nmemb, void *stream)
{
  unsigned long nread;
  /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */
  size_t retcode = fread(ptr, size, nmemb, stream);
 
  if(retcode > 0) {
    nread = (unsigned long)retcode;
    fprintf(stderr, "*** We read %lu bytes from file\n", nread);
  }
 
  return retcode;
}
 
int ftp_upload(char* localfile, char* url)
{
  CURL *curl;
  CURLcode res;
  FILE *hd_src;
  struct stat file_info;
  unsigned long fsize;
 
  struct curl_slist *headerlist = NULL;
  static const char buf_1 [] = "RNFR " UPLOAD_FILE_AS;
  static char buf_2 [] = "RNTO " RENAME_FILE_TO;
  sprintf(buf_2,"RNTO %s",localfile);
 
  /* get the file size of the local file */
  if(stat(localfile, &file_info)) {
    printf("Couldn't open '%s': %s\n", localfile, strerror(errno));
    return 1;
  }
  fsize = (unsigned long)file_info.st_size;
 
  printf("Local file size: %lu bytes.\n", fsize);
 
  /* get a FILE * of the same file */
  hd_src = fopen(localfile, "rb");
 
  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */
  curl = curl_easy_init();
  if(curl) {
    /* build a list of commands to pass to libcurl */
    headerlist = curl_slist_append(headerlist, buf_1);
    headerlist = curl_slist_append(headerlist, buf_2);
 
    /* we want to use our own read function */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
 
    /* enable uploading */
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 
    /* specify target */
    curl_easy_setopt(curl, CURLOPT_URL, REMOTE_URL);
 
    /* pass in that last of FTP commands to run after the transfer */
    curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);
 
    /* now specify which file to upload */
    curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);
 
    /* Set the size of the file to upload (optional).  If you give a *_LARGE
       option you MUST make sure that the type of the passed-in argument is a
       curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
       make sure that to pass in a type 'long' argument. */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)fsize);
 
    /* Now run off and do what you have been told! */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* clean up the FTP commands list */
    curl_slist_free_all(headerlist);
 
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  fclose(hd_src); /* close the local file */
 
  curl_global_cleanup();
  return 0;
}
