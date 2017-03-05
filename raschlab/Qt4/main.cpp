/*
 * RASCHlab Qt
 *
 * Copyright 2006 by Raphael Schneider <librasch@gmail.com>
 *
 *
 * $Id: $
 */


#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QSplashScreen>

#include "main_window.h"


int
main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QSplashScreen *splash = new QSplashScreen;
	splash->setPixmap(QPixmap(":/images/splash.png"));
	splash->show();

	Qt::Alignment top_right = Qt::AlignRight | Qt::AlignTop;

	splash->showMessage(QString(gettext("load and init libRASCH...")), top_right, Qt::black);

	// init of libRASCH has to be done here (before initializing the Qt stuff)
	// so locale is set correctly
	ra_handle ra = ra_lib_init();
	if (!ra)
	{
		char err_txt[200];
		strcpy(err_txt, gettext("Couldn't initialize libRASCH. Something fundamental is broken."));
		QMessageBox::critical(0, "RASCHlab", err_txt);
		return -1;
	}	
	if (ra_lib_get_error(ra, NULL, 0) != RA_ERR_NONE)
	{
		char t[200];
		char err_txt[200];
		long err_num = ra_lib_get_error(ra, t, 100);
		sprintf(err_txt, gettext("Couldn't initialize libRASCH.\nReason: %s (%ld)"), t, err_num);
		QMessageBox::critical(0, "RASCHlab", err_txt);
		return -1;
	}

	main_window main_win(ra);
	main_win.show();

	if (argc > 1)
	{
		splash->showMessage(QString(gettext("load given measurement...")), top_right, Qt::black);
		main_win.open_signal_file(argv[1]);
	}

	splash->finish(&main_win);
	delete splash;

	int ret = app.exec();

	if (ra)
		ra_lib_close(ra);

	return ret;
} // main()
