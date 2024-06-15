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
    printf("Socket Created Successfully\n");

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
        // 'close(sock_fd)' SYSTEM CALL RELEASES RESOURCES ASSIGNED TO SOCKET WITH SOCKET_FILE_DESCRIPTOR :
        close(socketFileDescriptor);
        return -1;
    }

    // NOTE: IP ADDRESS AND PORT NUMBER ON MACHINE HAS TO BE IN HOST ORDER(NOT IN NETWORK ORDER) !!!
    printf("Bind successfully with %s:%d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));

    close(socketFileDescriptor);
    return 0;
}