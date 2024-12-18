/*
    gmp-ino.h - GNU Multiple Precision (GMP) Arithmetic Library for Arduino
    Port of the mini-gmp library with support for arbitrary-precision integers and rational numbers.
    No support for floats.

    2020 Carlo Alberto Ferraris.
    This library is distributed under the terms of the GNU LGPL v3.
*/
#ifndef gmp_ino_h
#define gmp_ino_h

#include "Arduino.h"

#ifndef MINI_GMP_LIMB_TYPE
#define MINI_GMP_LIMB_TYPE int
#endif // MINI_GMP_LIMB_TYPE

#include "mini-gmp.h"

#ifndef GMP_INO_NO_RATIONAL
#include "mini-mpq.h"
#endif // GMP_INO_NO_RATIONAL

#endif // gmp_ino_h
