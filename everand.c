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

#if (UCHAR_MAX > RAND_MAX)
#error "UCHAR_MAX > RAND_MAX"
#endif

#if (UCHAR_MAX >= INT_MAX)
#error "(UCHAR_MAX + 1) > INT_MAX"
#endif

static const everand_t MAX = -1;

static unsigned char everand_uchar_ul(void) {
#if ((RAND_MAX % (UCHAR_MAX + 1)) == UCHAR_MAX)
  /* no modulo bias */
  return rand() % (UCHAR_MAX + 1);
#else
  int bias_free_from = RAND_MAX % (UCHAR_MAX + 1);
  int r;
  /* loop until we get a number in the bias free range */
  do {
    r = rand();
  } while (r <= bias_free_from);
  return r % (UCHAR_MAX + 1);
#endif
}

void everand_seed(unsigned int seed) { srand(seed); }

void everand_arr(void *arr, size_t size) {
  unsigned char *p = arr;
  unsigned char *end = p + size;
  while (p != end) {
    *p++ = everand_uchar_ul();
  }
}

everand_t everand(everand_t limit) {
  everand_t r = 0;
  everand_arr(&r, sizeof(r));
  if (limit != MAX) {
    ++limit;
    everand_t bias_free_from = MAX % limit;
    while (r <= bias_free_from) {
      everand_arr(&r, sizeof(r));
    }
    r %= limit;
  }
  return r;
}
