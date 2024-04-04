#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <mutex>
#include <ctime>

#define MAX_LEN 200
#define NUM_COLORS 6

using namespace std;


// Application Variables
bool exit_flag = false;
string last_sender;
thread t_send, t_recv;
int client_socket;
string def_col = "\033[0m";		// default color
string grey_col = "\033[37m";		// grey
string client_colors[] = { 
	"\033[31m", 			// red 
	"\033[32m", 			// green
	"\033[33m", 			// yellow
	"\033[34m", 			// blue
	"\033[35m", 			// purple
	"\033[36m"};			// cyan


// Function declarations
void catch_ctrl_c(int signal);
int eraseText(int cnt);
void send_message(int client_socket);
void recv_message(int client_socket);
string getTime();


int main(){
	// Creates the client socket
	if((client_socket = socket(AF_INET,SOCK_STREAM, 0)) == -1){
		perror("Error creating client socket... ");
		exit(-1);
	}

	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port=htons(10000); // Port no. of server
	client.sin_addr.s_addr = INADDR_ANY;
	bzero(&client.sin_zero, 0);
	
	// Connect client to server
	if((connect(client_socket, (struct sockaddr *)&client, sizeof(struct sockaddr_in))) == -1){
		perror("Error connecting client... ");
		exit(-1);
	}
	signal(SIGINT, catch_ctrl_c);
	char name[MAX_LEN];
	cout << grey_col << "Enter your name : " << def_col;
	cin.getline(name, MAX_LEN);
	send(client_socket, name, sizeof(name), 0);

	cout << grey_col << "\n\t  ====== Welcome to the chat-room ======   "<< endl << def_col;

	thread t1(send_message, client_socket);
	thread t2(recv_message, client_socket);

	t_send = move(t1);
	t_recv = move(t2);

	if(t_send.joinable()){
		t_send.join();
	}
	
	if(t_recv.joinable()){
		t_recv.join();
	}
			
	return 0;
}

// Handler for "Ctrl + C"
void catch_ctrl_c(int signal){
	char str[MAX_LEN]="Bye";
	send(client_socket, str, sizeof(str), 0);
	exit_flag = true;
	t_send.detach();
	t_recv.detach();
	close(client_socket);
	cout << grey_col << "\n\t  ====== You have left the chat-room ======   "<< endl << def_col;
	exit(signal);
}


// Send message to everyone
void send_message(int client_socket){
	while(true){
		cout << grey_col << "You : " << def_col;
		char str[MAX_LEN];
		cin.getline(str, MAX_LEN);
		send(client_socket, str, sizeof(str), 0);
		last_sender = "";
		if(strcmp(str, "Bye") == 0 || strcmp(str, "bye") == 0){
			exit_flag = true;
			t_recv.detach();	
			close(client_socket);
			cout << grey_col << "\n\t  ====== You have left the chat-room ======   "<< endl << def_col;
			return;
		}
	}
}


// Receive message
void recv_message(int client_socket){
	while(true){
		if(exit_flag){
			return;
		}
		
		char name[MAX_LEN], str[MAX_LEN];
		int color_code;
		int bytes_received = recv(client_socket, name, sizeof(name), 0);
		if(bytes_received <= 0){
			continue;
		}
		
		recv(client_socket, &color_code, sizeof(color_code), 0);
		recv(client_socket, str, sizeof(str), 0);
		eraseText(6);
		
		if(strcmp(name, "#NULL") != 0){
			if(last_sender != name){
				cout << client_colors[color_code%NUM_COLORS] << name <<" :   "<<grey_col<<getTime()<<endl;
			}
			cout <<"   "<<def_col<<str<<endl;
			last_sender = name;
		}else{
			cout << client_colors[color_code%NUM_COLORS] << str << endl;
			last_sender = "";
		}
		cout << grey_col << "You : " << def_col;
		fflush(stdout);
	}	
}


// Erase text from terminal
int eraseText(int cnt){
	char back_space = 8;
	for(int i = 0; i < cnt; i++){
		cout << back_space;
	}
	
}


// Returns the current time
string getTime(){
	time_t now = time(0);
   	tm *ltm = localtime(&now);
   	int time_hh = ltm->tm_hour;
   	string time_mm = std::to_string(ltm->tm_min);
   	time_mm.insert(time_mm.begin(), 2 - time_mm.length(), '0');
   	
   	if(time_hh >= 12){
   		return std::to_string(time_hh-12)+":"+time_mm+" PM";
	}
	
	return std::to_string(time_hh)+":"+time_mm+" AM";
}

