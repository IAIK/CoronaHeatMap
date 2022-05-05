#pragma once

#if (__GLIBC__ >= 2) && (__GLIBC__MINOR >= 25)

#include <sys/random.h>

#else

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define SYS_getrandom 318
#define GRND_NONBLOCK 0x01
#define GRND_RANDOM 0x02

#include <sys/syscall.h>
#include <unistd.h>
int getrandom(void *buf, size_t buflen, unsigned int flags) {
  return syscall(SYS_getrandom, buf, buflen, flags);
}

#endif
