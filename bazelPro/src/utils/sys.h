#include <unistd.h>


namespace UTILS {

pid_t get_pid_with_name(const char* name);
void terminate_with_pid(pid_t pid);
double get_disk_usage();
double get_memory_usage();
double get_cpu_usage();

}//NAMESPACE