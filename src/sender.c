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
        bzero(recvbuffer, 16);

        //read from client until send is sent
        recvbytes = read(connfd, recvbuffer, sizeof(recvbuffer));

        const int filepathbytes = DEFAULT_FILEPATH_SENDER_LEN + recvbytes + 2; //for the '/' and '\0'
        char filepath[filepathbytes];

        snprintf(filepath, filepathbytes, "%s/%s", DEFAULT_FILEPATH_SENDER, recvbuffer);
    
        //open file
        FILE *sendfile;
        sendfile = fopen(filepath, "r");
        char sendfilebuf[sizeof(long)];
        long sendfilebytes;
        printf("Fetching file..\n");

        if(sendfile != NULL){
            //if file exists
            bzero(sendfilebuf, sizeof(long));
            //get num of bytes
            fseek(sendfile, 0 , SEEK_END);
            sendfilebytes = ftell(sendfile);
            fseek(sendfile, 0, SEEK_SET);

            //send num of bytes
            snprintf(sendfilebuf, sizeof(long), "%ld", sendfilebytes);
            write(connfd, sendfilebuf, sizeof(sendfilebuf));
            printf("File found..\n");

            //wait for final confirmation
            bzero(recvbuffer, 16);
            read(connfd, recvbuffer, 2);
            if(strncmp(recvbuffer, "OK", 2) != 0)
                err("Error with final OK");

            //send the file
            printf("Begin sending %ld bytes from %s\n", sendfilebytes , filepath);
            
            char senddata[sendfilebytes];
            bzero(senddata, sendfilebytes);

            while(fgets(senddata, sendfilebytes, sendfile) != NULL){
                if(write(connfd, senddata, sizeof(senddata)) == -1)
                    err("Error sending file bytes");
                bzero(senddata, sendfilebytes);
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