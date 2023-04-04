#ifndef CLI_PROCESS_H_
#define CLI_PROCESS_H_

#include <Arduino.h>
#include <multitasker.h>

#define MAX_PIDS_IN_PROCESS 10

class CliProcess{
public:
    CliProcess();
    ~CliProcess();
    //Register a task PID in a process so that when the process terminates, the registered tasks are destroyed by their PID
    int regTaskPid(pid_t pid);
    //Set a MultiTasker that services tasks in progress
    int setMultiTasker(MultiTasker *multitasker);
    //Destroy all registered tasks
    int destroyRegisteredTasks();
    //Get process argc
    int getArgC();
    //Get process argv
    char **getArgV();
    //Set process arguments
    void setArg(int argc, char **argv);
private:
    
public:
    
private:
    pid_t pids[MAX_PIDS_IN_PROCESS];
    MultiTasker *mtasker;
    int argc;
    char **argv;
};


#endif //CLI_PROCESS_H_
