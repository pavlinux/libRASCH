/*----------------------------------------------------------------------------
 * color_button.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/plot_view/color_button.cpp,v 1.2 2003/03/21 09:43:10 rasch Exp $
 *--------------------------------------------------------------------------*/

#include "stdafx.h"
#include "color_button.h"


// !!!!!!!!!!!!!!!!!!!!!!!! Important !!!!!!!!!!!!!!!!!!!!!!!!!!
// Don't forget to enable owner-draw property in Dialog-Designer


color_button::color_button():CButton()
{
	_color = RGB(0, 0, 0);
}				// Constructor


void
color_button::DrawItem(LPDRAWITEMSTRUCT item)
{
	HPEN white = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HPEN black = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	LOGBRUSH br;
	br.lbStyle = BS_SOLID;
	br.lbColor = _color;
	br.lbHatch = 0;
	HBRUSH h = CreateBrushIndirect(&br);

	HPEN pen_save = (HPEN) SelectObject(item->hDC, black);
	POINT pts[3];
	UINT nState = GetState();
	if (nState & 0x0004)
	{
		// button pressed
		pts[0].x = item->rcItem.left;
		pts[0].y = item->rcItem.bottom - 1;
		pts[1].x = item->rcItem.left;
		pts[1].y = item->rcItem.top;
		pts[2].x = item->rcItem.right - 1;
		pts[2].y = item->rcItem.top;
		Polyline(item->hDC, pts, 3);

		pts[0].x = item->rcItem.left + 1;
		pts[0].y = item->rcItem.bottom - 2;
		pts[1].x = item->rcItem.left + 1;
		pts[1].y = item->rcItem.top + 1;
		pts[2].x = item->rcItem.right - 1;
		pts[2].y = item->rcItem.top + 1;
		Polyline(item->hDC, pts, 3);

		pts[0].x = item->rcItem.left;
		pts[0].y = item->rcItem.bottom - 1;
		pts[1].x = item->rcItem.right - 1;
		pts[1].y = item->rcItem.bottom - 1;
		pts[2].x = item->rcItem.right - 1;
		pts[2].y = item->rcItem.top;
		Polyline(item->hDC, pts, 3);

		RECT rect = item->rcItem;
		rect.left += 2;
		rect.right -= 1;
		rect.top += 2;
		rect.bottom -= 1;
		FillRect(item->hDC, &rect, h);
	}
	else
	{
		// button not pressed
		pts[0].x = item->rcItem.left;
		pts[0].y = item->rcItem.bottom - 1;
		pts[1].x = item->rcItem.left;
		pts[1].y = item->rcItem.top;
		pts[2].x = item->rcItem.right;
		pts[2].y = item->rcItem.top;
		Polyline(item->hDC, pts, 3);

		SelectObject(item->hDC, white);
		pts[0].x = item->rcItem.left + 1;
		pts[0].y = item->rcItem.bottom - 2;
		pts[1].x = item->rcItem.left + 1;
		pts[1].y = item->rcItem.top + 1;
		pts[2].x = item->rcItem.right - 1;
		pts[2].y = item->rcItem.top + 1;
		Polyline(item->hDC, pts, 3);

		SelectObject(item->hDC, black);
		pts[0].x = item->rcItem.left;
		pts[0].y = item->rcItem.bottom - 1;
		pts[1].x = item->rcItem.right - 1;
		pts[1].y = item->rcItem.bottom - 1;
		pts[2].x = item->rcItem.right - 1;
		pts[2].y = item->rcItem.top;
		Polyline(item->hDC, pts, 3);

		pts[0].x = item->rcItem.left + 1;
		pts[0].y = item->rcItem.bottom - 2;
		pts[1].x = item->rcItem.right - 2;
		pts[1].y = item->rcItem.bottom - 2;
		pts[2].x = item->rcItem.right - 2;
		pts[2].y = item->rcItem.top;
		Polyline(item->hDC, pts, 3);

		RECT rect = item->rcItem;
		rect.left += 2;
		rect.right -= 2;
		rect.top += 2;
		rect.bottom -= 2;
		FillRect(item->hDC, &rect, h);
	}
	SelectObject(item->hDC, pen_save);

	DeleteObject(h);
	DeleteObject(white);
	DeleteObject(black);
}				// DrawItem()


void
color_button::SetColor(COLORREF c)
{
	_color = c;
	Invalidate();
}				// SetColor()
