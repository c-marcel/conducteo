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

#ifndef _COMPUTATION_THREAD_H
#define _COMPUTATION_THREAD_H

#include <errors/ErrorHandler.h>
#include <StatesManager.h>
#include <model/Model.h>
#include <QThread>

class ThreadProxy: public QObject
{

    Q_OBJECT

public:

    ThreadProxy();
    virtual ~ThreadProxy();

public slots:

    void simulationStateChanged(StatesManager::SimulationState state);

};

class ComputationThread: public QThread
{

    Q_OBJECT

public:

    ComputationThread(QObject *parent);
    virtual ~ComputationThread();

    void setData(Model::CallbackType type, int level, int total);

public slots:

    void abort();

signals:

    void exceptionCaught(ErrorHandler::ErrorCode code);
    void simulationStateChanged(StatesManager::SimulationState state);
    void simulationTypeChanged(Model::CallbackType type);
    void simulationLevelChanged(int level, int total);

protected:

    virtual void run();
    bool run_seh(Model *model, ErrorHandler *error);

};

#endif
