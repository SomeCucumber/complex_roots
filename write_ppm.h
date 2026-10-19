#ifndef PPM_H
#define PPM_H

#include <stdlib.h>

#include "thrds.h"

void
write_row(
	const thrd_info_write_t *thrd_info_write,
	uint8_t rgb_normalize_attr,
	uint8_t rgb_normalize_iter,
	const size_t row
	);

#endif