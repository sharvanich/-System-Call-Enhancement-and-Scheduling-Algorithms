#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    uint64 utilization = cpuutilization();
    printf("CPU Utilization: %d%%\n", utilization);
    exit(0);
}
