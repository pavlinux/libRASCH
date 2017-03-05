#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#ifndef M_PI
  #define M_PI 3.1415926535897932384626433832795
#endif

#include <gd.h>

#define _RA_PLOT_LIB_BUILD
#include "ra_plot_lib.h"
#include "ra_plot_defines.h"
#include "gd_device.h"


#define GD_NAME   "gd-device"
#define GD_DESC   "plot device for png images"

#define GD_DEFAULT_FONT  "times"


struct gd_device
{
	char fn[RA_MAX_PATH];
	int img_type;

	int height;
	int width;

	double scale_factor;

	gdImagePtr im;

	int bg_color;
	int color;

	int color_r;
	int color_g;
	int color_b;

	int line_width;
	int line_type;
	int line_arrow;

	char *font;
	char *font_name;
	int font_face;
	int font_size;
};   /* struct gd_device */

/* prototypes */
struct plot_device * gd_open_device(rpl_handle rpl, void *add_info);
int gd_close_device(device_handle dh);

int gd_height(device_handle dh);
int gd_width(device_handle dh);
int gd_set_color(device_handle dh, int r, int g, int b);
int gd_get_color(device_handle dh, int *r, int *g, int *b);
int gd_set_line(device_handle dh, int width, int type, int arrow);
int gd_get_line(device_handle dh, int *width, int *type, int *arrow);
int gd_set_font(device_handle dh, const char *family, int face, int size);
int gd_get_font(device_handle dh, char *family, int buf_size, int *face, int *size);
int gd_set_scale(device_handle dh, double scale_factor);
int gd_get_scale(device_handle dh, double *scale_factor);
int gd_get_text_extent(device_handle dh, const char *string, int *width, int *height);
int gd_set_clipping_area(device_handle dh, int left, int top, int right, int bottom);
int gd_draw_points(device_handle dh, int *x, int *y, int num_points, int symbol, int size);
int gd_draw_line(device_handle dh, int xs, int ys, int xe, int ye);
int gd_draw_text(device_handle dh, int x, int y, int width, int height, int alignment, double degree, const char *s);
int gd_draw_polyline(device_handle dh, int *x, int *y, int num_points);


int
gd_set_dev_info(struct device_info *i)
{
	i->name = (char *)malloc(sizeof(char) * (strlen(GD_NAME)+1));
	strcpy(i->name, GD_NAME);
	i->desc = (char *)malloc(sizeof(char) * (strlen(GD_DESC)+1));
	strcpy(i->desc, GD_DESC);

	i->need_add_info = 1;

	i->open_device = gd_open_device;
	i->close_device = gd_close_device;

	return 0;
} /* gd_set_dev_info() */


struct plot_device *
gd_open_device(rpl_handle rpl, void *add_info)
{
	struct gd_device *gd;
	struct plot_device *d;
	struct gd_add_info *i;
	int ok;
#ifdef WIN32
	char font_path[RA_MAX_PATH];
#endif

	if ((add_info == NULL) || (((const char *)add_info)[0] == '\0'))
		return NULL;

	i = (struct gd_add_info *)add_info;

	gd = (struct gd_device *)calloc(1, sizeof(struct gd_device));
	gd->scale_factor = i->scale_factor;
	gd->im = gdImageCreate((int)(i->width * gd->scale_factor), (int)(i->height * gd->scale_factor));
	gd->width = i->width;
	gd->height = i->height;
	/* first color is used as the background color (here use white) */
	gd->bg_color = gdImageColorAllocate(gd->im, 255, 255, 255);
	/* default draw color is black */
	gd->color = gdImageColorAllocate(gd->im, 0, 0, 0);

	gd->img_type = i->img_type;
	strncpy(gd->fn, (const char *)i->file_name, RA_MAX_PATH);

	d = (struct plot_device *)calloc(1, sizeof(struct plot_device));
	d->rpl = rpl;
	d->plot_handle = (void *)gd;

	d->height = gd_height;
	d->width = gd_width;
	d->set_color = gd_set_color;
	d->get_color = gd_get_color;
	d->set_line = gd_set_line;
	d->get_line = gd_get_line;
	d->set_font = gd_set_font;
	d->get_font = gd_get_font;
	d->set_scale = gd_set_scale;
	d->get_scale = gd_get_scale;
	d->get_text_extent = gd_get_text_extent;
	d->set_clipping_area = gd_set_clipping_area;
	d->draw_points = gd_draw_points;
	d->draw_line = gd_draw_line;
	d->draw_text = gd_draw_text;
	d->draw_polyline = gd_draw_polyline;

	gd_set_line(d, 1, RA_PLOT_LINE_SOLID, 0);
	ok = gdFTUseFontConfig(0);
	if (!ok)
		fprintf(stderr, "fontconfig library is not available\n");
#ifdef WIN32
	sprintf(font_path, "%s\\fonts\\%s.ttf", getenv("SYSTEMROOT"), GD_DEFAULT_FONT);
	gd_set_font(d, font_path, 0, 10);
#else
	gd_set_font(d, GD_DEFAULT_FONT, 0, 10);
#endif
	/* TODO: move getenv() call to gd_set_font() to handle font issues correctly */

	return d;
} /* init_device() */



int
gd_close_device(device_handle dh)
{
	int ret;
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;
	FILE *fp;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;


	fp = fopen(gd->fn, "wb");
	if (fp)
	{
		int size = 0;
		char *data = NULL;

		ret = 0;
		switch (gd->img_type)
		{
		case GD_PNG:
			data = (char *)gdImagePngPtr(gd->im, &size);
			break;
		case GD_JPEG:
			data = (char *)gdImageJpegPtr(gd->im, &size, -1);
			break;
		default:
			ret = -1;
			break;
		}
		if (data)
		{
			if (fwrite(data, 1, size, fp) != (size_t)size)
				ret = -1;
			gdFree(data);
		}
		fclose(fp);
	}
	else
		ret = -1;

	gdImageDestroy(gd->im);

	free(gd);
	d->plot_handle = NULL;

	free(d);

	return ret;
} /* gd_close_device() */


int
gd_height(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	return gd->height;
} /* gd_height() */


int
gd_width(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	return gd->width;
} /* gd_width() */


int
gd_set_color(device_handle dh, int r, int g, int b)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	gd->color_r = r;
	gd->color_g = g;
	gd->color_b = b;

	gd->color = gdImageColorAllocate(gd->im, r, g, b);
	
	return 0;
} /* gd_set_color() */


int
gd_get_color(device_handle dh, int *r, int *g, int *b)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	if (r)
		*r = gd->color_r;
	if (g)
		*g = gd->color_g;
	if (b)
		*b = gd->color_b;

	return 0;
} /* gd_get_color() */


int
gd_set_line(device_handle dh, int width, int type, int arrow)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;
	int style[8], num;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	gd->line_width = (int)(width * gd->scale_factor);
	gd->line_type = type;
	gd->line_arrow = arrow;

	switch (gd->line_type)
	{
	case RA_PLOT_LINE_SOLID:
		style[0] = gd->color;
		num = 1;
		break;
	case RA_PLOT_LINE_DASH:
		style[0] = gd->color;
		style[1] = gd->color;
		style[2] = gd->color;
		style[3] = gdTransparent;
		style[4] = gdTransparent;
		style[5] = gdTransparent;
		num = 6;
		break;
	case RA_PLOT_LINE_DOT:
		style[0] = gd->color;
		style[1] = gdTransparent;
		num = 2;
		break;
	case RA_PLOT_LINE_DASHDOT:
		style[0] = gd->color;
		style[1] = gd->color;
		style[2] = gd->color;
		style[3] = gdTransparent;
		style[4] = gd->color;
		style[5] = gdTransparent;
		num = 6;
		break;
	case RA_PLOT_LINE_DASHDOTDOT:
		style[0] = gd->color;
		style[1] = gd->color;
		style[2] = gd->color;
		style[3] = gdTransparent;
		style[4] = gd->color;
		style[5] = gdTransparent;
		style[6] = gd->color;
		style[7] = gdTransparent;
		num = 8;
		break;
	default:
		return 0;
	}
	gdImageSetStyle(gd->im, style, num);
	gdImageSetThickness(gd->im, gd->line_width);

	return 0;
} /* gd_set_line() */


int
gd_get_line(device_handle dh, int *width, int *type, int *arrow)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	if (width)
		*width = (int)(gd->line_width / gd->scale_factor);
	if (type)
		*type = gd->line_type;
	if (arrow)
		*arrow = gd->line_arrow;

	return 0;
} /* gd_get_line() */


int
gd_set_font(device_handle dh, const char *family, int face, int size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (face);

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	if (family && (family[0] != '\0'))
	{
		if (gd->font_name)
		{
			free(gd->font_name);
			gd->font_name = NULL;
		}
		
		gd->font_name = malloc(sizeof(char) * (strlen(family) + 1));
		strcpy(gd->font_name, family);
	}
	if (gd->font)
	{
		free(gd->font);
		gd->font = NULL;
	}
	gd->font = malloc(sizeof(char) * (strlen(gd->font_name) + 50));
	strcpy(gd->font, gd->font_name);

/*	if (face != -1)
	{
		if (face & FONT_BOLD)
			strcat(gd->font, ":bold");
		if (face & FONT_ITALIC)
			strcat(gd->font, ":italic");
		gd->font_face = face;
	}
*/
	if (size != -1)
		gd->font_size = (int)(size * gd->scale_factor);

	return 0;
} /* gd_set_font() */


int
gd_get_font(device_handle dh, char *family, int buf_size, int *face, int *size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	if ((buf_size > 0) && (family))
		strncpy(family, gd->font_name, buf_size);

	if (face)
		*face = gd->font_face;
	if (size)
		*size = (int)(gd->font_size / gd->scale_factor);

	return 0;
} /* gd_get_font() */


int
gd_set_scale(device_handle dh, double scale_factor)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	gd->scale_factor = scale_factor;

	return 0;
} /* gd_set_scale() */


int
gd_get_scale(device_handle dh, double *scale_factor)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	*scale_factor = gd->scale_factor;

	return 0;
} /* gd_get_scale() */


int
gd_get_text_extent(device_handle dh, const char *string, int *width, int *height)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;
	int brect[8];

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	gdImageStringFT(NULL, &brect[0], 0, gd->font, gd->font_size, 0.0, 0, 0, (char *)string);

	if (width)
		*width = brect[2] - brect[0];
	if (height)
		*height = brect[3]-brect[7];

	return 0;
} /* gd_get_text_extent() */


int
gd_set_clipping_area(device_handle dh, int left, int top, int right, int bottom)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	gdImageSetClip(gd->im, (int)(left * gd->scale_factor), (int)(top * gd->scale_factor),
					(int)(right * gd->scale_factor), (int)(bottom * gd->scale_factor));

	return 0;
} /* gd_set_clipping_area() */


int
gd_draw_points(device_handle dh, int *x, int *y, int num_points, int symbol, int size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (x || y || num_points || symbol || size)
		;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	return 0;
} /* gd_draw_points() */


int
gd_draw_line(device_handle dh, int xs, int ys, int xe, int ye)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	if (gd->line_type == RA_PLOT_LINE_NONE)
		return 0;

	if (gd->line_type == RA_PLOT_LINE_SOLID)
	{
		gdImageLine(gd->im, (int)(xs * gd->scale_factor), (int)(ys * gd->scale_factor),
			    (int)(xe * gd->scale_factor), (int)(ye * gd->scale_factor), gd->color);
	}
	else
	{
		/* do set line again to be sure the correct color is used */
		gd_set_line(dh, gd->line_width, gd->line_type, gd->line_arrow);
		gdImageSetThickness(gd->im, 1);	/* there is an error with styled and thicker lines */
		gdImageLine(gd->im, (int)(xs * gd->scale_factor), (int)(ys * gd->scale_factor),
			    (int)(xe * gd->scale_factor), (int)(ye * gd->scale_factor), gdStyled);
	}

	return 0;
} /* gd_draw_line() */


int
gd_draw_text(device_handle dh, int x, int y, int width, int height, int alignment, double degree, const char *s)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;
	int brect[8];
	char *err;
	double angle;
	int x_use, y_use;
	int text_w, text_h;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;
	
	if (degree != 0)
		angle = 2.0 * M_PI * ((360.0 - degree) / 360.0);
	else
		angle = 0.0;

	/* get bounding rect and adapt x and y coordinates */
	err = gdImageStringFT(NULL, &brect[0], 0, gd->font, gd->font_size, angle, 0, 0, (char *)s);
	if (err) fputs(err, stderr);
	
	x_use = (int)(x * gd->scale_factor);
	y_use = (int)(y * gd->scale_factor);

	text_w = abs(brect[4] - brect[0]);
	text_h = abs(brect[3] - brect[7]);

/*  	fprintf(stderr, "%s  ->  %d/%d  %d/%d  %d/%d  %d/%d\n", s, brect[0], brect[1], brect[2], brect[3], */
/* 		brect[4], brect[5], brect[6], brect[7]); */

	if (alignment & ALIGN_RIGHT)
		x_use = (int)(x * gd->scale_factor + (width * gd->scale_factor - text_w));
	if (alignment & ALIGN_H_CENTER)
		x_use = (int)(x * gd->scale_factor + width * gd->scale_factor/2 - text_w/2);
	if (alignment & ALIGN_TOP)
		y_use = (int)(y * gd->scale_factor + text_h);
	if (alignment & ALIGN_V_CENTER)
		y_use = (int)(y * gd->scale_factor - (height * gd->scale_factor/2 - text_h/2));

/*  	fprintf(stderr, "%s at %d/%d (w=%d  h=%d  d=%.f) -> text-w=%d text-h=%d -> %d/%d\n", s, x, y, width, height, degree, */
/*  		text_w, text_h, x_use, y_use); */

	err = gdImageStringFT(gd->im, &brect[0], gd->color, gd->font, gd->font_size,
							angle, x_use, y_use, (char *)s);
	if (err) fputs(err, stderr);

	return 0;
} /* gd_draw_text() */


int
gd_draw_polyline(device_handle dh, int *x, int *y, int num_points)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct gd_device *gd;
	gdPoint *p;
	int i;

	if (!dh)
		return -1;

	gd = (struct gd_device *)d->plot_handle;
	if (!gd)
		return -1;

	if (gd->line_type == RA_PLOT_LINE_NONE)
		return 0;

	p = (gdPoint *)malloc(sizeof(gdPoint) * num_points);
	for (i = 0; i < num_points; i++)
	{
		p[i].x = (int)(x[i] * gd->scale_factor);
		p[i].y = (int)(y[i] * gd->scale_factor);
	}

	if (gd->line_type == RA_PLOT_LINE_SOLID)
		gdImageOpenPolygon(gd->im, p, num_points, gd->color);
	else
	{
		/* do set line again to be sure the correct color is used */
		gd_set_line(dh, gd->line_width, gd->line_type, gd->line_arrow);
		gdImageSetThickness(gd->im, 1);	/* there is an error with styled and thicker lines */
		gdImageOpenPolygon(gd->im, p, num_points, gdStyled);
	}

	free(p);

	return 0;
} /* gd_draw_polyline() */
