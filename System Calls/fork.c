// The 'fork()' system call is used for creating a new process.

// SYNTAX: 'int fork()' (fork() takes no arguements)

// The child process created uses the same Program Counter(PC), same CPU registers and same open files which are in use by the parent process.
// After child creation, both the parent and child processes executes the next following instruction.

// RETURNS :
//    ->  0, if the process is the CHILD process
//    -> >0, if the process is the PARENT process (it returns the process ID(PID) of the child process)
//    -> -1, if fork() fails to create child process and the value is returned to the PARENT process

// HEADER FILE :
//     #include <unistd.h>

// # NOTES :
//     -> Both PARENT and CHILD process runs concurrently and may execute in any order relative to each other. Therefore, PARENT and CHILD process can execute different tasks simultaneously.
//     -> The CHILD process is the copy of the PARENT process. There is a technique called COPY-ON-WRITE that optimises the memory usage of the overall system call, meaning that the memory pages are only copied if they are modified.

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid;

    // CREATING A CHILD PROCESS :
    pid = fork();

    // IF CHILD CREATION FAILED :
    if (pid < 0) {
        perror("Child process creation failed...\n");
        return 1;
    }

    // THIS BLOCK IS ONLY EXECUTED BY THE CHILD PROCESS :
    else if (pid == 0) {
        printf("\tChild Process::Hello, I'm the child process with pid %d\n", getpid());
    }

    // THIS BLOCK IS GOING TO BE EXECUTED BY THE PARENT PROCESS :
    else {
        printf("Parent Process::Parent's process ID is %d\n", getpid());
        printf("Parent Process::Child's process ID is %d\n", pid);

        // 'waitpid()' SYSTEM CALL IS USED TO WAIT FOR SPECIFIC CHILD PROCESS TO TERMINATE & RETRIEVE ITS TERMINATION STATUS.
        // SYNTAX: 'pid_t waitpid(pid_t pid, int *status, int options)'
        // HERE, 'pid', PID OF THE CHILD PROCESS TO WAIT FOR,
        //             -> IF 'pid == -1', WAIT FOR ANY CHILD PROCESS
        //             -> IF 'pid == 0', WAIT FOR ANY CHILD PROCESS OF THE CURRENT PROCESS
        //             -> IF 'pid > 0', WAIT FOR THE CHILD PROCESS WITH THE GIVEN PID
        // HERE, 'status', POINTER TO THE INTEGER WHERE EXIT STATUS OF CHILD PROCESS WILL BE STORED,
        // HERE, 'options', ADDITIONAL OPTIONS THAT CONTROL THE BEHAVIOUR OF waitpid(),
        // RETURNS, 'PID' OF THE TERMINATED CHILD PROCESS IF PROCESS IS SUCCESSFUL, OE '-1' IF AN ERORR OCCURED
        int status;
        printf("Parent Process::Waiting for child with pid %d to terminate...\n", pid);
        waitpid(pid, &status, 0);

        // WIFEXITED(status) QUERIES THE CHILD TERMINATION STATUS PROVIDED BY THE wait() or waitpid() FUNCTIONS AND DETERMINES WHETHER THE CHILD PROCESS ENDED NORMALLY.
        if (WIFEXITED(status)) {
            printf("Parent Process::Child with pid %d terminated normally with exit status %d\n", pid, WEXITSTATUS(status));
        }
    }
    return 0;
}