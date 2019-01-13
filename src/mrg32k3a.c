/**
 * 32-bits Random number generator U(0,1): MRG32k3a
 * Author: Pierre L'Ecuyer,
 * Source: 	Good Parameter Sets for Combined Multiple Recursive Random
 *			Number Generators,
 *			Shorter version in Operations Research,
 *			47, 1 (1999), 159--164.
 *
 * ====================================================================
 * Update by Andriamilanto Tompoariniaina:
 *     - Code redesign and comments
 *     - The seed is now taken from the current time
**/
#include "mrg32k3a.h"


// Constants
#define NORM 2.328306549295728e-10
#define M1 4294967087.0
#define M2 4294944443.0
#define A12 1403580.0
#define A13N 810728.0
#define A21 527612.0
#define A23N 1370589.0


/**
 * Get a pseudo-random double number
 * EDIT: This uses the current time as seed
 * \return A pseudo-random double number
 */
double MRG32k3a() {

	// Update: Generate the seed from the current time
	double SEED = time(NULL);

	long k;
	double p1, p2, s10 = SEED, s11 = SEED, s12 = SEED, s20 = SEED, s21 = SEED, s22 = SEED;

	/* Component 1 */
	p1 = A12 * s11 - A13N * s10;
	k = p1 / M1;
	p1 -= k * M1;
	if (p1 < 0.0) p1 += M1;
	s10 = s11;
	s11 = s12;
	s12 = p1;

	/* Component 2 */
	p2 = A21 * s22 - A23N * s20;
	k = p2 / M2;
	p2 -= k * M2;
	if (p2 < 0.0) p2 += M2;
	s20 = s21;
	s21 = s22;
	s22 = p2;

	/* Combination */
	if (p1 <= p2) return ((p1 - p2 + M1) * NORM);
	else return ((p1 - p2) * NORM);
}
