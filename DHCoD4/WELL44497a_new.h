/* ***************************************************************************** */
/* Copyright:      Francois Panneton and Pierre L'Ecuyer, University of Montreal */
/*                 Makoto Matsumoto, Hiroshima University                        */
/* Notice:         This code can be used freely for personal, academic,          */
/*                 or non-commercial purposes. For commercial purposes,          */
/*                 please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca       */
/*                 Output transformation is modified by Shin Harase.             */
/* ***************************************************************************** */

#ifndef WELL44497A_NEW_H
#define WELL44497A_NEW_H

#define R 1391

class CWELLRNG44497
{
public:
	CWELLRNG44497();
	void InitWELLRNG44497(unsigned int *);
	unsigned int (*WELLRNG44497)(CWELLRNG44497 *r);
	unsigned int STATE[R];
	unsigned int z0,z1,z2;
	int state_i;
};

#endif
