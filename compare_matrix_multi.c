#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#define PFIRST 40
#define PLAST  800
#define PINC   40
#define NREPEATS 2

void MY_MMult(int, int, int, double *, int, double *, int, double *, int );

int main() {
    int 
    p, 
    m, n, k,
    lda, ldb, ldc, 
    rep;

  double
    dtime, dtime_best,        
    gflops, 
    diff;

  double 
    *a, *b, *c, *cref, *cold;  

  // printf( "MY_MMult = [\n" );
  printf("Size,Gflops,Diff\n");
  
  for ( p=PFIRST; p<=PLAST; p+=PINC ){
    m = p;
    n = p;
    k = p;

    gflops = 2.0 * m * n * k * 1.0e-09;

    // lda = ( LDA == -1 ? m : LDA );
    // ldb = ( LDB == -1 ? k : LDB );
    // ldc = ( LDC == -1 ? m : LDC );
    lda = 1000;
    ldb = 1000;
    ldc = 1000;

    /* Allocate space for the matrices */
    /* Note: I create an extra column in A to make sure that
       prefetching beyond the matrix does not cause a segfault */
    a = ( double * ) malloc( lda * (k+1) * sizeof( double ) );  
    b = ( double * ) malloc( ldb * n * sizeof( double ) );
    c = ( double * ) malloc( ldc * n * sizeof( double ) );
    cold = ( double * ) malloc( ldc * n * sizeof( double ) );
    cref = ( double * ) malloc( ldc * n * sizeof( double ) );

    /* Generate random matrices A, B, Cold */
    random_matrix( m, k, a, lda );
    random_matrix( k, n, b, ldb );
    random_matrix( m, n, cold, ldc );

    copy_matrix( m, n, cold, ldc, cref, ldc );

    /* Run the reference implementation so the answers can be compared */

    REF_MMult( m, n, k, a, lda, b, ldb, cref, ldc );

    /* Time the "optimized" implementation */
    for ( rep=0; rep<NREPEATS; rep++ ){
      copy_matrix( m, n, cold, ldc, c, ldc );

      /* Time your implementation */
      dtime = dclock();

      MY_MMult( m, n, k, a, lda, b, ldb, c, ldc );
      
      dtime = dclock() - dtime;

      if ( rep==0 )
	dtime_best = dtime;
      else
	dtime_best = ( dtime < dtime_best ? dtime : dtime_best );
    }

    diff = compare_matrices( m, n, c, ldc, cref, ldc );

    printf( "%d,%le,%le\n", p, gflops / dtime_best, diff );
    fflush( stdout );

    free( a );
    free( b );
    free( c );
    free( cold );
    free( cref );
  }

  // printf( "];\n" );

  exit( 0 );
}