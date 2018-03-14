#pragma once
#include <rawrtcc_configuration.h>
#include <stdbool.h> // bool
#include <re.h>

/// Current version of the library.
///
/// Follows [Semantic Versioning 2.0.0](https://semver.org)
#define RAWRTCC_VERSION "0.0.1"

/// Return codes.
///
/// To make it easy to test for errors, the *success* return code's
/// value will always be `0`. Therefore, you can test for errors
/// in the following way:
///
///     enum rawrtc_code const error = rawrtc_some_function();
///     if (error) {
///        // Handle the error...
///     }
enum rawrtc_code {
    /// An unknown (or non-translatable) error occurred.
    RAWRTC_CODE_UNKNOWN_ERROR = -2,
    /// The necessary functionality has not been implemented.
    RAWRTC_CODE_NOT_IMPLEMENTED = -1,
    /// Success! Nothing went wrong - you're fine to proceed.
    RAWRTC_CODE_SUCCESS = 0,
    /// Initialisation failed.
    RAWRTC_CODE_INITIALISE_FAIL,
    /// Invalid argument.
    RAWRTC_CODE_INVALID_ARGUMENT,
    /// Memory could not be allocated.
    RAWRTC_CODE_NO_MEMORY,
    /// Invalid state.
    RAWRTC_CODE_INVALID_STATE,
    /// Unsupported protocol.
    RAWRTC_CODE_UNSUPPORTED_PROTOCOL,
    /// Unsupported algorithm.
    RAWRTC_CODE_UNSUPPORTED_ALGORITHM,
    /// No value has been set.
    /// @note This is often used for functions that change the value of
    ///       a variable declared outside of the function to indicate
    ///       that no change occurred.
    RAWRTC_CODE_NO_VALUE,
    /// Socket could not be found.
    RAWRTC_CODE_NO_SOCKET,
    /// Invalid certificate.
    RAWRTC_CODE_INVALID_CERTIFICATE,
    /// Invalid fingerprint.
    RAWRTC_CODE_INVALID_FINGERPRINT,
    /// Insufficient space.
    RAWRTC_CODE_INSUFFICIENT_SPACE,
    /// Target is still being used.
    RAWRTC_CODE_STILL_IN_USE,
    /// Invalid message.
    RAWRTC_CODE_INVALID_MESSAGE,
    /// Message is too long.
    RAWRTC_CODE_MESSAGE_TOO_LONG,
    /// Try again later.
    /// @note This is semantically equivalent to `EAGAIN` and
    ///       `EWOULDBLOCK`.
    RAWRTC_CODE_TRY_AGAIN_LATER,
    /// Stopped iterating (early).
    RAWRTC_CODE_STOP_ITERATION,
    /// Operation not permitted.
    RAWRTC_CODE_NOT_PERMITTED,
    /// An external function returned an error.
    RAWRTC_CODE_EXTERNAL_ERROR,
}; // IMPORTANT: Add translations for new return codes in `utils.c`!

/// Certificate private key types.
enum rawrtc_certificate_key_type {
    /// An RSA private key.
    RAWRTC_CERTIFICATE_KEY_TYPE_RSA = TLS_KEYTYPE_RSA,
    /// An elliptic curve private key.
    RAWRTC_CERTIFICATE_KEY_TYPE_EC = TLS_KEYTYPE_EC
};

/// Certificate signing hash algorithms.
enum rawrtc_certificate_sign_algorithm {
    /// Sign algorithm not set.
    /// @note When passing this as an argument, this may indicate that
    ///       a sensible default signing algorithm shall be used.
    RAWRTC_CERTIFICATE_SIGN_ALGORITHM_NONE = 0,
    /// SHA-256 sign algorithm.
    RAWRTC_CERTIFICATE_SIGN_ALGORITHM_SHA256 = TLS_FINGERPRINT_SHA256,
    /// SHA-384 sign algorithm.
    RAWRTC_CERTIFICATE_SIGN_ALGORITHM_SHA384,
    /// SHA-512 sign algorithm.
    RAWRTC_CERTIFICATE_SIGN_ALGORITHM_SHA512
};

/// Certificate encoding.
enum rawrtc_certificate_encode {
    /// Only encode the certificate.
    RAWRTC_CERTIFICATE_ENCODE_CERTIFICATE,
    /// Only encode the private key.
    RAWRTC_CERTIFICATE_ENCODE_PRIVATE_KEY,
    /// Encode both the certificate and the private key.
    RAWRTC_CERTIFICATE_ENCODE_BOTH
};



/// Certificate options.
struct rawrtc_certificate_options;

/// Certificate.
struct rawrtc_certificate;

/// Array container.
struct rawrtc_array_container {
    size_t n_items;
    void* items[];
};



/// Create certificate options.
///
/// All arguments but `key_type` are optional. Sane and safe default
/// values will be applied, don't worry!
///
/// `*optionsp` must be unreferenced.
///
/// If `common_name` is `NULL` the default common name will be applied.
/// If `valid_until` is `0` the default certificate lifetime will be
/// applied.
/// If the key type is `ECC` and `named_curve` is `NULL`, the default
/// named curve will be used.
/// If the key type is `RSA` and `modulus_length` is `0`, the default
/// amount of bits will be used. The same applies to the
/// `sign_algorithm` if it has been set to `NONE`.
enum rawrtc_code rawrtc_certificate_options_create(
    struct rawrtc_certificate_options** const optionsp, // de-referenced
    enum rawrtc_certificate_key_type const key_type,
    char* common_name, // nullable, copied
    uint_fast32_t valid_until,
    enum rawrtc_certificate_sign_algorithm sign_algorithm,
    char* named_curve, // nullable, copied, ignored for RSA
    uint_fast32_t modulus_length // ignored for ECC
);

/// Create and generate a self-signed certificate.
///
/// Sane and safe default options will be applied if `options` is
/// `NULL`.
///
/// `*certificatep` must be unreferenced.
enum rawrtc_code rawrtc_certificate_generate(
    struct rawrtc_certificate** const certificatep,
    struct rawrtc_certificate_options* options // nullable
);

/// TODO http://draft.ortc.org/#dom-rtccertificate
/// rawrtc_certificate_from_bytes
/// rawrtc_certificate_get_expires
/// rawrtc_certificate_get_fingerprint
/// rawrtc_certificate_get_algorithm

/// Translate a certificate sign algorithm to str.
char const* rawrtc_certificate_sign_algorithm_to_str(
    enum rawrtc_certificate_sign_algorithm const algorithm
);

/// Translate a str to a certificate sign algorithm (case-insensitive).
enum rawrtc_code rawrtc_str_to_certificate_sign_algorithm(
    enum rawrtc_certificate_sign_algorithm* const algorithmp, // de-referenced
    char const* const str
);



/// Translate a rawrtc return code to a string.
char const* rawrtc_code_to_str(
    enum rawrtc_code const code
);

/// Translate an re error to a rawrtc code.
enum rawrtc_code rawrtc_error_to_code(
    const int code
);

/// Duplicate a string.
/// `*destinationp` will be set to a copy of `source` and must be
/// unreferenced.
enum rawrtc_code rawrtc_strdup(
    char** const destinationp, // de-referenced
    char const* const source
);

/// Print a formatted string to a buffer.
enum rawrtc_code rawrtc_snprintf(
    char* const destinationp, // de-referenced
    size_t const size,
    char* const formatter,
    ...
);

/// Print a formatted string to a dynamically allocated buffer.
/// `*destinationp` must be unreferenced.
enum rawrtc_code rawrtc_sdprintf(
    char** const destinationp,
    char* const formatter,
    ...
);
