/*----------------------------------------------------------------------------
 * color_button.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/plot_view/color_button.h,v 1.2 2003/03/21 09:43:10 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H


class color_button:public CButton
{
      public:
	color_button();

	void SetColor(COLORREF Color);

	virtual void color_button::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

      protected:
	  COLORREF _color;
};				// class color_button


#endif // class color_button COLOR_BUTTON_H
