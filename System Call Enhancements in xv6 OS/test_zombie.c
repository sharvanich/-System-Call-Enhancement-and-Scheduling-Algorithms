#include "user.h"

int main() {
    int zombie_count = count_zombies();
    printf("Number of zombie processes: %d\n", zombie_count);
    exit(0);
}
