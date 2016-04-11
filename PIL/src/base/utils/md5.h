#ifndef __MD5_H__
#define __MD5_H__

/*
 * md5.h and md5.c are based off of md5hl.c, md5c.c, and md5.h from libmd, which in turn are
 * based off the FreeBSD libmd library.  Their respective copyright notices follow:
 */

/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 */

#include <sys/types.h>

#define MD5_HASHBYTES 16

typedef struct MD5Context {
        u_int32_t buf[4];
        u_int32_t bits[2];
        unsigned char in[64];
} MD5_CTX;

char* md5_file(const char *filename, char *buf);
char* md5_data(const unsigned char *data, unsigned int len, char *buf);

#endif // end of __MD5_H__
