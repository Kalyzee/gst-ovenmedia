#ifndef __GST_OVENMEDIA_SRC_H__
#define __GST_OVENMEDIA_SRC_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_OVENMEDIA_SRC gst_ovenmedia_src_get_type ()
G_DECLARE_FINAL_TYPE (GstOvenmediaSrc, gst_ovenmedia_src, GST, OVENMEDIA_SRC, GstBin)

struct GstOvenmediaSrcClass {
  GstBinClass parent_class;
};

G_END_DECLS

#endif
