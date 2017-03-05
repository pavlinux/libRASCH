#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define _RA_PLOT_LIB_BUILD
#include "ra_plot_lib.h"
#include "scattergram.h"

#ifdef WIN32
#define snprintf _snprintf
#endif


int
rpl_scattergram(device_handle dh, struct ra_plot_options *opt)
{
	int ret = 0;
	struct plot_device *pd = (struct plot_device *)dh;

	if (!dh)
		return -1;

	if ((ret = check_infos(opt)) != 0)
		return ret;

	if ((ret = init_axis(opt)) != 0)
		return ret;

	if ((ret = fill_plot_infos(dh, opt, &(pd->pi))) != 0)
		return ret;

	if ((ret = plot_axis(dh, opt, &(pd->pi))) != 0)
		return ret;

	if ((ret = plot_data(dh, opt, &(pd->pi))) != 0)
		return ret;

	if ((ret = plot_lines(dh, opt, &(pd->pi))) != 0)
		return ret;

	if ((ret = plot_text(dh, opt, &(pd->pi))) != 0)
		return ret;

	return ret;
}  /* scattergram() */


int
check_infos(struct ra_plot_options *opt)
{
	if (opt);

	return 0;
} /* check_infos() */


int
init_axis(struct ra_plot_options *opt)
{
	int ret = 0;
	int i;

	for (i = 0; i < opt->num_axis; i++)
	{
		if ((ret = set_axis_ticks(opt->axis+i)) != 0)
			break;
	}

	return ret;
}  /* init_axis() */


int
set_axis_ticks(struct ra_plot_axis *axis)
{
	int ret = 0;
	long l;
	double range;

	if (axis->data_log)
	{
		free(axis->data_log);
		axis->data_log = NULL;
	}

	if ((axis->num_data <= 0) || (axis->data == NULL))
		return -1;

	if (axis->use_min && axis->use_max)
	{
		axis->tic_start = axis->min;
		axis->tic_end = axis->max;
	}
	else
	{
		axis->tic_start = axis->data[0];
		for (l = 0; l < axis->num_data; l++)
		{
			if (axis->tic_start > axis->data[l])
				axis->tic_start = axis->data[l];
			if (axis->tic_end < axis->data[l])
				axis->tic_end = axis->data[l];			
		}
		if (axis->use_min)
			axis->tic_start = axis->min;
		if (axis->use_max)
			axis->tic_end = axis->max;
	}

	range = axis->tic_end - axis->tic_start;
	axis->tic_step = get_step(range);
	axis->tic_start = floor(axis->tic_start / axis->tic_step) * axis->tic_step;
	axis->tic_end = ceil(axis->tic_end / axis->tic_step) * axis->tic_step;
	
	return ret;
}  /* set_axis_ticks() */



double
get_step(double range)
{
	double abs_range, log_range, norm, div, step;
	int n;

	abs_range = fabs(range);
	log_range = log10(abs_range);

	norm = pow(10.0, log_range - (log_range >= 0.0 ? log_range : (log_range - 1.0)));
	if (norm <= 2.0)
		div = 0.2;
	else if (norm <= 5.0)
		div = 0.5;
	else
		div = 1.0;

	step = div * my_raise(10.0, (int)(log_range >= 0.0 ? log_range : (log_range - 1.0)));
	n = (int)(abs_range / step);
	while (n > 10)
	{
		step *= 2;
		n = (int)(abs_range / step);
	}

	return step;
}  /* get_step() */


double
my_raise(double x, int y)
{
	double val;
	int i;

	val = 1.0;
	for (i = 0; i < abs(y); i++)
		val *= x;
	if (y < 0)
		return (1.0 / val);
	else
		return val;
}  /* my_raise() */


int
get_tics_precision(double step)
{
	int l;

	if (step >= 1)
		return 0;

	l = (int)(log10(step) - 1);
	l = abs(l);

	return l;
} /* get_tics_precision() */


int
fill_plot_infos(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf)
{
	int h, w;
	int i, j;

	inf->height_title_line = 30;

	inf->height_x_label_line = 20;
	inf->height_x_tics_line = 20;

	inf->width_y_label_line = 20;
	inf->width_y_tics_line = 20;

	h = rpl_device_height(dh);
	w = rpl_device_width(dh);

	inf->num_y_axis = 0;
	for (i = 0; i < opt->num_plot_pairs; i++)
	{
		int already_handled = 0;

		if (opt->axis[opt->pair[i].y_idx].dont_draw_axis)
			continue;

		for (j = i+1; j < opt->num_plot_pairs; j++)
		{
			if (opt->axis[opt->pair[j].y_idx].dont_draw_axis)
				continue;

			if ((opt->axis[opt->pair[i].y_idx].unit[0] != '\0') &&
			    (strcmp(opt->axis[opt->pair[i].y_idx].unit, opt->axis[opt->pair[j].y_idx].unit) == 0))
			{
				already_handled = 1;
				break;
			}
		}
		if (already_handled)
		{
			opt->axis[opt->pair[i].y_idx].dont_draw_axis = 1;
			continue;
		}

		inf->num_y_axis += 1;
	}

	inf->plot_area_x = ((inf->width_y_label_line + inf->width_y_tics_line) * inf->num_y_axis) + BORDER_X_LEFT;
	inf->plot_area_y = inf->height_title_line + BORDER_Y;
	inf->plot_area_width = w - inf->plot_area_x - BORDER_X_RIGHT;
	inf->plot_area_height = h - inf->plot_area_y - (inf->height_x_label_line + inf->height_x_tics_line) - BORDER_Y;	

	for (i = 0; i < opt->num_axis; i++)
	{
		opt->axis[i].pix_per_unit_x = (double)(inf->plot_area_width) /
			(double)(opt->axis[i].tic_end - opt->axis[i].tic_start);

		opt->axis[i].pix_per_unit_y = (double)(inf->plot_area_height) /
			(double)(opt->axis[i].tic_end - opt->axis[i].tic_start);
	}

	return 0;
} /* fill_plot_infos() */


int
plot_axis(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf)
{
	int ret = 0;

	if ((ret = plot_x_axis(dh, opt, inf)) != 0)
		return ret;
 	if ((ret = plot_y_axis(dh, opt, inf)) != 0)
 		return ret;

	return ret;
}  /* plot_axis() */


int
plot_x_axis(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf)
{
	int ret = 0;
	struct ra_plot_axis *a;
	double curr;
	int precision;
	char tic[100];

	a = &(opt->axis[opt->pair[0].x_idx]);

	/* draw upper and lower frame lines */
	rpl_draw_line(dh, inf->plot_area_x, inf->plot_area_y,
		      inf->plot_area_x + inf->plot_area_width, inf->plot_area_y);
	rpl_draw_line(dh, inf->plot_area_x, inf->plot_area_y + inf->plot_area_height,
		      inf->plot_area_x + inf->plot_area_width, inf->plot_area_y + inf->plot_area_height);

	/* draw tic lines and numbers for the x axis */
	curr = a->tic_start;
	precision = get_tics_precision(a->tic_step);
	while (curr <= a->tic_end)
	{
		int x, y;

		x = (int)((curr - a->tic_start) * a->pix_per_unit_x);
		x += inf->plot_area_x;
		y = inf->plot_area_y + inf->plot_area_height;

		rpl_draw_line(dh, x, y, x, y + 5);

		format_tic(curr, a, precision, tic, 100);

		rpl_draw_text(dh, x-50, y + 7, 100, 20, ALIGN_TOP|ALIGN_H_CENTER, 0.0, tic);

		curr += a->tic_step;
	}

	/* draw unit */
	if (a->name)
	{
		char *s;
		
		s = malloc(sizeof(char) * (strlen(a->name) + (a->unit ? strlen(a->unit) : 0) + 4));
		if (a->unit)
			sprintf(s, "%s [%s]", a->name, a->unit);
		else
			strcpy(s, a->name);
			
		rpl_draw_text(dh, inf->plot_area_x,
			      inf->plot_area_y + inf->plot_area_height + inf->height_x_tics_line, 
			      inf->plot_area_width, inf->height_x_label_line, ALIGN_TOP|ALIGN_H_CENTER, 0.0, s);

		free(s);
	}
	
	return ret;
}  /* plot_x_axis() */


int
plot_y_axis(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf)
{
	int i;
	int curr_left, curr_right, left_side;
	int axis_width;
	struct ra_plot_axis *a;
	double curr, step;
	int precision;
	char tic[100];

	/* draw left and right frame lines */
	rpl_draw_line(dh, inf->plot_area_x, inf->plot_area_y,
		      inf->plot_area_x, inf->plot_area_y + inf->plot_area_height);
	rpl_draw_line(dh, inf->plot_area_x + inf->plot_area_width, inf->plot_area_y,
		      inf->plot_area_x + inf->plot_area_width, inf->plot_area_y + inf->plot_area_height);

	axis_width = inf->width_y_label_line + inf->width_y_tics_line;

	curr_left = curr_right = 0;
	left_side = 1;
	for (i = 0; i < opt->num_plot_pairs; i++)
	{
		int x;

		if (opt->axis[opt->pair[i].y_idx].dont_draw_axis)
			continue;

		if (left_side)
		{
			x = inf->plot_area_x - (curr_left * axis_width);
			curr_left++;
		}
		else
		{
			x = inf->plot_area_x + inf->plot_area_width + (curr_right * axis_width);
			curr_right++;
		}

		/* draw axis line */
		rpl_draw_line(dh, x, inf->plot_area_y, x, inf->plot_area_y + inf->plot_area_height);

		a = &(opt->axis[opt->pair[i].y_idx]);
		
		curr = a->tic_start;
		step = a->tic_step;
 		if (inf->plot_area_height < 200)
 			step *= 4.0;
		else if (inf->plot_area_height < 300)
 			step *= 3.0;
		else if (inf->plot_area_height < 400)
 			step *= 2.0;

		precision = get_tics_precision(a->tic_step);

		while (curr <= a->tic_end)
		{
			int y;

			y = (int)((a->tic_end - curr) * a->pix_per_unit_y);
			y += inf->plot_area_y;
			
			rpl_draw_line(dh, x, y, x - 5, y);
			
			format_tic(curr, a, precision, tic, 100);
			
			rpl_draw_text(dh, x-inf->width_y_label_line, y+30,
				      inf->width_y_tics_line, 60, ALIGN_V_CENTER|ALIGN_RIGHT, 270.0, tic);
			
			curr += step;
		}

		/* draw unit */
		if (a->name)
		{
 			char *s;
			
 			s = malloc(sizeof(char) * (strlen(a->name) + (a->unit ? strlen(a->unit) : 0) + 4));
 			if (a->unit)
 				sprintf(s, "%s [%s]", a->name, a->unit);
 			else
 				strcpy(s, a->name);
			
 			rpl_draw_text(dh, inf->plot_area_x - inf->width_y_tics_line - inf->width_y_label_line,
 				      inf->plot_area_y + inf->plot_area_height,
 				      inf->width_y_label_line, inf->plot_area_height, ALIGN_RIGHT|ALIGN_V_CENTER, 270.0, s);
			
			free(s);
		}
		
		left_side = (left_side + 1) % 2;
	}

	return 0;
} /* plot_y_axis() */


int
format_tic(double value, struct ra_plot_axis *a, int precision, char *tic, size_t buf_size)
{
	int h, m, s, ms;
	double t;

	t = value;

	if (a->is_time != 1)
	{
		switch (precision)
		{
		case 0:
			snprintf(tic, buf_size, "%.0f", t);
			break;
		case 1:
			snprintf(tic, buf_size, "%.1f", t);
			break;
		case 2:
			snprintf(tic, buf_size, "%.2f", t);
			break;
		case 3:
			snprintf(tic, buf_size, "%.3f", t);
			break;
		case 4:
			snprintf(tic, buf_size, "%.4f", t);
			break;
		case 5:
			snprintf(tic, buf_size, "%.5f", t);
			break;
		default:
			snprintf(tic, buf_size, "%f", t);
			break;
		}

		return 0;
	}

	/* it is a time format */
	switch (a->time_format)
	{
	case RA_PLOT_TIME_HHMM:
		h = (int)t;
		t = (t - (double)h) * 60.0;
		m = (int)t;

		h = h % 24;
		snprintf(tic, buf_size, "%02d:%02d", h, m);
		break;
	case RA_PLOT_TIME_HHMMSS:		
		h = (int)t;
		t = (t - (double)h) * 60.0;
		m = (int)t;
		t = (t - (double)m) * 60.0;
		s = (int)t;

		h = h % 24;
		snprintf(tic, buf_size, "%02d:%02d:%02d", h, m, s);
		break;
	case RA_PLOT_TIME_HHMMSS_MS:
		h = (int)t;
		t = (t - (double)h) * 60.0;
		m = (int)t;
		t = (t - (double)m) * 60.0;
		s = (int)t;
		t = (t - (double)s) * 1000.0;
		ms = (int)t;

		h = h % 24;
		snprintf(tic, buf_size, "%02d:%02d:%02d.%d", h, m, s, ms);
		break;
	case RA_PLOT_TIME_MMSS:
		m = (int)t;
		t = (t - (double)m) * 60.0;
		s = (int)t;
		snprintf(tic, buf_size, "%02d:%02d", m, s);
		break;
	case RA_PLOT_TIME_MMSS_MS:
		m = (int)t;
		t = (t - (double)m) * 60.0;
		s = (int)t;
		t = (t - (double)s) * 1000.0;
		ms = (int)t;
		snprintf(tic, buf_size, "%02d:%02d.%d", m, s, ms);
		break;
	case RA_PLOT_TIME_SS_MS:
		s = (int)t;
		t = (t - (double)s) * 1000.0;
		ms = (int)t;
		snprintf(tic, buf_size, "%02d.%d", s, ms);
		break;
	}

	return 0;
} /* format_tic() */


int
plot_data(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf)
{
	int ret = 0;
	int i, j;
	struct ra_plot_axis *axis_x, *axis_y;
	int *x, *y;
	int r, g, b;
	int w, t, a;

	for (i = 0; i < opt->num_plot_pairs; i++)
	{
		axis_x = &(opt->axis[opt->pair[i].x_idx]);
		axis_y = &(opt->axis[opt->pair[i].y_idx]);

		if (axis_x->num_data != axis_y->num_data)
			continue;

		x = (int *)malloc(sizeof(int) * axis_x->num_data);
		y = (int *)malloc(sizeof(int) * axis_y->num_data);

		for (j = 0; j < axis_x->num_data; j++)
		{
			x[j] = (int)((axis_x->data[j] - axis_x->tic_start) * axis_x->pix_per_unit_x) + inf->plot_area_x;
			y[j] = (int)((axis_y->tic_end - axis_y->data[j]) * axis_y->pix_per_unit_y) + inf->plot_area_y;
		}

		/* use clipping; TODO: think how to make it an user-option */
		rpl_set_clipping_area(dh, inf->plot_area_x, inf->plot_area_y, inf->plot_area_x + inf->plot_area_width,
				      inf->plot_area_y + inf->plot_area_height);

		if (opt->pair[i].plot_line)
		{
			rpl_get_color(dh, &r, &g, &b);
			rpl_set_color(dh, opt->pair[i].line_r, opt->pair[i].line_g, opt->pair[i].line_b);
			rpl_draw_polyline(dh,x, y, axis_x->num_data);
			rpl_set_color(dh, r, g, b);
		}

		for (j = 0; j < opt->pair[i].num_highlight; j++)
		{
			int s, e, num;
			
			s = opt->pair[i].x_start_idx[j];
			if (s < 0)
				s = 0;
			e = opt->pair[i].x_end_idx[j];
			if (e >= axis_x->num_data)
				e = axis_x->num_data - 1;
			
			num = e - s + 1;

			rpl_get_color(dh, &r, &g, &b);
			rpl_set_color(dh, opt->pair[i].line_r, opt->pair[i].line_g, opt->pair[i].line_b);
			rpl_get_line(dh, &w, &t, &a);
			rpl_set_line(dh, opt->pair[i].highlight_line_width, t, a);
			
			rpl_draw_polyline(dh, x+s, y+s, num);

			rpl_set_color(dh, r, g, b);
			rpl_set_line(dh, w, t, a);
		}

		rpl_set_clipping_area(dh, 0, 0, rpl_device_width(dh), rpl_device_height(dh));

		free(x);
		free(y);
	}
	
	return ret;
}  /* plot_data() */


int
plot_lines(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf)
{
	int ret = 0;
	int i;
	int r, g, b;
	int width, type, arrow;

	for (i = 0; i < opt->num_lines; i++)
	{
		double xs, ys, xe, ye;
		struct ra_plot_axis *a;

		a = &(opt->axis[opt->pair[opt->line[i].pair_idx].x_idx]);

		xs = (opt->line[i].x_s - a->tic_start) * a->pix_per_unit_x;
		xs += inf->plot_area_x;
		xe = (opt->line[i].x_e - a->tic_start) * a->pix_per_unit_x;
		xe += inf->plot_area_x;

		a = &(opt->axis[opt->pair[opt->line[i].pair_idx].y_idx]);
		ys = (a->tic_end - opt->line[i].y_s) * a->pix_per_unit_y;
		ys += inf->plot_area_y;
		ye = (a->tic_end - opt->line[i].y_e) * a->pix_per_unit_y;
		ye += inf->plot_area_y;

		if (opt->line[i].whole_x)
		{
			xs = inf->plot_area_x;
			xe = xs + inf->plot_area_width;
			ye = ys;
		}
		if (opt->line[i].whole_y)
		{
			ys = inf->plot_area_y;
			ye = ys + inf->plot_area_height;
			xe = xs;
		}

		rpl_get_color(dh, &r, &g, &b);
		rpl_get_line(dh, &width, &type, &arrow);

		rpl_set_color(dh, opt->line[i].line_r, opt->line[i].line_g, opt->line[i].line_b);
 		rpl_set_line(dh, opt->line[i].line_width, opt->line[i].line_type, opt->line[i].line_arrow);

		rpl_draw_line(dh, (int)xs, (int)ys, (int)xe, (int)ye);

		rpl_set_color(dh, r, g, b);
 		rpl_set_line(dh, width, type, arrow);
	}

	return ret;
} /* plot_lines() */


int
plot_text(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf)
{
	int ret = 0;
	int i;
	int r, g, b;
	char *font_name;
	int font_size, font_face;

	font_name = (char *)malloc(sizeof(char) * 100);

	for (i = 0; i < opt->num_text; i++)
	{
		double x, y;
		struct ra_plot_axis *a;
		int width, height;

		/* first we set the text properties */
		rpl_get_color(dh, &r, &g, &b);
		rpl_get_font(dh, font_name, 100, &font_face, &font_size);

		rpl_set_color(dh, opt->text[i].r, opt->text[i].g, opt->text[i].b);
 		rpl_set_font(dh, opt->text[i].font_name, opt->text[i].font_face, opt->text[i].font_size);

		/* and now we can get the size of the drawn string */
		rpl_get_text_extent(dh, opt->text[i].string, &width, &height);

		/* now calculate the x- and y-postion */
		a = &(opt->axis[opt->pair[opt->text[i].pair_idx].x_idx]);
		x = (opt->text[i].x - a->tic_start) * a->pix_per_unit_x;
		x += inf->plot_area_x;

		a = &(opt->axis[opt->pair[opt->text[i].pair_idx].y_idx]);
		y = (a->tic_end - opt->text[i].y) * a->pix_per_unit_y;
		y += inf->plot_area_y;

		if (opt->text[i].x_is_line)
		{
			x = inf->plot_area_x;
			x += ((double)height * opt->text[i].x);
		}
		if (opt->text[i].y_is_line)
		{
			y = inf->plot_area_y;
			y += ((double)height * opt->text[i].y);
		}

		if (opt->text[i].alignment & ALIGN_RIGHT)
			x -= width;

		rpl_draw_text(dh, (int)x, (int)y, width, height, opt->text[i].alignment, opt->text[i].degree, opt->text[i].string);

		/* restore drawing properties */
		rpl_set_color(dh, r, g, b);
 		rpl_set_font(dh, font_name, font_face, font_size);
	}

	free(font_name);

	return ret;
} /* plot_text() */
