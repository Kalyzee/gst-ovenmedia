#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gstovenmediasrc.h"
#include <gst/gst.h>

gboolean ovenmedia_plugin_init(GstPlugin *plugin)
{
    
    gst_element_register(plugin, "ovenmediasrc",
                              GST_RANK_NONE,
                              GST_TYPE_OVENMEDIA_SRC);   
    return TRUE;
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "gstovenmedia"
#endif

/* gstreamer looks for this structure to register trackers
 *
 * exchange the string 'Template tracker' with your tracker description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    ovenmedia,
    "ovenmedia",
    ovenmedia_plugin_init,
    PACKAGE_VERSION,
    "LGPL",
    "Kalyzee",
    "https://kalyzee.com/"
)
