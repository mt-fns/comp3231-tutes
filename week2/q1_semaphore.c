/*
typedef struct {
    int count;
    struct process *L;
} semaphore;
*/

semaphore updated = 10;

void wait_for_update() {
    P(updated);
}

void signal_update_occurred() {
    V(updated);
}