#include <gst/gst.h>
#include <glib.h>
#include <unistd.h>
#include <pthread.h>

pthread_t dot_thread;

GstElement *pipeline, *source, *muxer, *encoder, *srtsink, *converter;


void *print_stats(void *sink) {
    while (TRUE) {
        sleep(1);
        GstStructure *stats;
        gdouble bandwidth;
        g_object_get(srtsink, "stats", &stats, NULL);

        if (gst_structure_has_field(stats, "callers") == FALSE)
            continue;

        GValueArray *callers = g_value_get_boxed((GValue *) gst_structure_get_value(stats, "callers"));

        if (callers->n_values < 1)
            continue;
        GstStructure *val = g_value_get_boxed((GValue *) g_value_array_get_nth(callers, 0));
        gst_structure_get_double(val, "bandwidth-mbps", &bandwidth);


        g_print("Bitrate: %f \n", bandwidth);
    }
    pthread_exit(0);
}

int
main(int argc,
     char *argv[]) {
    GMainLoop *loop;

    /* Initialisation */
    gst_init(&argc, &argv);

    loop = g_main_loop_new(NULL, FALSE);

    /* Create gstreamer elements */
    pipeline = gst_pipeline_new("srt-example");
    source = gst_element_factory_make("videotestsrc", "source");
    converter = gst_element_factory_make("videoconvert", "converter");
    encoder = gst_element_factory_make("x264enc", "encoder");
    muxer = gst_element_factory_make("mpegtsmux", "muxer");
    srtsink = gst_element_factory_make("srtsink", "srtsink");


    if (!pipeline || !source || !converter || !encoder || !muxer || !srtsink) {
        g_printerr("One element could not be created. Exiting.\n");
        return -1;
    }

    /* Set up the pipeline */
    g_object_set(G_OBJECT (srtsink), "uri", "srt://:8888", NULL);    
    gst_bin_add_many(GST_BIN (pipeline),
                     source, converter, encoder, muxer, srtsink, NULL);

    gst_element_link_many(source, converter, encoder, muxer, srtsink, NULL);
    
    
    /* Set the pipeline to "playing" state*/
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    pthread_create(&dot_thread, NULL, print_stats, srtsink);

    /* Iterate */
    g_print("Running...\n");
    g_main_loop_run(loop);

    /* Out of the main loop, clean up nicely */
    g_print("Returned, stopping playback\n");
    gst_element_set_state(pipeline, GST_STATE_NULL);

    g_print("Deleting pipeline\n");
    gst_object_unref(GST_OBJECT (pipeline));
    g_main_loop_unref(loop);

    return 0;
}