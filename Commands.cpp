#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <bits/stdc++.h>
#include <fcntl.h>
#include "Commands.h"
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sched.h>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/syscall.h>


using namespace std;

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

#define WHITESPACE " "
#define NULLJOB -1
#define EMPTYTEXT ""

string _ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
  return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell() {
// TODO: add your implementation
  this->prompt = "smash";
  this->last_path_dirrectory = "";
  this->smash_jobs_list = new JobsList();
  this->current_job = new JobsList::JobEntry(NULLJOB , NULLJOB , EMPTYTEXT , FOREGROUND);
  this->smash_pid= getpid();

  this->is_alias = false;

}


SmallShell::~SmallShell() {
// TODO: add your implementation
    delete this->smash_jobs_list;
    delete this->current_job;
}

void JobsList::JobEntry::resetJob() {
    this->jobPID = NULLJOB;
    this->jobID = NULLJOB;
    this->commandLine = EMPTYTEXT;
}

void JobsList::JobEntry::updateValues(int jobID, pid_t jobPID, std::string commandLine, Mode mode, time_t start_time) {
    this->jobID = jobID;
    this->jobPID = jobPID;
    this->commandLine = commandLine;
    this->mode = mode;
    this->start_time = start_time;
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
    //std::cout<<"c1"<<endl;
    smash_jobs_list->removeFinishedJobs();
    //std::cout<<"c22"<<endl;

    char* comand[COMMAND_MAX_ARGS+1];

    int parameters_num = _parseCommandLine(cmd_line,comand);
    if (parameters_num==0){
        return nullptr;
    }

   // std::cout<<"c2"<<endl;
    if(string (cmd_line).find("|&")!=string::npos){
        if(strcmp(comand[0],"alias")==0 || strcmp(comand[0],"unalias")==0){
            size_t pos1=string (cmd_line).find("|&");
            string new_str1=string(cmd_line).substr(0,pos1);
            if(std::count(new_str1.begin(),new_str1.end(),'\'')%2 != 0){
                if (strcmp(comand[0],"alias")==0){
                    return new aliasCommand(cmd_line);
                }
                if (strcmp(comand[0],"unalias")==0){
                    return new unaliasCommand(cmd_line);
                }
            }
        }
        return new PipeCommand(cmd_line, true);
    }
    if(string (cmd_line).find("|")!=string::npos){
        if(strcmp(comand[0],"alias")==0 || strcmp(comand[0],"unalias")==0){
            size_t pos3=string (cmd_line).find("|");
            string new_str3=string(cmd_line).substr(0,pos3);
            if(std::count(new_str3.begin(),new_str3.end(),'\'')%2 != 0){
                if (strcmp(comand[0],"alias")==0){
                    return new aliasCommand(cmd_line);
                }
                if (strcmp(comand[0],"unalias")==0){
                    return new unaliasCommand(cmd_line);
                }
            }
        }
        return new PipeCommand(cmd_line, false);
    }

    if(string (cmd_line).find(">")!=string::npos){
        if(strcmp(comand[0],"alias")==0 || strcmp(comand[0],"unalias")==0){
            size_t pos2=string (cmd_line).find(">");
            string new_str2=string(cmd_line).substr(0,pos2);
            if(std::count(new_str2.begin(),new_str2.end(),'\'')%2 != 0){
                if (strcmp(comand[0],"alias")==0){
                    return new aliasCommand(cmd_line);
                }
                if (strcmp(comand[0],"unalias")==0){
                    return new unaliasCommand(cmd_line);
                }
            }
        }
        return new RedirectionCommand(cmd_line);
    }
    /*
    if (_parseCommandLine(cmd_line,comand)==0){
        return nullptr;
    }*/



    /////////////////////////updates for the alias command ////////////////////
    //check if comand[0] exist in the alias vector, notice that alias vector is string and comand is char**
    //if it exists, switch the smasch.is ilias to true, add it to the jobs list by its format, and bulid new command
    //dont forget to return the smash is ilias to false
    bool is_alias_command=false;
    for(size_t i=0;i<alias_keys_vector.size();i++){
        if(strcmp(comand[0],alias_keys_vector[i].c_str())==0){
            is_alias_command= true;
            break;
        }
    }

    if(is_alias_command){
        string new_command = alias_map[string(comand[0])];
        for(int i=1; i<parameters_num;i++){
            new_command+=" ";
            new_command+=comand[i];
        }
        if(_isBackgroundComamnd(new_command.c_str())){

        }
        Command* temp_command = CreateCommand(new_command.c_str());
        temp_command->come_from_alias= true;
        temp_command->alias_line=string(cmd_line);
        return temp_command;

        //is_alias=true;l
        //need to convert the alias to data in the map and to add to it the rest of the command
        //need to add it to the job list by the format provided by the user
        //change the other places that we add to the job list for not adding
        //return the is_alias to false at the end
    }

    if (strcmp(comand[0],"chprompt")==0) {
        return new ChPromptCommand(cmd_line);
    }
    if (strcmp(comand[0],"showpid")==0||strcmp(comand[0],"showpid&")==0) {
        return new ShowPidCommand(cmd_line);
    }
   // std::cout<<"c3"<<endl;
    if (strcmp(comand[0],"pwd")==0) {
     // cout<<endl<<"pwd command"<<endl;
        return new PWDCommand(cmd_line);
    }
    if (strcmp(comand[0],"cd")==0) {
        return new ChangeDirCommand(cmd_line);
    }
    if (strcmp(comand[0],"jobs")==0) {
        return new JobsCommand(cmd_line);
    }
    if (strcmp(comand[0],"kill")==0) {
        return new KillCommand(cmd_line);
    }
    if (strcmp(comand[0],"fg")==0) {
        return new ForegroundCommand(cmd_line);
    }

    if (strcmp(comand[0],"alias")==0){
        return new aliasCommand(cmd_line);
    }
    if (strcmp(comand[0],"unalias")==0){
        return new unaliasCommand(cmd_line);
    }

    if (strcmp(comand[0],"quit")==0) {
        return new QuitCommand(cmd_line);
    }
    if (strcmp(comand[0],"getuser")==0) {
        return new GetUserCommand(cmd_line);
    }
    if (strcmp(comand[0],"listdir")==0) {
        return new ListDirCommand(cmd_line);
    }
    return new ExternalCommand(cmd_line);
}

void SmallShell::executeCommand(const char *cmd_line) {
  //std::cout<<"ex1"<<endl;
  string temp_command = string(cmd_line);
  //std::cout<<"ex2"<<endl;
  string command = _rtrim(temp_command);
  //std::cout<<"ex3"<<endl;
  if(command != EMPTYTEXT) {
    //std::cout<<"ex4"<<endl;
      Command *cmd = CreateCommand(cmd_line);
      //std::cout<<"ex5"<<endl;
      cmd->execute();
  }
}

//////////////////////////////////////////////Jobs List Implementation/////////////////////////////////////////////

bool _is_built_in_command(string cmd){
    if(cmd.find("chprompt")==string::npos&&cmd.find("fg")==string::npos&&cmd.find("jobs")==string::npos&&cmd.find("quit")==string::npos&&
       cmd.find("kill")==string::npos&&cmd.find("cd")==string::npos&&cmd.find("pwd")==string::npos&&cmd.find("showpid")==string::npos){
        return false;
    }
    return true;
}

int JobsList::maxJobID() {
    if(this->jobs_list.empty()){
        return 0;
    }
    return this->jobs_list.back().jobID;
}

//update the time for the job
void JobsList::addJob(Command* cmd,pid_t jobPID,bool isStopped){
    time_t starting_time;
    if(cmd == NULL){
        return;
    }
    this->removeFinishedJobs();
    Mode new_job_mode;
    if (isStopped){
        new_job_mode = STOPPED;
    }else{
        new_job_mode = BACKGROUND;
    }
    time(&starting_time);
    if(cmd->come_from_alias){
        JobEntry new_job = JobEntry(this->maxJobID()+1 ,jobPID , cmd->alias_line , new_job_mode , starting_time);
        this->jobs_list.push_back(new_job);
        return;
    }
    JobEntry new_job = JobEntry(this->maxJobID()+1 ,jobPID , cmd->commandLine , new_job_mode , starting_time);
    this->jobs_list.push_back(new_job);
    //this->maxJobID++;
}

void JobsList::stopJob(pid_t jobPID, int jobID, std::string commandLine, time_t strat_time) {
    this->removeFinishedJobs();
    for (vector<JobEntry>::iterator it = jobs_list.begin() ; it != jobs_list.end() ; it++) {
        if((*it).jobPID == jobPID){//means that thejob had entered the list before
            (*it).mode = STOPPED;
            return;
        }
    }
    JobEntry new_job = JobEntry(jobID ,jobPID, commandLine , STOPPED , strat_time);
    this->jobs_list.push_back(new_job);
}


////add the time to the jobentry and print
void JobsList::printJobsList() {
    this->removeFinishedJobs();
    time_t starting_time;
    time_t current_time;
    for (size_t i=0 ; i< this->jobs_list.size() ; i++) {
        if(jobs_list[i].mode != FOREGROUND){
            starting_time = jobs_list[i].start_time;
            time(&current_time);
            if(this->jobs_list[i].mode == STOPPED){
                cout<<"["<<jobs_list[i].jobID<<"] "<<jobs_list[i].commandLine<<" : "<<jobs_list[i].jobPID<<" "<<difftime(current_time,starting_time)<<" secs (stopped)" << std::endl;
            }else{
                //cout<<"["<<jobs_list[i].jobID<<"] "<<jobs_list[i].commandLine<<" : "<<jobs_list[i].jobPID<<" "<<difftime(current_time,starting_time)<<" secs" << std::endl;
                cout<<"["<<jobs_list[i].jobID<<"] "<<jobs_list[i].commandLine << std::endl;
            }
        }
    }
    this->removeFinishedJobs();
}



void JobsList::removeFinishedJobs() {
    for (vector<JobEntry>::iterator it = jobs_list.begin() ; it != jobs_list.end() ; it++) {
      //cout<<"r2"<<endl;
        if(waitpid((*it).jobPID, NULL,WNOHANG)<0){
            jobs_list.erase(it);
            if(jobs_list.empty())
            {
              break;
            }
            it = jobs_list.begin();
        }
    }

}

void JobsList::removeJobById(int jobId) {
    this->removeFinishedJobs();
    for (vector<JobEntry>::iterator it = jobs_list.begin() ; it != jobs_list.end() ; it++){
        if((*it).jobID == jobId){
            jobs_list.erase(it);
            break;
        }
    }
}

JobsList::JobEntry *JobsList::getJobById(int jobId) {
    this->removeFinishedJobs();
    for (vector<JobEntry>::iterator it = jobs_list.begin() ; it != jobs_list.end() ; it++){
        if((*it).jobID == jobId){
            return &(*it);
        }
    }
    ////return null if you dont find job with similar id
    return nullptr;
}

JobsList::JobEntry *JobsList::getLastJob(int* lastJobId)
{
  JobEntry* job;
  int max_job_id=0;
  if(jobs_list.empty())
  {
    *lastJobId=max_job_id;
    return nullptr;
  }
  for (size_t i = 0; i < jobs_list.size(); i++)
  {
    if(jobs_list[i].jobID >max_job_id)
    {
      max_job_id=jobs_list[i].jobID;
      job=&jobs_list[i];
    }
  }
  *lastJobId=max_job_id;
  return job;

}

JobsList::JobEntry *JobsList::getLastStoppedJob(int *jobId)
{
  JobEntry* job;
  int max_stopped_job_id=0;
  if(jobs_list.empty())
  {
    *jobId=max_stopped_job_id;
    return nullptr;
  }
  for (size_t i = 0; i < jobs_list.size(); i++)
  {
    if(jobs_list[i].jobID >max_stopped_job_id && jobs_list[i].mode==STOPPED)
    {
      max_stopped_job_id=jobs_list[i].jobID;
      job=&jobs_list[i];
    }
  }
  *jobId=max_stopped_job_id;
  return job;

}

void JobsList::removeAllJobs()
{
  this->removeFinishedJobs();
  std::cout<<"smash: sending SIGKILL signal to "<< this->jobs_list.size() << " jobs:"<<std::endl;
  for (vector<JobEntry>::iterator it = jobs_list.begin() ; it != jobs_list.end() ; it++) {
      cout << (*it).jobPID << ": " << (*it).commandLine << endl;
      if (kill((*it).jobPID, SIGKILL) == -1) {
          std::perror("smash error: kill failed");
          return;
      }
  }
}



/////////////////////////constructors//////////////////////////////////////

BuiltInCommand::BuiltInCommand(const char *cmd_line): Command(cmd_line){}
/*Command::Command(const char *cmd_line){
    this->line=std::string (cmd_line);
}*/


ChPromptCommand::ChPromptCommand(const char *cmd_line):BuiltInCommand(cmd_line) {}
ShowPidCommand::ShowPidCommand(const char *cmd_line):BuiltInCommand(cmd_line)  {}
PWDCommand::PWDCommand(const char *cmd_line):BuiltInCommand(cmd_line) {}
ChangeDirCommand::ChangeDirCommand(const char *cmd_line):BuiltInCommand(cmd_line)  {}
JobsCommand::JobsCommand(const char *cmd_line):BuiltInCommand(cmd_line)  {}
KillCommand::KillCommand(const char* cmd_line):BuiltInCommand(cmd_line) {}
ForegroundCommand::ForegroundCommand(const char *cmd_line):BuiltInCommand(cmd_line)  {}
QuitCommand::QuitCommand(const char *cmd_line):BuiltInCommand(cmd_line)  {}
PipeCommand::PipeCommand(const char* cmd_line , bool bg): Command(cmd_line), is_bg(bg){}
RedirectionCommand::RedirectionCommand(const char *cmd_line): Command(cmd_line){}
aliasCommand::aliasCommand(const char *cmd_line): BuiltInCommand(cmd_line) {}
unaliasCommand::unaliasCommand(const char *cmd_line): BuiltInCommand(cmd_line) {}
ExternalCommand::ExternalCommand(const char *cmd_line): Command(cmd_line) {}
ListDirCommand::ListDirCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
GetUserCommand::GetUserCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}



/////////////////////////////////////Commands Execute Implementation////////////////////////////////////////////////////////

void ChPromptCommand::execute() {
    SmallShell& smash=SmallShell::getInstance();
    char * parameters[COMMAND_MAX_ARGS+1];
    if(_parseCommandLine(commandLine.c_str(),parameters)!=1){
        smash.prompt=parameters[1];
    }
    else{

        smash.prompt="smash";
    }
}

void ShowPidCommand::execute() {
    SmallShell& smash=SmallShell::getInstance();
    //cout<<"smash pid:   "<<smash.current_job->jobPID<<endl;
    cout<<"smash pid is "<<smash.smash_pid<< std::endl;
}

void PWDCommand::execute() {
    char pathname[1000]; // check what is the max long of the path
    if(getcwd(pathname , 1000) == NULL){
        perror("smash error: getcwd failed");
    }
    cout<<pathname<<endl;
}

void ChangeDirCommand::execute() {
    SmallShell& smash=SmallShell::getInstance();
    char * parameters[COMMAND_MAX_ARGS+1];
    char pathname[1000]; // check what is the max long of the path
    int parameters_num = _parseCommandLine(commandLine.c_str(),parameters) - 1;
    if(parameters_num > 1){
        cerr<<"smash error: cd: too many arguments" << std::endl;
        return;
    }
    if(strcmp(parameters[1] ,"-")!=0){//check if can compare a char* in this sentax
        getcwd(pathname , 1000);
        if (chdir(parameters[1]) == -1){
            std::perror("smash error: chdir failed");
            return;
        }else{
            string last_path(pathname);
            smash.last_path_dirrectory= last_path;
        }
    }else{

        if(smash.last_path_dirrectory.compare("") == 0){//if doesn't work , change it to .empty function
            cerr << "smash error: cd: OLDPWD not set" << std::endl;
            return;
        } else{
            getcwd(pathname , 1000);
            if (chdir(smash.last_path_dirrectory.c_str()) == -1){
                std::perror("smash error: chdir failed");
            }else{
                string last_path(pathname);
                smash.last_path_dirrectory= last_path;
            }
        }
    }

}

void JobsCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    smash.smash_jobs_list->removeFinishedJobs();
    smash.smash_jobs_list->printJobsList();
    smash.smash_jobs_list->removeFinishedJobs();
}

void QuitCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    char * parameters[COMMAND_MAX_ARGS+1];
    string kill_str = "kill";
    //string parm1_str(parameters[1]);
    char* parm1=parameters[1];
    int parameters_num = _parseCommandLine(commandLine.c_str(),parameters) - 1;
    if(parameters_num >0){
        parm1=parameters[1];
    }
    if(parameters_num >= 1 && strcmp(parm1, kill_str.c_str()) == 0){ // check the condition
        smash.smash_jobs_list->removeAllJobs();
    }
    exit(0);
}

bool isComplexCommand(string command_line){
    if(command_line.find("?")==string::npos && command_line.find("*")==string::npos){
        return false;
    }
    return true;
}


void ExternalCommand::execute() {
    SmallShell &smash = SmallShell::getInstance();
    char * not_complex_parameters[COMMAND_MAX_ARGS+1];


    char newNotComplexCommandLine[COMMAND_MAX_LENGTH+1];
    strcpy(newNotComplexCommandLine, commandLine.c_str());
    if(!isComplexCommand(commandLine)){
        if(_isBackgroundComamnd(commandLine.c_str())){
            _removeBackgroundSign(newNotComplexCommandLine);
        }
        _parseCommandLine(newNotComplexCommandLine,not_complex_parameters);
    }

    pid_t returned_pid;
    char a1[]="/bin/bash";
    char a2[]="-c";
    char newCommandLine[COMMAND_MAX_LENGTH+1];
    strcpy(newCommandLine, commandLine.c_str());
    char *argv[] = {a1,a2 , newCommandLine, NULL};
    smash.smash_jobs_list->removeFinishedJobs();
    if(_isBackgroundComamnd(commandLine.c_str())){// means that this is a background job ,, check the condition
        returned_pid = fork();
        if(returned_pid == -1){
            perror("smash error: fork failed");
        }
        setpgrp();
        if(returned_pid == 0){// this is the son(the job we should execute
            if(isComplexCommand(commandLine)){

                _removeBackgroundSign(newCommandLine);

                if(execv(argv[0], argv) == -1){
                    perror("smash error: execv failed");
                    smash.smash_jobs_list->removeFinishedJobs();
                    exit(0);
                }
            }else{
                if(execvp(not_complex_parameters[0], not_complex_parameters) == -1){
                    perror("smash error: execvp failed");
                    smash.smash_jobs_list->removeFinishedJobs();
                    exit(0);
                }
            }


        }
        else{//here this is the smash ,, we should update the forground fields in smash without wait
            Command* cmd =smash.CreateCommand(commandLine.c_str());
            cmd->alias_line=alias_line;
            cmd->come_from_alias=come_from_alias;
            //if its came from alias command don't add itl
            smash.smash_jobs_list->addJob(cmd,returned_pid, false);

            smash.current_job->resetJob();

        }

    } else{ // this is not a background job so we should wait in the smash
        returned_pid = fork();
        if(returned_pid == -1){
            perror("smash error: fork failed");
        }
        setpgrp();
        if(returned_pid == 0){// this is the son(the job we should execute
            if(isComplexCommand(commandLine)){

                if(execv(argv[0], argv) == -1){
                    perror("smash error: execv failed");
                    smash.smash_jobs_list->removeFinishedJobs();
                    exit(0);
                }
            }else{
                if(execvp(not_complex_parameters[0], not_complex_parameters) == -1){
                    perror("smash error: execvp failed");
                    smash.smash_jobs_list->removeFinishedJobs();
                    exit(0);
                }
            }
        } else{
            time_t starting_time;
            time(&starting_time);
            smash.current_job->updateValues(smash.smash_jobs_list->maxJobID()+1 , returned_pid , commandLine , FOREGROUND , starting_time);
            //smash.smash_jobs_list->maxJobID++;
            waitpid(returned_pid , NULL, WUNTRACED);
            smash.current_job->resetJob();

        }
    }
    smash.smash_jobs_list->removeFinishedJobs();
}


bool isNum (char* str)
{
  int i=0;
  if(str[i]== '-')
  {
    i=1;
  }
  while (str[i]!='\0')
  {
    if(str[i] <'0' || str[i] >'9')
    {
      return false;
    }
    i++;
  }
  return true;
}


//remove finished job
void  KillCommand::execute()
{
  SmallShell& smash=SmallShell::getInstance();
  char * parameters[COMMAND_MAX_ARGS+1];
  int parameters_num = _parseCommandLine(commandLine.c_str(),parameters) - 1;
  int job_id;
  JobsList::JobEntry* job;
    if(parameters_num !=2)
    {
        cerr<<"smash error: kill: invalid arguments"<< std::endl;
        return;
    }

    if(parameters[1][0]!= '-' || !isNum(parameters[1]++) ||!isNum(parameters[2]))
    {
        cerr<<"smash error: kill: invalid arguments"<< std::endl;
        return;
    }

    if(parameters_num >=2 && isNum(parameters[2]))
    {
        job_id= atoi(parameters[2]);
        if(job_id<0){
            cerr<<"smash error: kill: invalid arguments"<< std::endl;
            return;
        }
        smash.smash_jobs_list->removeFinishedJobs();
        job = smash.smash_jobs_list->getJobById(job_id);
        if(!job)
        {
            cerr<<"smash error: kill: job-id "<<job_id<<" does not exist"<<endl;
            smash.smash_jobs_list->removeFinishedJobs();
            return;
        }
    }


  char* ch_signum=parameters[1]++;
  int signum=atoi(ch_signum);


  job_id= atoi(parameters[2]);
  smash.smash_jobs_list->removeFinishedJobs();
  job = smash.smash_jobs_list->getJobById(job_id);
  if(!job)
  {
    cerr<<"smash error: kill: job-id "<<job_id<<" does not exist"<<endl;
    smash.smash_jobs_list->removeFinishedJobs();
    return;
  }

  //cout<<"signal number "<<signum<<" was sent to pid "<<job->jobPID<<endl;

  if(kill(job->jobPID,signum)==-1)
  {
      cout<<"signal number "<<signum<<" was sent to pid "<<job->jobPID<<endl;
   // cout<<"smash error: kill failed"<<endl;//?
      perror("smash error: kill failed");
      return;
  }
  cout<<"signal number "<<signum<<" was sent to pid "<<job->jobPID<<endl;
  /*if (signum==SIGSTOP)
  {
    job->mode=STOPPED;
  }*/

  if(signum==SIGCONT)
  {
    job->mode=BACKGROUND;
  }
  smash.smash_jobs_list->removeFinishedJobs();
}

//remove finished jobs
void  ForegroundCommand::execute()
{
  SmallShell& smash=SmallShell::getInstance();
  smash.smash_jobs_list->removeFinishedJobs();
  char * parameters[COMMAND_MAX_ARGS+1];
  int parameters_num = _parseCommandLine(commandLine.c_str(),parameters) - 1;

  int job_id;
  if(parameters_num<0 ||parameters_num >1)
  {
    cerr<<"smash error: fg: invalid arguments"<<endl;
    return;
  }
  
  if(parameters_num ==1)
  {
    if(!isNum(parameters[1]) || atoi(parameters[1])<0)
    {
      cerr<<"smash error: fg: invalid arguments"<<endl;
      smash.smash_jobs_list->removeFinishedJobs();
      return;
    }
    job_id=atoi(parameters[1]);
    JobsList::JobEntry* job = smash.smash_jobs_list->getJobById(job_id);
    if(!job)
    {
      cerr<<"smash error: fg: job-id "<<job_id<<" does not exist"<<endl;
      smash.smash_jobs_list->removeFinishedJobs();
      return;
    }
    else
    {
      if(job->mode == STOPPED)
      {
        if(kill(job->jobPID, SIGCONT)==-1)
        {
         // cerr<<"smash error: kill failed"<<endl;//?
          perror("smash error: kill failed");
          smash.smash_jobs_list->removeFinishedJobs();
          return;
        }
        cout<<job->commandLine<<" "<<job->jobPID<<endl;
        smash.current_job->updateValues(job->jobID , job->jobPID , job->commandLine , FOREGROUND , job->start_time);
        waitpid(job->jobPID,nullptr, WUNTRACED);//?
      }
      else
      {
        cout<<job->commandLine<<" "<<job->jobPID<<endl;
        smash.current_job->updateValues(job->jobID , job->jobPID , job->commandLine , FOREGROUND , job->start_time);
        waitpid(job->jobPID,nullptr, WUNTRACED);//?
      }
      //?
    }
  }
  else
  {
    smash.smash_jobs_list->removeFinishedJobs();
    if(smash.smash_jobs_list->jobs_list.empty())
    {
      cerr<<"smash error: fg: jobs list is empty"<<endl;
      smash.smash_jobs_list->removeFinishedJobs();
      return;
    }

    JobsList::JobEntry* job =smash.smash_jobs_list->getLastJob(&job_id);
    if(job->mode == STOPPED)
    {
      if(kill(job->jobPID, SIGCONT)==-1)
      {
        //cout<<"smash error: kill failed"<<endl;//perror?
        perror("smash error: kill failed");
        smash.smash_jobs_list->removeFinishedJobs();
        return;
      }

      cout<<job->commandLine<<" "<<job->jobPID<<endl;
      smash.current_job->updateValues(job->jobID , job->jobPID , job->commandLine , FOREGROUND , job->start_time);
      waitpid(job->jobPID,nullptr, WUNTRACED);//?
    }
    else
    {
      cout<<job->commandLine<<" "<<job->jobPID<<endl;
      smash.current_job->updateValues(job->jobID , job->jobPID , job->commandLine , FOREGROUND , job->start_time);
      waitpid(job->jobPID,nullptr, WUNTRACED);//?
    }
  }
  smash.smash_jobs_list->removeFinishedJobs();//?
}






void RedirectionCommand::execute() {

    SmallShell& smash=SmallShell::getInstance();
    if(_isBackgroundComamnd(commandLine.c_str())) {
        size_t last = commandLine.find_last_not_of(WHITESPACE);
        commandLine=commandLine.substr(0,last+1);
        commandLine=commandLine.substr(0,commandLine.length()-1);
    }
    std::size_t findReSymbol = commandLine.find_first_of(">");//"<"
    //pid_t returned_pid;
    if(findReSymbol == std::string::npos){
        return;
    }
    bool doubleBrackets = (commandLine.find(">>") != string::npos); // check
    std::size_t start = commandLine.find_first_not_of(WHITESPACE);
    std::string temp_comandline = commandLine.substr(start , findReSymbol-start);
    string temp_filename , filename;
    if(doubleBrackets){
        temp_filename = commandLine.substr(findReSymbol+2);
        filename = _trim(temp_filename);
    }else{
        temp_filename = commandLine.substr(findReSymbol+1);
        filename = _trim(temp_filename);
    }


    int newFile , screenfd ;
    if(doubleBrackets){
        newFile = open(filename.c_str(), O_RDWR | O_APPEND | O_CREAT, 0666);
        if(newFile == -1){
            perror("smash error: open failed");
            return;
        }
    }else{
        newFile = open(filename.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0666);
        if(newFile == -1){
            perror("smash error: open failed");
            return;
        }
    }
    screenfd = dup(1);
    dup2(newFile, 1);
    smash.CreateCommand(temp_comandline.c_str())->execute();
    dup2(screenfd, 1);
    close(screenfd);
    close(newFile);

}



bool isNumeric(string str)
{
  for (unsigned int i = 0; i < str.length(); i++)
  {
    if(isdigit(str[i]) == false)
      return false; //when one non numeric value is found, return false
  }
  return true;
}


bool checkReserveKeyWord(std::string name){
    /////////////complete the function ////////////////
    if(_is_built_in_command(name) || name.find("listdir")!=string::npos || name.find("getuser")!=string::npos){
        return true;
    }
    return false;
}

bool hasCorrectAliasSyntax(std::string command_line){
    std::regex pattern("^alias [a-zA-Z0-9_]+='[^']*'$");
    if (std::regex_match(command_line, pattern)) {
        return true;
    }
    return false;


    /*if(str[0]!='\'' || str[str.length()-1] !='\''){
        return false;
    }
    std::size_t pos = str.find('=');
    if(pos == std::string::npos){
        return false;
    }
    std::string name = str.substr(0, pos);
    std::string other_side = str.substr(pos+1);*/

}

//return the rest of the command after erasing the whitespaces and the word alias
string getTheRest(string str){
    size_t first=str.find_first_not_of(WHITESPACE);
    string new_string = str.substr(first);
    size_t  last = new_string.find_last_not_of(WHITESPACE);
    new_string = new_string.substr(0,last+1);
    new_string = new_string.substr(6);
    first=new_string.find_first_not_of(WHITESPACE);
    new_string = new_string.substr(first);
    return new_string;
}



void aliasCommand::execute() {
    SmallShell& smash=SmallShell::getInstance();
    char * parameters[COMMAND_MAX_ARGS+1];
    int parameters_num = _parseCommandLine(commandLine.c_str(),parameters) - 1;
    if(parameters_num == 0){
        ////////////////////////////////////////////////////////////////////print alias
        for(size_t  i=0; i<smash.alias_keys_vector.size();i++){
            cout<<smash.alias_keys_vector[i]<<"='"<<smash.alias_map[smash.alias_keys_vector[i]]<<"'"<<endl;
        }
    }else{
        size_t last = commandLine.find_last_not_of(WHITESPACE);
        commandLine=commandLine.substr(0,last+1);
        if(_isBackgroundComamnd(commandLine.c_str())) {
            commandLine=commandLine.substr(0,commandLine.length()-1);
        }
        last = commandLine.find_last_not_of(WHITESPACE);
        commandLine=commandLine.substr(0,last+1);
        if(hasCorrectAliasSyntax(commandLine) == false){
            cerr<<"smash error: alias: invalid alias format"<<std::endl;
            return;
        }
        std::string command_without_alias = getTheRest(commandLine);
        size_t pos=command_without_alias.find('='); //there is no option for not finding '=' because we already checked it
        if(pos == std::string::npos){
            cerr<<"smash error: alias: invalid alias format"<<std::endl;
            return;
        }
        std::string name = command_without_alias.substr(0, pos);
        std::string other_side = command_without_alias.substr(pos+1);
        //cout<<"other side before "<< other_side << endl;
        other_side=other_side.substr(1,other_side.length()-2);
        //cout<<"other side after "<< other_side << endl;
        if(checkReserveKeyWord(name) == true){
            cerr<<"smash error: alias: "<< name <<" already exists or is a reserved command"<<std::endl;
            return;
        }
        //check if the name exist in the keys_vector
        for(size_t  i=0; i<smash.alias_keys_vector.size();i++){
            if(smash.alias_keys_vector[i] == name){
                cerr<<"smash error: alias: "<< name <<" already exists or is a reserved command"<<std::endl;
                return;
            }
        }
        //here it is legal syntax and name so we should add it to the vector and to map
        smash.alias_keys_vector.push_back(name);
        smash.alias_map[name]=other_side;
    }
    //check if the syntax is correct
    //if there is no '=', then print the vector after every key print '=' and the alias_map[key]
    //else
    //get the second argument and cut it to two strings before and after the '='
    //check if the name is a reserved keyword (built in or special)
    //check if the name exist in the alias_keys_vector
    //then insert the name to the alias_keys_vector
    //insert [name,after '='] to alias map

}

int getAliasPos(vector<string> vec,string alias){
    for(size_t  i=0; i<vec.size();i++){
        if(vec[i]==alias){
            return i;
        }
    }
    return -1;
}

void unaliasCommand::execute() {
    SmallShell& smash=SmallShell::getInstance();
    char * parameters[COMMAND_MAX_ARGS+1];
    int parameters_num = _parseCommandLine(commandLine.c_str(),parameters) - 1;
    if(parameters_num == 0){
        cerr<<"smash error: unalias: not enough arguments"<<endl;
        return;
    }
    string parameter_as_string="";
    int alias_pos ;
    for(int i=0; i<parameters_num;i++){
        parameter_as_string = string(parameters[i+1]);
        alias_pos = getAliasPos(smash.alias_keys_vector,parameter_as_string);
        if(alias_pos == -1){
            cerr<<"smash error: unalias: "<<parameter_as_string<<" alias does not exist"<<endl;
            return;
        }
        vector<string>::iterator it= smash.alias_keys_vector.begin();
        it+=alias_pos;
        smash.alias_keys_vector.erase(it);
        smash.alias_map.erase(parameter_as_string);

    }

}
    //if there is no argument then print error
    //else, scan the arguments, for every argument, check if it is exist in the alias_keys_vector,
    //if not print error
    //else delete it from the vector and from the map


string GetCommand1(std::string commandLine)
{
  size_t command1_max_len=commandLine.find('|');
  return commandLine.substr(0,command1_max_len);
}

string GetCommand2(std::string commandLine)
{
  string command2;
  if(commandLine.find("|&") != string::npos)//with |&
  {
    size_t bg_pos=commandLine.find("|&");
    size_t command2_len = commandLine.size() -bg_pos-2;
    command2 = commandLine.substr(bg_pos+2,command2_len);
  }
  else//with |
  {
    size_t not_bg_pos=commandLine.find("|");
    size_t command2_len = commandLine.size() - not_bg_pos -1;
    command2 = commandLine.substr(not_bg_pos +1, command2_len);
  }
  //ignore the & symbol
  if(command2.find('&') != string::npos)
  {
    command2=command2.substr(0,command2.find('&'));
  }
  return command2;
}

void PipeCommand::execute()
{
  SmallShell& smash=SmallShell::getInstance();
  string command1 = GetCommand1(commandLine);
  string command2 = GetCommand2(commandLine);

  int returned_pid1 = fork();
  if(returned_pid1 == -1)
  {
    perror("smash error: fork failed");
  }

  if (returned_pid1 == 0) 
  {
    setpgrp();
    setpgid(getpid(), getpid());

    int fd[2];
    pipe(fd);
    
    int returned_pid2 = fork();
    if(returned_pid2 == -1)
    {
      perror("smash error: fork failed");
    }

    if (returned_pid2 == 0)
    {
      setpgrp();
      setpgid(returned_pid2, getppid());
      if (!is_bg)
      {
        dup2(fd[1], 1);
      }
      else
      {
        dup2(fd[1], 2);
      }
      close(fd[0]);
      close(fd[1]);
      smash.CreateCommand(command1.c_str())->execute();
      exit(0);

    }
    else
    {
      dup2(fd[0], 0);
      close(fd[0]);
      close(fd[1]);
      smash.CreateCommand(command2.c_str())->execute();
      if (waitpid(returned_pid2, nullptr, WUNTRACED) == -1)
      {
        perror("smash error: waitpid failed");
        exit(0);
      }
      exit(0);
    }
    exit(0);
  }
  else
  {
    setpgrp();
    setpgid(returned_pid1, returned_pid1);

    if (waitpid(returned_pid1, nullptr, WUNTRACED) == -1)
    {
      perror("smash error: waitpid failed");
    }
  }
  
}



void GetUserCommand::execute() {
    char* args[COMMAND_MAX_ARGS];
    int arg_Count = _parseCommandLine(commandLine.c_str(), args);

    if (arg_Count != 2) {
        std::cerr << "smash error: getuser: too many arguments" << std::endl;
        return;
    }

    pid_t pid = atoi(args[1]);
    std::stringstream proc_path;
    proc_path << "/proc/" << pid << "/status";

    std::ifstream status_file(proc_path.str());
    if (!status_file.is_open()) {
        std::cerr << "smash error: getuser: process " << pid << " does not exist" << std::endl;
        return;
    }
    std::string line;
    uid_t uid = -100;
    gid_t gid = -100;
    for (std::string line; std::getline(status_file, line); ) {
        if (line.find("Gid:") == 0) {
            std::istringstream gid_line(line.substr(4));
            gid_line >> gid;
        }
        else if (line.find("Uid:") == 0) {
            std::istringstream uid_line(line.substr(4));
            uid_line >> uid;
        }
        else {
            continue;
        }
    }

    struct passwd pwd;
    struct passwd *pw;
    struct group grp;
    struct group *gr;
    char buf1[2048];
    char buf2[2048];
    getpwuid_r(uid, &pwd, buf1, 2048, &pw);
    getgrgid_r(gid, &grp, buf2, 2048, &gr);
    std::cout << "User: " << pw->pw_name << std::endl;
    std::cout << "Group: " << gr->gr_name << std::endl;
}








void ListDirCommand::execute() {
    char* args[COMMAND_MAX_ARGS];
    int num_args = _parseCommandLine(commandLine.c_str(), args),dirctionfolder;
    std::string path;
    std::vector<std::string> files;
    std::vector<std::string> directories;
    std::vector<std::string> links;

    if (num_args > 2) {
        std::cerr << "smash error: listdir: too many arguments" << std::endl;
        return;
    }

    if (num_args == 1) {
        path = ".";
    } else {
        path = args[1];
    }

     dirctionfolder = open(path.c_str(), O_RDONLY | O_DIRECTORY);
    if (dirctionfolder == -1) {
        perror(" error with  listdir");
        return;
    }



    char buffer[8192];
    int nread;
    linux_dirent *d;
    int bpos;
    char d_type;

    while ((nread = syscall(SYS_getdents, dirctionfolder, buffer, sizeof(buffer))) > 0) {
        for (bpos = 0; bpos < nread; bpos += d->d_reclen) {
            d = (struct linux_dirent *) (buffer + bpos);
            d_type = *(buffer + bpos + d->d_reclen - 1);
            std::string entry_name(d->d_name);

            if ((entry_name == "." || entry_name == "..")||(d_type != DT_REG && d_type != DT_DIR && d_type != DT_LNK)) {
                continue;
            }

            switch (d_type) {
                case DT_REG:
                    files.push_back( entry_name);
                break;
                case DT_DIR:
                    directories.push_back(entry_name);
                break;
                case DT_LNK: {
                    char link_target[PATH_MAX];
                    ssize_t len = readlink((std::string(path) + "/" + entry_name).c_str(), link_target, sizeof(link_target) - 1);
                    if (len != -1) {
                        links.push_back(entry_name + " -> " + link_target);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    if (nread == -1) {
        perror("smash error: listdir");
        close(dirctionfolder);
        return;
    }

    close(dirctionfolder);

    std::sort(files.begin(), files.end());
    std::sort(directories.begin(), directories.end());
    std::sort(links.begin(), links.end());

    for (const auto& file : files) {
        std::cout << "file: "<< file << std::endl;
    }
    for (const auto& directory : directories) {
        std::cout <<"directory: "<< directory << std::endl;
    }
    for (const auto& link : links) {
        std::cout <<"link: "<< link << std::endl;
    }
}
