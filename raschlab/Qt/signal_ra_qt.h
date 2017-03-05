/***************************************************************************
                          signal_ra_qt.h  -  description
                             -------------------
    begin                : Thu Jan 10 2002
    copyright            : (C) 2002 by Raphael Schneider
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

 #include "../general/signal_ra.h"

 class signal_ra_qt : public signal_ra
 {
 public:
 	signal_ra_qt(ra_handle m) : signal_ra(m) {};

 protected: 	
	virtual void close_view(int index);
	virtual dest_handle get_dest_handle(int index);
	virtual void update_view(int index);
 };  // class signal_ra_qt
