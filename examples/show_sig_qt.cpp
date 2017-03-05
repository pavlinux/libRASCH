//<![CDATA[
#include <stdio.h>
#include <QtGui/qapplication.h>
#include <QtGui/QMainWindow>
#include <QtGui/qwidget.h>

extern "C" {
#include <ra.h>
}

int
main(int argc, char *argv[])
{
	/* initialize libRASCH */
	ra_handle ra = ra_lib_init();
	if (ra == NULL)
	{
		printf("error initializing libRASCH\n");
		return -1;
	}
	
	/* open measurement */
	meas_handle meas = ra_meas_open(ra, argv[1], NULL, 0);
	if (meas == NULL)
	{
		printf("can't open measurement %s\n", argv[1]);
		return -1;
	}

	plugin_handle pl = ra_plugin_get_by_name(ra, "rasch-view", 0);
	if (pl == NULL)
	{
		printf("can't find plugin 'ra-view'\n");
		return -1;
	}

	// create Qt application
	QApplication a(argc, argv);
	QMainWindow main_win;

	// create view using libRASCH view-plugin 'rasch-view'
	view_handle vih = ra_view_get(meas, pl, &main_win);
	if (vih == NULL)
		return -1;
	if (ra_view_create(vih) != 0)
	{
		ra_view_free(vih);
		return -1;
	}

	// view object will be returned in struct view_info.views
	QWidget *view = (QWidget *)ra_view_get_window(vih, 0);
	if (!view)
	{
		ra_view_free(vih);
		return -1;
	}

	main_win.show();
	int ret = a.exec();

	ra_view_free(vih);
	ra_meas_close(meas);
	ra_lib_close(ra);

	return ret;
}  // main()
//]]>
