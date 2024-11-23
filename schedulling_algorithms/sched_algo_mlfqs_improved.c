#include <stdio.h>
#include <stdlib.h>

#define MAX 100
#define SIZE 3

int current_time = 0;
int time_quantum_rr = 2;
int time_quantum_sjf = 0;

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

struct process* create_process(struct process *head) {
    int v1, v2, v3;
    printf("Enter process id: ");
    scanf("%d", &v1);
    printf("Enter arrival time: ");
    scanf("%d", &v2);
    printf("Enter burst time: ");
    scanf("%d", &v3);

    struct process *new_process = (struct process*)malloc(sizeof(struct process));
    new_process->pid = v1;
    new_process->at = v2;
    new_process->bt = v3;
    new_process->pr = -1;  
    new_process->ct = -1;
    new_process->tat = -1;
    new_process->wt = -1;
    new_process->rt = -1;
    new_process->remain = v3;  // Initialize remain to burst time
    new_process->link = NULL;

    if (head == NULL) {
        head = new_process;
    } else {
        struct process *temp = head;
        while (temp->link != NULL) {
            temp = temp->link;
        }
        temp->link = new_process;
    }
    return head;
}

void enqueue(struct process* head, int* queue, int* front, int* rear, int pid) {
    struct process* temp = head;
    while (temp != NULL) {
        if (temp->at <= current_time && temp->remain > 0) {
            int is_in_queue = 0;
            for (int i = *front; i <= *rear; i++) {
                if (queue[i] == temp->pid) {
                    is_in_queue = 1;
                    break;
                }
            }
            if (!is_in_queue) {
                if (*rear == -1) {
                    *front = *rear = 0;
                } else {
                    (*rear)++;
                }
                queue[*rear] = temp->pid;
            }
        }
        temp = temp->link;
    }
    if (pid != -1) {
        int is_in_queue = 0;
        for (int i = *front; i <= *rear; i++) {
            if (queue[i] == pid) {
                is_in_queue = 1;
                break;
            }
        }
        if (!is_in_queue) {
            if (*rear == -1) {
                *front = *rear = 0;
            } else {
                (*rear)++;
            }
            queue[*rear] = pid;
        }
    }
}

int dequeue(int* front, int* rear, int* queue) {
    if (*front == -1) {
        return -1;
    }
    int pid = queue[*front];
    if (*front == *rear) {
        *front = *rear = -1;
    } else {
        (*front)++;
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

void dynamic_time_quantum(struct process* head, int* time_quantum) {
    int count = 0;
    struct process* temp = head;
    while (temp != NULL) {
        if (temp->remain > 0) {
            count++;
        }
        temp = temp->link;
    }
    if (count > 0) {
        struct process* sorted[count];
        temp = head;
        int i = 0;
        while (temp != NULL) {
            if (temp->remain > 0) {
                sorted[i++] = temp;
            }
            temp = temp->link;
        }
        for (int j = 0; j < count - 1; j++) {
            for (int k = 0; k < count - j - 1; k++) {
                if (sorted[k]->remain > sorted[k + 1]->remain) {
                    struct process* tmp = sorted[k];
                    sorted[k] = sorted[k + 1];
                    sorted[k + 1] = tmp;
                }
            }
        }
        int position = (2 * count) / 3;
        if (position >= count) position = count - 1;
        *time_quantum = sorted[position]->remain;
    }
}

int all_processes_completed(struct process* head) {
    struct process* temp = head;
    while (temp != NULL) {
        if (temp->remain > 0) return 0;
        temp = temp->link;
    }
    return 1;
}

void rr(struct process* head) {
    int queue[MAX], front = -1, rear = -1;
    enqueue(head, queue, &front, &rear, -1);
    while (!all_processes_completed(head)) {
        int pid = dequeue(&front, &rear, queue);
        if (pid == -1) {
            current_time++;
            enqueue(head, queue, &front, &rear, -1);
            continue;
        }
        struct process* ptr = find_process(head, pid);
        if (ptr->remain <= time_quantum_rr) {
            current_time += ptr->remain;
            ptr->remain = 0;
            ptr->ct = current_time;
            ptr->tat = ptr->ct - ptr->at;
            ptr->wt = ptr->tat - ptr->bt;
            ptr->rt = current_time - ptr->at - ptr->bt;
        } else {
            current_time += time_quantum_rr;
            ptr->remain -= time_quantum_rr;
            enqueue(head, queue, &front, &rear, ptr->pid);
        }
    }
}

void sjf(struct process* head) {
    int queue[MAX], front = -1, rear = -1;
    enqueue(head, queue, &front, &rear, -1);
    dynamic_time_quantum(head, &time_quantum_sjf);
    while (!all_processes_completed(head)) {
        int pid = dequeue(&front, &rear, queue);
        if (pid == -1) {
            current_time++;
            enqueue(head, queue, &front, &rear, -1);
            continue;
        }
        struct process* ptr = find_process(head, pid);
        if (ptr->remain <= time_quantum_sjf) {
            current_time += ptr->remain;
            ptr->remain = 0;
            ptr->ct = current_time;
            ptr->tat = ptr->ct - ptr->at;
            ptr->wt = ptr->tat - ptr->bt;
            ptr->rt = current_time - ptr->at - ptr->bt;
        } else {
            current_time += time_quantum_sjf;
            ptr->remain -= time_quantum_sjf;
            enqueue(head, queue, &front, &rear, ptr->pid);
        }
    }
}

void assign_processors(int processors, struct process*head) {
    struct process *temp = head;
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
    
    assign_processors(processors, head);
    printf("Running Round Robin Scheduling...\n");
    rr(head);
    printf("Running Shortest Job First Scheduling...\n");
    sjf(head);

    printf("Final Process Table:\n");
    print_linkedlist(head);
    return 0;
}
