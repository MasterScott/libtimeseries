/*
 * libtimeseries
 *
 * Alistair King, CAIDA, UC San Diego
 * corsaro-info@caida.org
 *
 * Copyright (C) 2012 The Regents of the University of California.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "config.h"

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <wandio.h>

#include "utils.h"

#include "timeseries_backend_int.h"
#include "timeseries_kp_int.h"
#include "timeseries_log_int.h"
#include "timeseries_backend_ascii.h"

#define BACKEND_NAME "ascii"

#define DEFAULT_COMPRESS_LEVEL 6

#define STATE(provname) (TIMESERIES_BACKEND_STATE(ascii, provname))

/** The basic fields that every instance of this backend have in common */
static timeseries_backend_t timeseries_backend_ascii = {
  TIMESERIES_BACKEND_ID_ASCII, BACKEND_NAME,
  TIMESERIES_BACKEND_GENERATE_PTRS(ascii)};

/** Holds the state for an instance of this backend */
typedef struct timeseries_backend_ascii_state {
  /** The filename to write metrics out to */
  char *ascii_file;

  /** A wandio output file pointer to write metrics to */
  iow_t *outfile;

  /** The compression level to use of the outfile is compressed */
  int compress_level;

  /** The number of values received for the current bulk set */
  uint32_t bulk_cnt;

  /** The time for the current bulk set */
  uint32_t bulk_time;

  /** The expected number of values in the current bulk set */
  uint32_t bulk_expect;

} timeseries_backend_ascii_state_t;

/** Print usage information to stderr */
static void usage(timeseries_backend_t *backend)
{
  fprintf(stderr,
          "backend usage: %s [-c compress-level] [-f output-file]\n"
          "       -c <level>    output compression level to use (default: %d)\n"
          "       -f            file to write ASCII timeseries metrics to\n",
          backend->name, DEFAULT_COMPRESS_LEVEL);
}

/** Parse the arguments given to the backend */
static int parse_args(timeseries_backend_t *backend, int argc, char **argv)
{
  timeseries_backend_ascii_state_t *state = STATE(backend);
  int opt;

  assert(argc > 0 && argv != NULL);

  /* NB: remember to reset optind to 1 before using getopt! */
  optind = 1;

  /* remember the argv strings DO NOT belong to us */

  while ((opt = getopt(argc, argv, ":c:f:?")) >= 0) {
    switch (opt) {
    case 'c':
      state->compress_level = atoi(optarg);
      break;

    case 'f':
      state->ascii_file = strdup(optarg);
      break;

    case '?':
    case ':':
    default:
      usage(backend);
      return -1;
    }
  }

  return 0;
}

/* ===== PUBLIC FUNCTIONS BELOW THIS POINT ===== */

timeseries_backend_t *timeseries_backend_ascii_alloc()
{
  return &timeseries_backend_ascii;
}

int timeseries_backend_ascii_init(timeseries_backend_t *backend, int argc,
                                  char **argv)
{
  timeseries_backend_ascii_state_t *state;

  /* allocate our state */
  if ((state = malloc_zero(sizeof(timeseries_backend_ascii_state_t))) == NULL) {
    timeseries_log(__func__,
                   "could not malloc timeseries_backend_ascii_state_t");
    return -1;
  }
  timeseries_backend_register_state(backend, state);

  /* set initial default values (that can be overridden on the command line) */
  state->compress_level = DEFAULT_COMPRESS_LEVEL;

  /* parse the command line args */
  if (parse_args(backend, argc, argv) != 0) {
    return -1;
  }

  /* if specified, open the output file */
  if (state->ascii_file != NULL &&
      (state->outfile = wandio_wcreate(
         state->ascii_file, wandio_detect_compression_type(state->ascii_file),
         state->compress_level, O_CREAT)) == NULL) {
    timeseries_log(__func__, "failed to open output file '%s'",
                   state->ascii_file);
    return -1;
  }

  /* ready to rock n roll */

  return 0;
}

void timeseries_backend_ascii_free(timeseries_backend_t *backend)
{
  timeseries_backend_ascii_state_t *state = STATE(backend);
  if (state != NULL) {
    if (state->ascii_file != NULL) {
      free(state->ascii_file);
      state->ascii_file = NULL;
    }

    if (state->outfile != NULL) {
      wandio_wdestroy(state->outfile);
      state->outfile = NULL;
    }

    timeseries_backend_free_state(backend);
  }
  return;
}

int timeseries_backend_ascii_kp_init(timeseries_backend_t *backend,
                                     timeseries_kp_t *kp, void **kp_state_p)
{
  /* we do not need any state */
  assert(kp_state_p != NULL);
  *kp_state_p = NULL;
  return 0;
}

void timeseries_backend_ascii_kp_free(timeseries_backend_t *backend,
                                      timeseries_kp_t *kp, void *kp_state)
{
  /* we did not allocate any state */
  assert(kp_state == NULL);
  return;
}

int timeseries_backend_ascii_kp_ki_update(timeseries_backend_t *backend,
                                          timeseries_kp_t *kp)
{
  /* we don't need to do anything */
  return 0;
}

void timeseries_backend_ascii_kp_ki_free(timeseries_backend_t *backend,
                                         timeseries_kp_t *kp,
                                         timeseries_kp_ki_t *ki, void *ki_state)
{
  /* we did not allocate any state */
  assert(ki_state == NULL);
  return;
}

#define PRINT_METRIC(func, file, key, value, time)                             \
  do {                                                                         \
    func(file, "%s %" PRIu64 " %s\n", key, value, time);                       \
  } while (0)

#define DUMP_METRIC(state, key, value, time)                                   \
  do {                                                                         \
    if (state->outfile != NULL) {                                              \
      PRINT_METRIC(wandio_printf, state->outfile, key, value, time);           \
    } else {                                                                   \
      PRINT_METRIC(fprintf, stdout, key, value, time);                         \
    }                                                                          \
  } while (0)

int timeseries_backend_ascii_kp_flush(timeseries_backend_t *backend,
                                      timeseries_kp_t *kp, uint32_t time)
{
  timeseries_backend_ascii_state_t *state = STATE(backend);
  timeseries_kp_ki_t *ki = NULL;
  int id;

  /* there are at most 10 digits in a 32bit unix time value, plus the nul */
  char time_buffer[11];

  /* we really only need to convert the time value to a string once */
  snprintf(time_buffer, 11, "%" PRIu32, time);

  TIMESERIES_KP_FOREACH_KI(kp, ki, id)
  {
    if (timeseries_kp_ki_enabled(ki) != 0) {
      DUMP_METRIC(state, timeseries_kp_ki_get_key(ki),
                  timeseries_kp_ki_get_value(ki), time_buffer);
    }
  }

  return 0;
}

int timeseries_backend_ascii_set_single(timeseries_backend_t *backend,
                                        const char *key, uint64_t value,
                                        uint32_t time)
{
  timeseries_backend_ascii_state_t *state = STATE(backend);

  /* there are at most 10 digits in a 32bit unix time value, plus the nul */
  char time_buffer[11];

  /* we really only need to convert the time value to a string once */
  snprintf(time_buffer, 11, "%" PRIu32, time);

  DUMP_METRIC(state, key, value, time_buffer);
  return 0;
}

int timeseries_backend_ascii_set_single_by_id(timeseries_backend_t *backend,
                                              uint8_t *id, size_t id_len,
                                              uint64_t value, uint32_t time)
{
  /* the ascii backend ID is just the key, decode and call set single */
  return timeseries_backend_ascii_set_single(backend, (char *)id, value, time);
}

int timeseries_backend_ascii_set_bulk_init(timeseries_backend_t *backend,
                                           uint32_t key_cnt, uint32_t time)
{
  timeseries_backend_ascii_state_t *state = STATE(backend);

  assert(state->bulk_expect == 0 && state->bulk_cnt == 0);
  state->bulk_expect = key_cnt;
  state->bulk_time = time;
  return 0;
}

int timeseries_backend_ascii_set_bulk_by_id(timeseries_backend_t *backend,
                                            uint8_t *id, size_t id_len,
                                            uint64_t value)
{
  timeseries_backend_ascii_state_t *state = STATE(backend);
  assert(state->bulk_expect > 0);

  if (timeseries_backend_ascii_set_single_by_id(backend, id, id_len, value,
                                                state->bulk_time) != 0) {
    return -1;
  }

  if (++state->bulk_cnt == state->bulk_expect) {
    state->bulk_cnt = 0;
    state->bulk_time = 0;
    state->bulk_expect = 0;
  }
  return 0;
}

size_t timeseries_backend_ascii_resolve_key(timeseries_backend_t *backend,
                                            const char *key,
                                            uint8_t **backend_key)
{
  if ((*backend_key = (uint8_t *)strdup(key)) == NULL) {
    return 0;
  }
  return strlen(key) + 1;
}

int timeseries_backend_ascii_resolve_key_bulk(
  timeseries_backend_t *backend, uint32_t keys_cnt, const char *const *keys,
  uint8_t **backend_keys, size_t *backend_key_lens, int *contig_alloc)
{
  int i;

  for (i = 0; i < keys_cnt; i++) {
    if ((backend_key_lens[i] = timeseries_backend_ascii_resolve_key(
           backend, keys[i], &(backend_keys[i]))) == 0) {
      timeseries_log(__func__, "Could not resolve key ID");
      return -1;
    }
  }

  assert(contig_alloc != NULL);
  *contig_alloc = 0;

  return 0;
}
