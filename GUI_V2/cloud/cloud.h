#ifndef CLOUD_H_ 
#define CLOUD_H_ 
  
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <errno.h> 
#include <arpa/inet.h> 
#include <fcntl.h> 
#include "../ui_2.h"


void printlist(List* list);
int cloud_launch();
void printRequest();
int initConnexion(int clientsocket, struct sockaddr_in serverSockAddr);
int initSocket(char* IP_server,int* clientsocket, struct sockaddr_in* serverSockAddr);
char* sendData(int clientsocket, char* data, char** result);
void closeConnexion(int clientsocket);
void ftp_request(int arg);

char* project_pull(int cfd, char* pname, char*name);
char*project_push(int cfd, char* pname, char* name);
void logout(char** result,int cfd, char* name);
void login(char** result, int cfd, char* NAME, char* PASSWD, int* token);
void signup(char** result,int cfd, char* NAME, char* PASSWD);
void project_create(char** result,int cfd, char* pname, char* name, int* token);
List* project_getplist(List** plist,int cfd, char* name);
int request_pget(int cfd, char* data, char* name);
int authorized(int cfd, char* name, char* project);
void request_addpict(char** result,int cfd, char* pName, char* pict, char* name);
void request_adduser(char** result,int cfd, char* pName, char* user, char* name);
char* request_pcreate(char* pName,char* name, int* token);


#endif

