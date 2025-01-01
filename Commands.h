#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <iostream>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <iomanip>
#include <vector>
#include <map>
#include <time.h>


#define COMMAND_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

typedef enum{STOPPED,BACKGROUND,FOREGROUND}Mode;

class Command {
// TODO: Add your data members
 public:
  std::string commandLine;
  bool come_from_alias;
  std::string alias_line;
  Command(const char* cmd_line):commandLine(cmd_line){
      come_from_alias=false;
      alias_line="";
  }
  virtual ~Command()=default;
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line);
  virtual ~BuiltInCommand()=default;//
};

class ExternalCommand : public Command {
 public:
  ExternalCommand(const char* cmd_line);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
  bool is_bg;
 public:
  PipeCommand(const char* cmd_line , bool bg);
  virtual ~PipeCommand() {}
  void execute() override;
};

class WatchCommand : public Command {
    // TODO: Add your data members
public:
    WatchCommand(const char *cmd_line);

    virtual ~WatchCommand() {}

    void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand()=default;
  void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
  public:
  ChangeDirCommand(const char* cmd_line);
  virtual ~ChangeDirCommand() {}
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
  public:
  QuitCommand(const char* cmd_line);
  virtual ~QuitCommand() {}
  void execute() override;
};




class JobsList {
 public:
  class JobEntry {
   // TODO: Add your data members
   public:
   int jobID;
   pid_t jobPID;
   std::string commandLine;
   Mode mode;
   time_t start_time;

   JobEntry(int jobID , pid_t jobPID , std::string commandLine , Mode mode , time_t start_time):jobID(jobID) , jobPID(jobPID) , commandLine(commandLine) , mode(mode) , start_time(start_time){}
   JobEntry(int jobID , pid_t jobPID , std::string commandLine , Mode mode):jobID(jobID) , jobPID(jobPID) , commandLine(commandLine) , mode(mode){}
   ~JobEntry() = default;
   void resetJob();
    void updateValues(int jobID,pid_t jobPID , std::string commandLine , Mode mode , time_t start_time);
  };

 // TODO: Add your data members
 public:
  std::vector<JobEntry> jobs_list;


  JobsList()=default;
  ~JobsList()=default;
  int maxJobID();
  void addJob(Command* cmd,pid_t jobPID,bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  void removeAllJobs();
  void stopJob(pid_t jobPID, int jobID, std::string commandLine, time_t strat_time);
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line);
  virtual ~JobsCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line);
  virtual ~KillCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line);
  virtual ~ForegroundCommand() {}
  void execute() override;
};


class aliasCommand : public BuiltInCommand {
public:
    aliasCommand(const char *cmd_line);

    virtual ~aliasCommand() {}

    void execute() override;
};

class unaliasCommand : public BuiltInCommand {
public:
    unaliasCommand(const char *cmd_line);

    virtual ~unaliasCommand() {}

    void execute() override;
};


class ChPromptCommand : public BuiltInCommand {
public:
    ChPromptCommand(const char* cmd_line);
    virtual ~ChPromptCommand() {}
    void execute() override;
};

class PWDCommand : public BuiltInCommand {
public:
    PWDCommand(const char* cmd_line);
    virtual ~PWDCommand() {}
    void execute() override;
};

struct linux_dirent {
 long d_ino;
 off_t d_off;
 unsigned short d_reclen;
 char d_name[];
};

class ListDirCommand : public BuiltInCommand {
public:
 ListDirCommand(const char* cmd_line);
 void execute() override;
};


class GetUserCommand : public BuiltInCommand {
public:
 GetUserCommand(const char *cmd_line);

 virtual ~GetUserCommand() {}

 void execute() override;
};




class SmallShell {
 private:
  // TODO: Add your data members
  SmallShell();
 public:
  JobsList::JobEntry* current_job;
  JobsList* smash_jobs_list;

  std::vector<std::string> alias_keys_vector;
  std::map<std::string,std::string> alias_map;
  bool is_alias;

  pid_t smash_pid;
  std::string prompt;
  std::string last_path_dirrectory;
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};




#endif //SMASH_COMMAND_H_
