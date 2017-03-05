/*
 * signal_ra_qt.h  - 
 *
 * Copyright 2001-2006 by Raphael Schneider <librasch@gmail.com>
 *
 * $Id: $
 */

#include "signal_ra.h"

class signal_ra_qt : public signal_ra
{
public:
        signal_ra_qt(ra_handle h) : signal_ra(h) {};
        ~signal_ra_qt();
	
protected: 	
	virtual void close_view(int index);
	virtual dest_handle get_dest_handle(int index);
	virtual void update_view(int index);
};  // class signal_ra_qt
