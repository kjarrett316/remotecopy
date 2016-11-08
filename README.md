# remotecopy
##Usage
###Compile
```
gcc -o remcpd remcpd.c
gcc -o remcp remcp.c 
```
###Run
Start the server.
`./remcpd`

Then run  the client.

`./rempcp HOST:FILE`

##Example

Server:
```
$ ./remcpd
Client says: <remcp>
Client says: <CS30615spr>
Client says: ~/Desktop/test/kitty
~/Desktop/test/kitty exists and is readable.
Client says: <send>
Transfer complete.
```

Client:
```
$ ./remcp 127.0.0.1:~/Desktop/test/kitty
Server says: <remcp>
Server says: <ok>
Server says: <ready>
Ready to recieve file.
File recieved. 
```
