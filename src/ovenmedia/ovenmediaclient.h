#ifndef OVENMEDIA_CLIENT_H
#define OVENMEDIA_CLIENT_H

#include <glib-object.h>

G_BEGIN_DECLS

#define OVENMEDIA_TYPE_CLIENT (ovenmedia_client_get_type())

G_DECLARE_FINAL_TYPE(OvenMediaClient, ovenmedia_client, OVENMEDIA, CLIENT, GObject)

/**
 * ovenmedia_client_set_url:
 * @self: (in): The #OvenMediaClient instance.
 * @url: (in): The WebSocket URL to connect to.
 *
 * Sets the WebSocket URL that the client will connect to.
 */
void ovenmedia_client_set_url(OvenMediaClient *self, const gchar *url);

/**
 * ovenmedia_client_connect:
 * @self: (in): The #OvenMediaClient instance.
 *
 * Initiates a connection to the WebSocket server specified by the URL set via
 * ovenmedia_client_set_url(). This function is asynchronous and will use signals
 * to indicate events such as receiving an offer or connection failure.
 */
void ovenmedia_client_connect(OvenMediaClient *self);

/**
 * Signal: OvenMediaClient::offer-received
 * @sdp: (type utf8): The SDP offer string received from the server.
 *
 * Emitted when an SDP offer is received from the WebSocket server.
 */
G_END_DECLS

#endif /* OVENMEDIA_CLIENT_H */