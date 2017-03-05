#include <QtGui/qcombobox.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qlabel.h>
#include <QtGui/qtextedit.h>

#include "annot_dlg_qt.h"


annot_dlg_qt::annot_dlg_qt(struct ch_info *ch, int num_ch, int curr_ch, char **predefs, int num_predefs,
			   QString text, bool noise_flag/*=false*/, bool ignore_flag/*=false*/, int ch_sel/*=-1*/,
			   QWidget *parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);
	
	_ch = ch;
	_curr_ch = curr_ch;
	ch_list->addItem("all channels");
	int i = 0;
	for (i = 0; i < num_ch; i++)
		ch_list->addItem(ch[i].name);

	if (ch_sel != -1)
	{
		for (int i = 0; i < num_ch; i++)
		{
			if (ch_sel == ch[i].ch_number)
			{
				ch_list->setCurrentIndex(i+1);
				break;
			}
		}
	}
	else
		ch_list->setCurrentIndex(0);

	for (i = 0; i < num_predefs; i++)
		annot_list->addItem(predefs[i]);

	if (!text.isEmpty())
	{
		annot_text->insertPlainText(text);
		noise->setChecked(noise_flag);
		ignore->setChecked(ignore_flag);
	}
	else
		del_btn->setEnabled(false);

	_del = false;

	QObject::connect(annot_list, SIGNAL(highlighted(const QString &)), this, SLOT(sel_annot(const QString &)));
	QObject::connect(cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ok_btn, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(del_btn, SIGNAL(clicked()), this, SLOT(del_pressed()));
} // constructor


annot_dlg_qt::~annot_dlg_qt()
{

} // destructor


void
annot_dlg_qt::sel_annot(const QString &annot)
{
	annot_text->insertPlainText(annot);
} // sel_annot()


QString
annot_dlg_qt::annot()
{
	return annot_text->toPlainText();
} // annot()


long
annot_dlg_qt::channel()
{
	int idx = ch_list->currentIndex();
	if (idx == 0)
		return -1;
	
	return _ch[idx-1].ch_number;
} // channel()


bool
annot_dlg_qt::is_noise()
{
	return noise->isChecked();
} // is_noise()


bool
annot_dlg_qt::do_ignore()
{
	return ignore->isChecked();
} // do_ignore()


void
annot_dlg_qt::del_pressed()
{
	_del = true;
	accept();
} // del_pressed()


bool
annot_dlg_qt::delete_annot()
{
	return _del;
} // delete_annot()
