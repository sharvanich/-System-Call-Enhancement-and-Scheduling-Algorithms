#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

struct process {
    int pid;
    int tt;               
    int at;              
    int bt;               
    int pr;              
    int ct;              
    int tat;             
    int wt;             
    int rt;              
    int remaining_time;   
    bool completed;      
    struct process *link; 
};

struct process *head = NULL;

struct process* create_process(struct process *head) {
    int v1, v2, v3, v4;
    printf("Enter process id: ");
    scanf("%d", &v1);
    printf("Enter no. of Tickets: ");
    scanf("%d", &v2);
    printf("Enter arrival time: ");
    scanf("%d", &v3);
    printf("Enter burst time: ");
    scanf("%d", &v4);

    struct process *new_process = (struct process*)malloc(sizeof(struct process));
    new_process->pid = v1;
    new_process->tt = v2;
    new_process->at = v3;
    new_process->bt = v4;
    new_process->pr = -1;  
    new_process->ct = -1;
    new_process->tat = -1;
    new_process->wt = -1;
    new_process->rt = -1;
    new_process->remaining_time = v4;
    new_process->completed = false;
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

void assign_processors(int processors) {
    struct process *temp = head;
    int i = 0;
    while (temp != NULL) {
        temp->pr = i % processors; 
        temp = temp->link;
        i++;
    }
}

int calculate_total_tickets(struct process *head, int current_time) {
    int total_tickets = 0;
    struct process *temp = head;
    while (temp != NULL) {
        if (!temp->completed && temp->at <= current_time) {
            total_tickets += temp->tt;
        }
        temp = temp->link;
    }
    return total_tickets;
}

struct process* lottery_draw(struct process *head, int total_tickets, int current_time) {
    if (total_tickets == 0) return NULL;
    int winning_ticket = rand() % total_tickets + 1;
    int ticket_count = 0;
    struct process *temp = head;
    while (temp != NULL) {
        if (!temp->completed && temp->at <= current_time) {
            ticket_count += temp->tt;
            if (ticket_count >= winning_ticket) {
                return temp;
            }
        }
        temp = temp->link;
    }
    return NULL;
}

void calculate_metrics(struct process *head) {
    struct process *temp = head;
    while (temp != NULL) {
        temp->tat = temp->ct - temp->at;
        temp->wt = temp->tat - temp->bt;
        if (temp->rt == -1) {
            temp->rt = temp->at;
        }
        temp = temp->link;
    }
}

void print_linkedlist(struct process *head) {
    printf("PID\tTT\tAT\tBT\tPR\tCT\tTAT\tWT\tRT\n");
    struct process *temp = head;
    while (temp != NULL) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               temp->pid, temp->tt, temp->at, temp->bt,
               temp->pr, temp->ct, temp->tat, temp->wt, temp->rt);
        temp = temp->link;
    }
}

void calculate_avg(struct process *head) {
    float sum_tat = 0.0, sum_wt = 0.0, sum_rt = 0.0, sum_ct = 0.0;
    struct process *temp = head;
    int count = 0;

    while (temp != NULL) {
        sum_ct += temp->ct;
        sum_tat += temp->tat;
        sum_wt += temp->wt;
        sum_rt += temp->rt;
        temp = temp->link;
        count++;
    }

    if (count > 0) {
        printf("Average CT : %.2f\n", sum_ct / count);
        printf("Average TAT : %.2f\n", sum_tat / count);
        printf("Average WT : %.2f\n", sum_wt / count);
        printf("Average RT : %.2f\n", sum_rt / count);
    }
}

int main() {
    srand(time(0));
    int processors, processes;
    printf("Enter number of processors: ");
    scanf("%d", &processors);

    printf("Enter number of processes: ");
    scanf("%d", &processes);

    for (int i = 0; i < processes; i++) {
        head = create_process(head);
    }

    assign_processors(processors);

    int active_processes = processes, round = 1, current_time = 0;

    while (active_processes > 0) {
        printf("\nRound %d:\n", round);
        int total_tickets = calculate_total_tickets(head, current_time);

        struct process *selected_process = lottery_draw(head, total_tickets, current_time);

        if (selected_process == NULL) {
            current_time++;
            continue;
        }

        printf("Processor %d is running Process %d\n", selected_process->pr + 1, selected_process->pid);

        if (selected_process->rt == -1) {
            selected_process->rt = current_time - selected_process->at;
        }

        selected_process->remaining_time--;
        current_time++;

        if (selected_process->remaining_time == 0) {
            selected_process->completed = true;
            selected_process->ct = current_time;
            active_processes--;
            printf("Process %d has completed!\n", selected_process->pid);
        }
        round++;
    }

    calculate_metrics(head);
    printf("\nAll processes have completed.\n");
    print_linkedlist(head);
    calculate_avg(head);

    return 0;
}
