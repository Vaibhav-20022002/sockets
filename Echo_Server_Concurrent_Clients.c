#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>     // For 'waitpid()'
#include <netinet/in.h>   // For 'struct sockaddr_in'
#include <sys/socket.h>   // For socket functions and definitions
#include <arpa/inet.h>    // For inet_ntoa() functions and definitions
#include <unistd.h>       // For close(sock_fd) system call
#include <stdlib.h>       // For exit() function


void handleClientConnections(int clientSockectFileDescriptor) {
  // 'read()' SYSTEM CALL READS DATA FROM THE GIVEN BUFFER
  // SYNTAX: 'sszie_t read(int sock_fd, void *buffer, size_t len)'
  // HERE, 'sock_fd' IS THE FILE DESCRIPTOR FOR WHERE THE DATA HAS TO BE READ,
  // HERE, '*buffer' IS THE ACTUAL BYTES WHICH HAS TO BE KEPT AFTER READING,
  // HERE, 'len' IS THE NUMBER OF BYTES TO BE READ
  // RETURNS THE NUMBER OF BYTES READ ON SUCCESS(0 INDICATES END OF FILE), '-1' ON ERROR
  char readBuffer[20];
  if (read(clientSockectFileDescriptor, readBuffer, sizeof(readBuffer)) < 0) {
    perror("SERVER::Error in reading from client...");
    close(clientSockectFileDescriptor);
    // EXITING THE CHILD PROCES WITH THE STATUS CODE '1' AS ERROR HAS OCCURED:
    exit(1);
  }
  printf("SERVER::Data from client is %s\n", readBuffer);

  // MIMICKING COMPUTATION BY THE SERVER:
  sleep(10);

  // SAME IS THE 'write()' SYSTEM CALL
  // SYNTAX: 'sszie_t write(int sock_fd, void *buffer, size_t len)'
  // NOTE: WRITING SAME AS WHAT READ, BECAUSE IT IS A ECHO SERVER !!!
  // ENSURING STRING GETS NULL-TERMINATED CORRECTLY:
  if (write(clientSockectFileDescriptor, readBuffer, strlen(readBuffer) + 1) < 0) {
    perror("SERVER::Error in writing to client...");
    close(clientSockectFileDescriptor);
    // EXITING THE CHILD PROCES WITH THE STATUS CODE '1' AS ERROR HAS OCCURED:
    // NOTE: 'return' can't be used instead of 'exit()' as return can leave the child process orphan and the child process cannot not be monitored anymore by the parent process!!!
    exit(1);
  }
  printf("SERVER::Sent data to the client\n");

  // COMMUNICATION IS ENDED SO CLOSING THE CURRENT CLIENT(ONE WITH sessionFileDescriptor):
  printf("SERVER::Closing the connection with client...\n");
  close(clientSockectFileDescriptor);
}

// NEEDS TO COLLECT THE EXIT STATUS OF THE TERMINATED CHILD PROCESS:
void handleChildSignal(int sig) {
    // WAIT FOR ALL TERMINATED CHILD PROCESSESS

    // THE 'waitpid(pid_t pid, int *status, int options)' SYSTEM CALL IS USED FOR SPECIFIC CHILD PREOCESSES TO TERMINATE AND RETRIEVE ITS TERMINATION STATUS
    // HERE, 'pid' : PID OF THE CHILD PREOCESS OF ONE WANT TO WAIT FOR.
    //      # IT HAS FOLLOWING OPTIONS:
    //          - '> 0' : WAIT FOR THE CHILD PROCESS WITY SPECIFIED PID.
    //          - '0' : WAIT FOR ANY CHILD PROCESS IN THE SAME PROCESS GROUP AS THE CALLER.
    //          - '-1' : WAIT FOR ANY CHILD PROCESS.
    // HERE, '*status' : POINTER TO AN INTEGER WHERE THE EXIT STATUS OF THE CHILD PROCESS WILL BE STORED.
    // HERE, 'options' : ADDITIONAL OPTIONS THAT CONTROL THE BEHAVIOUR OF 'waitpid()'. COMMON OPTIONS INCLUDE -
    //                   - 'WNOHANG' [NON-BLOCKING NATURE FOR 'waitpid()']
    //                   - 'WUNTRACED' [REPORT STOPPED PROCESSES], ETC...
    // RETURNS, PID OF THE TERMINATED CHILD PROCESS IF SUCCESS, OR '-1' ON ERROR/FAILURE.
    int status;
    waitpid(-1, &status, 0);
    printf("SERVER::Handling child signal and the exit status is %d\n", status);
}

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

    // SETTING UP SIGNAL HANDLER FOR CHILD SIGNALS :
    // NOTE: WHENEVER 'SIGCHLD' IS RECIEVED/CATCHED BY PARENT, IT SUSPENDS THE ACTUAL FLOW OF EXECUTION AND THEN IT CALLS 'handleChildSignal' AND THEN RESUMES ITS FLOW !!!
    signal(SIGCHLD, handleChildSignal);

    // 'accept()' SYSTEM CALL MAKE SERVER ABLE TO ACCEPT THE NEW CLIENT CONNECTIONS
    // SYNTAX: 'int accept(int sock_fd, struct sockaddr *clientAddress, socklen_t *client)'
    // HERE, 'sock_fd' IS THE SOCKET_FILE_DESCRIPTOR FOR THE SERVER,
    // HERE, 'clientAddress' IS THE CLIENT STRUCTURE 'sockaddr_in',
    // HERE, 'client' IS THE POINTER TO STORE THE RETURNED SIZE OF addr (clientAddress)
    // RETURNS (small integer) FILE DESCRIPTOR FOR THE CLIENT (KNOWN AS SESSION_FILE_DESCRIPTOR) ON SUCCESS OTHERWISE '-1' ON ERROR
    // NOTE: 'accept()' SYSTEM CALL TAKES THE FIRST CONNECTION OFF THE QUEUE FOR sockfd AND CREATE A NEW SOCKET(NEW SOCKET_FILE_DESCRIPTOR) FOR COMMUNICATING WITH THE CLIENT !!!
    while (1) {
        // CREATING A CLIENT (ASSIGNING SOCKET AND ADDRESS):
        struct sockaddr_in clientAddress;
        socklen_t len = sizeof(clientAddress);

        int sessionFileDescriptor = accept(socketFileDescriptor, (struct sockaddr*) &clientAddress, &len);
        if (sessionFileDescriptor < 0) {
            perror("SERVER::Accept for new connection failed\n");
            printf("SERVER::Closing the connection\n");
            close(socketFileDescriptor);
            return -1;
        }
        printf("SERVER::Client request accepted from %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

        // CHECK 'SYSTEM CALLS' FOLDER FOR MORE DETAILS:
        pid_t pid = fork();

        // IF CHILD PROCESS CREATION FAILED, CONTINUE TO LISTEN TO NEW CLIENTS:
        if (pid < 0) {
          perror("SERVER::Error creating child...\n");
          close(socketFileDescriptor);
          continue;
        }

        // PROCESSING NEW CLIENT CONNECTIONS:
        else if (pid == 0) {
          // CHILD PROCESS WILL NOT EXECUTE PARENT'S JOB, THUS, CLOSING SOCKET_FILE_DESCRIPTOR:
          close(socketFileDescriptor);

          // CHILD PROCESS CALLS THIS FUNCTION TO HANDLE THE CLIENT CONNECTIONS:
          handleClientConnections(sessionFileDescriptor);

          // TERMINATING THE CHILD PROCESS :
          exit(0);

          // HERE, OS AUTOMATICALLY GENERATES 'SIGCHLD' SIGNAL AND SENDS IT TO THE PARENT PROCESS:
        }

        // PROCESSING PARENT'S JOBS:
        else {
          // PARENT WILL NOT EXECUTE CHILD'S CONNECTIONS, THUS, CLOSING SESSION_FILE_DESCRIPTOR:
          close(sessionFileDescriptor);
        }
    }
    printf("SERVER::Closing the server connection...\n");
    close(socketFileDescriptor);
    return 0;
}
