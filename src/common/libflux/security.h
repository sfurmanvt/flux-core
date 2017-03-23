#ifndef _FLUX_CORE_SECURITY_H
#define _FLUX_CORE_SECURITY_H

#include <stdbool.h>

typedef struct flux_sec_struct flux_sec_t;

enum {
    /* enabled security modes */
    FLUX_SEC_TYPE_PLAIN = 1,
    FLUX_SEC_TYPE_CURVE = 2,
    FLUX_SEC_TYPE_MUNGE = 4,

    /* flags */
    FLUX_SEC_FAKEMUNGE = 0x10, // testing only
    FLUX_SEC_VERBOSE = 0x20,
    FLUX_SEC_KEYGEN_FORCE = 0x40,
};

/* Create a security context.
 * The default mode depends on compilation options.
 */
flux_sec_t *flux_sec_create (int typemask, const char *confdir);
void flux_sec_destroy (flux_sec_t *c);

/* Test security modes.
 */
bool flux_sec_type_enabled (flux_sec_t *c, int tm);

/* Get config directory used by security context.
 */
const char *flux_sec_get_directory (flux_sec_t *c);

/* Generate key material for configured security modes, if applicable.
 */
int flux_sec_keygen (flux_sec_t *c);

/* Initialize security context for communication.
 */
int flux_sec_comms_init (flux_sec_t *c);

/* Enable client or server mode ZAUTH security on a zmq socket.
 * Calling these when relevant security modes are disabled is a no-op.
 */
int flux_sec_csockinit (flux_sec_t *c, void *sock);
int flux_sec_ssockinit (flux_sec_t *c, void *sock);

/* Retrieve a string describing the last error.
 * This value is valid after one of the above calls returns -1.
 * The caller should not free this string.
 */
const char *flux_sec_errstr (flux_sec_t *c);

/* Retrieve a string describing the security modes selected.
 * The caller should not free this string.
 */
const char *flux_sec_confstr (flux_sec_t *c);

/* Convert a buffer to/from a Munge credential.
 * Privacy is ensured through the use of MUNGE_OPT_UID_RESTRICTION
 * Caller must free resulting string.
 */
int flux_sec_munge (flux_sec_t *c, const char *inbuf, size_t insize,
                    char **outbuf, size_t *outsize);
int flux_sec_unmunge (flux_sec_t *c, const char *inbuf, size_t insize,
                      char **outbuf, size_t *outsize);

#endif /* _FLUX_CORE_SECURITY_H */

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
