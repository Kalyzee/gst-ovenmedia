plugin_c_args = ['-DHAVE_CONFIG_H']

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', '1.0')
cdata.set_quoted('PACKAGE', 'gst-plugins')
cdata.set_quoted('GST_LICENSE', 'LGPL')
cdata.set_quoted('GST_API_VERSION', '1.20')
cdata.set_quoted('GST_PACKAGE_NAME', 'GStreamer template Plug-ins')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://www.kalyzee.com')

configure_file(output : 'config.h',
               configuration : cdata)

libsoup_dep = dependency('libsoup-3.0')
libjsonglib_dep = dependency('json-glib-1.0')
glib_dep = dependency('glib-2.0')
libgstbase_dep = dependency('gstreamer-plugins-base-1.0')
lbgstsdp_dep = dependency('gstreamer-sdp-1.0') # Adds the -lgstsdp-1.0 flag
libgstwebrtc_dep = dependency('gstreamer-webrtc-1.0')

ovenmedia_sources = [
'ovenmedia/ovenmediaclient.c',
'ovenmedia/gstovenmediasrc.c',
'ovenmedia/gstovenmedia.c',
]

ovenmedia = library('gstovenmedia',
    ovenmedia_sources,
    dependencies : [gst_dep, glib_dep, libjsonglib_dep, libsoup_dep, libgstbase_dep, lbgstsdp_dep, libgstwebrtc_dep],
    c_args: plugin_c_args,
    install : true,
    install_dir : plugins_install_dir,
)
