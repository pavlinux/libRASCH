#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ra_plot_defines.h"
#include "text_device.h"


#define TD_NAME   "text-device"
#define TD_DESC   "print plot funtion calls in a file"


struct text_device
{
	char *fn;
	FILE *fp;

	long width;
	long height;

	double pix_per_mm_v;
	double pix_per_mm_h;

	int color_r, color_g, color_b;
	int line_w, line_t, line_a;
	char *font_family;
	int font_face, font_size;
};   /* struct text_device */

struct plot_device * td_open_device(rpl_handle rpl, void *add_info);
int td_close_device(device_handle dh);

int td_height(device_handle dh);
int td_width(device_handle dh);
int td_set_color(device_handle dh, int r, int g, int b);
int td_get_color(device_handle dh, int *r, int *g, int *b);
int td_set_line(device_handle dh, int width, int type, int arrow);
int td_get_line(device_handle dh, int *width, int *type, int *arrow);
int td_set_font(device_handle dh, const char *family, int face, int size);
int td_get_font(device_handle dh, char *family, int buf_size, int *face, int *size);
int td_draw_points(device_handle dh, int *x, int *y, int num_points, int symbol, int size);
int td_draw_line(device_handle dh, int xs, int ys, int xe, int ye);
int td_draw_text(device_handle dh, int x, int y, int width, int height, int alignment, double degree, const char *s);
int td_draw_polyline(device_handle dh, int *x, int *y, int num_points);


int
td_set_dev_info(struct device_info *i)
{
	i->name = malloc(sizeof(char) * (strlen(TD_NAME)+1));
	strcpy(i->name, TD_NAME);
	i->desc = malloc(sizeof(char) * (strlen(TD_DESC)+1));
	strcpy(i->desc, TD_DESC);

	i->need_add_info = 1;

	i->open_device = td_open_device;
	i->close_device = td_close_device;

	return 0;
} /* td_set_dev_info() */


struct plot_device *
td_open_device(rpl_handle rpl, void *add_info)
{
	struct text_device *td;
	struct plot_device *d;

	if ((add_info == NULL) || (((const char *)add_info)[0] == '\0'))
		return NULL;

	td = calloc(1, sizeof(struct text_device));
	td->fp = fopen(add_info, "w");
	if (td->fp == NULL)
	{
		free(td);
		return NULL;
	}
	td->fn = malloc(sizeof(char) * (strlen((const char *)add_info)+1));
	strcpy(td->fn, (const char *)add_info);

	td->height = 100;
	td->width = 200;

	d = calloc(1, sizeof(struct plot_device));
	d->rpl = rpl;
	d->plot_handle = (void *)td;

	d->height = td_height;
	d->width = td_width;
	d->set_color = td_set_color;
	d->get_color = td_get_color;
	d->set_line = td_set_line;
	d->get_line = td_get_line;
	d->set_font = td_set_font;
	d->get_font = td_get_font;
	d->draw_points = td_draw_points;
	d->draw_line = td_draw_line;
	d->draw_text = td_draw_text;
	d->draw_polyline = td_draw_polyline;
	
	return d;
} /* init_device() */



int
td_close_device(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;

	fclose(td->fp);
	td->fp = NULL;

	free(td->fn);
	td->fn = NULL;

	free(td);
	d->plot_handle = NULL;

	free(d);

	return 0;
} /* td_close_device() */


int
td_height(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;

	return td->height;
} /* td_height() */


int
td_width(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;

	return td->width;
} /* td_width() */


int
td_set_color(device_handle dh, int r, int g, int b)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	td->color_r = r;
	td->color_g = g;
	td->color_b = b;

	fprintf(td->fp, "set color: r=%d  g=%d  b=%d\n", r, g, b);

	return 0;
} /* td_set_color() */


int
td_get_color(device_handle dh, int *r, int *g, int *b)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	*r = td->color_r;
	*g = td->color_g;
	*b = td->color_b;

	fprintf(td->fp, "get color: r=%d  g=%d  b=%d\n", *r, *g, *b);

	return 0;
} /* td_get_color() */


int
td_set_line(device_handle dh, int width, int type, int arrow)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	td->line_w = width;
	td->line_t = type;
	td->line_a = arrow;

	fprintf(td->fp, "set line: width=%d  type=%d  arrow=%d\n", width, type, arrow);

	return 0;
} /* td_set_line() */


int
td_get_line(device_handle dh, int *width, int *type, int *arrow)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	*width = td->line_w;
	*type = td->line_t;
	*arrow = td->line_a;

	fprintf(td->fp, "get line: width=%d  type=%d  arrow=%d\n", *width, *type, *arrow);

	return 0;
} /* td_get_line() */


int
td_set_font(device_handle dh, const char *family, int face, int size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	if (td->font_family)
	{
		free(td->font_family);
		td->font_family = NULL;
	}
	if (family)
	{
		td->font_family = (char *)malloc(sizeof(char) * (strlen(family)+1));
		strcpy(td->font_family, family);
	}
	td->font_face = face;
	td->font_size = size;
	
	fprintf(td->fp, "set font: family=%s  face=%d  size=%d\n", family, face, size);

	return 0;
} /* td_set_font() */


int
td_get_font(device_handle dh, char *family, int buf_size, int *face, int *size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (buf_size);

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

/* 	if (buf_size > 0) */
/* 		strncpy(family, td->font_family, buf_size); */
	*face = td->font_face;
	*size = td->font_size;
	
	fprintf(td->fp, "set font: family=%s  face=%d  size=%d\n", family, *face, *size);

	return 0;
} /* td_get_font() */


int
td_draw_points(device_handle dh, int *x, int *y, int num_points, int symbol, int size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (x || y);

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "draw points: num-points=%d  symbol=%d  size=%d\n", num_points, symbol, size);

	return 0;
} /* td_draw_points() */


int
td_draw_line(device_handle dh, int xs, int ys, int xe, int ye)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "draw line: from %d/%d to %d/%d\n", xs, ys, xe, ye);

	return 0;
} /* td_draw_line() */


int
td_draw_text(device_handle dh, int x, int y, int width, int height, int alignment, double degree, const char *s)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (width || height);

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "draw text: '%s' at %d/%d  alignment=%d  degree=%.2f\n", s, x, y, alignment, degree);

	return 0;
} /* td_draw_text() */


int
td_draw_polyline(device_handle dh, int *x, int *y, int num_points)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (x || y);

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "draw polyline: num-points=%d\n", num_points);

	return 0;
} /* td_draw_polyline() */
