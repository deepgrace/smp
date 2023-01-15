//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

#ifndef SMP_VERSION_HPP
#define SMP_VERSION_HPP

#define SMP_STRINGIZE(T) #T

/*
 *   SMP_VERSION_NUMBER
 *
 *   Identifies the API version of smp.
 *   This is a simple integer that is incremented by one every
 *   time a set of code changes is merged to the master branch.
 */

#define SMP_VERSION_NUMBER 7
#define SMP_VERSION_STRING "smp/" SMP_STRINGIZE(SMP_VERSION)

#endif
