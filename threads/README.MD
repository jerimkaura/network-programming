### CONCURRENT CONNECTION ORIENTED SERVER
- Compile the server `gcc server.c -pthread -o server`
- Run the server code by  `./server portnumber`
- Compile the cleint by th command `gcc -pthread client.c -o client`
- Run the cleint code by `./client username portnumber`
- In case of the error **binding unsuccessful: Permission denied** when compiling, comile with root permissions

Note: The port number of the server must be similar to that of the client. 

### MODEL
The server is an intermediary between the clients. It receives a message from one client and forwards to all other clients except the one form which the message came from. 

The server has two threads running concurrently. One for sending the message to cleints and another for receiving incoming messages from the cleints.

To join the chat, each client must provide thier username for identifcation by other clients in the chat. However, the server uniquely idetifies the clients by their ip addresses.

### GROUP CHAT
For a group chat, every client must connect to the server theough the same socket. For instance if the server is runnning on port 4001, then all clients that might wnat to join the group chat will specify thier port numbers as 4001 as well.

### CLIENT TO CLIENT CHAT
If two clients wants to chat privately, they can use a different port private for them. 




*I dont know what i am doing* :confused: 

