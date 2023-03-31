
/* Fast Fibonacci Calculator.                                           */
/* Complexity - Time: O(n), Memory: O(1).                               */
/* The x86 Assembly code of this algorithm is in the file Fast_Fib.s    */

unsigned long fastFib(unsigned int n){

    unsigned long A[3] = {0,1,1};
    
    for(unsigned int i = 3; i <= n; i++){

        A[i % 3] = A[(i - 1) % 3] + A[(i - 2) % 3];
    }

    return A[n % 3];
}

