/**
 * Suppose we have three processes related as grandparent, parent and child 
 * and that grandparent does not immediately perform a wait after the parent exits, 
 * so that the parent becomes a zombie. When do you think the grandchild will be 
 * adopted by init (so that getppid() in the grandchild returns 1): after the parent 
 * terminates or after the grandparent does a wait()? Write a program to verify the answer.
 * 
 * Answer: The grandchild will be adopted by init after the parent terminates,
 * regardless of whether the grandparent does a wait() or not. This is because once the parent
 * process terminates, the grandchild becomes an orphan and is adopted by init.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t gpid;                         // The grandparent process ID.
    pid_t ppid;                         // The parent process ID.
    pid_t cpid;                         // The child process ID.
    gpid = getpid();                    // Get the grandparent process ID.
    printf("Grandparent PID: %ld\n", (long) gpid);

    // Fork the grandparent to create the parent.
    ppid = fork();                      // Create the parent.
    if (ppid == -1)                     // Did the fork fail?
    {
        perror("fork");                 // Yes, print an error message.
        exit(EXIT_FAILURE);             // Exit the program.
    }

    if (ppid == 0)                      // Is it the parent?
    {                                   // Yes
        ppid = getpid();                // Get the parent process ID.
        printf("Parent: Grandparent PID: %ld\n", (long) getppid());

        // Fork the parent to create the child:
        cpid = fork();                  // Create the child.
        if (cpid == -1)                 // Did we fail to create the child?
        {                               // Yes
            perror("fork");             // Print an error message.
            exit(EXIT_FAILURE);         // Exit the program.
        }

        if (cpid == 0)                  // Is it the child?
        {                               // Yes
            printf("Child: My parent PID: %ld\n", (long) getppid());
            sleep(5);                   // Sleep for 5 seconds.
            printf("Child: My parent PID after sleep: %ld\n", (long) getppid());
            exit(EXIT_SUCCESS);         // Exit
        }
        else                            // It is the parent.
        {                               // Wait for the child to exit.
            printf("Parent: Exiting\n");
            exit(EXIT_SUCCESS);         // Exit the parent.
        }
    }
    else                                // It is the grandparent.
    {
        sleep(10);                      // Sleep to ensure the parent becomes a zombie.
        printf("Grandparent: Waiting for child processes to terminate.\n");
        wait(NULL);                     // Wait for the parent process to terminate.
        wait(NULL);                     // Wait for the child process to terminate.
        printf("Grandparent: All child processes terminated.\n");
    }

    return 0;                           // If we got here we are done.
}