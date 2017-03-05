/********************************************************************************
** Form generated from reading ui file 'simple_calc_dlg.ui'
**
** Created: Mon Sep 15 20:07:09 2008
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef SIMPLE_CALC_DLG_UI_H
#define SIMPLE_CALC_DLG_UI_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_simple_calc_dlg
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *textLabel1;
    QComboBox *pl_list;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem;
    QPushButton *options_btn;
    QPushButton *calc_btn;
    QVBoxLayout *vboxLayout1;
    QLabel *textLabel2;
    QTextBrowser *res_list;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem1;
    QPushButton *copy_btn;
    QPushButton *exit_btn;

    void setupUi(QDialog *simple_calc_dlg)
    {
    if (simple_calc_dlg->objectName().isEmpty())
        simple_calc_dlg->setObjectName(QString::fromUtf8("simple_calc_dlg"));
    simple_calc_dlg->resize(450, 302);
    gridLayout = new QGridLayout(simple_calc_dlg);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(9);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    textLabel1 = new QLabel(simple_calc_dlg);
    textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(5));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(textLabel1->sizePolicy().hasHeightForWidth());
    textLabel1->setSizePolicy(sizePolicy);

    hboxLayout->addWidget(textLabel1);

    pl_list = new QComboBox(simple_calc_dlg);
    pl_list->setObjectName(QString::fromUtf8("pl_list"));

    hboxLayout->addWidget(pl_list);


    vboxLayout->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    spacerItem = new QSpacerItem(200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    options_btn = new QPushButton(simple_calc_dlg);
    options_btn->setObjectName(QString::fromUtf8("options_btn"));

    hboxLayout1->addWidget(options_btn);

    calc_btn = new QPushButton(simple_calc_dlg);
    calc_btn->setObjectName(QString::fromUtf8("calc_btn"));

    hboxLayout1->addWidget(calc_btn);


    vboxLayout->addLayout(hboxLayout1);

    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    textLabel2 = new QLabel(simple_calc_dlg);
    textLabel2->setObjectName(QString::fromUtf8("textLabel2"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(textLabel2->sizePolicy().hasHeightForWidth());
    textLabel2->setSizePolicy(sizePolicy1);

    vboxLayout1->addWidget(textLabel2);

    res_list = new QTextBrowser(simple_calc_dlg);
    res_list->setObjectName(QString::fromUtf8("res_list"));

    vboxLayout1->addWidget(res_list);


    vboxLayout->addLayout(vboxLayout1);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    spacerItem1 = new QSpacerItem(141, 32, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem1);

    copy_btn = new QPushButton(simple_calc_dlg);
    copy_btn->setObjectName(QString::fromUtf8("copy_btn"));

    hboxLayout2->addWidget(copy_btn);

    exit_btn = new QPushButton(simple_calc_dlg);
    exit_btn->setObjectName(QString::fromUtf8("exit_btn"));

    hboxLayout2->addWidget(exit_btn);


    vboxLayout->addLayout(hboxLayout2);


    gridLayout->addLayout(vboxLayout, 0, 0, 1, 1);


    retranslateUi(simple_calc_dlg);

    QMetaObject::connectSlotsByName(simple_calc_dlg);
    } // setupUi

    void retranslateUi(QDialog *simple_calc_dlg)
    {
    simple_calc_dlg->setWindowTitle(QApplication::translate("simple_calc_dlg", "calculate parameters", 0, QApplication::UnicodeUTF8));
    textLabel1->setText(QApplication::translate("simple_calc_dlg", "Process Plugins", 0, QApplication::UnicodeUTF8));
    options_btn->setText(QApplication::translate("simple_calc_dlg", "Options...", 0, QApplication::UnicodeUTF8));
    calc_btn->setText(QApplication::translate("simple_calc_dlg", "Calculate", 0, QApplication::UnicodeUTF8));
    textLabel2->setText(QApplication::translate("simple_calc_dlg", "Results", 0, QApplication::UnicodeUTF8));
    copy_btn->setText(QApplication::translate("simple_calc_dlg", "Copy to Clipboard", 0, QApplication::UnicodeUTF8));
    exit_btn->setText(QApplication::translate("simple_calc_dlg", "Exit", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(simple_calc_dlg);
    } // retranslateUi

};

namespace Ui {
    class simple_calc_dlg: public Ui_simple_calc_dlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SIMPLE_CALC_DLG_UI_H
