#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <stdint.h>

#include "args.h"
#include "calc_attr.h"
#include "thrds.h"

int
main(
	const int argc,
	const char *argv[]
	)
{
	// Parse flags:
	args_t flags;
	int arg_error = parse_args(
		argc,
		argv,
		&flags
		);
	if (arg_error != 0) {return 1;}
	const uint16_t size = flags.resolution;
	const uint16_t n_thrds = flags.threads;
	const uint16_t degree = flags.degree;

	// Calculate attractors:
	_Float32 attractors[2*degree];
	int attr_error = calc_roots(degree, attractors);
	if (attr_error != 0) {return 1;}

	// Allocate inputs:
	_Float32 *input_re = malloc(size * n_thrds * sizeof(*input_re));
	_Float32 *input_im = malloc(size * n_thrds * sizeof(*input_im));

	// Allocate outputs:
	uint8_t **output_attr_raw = malloc(size * sizeof(*output_attr_raw));
	uint8_t **output_iter_raw = malloc(size * sizeof(*output_iter_raw));
	char *attr_buffer = malloc(1 + 3 * 4 * size * sizeof(*attr_buffer)); // 1+3*4* since we convert to a string and want a whitespace after each char and three values per pixel
	char *iter_buffer = malloc(1 + 3 * 4 * size * sizeof(*iter_buffer));
	attr_buffer[3 * 4 * size * sizeof(*attr_buffer)] = 0;
	iter_buffer[3 * 4 * size * sizeof(*iter_buffer)] = 0;

	// Create output ppm:
	char attr_name[64];
	char iter_name[64];
	sprintf(attr_name, "newton_attractors_x%hu.ppm", degree);
	sprintf(iter_name, "newton_convergence_x%hu.ppm", degree);
	FILE *output_attr = fopen(attr_name, "w");
	FILE *output_iter = fopen(iter_name, "w");
	if (!output_attr || !output_iter) {
		fprintf(stderr, "file creation error\n");
		return EXIT_FAILURE;
	}
	char ppm_header[20];
	int ppm_header_len = sprintf(
		ppm_header,
		"P3\n%hu %hu\n255\n",
		size,
		size
		);
	fwrite(ppm_header, sizeof(char), ppm_header_len, output_attr);
	fwrite(ppm_header, sizeof(char), ppm_header_len, output_iter);

	// Initialise threads:
	mtx_t mtx;
	mtx_init(&mtx, mtx_plain);

	cnd_t cnd;
	cnd_init(&cnd);

	int_padded status[n_thrds];

	// Compute:
	thrd_t thrds[n_thrds];
	thrd_info_t thrds_info[n_thrds];

	for (size_t thrd_idx = 0; thrd_idx < n_thrds; ++thrd_idx) {
		/* This next lines assigns one line worth of memory
		 * for input to each thread, note that the entire
		 * input is never created.
		 */
		thrds_info[thrd_idx].input_re = input_re+(size*thrd_idx);
		thrds_info[thrd_idx].input_im = input_im+(size*thrd_idx);

		thrds_info[thrd_idx].output_attr_raw 	= output_attr_raw;
		thrds_info[thrd_idx].output_iter_raw 	= output_iter_raw;
		thrds_info[thrd_idx].row_begin 				= thrd_idx;
		thrds_info[thrd_idx].row_step 				= n_thrds;
		thrds_info[thrd_idx].size 						= size;
		thrds_info[thrd_idx].degree 					= degree;
		thrds_info[thrd_idx].thrd_idx 				= thrd_idx;
		thrds_info[thrd_idx].mtx 							= &mtx;
		thrds_info[thrd_idx].cnd 							= &cnd;
		thrds_info[thrd_idx].attractors				= attractors;
		thrds_info[thrd_idx].status 					= status;
		status[thrd_idx].val 									= 0;

		int thrd_error = run_compute_threads(
			thrds+thrd_idx,
			thrds_info+thrd_idx
			);
		if (thrd_error != 0) {
			return 1;
		}
	}

	// Write:
	thrd_t thrd_write;
	thrd_info_write_t thrd_info_write;

	thrd_info_write.output_attr_raw 	= output_attr_raw;
	thrd_info_write.output_iter_raw 	= output_iter_raw;
	thrd_info_write.attr_buffer 	 		= attr_buffer;
	thrd_info_write.iter_buffer 	 		= iter_buffer;
	thrd_info_write.output_attr 	 		= output_attr;
	thrd_info_write.output_iter 	 		= output_iter;
	thrd_info_write.size 					 		= size;
	thrd_info_write.degree				 		= degree;
	thrd_info_write.n_thrds 		 			= n_thrds;
	thrd_info_write.mtx 					 		= &mtx;
	thrd_info_write.cnd 					 		= &cnd;
	thrd_info_write.status 				 		= status;

	int thrd_error = run_write_thread(
		&thrd_write,
		&thrd_info_write
		);
	if (thrd_error != 0) {
		return 1;
	}

	// Wait for write thread:
	{
		int thrd_error;
		thrd_join(thrd_write, &thrd_error);
	}

	// Cleanup:
	free(input_re);
	free(input_im);
	free(output_attr_raw);
	free(output_iter_raw);
	free(attr_buffer);
	free(iter_buffer);
	fclose(output_attr);
	fclose(output_iter);

	mtx_destroy(&mtx);
	cnd_destroy(&cnd);

	return 0;
}