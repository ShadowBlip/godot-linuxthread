#ifndef LINUX_THREAD_CLASS_H
#define LINUX_THREAD_CLASS_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#include <godot_cpp/core/binder_common.hpp>

class LinuxThread : public godot::RefCounted {
  GDCLASS(LinuxThread, godot::RefCounted);

protected:
  static void _bind_methods();

public:
  // Constructor/deconstructor
  LinuxThread();
  ~LinuxThread();

  static int get_tid();
  static int set_thread_priority(int niceness);
};

#endif // LINUX_THREAD_CLASS_H
