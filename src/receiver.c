#include "common.h"

int main(int argc, char **argv){

    //create socket and connect to server
    
    //upon getting client connectio, start loop

        //read from client until 'send' is read

        //send sharefolder/hello.text to client

        //await client confirmation

    //repeat loop


    //create socket

    int sockfd;
    struct sockaddr_in servaddr;

    if((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        err("Error creating socket");
    printf("Socket created successfully..\n");

    //associate and reserve port for use by the socket (bind)
    bzero(&servaddr, sizeof(servaddr)); //make sure servaddr is empty
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.115");
    servaddr.sin_port = htons(PORT);

    //connect
    if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0)
        err("Error connecting to server");
    printf("Connected to the server..\n");

    //start loop
    char sendbuffer[16]; //This ought to be enough to handle any file name
    int sendbytes;
    char filenamebuffer[16];
    int filenamebytes;
    for(;;){
        bzero(sendbuffer, 16);
        bzero(filenamebuffer, 16);
        

        printf("Enter the filename to send: ");
        sendbytes = 0;
        while ((sendbuffer[sendbytes++] = getchar()) != '\n' && sendbytes < 16);
        sendbuffer[--sendbytes] = '\0';
        //copy sendbuffer to save filename//+1?
        snprintf(filenamebuffer, sendbytes + 1, "%s", sendbuffer);
        filenamebytes = sendbytes;

        write(sockfd, sendbuffer, sendbytes);
        
        //get filesize from server
        long filesize;
        if(read(sockfd, &filesize, sizeof(filesize)) <=0 ) break;
        
        //prepare to write recieved file
        const int filepathbytes = DEFAULT_FILEPATH_RECEIVER_LEN + filenamebytes + 2; //for the '/' and '\0'
        char filepath[filepathbytes];
        snprintf(filepath, filepathbytes, "%s/%s", DEFAULT_FILEPATH_RECEIVER, filenamebuffer);
        
        FILE *writefile;
        writefile = fopen(filepath, "wb");
        if(writefile != NULL){
            //Begin writing to file
            
            printf("Writing %ld bytes to %s\n", filesize ,filepath);

            char writedata[1024];
            long bytesreceived = 0;
            size_t bytestoread;

            while(bytesreceived < filesize){
                //basically read 1024 bytes unless the chunk is smaller than it will only read the actual ammount needed
                bytestoread = (filesize - bytesreceived < sizeof(writedata)) ? filesize - bytesreceived : sizeof(writedata);
                int r = read(sockfd, writedata, bytestoread);
                if(r <= 0) break;
                fwrite(writedata, 1, r, writefile);
                bytesreceived += r;
            }
            
            fclose(writefile);

        }else{
            err("Error when creating new file");
        }

        
    }
    
    
    close(sockfd);
    

    return 1;

} 