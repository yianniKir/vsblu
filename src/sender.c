#include "common.h"

int main(int argc, char **argv){

    //create socket and wait for connection from client
    
    //upon getting client connectio, start loop

        //read from client until 'send' is read

        //send file size to client

        //wait for confirmation

        //send sharefolder/hello.text to client

        //await client confirmation

    //repeat loop


    //create socket

    int sockfd, connfd;
    struct sockaddr_in servaddr, clientaddr;

    if((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        err("Error creating socket");
    printf("Socket created successfully..\n");

    //associate and reserve port for use by the socket (bind)
    bzero(&servaddr, sizeof(servaddr)); //make sure servaddr is empty
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if(bind(sockfd, (SA*) &servaddr, sizeof(servaddr))!= 0)
        err("Error binding socket");
    printf("Socket successfully binded..\n");

    //listen for connections
    if((listen(sockfd, 1)) != 0)
        err("Listen failed");
    printf("Server listening on socket..\n");

    //accept connection (automaticcaly halts program until accepted)
    //accept returns the fd for the connection between client and server
    socklen_t clientaddrlen = sizeof(clientaddr);
    connfd = accept(sockfd, (SA*) &clientaddr, &clientaddrlen);
    if(connfd < 0)
        err("Server accept failed");
    printf("Server accepted the client..\n");

    //start loop
    char recvbuffer[16]; //This should be enough to handle any file name
    int recvbytes;
    for(;;){
        //read filename from client
        bzero(recvbuffer, 16);
        recvbytes = read(connfd, recvbuffer, sizeof(recvbuffer) - 1);
        if(recvbytes <= 0) break;
        recvbuffer[recvbytes] = '\0';

        const int filepathbytes = DEFAULT_FILEPATH_SENDER_LEN + recvbytes + 2; //for the '/' and '\0'
        char filepath[filepathbytes];

        snprintf(filepath, filepathbytes, "%s/%s", DEFAULT_FILEPATH_SENDER, recvbuffer);
    
        //open file
        FILE *sendfile;
        sendfile = fopen(filepath, "rb");

        if(sendfile != NULL){            
            //if file exists

            //get num of bytes
            fseek(sendfile, 0 , SEEK_END);
            long sendfilebytes = ftell(sendfile);
            fseek(sendfile, 0, SEEK_SET);

            //send num of bytes
            write(connfd, &sendfilebytes, sizeof(sendfilebytes));
        
            //send the file
            printf("Begin sending %ld bytes from %s\n", sendfilebytes , filepath);
            
            char senddata[1024];
            size_t bytesread;

            while((bytesread = fread(senddata, 1, sizeof(senddata), sendfile)) > 0){
                if(write(connfd, senddata, bytesread) != bytesread)
                    err("Error sending file bytes");
            }

            fclose(sendfile);
        }else{
            printf("Couldn't find file. Path entered: %s\n", filepath);
            exit(1);
        }
    }

    close(sockfd);
    close(connfd);
} 