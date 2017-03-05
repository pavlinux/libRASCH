/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sat Nov 3 2001
    copyright            : (C) 2001 by Raphael Schneider
    email                : rasch@med1.med.tum.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>

#include "raschlab_qt.h"

int
main(int argc, char *argv[])
{
	// init of libRASCH has to be done here (before initializing the Qt stuff)
	// so locale is set correctly
	ra_handle ra = ra_lib_init();
	if (!ra)
	{
		char err_txt[200];
		strcpy(err_txt, "Couldn't initialize libRASCH. Something fundamental is broken.");
		QMessageBox::critical(0, "RASCHlab", err_txt);
		return -1;
	}	

        QApplication app(argc, argv);
	raschlab_qt *main_win = new raschlab_qt(ra);
	main_win->show();
	if (argc > 1)
		main_win->open_signal_file(argv[1]);

        return app.exec();
}  // main()
