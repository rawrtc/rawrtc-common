#pragma once
#include <rawrtcc.h>

/*
 * Handle buffered messages.
 *
 * Return `true` if the message has been handled successfully and can
 * be unlinked, `false` to stop processing messages and keep the current
 * message in the list.
 */
typedef bool (rawrtc_message_buffer_handler)(
    struct mbuf* const buffer,
    void* const context,
    void* const arg
);

/*
 * Message buffer.
 */
struct rawrtc_buffered_message {
    struct le le;
    struct mbuf* buffer; // referenced
    void* context; // referenced, nullable
};

enum rawrtc_code rawrtc_message_buffer_append(
    struct list* const message_buffer,
    struct mbuf* const buffer, // referenced
    void* const context // referenced, nullable
);

enum rawrtc_code rawrtc_message_buffer_clear(
    struct list* const message_buffer,
    rawrtc_message_buffer_handler* const message_handler,
    void* arg
);
