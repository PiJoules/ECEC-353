/*  Purpose: Calculate definite integral using the trapezoidal rule.
 *
 * Input:   a, b, n
 * Output:  Estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Author: Naga Kandasamy, Michael Lui
 * Date: 6/22/2016
 *
 * Compile: gcc -o trap trap.c -lpthread -lm
 * Usage:   ./trap
 *
 *
 * Note:    The function f(x) is hardwired.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <pthread.h>

#define LEFT_ENDPOINT 5
#define RIGHT_ENDPOINT 1000
#define NUM_TRAPEZOIDS 100000000
#define NUM_THREADs 4 /* Number of threads to run. */


/*------------------------------------------------------------------
 * Function:    func
 * Purpose:     Compute value of function to be integrated
 * Input args:  x
 * Output: (x+1)/sqrt(x*x + x + 1)
 */
__attribute__((const)) float func(float x) 
{
	return (x + 1)/sqrt(x*x + x + 1);
}  


double compute_gold(float, float, int, float (*)(float));
double compute_using_pthreads(float, float, int, float (*)(float));

int main(int argc, char *argv[])
{
	int n = NUM_TRAPEZOIDS;
	float a = LEFT_ENDPOINT;
	float b = RIGHT_ENDPOINT;
	
	double reference = compute_gold(a, b, n, func);
	printf("Reference solution computed on the CPU = %f \n", reference);
	
	double pthread_result = compute_using_pthreads(a, b, n, func); /* Write this function using pthreads. */
	printf("Solution computed using pthreads = %f \n", pthread_result);
} 

/*------------------------------------------------------------------
 * Function:    Trap
 * Purpose:     Estimate integral from a to b of f using trap rule and
 *              n trapezoids
 * Input args:  a, b, n, f
 * Return val:  Estimate of the integral 
 */
double compute_gold(float a, float b, int n, float(*f)(float))
{
	float h = (b-a)/(float)n; /* 'Height' of each trapezoid. */
	double integral = (f(a) + f(b))/2.0;
	
	for (int k = 1; k <= n-1; k++) 
		integral += f(a+k*h);
	
	integral = integral*h;
	
	return integral;
}  

struct fargs {
    float a;
    float b;
    int n;
    float (*f)(float);
    double result;
};

void* compute_gold_wrapper(void* args){
    struct fargs function_args = *((struct fargs*) args);
    ((struct fargs*) args)->result = compute_gold(
                 function_args.a,
                 function_args.b,
                 function_args.n,
                 function_args.f);
    return NULL;
}

double compute_using_pthreads(float a, float b, int n, float(*f)(float))
{
    // Split up ranges
    struct fargs args[NUM_THREADs];
    float width = fabsf(b - a) / NUM_THREADs;
    pthread_t pths[NUM_THREADs];
    for (size_t i = 0; i < NUM_THREADs; i++){
        args[i].a = a + i*width;
        args[i].b = a + (i+1)*width;
        args[i].n = n / NUM_THREADs;
        args[i].f = f;
        args[i].result = 0xdead;

        pthread_t pth;
        pths[i] = pth;
        pthread_create(&pths[i], NULL, compute_gold_wrapper, &args[i]);
    }

    double sum = 0;
    for (size_t i = 0; i < NUM_THREADs; i++){
        pthread_join(pths[i], NULL);
        sum += args[i].result;
    }

    return sum;
}  

