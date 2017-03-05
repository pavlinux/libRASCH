/********************************************************************************
** Form generated from reading ui file 'evprop_dlg.ui'
**
** Created: Mon Sep 15 20:06:32 2008
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef EVPROP_DLG_UI_H
#define EVPROP_DLG_UI_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_evprop_dlg_ui
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout1;
    QLabel *textLabel1;
    QHBoxLayout *hboxLayout;
    QListWidget *class_list;
    QVBoxLayout *vboxLayout2;
    QPushButton *del_class_btn;
    QSpacerItem *spacerItem;
    QVBoxLayout *vboxLayout3;
    QLabel *textLabel2;
    QHBoxLayout *hboxLayout1;
    QListWidget *prop_list;
    QVBoxLayout *vboxLayout4;
    QPushButton *del_prop_btn;
    QSpacerItem *spacerItem1;
    QGroupBox *group;
    QHBoxLayout *hboxLayout2;
    QVBoxLayout *vboxLayout5;
    QVBoxLayout *vboxLayout6;
    QLabel *desc_text;
    QLabel *desc_field;
    QHBoxLayout *hboxLayout3;
    QHBoxLayout *hboxLayout4;
    QLabel *val_text;
    QLabel *val_field;
    QSpacerItem *spacerItem2;
    QHBoxLayout *hboxLayout5;
    QLabel *occur_text;
    QLabel *occur_field;
    QHBoxLayout *hboxLayout6;
    QSpacerItem *spacerItem3;
    QPushButton *exit_btn;

    void setupUi(QDialog *evprop_dlg_ui)
    {
    if (evprop_dlg_ui->objectName().isEmpty())
        evprop_dlg_ui->setObjectName(QString::fromUtf8("evprop_dlg_ui"));
    evprop_dlg_ui->resize(408, 373);
    evprop_dlg_ui->setSizeGripEnabled(true);
    gridLayout = new QGridLayout(evprop_dlg_ui);
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
    textLabel1 = new QLabel(evprop_dlg_ui);
    textLabel1->setObjectName(QString::fromUtf8("textLabel1"));

    vboxLayout1->addWidget(textLabel1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    class_list = new QListWidget(evprop_dlg_ui);
    class_list->setObjectName(QString::fromUtf8("class_list"));

    hboxLayout->addWidget(class_list);

    vboxLayout2 = new QVBoxLayout();
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(0);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    del_class_btn = new QPushButton(evprop_dlg_ui);
    del_class_btn->setObjectName(QString::fromUtf8("del_class_btn"));

    vboxLayout2->addWidget(del_class_btn);

    spacerItem = new QSpacerItem(87, 91, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout2->addItem(spacerItem);


    hboxLayout->addLayout(vboxLayout2);


    vboxLayout1->addLayout(hboxLayout);


    vboxLayout->addLayout(vboxLayout1);

    vboxLayout3 = new QVBoxLayout();
    vboxLayout3->setSpacing(6);
    vboxLayout3->setMargin(0);
    vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
    textLabel2 = new QLabel(evprop_dlg_ui);
    textLabel2->setObjectName(QString::fromUtf8("textLabel2"));

    vboxLayout3->addWidget(textLabel2);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    prop_list = new QListWidget(evprop_dlg_ui);
    prop_list->setObjectName(QString::fromUtf8("prop_list"));

    hboxLayout1->addWidget(prop_list);

    vboxLayout4 = new QVBoxLayout();
    vboxLayout4->setSpacing(6);
    vboxLayout4->setMargin(0);
    vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
    del_prop_btn = new QPushButton(evprop_dlg_ui);
    del_prop_btn->setObjectName(QString::fromUtf8("del_prop_btn"));

    vboxLayout4->addWidget(del_prop_btn);

    spacerItem1 = new QSpacerItem(87, 133, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout4->addItem(spacerItem1);


    hboxLayout1->addLayout(vboxLayout4);


    vboxLayout3->addLayout(hboxLayout1);


    vboxLayout->addLayout(vboxLayout3);

    group = new QGroupBox(evprop_dlg_ui);
    group->setObjectName(QString::fromUtf8("group"));
    hboxLayout2 = new QHBoxLayout(group);
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(9);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    vboxLayout5 = new QVBoxLayout();
    vboxLayout5->setSpacing(6);
    vboxLayout5->setMargin(0);
    vboxLayout5->setObjectName(QString::fromUtf8("vboxLayout5"));
    vboxLayout6 = new QVBoxLayout();
    vboxLayout6->setSpacing(6);
    vboxLayout6->setMargin(0);
    vboxLayout6->setObjectName(QString::fromUtf8("vboxLayout6"));
    desc_text = new QLabel(group);
    desc_text->setObjectName(QString::fromUtf8("desc_text"));

    vboxLayout6->addWidget(desc_text);

    desc_field = new QLabel(group);
    desc_field->setObjectName(QString::fromUtf8("desc_field"));
    desc_field->setWordWrap(true);

    vboxLayout6->addWidget(desc_field);


    vboxLayout5->addLayout(vboxLayout6);

    hboxLayout3 = new QHBoxLayout();
    hboxLayout3->setSpacing(6);
    hboxLayout3->setMargin(0);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    hboxLayout4 = new QHBoxLayout();
    hboxLayout4->setSpacing(6);
    hboxLayout4->setMargin(0);
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
    val_text = new QLabel(group);
    val_text->setObjectName(QString::fromUtf8("val_text"));

    hboxLayout4->addWidget(val_text);

    val_field = new QLabel(group);
    val_field->setObjectName(QString::fromUtf8("val_field"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(5));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(val_field->sizePolicy().hasHeightForWidth());
    val_field->setSizePolicy(sizePolicy);

    hboxLayout4->addWidget(val_field);


    hboxLayout3->addLayout(hboxLayout4);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem2);

    hboxLayout5 = new QHBoxLayout();
    hboxLayout5->setSpacing(6);
    hboxLayout5->setMargin(0);
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    occur_text = new QLabel(group);
    occur_text->setObjectName(QString::fromUtf8("occur_text"));

    hboxLayout5->addWidget(occur_text);

    occur_field = new QLabel(group);
    occur_field->setObjectName(QString::fromUtf8("occur_field"));
    sizePolicy.setHeightForWidth(occur_field->sizePolicy().hasHeightForWidth());
    occur_field->setSizePolicy(sizePolicy);
    occur_field->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    hboxLayout5->addWidget(occur_field);


    hboxLayout3->addLayout(hboxLayout5);


    vboxLayout5->addLayout(hboxLayout3);


    hboxLayout2->addLayout(vboxLayout5);


    vboxLayout->addWidget(group);

    hboxLayout6 = new QHBoxLayout();
    hboxLayout6->setSpacing(6);
    hboxLayout6->setMargin(0);
    hboxLayout6->setObjectName(QString::fromUtf8("hboxLayout6"));
    spacerItem3 = new QSpacerItem(350, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout6->addItem(spacerItem3);

    exit_btn = new QPushButton(evprop_dlg_ui);
    exit_btn->setObjectName(QString::fromUtf8("exit_btn"));
    exit_btn->setDefault(true);

    hboxLayout6->addWidget(exit_btn);


    vboxLayout->addLayout(hboxLayout6);


    gridLayout->addLayout(vboxLayout, 0, 0, 1, 1);


    retranslateUi(evprop_dlg_ui);

    QMetaObject::connectSlotsByName(evprop_dlg_ui);
    } // setupUi

    void retranslateUi(QDialog *evprop_dlg_ui)
    {
    evprop_dlg_ui->setWindowTitle(QApplication::translate("evprop_dlg_ui", "Event-Properties", 0, QApplication::UnicodeUTF8));
    textLabel1->setText(QApplication::translate("evprop_dlg_ui", "Event-Class's", 0, QApplication::UnicodeUTF8));
    del_class_btn->setText(QApplication::translate("evprop_dlg_ui", "delete", 0, QApplication::UnicodeUTF8));
    textLabel2->setText(QApplication::translate("evprop_dlg_ui", "Event Properties", 0, QApplication::UnicodeUTF8));
    del_prop_btn->setText(QApplication::translate("evprop_dlg_ui", "delete", 0, QApplication::UnicodeUTF8));
    group->setTitle(QApplication::translate("evprop_dlg_ui", "name", 0, QApplication::UnicodeUTF8));
    desc_text->setText(QApplication::translate("evprop_dlg_ui", "Description", 0, QApplication::UnicodeUTF8));
    desc_field->setText(QApplication::translate("evprop_dlg_ui", "TextLabel", 0, QApplication::UnicodeUTF8));
    val_text->setText(QApplication::translate("evprop_dlg_ui", "value-type:", 0, QApplication::UnicodeUTF8));
    val_field->setText(QApplication::translate("evprop_dlg_ui", "double", 0, QApplication::UnicodeUTF8));
    occur_text->setText(QApplication::translate("evprop_dlg_ui", "occurence:", 0, QApplication::UnicodeUTF8));
    occur_field->setText(QApplication::translate("evprop_dlg_ui", "continues12345", 0, QApplication::UnicodeUTF8));
    exit_btn->setText(QApplication::translate("evprop_dlg_ui", "Exit", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(evprop_dlg_ui);
    } // retranslateUi

};

namespace Ui {
    class evprop_dlg_ui: public Ui_evprop_dlg_ui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // EVPROP_DLG_UI_H
