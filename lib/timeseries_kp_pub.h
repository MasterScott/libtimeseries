/*
 * libtimeseries
 *
 * Alistair King, CAIDA, UC San Diego
 * corsaro-info@caida.org
 *
 * Copyright (C) 2012 The Regents of the University of California.
 *
 * This file is part of libtimeseries.
 *
 * libtimeseries is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libtimeseries is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libtimeseries.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef __TIMESERIES_KP_PUB_H
#define __TIMESERIES_KP_PUB_H

#include <stdlib.h>
#include <stdint.h>

#include <timeseries_pub.h>

/** @file
 *
 * @brief Header file that exposes the public interface of a Key Package object
 *
 * @author Alistair King
 *
 */

/**
 * @name Public Opaque Data Structures
 *
 * @{ */

/** Opaque struct holding state for a timeseries key package */
typedef struct timeseries_kp timeseries_kp_t;

/** @} */

/**
 * @name Public Data Structures
 *
 * @{ */


/** @} */

/**
 * @name Public Enums
 *
 * @{ */

/** @} */

/** Initialize a Key Package
 *
 * @param timeseries    Pointer to the timeseries instance to associate the key
 *                      package with
 * @param reset         Should the values be reset upon a flush.
 * @return a pointer to a Key Package structure, NULL if an error occurs
 *
 * DBATS supports highly-efficient writes if the key names are known a priori,
 * and all values are inserted simultaneously. A Key Package is an easy way to
 * define a set of keys that will be updated in concert.
 *
 * Given that Corsaro operates in intervals, the most common use case will be
 * to establish a Key Package during initialization, and then either set
 * values on a per-packet basis, or, more likely, set values for all keys at
 * the end of an interval, after which the kp_flush function can
 * be used to write the key package out.
 *
 * You may safely assume that values for all keys will be initialized to 0.
 * If you know you will set a value for every key every interval then setting
 * the _reset_ parameter to 0 will improve performance slightly.
 *
 * If not all key names are known during initialization, then the
 * timeseries_kp_add_key function can be used to add keys incrementally.
 * There is currently no mechanism for removing keys.
 */
timeseries_kp_t *timeseries_kp_init(timeseries_t *timeseries, int reset);

/** Free a Key Package
 *
 * @param kp_p          Double pointer to the Key Package to free
 */
void timeseries_kp_free(timeseries_kp_t **kp_p);

/** Add a key to an existing Key Package
 *
 * @param kp          The Key Package to add the key to
 * @param key         String containing the name of the key to add
 * @return the index of the key that was added, -1 if an error occurred
 */
int timeseries_kp_add_key(timeseries_kp_t *kp, const char *key);

/** Set the current value for the given key in a Key Package
 *
 * @param kp            Pointer to the KP to set the value on
 * @param key           Index of the key (as returned by kp_add_key) to
 *                      set the value for
 * @param value         Value to set the key to
 */
void timeseries_kp_set(timeseries_kp_t *kp, uint32_t key, uint64_t value);

/** Flush the current values in the given Key Package to all enabled backends
 *
 * @param kp            Pointer to the KP to flush values for
 * @param time          The timestamp to associate the values with in the DB
 * @return 0 if the data was written successfully, -1 otherwise.
 *
 * @note this will only flush to those backends enabled when the KP was created
 */
int timeseries_kp_flush(timeseries_kp_t *kp, uint32_t time);

#endif /* __TIMESERIES_KP_PUB_H */