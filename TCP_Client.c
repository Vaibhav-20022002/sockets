#include <stdio.h>
#include <string.h>
#include <netinet/in.h>   // For 'struct sockaddr_in'
#include <sys/socket.h>   // For socket functions and definitions
#include <arpa/inet.h>    // For inet_ntoa() functions and definitions
#include <unistd.h>       // For close(sock_fd) system call

int main() {
    int socketFileDescriptor = -1;

    // CREATING CLIENT-SIDE SOCKET :
    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor < 0) {
        perror("CLIENT::Socket Creation Failed\n");
        close(socketFileDescriptor);
    }
    printf("CLIENT::Socket Created Successfully\n");

    // MENTIONING SERVER DETAILS TO CLIENT:
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // LOOPBACK ADDRESS

    // 'connect()' SYSTEM CALL IS USED THE CLIENT WITH THE SERVER
    // SYNTAX: 'int connect(int sock_fd, const struct sockaddr *serverAddress, socklen_t addrlen)'
    // HERE, 'sock_fd' IS THE SOCKET_FILE_DESCRIPTOR OF TH \E CLIENT,
    // HERE, 'serverAddress' IS THE STRUCTURE WITH SERVER DETAILS,
    // HERE, 'addrlen' IS THE SIZE OF STRUCTURE 'serverAddress',
    // RETURNS, (small integer) SOCKET_FILE_DESCRIPTOR ON SUCCESS, OTHERWISE '-1' ON FAILURE/ERROR
    int ret = connect(socketFileDescriptor, (const struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (ret < 0) {
        perror("CLIENT::Connection error while connecting to server\n");
        close(socketFileDescriptor);
        return -1;
    }
    printf("CLIENT::Connection established with server at %s:%d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));

    // COMMUNICATING WITH SERVER:
    char buffer[20];
    strncpy(buffer, "Hello...", 9);
    // WRITING TO THE SERVER:
    // ENSURING STRING GETS NULL-TERMINATED CORRECTLY:
    write(socketFileDescriptor, buffer, strlen(buffer) + 1);
    printf("CLIENT::Data written to the server\n");

    memset(buffer, 0, sizeof(buffer));

    // READING FROM THE SERVER:
    read(socketFileDescriptor, buffer, sizeof(buffer));
    printf("CLIENT::Data read from the server is %s\n", buffer);

    printf("CLIENT::Communication ended...\n");
    printf("CLIENT::Closing the connection with the server...\n");
    close(socketFileDescriptor);

    return 0;
}