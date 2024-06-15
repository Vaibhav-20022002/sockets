#include <stdio.h>
#include <string.h>
#include <netinet/in.h>   // For 'struct sockaddr_in'
#include <sys/socket.h>   // For socket functions and definitions
#include <arpa/inet.h>    // For inet_ntoa() functions and definitions
#include <unistd.h>       // For close(sock_fd) system call

int main() {
    // SOCKET FILE DESCRIPTOR IS AN INTEGER ASSOCIATED WITH AN OPEN FILE:
    int socketFileDescriptor = -1;

    // SOCKET CREATION:
    struct sockaddr_in serverAddress;

    // 'socket()' SYSTEM CALL ALLOCATES RESOURCES NEEDED FOR A COMMUNICATION ENDPOINT
    // SYNTAX: "int socket(int FAMILY, int TYPE, int PROTOCOL)" SYSTEM CALL
    // HERE, FAMILY = AF_INET FOR IPV4 INTERNET SOCKETS,
    // HERE, TYPE = SOCK_STREAM FOR 'TCP' CONNECTION,
    // RETURNS 'SOCKET DESCRIPTOR' (small integer) on success, and
    // RETURNS '-1' on failure/error
    // NOTE: 'socket()' SYSTEM CALL DOESN'T DEAL WITH ENDPOINT ADDRESSING !!!
    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor < 0) {
        // SAME AS 'cerr << "Socket Creation Failed!"'
        perror("Socket Creation Failed!\n");
        return -1;
    }
    printf("SERVER::Socket Created Successfully\n");

    // INITIALIZING A SOCKET ADDRESS:
    // STRUCTURE OF IPV4 SOCKET IS 'sockaddr_in', IT IS USED TO GIVE ADDRESS FOR IPV4 SOCKETS
    // IT'S BODY IS DEFINED AS FOLLOWS:
    //      struct sockaddr_in {
    //          short sin_family; // ADDRESS FAMILY e.g. AF_INET
    //          uint16_t sin_port; // PORT NUMBER IN NETWORK BYTE ORDER
    //          struct in_addr sin_addr; // INTERNET ADDRESS
    //          char sin_zero[8]; // PAD BYTES
    //      };
    //
    //      struct in_addr {
    //          uint32_t s_addr; // INTERNET ADDRESS LOAD WITH inet_aton()
    //      };

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // ANY ARBITRARY PORT
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // LOOPBACK ADDRESS

    // 'bind()' SYSTEM CALL ASSIGNS AN ADDRESS TO AN EXISITING SOCKET
    // SYNTAX : 'int bind(int sockfd, const struct sockaddr *myaddr, int addrlen)'
    // ON SUCCESS RETURNS 0, AND '-1' ON ERROR
    // NOTE : ONLY THE SERVER HAS TO BE BINDED !!!

    int ret = bind(socketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (ret < 0) {
        perror("Bind failed!\n");
        printf("SERVER::Closing the connection\n");
        // 'close(sock_fd)' SYSTEM CALL RELEASES RESOURCES ASSIGNED TO SOCKET WITH SOCKET_FILE_DESCRIPTOR :
        close(socketFileDescriptor);
        return -1;
    }

    // NOTE: IP ADDRESS AND PORT NUMBER ON MACHINE HAS TO BE IN HOST ORDER(NOT IN NETWORK ORDER) !!!
    printf("SERVER::Bind successfully with %s:%d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));

    // 'listen()' SYSTEM CALL LISTENS TO NEW INCOMING CLIENTS
    // SYNTAX: 'int listen(int sock_fd, int backlog)'
    // HERE, 'sock_fd' IS SOCKET_FILE_DESCRIPTOR OF serverAddress
    // HERE, 'backlog' IS MAXIMUM NUMBER OF CONNECTIONS THAT THE SERVER SHOULD QUEUE FOR THIS SOCKET
    // RETURNS 0 IF SUCCESSFUL, OTHERWISE '-1' ON ERROR
    // NOTE: HISTORICALLY, 5 CONNECTIONS WERE IDEALLY MADE BUT NOW RARELY ABOVE 15 ARE IDEALLY MADE FOR EVEN MODERATE WEB SERVER
    // NOTE: ONLY THE SERVER NEEDS TO LISTEN !!!
    ret = listen(socketFileDescriptor, 5);
    if (ret < 0) {
        perror("SERVER::Listening failed\n");
        printf("SERVER::Closing the connection\n");
        close(socketFileDescriptor);
        return -1;
    }
    printf("SERVER::Listening to new connections...\n");

    // CREATING A CLIENT (ASSIGNING SOCKET AND ADDRESS):
    struct sockaddr_in clientAddress;
    int len = sizeof(clientAddress);

    // 'accept()' SYSTEM CALL MAKE SERVER ABLE TO ACCEPT THE NEW CLIENT CONNECTIONS
    // SYNTAX: 'int accept(int sock_fd, struct sockaddr *clientAddress, socklen_t *client)'
    // HERE, 'sock_fd' IS THE SOCKET_FILE_DESCRIPTOR FOR THE SERVER,
    // HERE, 'clientAddress' IS THE CLIENT STRUCTURE 'sockaddr_in',
    // HERE, 'client' IS THE POINTER TO STORE THE RETURNED SIZE OF addr (clientAddress)
    // RETURNS (small integer) FILE DESCRIPTOR FOR THE CLIENT (KNOWN AS SESSION_FILE_DESCRIPTOR) ON SUCCESS OTHERWISE '-1' ON ERROR
    // NOTE: 'accept()' SYSTEM CALL TAKES THE FIRST CONNECTION OFF THE QUEUE FOR sockfd AND CREATE A NEW SOCKET(NEW SOCKET_FILE_DESCRIPTOR) FOR COMMUNICATING WITH THE CLIENT !!!
    int sessionFileDescriptor = accept(socketFileDescriptor, (struct sockaddr*) &clientAddress, &len);
    if (sessionFileDescriptor < 0) {
        perror("SERVER::Accept for new connection failed\n");
        printf("SERVER::Closing the connection\n");
        close(socketFileDescriptor);
        return -1;
    }
    printf("SERVER::Client request accepted from%s:%hd\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

    // 'read()' SYSTEM CALL READS DATA FROM THE GIVEN BUFFER
    // SYNTAX: 'sszie_t read(int sock_fd, void *buffer, size_t len)'
    // HERE, 'sock_fd' IS THE FILE DESCRIPTOR FOR WHERE THE DATA HAS TO BE READ,
    // HERE, '*buffer' IS THE ACTUAL BYTES WHICH HAS TO BE KEPT AFTER READING,
    // HERE, 'len' IS THE NUMBER OF BYTES TO BE READ
    // RETURNS THE NUMBER OF BYTES READ ON SUCCESS(0 INDICATES END OF FILE), '-1' ON ERROR
    char readBuffer[20];
    read(sessionFileDescriptor, readBuffer, sizeof(readBuffer));
    printf("SERVER::Data from client is %s\n", readBuffer);

    // SAME IS THE 'write()' SYSTEM CALL
    // SYNTAX: 'sszie_t write(int sock_fd, void *buffer, size_t len)'
    // NOTE: WRITING SAME AS WHAT READ, BECAUSE IT IS A ECHO SERVER !!!
    write(sessionFileDescriptor, readBuffer, strlen(readBuffer));
    printf("SERVER::Sent data to the client\n");

    // COMMUNICATION IS ENDED SO CLOSING THE CURRENT CLIENT(ONE WITH sessionFileDescriptor):
    printf("SERVER::Closing the connection with client\n");
    close(sessionFileDescriptor);


    printf("SERVER::Closing the connection\n");
    close(socketFileDescriptor);
    return 0;
}