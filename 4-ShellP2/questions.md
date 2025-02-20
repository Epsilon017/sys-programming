1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  `fork()` creates a new process, allowing the shell to continue running while the command executes in the child process. Without `fork()`, calling `execvp()` would replace the shell itself, preventing it from accepting more commands.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If `fork()` fails, it returns -1, indicating that no child process was created. In this case, my implementation simply skips execution and continues the loop, preventing the shell from crashing.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  `execvp()` searches for the command in directories listed in the PATH environment variable. If the command isn’t found in any of these directories, `execvp()` fails.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  `wait()` ensures the parent process (the shell) waits for the child process to finish, preventing zombie processes. Without it, terminated child processes remain in the system until the parent explicitly collects their exit status.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  `WEXITSTATUS()` extracts the exit code of the child process from the value returned by `wait()`. This is useful for detecting errors or command failures.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  It treats quoted sections as single arguments, ensuring spaces within quotes aren’t treated as delimiters. This is necessary for handling commands that contain spaces, like echo "hello world", correctly.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I improved handling of quoted arguments and removed duplicate spaces. A challenge was ensuring tokens were correctly extracted while keeping the arguments intact. I had to switch to managing the arguments myself through a lot of if statements rather than using `strtok()`, since that command edits the original string.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals provide a way to asynchronously notify processes of events, like termination or interruptions. Unlike other IPC methods, such as pipes and message queues, signals are unique in that they don’t require a direct communication channel and can happen while another process is running without it asking.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL (9): Forcefully terminates a process without cleanup. Used when a process is unresponsive. SIGTERM (15): Requests a process to terminate gracefully, allowing it to clean up resources. SIGINT (2): Our dear friend when things go wrong. Sent when the user presses Ctrl+C, allowing a process to handle interruptions.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP pauses a process but cannot be caught or ignored because it is handled at the kernel level, ensuring processes can always be paused by some means, usually by the system or a user with enough authority.