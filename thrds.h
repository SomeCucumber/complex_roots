#ifndef THRDS_H
#define THRDS_H

#define MIN_MAX 2.f

#include <stdio.h>
#include <stdint.h>
#include <threads.h>

typedef struct {
  /* This bad boy creates a new int type 
   * that simply create an empty space
   * after the int itself so that the threads
   * dont grab each others data
   */
  int val;
  char pad[60];
} int_padded;

typedef struct {
  _Float32 *input_re;
  _Float32 *input_im;
  uint8_t **output_attr_raw;
  uint8_t **output_iter_raw;
  int row_begin;
  int row_step;
  uint16_t size;
  uint16_t degree;
  size_t thrd_idx;
  mtx_t *mtx;
  cnd_t *cnd;
  const _Float32 *attractors;
  int_padded *status;
} thrd_info_t;

typedef struct {
  uint8_t **output_attr_raw;
  uint8_t **output_iter_raw;
  char *attr_buffer;
  char *iter_buffer;
  FILE *output_attr;
  FILE *output_iter;
  uint16_t size;
  uint16_t degree;
  uint16_t n_thrds;
  mtx_t *mtx;
  cnd_t *cnd;
  int_padded *status;
} thrd_info_write_t;

int
run_compute_threads(
  thrd_t *thrd,
  thrd_info_t *thrds_info
  );

int
run_write_thread(
  thrd_t *thrd_write,
  thrd_info_write_t *thrds_info_write
  );

#endif