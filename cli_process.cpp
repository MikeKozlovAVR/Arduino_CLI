#include "cli_process.h"

CliProcess::CliProcess(){
    memset(this->pids, 0, sizeof(this->pids));
}

int CliProcess::setMultiTasker(MultiTasker *multitasker){
    this->mtasker = multitasker;
}

int CliProcess::regTaskPid(pid_t pid){
    if (pid < 100){
        return -1;
    }
    for (int i=0; i<MAX_PIDS_IN_PROCESS; i++){
        if (this->pids[i] == 0){
            this->pids[i] = pid;
            return 0;
        }
    }
    return -1;
}

int CliProcess::destroyRegisteredTasks(){
    uint8_t process_is_empty_flag = 1;
    for (int i=0; i<MAX_PIDS_IN_PROCESS; i++){
        if (this->pids[i] != 0){
            this->mtasker->destroyTaskAtPid(this->pids[i]);
            process_is_empty_flag = 0;
        }
    }
    if (process_is_empty_flag) { return -1; }
    return 0;
}

CliProcess::~CliProcess(){
    this->destroyRegisteredTasks();
}

int CliProcess::getArgC(){
    return this->argc;
}

char **CliProcess::getArgV(){
    return this->argv;
}

void CliProcess::setArg(int argc, char **argv){
    this->argc = argc;
    this->argv = argv;
}
