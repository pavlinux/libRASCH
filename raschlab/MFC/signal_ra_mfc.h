#ifndef SIGNAL_RA_MFC_H
#define SIGNAL_RA_MFC_H


#include "..\\general\\signal_ra.h"

class signal_ra_mfc : public signal_ra
{
public:
	signal_ra_mfc(ra_handle m) : signal_ra(m) {};

protected:
	virtual void close_view(int index);
	virtual dest_handle get_dest_handle(int index);
	virtual void update_view(int index);
};  // class signal_ra_mfc


#endif  // SIGNAL_RA_MFC_H
