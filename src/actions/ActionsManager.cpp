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

#include <actions/ActionsManager.h>
#include <actions/UserAction.h>

ActionsManager::ActionsManager():
    _savedModel(true)
{
}

ActionsManager::~ActionsManager()
{
    clearRedoActions();
    clearUndoActions();
}

void ActionsManager::exec(UserAction *action)
{
    // Add action to undo list.
    _undo.push_back(action);

    // Execute action.
    action->exec();

    // Model alteration.
    if (action->alterModel())
    {
        setSavedModel(false);
        modelAltered();
    }

    if (action->alterResults())
        modelResultsAltered();

    // Clear redo list.
    clearRedoActions();

    changed();
}

void ActionsManager::removeAction(UserAction *action)
{
    for (unsigned int i=0 ; i<_redo.size() ; i++)
    {
        if (_redo.at(i)==action)
        {
            _redo.erase(_redo.begin()+i);
            break;
        }
    }

    for (unsigned int i=0 ; i<_undo.size() ; i++)
    {
        if (_undo.at(i)==action)
        {
            _undo.erase(_undo.begin()+i);
            break;
        }
    }

    delete action;

    changed();
}

void ActionsManager::undo()
{
    if (_undo.empty())
        return;

    // Get action to be un-done.
    UserAction *action=_undo.at(_undo.size()-1);

    // Remove action from undo list.
    _undo.resize(_undo.size()-1);

    // Insert action into redo list (at first position).
    _redo.insert(_redo.begin(), action);

    // Undo action.
    action->undo();

    // Model alteration.
    if (action->alterModel())
    {
        setSavedModel(false);
        modelAltered();
    }

    if (action->alterResults())
        modelResultsAltered();

    changed();
}

void ActionsManager::redo()
{
    if (_redo.empty())
        return;

    // Get action to be done.
    UserAction *action=_redo.at(0);

    // Remove action from redo list.
    _redo.erase(_redo.begin());

    // Insert action into undo list (at last position).
    _undo.push_back(action);

    // Do action.
    action->exec();

    // Model alteration.
    if (action->alterModel())
    {
        setSavedModel(false);
        modelAltered();
    }

    if (action->alterResults())
        modelResultsAltered();

    changed();
}

void ActionsManager::clearRedoActions()
{
    for (unsigned int i=0 ; i<_redo.size() ; i++)
        delete _redo.at(i);
    _redo.resize(0);

    changed();
}

void ActionsManager::clearUndoActions()
{
    for (unsigned int i=0 ; i<_undo.size() ; i++)
        delete _undo.at(i);
    _undo.resize(0);

    changed();
}

void ActionsManager::clear()
{
    clearRedoActions();
    clearUndoActions();
}

bool ActionsManager::canUndo() const
{
    return !_undo.empty();
}

bool ActionsManager::canRedo() const
{
    return !_redo.empty();
}

void ActionsManager::setSavedModel(bool saved)
{
    _savedModel=saved;
    changed();
}

bool ActionsManager::saved() const
{
    return _savedModel;
}
