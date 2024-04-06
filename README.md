# Chatroom Application Using C++
This chatroom application is written in C++ utilizing web socket concepts for the Linux operating system. This project allows multiple clients to communicate with each other through the server as shown in the following image.

## How to Build
1. Clone the repository and put the `.cpp` files in the Home tab of your files explorer.
2. Run the following commands in the terminal:
</br> NOTE: To paste, use `CTRL+SHIFT+V`.
```
g++ server.cpp -lpthread -o server
```
```
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

## How to Use
- Once client application has been ran, you can enter the username you want to use.
- Then, you will be able to send messages to all other clients on the server.
- To leave the chat room, you can send `Bye` or `bye`, or use `CTRL+C`. 

### Private Messaging
To send a private message:
- Start your message with `@Name` with Name being your desired client's username.
- Follow this with your message.
- As a result, your message will be sent to a specific client. 
</br> Example: "@Khoa This is a secret."

## References
- [ ] https://github.com/cjchirag7/chatroom-cpp/tree/main
- [ ] https://github.com/nnnyt/chat/tree/master
