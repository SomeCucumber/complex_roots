#include <stdio.h>
#include <stdlib.h>

#include "args.h"

int
parse_args(
	const int argc,
	const char *argv[],
	args_t *flag_ptr
	)
{
	if (argc != 4) {
	fprintf(
		stderr,
		"need flags (max threads) '-t(int)', (resolution, square) '-l(int)' and (positional) <degree>\n"
		);
	return 1;
	}

	for (size_t i = 1; i < 3; ++i) {
		if (argv[i][0] == '-' && argv[i][1] == 't') {
			flag_ptr->threads = strtol(argv[i]+2, NULL, 10);
		}
		else if (argv[i][0] == '-' && argv[i][1] == 'l') {
			flag_ptr->resolution = strtol(argv[i]+2, NULL, 10);
		}
		else {
			fprintf(
				stderr,
				"invalid flag: %s\n",
				argv[i]
				);
			return 1;
		}
	}
	flag_ptr->degree = strtol(argv[3], NULL, 10);

	if (
		flag_ptr->resolution > MAX_RESOLUTION
		||
		flag_ptr->resolution < 2
		||
		flag_ptr->degree > MAX_DEGREE
		) {
		fprintf(
			stderr,
			"max degree: %i, max resolution: %i\n",
			MAX_DEGREE,
			MAX_RESOLUTION
			);
		return 1;
	}
	if (flag_ptr->resolution < flag_ptr->threads) {
		fprintf(
			stderr,
			"size has to be larger or equal than number of threads\n");
		return 1;
	}

	return 0;
}