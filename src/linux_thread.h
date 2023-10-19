#ifndef LINUX_THREAD_CLASS_H
#define LINUX_THREAD_CLASS_H

#include <csignal>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/char_string.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/binder_common.hpp>

using godot::PackedStringArray;
using godot::String;

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
  static int subreaper_create_process(String cmd, PackedStringArray args);
  static int subreaper_exec(String cmd, PackedStringArray args);
};

#endif // LINUX_THREAD_CLASS_H
