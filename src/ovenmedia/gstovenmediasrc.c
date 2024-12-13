#include "gstovenmediasrc.h"
#include "ovenmediaclient.h"
#include <gst/sdp/sdp.h>
#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* properties */
enum
{
  PROP_0,
};

struct _GstOvenmediaSrc
{
  GstBin parent_instance;
  OvenMediaClient *client;
  GstElement *webrtc;
};

G_DEFINE_TYPE(GstOvenmediaSrc, gst_ovenmedia_src, GST_TYPE_BIN);

static void on_ice_candidate(GstElement *webrtcbin, guint mline_index, gchar *candidate, gpointer user_data) {
    //send_ice_candidate(webrtcbin, mline_index, candidate);
}

static void
on_answer_created (GstPromise * promise, gpointer user_data)
{
  GstOvenmediaSrc *self = GST_OVENMEDIA_SRC(user_data);
  GstElement *webrtc = self->webrtc;

  GstWebRTCSessionDescription *answer = NULL;
  const GstStructure *reply;

  reply = gst_promise_get_reply (promise);
  gst_structure_get (reply, "answer", GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &answer, NULL);
  gst_promise_unref (promise);

  g_print("Answer created");

  promise = gst_promise_new ();
  g_signal_emit_by_name (webrtc, "set-local-description", answer, promise);
  gst_promise_interrupt (promise);
  gst_promise_unref (promise);

  /* Send answer to peer */
  gst_webrtc_session_description_free (answer);
}


static void on_offer_received(OvenMediaClient *client, const gchar *sdp, gpointer user_data) {
    
  GstOvenmediaSrc *self = GST_OVENMEDIA_SRC(user_data);
  GstElement *webrtcbin = self->webrtc;

    // Créer une réponse SDP
    GstWebRTCSessionDescription *offer = NULL;
    GstPromise *promise = gst_promise_new();
    
    GstSDPMessage *msg;
    GstSDPResult result = gst_sdp_message_new_from_text(sdp, &msg);
    if (result != GST_SDP_OK) {
        g_printerr("Failed to parse SDP offer\n");
        return;
    }

    offer = gst_webrtc_session_description_new (GST_WEBRTC_SDP_TYPE_OFFER, msg);

    g_signal_emit_by_name(webrtcbin, "set-remote-description", offer, promise);
    gst_promise_interrupt(promise);
    gst_promise_unref(promise);

    promise = gst_promise_new_with_change_func (on_answer_created, self,
      NULL);
    g_signal_emit_by_name(webrtcbin, "create-answer", NULL, promise);
    gst_promise_interrupt(promise);
}

static void gst_ovenmedia_src_init(GstOvenmediaSrc *self)
{
  GstBin *bin = GST_BIN(self);
  GstElement *element = GST_ELEMENT(self);
  self->client = g_object_new(OVENMEDIA_TYPE_CLIENT, NULL);

  g_signal_connect(self->client, "offer-received", G_CALLBACK(on_offer_received), self);
  ovenmedia_client_set_url(self->client, "ws://localhost:3333/app/test");
  
  self->webrtc = gst_element_factory_make("webrtcbin", "webrtcbin");
  gst_bin_add(bin, self->webrtc);

  ovenmedia_client_connect(self->client);
}

static void gst_ovenmedia_src_set_property(GObject *object,
                                                guint prop_id,
                                                const GValue *value,
                                                GParamSpec *pspec){
    GstOvenmediaSrc *self = GST_OVENMEDIA_SRC(object);

    switch (prop_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;    
    }

}

static void gst_ovenmedia_src_get_property(GObject *object,
                                                guint prop_id,
                                                GValue *value,
                                                GParamSpec *pspec){

    GstOvenmediaSrc *self = GST_OVENMEDIA_SRC(object);

    switch (prop_id) {   
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }

}

static void gst_ovenmedia_src_class_init(GstOvenmediaSrcClass *klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->set_property = gst_ovenmedia_src_set_property;
  object_class->get_property = gst_ovenmedia_src_get_property;


  gst_element_class_set_static_metadata(element_class,
                                        "Custom Bin",
                                        "Custom Bin",
                                        "Custom Bin",
                                        "Ludovic Bouguerra <ludovic.bouguerra@kalyzee.com>");
}
