#include <stdlib.h>

#include "write_ppm.h"
#include "thrds.h"

void
write_row(
	const thrd_info_write_t *thrd_info_write,
	uint8_t rgb_normalize_attr,
	uint8_t rgb_normalize_iter,
	const size_t row
	)
{
	uint8_t **output_attr_raw = thrd_info_write->output_attr_raw;
  uint8_t **output_iter_raw = thrd_info_write->output_iter_raw;
  char *attr_buffer = thrd_info_write->attr_buffer;
  char *iter_buffer = thrd_info_write->iter_buffer;
  FILE *output_attr = thrd_info_write->output_attr;
  FILE *output_iter = thrd_info_write->output_iter;
  uint16_t size = thrd_info_write->size;

  uint8_t chars_per_nr = 4;
  uint8_t nrs_per_item = 3;
	for (size_t col = 0, buffer_col = 0; col < size; ++col, buffer_col += (chars_per_nr*nrs_per_item)) {
		// Attractor row:
		// R:
		uint8_t red_value = (
			output_attr_raw[row][col]
			*rgb_normalize_attr
			);
		attr_buffer[buffer_col]   = (red_value)/100 + '0';
		attr_buffer[buffer_col+1] = (red_value)/10%10 + '0';
		attr_buffer[buffer_col+2] = (red_value)%10 + '0';
		attr_buffer[buffer_col+3] = ' ';

		// G:
		attr_buffer[(buffer_col+chars_per_nr)]   = '0';
		attr_buffer[(buffer_col+chars_per_nr)+1] = '0';
		attr_buffer[(buffer_col+chars_per_nr)+2] = '0';
		attr_buffer[(buffer_col+chars_per_nr)+3] = ' ';

		// B:
		uint8_t blue_value = (
			255
			-output_attr_raw[row][col]
			*rgb_normalize_attr
			);
		attr_buffer[(buffer_col+2*chars_per_nr)]   = (blue_value)/100 + '0';
		attr_buffer[(buffer_col+2*chars_per_nr)+1] = (blue_value)/10%10 + '0';
		attr_buffer[(buffer_col+2*chars_per_nr)+2] = (blue_value)%10 + '0';
		attr_buffer[(buffer_col+2*chars_per_nr)+3] = ' ';

		// Convergence row:
		uint8_t gray_value = (
			output_iter_raw[row][col]
			*rgb_normalize_iter
			);

		// R:
		iter_buffer[buffer_col]   = (gray_value)/100 + '0';
		iter_buffer[buffer_col+1] = (gray_value)/10%10 + '0';
		iter_buffer[buffer_col+2] = (gray_value)%10 + '0';
		iter_buffer[buffer_col+3] = ' ';

		// G:
		iter_buffer[(buffer_col+chars_per_nr)]   = (gray_value)/100 + '0';
		iter_buffer[(buffer_col+chars_per_nr)+1] = (gray_value)/10%10 + '0';
		iter_buffer[(buffer_col+chars_per_nr)+2] = (gray_value)%10 + '0';
		iter_buffer[(buffer_col+chars_per_nr)+3] = ' ';

		// B:
		iter_buffer[(buffer_col+2*chars_per_nr)]   = (gray_value)/100 + '0';
		iter_buffer[(buffer_col+2*chars_per_nr)+1] = (gray_value)/10%10 + '0';
		iter_buffer[(buffer_col+2*chars_per_nr)+2] = (gray_value)%10 + '0';
		iter_buffer[(buffer_col+2*chars_per_nr)+3] = ' ';
	}

	attr_buffer[nrs_per_item*chars_per_nr*size-1] = '\n';
	iter_buffer[nrs_per_item*chars_per_nr*size-1] = '\n';
	fwrite(attr_buffer, sizeof(char), nrs_per_item*chars_per_nr*size, output_attr);
	fwrite(iter_buffer, sizeof(char), nrs_per_item*chars_per_nr*size, output_iter);
}