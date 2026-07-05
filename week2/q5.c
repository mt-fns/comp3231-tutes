/*
laurel: mutex -> file1 -> file2; file2->file1
hardy: file1 -> file2; mutex -> file1
*/

lock *file1, *file2, *mutex;
 
void laurel() {
	lock_acquire(mutex);
	/* do something */
	
	lock_acquire(file1);
    	/* write to file 1 */
 
	lock_acquire(file2);
	/* write to file 2 */
 
	lock_release(file1);
	lock_release(mutex);
 
    /* if we context switch here a deadlock might occur */

	/* do something */
	
	lock_acquire(file1);
 
	/* read from file 1 */
	/* write to file 2 */
 
	lock_release(file2);
	lock_release(file1);
}
 
void hardy() {
    	/* do stuff */
	
	lock_acquire(file1);
	/* read from file 1 */
 
	lock_acquire(file2);
	/* write to file 2 */
	
	lock_release(file1);
	lock_release(file2);
 
	lock_acquire(mutex);
	/* do something */
	lock_acquire(file1);
	/* write to file 1 */
	lock_release(file1);
	lock_release(mutex);
}