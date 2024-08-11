#include <stdio.h>
#include <netinet/in.h>   // For 'struct sockaddr_in'
#include <sys/socket.h>   // For socket functions and definitions
#include <arpa/inet.h>    // For inet_ntoa() functions and definitions
#include <unistd.h>       // For close(sock_fd) system call

int main() {
    int ret = -1, socketFileDescriptor = -1;
    char buffer[100];

    struct sockaddr_in server, client;
    socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFileDescriptor < 0) {
        perror("SERVER::Socket Creation failed...\n");
        return -1;
    }

    printf("SERVER::Socket Created successfully...\n");

    // ASSIGNING PORT AND ADDRESS TO SOCKET:
    server.sin_port = htons(8076);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // LOOPBACK ADDRESS

    // BIND ADDRESS TO SOCKET:
    ret = bind(socketFileDescriptor, (struct sockaddr*)&server, sizeof server);
    if (ret < 0) {
        perror("SERVER::Socket binding failed...\n");
        close(socketFileDescriptor);
        return -1;
    }

    printf("SERVER::Socket binded to %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
    printf("SERVER::Server is ready to commute...\n");

    // RECEIVING FROM CLIENT:
    // 'recvfrom()' SYSTEM CALL:
    // NOTE: SAME AS 'sendto()' SYSTEM CALL BUT OPPPOSITE!!!
    socklen_t len = sizeof(client);
    ret = recvfrom(socketFileDescriptor, (void *)&buffer , sizeof(buffer), 0, (struct sockaddr *)&client, &len);
    if (ret < 0) {
        perror("SERVER::Message failed to receive...\n");
        close(socketFileDescriptor);
        return -1;
    }

    printf("SERVER::Message received from client %s is '%s'", inet_ntoa(client.sin_addr), buffer);

    // SENDING TO CLIENT:
    // 'sentto()' SYSTEM CALL, USED TO SEND A MESSAGE TO SPECID\FIC DESTINATION, AND IS USED IN DATAGRAM BASED PROTOCOLS, WHERE INDIVIDUALLY PACKET IS SENT WITHOUT ESTABLISHING A CONNECTION:
    // SYNTAX: 'ssize_t sendto(int fd, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len)'
    // HERE, 'fd': SOCKET FILE DESCRIPTOR FROM WHERE DATA HAS TO BE SENT.
    // HERE, 'buf': POINTER TO BUFFER CONTAINING DATA TO BE SENT.
    // HERE, 'n': SIZE OF DATA IN BYTES.
    // HERE, 'flags': BITWISE OR (|) OF ZERO AND MORE OPTIONS. GENERALLY '0'.
    // HERE, 'addr': POINTER TO 'sockaddr' STRUCTURE THAT CONTAINS THE DESTINATION ADDRESS(INCLUDING IP ADDRESS & PORT), WHERE DATA HAS TO BE SENT.
    // HERE, 'addr_len': SIZE OF ADDRESS STRUCTURE POINTER BY 'addr'
    // RETURNS, NUMBER OF BYTES SENT OR '-1' OR 'errno' ON ERROR/FAILURE.
    ret = sendto(socketFileDescriptor, (void *)&buffer, sizeof buffer, 0, (struct sockaddr *)&client, len);
    if (ret <  0) {
        printf("SERVER::Could not send data to the client %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
        close(socketFileDescriptor);
        return -1;
    }

    printf("SERVER::Data sent to the client %s\n", inet_ntoa(client.sin_addr));

    close(socketFileDescriptor);
    printf("SERVER::Communication ended...\nClosing the Server...\n");
    return 0;
}
