
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ftpget.h"
#include "ftpupload.h"
#include "cloud.h"
//#define SERVEURNAME "192.168.1.23"
#define SERVEURNAME "195.132.24.123"
//#define SERVEURNAME "127.0.0.1"
#define FTPURL "ftp://192.168.1.23/ftp/"
//define FTPURL "ftp://195.132.24.123/ftp/"
int cloud_launch();
void printRequest();
int initConnexion(int clientsocket, struct sockaddr_in serverSockAddr);
int initSocket(char* IP_server,int* clientsocket, struct sockaddr_in* serverSockAddr);
char* sendData(int clientsocket, char* data, char** result);
void closeConnexion(int clientsocket);
void ftp_request(int arg);

void login(char** result,int cfd, char* NAME, char* PASSWD, int *token);

void signup(char** result, int cfd, char* NAME, char* PASSWD);
char* project_push(int cfd, char*pname, char* name);
char* project_pull(int cfd, char* pname, char* name);
void logout(char** result,int cfd, char* name);
void project_create(char** result,int cfd, char* pname, char* name, int* token);
List* project_getplist(List** plist,int cfd, char* name);
int request_pget(int cfd, char* data, char* name);
int authorized(int cfd, char* name, char* project);
void request_addpict(char** result,int cfd, char* pName, char* pict, char* name);
void request_adduser(char** result,int cfd, char* pName, char* user, char* name);
char* request_plist(char* pName);
char* request_pcreate(char* pName, char* name, int* token);
int cloud_launch()
{
    /*srand(time(NULL));
    char* NAME = malloc(256*sizeof(char));
    char* PASSWORD = malloc(256*sizeof(char));
    int TOKEN = -1;*/
    int clientsocket = -1;
	struct sockaddr_in serverSockAddr;
	initSocket(SERVEURNAME, &clientsocket, &serverSockAddr);
	initConnexion(clientsocket, serverSockAddr);
    return clientsocket;
/*	char* data = malloc(200*sizeof(char));
    	printRequest();
    while(1)
	{
            int autho = 0;
            memset(data,0,sizeof(char)*200);
	        printf("Saisir la requete : ");
		    fflush( stdout );
    		scanf( "%s", data );
        	if(strcmp(data,"EXIT")==0)
			{	printf("BYE, SEE YOU SOON\n");
				break;	
			}
            if(strcmp(data,"LOGIN")==0)
                login(&data,&NAME,&PASSWORD,&TOKEN);
            else if(strcmp(data,"UPLOAD")==0)
            {
                ftp_request(0);
                continue;
            }
            else if(strcmp(data,"DOWNLOAD")==0)
            {
                ftp_request(1);
                continue;
            }
            else if(strcmp(data,"PROJECTCREATE")==0)
            {    if(request_pcreate(&data,&NAME,&TOKEN)==-1)
                    continue;
            }

            else if(strcmp(data,"GETPROJECT")==0)
            {
                if(request_pget(clientsocket,data,NAME)==-1)
                    continue;
            }
            else if(strcmp(data,"ADDUSER")==0)
            {
                if(request_adduser(&data,&NAME,&TOKEN)==-1)
                    continue;
            }
            else if(strcmp(data,"ADDPICT")==0)
            {   if(request_addpict(&data,&NAME,&TOKEN)==-1)
                    continue;
                autho = 1;
            }
            //printf("after %s\n and token : %d\nand name %s\n",data,TOKEN,NAME);
            //char send[300];
            char *result = malloc(200*sizeof(char));
       	    memset(result,0,200*sizeof(char));
	        sendData(clientsocket, data, &result);
	        if(strcmp(result,"FAIL REQUEST SYNTAX or TREATMENT")==0)
		        printRequest();
            if(autho == 1 && strcmp(result,"SUCCESS"))
                    ftp_request(0);
               
	        printf("%s\n",result);
	}*/
    }
//PROJECT_CREATE_name
//PROJECT_DELETE_name
//PROJECT_ADDPICTURE_nameProject_namePicture
//PROJECT_ADDUSER_nameProject_nameUser
//PROJECT_DELETEUSER_nameProject_nameUser

void printRequest()
{
        printf("Request Format : \nLOGIN\nLOGOUT_name\nSIGNUP_name_password\nSIGNOUT_name_password\nPROJECTCREATE\nADDUSER\nADDPICT\nEXIT\n");
}
/*
char** ftp_filetolist()
{
    FILE* ptr;
    char c;
    ptr = fopen("ftp-list","r");
    if(NULL!=ptr)
    {
        do{
            c=fgetc(ptr);
            printf("%c",c);
        }while(c!=EOF);
    }
    return NULL;
}*/


void signup(char** result, int cfd, char* NAME, char* PASSWD)
{
    char data[256];
    sprintf(data,"SIGNUP_%s_%s",NAME,PASSWD);
    sendData(cfd,data,result);
}

void request_addpict(char** result, int cfd, char* pName, char* pict, char* name)
{

    char data[256];
    sprintf(data,"ADDPICT_%s_%s_%s",pName,pict,name);
    sendData(cfd,data,result);
}


void request_adduser(char** result,int cfd, char* pName, char* user, char* name)
{
    char data[256];
    sprintf(data,"ADDUSER_%s_%s_%s",pName,user,name);
    sendData(cfd,data,result);
}

int authorized(int cfd, char* name, char* project)
{
    char* res = malloc(256*sizeof(char));
    char* data = malloc(256* sizeof(char));
    sprintf(data,"AUTHO_%s_%s",name,project);
    printf("la data: %s\n",data);
    sendData(cfd,data,&res);
    if(strcmp(res,"SUCCESS")==0)
        return 0;
    return -1;
}

int request_pget(int cfd, char* data, char* name)
{
    char* proj = malloc(256*sizeof(char));
    printf("nom du projet : \n");
    scanf("%s",proj);
    if(authorized(cfd,name,proj)==-1)
        return -1;
    printf("path : %s\n",proj);
    
    //ftp_get2(proj);
    return 0;
}

char* request_pcreate(char* pName, char* name, int* token)
{
    char* data = malloc(200*sizeof(char));
    if(*token == -1)
    {
        printf("must be logged in\n");
        return "nope";
    }
    printf("4\n");
    sprintf(data,"PROJECTCREATE_%s_%d",name,*token);
    printf("5\n");
    printf("data pcreate : %s\n",data);
    mkdir(pName,S_IRWXU);
    return data;
}
void printlist(List* list)
{   while(list)
    {
        printf("l : %s\n",list->str);
        list=list->next;
    }
}

char* project_push(int cfd, char* pname, char* name)
{
    
    if(authorized(cfd,name,pname)==-1)
        return "NOT ALLOWED";
    char url[64];
    List* list = malloc(sizeof(List));
    project_getplist(&list,cfd,pname);
    char file[64];
    while(list)
    {

        sprintf(url,"%s%s/%s",FTPURL,pname,list->str);
        sprintf(file,"%s/%s",pname,list->str);
        printf("file : %s\n",file);
        ftp_upload(file,url);
        list=list->next;
    }
    printf("project_push() DONE");
    return "SUCCESS";
}


char* project_pull(int cfd, char* pname, char* name)
{

    if(authorized(cfd,name,pname)==-1)
        return "NOT ALLOWED";
    mkdir(pname,S_IRWXU);
    char url[64];
    List* list = malloc(sizeof(List));
    project_getplist(&list,cfd,pname);
    char file[64];
    while(list)
    {

        sprintf(url,"%s%s/%s",FTPURL,pname,list->str);
        sprintf(file,"%s/%s",pname,list->str);
        printf("file : %s\n url : %s\n",file,url);
        ftp_get(url,file);
        list=list->next;
    }
    printf("project_pull() DONE\n");
    return "SUCCESS";
}

List* project_getplist(List** plist,int cfd, char* name)
{
    List* list =*plist;
    list->next = NULL;
    List* currentList = *plist;
    //////////////////////
    char* result = malloc(200*sizeof(char));
    char* data = malloc(200*sizeof(char));
    sprintf(data,"GETPICLIST_%s",name);
    sendData(cfd,data,&result);
    ///////////////////////
    result++;
    int i = 0;
    while(result!=NULL && *result != '\0' && *result!='|')
    {
        if(*result=='_' || *result=='\0')
        { 
            currentList->str[i] = '\0';
            currentList->next = malloc(sizeof(List));
            currentList->next->next=NULL;
            currentList=currentList->next;
            i=0; 
            result++;
            continue;
        }
        currentList->str[i] = *result;
        result++;
        i++;
    }
    currentList->str[i] = '\0';
    return list;
}



void project_create(char** result, int cfd, char* pname, char* name, int* token)
{
    char* data = request_pcreate(pname,name,token);
    sendData(cfd,data,result);
}
void login(char** result,int cfd, char* NAME, char* PASSWD, int* token)
{
    char data[256];
    srand(time(NULL));
    *token = rand();
    sprintf(data,"LOGIN_%s_%s_%d",NAME,PASSWD,*token);
    sendData(cfd,data,result);
}
void logout(char** result,int cfd, char* name)
{
    char* data = malloc(200*sizeof(char));
    sprintf(data,"LOGOUT_%s",name);
    sendData(cfd,data,result);
}
void ftp_request(int arg)
{
    char data[64];
    printf("File to upload/download : \n");
    scanf("%s",data);
    if(arg==0)
        ftp_upload(data,FTPURL);
    else
    {
        //char path[128];
        //sprintf(path,"%s%s",FTPURL,data);
        ftp_get(data,data);
    }
}
int initSocket(char* IP_server,int* clientsocket,struct sockaddr_in* serverSockAddr)
{
//	char *server_name = IP_server;
	struct hostent *serverHostEnt;
    long hostAddr;
	hostAddr = inet_addr(SERVEURNAME);

  	serverHostEnt = gethostbyname(SERVEURNAME);
  	if (serverHostEnt == NULL)
  	{
    	printf("Echec de getHost: %s\n", strerror(errno));
    	exit(0);
	}
	serverSockAddr->sin_port =htons( 6900 );
	serverSockAddr->sin_family = AF_INET;
    serverSockAddr->sin_addr.s_addr=inet_addr(SERVEURNAME);
	// creation de la socket 
	if ( (*clientsocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
  	printf("Echec creation socket client: %s\n", strerror(errno));
  	exit(0);
	}
	return 0;

}

char* sendData(int clientsocket, char* data, char** result)
{
	char buffer[512];
	//envoie de donn??es et r??c??ption du retour serveur
	send(clientsocket, data,strlen(data)/*1+ sizeof(char)*strlen(data)*/, 0);
	recv(clientsocket, buffer, 512, 0);
	*result = buffer;
	return 0;
}

void closeConnexion(int clientsocket)
{
	shutdown(clientsocket,2);
	close(clientsocket);
}

int initConnexion(int clientsocket, struct sockaddr_in serverSockAddr)
{

	// requete de connexion
	if(connect( clientsocket,
        	(struct sockaddr *)&serverSockAddr,
        	sizeof(serverSockAddr)) < 0 )
	{
  	printf("Echec demande de connection %s\n", strerror(errno));
  	exit(0);
	}
    return 0;
}



