/* MoodSound - Rocking blinking for your Moodlamps!
 * Copyright (C) 2009 Franz Pletz <fpletz@franz-pletz.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <gst/gst.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SRV_IP "10.23"
#define PORT 2324

GstElement *audio, *pipeline;

void
moodsend(char *lamp, char *r, char *g, char *b) {
    static int sockfd = -1;
    static struct sockaddr_in si_other;

    if(sockfd < 0) {
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockfd < 0) printf("error opening socket\n");

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	inet_aton(SRV_IP, &si_other.sin_addr);
    }

    char buf[255];
    sprintf(buf, "acF#%s#%s%s%s0900ab", lamp, r, g, b);

    sendto(sockfd, buf, strlen(buf), 0,
	    (struct sockaddr *) &si_other, sizeof(si_other));
    sync(sockfd);
}


static guint spect_bands = 2;
static int col = 0;
static float last = -1;
static bool last_beat = false;
#define AUDIOFREQ 800

/* receive spectral data from element message */
gboolean
message_handler (GstBus * bus, GstMessage * message, gpointer data)
{
    if(message->type == GST_MESSAGE_ERROR) {
	gst_element_set_state (pipeline, GST_STATE_NULL);
	gst_element_set_state (pipeline, GST_STATE_PLAYING);
    } else if (message->type == GST_MESSAGE_ELEMENT) {
	const GstStructure *s = gst_message_get_structure (message);
	const gchar *name = gst_structure_get_name (s);
	GstClockTime endtime;

	g_print("%s\n", name);

	if (strcmp (name, "spectrum") == 0) {
	    const GValue *magnitudes;
	    const GValue *phases;
	    const GValue *mag, *phase;
	    gdouble freq;
	    guint i;

	    if (!gst_structure_get_clock_time (s, "endtime", &endtime))
		endtime = GST_CLOCK_TIME_NONE;

	    //g_print ("New spectrum message, endtime %" GST_TIME_FORMAT "\n",
	    //    GST_TIME_ARGS (endtime));

	    magnitudes = gst_structure_get_value (s, "magnitude");
	    phases = gst_structure_get_value (s, "phase");

	    for (i = 0; i < spect_bands; ++i) {
		freq = (gdouble)((AUDIOFREQ/2)*i + AUDIOFREQ/4) / spect_bands;
		mag = gst_value_list_get_value (magnitudes, i);
		phase = gst_value_list_get_value (phases, i);

		if (mag != NULL && phase != NULL) {
		    float x = g_value_get_float(mag);
		    if(last == -1)
			last = x;

		    //printf("%i", col);
		    if(i > 0 || x < -65) break;
		    printf("%f", x - last);
		    if(x - last > 2.0) { //x > -30) {
			if(!last_beat) {
			    /*if(col == 0)
			      col = 1;
			      else if (col == 1)
			      col = 2;
			      else
			      col = 0;*/
			    col++;
			    if(col > 6) col = 0;
			    last_beat = true;
			    g_print("\t\tBEAT!");
			}
		    } else {
			last_beat = false;
		    }
		    g_print("\n");
		    //printf("%i\n", col);

		    if(last_beat)
			if(col == 0)
			    moodsend("00","FF","FF","00");
			else if (col == 1)
			    moodsend("00","00","FF","00");
			else if (col == 2)
			    moodsend("00","00","FF","FF");
			else if (col == 3)
			    moodsend("00","FF","00","FF");
			else if (col == 4)
			    moodsend("00","00","00","FF");
			else if (col == 5)
			    moodsend("00","00","FF","FF");
			else
			    moodsend("00","AA","77","EE");

		    g_print ("band %d (freq %g): magnitude %f dB phase %f\n",
			    i, freq, g_value_get_float (mag),
			    g_value_get_float (phase));
		    last = x;
		}
	    }
	//g_print ("\n");
	}
    }
    return TRUE;
}

static void
cb_newpad (GstElement *decodebin,
	GstPad     *pad,
	gboolean    last,
	gpointer    data)
{
    GstCaps *caps;
    GstStructure *str;
    GstPad *audiopad;

    /* only link once */
    audiopad = gst_element_get_pad (audio, "sink");
    if (GST_PAD_IS_LINKED (audiopad)) {
	g_object_unref (audiopad);
	return;
    }

    /* check media type */
    caps = gst_pad_get_caps (pad);
    str = gst_caps_get_structure (caps, 0);
    if (!g_strrstr (gst_structure_get_name (str), "audio")) {
	gst_caps_unref (caps);
	gst_object_unref (audiopad);
	return;
    }
    gst_caps_unref (caps);

    /* link'n'play */
    gst_pad_link (pad, audiopad);
}

int
main (int argc, char *argv[])
{
    GMainLoop *loop;
    GstElement *src, *dec, *conv, *spectrum, *sink;
    GstPad *audiopad;
    GstBus *bus;

    /* init GStreamer */
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, TRUE);

    /* make sure we have input */
    if (argc != 2) {
	g_print ("Usage: %s <filename>\n", argv[0]);
	return -1;
    }

    /* setup */
    pipeline = gst_pipeline_new ("pipeline");

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, message_handler, NULL);
    gst_object_unref (bus);

    src = gst_element_factory_make ("filesrc", "source");
    g_object_set (G_OBJECT (src), "location", argv[1], NULL);
    dec = gst_element_factory_make ("decodebin", "decoder");
    g_signal_connect (dec, "new-decoded-pad", G_CALLBACK (cb_newpad), NULL);
    gst_bin_add_many (GST_BIN (pipeline), src, dec, NULL);
    gst_element_link (src, dec);

    /* create audio output */
    audio = gst_bin_new ("audiobin");
    conv = gst_element_factory_make ("audioconvert", "aconv");

    spectrum = gst_element_factory_make ("spectrum", "spectrum");
    g_object_set (G_OBJECT (spectrum), "bands", spect_bands, "threshold", -80,
	    "message", TRUE, "message-phase", TRUE, "interval", 70000000, NULL);
    audiopad = gst_element_get_pad (conv, "sink");

    sink = gst_element_factory_make ("alsasink", "sink");
    g_object_set (G_OBJECT (sink), "sync", TRUE, "buffer-time", (gint64)100000,
	    NULL);

    gst_bin_add_many (GST_BIN (audio), conv, spectrum, sink, NULL);
    gst_element_link (conv, spectrum);
    gst_element_link (spectrum, sink);
    gst_element_add_pad (audio, gst_ghost_pad_new ("sink", audiopad));
    gst_object_unref (audiopad);
    gst_bin_add (GST_BIN (pipeline), audio);

    /* run */
    while(1) {
	gst_element_set_state (pipeline, GST_STATE_PLAYING);
	g_main_loop_run (loop);
    }

    /* cleanup */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));

    return 0;
}

