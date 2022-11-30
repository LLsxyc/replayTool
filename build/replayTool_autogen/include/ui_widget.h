/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <MultiSelectComboBox.h>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "chartview.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout_11;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QCustomPlot *control_plot;
    QSplitter *splitter;
    QTabWidget *tabWidget;
    QWidget *tab_1;
    QGridLayout *gridLayout_2;
    ChartView *chartview;
    QWidget *tab_2;
    QGridLayout *gridLayout_10;
    ChartView *globalview_icu;
    QCustomPlot *prediction_plot;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout_9;
    QSlider *horizontalSlider_local;
    QLineEdit *objSizeLocal;
    QHBoxLayout *horizontalLayout_globalobj;
    QSlider *horizontalSlider_global;
    QLineEdit *objSizeGlobal;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_inputdir;
    QLineEdit *lineEdit_dir;
    QSpacerItem *horizontalSpacer;
    QLabel *label_6;
    QLineEdit *interval_2;
    QLabel *label_7;
    QPushButton *pushButton_analysis;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_13;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *radioButton_front;
    QRadioButton *radioButton_back;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *checkBox_icu;
    QCheckBox *checkBox_prediction;
    QCheckBox *checkBox_control;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_12;
    QGridLayout *gridLayout_6;
    QLabel *label_start;
    QLabel *label_hour;
    QLabel *label_min;
    QLineEdit *lineEdit_end;
    QLineEdit *lineEdit_msec;
    QLabel *label_msec;
    QLabel *label_sec;
    QPushButton *pushButton_search;
    QLineEdit *lineEdit_start;
    QLineEdit *lineEdit_hour;
    QLineEdit *lineEdit_min;
    QLabel *label_end;
    QLineEdit *lineEdit_sec;
    QLineEdit *lineEdit_ctrl_hour;
    QLineEdit *lineEdit_ctrl_min;
    QLineEdit *lineEdit_ctrl_sec;
    QLineEdit *lineEdit_ctrl_msec;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_9;
    QGridLayout *gridLayout_3;
    QPushButton *pushButton_loadmap;
    QLineEdit *interval;
    QPushButton *pushButton_play;
    QPushButton *pushButton_pause;
    QPushButton *pushButton_loadfile;
    QPushButton *pushButton_set;
    QPushButton *pushButton_next;
    QPushButton *pushButton_pre;
    QPushButton *pushButton_reset;
    QPushButton *pushButton_param;
    QLineEdit *lineEdit_num;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_8;
    QGridLayout *gridLayout_7;
    MultiSelectComboBox *comboBox_horizontal;
    QLabel *label_2;
    QLabel *label_3;
    MultiSelectComboBox *comboBox_vertical;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_5;
    QFormLayout *formLayout_2;
    QLabel *label_index;
    QLineEdit *lineEdit_index;
    QLabel *label_framecount;
    QLineEdit *lineEdit_framecount;
    QLabel *label_obscount;
    QLineEdit *lineEdit_obscount;
    QLabel *label;
    QLineEdit *lineEdit_head;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(1676, 919);
        gridLayout_11 = new QGridLayout(Widget);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        centralwidget = new QWidget(Widget);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setMinimumSize(QSize(0, 901));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        control_plot = new QCustomPlot(centralwidget);
        control_plot->setObjectName(QString::fromUtf8("control_plot"));

        gridLayout->addWidget(control_plot, 0, 3, 2, 1);

        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab_1 = new QWidget();
        tab_1->setObjectName(QString::fromUtf8("tab_1"));
        gridLayout_2 = new QGridLayout(tab_1);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        chartview = new ChartView(tab_1);
        chartview->setObjectName(QString::fromUtf8("chartview"));

        gridLayout_2->addWidget(chartview, 0, 0, 1, 1);

        tabWidget->addTab(tab_1, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_10 = new QGridLayout(tab_2);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        globalview_icu = new ChartView(tab_2);
        globalview_icu->setObjectName(QString::fromUtf8("globalview_icu"));

        gridLayout_10->addWidget(globalview_icu, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        splitter->addWidget(tabWidget);
        prediction_plot = new QCustomPlot(splitter);
        prediction_plot->setObjectName(QString::fromUtf8("prediction_plot"));
        splitter->addWidget(prediction_plot);

        gridLayout->addWidget(splitter, 1, 0, 2, 3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        horizontalSlider_local = new QSlider(centralwidget);
        horizontalSlider_local->setObjectName(QString::fromUtf8("horizontalSlider_local"));
        horizontalSlider_local->setOrientation(Qt::Horizontal);

        horizontalLayout_9->addWidget(horizontalSlider_local);

        objSizeLocal = new QLineEdit(centralwidget);
        objSizeLocal->setObjectName(QString::fromUtf8("objSizeLocal"));

        horizontalLayout_9->addWidget(objSizeLocal);

        horizontalLayout_globalobj = new QHBoxLayout();
        horizontalLayout_globalobj->setObjectName(QString::fromUtf8("horizontalLayout_globalobj"));
        horizontalSlider_global = new QSlider(centralwidget);
        horizontalSlider_global->setObjectName(QString::fromUtf8("horizontalSlider_global"));
        horizontalSlider_global->setOrientation(Qt::Horizontal);

        horizontalLayout_globalobj->addWidget(horizontalSlider_global);

        objSizeGlobal = new QLineEdit(centralwidget);
        objSizeGlobal->setObjectName(QString::fromUtf8("objSizeGlobal"));

        horizontalLayout_globalobj->addWidget(objSizeGlobal);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton_inputdir = new QPushButton(centralwidget);
        pushButton_inputdir->setObjectName(QString::fromUtf8("pushButton_inputdir"));

        horizontalLayout->addWidget(pushButton_inputdir);

        lineEdit_dir = new QLineEdit(centralwidget);
        lineEdit_dir->setObjectName(QString::fromUtf8("lineEdit_dir"));
        QFont font;
        font.setPointSize(11);
        lineEdit_dir->setFont(font);
        lineEdit_dir->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(lineEdit_dir);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout->addWidget(label_6);

        interval_2 = new QLineEdit(centralwidget);
        interval_2->setObjectName(QString::fromUtf8("interval_2"));
        interval_2->setFont(font);
        interval_2->setFocusPolicy(Qt::ClickFocus);

        horizontalLayout->addWidget(interval_2);

        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout->addWidget(label_7);

        pushButton_analysis = new QPushButton(centralwidget);
        pushButton_analysis->setObjectName(QString::fromUtf8("pushButton_analysis"));

        horizontalLayout->addWidget(pushButton_analysis);


        horizontalLayout_globalobj->addLayout(horizontalLayout);


        horizontalLayout_9->addLayout(horizontalLayout_globalobj);


        horizontalLayout_2->addLayout(horizontalLayout_9);


        gridLayout->addLayout(horizontalLayout_2, 0, 2, 1, 1);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_4 = new QGridLayout(groupBox);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        groupBox_6 = new QGroupBox(groupBox);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        gridLayout_13 = new QGridLayout(groupBox_6);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        radioButton_front = new QRadioButton(groupBox_6);
        radioButton_front->setObjectName(QString::fromUtf8("radioButton_front"));

        horizontalLayout_3->addWidget(radioButton_front);

        radioButton_back = new QRadioButton(groupBox_6);
        radioButton_back->setObjectName(QString::fromUtf8("radioButton_back"));

        horizontalLayout_3->addWidget(radioButton_back);


        gridLayout_13->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        checkBox_icu = new QCheckBox(groupBox_6);
        checkBox_icu->setObjectName(QString::fromUtf8("checkBox_icu"));

        horizontalLayout_4->addWidget(checkBox_icu);

        checkBox_prediction = new QCheckBox(groupBox_6);
        checkBox_prediction->setObjectName(QString::fromUtf8("checkBox_prediction"));

        horizontalLayout_4->addWidget(checkBox_prediction);

        checkBox_control = new QCheckBox(groupBox_6);
        checkBox_control->setObjectName(QString::fromUtf8("checkBox_control"));

        horizontalLayout_4->addWidget(checkBox_control);


        gridLayout_13->addLayout(horizontalLayout_4, 1, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_6, 1, 1, 2, 1);

        groupBox_5 = new QGroupBox(groupBox);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_12 = new QGridLayout(groupBox_5);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        gridLayout_6 = new QGridLayout();
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        label_start = new QLabel(groupBox_5);
        label_start->setObjectName(QString::fromUtf8("label_start"));

        gridLayout_6->addWidget(label_start, 5, 1, 1, 1);

        label_hour = new QLabel(groupBox_5);
        label_hour->setObjectName(QString::fromUtf8("label_hour"));

        gridLayout_6->addWidget(label_hour, 0, 1, 1, 1);

        label_min = new QLabel(groupBox_5);
        label_min->setObjectName(QString::fromUtf8("label_min"));

        gridLayout_6->addWidget(label_min, 1, 1, 1, 1);

        lineEdit_end = new QLineEdit(groupBox_5);
        lineEdit_end->setObjectName(QString::fromUtf8("lineEdit_end"));
        lineEdit_end->setFocusPolicy(Qt::ClickFocus);

        gridLayout_6->addWidget(lineEdit_end, 6, 0, 1, 1);

        lineEdit_msec = new QLineEdit(groupBox_5);
        lineEdit_msec->setObjectName(QString::fromUtf8("lineEdit_msec"));
        lineEdit_msec->setFocusPolicy(Qt::ClickFocus);

        gridLayout_6->addWidget(lineEdit_msec, 3, 0, 1, 1);

        label_msec = new QLabel(groupBox_5);
        label_msec->setObjectName(QString::fromUtf8("label_msec"));

        gridLayout_6->addWidget(label_msec, 3, 1, 1, 1);

        label_sec = new QLabel(groupBox_5);
        label_sec->setObjectName(QString::fromUtf8("label_sec"));

        gridLayout_6->addWidget(label_sec, 2, 1, 1, 1);

        pushButton_search = new QPushButton(groupBox_5);
        pushButton_search->setObjectName(QString::fromUtf8("pushButton_search"));

        gridLayout_6->addWidget(pushButton_search, 4, 1, 1, 1);

        lineEdit_start = new QLineEdit(groupBox_5);
        lineEdit_start->setObjectName(QString::fromUtf8("lineEdit_start"));
        lineEdit_start->setFocusPolicy(Qt::ClickFocus);

        gridLayout_6->addWidget(lineEdit_start, 5, 0, 1, 1);

        lineEdit_hour = new QLineEdit(groupBox_5);
        lineEdit_hour->setObjectName(QString::fromUtf8("lineEdit_hour"));
        lineEdit_hour->setFocusPolicy(Qt::ClickFocus);

        gridLayout_6->addWidget(lineEdit_hour, 0, 0, 1, 1);

        lineEdit_min = new QLineEdit(groupBox_5);
        lineEdit_min->setObjectName(QString::fromUtf8("lineEdit_min"));
        lineEdit_min->setFocusPolicy(Qt::ClickFocus);

        gridLayout_6->addWidget(lineEdit_min, 1, 0, 1, 1);

        label_end = new QLabel(groupBox_5);
        label_end->setObjectName(QString::fromUtf8("label_end"));

        gridLayout_6->addWidget(label_end, 6, 1, 1, 1);

        lineEdit_sec = new QLineEdit(groupBox_5);
        lineEdit_sec->setObjectName(QString::fromUtf8("lineEdit_sec"));
        lineEdit_sec->setFocusPolicy(Qt::ClickFocus);

        gridLayout_6->addWidget(lineEdit_sec, 2, 0, 1, 1);

        lineEdit_ctrl_hour = new QLineEdit(groupBox_5);
        lineEdit_ctrl_hour->setObjectName(QString::fromUtf8("lineEdit_ctrl_hour"));

        gridLayout_6->addWidget(lineEdit_ctrl_hour, 0, 2, 1, 1);

        lineEdit_ctrl_min = new QLineEdit(groupBox_5);
        lineEdit_ctrl_min->setObjectName(QString::fromUtf8("lineEdit_ctrl_min"));

        gridLayout_6->addWidget(lineEdit_ctrl_min, 1, 2, 1, 1);

        lineEdit_ctrl_sec = new QLineEdit(groupBox_5);
        lineEdit_ctrl_sec->setObjectName(QString::fromUtf8("lineEdit_ctrl_sec"));

        gridLayout_6->addWidget(lineEdit_ctrl_sec, 2, 2, 1, 1);

        lineEdit_ctrl_msec = new QLineEdit(groupBox_5);
        lineEdit_ctrl_msec->setObjectName(QString::fromUtf8("lineEdit_ctrl_msec"));

        gridLayout_6->addWidget(lineEdit_ctrl_msec, 3, 2, 1, 1);


        gridLayout_12->addLayout(gridLayout_6, 0, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_5, 2, 0, 2, 1);

        groupBox_2 = new QGroupBox(groupBox);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_9 = new QGridLayout(groupBox_2);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        pushButton_loadmap = new QPushButton(groupBox_2);
        pushButton_loadmap->setObjectName(QString::fromUtf8("pushButton_loadmap"));

        gridLayout_3->addWidget(pushButton_loadmap, 2, 2, 1, 1);

        interval = new QLineEdit(groupBox_2);
        interval->setObjectName(QString::fromUtf8("interval"));
        interval->setFocusPolicy(Qt::ClickFocus);

        gridLayout_3->addWidget(interval, 0, 0, 1, 1);

        pushButton_play = new QPushButton(groupBox_2);
        pushButton_play->setObjectName(QString::fromUtf8("pushButton_play"));

        gridLayout_3->addWidget(pushButton_play, 1, 1, 1, 1);

        pushButton_pause = new QPushButton(groupBox_2);
        pushButton_pause->setObjectName(QString::fromUtf8("pushButton_pause"));

        gridLayout_3->addWidget(pushButton_pause, 1, 2, 1, 1);

        pushButton_loadfile = new QPushButton(groupBox_2);
        pushButton_loadfile->setObjectName(QString::fromUtf8("pushButton_loadfile"));

        gridLayout_3->addWidget(pushButton_loadfile, 3, 2, 1, 1);

        pushButton_set = new QPushButton(groupBox_2);
        pushButton_set->setObjectName(QString::fromUtf8("pushButton_set"));

        gridLayout_3->addWidget(pushButton_set, 0, 1, 1, 1);

        pushButton_next = new QPushButton(groupBox_2);
        pushButton_next->setObjectName(QString::fromUtf8("pushButton_next"));

        gridLayout_3->addWidget(pushButton_next, 3, 1, 1, 1);

        pushButton_pre = new QPushButton(groupBox_2);
        pushButton_pre->setObjectName(QString::fromUtf8("pushButton_pre"));

        gridLayout_3->addWidget(pushButton_pre, 2, 1, 1, 1);

        pushButton_reset = new QPushButton(groupBox_2);
        pushButton_reset->setObjectName(QString::fromUtf8("pushButton_reset"));

        gridLayout_3->addWidget(pushButton_reset, 4, 1, 1, 1);

        pushButton_param = new QPushButton(groupBox_2);
        pushButton_param->setObjectName(QString::fromUtf8("pushButton_param"));

        gridLayout_3->addWidget(pushButton_param, 4, 2, 1, 1);

        lineEdit_num = new QLineEdit(groupBox_2);
        lineEdit_num->setObjectName(QString::fromUtf8("lineEdit_num"));

        gridLayout_3->addWidget(lineEdit_num, 4, 0, 1, 1);


        gridLayout_9->addLayout(gridLayout_3, 0, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_2, 0, 0, 2, 1);

        groupBox_4 = new QGroupBox(groupBox);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        gridLayout_8 = new QGridLayout(groupBox_4);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        gridLayout_7 = new QGridLayout();
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        comboBox_horizontal = new MultiSelectComboBox(groupBox_4);
        comboBox_horizontal->setObjectName(QString::fromUtf8("comboBox_horizontal"));

        gridLayout_7->addWidget(comboBox_horizontal, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox_4);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_7->addWidget(label_2, 0, 0, 1, 1);

        label_3 = new QLabel(groupBox_4);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_7->addWidget(label_3, 1, 0, 1, 1);

        comboBox_vertical = new MultiSelectComboBox(groupBox_4);
        comboBox_vertical->setObjectName(QString::fromUtf8("comboBox_vertical"));

        gridLayout_7->addWidget(comboBox_vertical, 1, 1, 1, 1);


        gridLayout_8->addLayout(gridLayout_7, 0, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_4, 0, 1, 1, 1);

        groupBox_3 = new QGroupBox(groupBox);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_5 = new QGridLayout(groupBox_3);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label_index = new QLabel(groupBox_3);
        label_index->setObjectName(QString::fromUtf8("label_index"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_index);

        lineEdit_index = new QLineEdit(groupBox_3);
        lineEdit_index->setObjectName(QString::fromUtf8("lineEdit_index"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, lineEdit_index);

        label_framecount = new QLabel(groupBox_3);
        label_framecount->setObjectName(QString::fromUtf8("label_framecount"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_framecount);

        lineEdit_framecount = new QLineEdit(groupBox_3);
        lineEdit_framecount->setObjectName(QString::fromUtf8("lineEdit_framecount"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, lineEdit_framecount);

        label_obscount = new QLabel(groupBox_3);
        label_obscount->setObjectName(QString::fromUtf8("label_obscount"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_obscount);

        lineEdit_obscount = new QLineEdit(groupBox_3);
        lineEdit_obscount->setObjectName(QString::fromUtf8("lineEdit_obscount"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, lineEdit_obscount);

        label = new QLabel(groupBox_3);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label);

        lineEdit_head = new QLineEdit(groupBox_3);
        lineEdit_head->setObjectName(QString::fromUtf8("lineEdit_head"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, lineEdit_head);


        gridLayout_5->addLayout(formLayout_2, 0, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_3, 3, 1, 1, 1);


        gridLayout->addWidget(groupBox, 2, 3, 1, 1);


        gridLayout_11->addWidget(centralwidget, 0, 0, 1, 1);


        retranslateUi(Widget);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "LogAnalysisTool", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_1), QCoreApplication::translate("Widget", "local", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("Widget", "global", nullptr));
        pushButton_inputdir->setText(QCoreApplication::translate("Widget", "\351\200\211\346\213\251\346\226\207\344\273\266\345\244\271", nullptr));
        label_6->setText(QCoreApplication::translate("Widget", "\351\232\234\347\242\215\347\211\251\345\207\272\347\216\260\351\227\264\351\232\224:", nullptr));
        interval_2->setText(QCoreApplication::translate("Widget", "0.5", nullptr));
        label_7->setText(QCoreApplication::translate("Widget", "\347\247\222", nullptr));
        pushButton_analysis->setText(QCoreApplication::translate("Widget", "\350\247\243\346\236\220", nullptr));
        groupBox->setTitle(QString());
        groupBox_6->setTitle(QCoreApplication::translate("Widget", "\346\222\255\346\224\276\346\250\241\345\274\217\350\256\276\347\275\256", nullptr));
        radioButton_front->setText(QCoreApplication::translate("Widget", "\345\211\215\345\220\221", nullptr));
        radioButton_back->setText(QCoreApplication::translate("Widget", "\345\220\216\345\220\221", nullptr));
        checkBox_icu->setText(QCoreApplication::translate("Widget", "ICU", nullptr));
        checkBox_prediction->setText(QCoreApplication::translate("Widget", "CCU", nullptr));
        checkBox_control->setText(QCoreApplication::translate("Widget", "Control", nullptr));
        groupBox_5->setTitle(QString());
        label_start->setText(QCoreApplication::translate("Widget", "\350\265\267\347\202\271\350\241\214\346\225\260", nullptr));
        label_hour->setText(QCoreApplication::translate("Widget", "\346\227\266", nullptr));
        label_min->setText(QCoreApplication::translate("Widget", "\345\210\206", nullptr));
        lineEdit_end->setText(QCoreApplication::translate("Widget", "0", nullptr));
        label_msec->setText(QCoreApplication::translate("Widget", "\346\257\253\347\247\222", nullptr));
        label_sec->setText(QCoreApplication::translate("Widget", "\347\247\222", nullptr));
        pushButton_search->setText(QCoreApplication::translate("Widget", "\346\220\234\347\264\242", nullptr));
        lineEdit_start->setText(QCoreApplication::translate("Widget", "0", nullptr));
        label_end->setText(QCoreApplication::translate("Widget", "\347\273\210\347\202\271\350\241\214\346\225\260", nullptr));
        groupBox_2->setTitle(QString());
        pushButton_loadmap->setText(QCoreApplication::translate("Widget", "\345\212\240\350\275\275\345\234\260\345\233\276", nullptr));
        interval->setText(QCoreApplication::translate("Widget", "50", nullptr));
        pushButton_play->setText(QCoreApplication::translate("Widget", "\346\222\255\346\224\276", nullptr));
        pushButton_pause->setText(QCoreApplication::translate("Widget", "\346\232\202\345\201\234", nullptr));
        pushButton_loadfile->setText(QCoreApplication::translate("Widget", "\345\212\240\350\275\275\346\226\207\344\273\266", nullptr));
        pushButton_set->setText(QCoreApplication::translate("Widget", "\350\256\276\347\275\256", nullptr));
        pushButton_next->setText(QCoreApplication::translate("Widget", "\344\270\213\344\270\200\345\270\247", nullptr));
        pushButton_pre->setText(QCoreApplication::translate("Widget", "\344\270\212\344\270\200\345\270\247", nullptr));
        pushButton_reset->setText(QCoreApplication::translate("Widget", "\351\207\215\347\275\256", nullptr));
        pushButton_param->setText(QCoreApplication::translate("Widget", "\345\212\240\350\275\275\345\217\202\346\225\260", nullptr));
        lineEdit_num->setText(QCoreApplication::translate("Widget", "1", nullptr));
        groupBox_4->setTitle(QString());
        label_2->setText(QCoreApplication::translate("Widget", "\346\250\252\345\220\221", nullptr));
        label_3->setText(QCoreApplication::translate("Widget", "\347\272\265\345\220\221", nullptr));
        groupBox_3->setTitle(QString());
        label_index->setText(QCoreApplication::translate("Widget", "\345\272\217\345\217\267", nullptr));
        label_framecount->setText(QCoreApplication::translate("Widget", "\345\270\247\346\225\260", nullptr));
        label_obscount->setText(QCoreApplication::translate("Widget", "\351\232\234\347\242\215\347\211\251\346\225\260\351\207\217", nullptr));
        label->setText(QCoreApplication::translate("Widget", "\350\210\252\345\220\221", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
