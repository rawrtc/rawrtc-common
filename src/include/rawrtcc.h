#pragma once
#include <stdbool.h> // bool

// TODO: Make this a build configuration
#define RAWRTC_DEBUG_LEVEL 5

#define HAVE_INTTYPES_H
#include <re.h>

/*
 * Version
 *
 * Follows Semantic Versioning 2.0.0,
 * see: https://semver.org
 *
 * TODO: Find a way to keep this in sync with the one in CMakeLists.txt
 */
#define RAWRTCC_VERSION "0.0.1"

/*
 * Return codes.
 */
enum rawrtc_code {
    RAWRTC_CODE_UNKNOWN_ERROR = -2,
    RAWRTC_CODE_NOT_IMPLEMENTED = -1,
    RAWRTC_CODE_SUCCESS = 0,
    RAWRTC_CODE_INITIALISE_FAIL,
    RAWRTC_CODE_INVALID_ARGUMENT,
    RAWRTC_CODE_NO_MEMORY,
    RAWRTC_CODE_INVALID_STATE,
    RAWRTC_CODE_UNSUPPORTED_PROTOCOL,
    RAWRTC_CODE_UNSUPPORTED_ALGORITHM,
    RAWRTC_CODE_NO_VALUE,
    RAWRTC_CODE_NO_SOCKET,
    RAWRTC_CODE_INVALID_CERTIFICATE,
    RAWRTC_CODE_INVALID_FINGERPRINT,
    RAWRTC_CODE_INSUFFICIENT_SPACE,
    RAWRTC_CODE_STILL_IN_USE,
    RAWRTC_CODE_INVALID_MESSAGE,
    RAWRTC_CODE_MESSAGE_TOO_LONG,
    RAWRTC_CODE_TRY_AGAIN_LATER,
    RAWRTC_CODE_STOP_ITERATION,
    RAWRTC_CODE_NOT_PERMITTED,
    RAWRTC_CODE_EXTERNAL_ERROR,
}; // IMPORTANT: Add translations for new return codes in `utils.c`!

/*
 * Certificate private key types.
 */
enum rawrtc_certificate_key_type {
    RAWRTC_CERTIFICATE_KEY_TYPE_RSA = TLS_KEYTYPE_RSA,
    RAWRTC_CERTIFICATE_KEY_TYPE_EC = TLS_KEYTYPE_EC
};

/*
 * Certificate signing hash algorithms.
 */
enum rawrtc_certificate_sign_algorithm {
    RAWRTC_CERTIFICATE_SIGN_ALGORITHM_NONE = 0,
    RAWRTC_CERTIFICATE_SIGN_ALGORITHM_SHA256 = TLS_FINGERPRINT_SHA256,
    RAWRTC_CERTIFICATE_SIGN_ALGORITHM_SHA384,
    RAWRTC_CERTIFICATE_SIGN_ALGORITHM_SHA512
};

/*
 * Certificate encoding.
 */
enum rawrtc_certificate_encode {
    RAWRTC_CERTIFICATE_ENCODE_CERTIFICATE,
    RAWRTC_CERTIFICATE_ENCODE_PRIVATE_KEY,
    RAWRTC_CERTIFICATE_ENCODE_BOTH
};



/*
 * Certificate options.
 */
struct rawrtc_certificate_options;

/*
 * Certificate.
 */
struct rawrtc_certificate;

/*
 * Array container.
 */
struct rawrtc_array_container {
    size_t n_items;
    void* items[];
};



/*
 * Create certificate options.
 *
 * All arguments but `key_type` are optional. Sane and safe default
 * values will be applied, don't worry!
 *
 * `*optionsp` must be unreferenced.
 *
 * If `common_name` is `NULL` the default common name will be applied.
 * If `valid_until` is `0` the default certificate lifetime will be
 * applied.
 * If the key type is `ECC` and `named_curve` is `NULL`, the default
 * named curve will be used.
 * If the key type is `RSA` and `modulus_length` is `0`, the default
 * amount of bits will be used. The same applies to the
 * `sign_algorithm` if it has been set to `NONE`.
 */
enum rawrtc_code rawrtc_certificate_options_create(
    struct rawrtc_certificate_options** const optionsp, // de-referenced
    enum rawrtc_certificate_key_type const key_type,
    char* common_name, // nullable, copied
    uint_fast32_t valid_until,
    enum rawrtc_certificate_sign_algorithm sign_algorithm,
    char* named_curve, // nullable, copied, ignored for RSA
    uint_fast32_t modulus_length // ignored for ECC
);

/*
 * Create and generate a self-signed certificate.
 *
 * Sane and safe default options will be applied if `options` is
 * `NULL`.
 *
 * `*certificatep` must be unreferenced.
 */
enum rawrtc_code rawrtc_certificate_generate(
    struct rawrtc_certificate** const certificatep,
    struct rawrtc_certificate_options* options // nullable
);

/*
 * TODO http://draft.ortc.org/#dom-rtccertificate
 * rawrtc_certificate_from_bytes
 * rawrtc_certificate_get_expires
 * rawrtc_certificate_get_fingerprint
 * rawrtc_certificate_get_algorithm
 */

/*
 * Translate a certificate sign algorithm to str.
 */
char const * rawrtc_certificate_sign_algorithm_to_str(
    enum rawrtc_certificate_sign_algorithm const algorithm
);

/*
 * Translate a str to a certificate sign algorithm (case-insensitive).
 */
enum rawrtc_code rawrtc_str_to_certificate_sign_algorithm(
    enum rawrtc_certificate_sign_algorithm* const algorithmp, // de-referenced
    char const* const str
);



/*
 * Translate a rawrtc return code to a string.
 */
char const* rawrtc_code_to_str(
    enum rawrtc_code const code
);

/*
 * Translate an re error to a rawrtc code.
 */
enum rawrtc_code rawrtc_error_to_code(
    const int code
);

/*
 * Duplicate a string.
 * `*destinationp` will be set to a copy of `source` and must be
 * unreferenced.
 */
enum rawrtc_code rawrtc_strdup(
    char** const destinationp, // de-referenced
    char const * const source
);

/*
 * Print a formatted string to a buffer.
 */
enum rawrtc_code rawrtc_snprintf(
    char* const destinationp, // de-referenced
    size_t const size,
    char* const formatter,
    ...
);

/*
 * Print a formatted string to a dynamically allocated buffer.
 * `*destinationp` must be unreferenced.
 */
enum rawrtc_code rawrtc_sdprintf(
    char** const destinationp,
    char* const formatter,
    ...
);
