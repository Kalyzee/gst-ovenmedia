#include "gstovenmediasrc.h"
#include "ovenmediaclient.h"

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
};

G_DEFINE_TYPE(GstOvenmediaSrc, gst_ovenmedia_src, GST_TYPE_BIN);


static void on_offer_received(OvenMediaClient *client, const gchar *sdp, gpointer user_data) {
    g_print("Offer SDP received:\n%s\n", sdp);
}

static void gst_ovenmedia_src_init(GstOvenmediaSrc *self)
{
  GstBin *bin = GST_BIN(self);
  GstElement *element = GST_ELEMENT(self);
  OvenMediaClient *client = g_object_new(OVENMEDIA_TYPE_CLIENT, NULL);
  ovenmedia_client_set_url(client, "ws://localhost:3333/app/test");
  g_signal_connect(client, "offer-received", G_CALLBACK(on_offer_received), NULL);
  ovenmedia_client_connect(client);

  //GstElement *webrtc = gst_element_factory_make("webrtcbin", "webrtcbin");
  //gst_bin_add(bin, webrtc);
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
