#ifndef ARGS_H
#define ARGS_H

#define MAX_DEGREE 9
#define MAX_RESOLUTION 9999

typedef struct {
	long threads;
	long resolution;
	long degree;
} args_t;

int
parse_args(
	const int argc,
	const char *argv[],
	args_t *flag_ptr
	);

#endif