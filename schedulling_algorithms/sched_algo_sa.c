#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define INITIAL_TEMPERATURE 1000.0
#define FINAL_TEMPERATURE 0.1
#define COOLING_RATE 0.99

struct process {
    int pid;
    int at;
    int bt;
    int pr;   
    int ct;     
    int tat;    
    int wt;     
    int rt;     
    struct process *link;
};

int processes, processors;

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
    new_process->link = NULL;

    if (head == NULL) {
        head = new_process;
    }
    else {
        struct process *temp = head;
        while (temp->link != NULL) {
            temp = temp->link;
        }
        temp->link = new_process;
    }
    return head;
}

int calculate_metrics(struct process proc_array[], int assignments[]) {
    int load[processors]; 
    int start_time[processes];
    
    for (int i = 0; i < processors; i++) {
        load[i] = 0;
    }
    for (int i = 0; i < processes; i++) {
        start_time[i] = 0;
    }
    
    for (int i = 0; i < processes; i++) {
        int processor = assignments[i];
        int arrival_time = proc_array[i].at;
        
        if(load[processor] > arrival_time){
            start_time[i] = load[processor];
        }
        else{
            start_time[i] = arrival_time;
        }

        proc_array[i].ct = start_time[i] + proc_array[i].bt;
        proc_array[i].tat = proc_array[i].ct - proc_array[i].at;  
        proc_array[i].wt = proc_array[i].tat - proc_array[i].bt;  
        proc_array[i].rt = start_time[i] - arrival_time;          
        proc_array[i].pr = processor;                           
        load[processor] = proc_array[i].ct;
    }
    
    int interval = load[0];
    for (int i = 1; i < processors; i++) {
        if (load[i] > interval) {
            interval = load[i];
        }
    }
    return interval;
}

void generate_neighbor(int assignments[]) {
    int process1 = rand() % processes;
    int process2 = rand() % processes;
    while (process1 == process2) {
        process2 = rand() % processes;
    }
    
    int temp = assignments[process1];
    assignments[process1] = assignments[process2];
    assignments[process2] = temp;
}

struct process* simulated_annealing(struct process* head) {
    struct process proc_array[processes];
    struct process *temp = head;

    int i = 0;
    while (temp != NULL) {
        proc_array[i++] = *temp;
        temp = temp->link;
    }

    double temperature = INITIAL_TEMPERATURE;
    int current_assignments[processes];
    for (int i = 0; i < processes; i++) {
        current_assignments[i] = i % processors;
    }

    int best_assignments[processes];
    for (int i = 0; i < processes; i++) {
        best_assignments[i] = current_assignments[i];
    }

    int best_interval = calculate_metrics(proc_array, current_assignments);

    while (temperature > FINAL_TEMPERATURE) {
        int new_assignments[processes];
        for (int i = 0; i < processes; i++) {
            new_assignments[i] = current_assignments[i];
        }

        generate_neighbor(new_assignments);

        int current_interval = calculate_metrics(proc_array, current_assignments);
        int new_interval = calculate_metrics(proc_array, new_assignments);

        if (new_interval < current_interval || exp((current_interval - new_interval) / temperature) > (double)rand() / RAND_MAX) {
            for (int i = 0; i < processes; i++) {
                current_assignments[i] = new_assignments[i];
            }
            current_interval = new_interval;
        }

        if (current_interval < best_interval) {
            best_interval = current_interval;
            for (int i = 0; i < processes; i++) {
                best_assignments[i] = current_assignments[i];
            }
        }

        temperature *= COOLING_RATE;
    }

    calculate_metrics(proc_array, best_assignments);
    temp = head;
    for (int i = 0; i < processes; i++) {
        temp->pr = proc_array[i].pr;
        temp->ct = proc_array[i].ct;
        temp->tat = proc_array[i].tat;
        temp->wt = proc_array[i].wt;
        temp->rt = proc_array[i].rt;
        temp = temp->link;
    }

    printf("Best interval: %d\n", best_interval);
    return head;
}

void print_linkedlist(struct process *head) {
    printf("pid\tat\tbt\tpr\tct\ttat\twt\trt\n");
    while (head != NULL) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", head->pid, head->at, head->bt, head->pr, head->ct, head->tat, head->wt, head->rt);
        head = head->link;
    }
}

void calculate_avg(struct process *head){
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
        printf("Average CT : %.2f\n", sum_ct/count);
        printf("Average TAT : %.2f\n", sum_tat/count);
        printf("Average WT : %.2f\n", sum_wt/count);
        printf("Average RT : %.2f\n", sum_rt/count);
    }
}

int main() {
    srand(time(0)); 

    printf("Enter number of processors: ");
    scanf("%d", &processors);

    printf("Enter number of processes: ");
    scanf("%d", &processes);

    struct process* head = NULL;
    for (int i = 0; i < processes; i++) {
        head = create_process(head);
    }

    head = simulated_annealing(head);
    print_linkedlist(head);
    calculate_avg(head);

    return 0;
}
