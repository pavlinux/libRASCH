/********************************************************************************
** Form generated from reading ui file 'progress_dlg_qt.ui'
**
** Created: Sun Jun 10 13:42:48 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef PROGRESS_DLG_QT_UI_H
#define PROGRESS_DLG_QT_UI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_progress_dlg_qt
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout;
    QLabel *text;
    QSpacerItem *spacerItem;
    QProgressBar *progress_bar;
    QSpacerItem *spacerItem1;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem2;
    QPushButton *cancel_btn;
    QSpacerItem *spacerItem3;

    void setupUi(QDialog *progress_dlg_qt)
    {
    progress_dlg_qt->setObjectName(QString::fromUtf8("progress_dlg_qt"));
    gridLayout = new QGridLayout(progress_dlg_qt);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    text = new QLabel(progress_dlg_qt);
    text->setObjectName(QString::fromUtf8("text"));

    vboxLayout->addWidget(text);

    spacerItem = new QSpacerItem(266, 16, QSizePolicy::Minimum, QSizePolicy::Minimum);

    vboxLayout->addItem(spacerItem);

    progress_bar = new QProgressBar(progress_dlg_qt);
    progress_bar->setObjectName(QString::fromUtf8("progress_bar"));
    progress_bar->setValue(24);
    progress_bar->setOrientation(Qt::Horizontal);

    vboxLayout->addWidget(progress_bar);

    spacerItem1 = new QSpacerItem(266, 16, QSizePolicy::Minimum, QSizePolicy::Minimum);

    vboxLayout->addItem(spacerItem1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem2);

    cancel_btn = new QPushButton(progress_dlg_qt);
    cancel_btn->setObjectName(QString::fromUtf8("cancel_btn"));

    hboxLayout->addWidget(cancel_btn);

    spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem3);


    vboxLayout->addLayout(hboxLayout);


    gridLayout->addLayout(vboxLayout, 0, 0, 1, 1);


    retranslateUi(progress_dlg_qt);

    QSize size(290, 133);
    size = size.expandedTo(progress_dlg_qt->minimumSizeHint());
    progress_dlg_qt->resize(size);


    QMetaObject::connectSlotsByName(progress_dlg_qt);
    } // setupUi

    void retranslateUi(QDialog *progress_dlg_qt)
    {
    progress_dlg_qt->setWindowTitle(QApplication::translate("progress_dlg_qt", "libRASCH progress", 0, QApplication::UnicodeUTF8));
    text->setText(QApplication::translate("progress_dlg_qt", "detection in channel 1", 0, QApplication::UnicodeUTF8));
    cancel_btn->setText(QApplication::translate("progress_dlg_qt", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(progress_dlg_qt);
    } // retranslateUi

};

namespace Ui {
    class progress_dlg_qt: public Ui_progress_dlg_qt {};
} // namespace Ui

#endif // PROGRESS_DLG_QT_UI_H
