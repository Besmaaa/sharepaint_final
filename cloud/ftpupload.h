#ifndef FTPUPLOAD_H
#define FTPUPLOAD_H

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

#define UPLOAD_FILE_AS  "while-uploading"
#define REMOTE_URL      "ftp://192.168.1.23/ftp/"  UPLOAD_FILE_AS
#define RENAME_FILE_TO  "renamed-and-fine"

size_t read_callback(char *ptr, size_t size, size_t nmemb, void *stream);

int ftp_upload(char* localfile, char* url);

#endif


