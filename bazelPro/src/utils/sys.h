#include <unistd.h>


namespace UTILS {

pid_t get_pid_with_name(const char* name);
void terminate_with_pid(pid_t pid);

}//NAMESPACE