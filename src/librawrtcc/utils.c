#include <stdio.h> // sprintf
#include <string.h> // strlen
#include <stdarg.h> // va_*
#include <rawrtcc.h>
#include "utils.h"

/*
 * Translate a rawrtc return code to a string.
 */
char const* rawrtc_code_to_str(
        enum rawrtc_code const code
) {
    switch (code) {
        case RAWRTC_CODE_UNKNOWN_ERROR:
            return "unknown error";
        case RAWRTC_CODE_NOT_IMPLEMENTED:
            return "not implemented";
        case RAWRTC_CODE_SUCCESS:
            return "success";
        case RAWRTC_CODE_INITIALISE_FAIL:
            return "failed to initialise";
        case RAWRTC_CODE_INVALID_ARGUMENT:
            return "invalid argument";
        case RAWRTC_CODE_NO_MEMORY:
            return "no memory";
        case RAWRTC_CODE_INVALID_STATE:
            return "invalid state";
        case RAWRTC_CODE_UNSUPPORTED_PROTOCOL:
            return "unsupported protocol";
        case RAWRTC_CODE_UNSUPPORTED_ALGORITHM:
            return "unsupported algorithm";
        case RAWRTC_CODE_NO_VALUE:
            return "no value";
        case RAWRTC_CODE_NO_SOCKET:
            return "no socket";
        case RAWRTC_CODE_INVALID_CERTIFICATE:
            return "invalid certificate";
        case RAWRTC_CODE_INVALID_FINGERPRINT:
            return "invalid fingerprint";
        case RAWRTC_CODE_INSUFFICIENT_SPACE:
            return "insufficient space";
        case RAWRTC_CODE_STILL_IN_USE:
            return "still in use";
        case RAWRTC_CODE_INVALID_MESSAGE:
            return "invalid message";
        case RAWRTC_CODE_MESSAGE_TOO_LONG:
            return "message too long";
        case RAWRTC_CODE_TRY_AGAIN_LATER:
            return "try again later";
        case RAWRTC_CODE_STOP_ITERATION:
            return "stop iteration";
        case RAWRTC_CODE_NOT_PERMITTED:
            return "not permitted";
        default:
            return "(no error translation)";
    }
}

/*
 * Translate an re error to a rawrtc code.
 * TODO: Add codes from trice_lcand_add
 */
enum rawrtc_code rawrtc_error_to_code(
        int const code
) {
    switch (code) {
        case 0:
            return RAWRTC_CODE_SUCCESS;
        case EAGAIN:
#if (EAGAIN != EWOULDBLOCK)
        case EWOULDBLOCK:
#endif
            return RAWRTC_CODE_TRY_AGAIN_LATER;
        case EAUTH:
            return RAWRTC_CODE_INVALID_CERTIFICATE;
        case EBADMSG:
            return RAWRTC_CODE_INVALID_MESSAGE;
        case EINVAL:
            return RAWRTC_CODE_INVALID_ARGUMENT;
        case EMSGSIZE:
            return RAWRTC_CODE_MESSAGE_TOO_LONG;
        case ENOMEM:
            return RAWRTC_CODE_NO_MEMORY;
        case EPERM:
            return RAWRTC_CODE_NOT_PERMITTED;
        default:
            return RAWRTC_CODE_UNKNOWN_ERROR;
    }
}

/*
 * Duplicate a string.
 * `*destinationp` will be set to a copy of `source` and must be
 * unreferenced.
 */
enum rawrtc_code rawrtc_strdup(
        char** const destinationp,
        char const * const source
) {
    int err = str_dup(destinationp, source);
    return rawrtc_error_to_code(err);
}

/*
 * Print a formatted string to a buffer.
 */
enum rawrtc_code rawrtc_snprintf(
        char* const destinationp,
        size_t const size,
        char* const formatter,
        ...
) {
    va_list args;
    va_start(args, formatter);
    int err = re_vsnprintf(destinationp, size, formatter, args);
    va_end(args);

    // For some reason, re_vsnprintf does return -1 on argument error
    switch (err) {
        case -1:
            return RAWRTC_CODE_INVALID_ARGUMENT;
        default:
            return rawrtc_error_to_code(err);
    }
}

/*
 * Print a formatted string to a dynamically allocated buffer.
 * `*destinationp` must be unreferenced.
 */
enum rawrtc_code rawrtc_sdprintf(
        char** const destinationp,
        char* const formatter,
        ...
) {
    va_list args;
    va_start(args, formatter);
    int err = re_vsdprintf(destinationp, formatter, args);
    va_end(args);
    return rawrtc_error_to_code(err);
}

/*
 * Convert binary to hex string where each value is separated by a
 * colon.
 */
enum rawrtc_code rawrtc_bin_to_colon_hex(
        char** const destinationp, // de-referenced
        uint8_t* const source,
        size_t const length
) {
    char* hex_str;
    char* hex_ptr;
    size_t i;
    int ret;
    enum rawrtc_code error = RAWRTC_CODE_SUCCESS;

    // Check arguments
    if (!destinationp || !source) {
        return RAWRTC_CODE_INVALID_ARGUMENT;
    }

    // Allocate hex string
    hex_str = mem_zalloc(length > 0 ? (length * 3) : 1, NULL);
    if (!hex_str) {
        return RAWRTC_CODE_NO_MEMORY;
    }

    // Bin to hex
    hex_ptr = hex_str;
    for (i = 0; i < length; ++i) {
        if (i > 0) {
            *hex_ptr = ':';
            ++hex_ptr;
        }
        ret = sprintf(hex_ptr, "%02X", source[i]);
        if (ret != 2) {
            error = RAWRTC_CODE_UNKNOWN_ERROR;
            goto out;
        } else {
            hex_ptr += ret;
        }
    }

out:
    if (error) {
        mem_deref(hex_str);
    } else {
        // Set pointer
        *destinationp = hex_str;
    }
    return error;
}

/*
 * Convert hex string with colon-separated hex values to binary.
 */
enum rawrtc_code rawrtc_colon_hex_to_bin(
        size_t* const bytes_written, // de-referenced
        uint8_t* const buffer, // written into
        size_t const buffer_size,
        char* source
) {
    size_t hex_length;
    size_t bin_length;
    size_t i;

    // Check arguments
    if (!bytes_written || !buffer || !source) {
        return RAWRTC_CODE_INVALID_ARGUMENT;
    }

    // Validate length
    hex_length = strlen(source);
    if (hex_length > 0 && hex_length % 3 != 2) {
        return RAWRTC_CODE_INVALID_ARGUMENT;
    }

    // Determine size
    bin_length = hex_length > 0 ? (size_t) ((hex_length + 1) / 3) : 0;
    if (bin_length > buffer_size) {
        return RAWRTC_CODE_INSUFFICIENT_SPACE;
    }

    // Hex to bin
    for (i = 0; i < bin_length; ++i) {
        if (i > 0) {
            // Skip colon
            ++source;
        }
        buffer[i] = ch_hex(*source) << 4;
        ++source;
        buffer[i] += ch_hex(*source);
        ++source;
    }

    // Done
    *bytes_written = bin_length;
    return RAWRTC_CODE_SUCCESS;
}

/*
 * Get the corresponding address family name for an DNS type.
 */
char const * const rawrtc_dns_type_to_address_family_name(
        uint_fast16_t const dns_type
) {
    switch (dns_type) {
        case DNS_TYPE_A:
            return "IPv4";
        case DNS_TYPE_AAAA:
            return "IPv6";
        default:
            return "???";
    }
}

/*
 * Destructor for an existing array container that did reference each
 * item.
 */
static void rawrtc_array_container_destroy(
        void* arg
) {
    struct rawrtc_array_container* const container = arg;
    size_t i;

    // Un-reference each item
    for (i = 0; i < container->n_items; ++i) {
        mem_deref(container->items[i]);
    }
}


/*
 * Convert a list to a dynamically allocated array container.
 *
 * If `reference` is set to `true`, each item in the list will be
 * referenced and a destructor will be added that unreferences each
 * item when unreferencing the array.
 */
enum rawrtc_code rawrtc_list_to_array(
        struct rawrtc_array_container** containerp, // de-referenced
        struct list const* const list,
        bool reference
) {
    size_t n;
    struct rawrtc_array_container* container;
    struct le* le;
    size_t i;

    // Check arguments
    if (!containerp || !list) {
        return RAWRTC_CODE_INVALID_ARGUMENT;
    }

    // Get list length
    n = list_count(list);

    // Allocate array & set length immediately
    container = mem_zalloc(
            sizeof(*container) + sizeof(void*) * n,
            reference ? rawrtc_array_container_destroy : NULL);
    if (!container) {
        return RAWRTC_CODE_NO_MEMORY;
    }
    container->n_items = n;

    // Copy pointer to each item
    for (le = list_head(list), i = 0; le != NULL; le = le->next, ++i) {
        if (reference) {
            mem_ref(le->data);
        }
        container->items[i] = le->data;
    }

    // Set pointer & done
    *containerp = container;
    return RAWRTC_CODE_SUCCESS;
}
