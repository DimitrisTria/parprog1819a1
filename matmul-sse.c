/*
* Όπoυ N αριθμός μεγέθους πινάκων και να διαιρείται με το 4
* run command: gcc -O2 matmul-see.c -o matmul-sse -DN=?
* export Assembler source file: gcc -O2 matmul-sse.c -S -DN=?
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <emmintrin.h>

void get_walltime(double *wct);

int main(int argc, char *argv[]) {
	/* a και b: πίνακες που θα πολλαπλασιαστούν μεταξύ τους
	* c: πίνακας με το αποτέλεσμα των παραπάνω
	* sum: βοηθητικός πίνακας τεσσάρων θέσεων που χρησιμεύει
	*	   στον στον υπολογισμό κάθε στοιχείου το πίνακα c */
	float *a, *b, *c, *sum;
	
	// Δέσμευση μνήμης με την posix_memalign και έλεγχος δημιουργίας των τριών πινάκων
	int check_a = posix_memalign((void**)&a, 16, N*N*sizeof(float));
    if(check_a != 0) {
        return 1;
    }
    int check_b = posix_memalign((void**)&b, 16, N*N*sizeof(float));
    if(check_b != 0) {
        free(a);
        return 1;
    }
    int check_c = posix_memalign((void**)&c, 16, N*N*sizeof(float));
    if(check_c != 0) {
        free(a);
        free(b);
        return 1;
    }
	int check_sum = posix_memalign((void**)&sum, 16, 4*sizeof(float));
    if(check_sum != 0) {
        free(a);
        free(b);
		free(c);
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
	
	//Αρχικοποίηση πίνακα που θα περιελαμβάνει τα αποτελέσματα για κάθε τετράδας
	float *psum = sum;
	for(i=0; i<4; i++) {
		*psum = 0.0;
		psum++;
	}
	
	// Επαναφορά δεικτών στην αρχή των πινάκων
	pa = a; pb = b; pc = c; psum = sum;
	
	// Δημιουργία των δεικτών τύπου __m128 για καθένα από τους πίνακες
	__m128 *ma, *mb, *mc = (__m128*)pc, *msum = (__m128*)psum;
	
	//Φορτίο
	double ts, te;
	get_walltime(&ts);
	for(k=0; k<N; k++) {
		mb = (__m128*)pb;
		for(j=0; j<N; j++) {
			ma = (__m128*)(a + k*N);
			*msum = _mm_set_ps(0.0, 0.0, 0.0, 0.0);		// Αρχικοποίηση τιμών του δείκτη msum στο 0
			for(i=0; i<N; i+=4) {
				*msum = _mm_add_ps(*msum, _mm_mul_ps(*ma, *mb));
				ma++; mb++;
			}
			
			// Υπολογισμός αποτέλεσματος του τρέχοντος στοιχείου pc
			psum = sum;
			for(i=0; i<4; i++) {
				*pc = *pc + *psum;
				psum++;
			}
			pc++;
		}
	}
	get_walltime(&te);
	
	// Υπολογισμός χρόνου, σε seconds, και απόδοσης φορτίου με τη μετρική Megaflops
	float time = (te-ts);
	float mflops = (1.0*N*N*N) / (time*1e6);
	printf("time: %f and mflops: %f\n", time, mflops);
	
	// Έλεγχος αποτελεσμάτων από το φορτίο
	float check_val = 2.0*3.0*N;
	pc = c;
	for(i=0; i<N*N; i++) {
		if(*pc != 2.0*3.0*N) {
			printf("Error at c[%d], excpected '%f', get '%f'\n", i, check_val, *pc);
			break;
		}
		pc++;
	}

	// Αποδέσμευση δυναμικής μνήμης των πινάκων
	free(a);
	free(b);
	free(c);
	free(sum);
	
	return 0;
}

void get_walltime(double *wct) {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	*wct = (double)(tp.tv_sec + tp.tv_usec / 1000000.0);
}
