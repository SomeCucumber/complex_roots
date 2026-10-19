#ifndef NWTN_H
#define NWTN_H

#include <stdlib.h>

#include "thrds.h"

#define MAX_ITERS 50

void
newton(
	const thrd_info_t *thrd_info,
	const size_t row
	);

#endif