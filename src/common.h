#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <netdb.h>

#define PORT 8808
#define SA struct sockaddr

#define DEFAULT_FILEPATH_SENDER "/Users/yianniKir/Desktop/vsblu/sharefolder"
#define DEFAULT_FILEPATH_SENDER_LEN sizeof(DEFAULT_FILEPATH_SENDER)

#define DEFAULT_FILEPATH_RECEIVER "/Users/yianniKir/Desktop/vsblu/recvfolder"
#define DEFAULT_FILEPATH_RECEIVER_LEN sizeof(DEFAULT_FILEPATH_RECEIVER)

void err(const char* errtxt){
    fprintf(stderr, "(errno = %d) : %s (%s)\n", errno, strerror(errno), errtxt);
    exit(1);
}

//this function will return a pointer to a series of memory with each file name separated by $ signs
char *readconfig(int *port, char **senderdirectory, char **receiverdirectory){
    //open file
    FILE *configfile = fopen("vsblu.config", "rb");
    if(configfile == NULL)
        err("No config file");
    
    //write the entire file in to memory

    size_t tmpmaxspace = 200;
    char *tmp = malloc(tmpmaxspace);

    size_t n = 0;
    int c;

    while((c = fgetc(configfile)) != EOF){
        if(n == tmpmaxspace - 1){
            tmpmaxspace = (size_t) 2 * tmpmaxspace;
            tmp = realloc(tmp, tmpmaxspace);
            if(tmp == NULL)
                err("Memory allocation error");
        }

        tmp[n++] = (char) c;

    }
    if(tmp[n-1] == '\n')
        tmp[n--] = '\0';
    else
        tmp[n] = '\0';

    fclose(configfile);
 
    char *currentpos = tmp;
    char *newpos;
    size_t linelen;

    //port
    newpos = strchr(tmp, '\n');
    if(newpos==NULL) err("No port");
    linelen = newpos - currentpos;
    char portstr[linelen + 1]; // '\0' char
    strncpy(portstr, currentpos, linelen);
    portstr[linelen] = '\0';
    *port = atoi(portstr);

    //sender
    currentpos = newpos + 1; // to skip the \n which wasnt previously considered
    newpos = strchr(currentpos, '\n');
    if(newpos==NULL) err("No sender path");
    linelen = newpos - currentpos;
    *senderdirectory = malloc(linelen + 1);
    strncpy(*senderdirectory, currentpos, linelen);
    (*senderdirectory)[linelen] = '\0';

    //reciever
    currentpos = newpos + 1; //same here
    newpos = strchr(currentpos, '\n');
    if(newpos==NULL) err("No receiver path");
    linelen = newpos - currentpos;
    *receiverdirectory = malloc(linelen + 1);
    strncpy(*receiverdirectory, currentpos, linelen);
    (*receiverdirectory)[linelen] = '\0';
    
    //now the rest of the file
    //directory support soon

    size_t filesmaxspace = 50;
    char *filesbuffer = malloc(filesmaxspace);

    n = 0;
    currentpos = newpos + 1;

    for(;;){
        if(n == filesmaxspace - 1){
            filesmaxspace = (size_t) 2 * filesmaxspace;
            filesbuffer = realloc(filesbuffer, filesmaxspace);
        }
        
        c = *currentpos;
        currentpos++;

        putchar(filesbuffer[n]);
        if(c == '\n'){
            filesbuffer[n++] = '$';
        }else if(c == '+'){
            break;
        }else{
            filesbuffer[n++] = (char) c;
        }
    }
    filesbuffer[n] = '\0';
    
    free(tmp);

    return filesbuffer;
}

#endif
/*currentpos = newpos + 1;
        newpos = strchr(currentpos, '\n');
        if(newpos==NULL) break;
        linelen = newpos - currentpos; //we also want the \n
        
        if(n + linelen >= filesmaxspace){
            filesmaxspace = (size_t) 2 * filesmaxspace;
            filesbuffer = realloc(filesbuffer, filesmaxspace);
        }
        n+=linelen;

        strncpy(filepos, currentpos, linelen);
        filesbuffer[linelen] = '$';
        filepos+=linelen;*/