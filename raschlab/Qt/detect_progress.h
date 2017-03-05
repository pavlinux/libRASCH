/***************************************************************************
                          detect_progress.h  -  description
                             -------------------
    begin                : Sun Nov 4 2001
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
#ifndef _DETECT_PROGRESS_H
#define _DETECT_PROGRESS_H


#include "detect_progress_dlg.h"

class detect_progress : public detect_progress_dlg
{
public:
	detect_progress(QWidget* parent = 0, const char* name = 0,
		bool modal = FALSE, Qt::WFlags fl = 0);
	~detect_progress();

	void set_text(QString s);
	void set_progress(int value);
};  // class detect_progress


#endif  // _DETECT_PROGRESS_H
