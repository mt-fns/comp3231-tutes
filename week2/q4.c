void me() {
	P(mutex);
	/* do something */
	
	P(data);
	/* do something else */
	
	V(mutex);
	
	/* clean up */
	V(data);
}
 
void you() {
    P(mutex);
	P(data)
	
	/* do something */
	
	V(data);
	V(mutex);
}

