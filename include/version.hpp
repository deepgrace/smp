//
// Copyright (c) 2016-present DeepGrace (complex dot invoke at gmail dot com)
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
 *   Identifies the API version of Smp.
 *   This is a simple integer that is incremented by one every
 *   time a set of code changes is merged to the master branch.
 */

#define SMP_VERSION_NUMBER 1
#define SMP_VERSION_STRING "Smp/" SMP_STRINGIZE(SMP_VERSION)

#endif
