int flag = 0;
lock_t l;
cv_t cv;

void wait_for_update() {
    lock_acquire(l);
    if (flag == 0)
        cv_wait(cv, l);
        
    lock_release(l);
}

void signal_update_occurred() {
    lock_acquire(l);
    flag = 1;
    cv_signal(cv, l);
    lock_release(l);
}