#include 
#include 
#include 
#include "list.h"

bool node_init(node_t *node)
{
        struct lock *lock;
        lock = lock_create("node lock");
        if (lock == NULL)
                return false;
        node->lock = lock;
        node->next = NULL;
        node->prev = NULL;
        return true;
}
void node_cleanup(node_t *node)
{
        lock_destroy(node->lock);
}

bool list_init(list_t *list)
{
        bool r;
        r = node_init(&list->head);
        if (! r)
                return false;
        r = node_init(&list->tail);
        if (! r) {
                node_cleanup(&list->head);
                return false;
        }
        list->head.next = &list->tail;
        list->tail.prev = & list->head;
        return true;
}
                
        
void list_cleanup(list_t *list)
{
        /* TBD */
        (void) list;
}

void add_head(list_t *list, node_t *node)
{
        insert_after(&list->head, node);
}

void add_tail(list_t *list, node_t *node)
{
        insert_before(list, &list->tail, node);
}

void insert_after(node_t *in_list, node_t *new_node)
{
        /* assumption, in_list is not removed before we successfully
           insert, this in only true for the head sentinal
           node */
        
        lock_acquire(new_node->lock); /* start new node in the locked state */

        /* acquire in order head -> tail */
        lock_acquire(in_list->lock);
        lock_acquire(in_list->next->lock);

        /* now can add */
        new_node->next = in_list->next;
        new_node->prev = in_list;

        in_list->next = new_node;
        new_node->next->prev = new_node;

        lock_release(new_node->next->lock);
        lock_release(new_node->lock);
        lock_release(in_list->lock);
}




static bool hand_over_locking(list_t *list, node_t *target)
{
        /* do hand over locking until we find the target */
        /* Either we find the target and return with target and
           predecessor locked or we return false if we don't find
           it */

        node_t *p, *t;
        
        p = &list->head;
        lock_acquire(p->lock);

        t = p->next;
        lock_acquire(t->lock);
        while (t != target) {
                if (t == &list->tail) {
                        /* can't find target */
                        lock_release(p->lock);
                        lock_release(t->lock);
                        return false;
                }
                lock_release(p->lock);
                p = t;
                t = t-> next;
                lock_acquire(t->lock);
        }
        return true;
}
        
        
bool insert_before(list_t *list, node_t *in_list, node_t *new_node)
{
        /* assume in_list is not removed before starting, this is only
         true for the tail sentinel node */
        
        bool r;
        
        lock_acquire(new_node->lock); /* again start in locked state */
        

        /* lock acquire in order head -> tail 

           This fragment attempt to avoid hand-over locking by starting at the
           tail. It accesses prev without holding the lock, so it might change,
           and thus needs to check it's still prev after both locks are held.


           p = in_list->prev;
           lock_acquire(p->lock);
           lock_acquire(in_list->lock);

           while (p->next != in_list) {
                 
                 a node must have got inserted
                 prior to getting the lock
                
                lock_release(p->lock);
                lock_release(in_list->lock);
                p = in_list->prev;
                lock_acquire(p->lock);
                lock_acquire(in_list->lock);
           }

           Yuck, this is livelock-able....  Basically one can't use the
           prev pointers to lock as we require the current node's lock
           prior to using prev, but then we can't lock the
           predecessor due to lock ordering needed to avoid deadlock.
           
        */


        /* we changed the prototype to include the list and use
           handover locking to lock in_list and predecessor */

        r = hand_over_locking(list, in_list);

        /* this should always succeed in a system that does not remove
           in_list */
        
        if (!r) {
                lock_release(new_node->lock);
                return r;
        }
        
        /* now have locks on prev and in_list */

        /* now can add */
        new_node->prev = in_list->prev;
        new_node->next = in_list;

        in_list->prev = new_node;
        new_node->prev->next = new_node;

        lock_release(new_node->prev->lock);
        lock_release(new_node->lock);
        lock_release(in_list->lock);
        return true;
}

static void remove_with_locks_held(node_t *in_list)
{
        in_list->prev->next = in_list->next;
        in_list->next->prev = in_list->prev;
                
}

void remove_head(list_t *list, node_t **node)
{
        node_t *n;
        lock_acquire(list->head.lock); /* sentinel head */
        lock_acquire(list->head.next->lock); /* removal candidate */
        
        if (list->head.next == &list->tail) {
                /* nothing to remove */
                lock_release(list->head.next->lock); 
                lock_release(list->head.lock); 
                *node = NULL; /* return null for empty list */
        }
                
        lock_acquire(list->head.next->next->lock); /* relies on lock
                                                    held above to use
                                                    the pointer */

        n = list->head.next;
        
        remove_with_locks_held(n);
        
        lock_release(list->head.lock); /* sentinel head */
        lock_release(n->lock); /* removed node */
        lock_release(list->head.next->lock); /* new head */

        *node = n;
}

void remove_tail(list_t *list, node_t **node)
{
        
/* leave this as an exercise, this is a handover involving three nodes
   where the last one is the tail */

        (void) list;
        (void) node;
}

bool remove(list_t *list, node_t *in_list)
{

        bool r;
        
        r = hand_over_locking(list, in_list);

        /* this should always succeed in a non-broken system,
           otherwise we have attempts to remove the same node more than
           once */
        
        if (!r)
                return r;

        /* in_list and predecessor is lock */

        lock_acquire(in_list->next->lock); /* grab the third lock */

        /* we can now remove the node */

        remove_with_locks_held(in_list);
        
        lock_release(in_list->prev->lock);
        lock_release(in_list->next->lock);
        lock_release(in_list->lock);
        return true;
}