#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <cstdio>
#include <strings.h>
#include <pthread.h>

#define PORT 8080

using namespace std;

int client_socket1, client_socket2;
void pError(string s){
    perror(s.c_str());
    exit(-1);
}

void* receive(void *arg){
    char buffer[1000];
    int socket = *((int*)(&arg));
    int socket1, socket2;
    if(socket == client_socket1){
        socket1 = client_socket1;
        socket2 = client_socket2;
    }
    else{
        socket1 = client_socket2;
        socket2 = client_socket1;
    }
    while(true){
        if (recv(socket1, buffer, 1000, 0) == -1)
                pError("Receive failed\n");

        if (send(socket2, buffer, 1000, 0) == -1)
                pError("Send failed\n"); 
    }
}
int main(){
    pthread_t thread1, thread2;
    int socket_id1, socket_id2;
    if((socket_id1 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        pError("Socket creation failed\n");

    struct sockaddr_in addr1;
    addr1.sin_family = AF_INET; 
    addr1.sin_port = htons(PORT); 
    addr1.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_id1, (struct sockaddr *) &addr1, sizeof(addr1)) == -1) 
        pError("Binding failed\n");

    cout << "Listening for client" << endl;

    if (listen(socket_id1, 2) == -1){
        pError("Listen failed\n");
    }

    char buffer[1000] = "Start";
    
    struct sockaddr_in client1, client2;

    while(true){
        socklen_t cl = sizeof(client1);
        if ((client_socket1 = accept(socket_id1, (struct sockaddr *) &client1, &cl)) == -1)
            pError("Accept failed\n");

        cout << "Connection Established with first user, Waiting for second user..." << endl;

        cl = sizeof(client2);
        if ((client_socket2 = accept(socket_id1, (struct sockaddr *) &client2, &cl)) == -1)
            pError("Accept failed\n");


        cout << "Connection Established with second user, Start Chat..." << endl;

        if (send(client_socket1, buffer, 1000, 0) == -1)
                pError("Send failed\n"); 
        if (send(client_socket2, buffer, 1000, 0) == -1)
                pError("Send failed\n"); 

        int ret1, ret2;
        ret1 =  pthread_create(&thread1, NULL, &receive, (void*)client_socket1);
        if(ret1 != 0) {
                printf("Error: pthread_create() failed\n");
                exit(EXIT_FAILURE);
        }
        ret2 =  pthread_create(&thread2, NULL, &receive, (void*)client_socket2);
        if(ret1 != 0) {
                printf("Error: pthread_create() failed\n");
                exit(EXIT_FAILURE);
        }

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
        //close(client_socket);
        cout << "Client connection closed" <<endl;
        //break;
    }
    
    close(socket_id1);
    close(socket_id2);
    cout << "Socket Connection closed" << endl;
    
}