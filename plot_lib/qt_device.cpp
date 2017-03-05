#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QMatrix>
#include <QtCore/QString>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ra_plot_defines.h"
#include "qt_device.h"


#define QT_NAME   "qt-device"
#define QT_DESC   "plot device for Qt"


struct qt_device
{
	QWidget *w;
	QPainter *p;
	
	int color_r, color_g, color_b;
	int line_width, line_type, line_arrows;
	QString *font_name;
	int font_face, font_size;
};   /* struct qt_device */

// prototypes 
extern "C"
{
	struct plot_device * qt_open_device(rpl_handle rpl, void *add_info);
	int qt_close_device(device_handle dh);

	int qt_height(device_handle dh);
	int qt_width(device_handle dh);
	int qt_set_painter(device_handle dh, void *painter);
	int qt_set_color(device_handle dh, int r, int g, int b);
	int qt_get_color(device_handle dh, int *r, int *g, int *b);
	int qt_set_line(device_handle dh, int width, int type, int arrow);
	int qt_get_line(device_handle dh, int *width, int *type, int *arrow);
	int qt_set_font(device_handle dh, const char *family, int face, int size);
	int qt_get_font(device_handle dh, char *family, int buf_size, int *face, int *size);
	int qt_get_text_extent(device_handle dh, const char *string, int *width, int *height);
	int qt_draw_points(device_handle dh, int *x, int *y, int num_points, int symbol, int size);
	int qt_draw_line(device_handle dh, int xs, int ys, int xe, int ye);
	int qt_draw_text(device_handle dh, int x, int y, int width, int height, int alignment, double degree, const char *s);
	int qt_draw_polyline(device_handle dh, int *x, int *y, int num_points);
}


int
qt_set_dev_info(struct device_info *i)
{
	i->name = (char *)malloc(sizeof(char) * (strlen(QT_NAME)+1));
	strcpy(i->name, QT_NAME);
	i->desc = (char *)malloc(sizeof(char) * (strlen(QT_DESC)+1));
	strcpy(i->desc, QT_DESC);

	i->need_add_info = 1;

	i->open_device = qt_open_device;
	i->close_device = qt_close_device;

	return 0;
} /* qt_set_dev_info() */


struct plot_device *
qt_open_device(rpl_handle rpl, void *add_info)
{
	struct qt_device *qt;
	struct plot_device *d;

	if ((add_info == NULL) || (((const char *)add_info)[0] == '\0'))
		return NULL;

	qt = (struct qt_device *)calloc(1, sizeof(struct qt_device));
	qt->w = (QWidget *)add_info;

	d = (struct plot_device *)calloc(1, sizeof(struct plot_device));
	d->rpl = rpl;
	d->plot_handle = (void *)qt;

	d->height = qt_height;
	d->width = qt_width;
	d->set_painter = qt_set_painter;
	d->set_color = qt_set_color;
	d->get_color = qt_get_color;
	d->set_line = qt_set_line;
	d->get_line = qt_get_line;
	d->set_font = qt_set_font;
	d->get_font = qt_get_font;
	d->get_text_extent = qt_get_text_extent;
	d->draw_points = qt_draw_points;
	d->draw_line = qt_draw_line;
	d->draw_text = qt_draw_text;
	d->draw_polyline = qt_draw_polyline;

	qt->line_width = 1;
	qt->line_type = Qt::SolidLine;
	qt->line_arrows = 0;
	qt->color_r = qt->color_g = qt->color_b = 0;

	qt->font_name = new QString();

	return d;
} /* init_device() */



int
qt_close_device(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;

	free(qt);
	d->plot_handle = NULL;

	free(d);

	return 0;
} /* qt_close_device() */


int
qt_height(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	return qt->w->height();
} /* qt_height() */


int
qt_width(device_handle dh)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	return qt->w->width();
} /* qt_width() */


int
qt_set_painter(device_handle dh, void *painter)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	qt->p = (QPainter *)painter;

	return 0;
} /* qt_set_painter() */


int
qt_set_color(device_handle dh, int r, int g, int b)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	qt->color_r = r;
	qt->color_g = g;
	qt->color_b = b;

	return 0;
} /* qt_set_color() */


int
qt_get_color(device_handle dh, int *r, int *g, int *b)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	*r = qt->color_r;
	*g = qt->color_g;
	*b = qt->color_b;

	return 0;
} /* qt_get_color() */


int
qt_set_line(device_handle dh, int width, int type, int arrow)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	qt->line_width = width;
	qt->line_type = type;
	qt->line_arrows = arrow;

	return 0;
} /* qt_set_line() */


int
qt_get_line(device_handle dh, int *width, int *type, int *arrow)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	*width = qt->line_width;
	*type = qt->line_type;
	*arrow = qt->line_arrows;

	return 0;
} /* qt_get_line() */


int
qt_set_font(device_handle dh, const char *family, int face, int size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	if (family && (family[0] != '\0'))
	{
		delete qt->font_name;
		qt->font_name = new QString(family);
	}
	qt->font_face = face;
	qt->font_size = size;

	return 0;
} /* qt_set_font() */


int
qt_get_font(device_handle dh, char *family, int buf_size, int *face, int *size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	if (buf_size > 0)
	{
		if (qt->font_name->isEmpty())
			family[0] = '\0';
		else
			strncpy(family, (const char *)(qt->font_name->toUtf8().data()), buf_size);
	}
	*face = qt->font_face;
	*size = qt->font_size;

	return 0;
} /* qt_get_font() */


int
qt_get_text_extent(device_handle dh, const char *string, int *width, int *height)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	QFontMetrics fm = qt->p->fontMetrics();
	*width = fm.width(string);
	*height = fm.height();

	return 0;
} /* qt_get_text_extent() */


int
qt_draw_points(device_handle dh, int *x, int *y, int num_points, int /*symbol*/, int size)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	qt->p->save();

	QPen pen;
	pen.setWidth(size);
	pen.setStyle(Qt::SolidLine);
	pen.setColor(QColor(qt->color_r, qt->color_g, qt->color_b));
	qt->p->setPen(pen);

	QPoint *p = new QPoint[num_points];
	for (int i = 0; i < num_points; i++)
	{
		p[i].setX(x[i]);
		p[i].setY(y[i]);
	}

	qt->p->drawPoints(p, num_points);

	delete[] p;

	qt->p->restore();

	return 0;
} /* qt_draw_points() */


int
qt_draw_line(device_handle dh, int xs, int ys, int xe, int ye)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	qt->p->save();

	QPen pen;
	pen.setWidth(qt->line_width);
	pen.setColor(QColor(qt->color_r, qt->color_g, qt->color_b));
	switch (qt->line_type)
	{
	case RA_PLOT_LINE_NONE: pen.setStyle(Qt::NoPen); break;
	case RA_PLOT_LINE_SOLID: pen.setStyle(Qt::SolidLine); break;
	case RA_PLOT_LINE_DASH: pen.setStyle(Qt::DashLine); break;
	case RA_PLOT_LINE_DOT: pen.setStyle(Qt::DotLine); break;
	case RA_PLOT_LINE_DASHDOT: pen.setStyle(Qt::DashDotLine); break;
	case RA_PLOT_LINE_DASHDOTDOT: pen.setStyle(Qt::DashDotDotLine); break;
	}
	qt->p->setPen(pen);

	qt->p->drawLine(xs, ys, xe, ye);

	qt->p->restore();

	return 0;
} /* qt_draw_line() */


int
qt_draw_text(device_handle dh, int x, int y, int width, int height, int alignment, double degree, const char *s)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;
	int qt_alignment = 0;
	int x_use = x;
	int y_use = y;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	qt->p->save();

	QPen pen;
	pen.setWidth(qt->line_width);
	pen.setStyle((Qt::PenStyle)qt->line_type);
	pen.setColor(QColor(qt->color_r, qt->color_g, qt->color_b));
	qt->p->setPen(pen);

	if (alignment & ALIGN_LEFT)
		qt_alignment |= Qt::AlignLeft;
	if (alignment & ALIGN_RIGHT)
		qt_alignment |= Qt::AlignRight;
	if (alignment & ALIGN_H_CENTER)
		qt_alignment |= Qt::AlignHCenter;
	if (alignment & ALIGN_TOP)
		qt_alignment |= Qt::AlignTop;
	if (alignment & ALIGN_BOTTOM)
		qt_alignment |= Qt::AlignBottom;
	if (alignment & ALIGN_V_CENTER)
		qt_alignment |= Qt::AlignVCenter;

	if (degree != 0.0)
	{
		qt->p->translate(x, y);
		qt->p->rotate(degree);
		x_use = 0;
		y_use = 0;
		
		if ((degree == 90) || (degree == 270))
		{
			int t = width;
			width = height;
			height = t;

			qt_alignment = 0;
			if (alignment & ALIGN_LEFT)
				qt_alignment |= Qt::AlignTop;
			if (alignment & ALIGN_RIGHT)
				qt_alignment |= Qt::AlignBottom;
			if (alignment & ALIGN_H_CENTER)
				qt_alignment |= Qt::AlignVCenter;
			if (alignment & ALIGN_TOP)
				qt_alignment |= Qt::AlignLeft;
			if (alignment & ALIGN_BOTTOM)
				qt_alignment |= Qt::AlignRight;
			if (alignment & ALIGN_V_CENTER)
				qt_alignment |= Qt::AlignHCenter;
		}
	}

 	qt->p->drawText(x_use, y_use, width, height, qt_alignment, s);

	qt->p->restore();

	return 0;
} /* qt_draw_text() */


int
qt_draw_polyline(device_handle dh, int *x, int *y, int num_points)
{
	struct plot_device *d = (struct plot_device *)dh;
	struct qt_device *qt;

	if (!dh)
		return -1;

	qt = (struct qt_device *)d->plot_handle;
	if (!qt)
		return -1;

	qt->p->save();

	QPen pen;
	pen.setWidth(qt->line_width);
	pen.setColor(QColor(qt->color_r, qt->color_g, qt->color_b));
	switch (qt->line_type)
	{
	case RA_PLOT_LINE_NONE: pen.setStyle(Qt::NoPen); break;
	case RA_PLOT_LINE_SOLID: pen.setStyle(Qt::SolidLine); break;
	case RA_PLOT_LINE_DASH: pen.setStyle(Qt::DashLine); break;
	case RA_PLOT_LINE_DOT: pen.setStyle(Qt::DotLine); break;
	case RA_PLOT_LINE_DASHDOT: pen.setStyle(Qt::DashDotLine); break;
	case RA_PLOT_LINE_DASHDOTDOT: pen.setStyle(Qt::DashDotDotLine); break;
	}
	qt->p->setPen(pen);

	QPoint *p = new QPoint[num_points];
	for (int i = 0; i < num_points; i++)
	{
		p[i].setX(x[i]);
		p[i].setY(y[i]);
	}

	qt->p->drawPolyline(p, num_points);

	delete[] p;

	qt->p->restore();

	return 0;
} /* qt_draw_polyline() */
