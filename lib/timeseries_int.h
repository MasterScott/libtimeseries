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

#ifndef __TIMESERIES_INT_H
#define __TIMESERIES_INT_H

#include <inttypes.h>

#include "timeseries_pub.h"

/** @file
 *
 * @brief Header file that contains the protected interface to a timeseries
 * object
 *
 * @author Alistair King
 *
 */

/* Provides a way for internal classes to iterate over the set of possible
   enabled backend IDs */
#define TIMESERIES_FOREACH_BACKEND_ID(id)                                      \
  for (id = TIMESERIES_BACKEND_ID_FIRST; id <= TIMESERIES_BACKEND_ID_LAST; id++)

#define TIMESERIES_FOREACH_ENABLED_BACKEND(timeseries, backend, id)            \
  TIMESERIES_FOREACH_BACKEND_ID(id)                                            \
  if ((backend = timeseries_get_backend_by_id(timeseries, id)) == NULL ||      \
      timeseries_backend_is_enabled(backend) == 0)                             \
    continue;                                                                  \
  else

/**
 * @name Protected Datastructures
 *
 * These data structures are available only to libtimeseries classes. Some may
 * be exposed as opaque structures by libtimeseries.h (e.g timeseries_t)
 *
 * @{ */

/** @} */

#endif /* __TIMESERIES_INT_H */
