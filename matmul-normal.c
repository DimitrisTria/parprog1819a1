/*
* Όπoυ N αριθμός μεγέθους πινάκων και να διαιρείται με το 4
* run command: gcc -O2 matmul-normal.c -o matmul-normal -DN=?
* export Assembler source file: gcc -O2 matmul-normal.c -S -DN=?
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void get_walltime(double *wct);

int main(int argc, char* argv[]) {
	/* a και b: πίνακες που θα πολλαπλασιαστούν μεταξύ τους
	* c: πίνακας με το αποτέλεσμα των παραπάνω */
	float *a, *b, *c;
	
	// Δέσμευση μνήμης με την malloc και έλεγχος δημιουργίας των τριών πινάκων
	a = (float*)malloc(N*N*sizeof(float));
	if(a==NULL) {
		return 1;
	}
	b = (float*)malloc(N*N*sizeof(float));
	if(b==NULL) {
		free(a);
		return 1;
	}
	c = (float*)malloc(N*N*sizeof(float));
	if(c==NULL) {
		free(a);
		free(b);
		return 1;
	}
	
	//Μετρητές για βρόγχους
	int i, j, k;
	
	// Αρχικοποίηση των τριών πινάκων
	float *pa = a, *pb = b, *pc = c;
	for(i=0; i<N*N; i++) {
		*pa = 2.0;
		*pb = 3.0;
		*pc = 0.0;
		pa++; pb++; pc++;
	}
	
	// Επαναφορά δεικτών στην αρχή των πινάκων που δείχνουν
	pa = a; pb = b; pc = c;
	
	// Φορτίο	
	double ts, te;
	get_walltime(&ts);
	for(k=0; k<N; k++) {
		pb = b;
		for(j=0; j<N; j++) {
			pa = a + k*N;
			for(i=0; i<N; i++) {
				*pc = *pc + (*pa) * (*pb);
				pa++; pb++;
			}
			pc++;
		}
	}
	get_walltime(&te);
	
	// Υπολογισμός χρόνου, σε seconds, και απόδοσης φορτίου με τη μετρική Megaflops
	float time = (te-ts);
	float mflops = (N*N*N) / (time*1e6);
	printf("time: %f and mflops: %f\n", time, mflops);
	
	// Έλεγχος αποτελεσμάτων από το φορτίο
	float check_val = 2.0*3.0*N;
	pc = c;
	for(i=0; i<N*N; i++) {
		if(*pc != 2.0*3.0*N) {
			printf("Error at c[%d], excpected '%f', get '%f'\n", i, check_val, *pc);
			return 1;
		}
		pc++;
	}
	
	// Αποδέσμευση δυναμικής μνήμης των πινάκων
	free(a);
	free(b);
	free(c);
	
	return 0;
}

void get_walltime(double *wct) {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	*wct = (double)(tp.tv_sec + tp.tv_usec / 1000000.0);
}
