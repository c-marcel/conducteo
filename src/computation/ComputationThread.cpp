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

#include <computation/ComputationThread.h>

ThreadProxy g_thread_proxy;

ThreadProxy::ThreadProxy()
{
}

ThreadProxy::~ThreadProxy()
{
}

void ThreadProxy::simulationStateChanged(StatesManager::SimulationState state)
{
    StatesManager::instance()->setSimulationState(state);
}

void api_callback(Model::CallbackType type, int level, int total, void *pthis)
{
    ComputationThread *thread=(ComputationThread*) pthis;
    thread->setData(type, level, total);
}

void ComputationThread::setData(Model::CallbackType type, int level, int total)
{
    emit simulationTypeChanged(type);
    emit simulationLevelChanged(level, total);
}

ComputationThread::ComputationThread(QObject *parent):
    QThread(parent)
{
    moveToThread(this);
    connect(this,
            &ComputationThread::simulationStateChanged,
            &g_thread_proxy,
            &ThreadProxy::simulationStateChanged,
            Qt::BlockingQueuedConnection);
}

ComputationThread::~ComputationThread()
{
}

void ComputationThread::abort()
{
    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    model->abort();
    StatesManager::instance()->setSimulationState(StatesManager::Idle);
}

void ComputationThread::run()
{
    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    emit simulationStateChanged(StatesManager::InProgress);
    ErrorHandler *error = new ErrorHandler;

    model->setCallback(api_callback, this);
    model->setCurrentErrorHandler(error);
    model->setRunning(true);

    if (!run_seh(model, error))
    {
        delete error;
        return;
    }

    model->setRunning(false);
    model->setCurrentErrorHandler(0);

    // Model must be deleted.
    if (model->deleteObject())
        delete model;
    else
    {
        if (error->code() == ErrorHandler::AbortByUser)
            emit simulationStateChanged(StatesManager::Idle);
        else
            emit simulationStateChanged(StatesManager::Done);
    }

    delete error;
}

bool ComputationThread::run_seh(Model *model, ErrorHandler *error)
{
#ifdef WIN32
    __try
#else
    try
#endif
    {
        model->compute(error, true);
    }
#ifdef WIN32
    __except(1)
#else
    catch(...)
#endif
    {
        model->setRunning(false);
        model->setCurrentErrorHandler(0);

        // Model must be deleted.
        if (model->deleteObject())
            delete model;

        else
        {
            // Exception caught.
            emit simulationStateChanged(StatesManager::Idle);
            emit exceptionCaught(error->code());
        }

        delete error;

        return false;
    }

    return true;
}
