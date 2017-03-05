#ifndef SCATTERGRAM_H
#define SCATTERGRAM_H

#define BORDER_X_LEFT    2
#define BORDER_X_RIGHT  15
#define BORDER_Y         5

int check_infos(struct ra_plot_options *opt);
int init_axis(struct ra_plot_options *opt);
int set_axis_ticks(struct ra_plot_axis *axis);
double get_step(double range);
double my_raise(double x, int y);
int get_tics_precision(double step);
int fill_plot_infos(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf);
int plot_axis(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf);
int plot_x_axis(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf);
int plot_y_axis(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf);
int plot_data(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf);
int format_tic(double value, struct ra_plot_axis *a, int precision, char *tic, size_t buf_size);

int plot_lines(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf);
int plot_text(device_handle dh, struct ra_plot_options *opt, struct plot_infos *inf);


#endif  /* SCATTERGRAM_H */
