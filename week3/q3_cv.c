disk_block_status_t disk_block_status = NOT_READY;
lock_t block_lock;
cv_t block_cv;

void wait_block() {
    lock_acquire(block_lock);
    while (disk_block_status != READY)
        cv_wait(block_cv, block_lock);
    lock_release(block_lock);

void make_block_ready() {
    lock_acquire(block_lock);
    disk_block_status = READY;
    // wake ALL waiting threads
    cv_broadcast(block_cv, block_lock);  
    lock_release(block_lock);
}