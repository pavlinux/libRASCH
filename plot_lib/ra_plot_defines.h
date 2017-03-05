#ifndef RA_PLOT_DEFINES_H
#define RA_PLOT_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RA_MAX_PATH
#define RA_MAX_PATH 4096
#endif  /* RA_MAX_PATH */

typedef void* rpl_handle;
typedef void* device_handle;

/*  */
#define TITLE_HEIGHT_MM          10
#define TIC_HEIGHT_MM             5
#define AXIS_TEXT_HEIGHT_MM      10
#define AXIS_LABEL_HEIGHT_MM     10


/* Text alignement constants */
#define ALIGN_LEFT           0x0001
#define ALIGN_RIGHT          0x0004
#define ALIGN_H_CENTER       0x0002

#define ALIGN_TOP            0x0100
#define ALIGN_BOTTOM         0x0400
#define ALIGN_V_CENTER       0x0200

#define ALIGN_CENTER         (ALIGN_H_CENTER | ALIGN_V_CENTER)


/* Font attributes */
#define FONT_BOLD     0x0001
#define FONT_ITALIC   0x0002


#define RA_PLOT_SYM_CIRCLE           0
#define RA_PLOT_SYM_CIRCLE_FULL      1
#define RA_PLOT_SYM_RECT             2
#define RA_PLOT_SYM_RECT_FULL        3
#define RA_PLOT_SYM_DIAMOND          4
#define RA_PLOT_SYM_DIAMOND_FULL     5
#define RA_PLOT_SYM_TRIANGLE         6
#define RA_PLOT_SYM_TRIANGLE_FULL    7

#define RA_PLOT_LINE_NONE        0
#define RA_PLOT_LINE_SOLID       1
#define RA_PLOT_LINE_DASH        2
#define RA_PLOT_LINE_DOT         3
#define RA_PLOT_LINE_DASHDOT     4
#define RA_PLOT_LINE_DASHDOTDOT  5

#define RA_PLOT_TIME_HHMM       4
#define RA_PLOT_TIME_HHMMSS     5
#define RA_PLOT_TIME_HHMMSS_MS  6
#define RA_PLOT_TIME_MMSS       7
#define RA_PLOT_TIME_MMSS_MS    8
#define RA_PLOT_TIME_SS_MS      9
#define RA_PLOT_TIME_DAY      100
#define RA_PLOT_TIME_WEEK     101
#define RA_PLOT_TIME_MONTH    102
#define RA_PLOT_TIME_YEAR     103
#define RA_PLOT_TIME_DDMMYY   104
#define RA_PLOT_TIME_DDMMYYYY 105

#define RA_PLOT_LOG_10  0
#define RA_PLOT_LOG_2   1
#define RA_PLOT_LOG_LN  2


#define RPL_DEVICE_TEXT   0x0001
#define RPL_DEVICE_QT     0x0002
#define RPL_DEVICE_MFC    0x0003
#define RPL_DEVICE_PS     0x0004
#define RPL_DEVICE_PDF    0x0005
#define RPL_DEVICE_JPEG   0x0006
#define RPL_DEVICE_PNG    0x0007


struct plot_infos
{
	int height_title_line;

	int height_x_label_line;
	int height_x_tics_line;

	int width_y_label_line;
	int width_y_tics_line;

	int num_y_axis;

	int plot_area_x;
	int plot_area_y;
	int plot_area_width;
	int plot_area_height;
};  /* struct plot_infos */


struct plot_device
{
	long device_id;

	rpl_handle rpl;
	long device_idx;	/* idx in device_info struct hold in rpl_instance */

	int device_ok;

	void *plot_handle;	/* holding the handle to the selected device-context */

	struct plot_infos pi;

	int (*set_painter)(device_handle dh, void *painter);
	int (*height)(device_handle dh);
	int (*width)(device_handle dh);
	int (*set_color)(device_handle dh, int r, int g, int b);
	int (*get_color)(device_handle dh, int *r, int *g, int *b);
	int (*set_line)(device_handle dh, int width, int type, int arrow);
	int (*get_line)(device_handle dh, int *width, int *type, int *arrow);
	int (*set_font)(device_handle dh, const char *family, int face, int size);
	int (*get_font)(device_handle dh, char *family, int buf_size, int *face, int *size);
	int (*set_scale)(device_handle dh, double scale_factor);
	int (*get_scale)(device_handle dh, double *scale_factor);
	int (*get_text_extent)(device_handle dh, const char *string, int *width, int *height);
	int (*set_clipping_area)(device_handle dh, int left, int top, int right, int bottom);
	int (*draw_points)(device_handle dh, int *x, int *y, int num_points, int symbol, int size);
	int (*draw_line)(device_handle dh, int xs, int ys, int xe, int ye);
	int (*draw_text)(device_handle dh, int x, int y, int width, int height, int alignment, double degree, const char *s);
	int (*draw_polyline)(device_handle dh, int *x, int *y, int num_points);
};  /* struct plot_device */


struct ra_plot_line
{
	double x_s, y_s;
	double x_e, y_e;

	int pair_idx;

	/* flag if the line should be go from the start to the end of the
	   shown x-axis at 'y_s' */
	int whole_x;
	/* flag if the line should be go from the start to the end of the
	   shown y-axis at 'x_s' */
	int whole_y;

	int line_width;
	int line_type;
	int line_arrow;
	int line_r, line_g, line_b;

	int num_text;
	char **text;
	int *text_pos;
};  /* struct ra_plot_line */


struct ra_plot_text
{
	char *string;

	double x, y;
	int x_is_line;
	int y_is_line;

	int pair_idx;

	int alignment;
	int r, g, b;
	double degree;

	char *font_name;
	int font_face;
	int font_size;
};  /* struct ra_plot_text */


struct ra_plot_axis
{
	char *name;
	char *unit;

	int use_min, use_max;
	double min, max;

	int is_time;
	int time_format;
	int plot_log;
	int log_type;

	long num_data;
	double *data;
	double *data_log;

	double tic_start;
	double tic_end;
	double tic_step;

	double pix_per_unit_x;
	double pix_per_unit_y;

	int dont_draw_axis;
	int dont_clip;
};  /* struct ra_plot_axis */


struct ra_plot_pair
{
	/* index for ra_plot_options.axis array */
	int x_idx;		/* x-axis */
	int y_idx;		/* y-axis */
	int z_idx;		/* z-axis */

	int plot_symbol;
	int symbol_type;
	int sym_r, sym_g, sym_b;
	int plot_line;
	int line_type;
	int line_r, line_g, line_b;

	int num_highlight;
	int *x_start_idx;
	int *x_end_idx;
	int highlight_line_width;
};  /* struct ra_plot_pair */


struct ra_plot_options
{
	char *title;

	int num_plot_pairs;
	struct ra_plot_pair *pair;

	int num_axis;
	struct ra_plot_axis *axis;

	int num_lines;
	struct ra_plot_line *line;

	int num_text;
	struct ra_plot_text *text;
	
	int show_grid;
	int frame_type;
	int show_legend;
};  /* struct ra_plot_options */



struct device_info
{
	char *name;
	char *desc;

	int need_add_info;

	struct plot_device *(*open_device)(rpl_handle rpl, void *add_info);
	int (*close_device)(device_handle dh);
};  /* struct device_info */


struct rpl_instance
{
	int num_devices;
	struct device_info *dev;
};  /* rpl_instance */


#ifdef __cplusplus
}
#endif

#endif  /* RA_PLOT_DEFINES_H */
