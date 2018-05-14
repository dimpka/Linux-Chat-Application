
● READ ME


●	Code
○	Included in this project are two files: ClientApp.c and ServerApp.py
■	The client was written in C.
■	The server was written in python 
■       Communication happens over TCP

●	Compilation
○	How to compile the the code: Compile the code client app by using gcc command like this: gcc -o client ClientApp.c -pthread (alternatively, you can compile the client app by running gcc -o client ClientApp.c)
○	Execute the client by supplying it three arguments: hostname, server port#, and myport#
■	Hostname is the hostname of the udp server
■	The server port# is the port number of the udp server
■	myport# is the port number where the ClientApp.c wants to chat


○	Compile and run ServerApp.py by using:  python ClientApp.py port#
●	Port# is the port where this udp server can be reached
