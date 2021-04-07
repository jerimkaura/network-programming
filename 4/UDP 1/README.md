### UDP CHAT

#### Changes

- Connectionless 
- We user port number and IP address
- We use bind function to bind the nodes with their IP-addresses
- We use `recvfrom` and `sendto` instead of `recv` and `send` used in TCP.
- Same ip address was used as the program is tested on the same machine i.e `127.0.0.1`