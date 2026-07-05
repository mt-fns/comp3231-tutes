semaphore_t room = 10;

void enter_room() {
    P(room);
}

void leave_room() {
    V(room);
}
