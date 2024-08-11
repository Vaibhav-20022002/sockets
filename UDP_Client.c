#include <stdio.h>
#include <netinet/in.h>   // For 'struct sockaddr_in'
#include <sys/socket.h>   // For socket functions and definitions
#include <arpa/inet.h>    // For inet_addr() function
#include <unistd.h>       // For close() system call
#include <string.h>       // For strlen() function

int main() {
    int ret = -1, socketFileDescriptor = -1;
    char buffer[100];
    struct sockaddr_in server;

    // CREATE SOCKET:
    socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFileDescriptor < 0) {
        perror("CLIENT::Socket Creation failed...\n");
        return -1;
    }

    printf("CLIENT::Socket Created successfully...\n");

    // CONFIGURE SERVER ADDRESS:
    server.sin_family = AF_INET;
    server.sin_port = htons(8076);
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // LOOPBACK ADDRESS

    // PREPARE MESSAGE:
    printf("CLIENT::Enter message to send to server: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline character

    // SEND MESSAGE TO SERVER:
    ret = sendto(socketFileDescriptor, (void *)buffer, strlen(buffer), 0, (struct sockaddr *)&server, sizeof(server));
    if (ret < 0) {
        perror("CLIENT::Message failed to send...\n");
        close(socketFileDescriptor);
        return -1;
    }

    printf("CLIENT::Message sent to server %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));

    // RECEIVE ECHO FROM SERVER:
    socklen_t len = sizeof(server);
    ret = recvfrom(socketFileDescriptor, (void *)buffer, sizeof(buffer), 0, (struct sockaddr *)&server, &len);
    if (ret < 0) {
        perror("CLIENT::Message failed to receive...\n");
        close(socketFileDescriptor);
        return -1;
    }

    printf("CLIENT::Echoed message received from server: '%s'\n", buffer);

    close(socketFileDescriptor);
    printf("CLIENT::Communication ended...\nClosing the Client...\n");
    return 0;
}
