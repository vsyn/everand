/* Copyright 2020 Julian Ingram
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "everand.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SAMPLE_COUNT (256)
#define OFFS_COUNT (1024 * 1024)
#define REPS (4)

static double get_offs(everand_t (*rand_ltd)(everand_t limit), everand_t size) {
  double offs = 0;
  unsigned int i = 0;
  everand_t limit = size - 1;
  while (i < SAMPLE_COUNT) {
    offs += rand_ltd(limit) - (limit / 2.0);
    ++i;
  }
  return offs / ((double)limit);
}

static double get_mean_offs(everand_t size) {
  double sum = 0;
  unsigned int i = 0;
  while (i < OFFS_COUNT) {
    sum += get_offs(everand, size);
    ++i;
  }
  return fabs(sum / ((double)OFFS_COUNT));
}

static everand_t rand_unsafe(everand_t limit) { return rand() % (limit + 1); }

static double get_expected_mean_offs(everand_t size) {
  double sum = 0;
  unsigned int i = 0;
  unsigned int adj_size = RAND_MAX;
  while (adj_size >= size) {
    adj_size >>= 1;
  }
  while (i < OFFS_COUNT) {
    sum += get_offs(&rand_unsafe, adj_size);
    ++i;
  }
  return fabs(sum / ((double)OFFS_COUNT));
}

static double get_biased_mean_offs(everand_t size) {
  double sum = 0;
  unsigned int i = 0;
  while (i < OFFS_COUNT) {
    sum += get_offs(&rand_unsafe, size);
    ++i;
  }
  return fabs(sum / ((double)OFFS_COUNT));
}

int main() {
  everand_seed(time(NULL));

  size_t t = 0;
  while (t < REPS) {
    printf("Testing expected");
    fflush(stdout);
    double offs = get_expected_mean_offs(RAND_MAX);
    double exp = get_expected_mean_offs(RAND_MAX);
    printf(" difference from expected: %f\n", fabs(offs - exp));
    ++t;
  }

  t = 0;
  while (t < REPS) {
    printf("Testing biased");
    fflush(stdout);
    everand_t limit;
    everand_arr(&limit, sizeof(limit));
    double offs = get_biased_mean_offs(limit);
    double exp = get_expected_mean_offs(limit);
    printf(" difference from expected: %f\n", fabs(offs - exp));
    ++t;
  }

  t = 0;
  while (t < REPS) {
    printf("Testing everand");
    fflush(stdout);
    everand_t limit;
    everand_arr(&limit, sizeof(limit));
    double offs = get_mean_offs(limit);
    double exp = get_expected_mean_offs(limit);
    printf(" difference from expected: %f\n", fabs(offs - exp));
    ++t;
  }

  printf("TESTS COMPLETED\n");
  return 0;
}
