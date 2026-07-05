struct lock *fork_locks[NUM_PHILOSOPHERS];

void take_both_forks(unsigned long phil_num)
{
    // num = 3

    int lower, higher;

    lower = phil_num; /* left fork */
    higher = (phil_num + 1) % NUM_PHILOSOPHERS; /* right fork */

    if (lower > higher) {
        /* swap lower/higher to avoid deadlock */
        lower = higher;
        higher = phil_num;
    }

    lock_acquire(fork_locks[lower]);
    lock_acquire(fork_locks[higher]);
}

void release_forks(unsigned long phil_num)
{
    lock_release(fork_locks[phil_num]);
    lock_release(fork_locks[(phil_num + 1) % NUM_PHILOSOPHERS]);
}