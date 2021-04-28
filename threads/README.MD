### CONCURRENT CONNECTIN ORIENTED SERVER
- Compile the server `gcc server.c -pthread -o server`
- Run the server code by  `./server portnumber`
- Compile the cleint by th command `gcc -pthread client.c -o client`
- Run the cleint code by `./client username portnumber`
- In case of the error **binding unsuccessful: Permission denied** when compiling, comile with root permissions