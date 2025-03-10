#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
    int pid = 999;  // An arbitrary PID that doesn't exist
    int ticks = ticks_run(pid);
    if (ticks == -1) {
        printf(1, "Correctly returned -1 for non-existent process %d\n", pid);
    } else {
        printf(1, "Unexpected behavior for process %d, returned %d ticks\n", pid, ticks);
    }
    exit();
}
