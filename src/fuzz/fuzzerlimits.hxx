/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_FUZZERLIMITS_HXX
#define INCLUDED_FUZZERLIMITS_HXX

#include <cstddef>

// The largest input the fuzzers parse. It is above the size of the largest Keynote seed. A gzip
// stream this size inflates to at most about 675 MB, so a parse stays within 2 GB of memory.
constexpr std::size_t maxFuzzInputSize = 640 * 1024;

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
