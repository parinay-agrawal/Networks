#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <cstdio>
#include <strings.h>
#include <fstream>
#include <streambuf>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#define PORT1 2330
#define PORT2 2340

using namespace std;

void pError(string s){
    perror(s.c_str());
    exit(-1);
}

int ACK;

std::ofstream out2;

bool recv_data(string s){
	if(s.length() <= 64){
		return false;
	}
	int x = rand() % 2;
	//s[0] = x == 0 ? '0' : '1';
	string checksum = s.substr(0,32);
	string seq_no = s.substr(32, 32);
	string data = s.substr(64);
	//cout << checksum << " " << seq_no << endl;
	unsigned int check_sum = stoi(checksum,nullptr,2);
	unsigned int seq = stoi(seq_no, nullptr, 2);
    unsigned int number = std::bitset<32>(seq_no).to_ulong();
    //cout << number << endl;
	int check = 0;
	for(int i = 0; i < data.length(); i++){
		check ^= data[i];
	}
	check ^= seq;
	//out2 << data << endl;
	//out2 << check << endl;
	//out2 << check_sum << endl;
    //cout << ACK << " " << number << endl;
    //cout << check << " " << check_sum << " " << ACK << " " << number << endl;
	if(check == check_sum && ACK >= number){
		// << endl;// << endl;
		return true;
	}
	else
		return false;
}

int main(){
    ofstream myfile;
    myfile.open ("Server.txt");
  //myfile << "Writing this to a file.\n";
  //myfile.close();

	//out2.open("ServerLog.txt", std::fstream::out);

    int socket_id1;
    if((socket_id1 = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        pError("Socket creation failed\n");

    struct sockaddr_in addr1;
    addr1.sin_family = AF_INET; 
    addr1.sin_port = htons(PORT1); 
    addr1.sin_addr.s_addr = htonl(INADDR_ANY);

    struct sockaddr_in addr2;
    addr2.sin_family = AF_INET; 
    addr2.sin_port = htons(PORT2); 
    inet_pton(AF_INET, "127.0.0.1", &(addr2.sin_addr)); 

    if (bind(socket_id1, (struct sockaddr *) &addr1, sizeof(addr1)) == -1) 
        pError("Binding failed\n");

    /*struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    
    if(setsockopt(socket_id1, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        perror("ERROR");
    */

    char buffer[1000];
    int index = 0;
    ACK = 0;
    int rn = 0;
    while(true){       
    		cout << "";   
    		socklen_t cl = sizeof(addr2);   
			if (recvfrom(socket_id1, buffer, 1000, 0, (struct sockaddr *) &addr2, &cl) == -1){
				cout << buffer;
                pError("Recv Failed\n");
                string st = to_string(rn);
	            strcpy(buffer, st.c_str());

	            cout << "";
		        if (sendto(socket_id1, buffer, 1000, 0, (struct sockaddr *) &addr2, sizeof(addr2)) == -1)
		     	   pError("Send Failed\n");  
		     	continue;
            }
            //cout << buffer << endl;
            //out2 << endl << buffer << endl;
            char *s = buffer;
			std::string str(s);
            bool flag = recv_data(s);
            //cout << flag << endl;
            memset(buffer, 0, sizeof(buffer));

            if(flag){
            	myfile << str.substr(64);
            	myfile.flush();
                //cout << str.substr(64) << "";// " << rn << " ";
				ACK = ACK + str.length()-64;
				rn++;
				cout << "";
            }
            
            str = to_string(rn);
            strcpy(buffer, str.c_str());

            cout << "";
            //cout << rn << endl;
	        if (sendto(socket_id1, buffer, 1000, 0, (struct sockaddr *) &addr2, sizeof(addr2)) == -1)
	     	   pError("Send Failed\n");  
    }
    
    close(socket_id1);
    
}