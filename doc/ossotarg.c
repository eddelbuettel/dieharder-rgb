#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* To print out the full PDF, uncomment the below line */
// #define FULL_PDF


/**
 * Calculate exact statistics for the ** non-overlapping ** form of OPSO,
 * OQSO, DNA, etc.
 *
 * Algorithm: Consider adding one word. It will either hit an occupied slot
 * or an empty slot.  If the current number of occupied slots is known, the
 * calculation is trivial.
 * Parameters:  n slots, j of which are already filled.
 * Probability of hitting a filled slot is (j/n); probability of hitting an
 * empty slot is ((n-j)/n).
 *
 * Instead of knowing the number of filled slots, we have a set of
 * probabilities.
 * Parameters: n slots, prob[j] is the probability that j are filled.
 * For each value of j, the probability of both having that many slots
 * filled and hitting a filled slot is (prob[j]*j/n); the probability of
 * having j slots filled and missing them all is (prob[j]*(n-j)/n);
 *
 * The code below has two minor complications.  First, because j is used
 * as an array index, (j+1) is the number of slots filled, instead of j.
 * Second, when a miss occurs, prob[j+1] needs to be updated. But, the
 * previous value of prob[j+1] is needed for computing the effects for
 * when there were (j+1) slots already filled. Thus, the "forward" variable
 * in the code.
 *
 * Two additional optimizations are done in the code.
 * First, as samples are added, the probability that only a small number of
 * slots will be filled goes to zero.  The EPS value is a cutoff for when
 * to ignore a probability because it is too close to zero.
 * On the bottom end of the tail, min keeps track of when probabilities get
 * too small.  On the top end of the tail, what part of the tail is still
 * significant will continue to shift up as more samples are added, so the
 * code determines the top cutoff every round (the break in the inner loop).
 *
 * The osso_targ function returns a complete (discrete) PDF. (Values below
 * EPS should probably be reduced to zero.) A second function calculates
 * the mean and standard deviation from the PDF. The mean returned is the
 * mean number of empty slots, instead of the mean number of filled slots.
 *
 * There is probably a much easier way to do this.
 * In his original Monkey Tests paper, Marsaglia provides a simple approximation formula
 * for the mean (assuming non-overlapping samples), but none for the standard deviation.
 * His approximation formula is correct to XX significant figures.
 *
 */

typedef unsigned int uint;
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define EPS (1e-40)
// With k=21, n=20:
// With EPS at 1e-20, mean is right to 14 digits and sd is right to 9
// With EPS at 1e-30, mean and sd are right for all digits displayed

/*
 * Calculate the target given k input words of size n.
 * Store the result in output probs, which must be preallocated
 * and at leats of size MIN(k, n).
 */
void osso_targ(uint k, uint n, double *probs) {
	uint i, j;
	uint min = 0;
	uint len = MIN(k, n);

	/* Zero out the probabilities, and then handle the
	 * first word of input */
	memset(probs, 0, sizeof(*probs) * (len));
	probs[0] = 1;

	k--;	// First sample is done by probs[0]=1

	for (i = 1; i <= k; i++) {	/* For each new word.... */
		double forward = 0;
		uint flag = 1;
		uint limit = MIN(i, n);
		/* The limit is the maximum number of slots that can currently be
		 * filled. It is the minimum of the number of samples i and the
		 * number of slots n.
		 */

		for (j = min; j < limit; j++) {	/* For each slot that needs updating.... */
			/* Perform the updates; the next three lines are the
			 * main algorithm */
			double tmp = forward + (probs[j] * (j+1) / n);
			forward = probs[j] * (n-(j+1)) / n;
			probs[j] = tmp;

			/* Optimization: skip initial trivial stuff and ending
			 * trivial stuff.
			 */
			if (flag && tmp < EPS) min = j + 1;
			else {
				flag = 0;
				if (tmp < EPS) break;
			}
		}
		probs[j] = forward;	// Save the last update
	}
}

/*
 * Calculate the mean and standard deviation of the given PDF, assuming it
 * is of the correct form.
 * This could probably be done in one pass through the data, but it is
 * already quite fast and probably at least as numerically stable.
 *
 * The returned mean is actually (len-mean), as that fits the problem.
 */
void calcMeanAndSD(double *pdf, uint len, double *outMean, double *outSD) {
	int i;
	double sum, mean;

	/* Calculate mean: density * values */
	sum = 0;
	for (i = 0; i < len; i++) sum += pdf[i] * (i+1);
	mean = sum;

	/* Calculate variance: sum(probability * (value - mean)^2) */
	sum = 0;
	for (i = 0; i < len; i++) sum += pdf[i] * ((i+1) - mean) * ((i+1) - mean);

	*outMean=len - mean;	/* Convert to empty slots from filled slots. */
	*outSD=sqrt(sum);
}

int main() {
	double *probs;
	double mean, sd;
	uint k = 1<<21;
	uint n = 1<<20;
	uint len = MIN(k, n);
#ifdef FULL_PDF
	uint i;
#endif

	probs = (double *) malloc(sizeof(double) * len);

	osso_targ(k, n, probs);
	calcMeanAndSD(probs, len, &mean, &sd);
	printf("Mean = %.15f, sd=%.15f\n", mean, sd);

#ifdef FULL_PDF
	for (i = 0; i < len; i++) printf("%.12g\n", probs[i]);
#endif

	free(probs);

	return 0;
}

