#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <streambuf>
#include <cstdio>
#include <strings.h>
#include <bits/stdc++.h>
#include <cstring>
#define PORT2 2330
#define PORT1 2340

using namespace std;
#define N 10
#define MSS 50
void pError(string s){
    perror(s.c_str());
    exit(-1);
}


string send_data(string s, int index){
    string sub = s.substr(index*MSS, MSS);
    //int ACK = index;
    int seq_no = index*MSS;
    //cout << seq_no << endl;
    int checksum = 0;
    for(int i = 0; i < sub.length(); i++){
        //out << checksum << endl;
        checksum ^= sub[i];
    }
    checksum ^= seq_no;
    //out << checksum << endl;
    string check = std::bitset<32>(checksum).to_string(); //to binary
    string seq = std::bitset<32>(seq_no).to_string();
    string fin_send = check + seq + sub;
    return fin_send;
}
int main(){
    std::ifstream t("file.txt");
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    //string str = "sdkgjewhgksjahdgjkhjsdgie";
    
    int socket_id1, socket_id2;
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

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    
    if(setsockopt(socket_id1, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        perror("ERROR");

    char buffer[1000];
    int index = 0;
    int ACK1 = 0;
    socklen_t cl = sizeof(addr2);
    int sb = 0, sm = N+1, rn = 0, rec = 0;
	int max_rn = (str.size()+MSS-1)/MSS;
	cout << str.size() << " " << MSS << " " << max_rn << endl;

    while(true){
    		sm = (sm - sb) + rn;
        	sb = rn;
        	//cout << sb << " " << sm << " " << rn << endl;
            for(int i = sb; i < sm; i++){
  				if(i == max_rn)
  					break;
            	string temp = send_data(str, i).c_str();  
	            strcpy(buffer, temp.c_str());
	            //cout << temp.substr(64) << endl;
	            if (sendto(socket_id1, buffer, 1000, 0, (struct sockaddr *) &addr2, sizeof(addr2)) == -1){
	                pError("Send Failed\n");
	            }
            }

            for(int i = sb; i < sm; i++){
            	if(i == max_rn)
            		break;
            	if (recvfrom(socket_id1, buffer, 1000, 0, (struct sockaddr *) &addr2, &cl) == -1){
	                cout << "Timeout in receiving ACK\n"; 
	                continue;
	            }

	            //out << buffer << endl;
	            std::string str2(buffer);
	            std::string::size_type sz;
	            rec = std::stoi (str2,&sz);
	            rn = rec > rn ? rec : rn;
	        }
	        if(rn == max_rn)
	        	break;
    }
    
    close(socket_id1);
    return 0;    
}