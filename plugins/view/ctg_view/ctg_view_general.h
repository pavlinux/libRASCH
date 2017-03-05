/*----------------------------------------------------------------------------
 * ctg_view_general.h
 *
 *
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2003-2008, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef CTG_VIEW_GENERAL_H
#define CTG_VIEW_GENERAL_H

#include "../ts_view/ts_view_general.h"


#ifdef _CTG_ANNOTS
static const char *ctg_annot[] = {
	"Temperatur",
	"Muttermund  cm",
	"Muttermund vollstaendig",
	"Kopflage ",
	"Stehen-Stresstest",
	"Muetterlicher Puls",
	"Tocokopf versetzt",
	"Blasensprung ( )",
	"Rechtsseitenlage",
	"Linksseitenlage",
	"Sitzen",
	"Spazieren",
	"Toilette",
	"Schuessel",
	"Katheterurin",
	"Bad",
	"vaginale Untersuchung",
	"Akupunktur",
	"Visite",
	"OP Vorbereitung",
	"Anaesthesie (PDA)",
	"Anaesthesie (Spinal)",
	"Ultraschall",
	"Medikation: Ringer",
	"Medikation: Glucose",
	"Medikation: NaCl",
	"Medikation: Dolantin",
	"Medikation: Meptid",
	"Medikation: Buscopan",
	"Medikation: Miniprostan-Vaginalgel",
	"Medikation: Oxytocin",
	"Medikation: Tractocile",
	"Medikation: Partusisten",
	"Medikation: Unacid",
	"Medikation: Erythromycin",
	"Medikation: Adalat",
	"Medikation: Nepressol",
	"Medikation: Magnesium 2+",
	"Medikation: Periplasmal"
}; /* ctg_annot */
#endif  // _CTG_ANNOTS


class ctg_view_general:virtual public ts_view_general
{
public:
	ctg_view_general(meas_handle h);
	~ctg_view_general();

	void init();
	virtual void sort_general(prop_handle ph, double value);

	void use_fixed_height(bool flag);

	virtual void save_settings(const char *file);
	virtual void load_settings(const char *file);

protected:
	void get_event_infos();

	virtual double draw_calc_max_value(int ch);
	virtual inline int draw_calc_amplitude(double max, double val, int ch);
	virtual bool key_press_char(char c, bool shift, bool ctrl);

	bool insert_max_uc_edit(long pos);
	void create_eval();
	void delete_event();

	void get_uc_pos(int **x, long **pos, int *num);

	int mouse_press(int x);
	long get_sel_pos(int x);

	prop_handle _prop_uc_templ;

	// last position set or send or received
	long _pos_sel_save;

	long _first_pos_idx;
	long _last_pos_idx;
}; // class ctg_view_general


inline int
ctg_view_general::draw_calc_amplitude(double max, double val, int ch)
{
	int amp = (int)((max - val) * _ch[ch].yscale);

	return amp;
} // draw_calc_amplitude (inline)


#endif // CTG_VIEW_GENERAL_H
