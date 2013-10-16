Donald Pagan
Jonathan Chen
machines:	cs352-5, cs352-6
user:		csuser
password:	cs198352

	The program was organized to separate the core functions and processes of the program from the program's interface. cs352proxy.c held all of the core functions, and cs352proxy.h had all of the include statements, typedefs and function declarations, and acted as an interface for main() to access the functions of cs352proxy.c and all of the other library functions and structures.
	
	We separated main() in two cases depending on its parameter set via a switch-case statement. In case 3, that is, if there were 2 parameters passed to the program, the program would act as a server with one socket that would listen to the specified port, and another that would listen to the tap device. In case 4, with three parameters, the program would act as a client, connecting to the remote host at the specified remote port, and connecting to the tap device with another file descriptor. The procedures to extract the port number from the argument list and to create the threads that would listen to each socket was the same between each of the cases; the only main differences were in which functions were called to create the ethernet socket.
