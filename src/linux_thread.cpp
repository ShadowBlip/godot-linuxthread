#include "linux_thread.h"

LinuxThread::LinuxThread(){};
LinuxThread::~LinuxThread(){};

// Return the thread ID of the currently running thread
int LinuxThread::get_tid() { return gettid(); }

// Set the priority of the current thread to the given niceness value
int LinuxThread::set_thread_priority(int nice) {
  return setpriority(PRIO_PROCESS, 0, nice);
}

// Register the methods with Godot
void LinuxThread::_bind_methods() {
  // Static methods
  godot::ClassDB::bind_static_method("LinuxThread", godot::D_METHOD("get_tid"),
                                     &LinuxThread::get_tid);
  godot::ClassDB::bind_static_method("LinuxThread",
                                     godot::D_METHOD("set_thread_priority"),
                                     &LinuxThread::set_thread_priority);
};
