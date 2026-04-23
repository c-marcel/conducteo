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

#ifndef _COMPUTATION_REPORTER_H
#define _COMPUTATION_REPORTER_H

#include <StatesObserver.h>
#include <Translatable.h>
#include <model/Model.h>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

class ComputationReporter: public QWidget,
                           public Translatable,
                           public StatesObserver
{
    Q_OBJECT
public:
    ComputationReporter(QWidget *parent=0);
    virtual ~ComputationReporter();

    void translate();
    void statesChanged();
    void applyTheme();

public slots:
    void setLevel(int level, int total);
    void setRefinementType(Model::CallbackType type);

protected:
    virtual void paintEvent(QPaintEvent *event);

private slots:
    void onLinkActivated(const QString &link);

signals:
    void computationAbortRequested();

private:
    QVBoxLayout         _layout;
    QHBoxLayout         _levelLayout;
    QLabel              _refinementTypeTitle;
    QLabel              _levelTitle;
    QLabel              _levelValue;
    QProgressBar        _progress;
    QLabel              _cancel;
    Model::CallbackType _type;
};

#endif
