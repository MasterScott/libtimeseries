/*
 * tsmq
 *
 * Alistair King, CAIDA, UC San Diego
 * corsaro-info@caida.org
 *
 * Copyright (C) 2012 The Regents of the University of California.
 *
 * This file is part of tsmq.
 *
 * tsmq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tsmq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tsmq.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __TSMQ_SERVER_INT_H
#define __TSMQ_SERVER_INT_H

#include "tsmq_server.h"
#include "tsmq_int.h"

/** @file
 *
 * @brief Header file that contains the private components of the tsmq metadata
 * server.
 *
 * @author Alistair King
 *
 */

struct tsmq_server {
  /** Common tsmq state */
  tsmq_t *tsmq;

  /** Timeseries backend to write to */
  timeseries_backend_t *backend;

  /** URI to connect to the broker on */
  char *broker_uri;

  /** Socket used to connect to the broker */
  void *broker_socket;

  /** Time (in ms) between heartbeats sent to the broker */
  uint64_t heartbeat_interval;

  /** Time (in ms) to send the next heartbeat to broker */
  uint64_t heartbeat_next;

  /** The number of heartbeats that can go by before the broker is declared
      dead */
  int heartbeat_liveness;

  /** The number of beats before the broker is declared dead */
  int heartbeat_liveness_remaining;

  /** The minimum time (in ms) after a broker disconnect before we try to
      reconnect */
  uint64_t reconnect_interval_min;

  /** The maximum time (in ms) after a broker disconnect before we try to
      reconnect (after exponential back-off) */
  uint64_t reconnect_interval_max;

  /** The time before we will next attempt to reconnect */
  uint64_t reconnect_interval_next;

  /** Re-used array of keys for bulk lookups */
  char **keys;

  /** Re-used array of key IDs for bulk lookups */
  uint8_t **key_ids;

  /** Re-used array of key ID lens for bulk lookups */
  size_t *key_id_lens;

  /** Number of elements allocated in the above three arrays */
  size_t key_alloc_cnt;
};

#endif /* __TSMQ_SERVER_INT_H */
