/*****************************************************************************
 *                                  _   _ ____  _     
 *  Project                     ___| | | |  _ \| |    
 *                             / __| | | | |_) | |    
 *                            | (__| |_| |  _ <| |___ 
 *                             \___|\___/|_| \_\_____|
 *
 * $Id$
 */

#include "curl/curl.h"
#include "curl/types.h"
#include "curl/easy.h"

#include "testconfig.h"

/*
 * Similar to ftpget.c but this also stores the received response-lines
 * in a separate file using our own callback!
 *
 * This functionality was introduced in libcurl 7.9.3.
 */

size_t
write_response(void *ptr, size_t size, size_t nmemb, void *data)
{
  FILE *writehere = (FILE *)data;
  return fwrite(ptr, size, nmemb, writehere);
}

int main(int argc, char **argv)
{
  CURL *curl;
  CURLcode res;
  FILE *ftpfile;
  FILE *respfile;
  (void)argc; (void)argv;
  
  /* local file name to store the file as */
  ftpfile = fopen(LIBCURL_BINARY_DIR "/Testing/ftpgetresp-list.txt", "wb"); /* b is binary, needed on win32 */

  /* local file name to store the FTP server's response lines in */
  respfile = fopen(LIBCURL_BINARY_DIR "/Testing/ftpgetresp-responses.txt", "wb"); /* b is binary, needed on win32 */

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  if(curl) {
    /* Get a file listing from sunet */
    curl_easy_setopt(curl, CURLOPT_URL, "ftp://public.kitware.com/");
    curl_easy_setopt(curl, CURLOPT_FILE, ftpfile);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_response);
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, respfile);
    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  fclose(ftpfile); /* close the local file */
  fclose(respfile); /* close the response file */

  return 0;
}
