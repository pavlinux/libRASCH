/********************************************************************************
** Form generated from reading ui file 'options_dlg.ui'
**
** Created: Mon Sep 15 20:07:09 2008
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef OPTIONS_DLG_UI_H
#define OPTIONS_DLG_UI_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_options_dlg
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout1;
    QLabel *textLabel1;
    QListWidget *opt_list;
    QVBoxLayout *vboxLayout2;
    QLabel *opt_name;
    QHBoxLayout *hboxLayout;
    QLineEdit *opt_value;
    QPushButton *change_btn;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem;
    QPushButton *cancel_btn;
    QSpacerItem *spacerItem1;
    QPushButton *ok_btn;
    QSpacerItem *spacerItem2;

    void setupUi(QDialog *options_dlg)
    {
    if (options_dlg->objectName().isEmpty())
        options_dlg->setObjectName(QString::fromUtf8("options_dlg"));
    options_dlg->resize(424, 363);
    gridLayout = new QGridLayout(options_dlg);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    textLabel1 = new QLabel(options_dlg);
    textLabel1->setObjectName(QString::fromUtf8("textLabel1"));

    vboxLayout1->addWidget(textLabel1);

    opt_list = new QListWidget(options_dlg);
    opt_list->setObjectName(QString::fromUtf8("opt_list"));

    vboxLayout1->addWidget(opt_list);


    vboxLayout->addLayout(vboxLayout1);

    vboxLayout2 = new QVBoxLayout();
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(0);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    opt_name = new QLabel(options_dlg);
    opt_name->setObjectName(QString::fromUtf8("opt_name"));

    vboxLayout2->addWidget(opt_name);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    opt_value = new QLineEdit(options_dlg);
    opt_value->setObjectName(QString::fromUtf8("opt_value"));

    hboxLayout->addWidget(opt_value);

    change_btn = new QPushButton(options_dlg);
    change_btn->setObjectName(QString::fromUtf8("change_btn"));

    hboxLayout->addWidget(change_btn);


    vboxLayout2->addLayout(hboxLayout);


    vboxLayout->addLayout(vboxLayout2);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    cancel_btn = new QPushButton(options_dlg);
    cancel_btn->setObjectName(QString::fromUtf8("cancel_btn"));

    hboxLayout1->addWidget(cancel_btn);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem1);

    ok_btn = new QPushButton(options_dlg);
    ok_btn->setObjectName(QString::fromUtf8("ok_btn"));

    hboxLayout1->addWidget(ok_btn);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem2);


    vboxLayout->addLayout(hboxLayout1);


    gridLayout->addLayout(vboxLayout, 0, 0, 1, 1);


    retranslateUi(options_dlg);

    QMetaObject::connectSlotsByName(options_dlg);
    } // setupUi

    void retranslateUi(QDialog *options_dlg)
    {
    options_dlg->setWindowTitle(QApplication::translate("options_dlg", "Set Options", 0, QApplication::UnicodeUTF8));
    textLabel1->setText(QApplication::translate("options_dlg", "Options", 0, QApplication::UnicodeUTF8));
    opt_name->setText(QApplication::translate("options_dlg", "textLabel2", 0, QApplication::UnicodeUTF8));
    change_btn->setText(QApplication::translate("options_dlg", "Change", 0, QApplication::UnicodeUTF8));
    cancel_btn->setText(QApplication::translate("options_dlg", "Cancel", 0, QApplication::UnicodeUTF8));
    ok_btn->setText(QApplication::translate("options_dlg", "OK", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(options_dlg);
    } // retranslateUi

};

namespace Ui {
    class options_dlg: public Ui_options_dlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // OPTIONS_DLG_UI_H
