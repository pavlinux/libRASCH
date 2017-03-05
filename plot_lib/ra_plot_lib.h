#ifndef RA_PLOT_LIB_H
#define RA_PLOT_LIB_H

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#ifndef _RA_PLOT_LIB_BUILD
#define RA_PLOT_API __declspec(dllimport)
#else
#define RA_PLOT_API __declspec(dllexport)
#endif
#else
#define RA_PLOT_API
#endif

#include "ra_plot_defines.h"

#ifdef __cplusplus
extern "C" {
#endif


#define GD_PNG    0
#define GD_JPEG   1

struct gd_add_info
{
	int width;
	int height;

	double scale_factor;

	int img_type;

	char file_name[RA_MAX_PATH];
};  /* struct gd_add_info */



RA_PLOT_API rpl_handle rpl_init();
RA_PLOT_API void rpl_close(rpl_handle rpl);
RA_PLOT_API long rpl_num_devices(rpl_handle rpl);
RA_PLOT_API int rpl_device_info(rpl_handle rpl, int device_idx, long info_id);

RA_PLOT_API device_handle rpl_open_device_idx(rpl_handle rpl, int device_idx, void *add_info);
RA_PLOT_API device_handle rpl_open_device(rpl_handle rpl, const char *name, void *add_info);
RA_PLOT_API long rpl_get_option_id(const char *option_ascii_id);
/* RA_PLOT_API int rpl_set_option(rpl_handle rpl, long option_id, value_handle vh); */
/* RA_PLOT_API int rpl_get_option(rpl_handle rpl, long option_id, value_handle vh); */
/* RA_PLOT_API int rpl_set_device_option(device_handle long option_id, value_handle vh); */
/* RA_PLOT_API int rpl_get_device_option(device_handle long option_id, value_handle vh); */
RA_PLOT_API int rpl_close_device(device_handle dh);

RA_PLOT_API int rpl_set_device_painter(device_handle dh, void *painter);

RA_PLOT_API long rpl_device_height(device_handle dh);
RA_PLOT_API long rpl_device_width(device_handle dh);

RA_PLOT_API int rpl_set_color(device_handle dh, int r, int g, int b);
RA_PLOT_API int rpl_get_color(device_handle dh, int *r, int *g, int *b);
RA_PLOT_API int rpl_set_line(device_handle dh, int width, int type, int arrow);
RA_PLOT_API int rpl_get_line(device_handle dh, int *width, int *type, int *arrow);
RA_PLOT_API int rpl_set_font(device_handle dh, const char *family, int face, int size);
RA_PLOT_API int rpl_get_font(device_handle dh, char *family, int buf_size, int *face, int *size);
RA_PLOT_API int rpl_set_scale(device_handle dh, double scale_factor);
RA_PLOT_API int rpl_get_scale(device_handle dh, double *scale_factore);

RA_PLOT_API int rpl_get_text_extent(device_handle dh, const char *string, int *width, int *height);

RA_PLOT_API int rpl_set_clipping_area(device_handle dh, int left, int top, int right, int bottom);

RA_PLOT_API int rpl_draw_points(device_handle dh, int *x, int *y, int num_points, int symbol, int size);
RA_PLOT_API int rpl_draw_line(device_handle dh, int xs, int ys, int xe, int ye);
RA_PLOT_API int rpl_draw_text(device_handle dh, int x, int y, int width, int height, int alignment, double degree, const char *s);
RA_PLOT_API int rpl_draw_polyline(device_handle dh, int *x, int *y, int num_points);

RA_PLOT_API int rpl_point2value(device_handle dh, struct ra_plot_options *opt, int pair_idx, int x, int y, double *x_value, double *y_value);

RA_PLOT_API int rpl_scattergram(device_handle dh, struct ra_plot_options *opt);


#ifdef __cplusplus
}
#endif

#endif  /* RA_PLOT_LIB_H */
