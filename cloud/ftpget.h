#ifndef FTPGET_H
#define FTPGET_H

#include <stdio.h>

#include <curl/curl.h>

size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream);
int ftp_get(char* path, char* final_name);

#endif


