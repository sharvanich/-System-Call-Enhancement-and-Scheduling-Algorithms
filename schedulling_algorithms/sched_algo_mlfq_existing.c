#include <stdio.h>
#include <stdlib.h>

#define MAX 100
#define LEVELS 3
#define QUANTUM_0 2
#define QUANTUM_1 4
#define QUANTUM_2 8

int current_time = 0;

struct process {
    int pid;
    int at;
    int bt;
    int pr;
    int ct;
    int tat;
    int wt;
    int rt;
    int remain;
    struct process* link;
};

struct process* create_process(struct process* head) {
    int v1, v2, v3;
    printf("Enter process id: ");
    scanf("%d", &v1);
    printf("Enter arrival time: ");
    scanf("%d", &v2);
    printf("Enter burst time: ");
    scanf("%d", &v3);

    struct process* new_process = (struct process*)malloc(sizeof(struct process));
    new_process->pid = v1;
    new_process->at = v2;
    new_process->bt = v3;
    new_process->pr = 0;
    new_process->ct = -1;
    new_process->tat = -1;
    new_process->wt = -1;
    new_process->rt = -1;
    new_process->remain = v3;
    new_process->link = NULL;

    if (head == NULL) {
        head = new_process;
    } else {
        struct process* temp = head;
        while (temp->link != NULL) {
            temp = temp->link;
        }
        temp->link = new_process;
    }
    return head;
}

void enqueue(int queue[MAX][MAX], int* front, int* rear, int level, int pid) {
    if (rear[level] == MAX - 1) {
        printf("Queue overflow at level %d.\n", level);
        return;
    }
    if (rear[level] == -1) {
        front[level] = rear[level] = 0;
    } else {
        rear[level]++;
    }
    queue[level][rear[level]] = pid;
}

int dequeue(int queue[MAX][MAX], int* front, int* rear, int level) {
    if (front[level] == -1) {
        return -1;
    }
    int pid = queue[level][front[level]];
    if (front[level] == rear[level]) {
        front[level] = rear[level] = -1;
    } else {
        front[level]++;
    }
    return pid;
}

struct process* find_process(struct process* head, int pid) {
    struct process* temp = head;
    while (temp != NULL) {
        if (temp->pid == pid) {
            return temp;
        }
        temp = temp->link;
    }
    return NULL;
}

struct process* bubble_sort(struct process* head) {
    struct process* ptr1 = NULL;
    struct process* lptr = NULL;
    int swapped;

    do {
        swapped = 0;
        ptr1 = head;
        while (ptr1->link != lptr) {
            if (ptr1->at > ptr1->link->at) {
                struct process temp = *ptr1;
                *ptr1 = *(ptr1->link);
                *(ptr1->link) = temp;

                struct process* temp_link = ptr1->link->link;
                ptr1->link->link = ptr1->link;
                ptr1->link = temp_link;

                swapped = 1;
            }
            ptr1 = ptr1->link;
        }
        lptr = ptr1;
    } while (swapped);
    return head;
}

void mlfq(struct process* head) {
    int queue[MAX][MAX] = {0}, front[LEVELS], rear[LEVELS];
    int quantum[LEVELS] = {QUANTUM_0, QUANTUM_1, QUANTUM_2};

    for (int i = 0; i < LEVELS; i++) {
        front[i] = rear[i] = -1;
    }

    struct process* temp = head;
    while (temp != NULL) {
        enqueue(queue, front, rear, 0, temp->pid);
        temp = temp->link;
    }

    while (1) {
        int all_done = 1;
        for (int i = 0; i < LEVELS; i++) {
            while (front[i] != -1) {
                int pid = dequeue(queue, front, rear, i);
                struct process* ptr = find_process(head, pid);
                if (ptr->at > current_time) {
                    current_time = ptr->at;
                }
                if (ptr->remain <= quantum[i]) {
                    current_time += ptr->remain;
                    ptr->remain = 0;
                    ptr->ct = current_time;
                    ptr->tat = ptr->ct - ptr->at;
                    ptr->wt = ptr->tat - ptr->bt;
                    ptr->rt = current_time - ptr->at - ptr->bt;
                } else {
                    current_time += quantum[i];
                    ptr->remain -= quantum[i];
                    if (i < LEVELS - 1) {
                        enqueue(queue, front, rear, i + 1, pid);
                    } else {
                        enqueue(queue, front, rear, i, pid);
                    }
                }
                all_done = 0;
            }
        }
        if (all_done) {
            break;
        }
        current_time++;  // Increment time if all queues are empty
    }
}

void assign_processors(int processors, struct process* head) {
    struct process* temp = head;
    int i = 0;
    while (temp != NULL) {
        temp->pr = i % processors; 
        temp = temp->link;
        i++;
    }
}

void print_linkedlist(struct process *head) {
    printf("pid\tat\tbt\tpr\tct\ttat\twt\trt\n");
    while (head != NULL) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", head->pid, head->at, head->bt, head->pr, head->ct, head->tat, head->wt, head->rt);
        head = head->link;
    }
}

int main() {
    int processors, processes;

    printf("Enter number of processors: ");
    scanf("%d", &processors);

    printf("Enter number of processes: ");
    scanf("%d", &processes);

    struct process* head = NULL;

    for (int i = 0; i < processes; i++) {
        head = create_process(head);
    }

    head = bubble_sort(head);
    assign_processors(processors, head);
    mlfq(head);

    printf("Final Process Table:\n");
    print_linkedlist(head);
    return 0;
}
