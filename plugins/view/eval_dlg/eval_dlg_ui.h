/********************************************************************************
** Form generated from reading ui file 'eval_dlg.ui'
**
** Created: Mon Sep 15 20:06:32 2008
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef EVAL_DLG_UI_H
#define EVAL_DLG_UI_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
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

class Ui_eval_dlg_ui
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QListWidget *eval_list;
    QVBoxLayout *vboxLayout1;
    QPushButton *del_btn;
    QSpacerItem *spacerItem;
    QGroupBox *group;
    QHBoxLayout *hboxLayout1;
    QVBoxLayout *vboxLayout2;
    QVBoxLayout *vboxLayout3;
    QLabel *desc_text;
    QLabel *desc_field;
    QHBoxLayout *hboxLayout2;
    QVBoxLayout *vboxLayout4;
    QHBoxLayout *hboxLayout3;
    QCheckBox *def_eval;
    QSpacerItem *spacerItem1;
    QHBoxLayout *hboxLayout4;
    QLabel *orig_text;
    QLabel *orig_field;
    QHBoxLayout *hboxLayout5;
    QLabel *created_label;
    QLabel *created_field;
    QHBoxLayout *hboxLayout6;
    QLabel *program_label;
    QLabel *program_field;
    QHBoxLayout *hboxLayout7;
    QHBoxLayout *hboxLayout8;
    QLabel *user_label;
    QLabel *user_field;
    QSpacerItem *spacerItem2;
    QHBoxLayout *hboxLayout9;
    QLabel *host_label;
    QLabel *host_field;
    QHBoxLayout *hboxLayout10;
    QLabel *modified_label;
    QLabel *modified_field;
    QVBoxLayout *vboxLayout5;
    QSpacerItem *spacerItem3;
    QPushButton *prop_btn;
    QHBoxLayout *hboxLayout11;
    QSpacerItem *spacerItem4;
    QPushButton *cancel_btn;
    QSpacerItem *spacerItem5;
    QPushButton *apply_btn;
    QSpacerItem *spacerItem6;
    QPushButton *ok_btn;
    QSpacerItem *spacerItem7;

    void setupUi(QDialog *eval_dlg_ui)
    {
    if (eval_dlg_ui->objectName().isEmpty())
        eval_dlg_ui->setObjectName(QString::fromUtf8("eval_dlg_ui"));
    eval_dlg_ui->resize(486, 324);
    eval_dlg_ui->setSizeGripEnabled(true);
    gridLayout = new QGridLayout(eval_dlg_ui);
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
    eval_list = new QListWidget(eval_dlg_ui);
    eval_list->setObjectName(QString::fromUtf8("eval_list"));

    hboxLayout->addWidget(eval_list);

    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    del_btn = new QPushButton(eval_dlg_ui);
    del_btn->setObjectName(QString::fromUtf8("del_btn"));

    vboxLayout1->addWidget(del_btn);

    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem);


    hboxLayout->addLayout(vboxLayout1);


    vboxLayout->addLayout(hboxLayout);

    group = new QGroupBox(eval_dlg_ui);
    group->setObjectName(QString::fromUtf8("group"));
    hboxLayout1 = new QHBoxLayout(group);
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(9);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    vboxLayout2 = new QVBoxLayout();
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(0);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    vboxLayout3 = new QVBoxLayout();
    vboxLayout3->setSpacing(6);
    vboxLayout3->setMargin(0);
    vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
    desc_text = new QLabel(group);
    desc_text->setObjectName(QString::fromUtf8("desc_text"));

    vboxLayout3->addWidget(desc_text);

    desc_field = new QLabel(group);
    desc_field->setObjectName(QString::fromUtf8("desc_field"));
    desc_field->setWordWrap(true);

    vboxLayout3->addWidget(desc_field);


    vboxLayout2->addLayout(vboxLayout3);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    vboxLayout4 = new QVBoxLayout();
    vboxLayout4->setSpacing(6);
    vboxLayout4->setMargin(0);
    vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
    hboxLayout3 = new QHBoxLayout();
    hboxLayout3->setSpacing(6);
    hboxLayout3->setMargin(0);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    def_eval = new QCheckBox(group);
    def_eval->setObjectName(QString::fromUtf8("def_eval"));
    def_eval->setEnabled(true);

    hboxLayout3->addWidget(def_eval);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem1);

    hboxLayout4 = new QHBoxLayout();
    hboxLayout4->setSpacing(6);
    hboxLayout4->setMargin(0);
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
    orig_text = new QLabel(group);
    orig_text->setObjectName(QString::fromUtf8("orig_text"));

    hboxLayout4->addWidget(orig_text);

    orig_field = new QLabel(group);
    orig_field->setObjectName(QString::fromUtf8("orig_field"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(5));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(orig_field->sizePolicy().hasHeightForWidth());
    orig_field->setSizePolicy(sizePolicy);

    hboxLayout4->addWidget(orig_field);


    hboxLayout3->addLayout(hboxLayout4);


    vboxLayout4->addLayout(hboxLayout3);

    hboxLayout5 = new QHBoxLayout();
    hboxLayout5->setSpacing(6);
    hboxLayout5->setMargin(0);
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    created_label = new QLabel(group);
    created_label->setObjectName(QString::fromUtf8("created_label"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(5));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(created_label->sizePolicy().hasHeightForWidth());
    created_label->setSizePolicy(sizePolicy1);

    hboxLayout5->addWidget(created_label);

    created_field = new QLabel(group);
    created_field->setObjectName(QString::fromUtf8("created_field"));
    sizePolicy.setHeightForWidth(created_field->sizePolicy().hasHeightForWidth());
    created_field->setSizePolicy(sizePolicy);
    created_field->setMinimumSize(QSize(140, 0));

    hboxLayout5->addWidget(created_field);


    vboxLayout4->addLayout(hboxLayout5);

    hboxLayout6 = new QHBoxLayout();
    hboxLayout6->setSpacing(6);
    hboxLayout6->setMargin(0);
    hboxLayout6->setObjectName(QString::fromUtf8("hboxLayout6"));
    program_label = new QLabel(group);
    program_label->setObjectName(QString::fromUtf8("program_label"));

    hboxLayout6->addWidget(program_label);

    program_field = new QLabel(group);
    program_field->setObjectName(QString::fromUtf8("program_field"));
    sizePolicy.setHeightForWidth(program_field->sizePolicy().hasHeightForWidth());
    program_field->setSizePolicy(sizePolicy);
    program_field->setMinimumSize(QSize(140, 0));
    program_field->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    hboxLayout6->addWidget(program_field);


    vboxLayout4->addLayout(hboxLayout6);

    hboxLayout7 = new QHBoxLayout();
    hboxLayout7->setSpacing(6);
    hboxLayout7->setMargin(0);
    hboxLayout7->setObjectName(QString::fromUtf8("hboxLayout7"));
    hboxLayout8 = new QHBoxLayout();
    hboxLayout8->setSpacing(6);
    hboxLayout8->setMargin(0);
    hboxLayout8->setObjectName(QString::fromUtf8("hboxLayout8"));
    user_label = new QLabel(group);
    user_label->setObjectName(QString::fromUtf8("user_label"));

    hboxLayout8->addWidget(user_label);

    user_field = new QLabel(group);
    user_field->setObjectName(QString::fromUtf8("user_field"));
    sizePolicy.setHeightForWidth(user_field->sizePolicy().hasHeightForWidth());
    user_field->setSizePolicy(sizePolicy);
    user_field->setMinimumSize(QSize(70, 0));

    hboxLayout8->addWidget(user_field);


    hboxLayout7->addLayout(hboxLayout8);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout7->addItem(spacerItem2);

    hboxLayout9 = new QHBoxLayout();
    hboxLayout9->setSpacing(6);
    hboxLayout9->setMargin(0);
    hboxLayout9->setObjectName(QString::fromUtf8("hboxLayout9"));
    host_label = new QLabel(group);
    host_label->setObjectName(QString::fromUtf8("host_label"));

    hboxLayout9->addWidget(host_label);

    host_field = new QLabel(group);
    host_field->setObjectName(QString::fromUtf8("host_field"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(7));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(host_field->sizePolicy().hasHeightForWidth());
    host_field->setSizePolicy(sizePolicy2);
    host_field->setMinimumSize(QSize(60, 0));

    hboxLayout9->addWidget(host_field);


    hboxLayout7->addLayout(hboxLayout9);


    vboxLayout4->addLayout(hboxLayout7);

    hboxLayout10 = new QHBoxLayout();
    hboxLayout10->setSpacing(6);
    hboxLayout10->setMargin(0);
    hboxLayout10->setObjectName(QString::fromUtf8("hboxLayout10"));
    modified_label = new QLabel(group);
    modified_label->setObjectName(QString::fromUtf8("modified_label"));

    hboxLayout10->addWidget(modified_label);

    modified_field = new QLabel(group);
    modified_field->setObjectName(QString::fromUtf8("modified_field"));
    sizePolicy.setHeightForWidth(modified_field->sizePolicy().hasHeightForWidth());
    modified_field->setSizePolicy(sizePolicy);

    hboxLayout10->addWidget(modified_field);


    vboxLayout4->addLayout(hboxLayout10);


    hboxLayout2->addLayout(vboxLayout4);

    vboxLayout5 = new QVBoxLayout();
    vboxLayout5->setSpacing(6);
    vboxLayout5->setMargin(0);
    vboxLayout5->setObjectName(QString::fromUtf8("vboxLayout5"));
    spacerItem3 = new QSpacerItem(117, 91, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout5->addItem(spacerItem3);

    prop_btn = new QPushButton(group);
    prop_btn->setObjectName(QString::fromUtf8("prop_btn"));

    vboxLayout5->addWidget(prop_btn);


    hboxLayout2->addLayout(vboxLayout5);


    vboxLayout2->addLayout(hboxLayout2);


    hboxLayout1->addLayout(vboxLayout2);


    vboxLayout->addWidget(group);

    hboxLayout11 = new QHBoxLayout();
    hboxLayout11->setSpacing(6);
    hboxLayout11->setMargin(0);
    hboxLayout11->setObjectName(QString::fromUtf8("hboxLayout11"));
    spacerItem4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout11->addItem(spacerItem4);

    cancel_btn = new QPushButton(eval_dlg_ui);
    cancel_btn->setObjectName(QString::fromUtf8("cancel_btn"));

    hboxLayout11->addWidget(cancel_btn);

    spacerItem5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout11->addItem(spacerItem5);

    apply_btn = new QPushButton(eval_dlg_ui);
    apply_btn->setObjectName(QString::fromUtf8("apply_btn"));

    hboxLayout11->addWidget(apply_btn);

    spacerItem6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout11->addItem(spacerItem6);

    ok_btn = new QPushButton(eval_dlg_ui);
    ok_btn->setObjectName(QString::fromUtf8("ok_btn"));
    ok_btn->setDefault(true);

    hboxLayout11->addWidget(ok_btn);

    spacerItem7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout11->addItem(spacerItem7);


    vboxLayout->addLayout(hboxLayout11);


    gridLayout->addLayout(vboxLayout, 0, 0, 1, 1);


    retranslateUi(eval_dlg_ui);

    QMetaObject::connectSlotsByName(eval_dlg_ui);
    } // setupUi

    void retranslateUi(QDialog *eval_dlg_ui)
    {
    eval_dlg_ui->setWindowTitle(QApplication::translate("eval_dlg_ui", "Evaluations", 0, QApplication::UnicodeUTF8));
    del_btn->setText(QApplication::translate("eval_dlg_ui", "delete", 0, QApplication::UnicodeUTF8));
    group->setTitle(QApplication::translate("eval_dlg_ui", "detect", 0, QApplication::UnicodeUTF8));
    desc_text->setText(QApplication::translate("eval_dlg_ui", "Description", 0, QApplication::UnicodeUTF8));
    desc_field->setText(QApplication::translate("eval_dlg_ui", "TextLabel", 0, QApplication::UnicodeUTF8));
    def_eval->setText(QApplication::translate("eval_dlg_ui", "default evaluation", 0, QApplication::UnicodeUTF8));
    orig_text->setText(QApplication::translate("eval_dlg_ui", "original evaluation:", 0, QApplication::UnicodeUTF8));
    orig_field->setText(QApplication::translate("eval_dlg_ui", "yes", 0, QApplication::UnicodeUTF8));
    created_label->setText(QApplication::translate("eval_dlg_ui", "created:", 0, QApplication::UnicodeUTF8));
    created_field->setText(QApplication::translate("eval_dlg_ui", "TextLabel4", 0, QApplication::UnicodeUTF8));
    program_label->setText(QApplication::translate("eval_dlg_ui", "program:", 0, QApplication::UnicodeUTF8));
    program_field->setText(QApplication::translate("eval_dlg_ui", "MTKlab", 0, QApplication::UnicodeUTF8));
    user_label->setText(QApplication::translate("eval_dlg_ui", "user:", 0, QApplication::UnicodeUTF8));
    user_field->setText(QApplication::translate("eval_dlg_ui", "rasch", 0, QApplication::UnicodeUTF8));
    host_label->setText(QApplication::translate("eval_dlg_ui", "host:", 0, QApplication::UnicodeUTF8));
    host_field->setText(QApplication::translate("eval_dlg_ui", "spirit", 0, QApplication::UnicodeUTF8));
    modified_label->setText(QApplication::translate("eval_dlg_ui", "last modified:", 0, QApplication::UnicodeUTF8));
    modified_field->setText(QApplication::translate("eval_dlg_ui", "13.06.1970  18:45:00", 0, QApplication::UnicodeUTF8));
    prop_btn->setText(QApplication::translate("eval_dlg_ui", "Event-Class's", 0, QApplication::UnicodeUTF8));
    cancel_btn->setText(QApplication::translate("eval_dlg_ui", "Cancel", 0, QApplication::UnicodeUTF8));
    apply_btn->setText(QApplication::translate("eval_dlg_ui", "Apply", 0, QApplication::UnicodeUTF8));
    ok_btn->setText(QApplication::translate("eval_dlg_ui", "OK", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(eval_dlg_ui);
    } // retranslateUi

};

namespace Ui {
    class eval_dlg_ui: public Ui_eval_dlg_ui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // EVAL_DLG_UI_H
