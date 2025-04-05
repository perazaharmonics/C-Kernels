/**
 * This program is an implementation of system() call conforming to appropiate rules to handle the call.
 * If a cmd is a NULL pointer, then system() should return a non-zero value if a shell is available, or 0
 * if no shell is available. The only way to reliably determine this information is to try to execute a shell.
 * We do this by recuresively calling system() to execute the : shell command and checking for a return 
 * status of 0 from the recursive call. (1) The : cmd is a shell built-in cmd that does nothing, but
 * always returns a success status. We could have executed the shell cmd exit 0 to achieve the same result.
 * (Note that it is not sufficient to use access() to check whether the file /bin/sh exists and has execute permission
 * enabled. In a chroot() environment, even if a shell executable is present, it may not be able to be executed if
 * it is dynamically linked and the required shared libraries are not available.).
 * 
 * It is only in the parent process (the caller of system()) that SIGCHLD needs to be blcoed (2), and SIGINT
 * and SIGQUIT need to be ignored. (3) However, we must perform these actions prior to the fork() call,
 * because if they are done in the parent after the for(), we would create a race condition. Consequently, the child
 * must undo these changes to the signal attributes. 
 * 
 * In the parent we ignore errors from the sigaction() and sigprocmask() calls used to manipulate signal dispositions
 * and the signal masks (2) (3) (9). We do this for two reasons. First, these calls are very unlikely to fail. First
 * these calls are very unlikely to fail. In practice, the only reason they might fail is an error in specifying their
 * arguments, and such error should be eliminated during initial debugging. Second we assume that the caller is more
 * interested in knowing if fork() or waitpid() failed than in knowing if these signal manipulation calls failed.
 * For similar reasons, we bracket the signal manipulation calls used at the end of the system() with code to save(8)
 * and restore errno (10), so that if fork() or waitpid() fails, then the caller can determine why.
 * 
 * Error checking is not peformed for signal-related system calls in the child (4)(5). On the one hand, there is no
 * way of reporting such an error (the use of _exit(127) is reserved for reporting an error when execing the shell);
 * on the other hand, such failures don't affect the caller of system() (The parent process), which is separate 
 * from the child process that executes the shell.
 * 
 * On return from fork(), the disposition of SIGINT and SIGQUIT is set to SIG_IGN (the disposition inherited from the parent).
 * However, in the child these signals should be treated as if the caller of system() did a fork() and an exec() instead.
 * A fork() leaves the treatment of leaves the treatment of these signals unchanged in the child, An exec() reset the
 * disposition of these signals to the default and leaves the disposition of other signals unchanged. Thus, if the
 * disposition of SIGINT and SIGQUIT in the caller were something other than SIG_IGN, then the child resets the
 * disposition of these signals to the SIG_DFL. (4)
 * 
 * If the execl() call fails in the child, then we use _exit() to terminate the process (6), rather than exit()
 * in order to avoid flushing of any unwritten data in the child's copy of the stdio buffers.
 * 
 * In the parent we must perform the waitpid() to wait specifically for the child that we created (7). If we used
 * wait(), then we might inadvertently fetch the status of some other child created by the calling program.
 * 
 * Although the implementation of system() does not require the use of signal handlers, the calling program
 * may have established signal handlers, and one of these could interrupt a a blocked call to waitpid(). Thus,
 * we use a loop to restart waitpid() if it fails with EINTR (7); any other error from waitpid() casues this
 * loop to terminate.
 */
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

int system(const char* cmd) {
    sigset_t bMask;                   // Mask for blocking signals.
    sigset_t oMask;                   // Mask for saving original signal mask.
    struct sigaction saIgnore;        // Signal action to ignore signals.
    struct sigaction saOrigQuit;      // Original disposition of SIGQUIT.
    struct sigaction saOrigInt;       // Original disposition of SIGINT.
    struct sigaction saDefault;       // Signal action for SIGCHLD.
    pid_t childPid;                   // PID of child process.

    int status;                       // Status returned by waitpid().
    int savedErrno;                   // Save the value of errno.

    /* (1) */
    if (cmd == NULL) {                // Is a shell available?
        return system(":") == 0;      // Yes, return 0.
    }

    sigemptyset(&bMask);              // Initialize block mask.
    sigaddset(&bMask, SIGCHLD);       // Block SIGCHLD.

    /* (2) */
    sigprocmask(SIG_BLOCK, &bMask, &oMask); // Save oMask, and add bMask to the signal mask, blocking SIGCHLD.

    saIgnore.sa_handler = SIG_IGN;    // Ignore SIGINT and SIGQUIT.
    saIgnore.sa_flags = 0;            // No flags.
    sigemptyset(&saIgnore.sa_mask);   // Blank out the mask.

    /* (3) */
    sigaction(SIGINT, &saIgnore, &saOrigInt);   // Ignore SIGINT.
    sigaction(SIGQUIT, &saIgnore, &saOrigQuit); // Ignore SIGQUIT.

    switch (childPid = fork()) {      // Create a child proc.
        case -1:                      // Error creating child.
            status = -1;              // Return -1.
            break;                    // Break out of the switch.
        case 0:                       // We created a child.
            saDefault.sa_handler = SIG_DFL; // Default signal action.
            saDefault.sa_flags = 0;   // No flags.
            sigemptyset(&saDefault.sa_mask); // Blank out the mask.

            /* (4) */
            if (saOrigInt.sa_handler != SIG_IGN) // If SIGINT was not ignored.
                sigaction(SIGINT, &saDefault, NULL); // Reset the disposition to default.
            if (saOrigQuit.sa_handler != SIG_IGN) // If SIGQUIT was not ignored.
                sigaction(SIGQUIT, &saDefault, NULL); // Reset the disposition to default.

            /* (5) */
            sigprocmask(SIG_SETMASK, &oMask, NULL); // Restore the original signal mask.
            execl("/bin/sh", "sh", "-c", cmd, (char*) NULL); // Execute the cmd.
            _exit(127);               // If execl() fails, exit with 127.
        default:                      // We are in the parent.
            while (waitpid(childPid, &status, 0) == -1) {
                if (errno != EINTR) { // If waitpid() failed for any reason other than EINTR.
                    status = -1;      // Set status to -1.
                    break;            // Break out of the loop.
                }
            }
            break;                    // Break out of the switch.
    }

    // Unblock SIGCHLD, restore disposition of SIGINT and SIGQUIT.
    /* (8) */
    savedErrno = errno;               // Save the value of errno.

    /* (9) */
    sigprocmask(SIG_SETMASK, &oMask, NULL); // Restore the original signal mask.
    sigaction(SIGINT, &saOrigInt, NULL);    // Restore the original disposition of SIGINT.
    sigaction(SIGQUIT, &saOrigQuit, NULL);  // Restore the original disposition of SIGQUIT.

    /* (10) */
    errno = savedErrno;               // Restore the value of errno.

    return status;
}