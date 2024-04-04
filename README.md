# Chatroom Application Using C++
This chatroom application is written in C++ utilizing web socket concepts. This project can be built on Linux where two clients can communicate with each other through the server.

## How to Build
1. Clone the repository.
2. Run the following commands in the terminal:
```
g++ server.cpp -lpthread -o server
g++ client.cpp -lpthread -o client
```
3. To run the server application, run the following command in the terminal:
```
./server
```
4. To run the client application, open a new terminal window and run the following command:
```
./client
```
5. To run multiple client applications, repeat Step 4. 

