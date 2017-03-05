#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ra_plot_defines.h"

#define _RA_PLOT_LIB_BUILD
#include "ra_plot_lib.h"

#include "text_device.h"

#ifdef RA_PLOT_LIB_USE_QT
#include "qt_device.h"
#endif

#ifdef RA_PLOT_LIB_USE_GD
#include "gd_device.h"
#endif

rpl_handle
rpl_init()
{
	struct rpl_instance *r;
	int curr_dev = -1;

	r = calloc(1, sizeof(struct rpl_instance));
	r->num_devices = 1;
	r->dev = calloc(r->num_devices, sizeof(struct device_info));

	/* init text-device */
	curr_dev++;
	td_set_dev_info(r->dev+curr_dev);

#ifdef RA_PLOT_LIB_USE_QT
	/* init qt-device */
	r->num_devices++;
	r->dev = realloc(r->dev, (r->num_devices * sizeof(struct device_info)));
	memset(&(r->dev[r->num_devices-1]), 0, sizeof(struct device_info));
	curr_dev++;
	qt_set_dev_info(r->dev+curr_dev);
#endif  /* RA_PLOT_LIB_USE_QT */

#ifdef RA_PLOT_LIB_USE_GD
	/* init gd-device */
	r->num_devices++;
	r->dev = realloc(r->dev, (r->num_devices * sizeof(struct device_info)));
	memset(&(r->dev[r->num_devices-1]), 0, sizeof(struct device_info));
	curr_dev++;
	gd_set_dev_info(r->dev+curr_dev);
#endif /* RA_PLOT_LIB_USE_GD */

	return r;
} /* rpl_init() */


void
rpl_close(rpl_handle rpl)
{
	struct rpl_instance *r = (struct rpl_instance *)rpl;
	int i;

	if (!r)
		return;

	for (i = 0; i < r->num_devices; i++)
	{
		free(r->dev[i].name);
		free(r->dev[i].desc);
	}
	free(r->dev);

	free(r);
} /* rpl_close() */


long
rpl_num_devices(rpl_handle rpl)
{
	struct rpl_instance *r = (struct rpl_instance *)rpl;

	if (!r)
		return -1;

	return r->num_devices;
} /* rpl_num_devices() */


int
rpl_device_info(rpl_handle rpl, int device_idx, long info_id)
{
	struct rpl_instance *r = (struct rpl_instance *)rpl;

	if (info_id);

	if (!r)
		return -1;

	if (device_idx >= r->num_devices)
		return -1;
	

	return 0;
} /* rpl_device_info() */


device_handle
rpl_open_device(rpl_handle rpl, const char *name, void *add_info)
{
	struct rpl_instance *r = (struct rpl_instance *)rpl;
	int i, idx;

	if (!r)
		return NULL;

	idx = -1;
	for (i = 0; i < r->num_devices; i++)
	{
		if (strcmp(name, r->dev[i].name) == 0)
		{
			idx = i;
			break;
		}
	}
	
	if (idx >= 0)
		return rpl_open_device_idx(rpl, idx, add_info);
	else
		return NULL;
} /* rpl_open_device() */


device_handle
rpl_open_device_idx(rpl_handle rpl, int device_idx, void *add_info)
{
	struct rpl_instance *r = (struct rpl_instance *)rpl;
	struct plot_device *d;

	if (!r)
		return NULL;
	if (device_idx >= r->num_devices)
		return NULL;
	if (r->dev[device_idx].need_add_info && !add_info)
		return NULL;

	if (r->dev[device_idx].open_device)
	{
		d = r->dev[device_idx].open_device(rpl, add_info);
		d->device_idx = device_idx;
		
		return d;
	}
	else
		return NULL;
} /* rpl_open_device() */


/* long */
/* rpl_get_option_id(const char *option_ascii_id) */
/* { */
/* 	return 0; */
/* } /\* rpl_get_option_id() *\/ */


/* int */
/* rpl_set_option(rpl_handle rpl, long option_id, value_handle vh) */
/* { */
/* 	return 0; */
/* } /\* rpl_set_option() *\/ */


/* int */
/* rpl_get_option(rpl_handle rpl, long option_id, value_handle vh) */
/* { */
/* 	return 0; */
/* } /\* rpl_get_option() *\/ */


/* int */
/* rpl_set_device_option(device_handle long option_id, value_handle vh) */
/* { */
/* 	return 0; */
/* } /\* rpl_set_device_option() *\/ */


/* int */
/* rpl_get_device_option(device_handle long option_id, value_handle vh) */
/* { */
/* 	return 0; */
/* } /\* rpl_get_device_option() *\/ */


int
rpl_close_device(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct rpl_instance *r;

	if (!d)
		return -1;

	r = (struct rpl_instance *)(d->rpl);
	if (r->dev[d->device_idx].close_device)
		return r->dev[d->device_idx].close_device(dh);
	else
		return -1;
} /* rpl_close_device() */


int
rpl_set_device_painter(device_handle dh, void *painter)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->set_painter)
		return d->set_painter(dh, painter);
	else
		return -1;
} /* rpl_set_device_painter() */


long
rpl_device_height(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->height)
		return d->height(dh);
	else
		return -1;
} /* rpl_device_height() */


long
rpl_device_width(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->width)
		return d->width(dh);
	else
		return -1;
} /* rpl_device_width() */


int
rpl_set_color(device_handle dh, int r, int g, int b)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->set_color)
		return d->set_color(d, r, g, b);
	else
		return -1;
} /* rpl_set_color() */


int
rpl_get_color(device_handle dh, int *r, int *g, int *b)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->get_color)
		return d->get_color(d, r, g, b);
	else
		return -1;
} /* rpl_get_color() */


int
rpl_set_line(device_handle dh, int width, int type, int arrow)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->set_line)
		return d->set_line(d, width, type, arrow);
	else
		return -1;
} /* rpl_set_line() */


int
rpl_get_line(device_handle dh, int *width, int *type, int *arrow)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->get_line)
		return d->get_line(d, width, type, arrow);
	else
		return -1;
} /* rpl_get_line() */


int
rpl_set_font(device_handle dh, const char *family, int face, int size)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->set_font)
		return d->set_font(d, family, face, size);
	else
		return -1;
} /* rpl_set_font() */


int
rpl_get_font(device_handle dh, char *family, int buf_size, int *face, int *size)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->get_font)
		return d->get_font(d, family, buf_size, face, size);
	else
		return -1;
} /* rpl_get_font() */


int
rpl_set_scale(device_handle dh, double scale_factor)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->set_scale)
		return d->set_scale(d, scale_factor);
	else
		return -1;
} /* rpl_set_scale() */


int
rpl_get_scale(device_handle dh, double *scale_factor)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->get_scale)
		return d->get_scale(d, scale_factor);
	else
		return -1;
} /* rpl_get_scale() */


int
rpl_get_text_extent(device_handle dh, const char *string, int *width, int *height)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->get_text_extent)
		return d->get_text_extent(d, string, width, height);
	else
		return -1;
} /* rpl_get_text_extent() */


int
rpl_set_clipping_area(device_handle dh, int left, int top, int right, int bottom)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->set_clipping_area)
		return d->set_clipping_area(d, left, top, right, bottom);
	else
		return -1;
} /* rpl_set_clipping_area() */


int
rpl_draw_points(device_handle dh, int *x, int *y, int num_points, int symbol, int size)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->draw_points)
		return d->draw_points(d, x, y, num_points, symbol, size);
	else
		return -1;
} /* rpl_draw_points() */


int
rpl_draw_line(device_handle dh, int xs, int ys, int xe, int ye)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->draw_line)
		return d->draw_line(d, xs, ys, xe, ye);
	else
		return -1;
} /* rpl_draw_line() */


int
rpl_draw_text(device_handle dh, int x, int y, int width, int height, int alignment, double degree, const char *s)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->draw_text)
		return d->draw_text(d, x, y, width, height, alignment, degree, s);
	else
		return -1;
} /* rpl_draw_text() */


int
rpl_draw_polyline(device_handle dh, int *x, int *y, int num_points)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d)
		return -1;

	if (d->draw_polyline)
		return d->draw_polyline(d, x, y, num_points);
	else
		return -1;
} /* rpl_draw_polyline() */


int
rpl_point2value(device_handle dh, struct ra_plot_options *opt, int pair_idx, int x, int y, double *x_value, double *y_value)
{
	struct plot_device *d = (struct plot_device *)dh;

	if (!d || !opt)
		return -1;

	if (pair_idx >= opt->num_plot_pairs)
		return -1;

	*x_value = ((double)(x - d->pi.plot_area_x) / opt->axis[opt->pair[pair_idx].x_idx].pix_per_unit_x);
	*x_value += opt->axis[opt->pair[pair_idx].x_idx].tic_start;

	*y_value = ((double)(y - d->pi.plot_area_y) / opt->axis[opt->pair[pair_idx].y_idx].pix_per_unit_y);
	*y_value = opt->axis[opt->pair[pair_idx].y_idx].tic_end - *y_value;
	
	return 0;
} /* rpl_point2value() */
