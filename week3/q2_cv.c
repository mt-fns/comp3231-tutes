int occupants = 0;
lock_t room_lock;
cv_t room_cv;

void enter_room() {
    lock_acquire(room_lock);
    while (occupants == 10)
        cv_wait(room_cv, room_lock);
    occupants = occupants + 1;
    lock_release(room_lock);
}

void leave_room() {
    lock_acquire(room_lock);
    occupants = occupants - 1;
    if (occupants == 9)
        cv_signal(room_cv, room_lock);
    lock_release(room_lock);
}