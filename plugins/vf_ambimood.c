/* vf_ambimood - Ambilight video filter plugin for mplayer
 * Copyright (C) 2009 Franz Pletz <fpletz@franz-pletz.org>
 *
 * This plugin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This plugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "config.h"
#include "mp_msg.h"

#include "img_format.h"
#include "mp_image.h"
#include "vf.h"

#define MOD_FRAME 4

#define NUM_LAMPS 5
//char lamps[][] = { { 65, -1 }, {

#define SRV_IP "10.23"
#define SRV_PORT 2324

struct vf_priv_s {
    unsigned int frame;
    int sockfd;
    struct sockaddr_in si;
};

static int config(struct vf_instance_s* vf, int width, int height, int d_width,
                    int d_height, unsigned int flags, unsigned int outfmt) {
    return vf_next_config(vf,width,height,d_width,d_height,flags,outfmt);
}

static int query_format(struct vf_instance_s *vf, unsigned int fmt){
    switch(fmt) {
	case IMGFMT_YV12:
	    return vf_next_query_format(vf, fmt);
    }
    return 0;
}

void moodsend(struct vf_instance_s *vf, char lamp, char r, char g, char b) {
    char tbuf[16], buf[32], *t;

    /* prepare string to send */
    sprintf(tbuf, "%02x#%02x%02x%02x", lamp, r, g, b);
    t = tbuf-1; while(*++t != '\0') *t = (char)toupper(*t);
    sprintf(buf, "acF#%s0900ab", tbuf);
    printf("%s\n", buf);

    /* send! */
    sendto(vf->priv->sockfd, buf, strlen(buf), 0,
	    (struct sockaddr *) &vf->priv->si, sizeof(vf->priv->si));
    sync(vf->priv->sockfd);
}

static void addyuv2rgb(unsigned char Y, unsigned char U, unsigned char V,
	    int *r_sum, int *g_sum, int *b_sum) {
    /* deep motherfucking magic! don't touch! */
    int r = 1.164*(Y - 16) + 1.596*(V - 128);
    int g = 1.164*(Y - 16) - 0.813*(V - 128) - 0.391*(U - 128);
    int b = 1.164*(Y - 16) + 2.018*(U - 128);

    /* reset caps */
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;

    /* add to sums */
    (*r_sum) += r;
    (*g_sum) += g;
    (*b_sum) += b;
}

void process(struct vf_instance_s *vf, mp_image_t *mpi) {
    int x, y;
    int r_sum=0, g_sum=0, b_sum=0;

    if(vf->priv->frame % MOD_FRAME == 0) {
	for(y=0; y < mpi->h; y++) {
	    for(x=0; x < mpi->w/8; x++)
		addyuv2rgb(mpi->planes[0][y*mpi->stride[0]+x],
			mpi->planes[1][y/2*mpi->stride[1]+x/2],
			mpi->planes[2][y/2*mpi->stride[2]+x/2],
			&r_sum, &g_sum, &b_sum);

	    /*for(x=7/8*mpi->w; x < mpi->w; x++)
		addyuv2rgb(mpi->planes[0][y*mpi->stride[0]+x],
			mpi->planes[1][y/2*mpi->stride[1]+x/2],
			mpi->planes[2][y/2*mpi->stride[2]+x/2],
			&r_sum, &g_sum, &b_sum);*/
	}

	r_sum /= mpi->h*mpi->w/8;
	g_sum /= mpi->h*mpi->w/8;
	b_sum /= mpi->h*mpi->w/8;

	mp_msg(MSGT_VFILTER, MSGL_INFO,"vf_ambimood(%i): %u %u %u  \n",
		vf->priv->frame, r_sum, g_sum, b_sum);
	moodsend(vf, 0, r_sum, g_sum, b_sum);
    }
}

static int put_image(struct vf_instance_s *vf, mp_image_t *mpi, double pts) {
    mp_image_t *dmpi;

    process(vf, mpi);

    /* copy over the frame */
    dmpi = vf_get_image(vf->next, mpi->imgfmt, MP_IMGTYPE_EXPORT, 0,
	    mpi->width, mpi->height);
    dmpi->planes[0] = mpi->planes[0];
    dmpi->stride[0] = mpi->stride[0];
    dmpi->planes[1] = mpi->planes[1];
    dmpi->stride[1] = mpi->stride[1];
    dmpi->planes[2] = mpi->planes[2];
    dmpi->stride[2] = mpi->stride[2];
    vf_clone_mpi_attributes(dmpi, mpi);

    /* increase internal frame counter */
    vf->priv->frame++;

    return vf_next_put_image(vf, dmpi, pts);
}

static int control(struct vf_instance_s* vf, int request, void* data){
    return vf_next_control(vf,request,data);
}

static void uninit(struct vf_instance_s *vf) {
    if (vf->priv) free(vf->priv);
}

static int open(vf_instance_t *vf, char* args) {
    vf->query_format = query_format;
    vf->put_image = put_image;
    vf->control = control;
    vf->uninit = uninit;
    vf->config = config;
    vf->priv = malloc(sizeof(struct vf_priv_s));
    vf->priv->frame = 0;

    /* open socket */
    if((vf->priv->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        mp_msg(MSGT_VFILTER, MSGL_INFO, "error opening socket\n");

    memset((char *) &vf->priv->si, 0, sizeof(vf->priv->si));
    vf->priv->si.sin_family = AF_INET;
    vf->priv->si.sin_port = htons(SRV_PORT);
    inet_aton(SRV_IP, &vf->priv->si.sin_addr);

    return 1;
}

vf_info_t vf_info_ambimood = {
    "Ambilight Moodlamp filter",
    "ambimood",
    "ich!",
    "based on me",
    open,
    NULL
};
