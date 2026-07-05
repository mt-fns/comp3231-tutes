int waiters_count = 0;
disk_block_status_t disk_block_status = NOT_READY;
semaphore_t block_mutex = 1;
semaphore_t block_sem = 0;

void wait_block() {
    P(block_mutex);
    if (disk_block_status == READY) {
        V(block_sem);
    } else {
        waiters_count = waiters_count + 1;
    }
    V(block_mutex);
    P(block_sem);

    
}

void make_block_ready() {
    P(block_mutex);
    disk_block_status = READY;
    while (waiters_count != 0) {
        waiters_count = waiters_count - 1;
        V(block_sem);
    }
    V(block_mutex);
}