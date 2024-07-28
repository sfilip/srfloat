#ifndef COMMON_H
#define COMMON_H

#define DOUBLE_TO_BITS(x) (*reinterpret_cast<uint64_t *>(x))
#define BITS_TO_DOUBLE(x) (*reinterpret_cast<double *>(x))

#endif