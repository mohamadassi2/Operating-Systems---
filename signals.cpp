#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

#define WHITESPACE " "
#define NULLJOB -1
#define EMPTYTEXT ""

using namespace std;


void ctrlCHandler(int sig_num) {
    SmallShell &smash = SmallShell::getInstance();
    std::cout << "smash: got ctrl-C" << std::endl;
    smash.smash_jobs_list->removeFinishedJobs();
    if(smash.current_job->jobPID != NULLJOB)
    {
        if(killpg(smash.current_job->jobPID,SIGKILL)==-1){
            std::perror("smash error: kill faild");
            return;
        }
        std::cout<<"smash: process "<<smash.current_job->jobPID<<" was killed"<<std::endl;
        smash.current_job->resetJob();
        smash.smash_jobs_list->removeFinishedJobs();
    }
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

