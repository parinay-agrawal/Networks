#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <bits/stdc++.h>
using namespace std;
#include<stdio.h>
#include<string.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 

int main(int argc, char *argv[]){

    int fd;


    if(argc < 3){
        cout << 0 << endl;
        fprintf(stderr, "Usage: %s <Address> <port>\n", argv[0]);
        exit(1); 
    }
    string s(argv[1]);

    size_t found1 = s.find_first_of("/");
    string s1 = s.substr(0,found1);
    string s2 = s.substr(found1);
    // cout << s1 << endl << s2 << endl;

       
    // cout << 1 << endl;
    string message1;
    char server_reply[10000];
    char *filename = "file.pdf";
    
    int port = atoi(argv[2]);

    FILE *file = NULL;
    struct sockaddr_in server;

    struct hostent *hp;
    struct sockaddr_in addr;
    int on = 1, sock;     
    // cout << s1 << endl << argv[2] << endl;
    if((hp = gethostbyname(s1.c_str())) == NULL){
        herror("gethostbyname");
        exit(1);
    }
    bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

    if(sock == -1){
        perror("setsockopt");
        exit(1);
    }
    
    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
        perror("connect");
        exit(1);
    }
    // cout << s1 << endl << s2 << endl;
    message1 = "GET " + s2 + " HTTP/1.1\r\nHost: " + s1 + "\r\n\r\n Connection: keep-alive\r\n\r\n Keep-Alive: 300\r\n";
    const char *message;
    message = message1.c_str();
    if(send(sock , message , strlen(message) , 0) < 0){
        puts("Send failed");
        return 0;
    }

    puts("Data Send\n"); 
    remove(filename);
    file = fopen(filename, "ab");

    if(file == NULL){
        printf("File could not opened");
    }   
    while(1){
        cout << "Receiving..\n";
        int received_len = recv(sock, server_reply , sizeof server_reply , 0);
        if(received_len <= 0){
            break;
        }
        fwrite(server_reply, received_len, 1, file);
    }
    cout << "File received\n";
    fclose(file);
    return 0;
}