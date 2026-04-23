// conducteö - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2020, Clément MARCEL.
//
// This file is part of conducteö software.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the  Free Software Foundation,  either version 3 of the License,  or
// (at your option) any later version.
//
// This program is  distributed  in  the  hope that it  will be useful,
// but  WITHOUT ANY WARRANTY ;  without even  the  implied  warranty of
// MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU  General  Public  License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <Preferences.h>
#include <materials/MaterialCategory.h>
#include <databases/Materials.h>
#include <LinguistManager.h>
#include <QDesktopServices>
#include <StatesManager.h>
#include <tools/ToolBox.h>
#include <MainWindow.h>
#include <LocalData.h>
#include <QFileDialog>
#include <QMenu>
#include <QFile>
#include <Log.h>

Preferences::Preferences(QWidget *parent):
    QMainWindow(parent),
    catchUserInputs(false)
{
    setWindowModality(Qt::ApplicationModal);

    centralWidget = new QWidget(this);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));
    gridLayout_3 = new QGridLayout(centralWidget);
    gridLayout_3->setSpacing(6);
    gridLayout_3->setContentsMargins(11, 11, 11, 11);
    gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
    tabWidget = new QTabWidget(centralWidget);
    tabWidget->setObjectName(QStringLiteral("tabWidget"));
    tab = new QWidget();
    tab->setObjectName(QStringLiteral("tab"));
    gridLayout = new QGridLayout(tab);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11, 11, 11, 11);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));
    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(6);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    label = new QLabel(tab);
    label->setObjectName(QStringLiteral("label"));
    label->setMinimumSize(QSize(150, 0));

    horizontalLayout->addWidget(label);

    comboBox = new QComboBox(tab);
    comboBox->setObjectName(QStringLiteral("comboBox"));
    comboBox->setMinimumSize(QSize(120, 0));
    comboBox->setMaximumSize(QSize(120, 16777215));

    horizontalLayout->addWidget(comboBox);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);


    verticalLayout->addLayout(horizontalLayout);

    horizontalLayout_c1 = new QHBoxLayout();
    horizontalLayout_c1->setSpacing(6);
    horizontalLayout_c1->setObjectName(QStringLiteral("horizontalLayout_c1"));
    label_c1 = new QLabel(tab);
    label_c1->setObjectName(QStringLiteral("label_c1"));
    label_c1->setMinimumSize(QSize(150, 0));

    horizontalLayout_c1->addWidget(label_c1);

    comboBox_c1 = new QComboBox(tab);
    comboBox_c1->setObjectName(QStringLiteral("comboBox_c1"));
    comboBox_c1->setMinimumSize(QSize(120, 0));
    comboBox_c1->setMaximumSize(QSize(120, 16777215));

    horizontalLayout_c1->addWidget(comboBox_c1);

    horizontalSpacer_c1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_c1->addItem(horizontalSpacer_c1);


    verticalLayout->addLayout(horizontalLayout_c1);

	// Dxf materials.
	hLayout_dxf_materials = new QHBoxLayout();
	hLayout_dxf_materials->setSpacing(6);

	label_dxf_materials = new QLabel(tab);
	label_dxf_materials->setMinimumSize(QSize(150, 0));
	label_dxf_materials->setMaximumSize(QSize(150, 16777215));

	hLayout_dxf_materials->addWidget(label_dxf_materials);

	combobox_dxf_materials = new QComboBox(tab);
	hLayout_dxf_materials->addWidget(combobox_dxf_materials);

	combobox_dxf_materials->setMinimumSize(QSize(120, 0));
	combobox_dxf_materials->setMaximumSize(QSize(120, 16777215));

	hSpace_dxf_materials = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	hLayout_dxf_materials->addItem(hSpace_dxf_materials);

	verticalLayout->addLayout(hLayout_dxf_materials);

	// End of Dxf materials.

    horizontalLayout_compute_on_quit = new QHBoxLayout();
    horizontalLayout_compute_on_quit->setSpacing(6);
    horizontalLayout_compute_on_quit->setObjectName(QStringLiteral("horizontalLayout_compute_on_quit"));
    label_compute_on_quit = new QLabel(tab);
    label_compute_on_quit->setObjectName(QStringLiteral("label_compute_on_quit"));
    label_compute_on_quit->setMinimumSize(QSize(150, 0));
    label_compute_on_quit->setMaximumSize(QSize(150, 16777215));

    horizontalLayout_compute_on_quit->addWidget(label_compute_on_quit);

    checkBox_compute_on_quit = new QCheckBox(tab);
    checkBox_compute_on_quit->setObjectName(QStringLiteral("checkBox_compute_on_quit"));

    horizontalLayout_compute_on_quit->addWidget(checkBox_compute_on_quit);

    verticalLayout->addLayout(horizontalLayout_compute_on_quit);

    verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout->addItem(verticalSpacer_3);


    gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

    tabWidget->addTab(tab, QString());
    tab_2 = new QWidget();
    tab_2->setObjectName(QStringLiteral("tab_2"));
    gridLayout_2 = new QGridLayout(tab_2);
    gridLayout_2->setSpacing(6);
    gridLayout_2->setContentsMargins(11, 11, 11, 11);
    gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
    verticalLayout_7 = new QVBoxLayout();
    verticalLayout_7->setSpacing(6);
    verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
    label_5 = new QLabel(tab_2);
    label_5->setObjectName(QStringLiteral("label_5"));

    verticalLayout_7->addWidget(label_5);

    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setSpacing(6);
    horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
    horizontalSpacer_6 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    horizontalLayout_4->addItem(horizontalSpacer_6);

    listWidget = new QListWidget(tab_2);
    listWidget->setObjectName(QStringLiteral("listWidget"));

    horizontalLayout_4->addWidget(listWidget);

    verticalLayout_2 = new QVBoxLayout();
    verticalLayout_2->setSpacing(6);
    verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
    pushButton_2 = new QPushButton(tab_2);
    pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

    verticalLayout_2->addWidget(pushButton_2);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout_2->addItem(verticalSpacer);


    horizontalLayout_4->addLayout(verticalLayout_2);

    horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_4->addItem(horizontalSpacer_5);


    verticalLayout_7->addLayout(horizontalLayout_4);

    label_6 = new QLabel(tab_2);
    label_6->setObjectName(QStringLiteral("label_6"));

    verticalLayout_7->addWidget(label_6);

    horizontalLayout_24 = new QHBoxLayout();
    horizontalLayout_24->setSpacing(6);
    horizontalLayout_24->setObjectName(QStringLiteral("horizontalLayout_24"));
    verticalLayout_6 = new QVBoxLayout();
    verticalLayout_6->setSpacing(6);
    verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
    horizontalSpacer_8 = new QSpacerItem(230, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    verticalLayout_6->addItem(horizontalSpacer_8);

    horizontalLayout_20 = new QHBoxLayout();
    horizontalLayout_20->setSpacing(6);
    horizontalLayout_20->setObjectName(QStringLiteral("horizontalLayout_20"));
    horizontalSpacer_7 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    horizontalLayout_20->addItem(horizontalSpacer_7);

    label_7 = new QLabel(tab_2);
    label_7->setObjectName(QStringLiteral("label_7"));
    label_7->setMinimumSize(QSize(200, 0));
    label_7->setMaximumSize(QSize(200, 16777215));

    horizontalLayout_20->addWidget(label_7);


    verticalLayout_6->addLayout(horizontalLayout_20);

    horizontalLayout_21 = new QHBoxLayout();
    horizontalLayout_21->setSpacing(6);
    horizontalLayout_21->setObjectName(QStringLiteral("horizontalLayout_21"));
    horizontalSpacer_11 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    horizontalLayout_21->addItem(horizontalSpacer_11);

    label_11 = new QLabel(tab_2);
    label_11->setObjectName(QStringLiteral("label_11"));
    label_11->setMinimumSize(QSize(200, 0));
    label_11->setMaximumSize(QSize(200, 16777215));

    horizontalLayout_21->addWidget(label_11);


    verticalLayout_6->addLayout(horizontalLayout_21);

    horizontalLayout_22 = new QHBoxLayout();
    horizontalLayout_22->setSpacing(6);
    horizontalLayout_22->setObjectName(QStringLiteral("horizontalLayout_22"));
    horizontalSpacer_12 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    horizontalLayout_22->addItem(horizontalSpacer_12);

    label_12 = new QLabel(tab_2);
    label_12->setObjectName(QStringLiteral("label_12"));
    label_12->setMinimumSize(QSize(200, 0));
    label_12->setMaximumSize(QSize(200, 16777215));

    horizontalLayout_22->addWidget(label_12);


    verticalLayout_6->addLayout(horizontalLayout_22);

    horizontalLayout_23 = new QHBoxLayout();
    horizontalLayout_23->setSpacing(6);
    horizontalLayout_23->setObjectName(QStringLiteral("horizontalLayout_23"));
    horizontalSpacer_13 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    horizontalLayout_23->addItem(horizontalSpacer_13);

    label_13 = new QLabel(tab_2);
    label_13->setObjectName(QStringLiteral("label_13"));
    label_13->setMinimumSize(QSize(200, 0));
    label_13->setMaximumSize(QSize(200, 16777215));

    horizontalLayout_23->addWidget(label_13);


    verticalLayout_6->addLayout(horizontalLayout_23);


    horizontalLayout_24->addLayout(verticalLayout_6);

    verticalLayout_3 = new QVBoxLayout();
    verticalLayout_3->setSpacing(6);
    verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
    horizontalLayout_9 = new QHBoxLayout();
    horizontalLayout_9->setSpacing(6);
    horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
    horizontalSpacer_26 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_9->addItem(horizontalSpacer_26);

    label_8 = new QLabel(tab_2);
    label_8->setObjectName(QStringLiteral("label_8"));
    label_8->setMinimumSize(QSize(60, 0));
    label_8->setMaximumSize(QSize(60, 16777215));
    label_8->setAlignment(Qt::AlignCenter);

    horizontalLayout_9->addWidget(label_8);

    horizontalSpacer_27 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_9->addItem(horizontalSpacer_27);


    verticalLayout_3->addLayout(horizontalLayout_9);

    horizontalLayout_5 = new QHBoxLayout();
    horizontalLayout_5->setSpacing(6);
    horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
    horizontalSpacer_18 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_5->addItem(horizontalSpacer_18);

    checkBox = new QCheckBox(tab_2);
    checkBox->setObjectName(QStringLiteral("checkBox"));
    checkBox->setMinimumSize(QSize(0, 0));
    checkBox->setMaximumSize(QSize(16777215, 16777215));

    horizontalLayout_5->addWidget(checkBox);

    horizontalSpacer_19 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_5->addItem(horizontalSpacer_19);


    verticalLayout_3->addLayout(horizontalLayout_5);

    horizontalLayout_6 = new QHBoxLayout();
    horizontalLayout_6->setSpacing(6);
    horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
    horizontalSpacer_20 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_6->addItem(horizontalSpacer_20);

    checkBox_6 = new QCheckBox(tab_2);
    checkBox_6->setObjectName(QStringLiteral("checkBox_6"));

    horizontalLayout_6->addWidget(checkBox_6);

    horizontalSpacer_23 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_6->addItem(horizontalSpacer_23);


    verticalLayout_3->addLayout(horizontalLayout_6);

    horizontalLayout_7 = new QHBoxLayout();
    horizontalLayout_7->setSpacing(6);
    horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
    horizontalSpacer_21 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_7->addItem(horizontalSpacer_21);

    checkBox_9 = new QCheckBox(tab_2);
    checkBox_9->setObjectName(QStringLiteral("checkBox_9"));

    horizontalLayout_7->addWidget(checkBox_9);

    horizontalSpacer_24 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_7->addItem(horizontalSpacer_24);


    verticalLayout_3->addLayout(horizontalLayout_7);

    horizontalLayout_8 = new QHBoxLayout();
    horizontalLayout_8->setSpacing(6);
    horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
    horizontalSpacer_22 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_8->addItem(horizontalSpacer_22);

    checkBox_12 = new QCheckBox(tab_2);
    checkBox_12->setObjectName(QStringLiteral("checkBox_12"));

    horizontalLayout_8->addWidget(checkBox_12);

    horizontalSpacer_25 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_8->addItem(horizontalSpacer_25);


    verticalLayout_3->addLayout(horizontalLayout_8);


    horizontalLayout_24->addLayout(verticalLayout_3);

    verticalLayout_4 = new QVBoxLayout();
    verticalLayout_4->setSpacing(6);
    verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
    horizontalLayout_10 = new QHBoxLayout();
    horizontalLayout_10->setSpacing(6);
    horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
    horizontalSpacer_28 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_10->addItem(horizontalSpacer_28);

    label_9 = new QLabel(tab_2);
    label_9->setObjectName(QStringLiteral("label_9"));
    label_9->setMinimumSize(QSize(60, 0));
    label_9->setMaximumSize(QSize(60, 16777215));
    label_9->setAlignment(Qt::AlignCenter);

    horizontalLayout_10->addWidget(label_9);

    horizontalSpacer_29 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_10->addItem(horizontalSpacer_29);


    verticalLayout_4->addLayout(horizontalLayout_10);

    horizontalLayout_11 = new QHBoxLayout();
    horizontalLayout_11->setSpacing(6);
    horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
    horizontalSpacer_30 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_11->addItem(horizontalSpacer_30);

    checkBox_2 = new QCheckBox(tab_2);
    checkBox_2->setObjectName(QStringLiteral("checkBox_2"));
    checkBox_2->setMinimumSize(QSize(0, 0));
    checkBox_2->setMaximumSize(QSize(60, 16777215));

    horizontalLayout_11->addWidget(checkBox_2);

    horizontalSpacer_31 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_11->addItem(horizontalSpacer_31);


    verticalLayout_4->addLayout(horizontalLayout_11);

    horizontalLayout_12 = new QHBoxLayout();
    horizontalLayout_12->setSpacing(6);
    horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
    horizontalSpacer_32 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_12->addItem(horizontalSpacer_32);

    checkBox_5 = new QCheckBox(tab_2);
    checkBox_5->setObjectName(QStringLiteral("checkBox_5"));

    horizontalLayout_12->addWidget(checkBox_5);

    horizontalSpacer_33 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_12->addItem(horizontalSpacer_33);


    verticalLayout_4->addLayout(horizontalLayout_12);

    horizontalLayout_13 = new QHBoxLayout();
    horizontalLayout_13->setSpacing(6);
    horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
    horizontalSpacer_34 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_13->addItem(horizontalSpacer_34);

    checkBox_8 = new QCheckBox(tab_2);
    checkBox_8->setObjectName(QStringLiteral("checkBox_8"));

    horizontalLayout_13->addWidget(checkBox_8);

    horizontalSpacer_35 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_13->addItem(horizontalSpacer_35);


    verticalLayout_4->addLayout(horizontalLayout_13);

    horizontalLayout_14 = new QHBoxLayout();
    horizontalLayout_14->setSpacing(6);
    horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
    horizontalSpacer_36 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_14->addItem(horizontalSpacer_36);

    checkBox_11 = new QCheckBox(tab_2);
    checkBox_11->setObjectName(QStringLiteral("checkBox_11"));

    horizontalLayout_14->addWidget(checkBox_11);

    horizontalSpacer_37 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_14->addItem(horizontalSpacer_37);


    verticalLayout_4->addLayout(horizontalLayout_14);


    horizontalLayout_24->addLayout(verticalLayout_4);

    verticalLayout_5 = new QVBoxLayout();
    verticalLayout_5->setSpacing(6);
    verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
    horizontalLayout_15 = new QHBoxLayout();
    horizontalLayout_15->setSpacing(6);
    horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
    horizontalSpacer_38 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_15->addItem(horizontalSpacer_38);

    label_10 = new QLabel(tab_2);
    label_10->setObjectName(QStringLiteral("label_10"));
    label_10->setMinimumSize(QSize(60, 0));
    label_10->setMaximumSize(QSize(60, 16777215));
    label_10->setAlignment(Qt::AlignCenter);

    horizontalLayout_15->addWidget(label_10);

    horizontalSpacer_39 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_15->addItem(horizontalSpacer_39);


    verticalLayout_5->addLayout(horizontalLayout_15);

    horizontalLayout_16 = new QHBoxLayout();
    horizontalLayout_16->setSpacing(6);
    horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
    horizontalSpacer_40 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_16->addItem(horizontalSpacer_40);

    checkBox_3 = new QCheckBox(tab_2);
    checkBox_3->setObjectName(QStringLiteral("checkBox_3"));

    horizontalLayout_16->addWidget(checkBox_3);

    horizontalSpacer_41 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_16->addItem(horizontalSpacer_41);


    verticalLayout_5->addLayout(horizontalLayout_16);

    horizontalLayout_17 = new QHBoxLayout();
    horizontalLayout_17->setSpacing(6);
    horizontalLayout_17->setObjectName(QStringLiteral("horizontalLayout_17"));
    horizontalSpacer_42 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_17->addItem(horizontalSpacer_42);

    checkBox_4 = new QCheckBox(tab_2);
    checkBox_4->setObjectName(QStringLiteral("checkBox_4"));

    horizontalLayout_17->addWidget(checkBox_4);

    horizontalSpacer_43 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_17->addItem(horizontalSpacer_43);


    verticalLayout_5->addLayout(horizontalLayout_17);

    horizontalLayout_18 = new QHBoxLayout();
    horizontalLayout_18->setSpacing(6);
    horizontalLayout_18->setObjectName(QStringLiteral("horizontalLayout_18"));
    horizontalSpacer_44 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_18->addItem(horizontalSpacer_44);

    checkBox_7 = new QCheckBox(tab_2);
    checkBox_7->setObjectName(QStringLiteral("checkBox_7"));

    horizontalLayout_18->addWidget(checkBox_7);

    horizontalSpacer_45 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_18->addItem(horizontalSpacer_45);


    verticalLayout_5->addLayout(horizontalLayout_18);

    horizontalLayout_19 = new QHBoxLayout();
    horizontalLayout_19->setSpacing(6);
    horizontalLayout_19->setObjectName(QStringLiteral("horizontalLayout_19"));
    horizontalSpacer_46 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_19->addItem(horizontalSpacer_46);

    checkBox_10 = new QCheckBox(tab_2);
    checkBox_10->setObjectName(QStringLiteral("checkBox_10"));

    horizontalLayout_19->addWidget(checkBox_10);

    horizontalSpacer_47 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_19->addItem(horizontalSpacer_47);


    verticalLayout_5->addLayout(horizontalLayout_19);


    horizontalLayout_24->addLayout(verticalLayout_5);

    horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_24->addItem(horizontalSpacer_9);


    verticalLayout_7->addLayout(horizontalLayout_24);

    verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    verticalLayout_7->addItem(verticalSpacer_2);



    gridLayout_2->addLayout(verticalLayout_7, 0, 0, 1, 1);

    tabWidget->addTab(tab_2, QString());

    gridLayout_3->addWidget(tabWidget, 0, 0, 1, 1);

    setCentralWidget(centralWidget);

    translate();
    updateContent();

    setWindowIcon(QPixmap(":/images/icon.png"));
    setFixedSize(450, 400);

    catchUserInputs=true;

    connect(comboBox, &QComboBox::currentIndexChanged, this, &Preferences::languageChanged);
    connect(comboBox_c1, &QComboBox::currentIndexChanged, this, &Preferences::categoryChanged);
    connect(combobox_dxf_materials,
            &QComboBox::currentIndexChanged,
            this,
            &Preferences::dxfCategoryChanged);
    connect(checkBox_compute_on_quit,
            &QAbstractButton::clicked,
            this,
            &Preferences::checkBox_compute_on_quitChanged);

    connect(checkBox, &QAbstractButton::clicked, this, &Preferences::checkBoxChanged);
    connect(checkBox_2, &QAbstractButton::clicked, this, &Preferences::checkBox_2Changed);
    connect(checkBox_3, &QAbstractButton::clicked, this, &Preferences::checkBox_3Changed);
    connect(checkBox_4, &QAbstractButton::clicked, this, &Preferences::checkBox_4Changed);
    connect(checkBox_5, &QAbstractButton::clicked, this, &Preferences::checkBox_5Changed);
    connect(checkBox_6, &QAbstractButton::clicked, this, &Preferences::checkBox_6Changed);
    connect(checkBox_7, &QAbstractButton::clicked, this, &Preferences::checkBox_7Changed);
    connect(checkBox_8, &QAbstractButton::clicked, this, &Preferences::checkBox_8Changed);
    connect(checkBox_9, &QAbstractButton::clicked, this, &Preferences::checkBox_9Changed);
    connect(checkBox_10, &QAbstractButton::clicked, this, &Preferences::checkBox_10Changed);
    connect(checkBox_11, &QAbstractButton::clicked, this, &Preferences::checkBox_11Changed);
    connect(checkBox_12, &QAbstractButton::clicked, this, &Preferences::checkBox_12Changed);

    connect(pushButton_2,
            &QAbstractButton::clicked,
            this,
            &Preferences::installDocxTemplateRequested);

    connect(listWidget, &QListWidget::customContextMenuRequested, this, &Preferences::customContextMenuRequested);
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void Preferences::checkBox_compute_on_quitChanged()
{
    if (checkBox_compute_on_quit->isChecked())
        LocalData::instance()->setParameter("compute-when-exiting", "true");
    else
        LocalData::instance()->setParameter("compute-when-exiting", "false");
}

Preferences::~Preferences()
{
}

void Preferences::checkBoxChanged()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-annotations-in-2d-view", v_s);
    StatesManager::instance()->includeAnnotationsIn2dView(v);
}

void Preferences::checkBox_2Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_2->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-images-in-2d-view", v_s);
    StatesManager::instance()->includeImagesIn2dView(v);
}

void Preferences::checkBox_3Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_3->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-dxf-in-2d-view", v_s);
    StatesManager::instance()->includeDxfIn2dView(v);
}

void Preferences::checkBox_4Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_4->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-dxf-in-1d-view", v_s);
    StatesManager::instance()->includeDxfIn1dView(v);
}

void Preferences::checkBox_5Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_5->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-images-in-1d-view", v_s);
    StatesManager::instance()->includeImagesIn1dView(v);
}

void Preferences::checkBox_6Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_6->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-annotations-in-1d-view", v_s);
    StatesManager::instance()->includeAnnotationsIn1dView(v);
}

void Preferences::checkBox_7Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_7->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-dxf-in-results-view", v_s);
    StatesManager::instance()->includeDxfInResultsView(v);
}

void Preferences::checkBox_8Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_8->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-images-in-results-view", v_s);
    StatesManager::instance()->includeImagesInResultsView(v);
}

void Preferences::checkBox_9Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_9->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-annotations-in-results-view", v_s);
    StatesManager::instance()->includeAnnotationsInResultsView(v);
}

void Preferences::checkBox_10Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_10->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-dxf-in-environments-view", v_s);
    StatesManager::instance()->includeDxfInEnvsView(v);
}

void Preferences::checkBox_11Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_11->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-images-in-environments-view", v_s);
    StatesManager::instance()->includeImagesInEnvsView(v);
}

void Preferences::checkBox_12Changed()
{
    if (!catchUserInputs)
        return;

    bool v=checkBox_12->isChecked();
    std::string v_s= v ? "true" : "false";

    LocalData::instance()->setParameter("include-annotations-in-environments-view", v_s);
    StatesManager::instance()->includeAnnotationsInEnvsView(v);
}

void Preferences::languageChanged(int idx)
{
    if (!catchUserInputs)
        return;

    LOG_INFO("Change language.");

    if (comboBox->currentIndex()==0)
    {
        LinguistManager::instance()->changeLanguage(LinguistManager::French);
        LocalData::instance()->setParameter("language", "french");
    }
    else
    {
        LinguistManager::instance()->changeLanguage(LinguistManager::English);
        LocalData::instance()->setParameter("language", "english");
    }

    updateContent();
}

void Preferences::categoryChanged(int idx)
{
    LOG_INFO("Import category changed.");

    if (!catchUserInputs)
        return;
    QString data=comboBox_c1->itemData(idx).toString();
    LocalData::instance()->setParameter("material-import-category", data.toStdString());
}

void Preferences::dxfCategoryChanged(int idx)
{
	LOG_INFO("DXF materials category changed.");

	if (!catchUserInputs)
		return;

	QString data = combobox_dxf_materials->itemData(idx).toString();
	LocalData::instance()->setParameter("material-dxf-category", data.toStdString());
}

void Preferences::translate()
{
    catchUserInputs=false;
    setWindowTitle(_tr("Préférences"));
    label->setText(_tr("Langue de l'interface :"));
    label_compute_on_quit->setText(_tr("Calcul de la modélisation :"));
    checkBox_compute_on_quit->setText(_tr("proposer lors de la fermeture"));
    tabWidget->setTabText(tabWidget->indexOf(tab), _tr("Général"));
    label_5->setText(_tr("Modèles de rapport DocX :"));
    pushButton_2->setText(_tr("Ajouter"));

    label_6->setText(_tr("Contenu des images exportées :"));
    label_7->setText(_tr("Modélisation avec pont thermique :"));
    label_11->setText(_tr("Modélisation sans pont thermique :"));
    label_12->setText(_tr("Champ de températures :"));
    label_13->setText(_tr("Ambiances thermiques :"));
    label_8->setText(_tr("Annotations"));
    label_9->setText(_tr("Images"));
    label_10->setText(_tr("Dxf"));
	label_dxf_materials->setText(_tr("Matériaux DXF :"));

    tabWidget->setTabText(tabWidget->indexOf(tab_2), _tr("Note de calcul"));
    label_c1->setText(_tr("Importer les matériaux dans :"));

    comboBox->clear();
    comboBox->addItem(_tr("Français"), LinguistManager::French);
    comboBox->addItem(_tr("Anglais"), LinguistManager::English);

    catchUserInputs=true;
}

void Preferences::updateCategoriesList()
{
    catchUserInputs=false;

    comboBox_c1->clear();
    comboBox_c1->addItem("", "");

	combobox_dxf_materials->clear();
	combobox_dxf_materials->addItem("", "");

    MainWindow *window=dynamic_cast<MainWindow*>(parent());
    if (window)
    {
        std::vector<MaterialCategory *> categories=window->categories();
        for (unsigned int i=0 ; i<categories.size() ; i++)
        {
            MaterialCategory *cat=categories.at(i);
            addCategory(cat, 0);
        }
    }

    std::string id=LocalData::instance()->getParameter("material-import-category");
    for (int i=0 ; i<comboBox_c1->count() ; i++)
    {
        if (comboBox_c1->itemData(i).toString().toStdString()==id)
        {
            comboBox_c1->setCurrentIndex(i);
            break;
        }
    }

	id = LocalData::instance()->getParameter("material-dxf-category");
	for (int i = 0; i < combobox_dxf_materials->count(); i++)
	{
		if (combobox_dxf_materials->itemData(i).toString().toStdString() == id)
		{
			combobox_dxf_materials->setCurrentIndex(i);
			break;
		}
	}

    catchUserInputs=true;
}

void Preferences::addCategory(MaterialCategory *category, int level)
{
    // Add parent.
    QString title;
    for (int i=0 ; i<level ; i++)
        title+="  ";
    title+=category->title();
	comboBox_c1->addItem(title, category->id());
	combobox_dxf_materials->addItem(title, category->id());

    // Add children.
    for (int i=0 ; i<category->childCount() ; i++)
    {
        MaterialCategory *cat=dynamic_cast<MaterialCategory*>(category->child(i));
        if (!cat)
            continue;

        addCategory(cat, level+1);
    }
}

void Preferences::updateContent()
{
    catchUserInputs=false;

    // Language.
    if (LinguistManager::instance()->currentLanguage()==LinguistManager::French)
        comboBox->setCurrentIndex(0);
    else
        comboBox->setCurrentIndex(1);

    // Compute when exiting.
    if (LocalData::instance()->getParameter("compute-when-exiting")=="false")
        checkBox_compute_on_quit->setChecked(false);
    else
        checkBox_compute_on_quit->setChecked(true);

    // Load DocX reports.
    updateDocxTemplateLists();

    // Images contents.
    checkBox->setChecked(StatesManager::instance()->includeAnnotationsIn2dView());
    checkBox_2->setChecked(StatesManager::instance()->includeImagesIn2dView());
    checkBox_3->setChecked(StatesManager::instance()->includeDxfIn2dView());

    checkBox_6->setChecked(StatesManager::instance()->includeAnnotationsIn1dView());
    checkBox_5->setChecked(StatesManager::instance()->includeImagesIn1dView());
    checkBox_4->setChecked(StatesManager::instance()->includeDxfIn1dView());

    checkBox_9->setChecked(StatesManager::instance()->includeAnnotationsInResultsView());
    checkBox_8->setChecked(StatesManager::instance()->includeImagesInResultsView());
    checkBox_7->setChecked(StatesManager::instance()->includeDxfInResultsView());

    checkBox_12->setChecked(StatesManager::instance()->includeAnnotationsInEnvsView());
    checkBox_11->setChecked(StatesManager::instance()->includeImagesInEnvsView());
    checkBox_10->setChecked(StatesManager::instance()->includeDxfInEnvsView());

    catchUserInputs=true;
}

void Preferences::updateDocxTemplateLists()
{
    listWidget->clear();

    std::string docx=LocalData::instance()->getParameter("docx-templates");
    std::vector<std::string> files=ToolBox::split(docx, '|');

    for (unsigned int i=0 ; i<files.size() ; i++)
    {
        QString file_name=files.at(i).c_str();
#ifdef WIN32
        file_name.replace("/", "\\");
#endif

        if (!QFile(file_name).exists())
            continue;

        listWidget->addItem(file_name);
    }
}

void Preferences::installDocxTemplateRequested()
{
    QString path=LocalData::instance()->getParameter("last-docx-path").c_str();

    QString filename=QFileDialog::getOpenFileName(this, _tr("Ajouter un modèle de rapport DocX"), path, _tr("document DocX (*.docx)") );
    if (filename.isEmpty())
        return;

    LOG_INFO("Install DocX model: " << filename.toStdString());

    QFileInfo info(filename);
    LocalData::instance()->setParameter("last-docx-path", info.absolutePath().toStdString());

    std::string docx=LocalData::instance()->getParameter("docx-templates");
    std::vector<std::string> files=ToolBox::split(docx, '|');

    // Check if file is already stored.
    for (unsigned int i=0 ; i<files.size() ; i++)
    {
        QString file_name=files.at(i).c_str();
        if (file_name==filename)
            return;
    }

    files.push_back(filename.toStdString());
    docx=ToolBox::merge(files, '|');
    LocalData::instance()->setParameter("docx-templates", docx);

    updateDocxTemplateLists();
}

void Preferences::customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *item=listWidget->itemAt(pos);
    if (!item)
        return;

    QString file=item->text();
    if (file.isEmpty())
        return;

    QMenu menu(this);
    QPoint globalPos=listWidget->mapToGlobal(pos);
    QAction *modify=menu.addAction(_tr("Modifier"));
    QAction *remove=menu.addAction(_tr("Supprimer"));
    QAction *choice=menu.exec(globalPos);

    if (choice==modify)
    {
        LOG_INFO("Modify DocX template: " << file.toStdString());
        QDesktopServices::openUrl(QUrl("file:///"+file));
    }

    else if (choice==remove)
    {
        std::string docx=LocalData::instance()->getParameter("docx-templates");
        std::vector<std::string> files=ToolBox::split(docx, '|');
        std::string out;

        LOG_INFO("Remove DocX template: " << file.toStdString());

        for (unsigned int i=0 ; i<files.size() ; i++)
        {
            QString file_name=files.at(i).c_str();
            file_name.replace("/", "\\");
            if (file_name!=file)
            {
                if (!out.empty())
                    out+="|";
                out+=file_name.toStdString();
            }
        }
        LocalData::instance()->setParameter("docx-templates", out);
        updateDocxTemplateLists();
    }
}
