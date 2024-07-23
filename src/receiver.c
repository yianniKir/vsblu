#include "common.h"

int main(int argc, char **argv){

    int optflag = -1; //0 = REPL mode //1 = read from config and exit //-1 = error //10 = help

    char opt;
    if((opt = getopt(argc, argv, "rch")) != -1){
        switch(opt){
            case 'r':
                optflag = 0;
                break;
            case 'c':
                optflag = 1;
                break;
            case 'h':
                optflag = 10;
                break;
            case '?':
                printf("Usage: ./receiver -[r,c,h]\n");
                exit(1);
        }
    }else{
        printf("Usage: ./receiver -[r,c,h]\n");
        exit(1);
    }
    if(optflag == 10){
        printf("0 = REPL, 1 = Read from config and exit\n");
        exit(1);
    }
    
    struct sockaddr_in servaddr;
    char *ip = malloc(16); //fixed len
    char *receiverirectory;
    int receiverdirecorylen;
    char *senderdirectory;
    int senderdirectorylen;
    int port;

    //read config or ask for ip of server (preliminary info)
    if(optflag == 0){
        for(;;){
            bzero(ip, 16);
            printf("Enter a valid IP of the server: ");
            fgets(ip, 16, stdin);

            if(inet_pton(AF_INET, ip, &(servaddr.sin_addr)) == 1) break;
            printf("IP not valid\n");
        }

        //get port and receiverdirectory //TODOOOOO
        /*
        
        TODO
        
        
        */
    }

    char *filestoreceive;
    int filestoreceivelen;
    if(optflag == 1){
        filestoreceive = readconfig(&ip, &port, &senderdirectory,&receiverirectory, &filestoreceivelen);
        if(inet_pton(AF_INET, ip, &(servaddr.sin_addr)) != 1){
            err("IP adress in config file not valid");
        }
     
    }

    //****ESTABLISH CONNECTION */
    int sockfd;
    if((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        err("Error creating socket");
    printf("Socket created successfully..\n");

    //associate and reserve port for use by the socket (bind)
    bzero(&servaddr, sizeof(servaddr)); //make sure servaddr is empty
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(PORT);

    //connect
    if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0)
        err("Error connecting to server");
    printf("Connected to the server..\n");

    //SEND INFO TO SERVER
    
    //Send setup info to server
    write(sockfd, &optflag, sizeof(optflag));

    senderdirectorylen = strlen(senderdirectory);
    write(sockfd, &senderdirectorylen, sizeof(senderdirectorylen));
    write(sockfd, senderdirectory, senderdirectorylen);


    if(optflag == 1){
        write(sockfd, &filestoreceivelen, sizeof(filestoreceivelen));
        printf("%s\n",filestoreceive);
        write(sockfd, filestoreceive, filestoreceivelen);

        //LISTEN AND RECEIVE FILES
        
        //disconnect client
        exit(1);
    }

    char sendbuffer[16]; //This ought to be enough to handle any file name THIS HAS TO CHANGE SOON
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