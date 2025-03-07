1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_The shell stores each child process's PID in an array and then iterates through that array, calling `waitpid()` for each one. This ensures that the parent process waits until every child has terminated before displaying the prompt again. Without the `waitpid()` calls, child processes would become zombies, potentially leading to resource exhaustion and unpredictable behavior._

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_After `dup2()` redirects the necessary file descriptors, closing the unused pipe ends is important to free system resources and signal the file has ended. If the extra ends remain open, processes might hang waiting for input that never ends, and open file descriptors could accumulate, causing resource leaks._

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_The `cd` command must be a built-in because it changes the shell's current working directory, which is a state maintained by the parent process. If `cd` were executed externally, any directory changes would affect the child, leaving the parent shell's directory unchanged and making directory navigation impossible._

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_To allow any number of piped commands, the fixed-size command array would have to be replaced with a dynamically allocated array that grows as needed. The downsides include increased complexity in memory management and error handling, as well as potential performance issues to account for if a very large number of commands are chained together._