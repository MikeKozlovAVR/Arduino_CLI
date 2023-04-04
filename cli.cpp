#include "cli.h"

namespace cli{
static char cli_noti[] = "CLI: ";
static char cli_welcome[] = "Start CLI system";
static char cli_version[] = "v.1.0.1";
static char cli_date[] = "date 31.03.23";
static char slay_procnm[] = "slay";
static char argv_divdr[1] = " ";
static char err_txt_isbig[] = " {ISBIG}";
}

static uint8_t script_input = 0; 

Cli::Cli(){

}

void Cli::init(HardwareSerial *serial, unsigned long baud){
    this->serial = serial;
    this->serial->begin(baud);
    this->out(cli::cli_welcome);
    this->outln();
    this->out(cli::cli_version);
    this->outln();
    this->out(cli::cli_date);
    this->outln();
}

int Cli::input(char in){
    if (!script_input){
        this->out(in);
    }
    this->inbuff.buff[this->inbuff.in_count] = in;
    if (in == '\r'){
        if (!script_input){
            this->out('\n');
        }
        this->inbuff.buff[this->inbuff.in_count] = '\0';
        this->inputProcessing();
        this->clearInputBuffer();
        return 0;
    }
    this->inbuff.in_count++;
    if (this->inbuff.in_count >= CLI_MAX_BUFF_SIZE){
        this->clearInputBuffer();
    }
    return 0;
}

void Cli::cliSerialEvent(){
    while(this->serial->available() > 0){
        this->input(this->serial->read());
    }
}

void Cli::echo(){
    this->out(this->inbuff.buff);
    this->outln();
}

int Cli::inputProcessing(){
    int argc = 0;
    char *argv[CLI_MAX_PROCESS_ARGV];
    char *tmp;
    tmp = strtok(this->inbuff.buff, cli::argv_divdr);
    while (tmp != NULL) {
        if (argc == CLI_MAX_PROCESS_ARGV){
            this->out(cli::cli_noti);
            this->out("argc err");
            this->out(cli::err_txt_isbig);
            this->outln();
            script_input = 0;
            return -1;
        }
        argv[argc] = tmp;
        argc++;
        tmp = strtok(NULL, cli::argv_divdr);
    }
    if (argc == 2 && strcmp(argv[0], cli::slay_procnm) == 0){
            if (this->slayProcess(argv[1]) != 0){
                this->out(cli::cli_noti);
                this->out(cli::slay_procnm);
                this->out(": [");
                this->out(argv[1]);
                this->out("] not running");
                this->outln();
            }
    }
    else {
        this->startProcess(argv[0], argc, argv);
    }
    script_input = 0;
    return 0;
}

int Cli::regProcess(void(*process_f)(CliProcess *process), const char* name, process_type_t type = PROCT_CONTINUOUS){
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (!this->process_units[i].used){
            this->process_units[i].process_f = process_f;
            this->process_units[i].name = name;
            this->process_units[i].type = type;
            this->process_units[i].used = 1;
            this->process_units[i].cli_proc = new CliProcess();
            return 0;
        }
    }
    return -1;
}

int Cli::startProcess(char *proc_name, int argc, char **argv){
    if (!script_input) { this->out(cli::cli_noti); }
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (this->process_units[i].used){
            if (strcmp(this->process_units[i].name, proc_name) == 0){
                if (!this->process_units[i].runned){
                    if (!script_input) {
                        this->out(proc_name);
                        this->out(" run");
                        this->outln();
                    }
                    this->process_units[i].cli_proc->setArg(argc, argv);
                    this->startProcess(&this->process_units[i]);
                    if (this->process_units[i].type == PROCT_SINGLE){
                        this->slayProcess(&this->process_units[i]);
                    }
                } else{
                    this->out("error:[");
                    this->out(proc_name);
                    this->out("] already launched");
                    this->outln();
                }
                return 0;
            }
        }
    }
    this->out("Process [");
    this->out(proc_name);
    this->out("] not found");
    this->outln();
    return -1;
}

void Cli::startProcess(process_unit_t *proc_unit){
    proc_unit->runned = 1;
    proc_unit->process_f(proc_unit->cli_proc);
}

int Cli::slayProcess(char *proc_name){
    for(int i = 0; i<CLI_MAX_PROCESSES; i++){
        if (this->process_units[i].used && this->process_units[i].runned){
            if (strcmp(this->process_units[i].name, proc_name) == 0){
                this->slayProcess(&this->process_units[i]);
                this->out(cli::cli_noti);
                this->out("Process [");
                this->out(proc_name);
                this->out("] destroyed");
                this->outln();
                return 0;
            }
        }
    }
    return -1;
}

void Cli::slayProcess(process_unit_t *proc_unit){
    proc_unit->runned = 0;
    proc_unit->cli_proc->destroyRegisteredTasks();
}

void Cli::clearInputBuffer(){
    memset(this->inbuff.buff, 0, sizeof(this->inbuff.buff));
    this->inbuff.in_count = 0;
}

size_t Cli::out(char* src){
    return this->serial->print(src);
}

size_t Cli::out(char src){
    return this->serial->print(src);
}

size_t Cli::out(int src){
    return this->serial->print(src);
}

size_t Cli::out(float src){
    return this->serial->print(src);
}

size_t Cli::out(float src, uint8_t dec_places){
    return this->serial->print(src, dec_places);
}

void Cli::outln(){
    this->out("\n\r");
}

int Cli::in(char* str, size_t size){
    if (size > CLI_MAX_BUFF_SIZE){
        this->out(cli::cli_noti);
        this->out("cmd err");
        this->out(cli::err_txt_isbig);
        this->outln();
        return -1;
    }
    for (int i = 0; i < size; i++){
        this->input(str[i]);
    }
    return 0;
}

int Cli::script(char* src, size_t size){
    int ptr = 0;
    char* tmp = src;
    this->clearInputBuffer();
    if (size > CLI_MAX_SCRIPT_SIZE){
        this->out(cli::cli_noti);
        this->out("script err");
        this->out(cli::err_txt_isbig);
        this->outln();
        return -1;
    }
    while (ptr != size){
        for (int i = ptr; i < size; i++){
            if (src[i] == '\r'){
                script_input = 1;
                if(this->in(tmp, i - ptr + 1) != 0){
                    return -1;
                }
                ptr = i + 1;
                tmp = src + ptr;
                break;
            }
            else if (src[i] == '\0'){
                return 0;
            }
        }
    }
    return 0;
}

