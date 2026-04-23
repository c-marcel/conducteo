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

#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#include <boundary-conditions/BoundaryConditionList.h>
#include <computation/ComputationReporter.h>
#include <computation/ValidationsThread.h>
#include <environments/EnvironmentList.h>
#include <computation/PsiReporter.h>
#include <materials/MaterialList.h>
#include <computation/PsiReporter.h>
#include <TemperatureDisplayer.h>
#include <HumidityDisplayer.h>
#include <SurfaceProperties.h>
#include <MouseCursorInfos.h>
#include <VolumeProperties.h>
#include <HorizontalScale.h>
#include <StatesObserver.h>
#include <reports/Report.h>
#include <VerticalScale.h>
#include <Translatable.h>
#include <ModelViewer.h>
#include <ButtonTabs.h>
#include <QMainWindow>
#include <QGridLayout>
#include <MainMenu.h>
#include <QShortcut>

class ErrorHandler;

class MainWindow: public QMainWindow,
                  public Translatable,
                  public StatesObserver
{

    Q_OBJECT

public:

    MainWindow(QWidget *parent=0);
    virtual ~MainWindow();

    void loadModel(const std::string &filename);

    void translate();
    void upgradeStateChanged();

    std::vector<MaterialCategory *> categories() const;

public slots:

    void newRequested();
    void openRequested();
    void saveRequested();
    void saveAsRequested();
    void aboutRequested();
    bool exitRequested();

    void openExample(const QString &filename);
    void openProject(const QString &filename);

    void fitViewRequested();
    void zoomInRequested();
    void zoomOutRequested();

protected:

    void closeEvent(QCloseEvent * event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:

    void validateSoftware();
    void showQuickStart();

private:

    enum DropFileType
    {
        UnknownType,
        ModelType,
        MaterialDbType,
        DxfFileType,
        ImageType,
        ImportModelType
    };

    void updateWindowTitle();
    bool userAcceptToStopSimulation();
    bool userAcceptToCloseCurrentModel();
    bool userWantToRecalculateModel();

    void displayErrorMessage(const QString &title, const QString &message, ErrorHandler *error);

	void importDxfContent(const std::string &filename);
	QString escapeString(const QString &text) const;

    DropFileType acceptDrop(QDropEvent *event) const;

    // Menus & toolbars.
    MainMenu                    _menu;

    // Layouts.
    QWidget                    *_centralWidget;
    QGridLayout                 _grid;
    QVBoxLayout                 _tabsLayout;
    QGridLayout                 _modelGrid;
    QVBoxLayout                 _listLayout;

    // Left column.
    MouseCursorInfos            _mouseCursor;
    TemperatureDisplayer        _temperatures;
    HumidityDisplayer           _humidity;
    SurfaceProperties           _surfaceProperties;
    EnvironmentList             _environmentsList;
    VolumeProperties            _volumeProperties;
    MaterialList                _materialsList;             //< Note: materials & bc lists must be constructed after all lists.
    BoundaryConditionList       _boundaryConditionsList;

    // Tabs.
    ButtonTabs                  _tabs;

    // Central view.
    HorizontalScale             _horizontalScale;
    VerticalScale               _verticalScale;
    ModelViewer                 _modelViewer;

    // Report.
    Report                      _report;

    // Computation.
    ComputationReporter         _computationReporter;
    QShortcut                   _validationsShortCut;
    ValidationsThread           _validationsThread;
    PsiReporter                 _psiReporter;

};

#endif
