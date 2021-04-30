### CONCURRENT CONNECTION ORIENTED SERVER

- Compile the server `gcc server.c -pthread -o server`
- Run the server code by  `./server portnumber`
- Compile the client by the command `gcc -pthread client.c -o client`
- Run the client code by `./client username portnumber`
- In case of the error **binding unsuccessful: Permission denied** when compiling, compile with root permissions or use port numbers > 1024

Note: The client must provide the same port that the server is running on

# Chat App Model

Chatting model: Group chat

The server is an intermediary between the clients. It receives a message from one client and forwards it to all other clients except the one from which the message came from. 

The server has two threads running concurrently. One for sending the message to cleints and another for receiving incoming messages from the clients.

To join the chat, each client must provide their username for identifcation by other clients in the chat. However, the server uniquely identifies the clients by their ip addresses.
