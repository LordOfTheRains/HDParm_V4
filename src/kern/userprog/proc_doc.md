**current implementation status**
for process related syscalls
- can't compile everything yet due to syntax and dereferencing errors;

* int sys_getpid; //done
* int sys_fork//done, untested
* int sys_execv//not started
* int sys_waitpid//not started
* int sys_exit//done, untested
**High level design**
so the overview is basiacally i need a mechanism to manage all the pids and threads can be managed using these pid information. for pid management, the major component is synchronization, since pids are recycled i would have to carefully manage the state the pids and lock them accordingly to avoid crazy things from happening when multple process try to change or read the state.  the current pid generation mechanism does not scale well but it was the easiest solution that came to my mind. pid status are the most improtant thing since thats what threads use to not only manage synchronization but also prevent deadlock (parent wait on child and child wait on parent...) by putting restriction on these waits based on the parent child relationship betweent the processes. 

side note: having my pdi management file separte form the sysystem calls definitely helepd with code organization, i was trying to go modular so the code base ended up to be lot bigger than i had expected and some functionalities are not even included....


**Detail Design**

## pid management system ##
I created a pid_factory file to organize all the pid related functions.
each pid is assigned a pidinfo that keeps track of pid status, for example, the
exit_status and exited properties are used to track if the pid can be reclaimed
and let whoever is waiting on this pid know about the exit status. each pid also
keeps track of parent pid so parent can be notify if the child pid exited or
whatever else happens.

the pid generation system is not fully impleemnted, the basic idea here is to
have a table of pids and each entry is a pidinfo. get_available_pid should go throught he list and find out whats exited and return that so the requesting pid can be granted. get_pidinfo can be called to check the status of each pid.

there are multiple locks in the file to lock all the shared resources such the total pid count and the pidinfo table. the purpose of this is to provide new pid for new process and lets process /parent process access and management necessary pids. majority of the funcitons are implemented but not tested or able to compile due to other errors in other files. but the logic should be clearn the funciton body.

## sys_fork ##

the design for sysfork is basically copy necessary info from calling process and make a copy child process. the main mechanism to create a new process is using the thread_fork fucntion, i was originally going to use md_forkentry then i realize the arguiements would nto let me pass this fucntion directly into thread_fork, but i didnt want to modify trapframe.h. so i made a new function called goto_new_process that conforms to the function signature of thread_fork and takes the new thread trapframe/addrspace to and calls mips_usermode to start the function on a child process. then at the end of it i set the retval to pid so parent can have childs pid and returs 0 to the child indicating fork success. again, it is fully implemented but not tested.


## sys_execv ##
honestly i have no idea how to approach this at all...
## sys_waitpid ##

so the strategy here is basically check the exited value and the exit_status of the pid info of the wpid and return accordingly. im panicing the thread if the pid doesnt exist since im not sure what error code should be returned.

the man page is confusing for this one because it says return the exit status of the wpid if it has already exited then later in the page it says to return the pid of the the process matching the status arguement.... anways i went with the first approach where if the pid they requested is exited, i return the exit status. else -1. i

## sys_exit ##

so this fucntion basically exit the thread, im utilizing the thread_exit functiont to exit the process, but since i have pid info i had change status on the thread so that the pid can be recycle and whoever is waiting on the pid can get the  correct status codes and do necessary things.
