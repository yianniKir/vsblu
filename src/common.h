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

#endif