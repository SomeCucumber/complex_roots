#include <stdio.h>
#include <stdint.h>
#include <threads.h>

#include "thrds.h"
#include "calc.h"
#include "write_ppm.h"

int
main_thrd(
	void *args
	)
{
	const thrd_info_t *thrd_info = (thrd_info_t*) args;

	_Float32 *input_re = thrd_info->input_re;
	_Float32 *input_im = thrd_info->input_im;
	uint8_t **output_attr_raw = thrd_info->output_attr_raw;
	uint8_t **output_iter_raw = thrd_info->output_iter_raw;
	int row_begin = thrd_info->row_begin;
	int row_step = thrd_info->row_step;
	uint16_t size = thrd_info->size;
	size_t thrd_idx = thrd_info->thrd_idx;
	mtx_t *mtx = thrd_info->mtx;
	cnd_t *cnd = thrd_info->cnd;
	int_padded *status = thrd_info->status;

	for (size_t row = row_begin; row < size; row += row_step) {
		/* Allocate one output row per output, will be
		 * linked to the raw output and freed by the
		 * writing thread.
		 */
		uint8_t *output_row_attr = malloc(size * sizeof(*output_row_attr));
		output_attr_raw[row] = output_row_attr;
		uint8_t *output_row_iter = malloc(size * sizeof(*output_row_iter));
		output_iter_raw[row] = output_row_iter;

		// Initialise input row:
		{
			_Float32 delta = 2*MIN_MAX/(size-1);
			for (size_t col = 0; col < size; ++col) {
				input_re[col] = -MIN_MAX + col*delta;
				input_im[col] = -MIN_MAX + row*delta;
			}
		}

		// Calculate row:
		newton(thrd_info, row);

		mtx_lock(mtx);
		status[thrd_idx].val = row + row_step;
		mtx_unlock(mtx);
		cnd_signal(cnd);
	}

	return 0;
}

int
main_thrd_write(
	void *args
	)
{
	const thrd_info_write_t *thrd_info_write = (thrd_info_write_t*) args;

	uint8_t **output_attr_raw = thrd_info_write->output_attr_raw;
  uint8_t **output_iter_raw = thrd_info_write->output_iter_raw;
  uint16_t size = thrd_info_write->size;
  uint16_t degree = thrd_info_write->degree;
  uint16_t n_thrds = thrd_info_write->n_thrds;
  mtx_t *mtx = thrd_info_write->mtx;
  cnd_t *cnd = thrd_info_write->cnd;
  int_padded *status = thrd_info_write->status;

  for (size_t row = 0, earliest_finished_row; row < size; ) {
  	for (mtx_lock(mtx); ; ) {
  		/* This loop:
  		 * - If the mutex is not locked, lock it and calculate
  		 *   earliest finished row.
  		 * - If earliest_finished_row == row => writer is up to date.
  		 * - Otherwise, unlock the mutex and continue to next loop
  		 *   where row is incremented as progress is made.
  		 */
  		earliest_finished_row = size;
  		for (size_t thrd_idx = 0; thrd_idx < n_thrds; ++thrd_idx) {
  			if (earliest_finished_row > status[thrd_idx].val) {
  				earliest_finished_row = status[thrd_idx].val;
  			}
  		}

  		if (earliest_finished_row == row) {
  			cnd_wait(cnd, mtx);
  		}
  		else {
  			mtx_unlock(mtx);
  			break;
  		}
  	}

  	uint8_t rgb_normalize_attr = 255/degree;
  	uint8_t rgb_normalize_iter = 255/MAX_ITERS;
  	for (; row < earliest_finished_row; ++row) {
  		write_row(
  			thrd_info_write,
  			rgb_normalize_attr,
  			rgb_normalize_iter,
  			row
  			);
  		free(output_attr_raw[row]);
  		free(output_iter_raw[row]);
  	}
  }
	
	return 0;
}

int
run_compute_threads(
  thrd_t *thrd,
  thrd_info_t *thrd_info
	)
{
	int thrd_error = thrd_create(
		thrd,
		main_thrd,
		(void*) (thrd_info)
		);
	if (thrd_error != thrd_success) {
		fprintf(stderr, "failed to create thread\n");
		return thrd_error;
	}
	thrd_detach(*thrd);

	return 0;
}

int
run_write_thread(
  thrd_t *thrd_write,
  thrd_info_write_t *thrd_info_write
	)
{
	int thrd_error = thrd_create(
		thrd_write,
		main_thrd_write,
		(void*) (thrd_info_write)
		);
	if (thrd_error != thrd_success) {
		fprintf(stderr, "failed to create thread\n");
		return thrd_error;
	}

	return 0;
}

