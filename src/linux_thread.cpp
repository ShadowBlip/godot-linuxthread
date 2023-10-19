#include "linux_thread.h"

#if !defined(PR_SET_CHILD_SUBREAPER)
#define PR_SET_CHILD_SUBREAPER 36
#endif

using godot::CharString;
using godot::ClassDB;
using godot::D_METHOD;
using godot::PackedStringArray;
using godot::String;
using godot::UtilityFunctions;
using godot::Vector;

LinuxThread::LinuxThread(){};
LinuxThread::~LinuxThread(){};

// Return the thread ID of the currently running thread
int LinuxThread::get_tid() { return gettid(); }

// Set the priority of the current thread to the given niceness value
int LinuxThread::set_thread_priority(int nice) {
  return setpriority(PRIO_PROCESS, 0, nice);
}

// Handle child finished signals
// https://stackoverflow.com/questions/26409877/cleaning-up-children-processes-asynchronously
void signal_handler(int sig) { wait(NULL); }

// Create a subreaper process that will execute the given command and wait
// until the command and its children have exited. Returns the PID of the
// subreaper.
int LinuxThread::subreaper_create_process(String cmd, PackedStringArray args) {
  // Create a new reaper process
  pid_t reaper_pid = fork();
  if (reaper_pid == -1) {
    UtilityFunctions::push_error("subreaper: initial fork() failed!");
    return -1;
  }

  // If we are not the new reaper process, return the reaper's PID
  if (reaper_pid != 0) {
    return reaper_pid;
  }

  // Create a signal action that will listen for child processes that exit and
  // need to be cleaned up by the reaper.
  struct sigaction siac;
  memset(&siac, 0, sizeof(struct sigaction));
  siac.sa_handler = signal_handler;
  sigaction(SIGCHLD, &siac, NULL);

  // Mark the process as child subreaper
  if (prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0) == -1) {
    UtilityFunctions::push_error("subreaper: prctl() failed!");
    quick_exit(1);
  }

  // Fork to execute the given command.
  pid_t pid = fork();
  if (pid == -1) {
    UtilityFunctions::push_error("subreaper: fork() failed!");
    quick_exit(1);
  }

  // Are we in the child?
  if (pid == 0) {
    Vector<CharString> cs;
    cs.push_back(cmd.utf8());
    for (int i = 0; i < args.size(); i++) {
      cs.push_back(args[i].utf8());
    }

    Vector<char *> argv;
    for (int i = 0; i < cs.size(); i++) {
      argv.push_back((char *)cs[i].get_data());
    }
    argv.push_back(0);

    execvp(cmd.utf8().get_data(), &argv[0]);
  }

  // If we are the subreaper, wait until all children exit
  UtilityFunctions::print("subreaper: waiting for children to exit");
  pid_t wait_ret;
  while (true) {
    wait_ret = wait(NULL);

    if (wait_ret == -1 && errno == ECHILD) {
      UtilityFunctions::print("subreaper: no more children exist");
      // No more children.
      break;
    }
  }
  UtilityFunctions::print("subreaper: exiting");
  quick_exit(0);
}

// Create a sub-reaper process
int LinuxThread::subreaper_exec(String cmd, PackedStringArray args) {
  // (Don't Lose) The Children
  if (prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0) == -1) {
    UtilityFunctions::push_error("subreaper: prctl() failed!");
    return -1;
  }

  pid_t pid = fork();

  if (pid == -1) {
    UtilityFunctions::push_error("subreaper: fork() failed!");
    return -1;
  }

  // Are we in the child?
  if (pid == 0) {
    Vector<CharString> cs;
    cs.push_back(cmd.utf8());
    for (int i = 0; i < args.size(); i++) {
      cs.push_back(args[i].utf8());
    }

    Vector<char *> argv;
    for (int i = 0; i < cs.size(); i++) {
      argv.push_back((char *)cs[i].get_data());
    }
    argv.push_back(0);

    execvp(cmd.utf8().get_data(), &argv[0]);
  }

  pid_t wait_ret;
  while (true) {
    wait_ret = wait(NULL);

    if (wait_ret == -1 && errno == ECHILD) {
      // No more children.
      break;
    }
  }

  return 0;
}

// Register the methods with Godot
void LinuxThread::_bind_methods() {
  // Static methods
  ClassDB::bind_static_method("LinuxThread", D_METHOD("get_tid"),
                              &LinuxThread::get_tid);
  ClassDB::bind_static_method("LinuxThread",
                              D_METHOD("set_thread_priority", "nice"),
                              &LinuxThread::set_thread_priority);
  ClassDB::bind_static_method(
      "LinuxThread", D_METHOD("subreaper_create_process", "cmd", "args"),
      &LinuxThread::subreaper_create_process);
  ClassDB::bind_static_method("LinuxThread",
                              D_METHOD("subreaper_exec", "cmd", "args"),
                              &LinuxThread::subreaper_exec);
};
