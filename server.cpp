#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <ctime>


#define MAX_LEN 200
#define NUM_COLORS 6

using namespace std;


// Program variables
struct client_info{
	int id;
	string name;
	int socket;
	thread thd;
};

vector<client_info> clients;
string def_col = "\033[0m";		// default color
string grey_col = "\033[37m";		// grey
string client_colors[] = { 
	"\033[31m", 			// red 
	"\033[32m", 			// green
	"\033[33m", 			// yellow
	"\033[34m", 			// blue
	"\033[35m", 			// purple
	"\033[36m"};			// cyan
int seed = 0;
mutex cout_mtx, clients_mtx;


// Function declarations 
void handle_client(int client_socket, int id);
void set_name(int id, char name[]);
void shared_print(string str0, string str1, bool endLine);
int broadcast_message(string message, int sender_id);
int broadcast_message(int num, int sender_id);
string getTime();
void end_connection(int id);


int main(){
	// Creates the server socket
	int server_socket;
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Error creating server socket... ");
		exit(-1);
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(10000);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(&server.sin_zero, 0);

	// Binds the server socket to the address
	if((bind(server_socket, (struct sockaddr *)&server, sizeof(struct sockaddr_in))) == -1){
		perror("Error binding server socket... ");
		exit(-1);
	}

	// Allows the server socket to look for client connections
	if((listen(server_socket, 8)) == -1){
		perror("Error in listening... ");
		exit(-1);
	}

	struct sockaddr_in client;
	int client_socket;
	unsigned int len=sizeof(sockaddr_in);

	cout << grey_col << "\n\t  ====== Chat-room is now being hosted by you ======   " << endl << def_col;

	while(true){
		// Accepts client requests
		if((client_socket = accept(server_socket, (struct sockaddr *)&client, &len)) == -1){
			perror("Error in accepting client... ");
			exit(-1);
		}
		seed++; // Increments for each client accepted
		thread t_client(handle_client, client_socket, seed);
		lock_guard<mutex> guard(clients_mtx);
		clients.push_back({seed, string("Anonymous"), client_socket, (move(t_client))});
	}

	for(int i = 0; i < clients.size(); i++){
		if(clients[i].thd.joinable()){
			clients[i].thd.join();
		}
	}

	close(server_socket);
	cout << grey_col << "\n\t  ====== Chat-room has been closed ======   " << endl << def_col;
	return 0;
}


void handle_client(int client_socket, int id){
	char name[MAX_LEN], str[MAX_LEN];
	recv(client_socket, name, sizeof(name), 0);
	
	// Sets the name of the client
	for(int i = 0; i < clients.size(); i++){
		if(clients[i].id == id){
			clients[i].name = string(name);
		}
	}

	// Display welcome message
	string welcome_message = string(name)+" has joined";
	broadcast_message("#NULL", id);	
	broadcast_message(id, id);								
	broadcast_message(welcome_message, id);	
	shared_print(client_colors[id%NUM_COLORS]+welcome_message+def_col, "", false);
	
	while(true){
		int bytes_received = recv(client_socket, str, sizeof(str), 0);
		if(bytes_received <= 0){
			return;
		}
		
		// Client can leave by typing "Bye" or "bye"
		if(strcmp(str, "Bye") == 0 || strcmp(str, "bye") == 0){
			// Display leaving message
			string message = string(name)+string(" has left");		
			broadcast_message("#NULL", id);			
			broadcast_message(id, id);						
			broadcast_message(message, id);
			shared_print(client_colors[id%NUM_COLORS]+message+def_col, "", false);
			end_connection(id);
			return;
		}
		
		broadcast_message(string(name), id);					
		broadcast_message(id, id);		
		broadcast_message(string(str), id);
		shared_print(client_colors[id%NUM_COLORS]+name+" :    "+grey_col+getTime(), "   "+def_col+str, true);		
	}	
}


// For synchronisation of cout statements
void shared_print(string str0, string str1, bool endLine){
	lock_guard<mutex> guard(cout_mtx);
	cout << str0 << endl << str1 << endl;
	if(endLine){
		cout << endl;
	}
}


// Broadcast message to all clients except the sender
int broadcast_message(string message, int sender_id){
	char temp[MAX_LEN];
	strcpy(temp,message.c_str());
	for(int i = 0; i < clients.size(); i++){
		if(clients[i].id != sender_id){
			send(clients[i].socket, temp, sizeof(temp), 0);
		}
	}		
}


// Broadcast a number to all clients except the sender
int broadcast_message(int num, int sender_id){
	for(int i = 0; i < clients.size(); i++){
		if(clients[i].id != sender_id){
			send(clients[i].socket, &num, sizeof(num), 0);
		}
	}
}


// Returns the current time
string getTime(){
	time_t now = time(0);
   	tm *ltm = localtime(&now);
   	int time_hh = ltm->tm_hour;
   	string time_mm = std::to_string(ltm->tm_min-33);
   	time_mm.insert(time_mm.begin(), 2 - time_mm.length(), '0');
   	
   	if(time_hh >= 12){
   		return std::to_string(time_hh-12)+":"+time_mm+" PM";
	}
	
	return std::to_string(time_hh)+":"+time_mm+" AM";
}


void end_connection(int id){
	for(int i = 0; i < clients.size(); i++){
		if(clients[i].id == id){
			lock_guard<mutex> guard(clients_mtx);
			clients[i].thd.detach();
			clients.erase(clients.begin()+i);
			close(clients[i].socket);
			break;
		}
	}
}

