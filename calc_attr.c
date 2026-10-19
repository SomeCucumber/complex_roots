#include <stdio.h>
#include <stdint.h>

#include "calc_attr.h"

int
calc_roots(
	const uint16_t degree,
	_Float32 *attractors
	)
{
	switch (degree) {
	case 1: // d=1, {re, im}
		attractors[0] = 1.;
		attractors[1] = 0.;
		break;

	case 2: // d=2, 2*{re, im}
		attractors[0] = 1.;
		attractors[1] = 0.;

		attractors[2] = -1.;
		attractors[3] = 0.;
		break;

	case 3: // d=3, 3*{re, im}
		attractors[0] = 1.;
		attractors[1] = 0.;

		attractors[2] = -0.5;
		attractors[3] = 0.866025403784;

		attractors[4] = -0.5;
		attractors[5] = -0.866025403784;
		break;

	case 4: // d=4, 4*{re, im}
		attractors[0] = 1.;
		attractors[1] = 0.;

		attractors[2] = 0.;
		attractors[3] = 1.;

		attractors[4] = -1.;
		attractors[5] = 0.;

		attractors[6] = 0.;
		attractors[7] = -1.;
		break;

	case 5: // d=5, 5*{re, im}
		attractors[0] = 1.;
		attractors[1] = 0.;

		attractors[2] = 0.309016994375;
		attractors[3] = 0.951056516295;

		attractors[4] = -0.809016994375;
		attractors[5] = 0.587785252292;

		attractors[6] = -0.809016994375;
		attractors[7] = -0.587785252292;

		attractors[8] = 0.309016994375;
		attractors[9] = -0.951056516295;
		break;

	case 6: // d=6, 6*{re, im}
		attractors[0] = 1.;
		attractors[1] = 0.;

		attractors[2] = 0.5;
		attractors[3] = 0.866025403784;

		attractors[4] = -0.5;
		attractors[5] = 0.866025403784;

		attractors[6] = -1.;
		attractors[7] = 0.;

		attractors[8] = -0.5;
		attractors[9] = -0.866025403784;

		attractors[10] = 0.5;
		attractors[11] = -0.866025403784;
		break;

	case 7: // d=7, 7*{re, im}
		attractors[0] = 1.;
		attractors[1] = 0.;

		attractors[2] = 0.623489801859;
		attractors[3] = 0.781831482468;

		attractors[4] = -0.222520933956;
		attractors[5] = 0.974927912182;

		attractors[6] = -0.900968867902;
		attractors[7] = 0.433883739118;

		attractors[8] = -0.900968867902;
		attractors[9] = -0.433883739118;

		attractors[10] = -0.222520933956;
		attractors[11] = -0.974927912182;

		attractors[12] = 0.623489801859;
		attractors[13] = -0.781831482468;
		break;

	case 8: // d=8, 8*{re, im}
		attractors[0] = 1.;
		attractors[1] = 0.;

		attractors[2] = 0.707106781187;
		attractors[3] = 0.707106781187;

		attractors[4] = 0.;
		attractors[5] = 1.;

		attractors[6] = -0.707106781187;
		attractors[7] = 0.707106781187;

		attractors[8] = -1.;
		attractors[9] = 0.;

		attractors[10] = -0.707106781187;
		attractors[11] = -0.707106781187;

		attractors[12] = 0.;
		attractors[13] = -1.;

		attractors[14] = 0.707106781187;
		attractors[15] = -0.707106781187;
		break;

	case 9: // d=9, 9*{re, im}
		attractors[0] = 1.;
		attractors[1] = 0.;

		attractors[2] = 0.766044443119;
		attractors[3] = 0.642787609687;

		attractors[4] = 0.173648177667;
		attractors[5] = 0.984807753012;

		attractors[6] = -0.5;
		attractors[7] = 0.866025403784;

		attractors[8] = -0.939692620786;
		attractors[9] = 0.342020143326;

		attractors[10] = -0.939692620786;
		attractors[11] = -0.342020143326;

		attractors[12] = -0.5;
		attractors[13] = -0.866025403784;

		attractors[14] = 0.173648177667;
		attractors[15] = -0.984807753012;

		attractors[16] = 0.766044443119;
		attractors[17] = -0.642787609687;
		break;

	default:
		fprintf(stderr, "unexpected degree\n");
		return 1;
	}
	
	return 0;
}