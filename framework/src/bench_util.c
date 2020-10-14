/****************************************************************************
**
** Copyright (C) 2015 Stiftung Secure Information and
**                    Communication Technologies SIC and
**                    Graz University of Technology
** Contact: http://opensource.iaik.tugraz.at
**
**
** Commercial License Usage
** Licensees holding valid commercial licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and SIC. For further information
** contact us at http://opensource.iaik.tugraz.at.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** This software is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this software. If not, see http://www.gnu.org/licenses/.
**
**
****************************************************************************/

#include "bench_util.h"
#include "util.h"
#include <stdio.h>

bm_time_t bm_start;
bm_time_t bm_stop;
long long bm_result;
unsigned int icache_misses;

#if ARCHITECTURE==ARCH_CORTEXM0
#define BENCH_OVERHEAD 26
#elif ARCHITECTURE==ARCH_CORTEXM33
#define BENCH_OVERHEAD 26   // TBD
#else
#define BENCH_OVERHEAD 0
#endif

/**
 * Start a benchmark.
 */
#if CACHE_PROFILING
// Reset icache misses if profiling used
void benchmark_start() {
#if (BENCHMARK_SRC == BM_CYCLE)
	util_icache_reset_misses();
	bm_start = get_cycles();
#elif (BENCHMARK_SRC == BM_CLOCK)
	util_icache_reset_misses();
	clock_gettime(CLOCK_REALTIME, &bm_start);
#endif
}
#else
void benchmark_start() {
#if (BENCHMARK_SRC == BM_CYCLE)
	bm_start = get_cycles();
#elif (BENCHMARK_SRC == BM_CLOCK)
	clock_gettime(CLOCK_REALTIME, &bm_start);
#endif
}
#endif

/**
 * Stop a benchmark.
 */
#if CACHE_PROFILING
// Cache profiling, also fetch icache misses
void benchmark_stop() {
#if (BENCHMARK_SRC == BM_CYCLE)
	bm_stop = get_cycles();
	icache_misses = util_icache_get_misses();
#elif (BENCHMARK_SRC == BM_CLOCK)
	clock_gettime(CLOCK_REALTIME, &bm_stop);
	icache_misses = util_icache_get_misses();
#endif
}
#else
void benchmark_stop() {
#if (BENCHMARK_SRC == BM_CYCLE)
	bm_stop = get_cycles();
#elif (BENCHMARK_SRC == BM_CLOCK)
	clock_gettime(CLOCK_REALTIME, &bm_stop);
#endif
}
#endif
/**
 * Compute the mean of all benchmarks for a given count of iterations.
 * @param iterations the number of benchmark iterations
 */
void benchmark_compute(int iterations) {
#if (BENCHMARK_SRC == BM_CYCLE)
	bm_result = (bm_stop - bm_start - BENCH_OVERHEAD) / iterations;
#elif (BENCHMARK_SRC == BM_CLOCK)
	bm_result = ((long)bm_stop.tv_sec - (long)bm_start.tv_sec) * 1000000000;
	bm_result += (bm_stop.tv_nsec - bm_start.tv_nsec);
	bm_result = bm_result / iterations;
#endif
}

/**
 * Computes the result of the previously executed benchmark.
 */
void benchmark_compute_single() {
#if (BENCHMARK_SRC == BM_CYCLE)
	bm_result = (bm_stop - bm_start - BENCH_OVERHEAD);
#elif (BENCHMARK_SRC == BM_CLOCK)
	bm_result = ((long)bm_stop.tv_sec - (long)bm_start.tv_sec) * 1000000000;
	bm_result += (bm_stop.tv_nsec - bm_start.tv_nsec);
#endif
}

/**
 * Print the result of the recent benchmark.
 * @param msg a message to be additionally printed
 */
void benchmark_print(const char *msg) {
	char buf[20];	// is only used in benchmark configs

#if (BENCHMARK_SRC == BM_CYCLE)
	#if CACHE_PROFILING
	sprintf(buf, "%u cycles, ", (unsigned) bm_result);
	#else
	sprintf(buf, "%u cycles\n", (unsigned) bm_result);
	#endif
#elif  (BENCHMARK_SRC == BM_CLOCK)
	#if CACHE_PROFILING
	sprintf(buf, "%lld ns, ", bm_result);
	#else
	sprintf(buf, "%lld ns\n", bm_result);
	#endif
#endif

	print(msg);
	print(buf);
}

#if CACHE_PROFILING
// Prints cache misses
void icache_miss_print(){
	char buf[20];
	sprintf(buf, "%u misses\n", icache_misses);
	print(buf);
}
#endif
