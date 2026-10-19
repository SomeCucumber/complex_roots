#include <stdint.h>

#include "calc.h"
#include "thrds.h"

void
mul_cpx(
	_Float32 *a_re,
	_Float32 *a_im,
	_Float32 b_re,
	_Float32 b_im,
	_Float32 c_re,
	_Float32 c_im
	)
{
	*a_re = b_re * c_re - b_im * c_im;
	*a_im = b_re * c_im + b_im * c_re;
}

void
div_cpx(
	_Float32 *a_re,
	_Float32 *a_im,
	_Float32 b_re,
	_Float32 b_im,
	_Float32 c_re,
	_Float32 c_im
	)
{
	_Float32 den = c_re * c_re + c_im * c_im;
	*a_re = (b_re * c_re + b_im * c_im) / den;
	*a_im = (b_im * c_re - b_re * c_im) / den;
}

_Float32
calc_sq_dist(
	_Float32 x1,
	_Float32 y1,
	_Float32 x2,
	_Float32 y2
	)
{
	return (
		(x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)
		);
}

void
newton(
	const thrd_info_t *thrd_info,
	const size_t row
	)
{
	uint16_t size = thrd_info->size;
	uint16_t degree = thrd_info->degree;
	size_t thrd_idx = thrd_info->thrd_idx;
	mtx_t *mtx = thrd_info->mtx;
	const _Float32 *attractors = thrd_info->attractors;
	int_padded *status = thrd_info->status;

	_Float32 *input_re = thrd_info->input_re;
	_Float32 *input_im = thrd_info->input_im;
	uint8_t **output_attr_raw = thrd_info->output_attr_raw;
	uint8_t **output_iter_raw = thrd_info->output_iter_raw;

	// Trivial case of degree = 1 with only one attractor and newton method simplifies to 1;
	if (degree == 1) {
		for (size_t col = 0; col < size; ++col) {
			output_attr_raw[row][col] = 0;
			output_iter_raw[row][col] = 0;
		}
		return;
	}

	for (size_t col = 0; col < size; ++col) {
		_Float32 x1_re = input_re[col];
		_Float32 x1_im = input_im[col];
		_Float32 float_degree = (float)degree;
		bool stop_iters = false;
		size_t attr;
		size_t i;
		for (i = 0; i < MAX_ITERS && !stop_iters; ++i) {
			_Float32 x2_re = x1_re;
			_Float32 x2_im = x1_im;

			// First term:
			mul_cpx(
				&x1_re, &x1_im,
				(float_degree-1)/float_degree, 0,
				x1_re, x1_im
				);

			// Second term:
			_Float32 x2_re_for_pow = x2_re;
			_Float32 x2_im_for_pow = x2_im;
			for (int power = 0; power < degree-2; ++power) {
				mul_cpx(
					&x2_re, &x2_im,
					x2_re, x2_im,
					x2_re_for_pow, x2_im_for_pow
					);
			}
			mul_cpx(
				&x2_re, &x2_im,
				float_degree, 0,
				x2_re, x2_im
				);
			div_cpx(
				&x2_re, &x2_im,
				1, 0,
				x2_re, x2_im
				);

			// Iteration finish:
			x1_re += x2_re;
			x1_im += x2_im;

			// Check conditions:
			// Near attractor:
			for (attr = 0; attr < degree; ++attr) {
				_Float32 sq_dist = calc_sq_dist(x1_re, x1_im, attractors[2*attr], attractors[2*attr+1]);

				if (sq_dist < 1e-9) {
					stop_iters = true;
					break;
				}
			}

			// Close to origin:
			_Float32 sq_dist_origin = calc_sq_dist(x1_re, x1_im, 0, 0);
			if (sq_dist_origin < 1e-9) {
				stop_iters = true;
				attr = degree+1;
			}

			// Diverged:
			if (
				-1e5 > x1_re || x1_re > 1e5 ||
				-1e5 > x1_im || x1_im > 1e5
				) {
				stop_iters = true;
				attr = degree+2;
			}
		}

		// Save result in pixel:
		/* attr will be:
		 * - [0, degree) if an attractor was found
		 * - degree+1 if too close to origin
		 * - degree+2 if too far from origin
		 * - degree otherwise, as in, if
		 *   i >= MAX_ITERATIONS
		 */
		output_attr_raw[row][col] = attr;
		output_iter_raw[row][col] = i;
	}

	mtx_lock(mtx);
	status[thrd_idx].val = row;
	mtx_unlock(mtx);
}