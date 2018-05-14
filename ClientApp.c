//
//  ClientApp.c
//  Lab3
//
//  Created by Dominus on 11/22/17.
//  Copyright © 2017 Dominus. All rights reserved.
//

//#include "ClientApp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include"pthread.h"

#define BUFSIZE 1024
#define PORT 10025
#define BUF_SIZE 2000
//#define PORT 10025
//#define BUF_SIZE 2000
#define CLADDR_LEN 100


/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

//This funtion if for the client to revceive messages
void * receiveMessage(void * socket) {
    int sockfd, ret;
    char buffer[BUF_SIZE];
    sockfd = (int) socket;
    memset(buffer, 0, BUF_SIZE);
    for (;;) {
        ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
        if (ret < 0) {
            printf("Error receiving data!\n");
        } else {
            printf("server: ");
            fputs(buffer, stdout);
            //printf("\n");
        }
    }
}

////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
    int sockfu, newfu; /* socket */
    int portno; /* port to listen on */
    int myport; /*port for tcp connection */
    int clientlen_t; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char buf[BUFSIZE]; /* message buf */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */
    struct hostent *hp;
    
    
    //Struct for responding to proper request
    struct messsage
    {
        uint32_t magicNum;
         uint16_t myPort;
        uint8_t GID;
        //uint16_t myPort;
    } __attribute__((__packed__));
    
    
    /*
     * check command line arguments
     */
    if (argc < 4) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
     char *host = argv[1];
     portno = atoi(argv[2]);
     myport = atoi(argv[3]);
     printf("\n Remote port number: %i\n", myport);
    
    
    /** socket: create the parent socket */
    sockfu = socket(AF_INET, SOCK_DGRAM, 0);
    
    //checks to see if the socket is open
    if (sockfu < 0){
        error("ERROR opening socket");
    }
    
    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    optval = 1;
    setsockopt(sockfu, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval , sizeof(int));
    
    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);
    
    
    /** bind: associate the parent socket with a port*/
    if (bind(sockfu, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0){
        error("ERROR on binding");
    }
    
    ////////////////////////////////////////////////////////////////
    /* fill in the server's address and data */
    memset((char*)&clientaddr, 0, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(portno);

    
    
    /** main loop: wait for a datagram, then echo it */
    clientlen_t = sizeof(clientaddr);
    
    printf("server: waiting for connections...\n");
    
    bzero(buf, BUFSIZE);
    
    hp = gethostbyname(host);
    
    //////////////// SEND DATA ////////////////////////////////////////
    
    /* look up the address of the server given its name */
    if (!hp) {
        fprintf(stderr, "could not obtain address of %s\n", host);
        return 0;
    }
    
    /* put the host's address into the server address structure */
    memcpy((void *)&clientaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
    
    //Create the outgoing struct
    struct messsage messagePacket;

    
    
    //Send message
    messagePacket.magicNum = htonl(0x4A6F7921);
    messagePacket.GID = 15;
    messagePacket.myPort = htons(myport);
    
    
    
    /* send a message to the server */
    if (sendto(sockfu, &messagePacket, 7, 0, (struct sockaddr *)&clientaddr, clientlen_t) < 0) {
        perror("sendto failed");
        return 0;
    }
    
    
    
    /////////////// //////////Receive Data /////////////////////////////////////////////////
while (1){
        /*
         * recvfrom: receive a UDP datagram from a client
         */
        bzero(buf, BUFSIZE);
        n = recvfrom(sockfu, buf, BUFSIZE, 0,(struct sockaddr *) &clientaddr, &clientlen_t);
    if (n < 0){
            printf("\n Error in recvfrom");
    }
 
    
    ///////////////////////////SERVER//////////////////////////////////
    //Connect message that turns on the server
    if (n == 7){
    
        
        //Read the transmitted information and print it our
        uint32_t magicNo =  *(uint32_t *) (buf);
        uint8_t GID = *(uint8_t *) (buf + 4);
        uint16_t portNum = *(uint16_t *) (buf + 5);
        close(sockfu);
        
        //uint16_t portNum = *(uint16_t *) (buf + 5);
        
        
        if (ntohl(magicNo) != 0x4A6F7921 || ntohs(portNum) == 001 ){
            perror("The Response from the server is not valid");
            return 0;
        }
        
        //Print the messages
        //printf("Magic number: %i\n", magicNum);
        printf("\n value of magicNo: %X \n",ntohl(magicNo));
        printf("\n GID number: %i\n", GID);
        printf("\n Port number: %i\n", ntohs(portNum));
        
        
        //Launch TCP Server
        printf("\n Launching TCP Server: %i\n", n);
        
        //////////////////////////////////////////////////////////////////////////////////
        struct sockaddr_in addr, cl_addr;
        int sockfs, len, ret, newsockfs;
        char buffer[BUF_SIZE];
        pid_t childpid;
        char clientAddr[CLADDR_LEN];
        pthread_t rThread;
        
        sockfs = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfs < 0) {
            printf("Error creating socket!\n");
            exit(1);
        }
        printf("Socket created...\n");
        
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        //addr.sin_addr.s_addr = htonl(INADDR_ANY);
        //addr.sin_port = htons(PORT);
        addr.sin_port = htons((unsigned short)myport);
        printf("Port: %i\n", myport);
        
        
        ret = bind(sockfs, (struct sockaddr *) &addr, sizeof(addr));
        if (ret < 0) {
            printf("Error binding!\n");
            exit(1);
        }
        printf("Binding done...\n");
        
        printf("Waiting for a connection...\n");
        listen(sockfs, 5);
        
        
        len = sizeof(cl_addr);
        newsockfs = accept(sockfs, (struct sockaddr *) &cl_addr, &len);
        if (newsockfs < 0) {
            printf("Error accepting connection!\n");
            exit(1);
        }
        
        inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, CLADDR_LEN);
        printf("Connection accepted from %s...\n", clientAddr);
        
        memset(buffer, 0, BUF_SIZE);
        printf("Enter your messages one by one and press return key!\n");
        
        //creating a new thread for receiving messages from the client
        ret = pthread_create(&rThread, NULL, receiveMessage, (void *) newsockfs);
        if (ret) {
            printf("ERROR: Return Code from pthread_create() is %d\n", ret);
            exit(1);
        }
        
        while (fgets(buffer, BUF_SIZE, stdin) != NULL) {
            ret = sendto(newsockfs, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
            if (ret < 0) {
                printf("Error sending data!\n");
                exit(1);
            }
        }
        
        close(newsockfs);
        close(sockfs);
        
        pthread_exit(NULL);
        return 0;
    }
        
        
        
        
    
    
    
    /////////////////////////////CLIENT///////////////////////////////////////////////////////
    //Connect message to that turns on the client
    if (n == 11){
        
        uint32_t magicNo = *(uint32_t *) (buf);
        //char *IP = buf + 4;
        //unsigned long readin = *(unsigned long *) (buf + 4);
        struct in_addr readin = *(struct in_addr*)(buf + 4);
        uint16_t portNum = *(uint16_t *) (buf + 8);
        uint8_t GID = *(uint8_t *) (buf + 10);
        
        //char *ip_addr = inet_ntoa(clientaddr.sin_addr); // return the IP
        
        if (ntohl(magicNo) != 0x4A6F7921 || ntohs(portNum) == 001 ){
            perror("The Response from the server is not valid");
            return 0;
        }
        
        //Print the messages
        //printf("Magic number: %i\n", magicNum);
        printf("\n value of magicNo: %X \n",ntohl(magicNo));
        printf("\n GID number: %i\n", GID);
        printf("\n Port number: %i\n", ntohs(portNum));
        printf(inet_ntoa(*(struct in_addr*)(&readin)));
        
        char *IP = (inet_ntoa(*(struct in_addr*)(&readin)));//Stores the ip address in dot notation
        
        
        //////////////////////////////////CLIENT : TCP////////////////////////////////
        //Launch TCP Server
        printf("\n Launching TCP Client: %i\n", n);
        
    //}
    
    struct sockaddr_in addr, cl_addr;
    int sockfd, ret;
    char buffer[BUF_SIZE];
    //char * serverAddr;
    struct hostent *hp;
    pthread_t rThread;
    
    /*if (argc < 2) {
        printf("usage: client < ip address >\n");
        exit(1);
    } */
    
   // serverAddr = argv[1];
        //serverAddr = (inet_ntoa(*(struct in_addr*)(&readin)));//Stores the ip address in dot notation
        
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error creating socket!\n");
        exit(1);
    }
    printf("Socket created...\n");
    
        hp = gethostbyname(inet_ntoa(readin));
        
        if (hp == NULL){
            printf("Error getting host by name");
            exit(0);
        }
        
        memset((char *)&addr, 0, sizeof(addr));
        bcopy((char*)hp->h_addr, (char *)&addr.sin_addr.s_addr, hp->h_length);
    //hp = gethostbyname(host);//new addition
    //memset(&addr, 0, sizeof(addr));
    //memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    //addr.sin_addr.s_addr = inet_addr(IP);
    //addr.sin_addr = *(struct in_addr*)(&readin);
    //addr.sin_addr.s_addr = readin;
      
        addr.sin_port = portNum;
    
    ret = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret < 0) {
        //printf("Error connecting to the server!\n");
        perror("error connecting to the server: \n");
        exit(1);
    }
    printf("Connected to the server...\n");
    
    memset(buffer, 0, BUF_SIZE);
    printf("Enter your messages one by one and press return key!\n");
    
    //creating a new thread for receiving messages from the server
    ret = pthread_create(&rThread, NULL, receiveMessage, (void *) sockfd);
    if (ret) {
        printf("ERROR: Return Code from pthread_create() is %d\n", ret);
        exit(1);
    }
    
    while (fgets(buffer, BUF_SIZE, stdin) != NULL) {
        ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));
        if (ret < 0) {
            printf("Error sending data!\n\t-%s", buffer);
        }
    }
    
    close(sockfd);
    pthread_exit(NULL);
    
    return 0;
 }

 }//closes th if loop
    
}
    

