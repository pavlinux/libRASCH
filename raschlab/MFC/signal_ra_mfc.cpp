#include "stdafx.h"
#include "signal_ra_mfc.h"
#include "ChildFrm.h"

void
signal_ra_mfc::close_view(int index)
{
	if (!_v_list || (index < 0) || (index >= _num_view))
		return;

	CChildFrame *v = (CChildFrame *)(_v_list[index]);
	v->DestroyWindow();
}  // close_view()


dest_handle
signal_ra_mfc::get_dest_handle(int index)
{
	if (!_v_list || (index < 0) || (index >= _num_view))
		return NULL;

	CChildFrame *v = (CChildFrame *)(_v_list[index]);

	return (dest_handle)v;
}  // get_dest_handle()


void
signal_ra_mfc::update_view(int index)
{
	if (!_v_list || (index < 0) || (index >= _num_view))
		return;

	CChildFrame *f = (CChildFrame *)_v_list[index];
	f->Invalidate();
}  // update_view()
