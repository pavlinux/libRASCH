#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QWorkspace>
#include <QtGui/QPainter>

#include <stdio.h>
#include "ra_plot_lib.h"

#include <ra_pp.h>

#define TEST_TEXT   0
#define TEST_GD     0
#define TEST_QT     1


int test1(device_handle dh);
int test_scattergram1(device_handle dh);
int test_scattergram2(device_handle dh);


class my_widget : public QWidget
{
public:
        my_widget(QWidget *parent = 0);
	void set_device_handle(device_handle dh) { _dh = dh; }
protected:
	virtual void paintEvent(QPaintEvent * ev);

	device_handle _dh;
};

my_widget::my_widget(QWidget *parent)
        : QWidget(parent)
{
        resize(200, 120);
	_dh = NULL;
}

void
my_widget::paintEvent(QPaintEvent * ev)
{
	if (_dh == NULL)
		return;
	
	QPainter p(this);
	rpl_set_device_painter(_dh, &p);
	
//	test1(_dh);
//	test_scattergram1(_dh);
	test_scattergram2(_dh);
}




int
main(int argc, char *argv[])
{
	rpl_handle rpl;
	device_handle dh;
	struct gd_add_info gd_info;

	rpl = rpl_init();

	// test text-device
#if TEST_TEXT
	dh = rpl_open_device(rpl, "text-device", (void *)"./test_text_device.txt");
	if (dh)
	{
//		test1(dh);
//		test_scattergram1(dh);
		test_scattergram2(dh);
		rpl_close_device(dh);
	}
	else
		fprintf(stderr, "Can't get text-device");
#endif  // TEST_TEXT

	
	// test gd-device
#if TEST_GD
	gd_info.width = 500;
	gd_info.height = 350;
	gd_info.img_type = GD_PNG;
	strcpy(gd_info.file_name, "./test_gd_device.png");
	dh = rpl_open_device(rpl, "gd-device", (void *)&gd_info);
	if (dh)
	{
//		test1(dh);
//		test_scattergram1(dh);
		test_scattergram2(dh);
		rpl_close_device(dh);
	}
	else
		fprintf(stderr, "Can't get gd-device");
#endif  // TEST_GD       


	// test qt-device
#if TEST_QT
	QApplication app(argc, argv);
	my_widget w;
	dh = rpl_open_device(rpl, "qt-device", &w);
	if (dh)
	{
		w.set_device_handle(dh);
		w.show();

		app.exec();

		rpl_close_device(dh);
	}
	else
		fprintf(stderr, "Can't get qt-device");
#endif // TEST_QT

	rpl_close(rpl);

	return 0;
} /* main() */


int
test1(device_handle dh)
{
	int x[100], y[100];

	for (int i = 0; i < 100; i++)
	{
		x[i] = i+10;
		y[i] = (int)((double)rand() * (100.0/(double)RAND_MAX));
	}


	rpl_set_color(dh, 255, 0, 0);
	rpl_draw_line(dh, 1, 1, 10, 10);

	rpl_set_color(dh, 0, 255, 0);
	rpl_draw_polyline(dh, x, y, 100);

	rpl_set_color(dh, 0, 0, 0);
	rpl_draw_text(dh, 10, 10, 100, 30, 0, 0.0, "Test String");

	return 0;
} /* test1() */


int
test_scattergram1(device_handle dh)
{
	struct ra_plot_options opt;
	int i;

	rasch r;
	rasch_meas meas = r.open_meas("/home/rasch/ecgs/AT011030_rdi.ART");
	rasch_rec rec = meas.get_first_rec();
	rasch_channel ch = rec.get_channel(0);
	vector<double> v = ch.get_raw_unit(0, 1600);	

	opt.title = new char[100];
	strcpy(opt.title, "Scattergram test");

	opt.num_axis = 2;
	opt.axis = new struct ra_plot_axis[opt.num_axis];
	memset(opt.axis, 0, sizeof(struct ra_plot_axis) * opt.num_axis);

	opt.axis[0].name = new char[100];
	strcpy(opt.axis[0].name, "x-axis");
	opt.axis[0].unit = new char[100];
	strcpy(opt.axis[0].unit, "unit-x");
	opt.axis[0].use_min = 0;
	opt.axis[0].use_max = 0;
	opt.axis[0].min = 0.0;
	opt.axis[0].max = 1600.0;
	opt.axis[0].is_time = 0;
	opt.axis[0].time_format = 0;
	opt.axis[0].plot_log = 0;
	opt.axis[0].log_type = 0;

	opt.axis[0].num_data = 1600;
	opt.axis[0].data = new double[opt.axis[0].num_data];

	opt.axis[1].name = new char[100];
	strcpy(opt.axis[1].name, "y-axis");
	opt.axis[1].unit = new char[100];
	strcpy(opt.axis[1].unit, "unit-y");
	opt.axis[1].use_min = 0;
	opt.axis[1].use_max = 0;
	opt.axis[1].min = -1000.0;
	opt.axis[1].max = 1000.0;
	opt.axis[1].is_time = 0;
	opt.axis[1].time_format = 0;
	opt.axis[1].plot_log = 0;
	opt.axis[1].log_type = 0;

	opt.axis[1].num_data = 1600;
	opt.axis[1].data = new double[opt.axis[1].num_data];

	for (i = 0; i < opt.axis[0].num_data; i++)
	{
		opt.axis[0].data[i] = i;
// 		opt.axis[1].data[i] = (int)((double)rand() * (100.0/(double)RAND_MAX));
		opt.axis[1].data[i] = v[i];
	}


	opt.num_plot_pairs = 1;
	opt.pair = new struct ra_plot_pair[opt.num_plot_pairs];
	memset(opt.pair, 0, sizeof(struct ra_plot_pair) * opt.num_plot_pairs);

	opt.pair[0].x_idx = 0;
	opt.pair[0].y_idx = 1;
	opt.pair[0].z_idx = -1;
	opt.pair[0].plot_symbol = 0;
	opt.pair[0].symbol_type = 0;
	opt.pair[0].sym_r = 0;
	opt.pair[0].sym_g = 0;
	opt.pair[0].sym_b = 0;
	opt.pair[0].plot_line = 1;
	opt.pair[0].line_type = 0;
	opt.pair[0].line_r = 0;
	opt.pair[0].line_g = 0;
	opt.pair[0].line_b = 255;


	opt.show_grid = 1;
	opt.frame_type = 0;	// not defined yet
	opt.show_legend = 1;

	// show lines
	opt.num_lines = 3;
	opt.line = new struct ra_plot_line[opt.num_lines];
	memset(opt.line, 0, sizeof(struct ra_plot_line) * opt.num_lines);

	opt.line[0].x_s = 100;
	opt.line[0].y_s = 700;
	opt.line[0].x_e = 1500;
	opt.line[0].y_e = -700;
	opt.line[0].pair_idx = 0;
	opt.line[0].line_width = 2;
	opt.line[0].line_type = RA_PLOT_LINE_SOLID;

	opt.line[1].x_s = 100;
	opt.line[1].y_s = 700;
	opt.line[1].whole_x = 1;
	opt.line[1].pair_idx = 0;
	opt.line[1].line_width = 4;
	opt.line[1].line_type = RA_PLOT_LINE_DASH;

	opt.line[2].x_s = 100;
	opt.line[2].y_s = 700;
	opt.line[2].whole_y = 1;
	opt.line[2].pair_idx = 0;
	opt.line[2].line_width = 6;
	opt.line[2].line_type = RA_PLOT_LINE_DOT;
	opt.line[2].line_r = 255;
	opt.line[2].line_g = 0;
	opt.line[2].line_b = 0;


	// show text
	opt.num_text = 2;
	opt.text = new struct ra_plot_text[opt.num_text];
	memset(opt.text, 0, sizeof(struct ra_plot_text) * opt.num_text);
	opt.text[0].x = 200;
	opt.text[0].y = 500;
	opt.text[0].string = new char[100];
	sprintf(opt.text[0].string, "test123");
	opt.text[0].font_face = opt.text[0].font_size = -1;

	opt.text[1].x = 200;
	opt.text[1].y = 1;
	opt.text[1].y_is_line = 1;
	opt.text[1].string = new char[100];
	sprintf(opt.text[1].string, "blablabla");
	opt.text[1].font_face = opt.text[1].font_size = -1;

	rpl_scattergram(dh, &opt);

	delete[] opt.axis[0].name;
	delete[] opt.axis[0].unit;
	delete[] opt.axis[0].data;
	delete[] opt.axis[1].name;
	delete[] opt.axis[1].unit;
	delete[] opt.axis[1].data;

	delete[] opt.text[0].string;
	delete[] opt.text[1].string;

	delete[] opt.text;
	delete[] opt.line;
	delete[] opt.pair;
	delete[] opt.axis;
	delete[] opt.title;

	return 0;
} /* test_scattergram1() */


int
test_scattergram2(device_handle dh)
{
	rasch r;
	rasch_meas meas = r.open_meas("/home/rasch/ecgs/AT011030_rdi.ART");
//	rasch_meas meas = r.open_meas("/home/rasch/ecgs/2700a554.ful");
	rasch_eval eval = meas.get_default_eval();
	vector<rasch_class> cl = eval.get_class("heartbeat");
	if (cl.size() <= 0)
		return 0;

	rasch_prop prop_pos = cl[0].get_prop("qrs-pos");
	rasch_prop prop_rri = cl[0].get_prop("rri");

	rasch_value ids, chs, pos, rri;
	prop_pos.get_value_all(ids, chs, pos);
	prop_rri.get_value_all(ids, chs, rri);

	double samplerate = meas.get_info(RA_INFO_MAX_SAMPLERATE_D).get_value_d();
	rasch_rec rec = meas.get_first_rec();
	string rec_start = rec.get_info(RA_INFO_REC_GEN_TIME_C).get_value_c();
	
	const char *s = rec_start.c_str();
	double rec_start_ms = (double)atoi(s) * (60.0 * 60.0 * 1000.0);
	rec_start_ms += (double)atoi(s+3) * (60.0 * 1000.0);
	rec_start_ms += (double)atoi(s+6) * 1000.0;
	
	struct ra_plot_options opt;
	memset(&opt, 0, sizeof(struct ra_plot_options));

	opt.title = new char[100];
	strcpy(opt.title, "Scattergram test");

	opt.num_axis = 2;
	opt.axis = new struct ra_plot_axis[opt.num_axis];
	memset(opt.axis, 0, sizeof(struct ra_plot_axis) * opt.num_axis);

	opt.axis[0].name = new char[100];
	strcpy(opt.axis[0].name, "qrs-pos");
	opt.axis[0].unit = new char[100];
	strcpy(opt.axis[0].unit, "hour");
	opt.axis[0].is_time = 1;
	opt.axis[0].time_format = RA_PLOT_TIME_HHMM;
	opt.axis[0].plot_log = 0;
	opt.axis[0].log_type = 0;

	opt.axis[0].num_data = pos.get_num_elem()-1;
	opt.axis[0].data = new double[opt.axis[0].num_data];
	vector<long> lv = pos.get_value_lv();
	for (long l = 0; l < opt.axis[0].num_data; l++)
	{
		double v = lv[l];
		v *= (1000.0/samplerate);
		v += rec_start_ms;
		v /= (1000.0 * 60.0 * 60.0);
		opt.axis[0].data[l] = v;
	}

	opt.axis[1].name = new char[100];
	strcpy(opt.axis[1].name, "rri");
	opt.axis[1].unit = new char[100];
	strcpy(opt.axis[1].unit, "ms");
	opt.axis[1].is_time = 0;
	opt.axis[1].use_min = 0;
	opt.axis[1].min = 200;
	opt.axis[1].use_max = 0;
	opt.axis[1].max = 2000;
	
	opt.axis[1].num_data = rri.get_num_elem();
	opt.axis[1].data = new double[opt.axis[1].num_data];	
	vector<double> dv = rri.get_value_dv();
	for (long l = 0; l < opt.axis[1].num_data; l++)
		opt.axis[1].data[l] = dv[l];

	opt.num_plot_pairs = 1;
	opt.pair = new struct ra_plot_pair[opt.num_plot_pairs];
	memset(opt.pair, 0, sizeof(struct ra_plot_pair) * opt.num_plot_pairs);

	opt.pair[0].x_idx = 0;
	opt.pair[0].y_idx = 1;
	opt.pair[0].z_idx = -1;
	opt.pair[0].plot_symbol = 0;
	opt.pair[0].symbol_type = 0;
	opt.pair[0].sym_r = 0;
	opt.pair[0].sym_g = 0;
	opt.pair[0].sym_b = 0;
	opt.pair[0].plot_line = 1;
	opt.pair[0].line_type = 0;
	opt.pair[0].line_r = 0;
	opt.pair[0].line_g = 0;
	opt.pair[0].line_b = 0;

	rpl_scattergram(dh, &opt);

	delete[] opt.axis[0].name;
	delete[] opt.axis[0].unit;
	delete[] opt.axis[0].data;
	delete[] opt.axis[1].name;
	delete[] opt.axis[1].unit;
	delete[] opt.axis[1].data;

	delete[] opt.pair;
	delete[] opt.axis;
	delete[] opt.title;

	return 0;
} /* test_scattergram2() */

