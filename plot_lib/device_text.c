#include "ra_plot_defines.h"
#include "text_device.h"


int
td_set_dev_info(struct device_info *i)
{
	i->name = malloc(sizeof(char) * (strlen(TD_NAME)+1));
	strcpy(i->name, TD_NAME);
	i->desc = malloc(sizeof(char) * (strlen(TD_DESC)+1));
	strcpy(i->desc, TD_DESC);

	i->need_add_info = 1;

	i->init_device = td_init_device;
	i->close_device = td_close_device;

	return 0;
} /* td_set_dev_info() */


device_handle
td_init_device(rpl_handle rpl, const char *add_info)
{
	struct text_device *td;
	struct plot_device *d;

	if ((add_info == NULL) || (add_info[0] == '\0'))
		return NULL;

	td = calloc(1, sizeof(struct text_device));
	td->fp = fopen(add_info, "w");
	if (td->fp == NULL)
	{
		free(td);
		return NULL;
	}
	td->fn = malloc(sizeof(char) * (strlen(add_info)+1));
	strcpy(td->fn, add_info);

	d = calloc(1, sizeof(struct plot_device));
	d->plot_handle = (void *)td;

	d->select_color = td_select_color;
	d->select_line = select_line;
	d->select_font = select_font;
	d->draw_point = draw_point;
	d->draw_line = draw_line;
	d->draw_text = draw_text;
	d->draw_polygon = draw_polygon;
	
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
td_select_color(device_handle dh, int r, int g, int b)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "select color: r=%d  g=%d  b=%d\n", r, g, b);

	return 0;
} /* td_select_color() */


int
td_select_line(device_handle dh, int width, int type, int arrow)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "select line: width=%d  type=%d  arrow=%d\n", width, type, arrow);

	return 0;
} /* td_select_line() */


int
td_select_font(device_handle dh, const char *family, int face, int size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "select font: family=%s  face=%d  size=%d\n", family, face, size);

	return 0;
} /* td_select_font() */


int
td_draw_point(device_handle dh, int *x, int *y, int num_points, int symbol, int size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "draw point: num-points=%d  symbol=%d  size=%d\n", num_points, symbol, size);

	return 0;
} /* td_draw_point() */


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
td_draw_text(device_handle dh, int x, int y, int alignment, const char *s)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "draw text: '%s' at %d/%d  alignment=%d\n", s, x, y, alignment);

	return 0;
} /* td_draw_text() */


int
td_draw_polygon(device_handle dh, int *x, int *y, int num_points)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct text_device *td;

	if (!dh)
		return -1;

	td = (struct text_device *)d->plot_handle;
	if (!td || !td->fp)
		return -1;

	fprintf(td->fp, "draw polygon: num-points=%d\n", num_points);

	return 0;
} /* td_draw_polygon() */
