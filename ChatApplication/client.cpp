#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <cstdio>
#include <strings.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
using namespace std;
pthread_t thread1, thread2;

void pError(string s){
    perror(s.c_str());
    exit(-1);
}

void *sending(void *arg){
    int socket_id = *((int*)(&arg));
    char buffer[1000];
    while(true){
        cout << "";
        cin >> buffer;
        if (send(socket_id, buffer, 1000, 0) == -1)
            pError("Send failed\n");         
    }
}

void *receive(void *arg){
    char buffer[1000];
    int socket_id = *((int*)(&arg));
    while(true){
        if (recv(socket_id, buffer, 1000, 0) == -1)
            pError("Receive failed\n");
        printf("Recv: %s\n", buffer);
        if(strcmp(buffer, "Bye") == 0){
            if (send(socket_id, buffer, 1000, 0) == -1)
                pError("Send failed\n"); 
            pthread_cancel(thread1);
            pthread_cancel(thread2);
        }
    }
}

int main(){
    int socket_id;
    if((socket_id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        pError("Socket creation failed\n");      
    
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port = htons(PORT); 
    inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr)); 
    if(connect(socket_id, (struct sockaddr *) &addr, sizeof(addr)) == -1)
        pError("Failed to establish connection\n");
    cout << "Connection Established..." << endl;
    string inp;
    char buffer[1000];

    if (recv(socket_id, buffer, 1000, 0) == -1)
        pError("Receive failed\n");   
    if(strcmp(buffer, "Start") == 0){
        cout << "Start Chat\n";
    } 
    int ret1, ret2;
    ret1 =  pthread_create(&thread1, NULL, &receive, (void*)socket_id);
    if(ret1 != 0) {
            printf("Error: pthread_create() failed\n");
            exit(EXIT_FAILURE);
    }
    ret2 =  pthread_create(&thread2, NULL, &sending, (void*)socket_id);
    if(ret1 != 0) {
            printf("Error: pthread_create() failed\n");
            exit(EXIT_FAILURE);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    close(socket_id);
    cout << "Socket Connection closed" << endl;
    
}