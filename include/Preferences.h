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

#ifndef _PREFERENCES_H
#define _PREFERENCES_H

#include <QtCore/QVariant>
#include <QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <StatesObserver.h>
#include <Translatable.h>
#include <QMainWindow>

class MaterialCategory;

class Preferences: public QMainWindow,
                   public Translatable,
                   public StatesObserver
{
    Q_OBJECT
public:
    Preferences(QWidget *parent=0);
    virtual ~Preferences();

    void translate();
    void updateContent();

    void updateDocxTemplateLists();
    void updateCategoriesList();

private:
    void addCategory(MaterialCategory *category, int level);

private slots:
    void languageChanged(int idx);
	void categoryChanged(int idx);
	void dxfCategoryChanged(int idx);
    void checkBox_compute_on_quitChanged();
    void checkBoxChanged();
    void checkBox_2Changed();
    void checkBox_3Changed();
    void checkBox_4Changed();
    void checkBox_5Changed();
    void checkBox_6Changed();
    void checkBox_7Changed();
    void checkBox_8Changed();
    void checkBox_9Changed();
    void checkBox_10Changed();
    void checkBox_11Changed();
    void checkBox_12Changed();

    void installDocxTemplateRequested();
    void customContextMenuRequested(const QPoint &pos);

private:
    QWidget *centralWidget;
    QGridLayout *gridLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;

    QHBoxLayout *horizontalLayout_compute_on_quit;
    QLabel *label_compute_on_quit;
    QCheckBox *checkBox_compute_on_quit;

	QHBoxLayout *hLayout_dxf_materials;
	QLabel *label_dxf_materials;
	QComboBox *combobox_dxf_materials;
	QSpacerItem *hSpace_dxf_materials;

    QHBoxLayout *horizontalLayout_c1;
    QLabel *label_c1;
    QComboBox *comboBox_c1;
    QSpacerItem *horizontalSpacer_c1;
    QSpacerItem *verticalSpacer_3;
    QWidget *tab_2;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_6;
    QListWidget *listWidget;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_2;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer;

    QLabel *label_6;
    QHBoxLayout *horizontalLayout_24;
    QVBoxLayout *verticalLayout_6;
    QSpacerItem *horizontalSpacer_8;
    QHBoxLayout *horizontalLayout_20;
    QSpacerItem *horizontalSpacer_7;
    QLabel *label_7;
    QHBoxLayout *horizontalLayout_21;
    QSpacerItem *horizontalSpacer_11;
    QLabel *label_11;
    QHBoxLayout *horizontalLayout_22;
    QSpacerItem *horizontalSpacer_12;
    QLabel *label_12;
    QHBoxLayout *horizontalLayout_23;
    QSpacerItem *horizontalSpacer_13;
    QLabel *label_13;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_9;
    QSpacerItem *horizontalSpacer_26;
    QLabel *label_8;
    QSpacerItem *horizontalSpacer_27;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_18;
    QCheckBox *checkBox;
    QSpacerItem *horizontalSpacer_19;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_20;
    QCheckBox *checkBox_6;
    QSpacerItem *horizontalSpacer_23;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_21;
    QCheckBox *checkBox_9;
    QSpacerItem *horizontalSpacer_24;
    QHBoxLayout *horizontalLayout_8;
    QSpacerItem *horizontalSpacer_22;
    QCheckBox *checkBox_12;
    QSpacerItem *horizontalSpacer_25;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer_28;
    QLabel *label_9;
    QSpacerItem *horizontalSpacer_29;
    QHBoxLayout *horizontalLayout_11;
    QSpacerItem *horizontalSpacer_30;
    QCheckBox *checkBox_2;
    QSpacerItem *horizontalSpacer_31;
    QHBoxLayout *horizontalLayout_12;
    QSpacerItem *horizontalSpacer_32;
    QCheckBox *checkBox_5;
    QSpacerItem *horizontalSpacer_33;
    QHBoxLayout *horizontalLayout_13;
    QSpacerItem *horizontalSpacer_34;
    QCheckBox *checkBox_8;
    QSpacerItem *horizontalSpacer_35;
    QHBoxLayout *horizontalLayout_14;
    QSpacerItem *horizontalSpacer_36;
    QCheckBox *checkBox_11;
    QSpacerItem *horizontalSpacer_37;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_15;
    QSpacerItem *horizontalSpacer_38;
    QLabel *label_10;
    QSpacerItem *horizontalSpacer_39;
    QHBoxLayout *horizontalLayout_16;
    QSpacerItem *horizontalSpacer_40;
    QCheckBox *checkBox_3;
    QSpacerItem *horizontalSpacer_41;
    QHBoxLayout *horizontalLayout_17;
    QSpacerItem *horizontalSpacer_42;
    QCheckBox *checkBox_4;
    QSpacerItem *horizontalSpacer_43;
    QHBoxLayout *horizontalLayout_18;
    QSpacerItem *horizontalSpacer_44;
    QCheckBox *checkBox_7;
    QSpacerItem *horizontalSpacer_45;
    QHBoxLayout *horizontalLayout_19;
    QSpacerItem *horizontalSpacer_46;
    QCheckBox *checkBox_10;
    QSpacerItem *horizontalSpacer_47;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *verticalSpacer_2;
    bool catchUserInputs;
};

#endif
