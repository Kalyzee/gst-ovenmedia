#include <libsoup/soup.h>
#include <json-glib/json-glib.h>
#include <glib-object.h>

#define OVENMEDIA_TYPE_CLIENT (ovenmedia_client_get_type())
G_DECLARE_FINAL_TYPE(OvenMediaClient, ovenmedia_client, OVENMEDIA, CLIENT, GObject)

struct _OvenMediaClient {
    GObject parent_instance;

    SoupSession *session;
    SoupWebsocketConnection *ws_conn;

    gchar *url;
};

G_DEFINE_TYPE(OvenMediaClient, ovenmedia_client, G_TYPE_OBJECT)

// Signal identifiers
enum {
    SIGNAL_OFFER_RECEIVED,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

// Forward declarations
static void on_message(SoupWebsocketConnection *conn, SoupWebsocketDataType type,
                        GBytes *message, gpointer user_data);
static void on_close(SoupWebsocketConnection *conn, gpointer user_data);
static void on_connect(GObject *session, GAsyncResult *res, gpointer user_data);

// Object initialization
static void ovenmedia_client_init(OvenMediaClient *self) {
    self->session = soup_session_new();
    self->ws_conn = NULL;
    self->url = NULL;
}

// Object disposal
static void ovenmedia_client_dispose(GObject *object) {
    OvenMediaClient *self = OVENMEDIA_CLIENT(object);

    if (self->ws_conn) {
        g_object_unref(self->ws_conn);
        self->ws_conn = NULL;
    }

    if (self->session) {
        g_object_unref(self->session);
        self->session = NULL;
    }

    g_free(self->url);

    G_OBJECT_CLASS(ovenmedia_client_parent_class)->dispose(object);
}

// Object class initialization
static void ovenmedia_client_class_init(OvenMediaClientClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = ovenmedia_client_dispose;

    // Register signals
    signals[SIGNAL_OFFER_RECEIVED] = g_signal_new(
        "offer-received",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_LAST,
        0,
        NULL, NULL, NULL,
        G_TYPE_NONE,
        1,
        G_TYPE_STRING // SDP string
    );
}

// Function to set the WebSocket URL
void ovenmedia_client_set_url(OvenMediaClient *self, const gchar *url) {
    g_free(self->url);
    self->url = g_strdup(url);
}

// Function to connect to the WebSocket server
void ovenmedia_client_connect(OvenMediaClient *self) {
    SoupMessage *msg = soup_message_new(SOUP_METHOD_GET, self->url);

    soup_session_websocket_connect_async(
        self->session, msg, NULL, NULL, 0, NULL, (GAsyncReadyCallback)on_connect, self);
}

// Callback for WebSocket connection
static void on_connect(GObject *session, GAsyncResult *res, gpointer user_data) {
    OvenMediaClient *self = OVENMEDIA_CLIENT(user_data);
    GError *error = NULL;

    self->ws_conn = soup_session_websocket_connect_finish(SOUP_SESSION(session), res, &error);
    if (error) {
        g_printerr("WebSocket connection failed: %s\n", error->message);
        g_error_free(error);
        return;
    }

    g_print("WebSocket connection established\n");

    g_signal_connect(self->ws_conn, "message", G_CALLBACK(on_message), self);
    g_signal_connect(self->ws_conn, "closed", G_CALLBACK(on_close), self);

    // Send request_offer command
    JsonObject *json_obj = json_object_new();
    json_object_set_string_member(json_obj, "command", "request_offer");

    JsonNode *root = json_node_new(JSON_NODE_OBJECT);
    json_node_set_object(root, json_obj);

    JsonGenerator *gen = json_generator_new();
    json_generator_set_root(gen, root);
    gchar *message = json_generator_to_data(gen, NULL);

    soup_websocket_connection_send_text(self->ws_conn, message);

    g_print("Sent: %s\n", message);

    g_free(message);
    g_object_unref(gen);
    json_node_free(root);
    g_object_unref(json_obj);
}

// Callback for messages received
static void on_message(SoupWebsocketConnection *conn, SoupWebsocketDataType type,
                        GBytes *message, gpointer user_data) {
    OvenMediaClient *self = OVENMEDIA_CLIENT(user_data);

    if (type == SOUP_WEBSOCKET_DATA_TEXT) {
        gsize size;
        const char *data = g_bytes_get_data(message, &size);
        //g_print("Message received: %.*s\n", (int)size, data);

        // Parse JSON response
        JsonParser *parser = json_parser_new();
        if (json_parser_load_from_data(parser, data, size, NULL)) {
            JsonNode *root = json_parser_get_root(parser);
            JsonObject *json_obj = json_node_get_object(root);

            const gchar *command = json_object_get_string_member(json_obj, "command");
            if (g_strcmp0(command, "offer") == 0) {
                const gchar *sdp = json_object_get_string_member(json_object_get_object_member(json_obj, "sdp"), "sdp");
                // Emit signal for offer received
                g_signal_emit(self, signals[SIGNAL_OFFER_RECEIVED], 0, sdp);
            }
        }

        g_object_unref(parser);
    }
}

// Callback for WebSocket close
static void on_close(SoupWebsocketConnection *conn, gpointer user_data) {
    OvenMediaClient *self = OVENMEDIA_CLIENT(user_data);
    g_print("WebSocket connection closed\n");
}