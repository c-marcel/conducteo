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

#ifndef _MAIN_MENU_H
#define _MAIN_MENU_H

#include <computation/ComputationThread.h>
#include <StatesObserver.h>
#include <Translatable.h>
#include <Preferences.h>
#include <QShortcut>
#include <QMenuBar>
#include <QToolBar>

class ComputationReporter;

class MainMenu: public QMenuBar,
                public Translatable,
                public StatesObserver
{

    Q_OBJECT

public:

    MainMenu(QWidget *parent=0);
    virtual ~MainMenu();

    void translate();
    void applyTheme();
    void statesChanged();
    void upgradeStateChanged();
    void zoomChanged();

    void addLastProject(QString filename);
    void connectReporter(ComputationReporter *reporter);

    void addImage(const QString &filename);
    void addDxf(const QString &filename);

    void updateLicense();

signals:

    void newRequested();
    void openRequested();
    void saveRequested();
    void saveAsRequested();
    void aboutRequested();
    void exitRequested();
    
    void downloadOrInstallUpgrade();

    void fitViewRequested();
    void zoomInRequested();
    void zoomOutRequested();

    void openExample(const QString &filename);
    void openProject(const QString &filename);

public slots:

    void selectionModeRequested();
    void addRectangleModeRequested();
    void addPolylineModeRequested();
    void addCircleModeRequested();
    void addEllipseModeRequested();

    void getMaterialModeRequested();
    void setPhysicsModeRequested();
    void setEnvironmentsModeRequested();
    void addPointModeRequested();

    void selectAllRequested();
    void deleteRequested();
    void rotationRequested(double, double);

    void computationRequested();
    void abortComputation();

    void undo();
    void redo();

    void cutRequested();
    void copyRequested();
    void pasteRequested();

    void onAddImageRequested();
    void onAddDxfRequested();
    void onToogleImageVisibility();
    void onToogleDxfVisibility();

    void frontSelectedRequested();
    void backSelectedRequested();
    void frontOneShotSelectedRequested();
    void backOneShotSelectedRequested();

    void getLengthRequested();
    void getAngleRequested();

    void helpRequested();
    void tutorialRequested();
    void quickStartRequested();
    void validationsReportRequested();

    void onOpenExample();
    void onOpenProject();

    void copy2DModelTo1DModel();
    void exceptionCaught(ErrorHandler::ErrorCode code);
    void showHideImagesAndDxfRequested();
    void toogleEn13370();

    void openPreferencesRequested();

    void addTextRequested();
    void addArrowRequested();

    void setThermalBridgeType();
    void setThermalFluxType();
    void setThermalTransmissionType();
    void setThermalConductivityType();
    void setCondensationType();

    void setLengthFull();
    void setLengthX();
    void setLengthY();
    void setLengthCustom();

    void setCustomWidth();
    void setCustomThickness();
    void setCustomResistances();

    void clearAllBoundaryConditions();
    void clearAllEnvironments();

private:
    void updateSubMenus();
    void installOnWindow();
    void updateUpgradeAction();
    void updateImagesVisibility();

    void updateFrontBack();
    void updateCustomLength();
    void updateConductivityData();
    QIcon getProgressIcon(int percent) const;

    void updatePasteIcon();
    void deleteSelectedVolumes();
    QString serializeSelection() const;
    void importVolumesFromXml(const std::string &xml);

    // Last projects & examples.
    void loadLastProjects();
    void loadExamples();
    void addExample(QMenu *menu, const QString& title, const QString& filename);
    QMenu *addMenu(QMenu *menu, const QString& title);

    // Main menus.
    QToolBar   _menuToolBar;
    QMenu      _fileMenu;
    QMenu      _editMenu;
    QMenu      _viewMenu;
    QMenu      _projectMenu;
    QMenu      _helpMenu;
    QMenu      _lastProjectsMenu;
    QMenu      _examplesMenu;
    QMenu      _documentationMenu;
    QMenu      _modelTypeMenu;
    QMenu      _lengthTypeMenu;
    QMenu      _equivalentConductivityMenu;

    // Main menus actions.
    QAction    _newDocumentAction;
    QAction    _openDocumentAction;
    QAction    _openLastAction;
    QAction    _openExampleAction;
    QAction    _saveDocumentAction;
    QAction    _saveDocumentAsAction;
    QAction    _cutAction;
    QAction    _copyAction;
    QAction    _pasteAction;
    QAction    _undoAction;
    QAction    _redoAction;
    QAction    _aboutAction;
    QAction    _quitAction;
    QAction    _selectAll;
    QAction    _delete;
    QAction    _help;
    QAction    _tutorial;
    QAction    _quickstart;
    QAction    _preferencesAction;

    QAction    _computeAction;
    QAction    _copy2DModelTo1DModel;
    QAction    _toogleEn13370;
    QAction    _modelTypeAction;
    QAction    _modelTypeThermalBridgeAction;
    QAction    _modelTypeThermalFluxAction;
    QAction    _modelTypeThermalTransmissionAction;
    QAction    _modelTypeThermalConductivityAction;
    QAction    _modelTypeGlaserCondensationAction;

    QAction     _modelClearAllBc;
    QAction     _modelClearAllEnv;

    QAction    _modelLengthAction;
    QAction    _modelLengthFullAction;
    QAction    _modelLengthXAction;
    QAction    _modelLengthYAction;
    QAction    _modelLengthCustomAction;

    QAction    _modelEqConductivityAction;
    QAction    _modelConductivityWidth;
    QAction    _modelConductivityThickness;
    QAction    _modelConductivityResistance;
    
    QAction    _documentationAction;
    QAction    _validationsReport;

    QAction     _addText;
    QAction     _addArrow;

    // Model toolbar.
    QToolBar   _modelToolBar;
    QAction    _selectionModeAction;
    QAction    _addRectangleModeAction;
    QAction    _getMaterialModeAction;
    QAction    _setPhysicModeAction;
    QAction    _setEnvironmentModeAction;

    QAction    _addPolyline;
    QAction    _addCircleModeAction;
    QAction    _addEllipseModeAction;
    QAction    _addImageAction;
    QAction    _addDxfAction;
    QShortcut  _showHideImagesAndDxf;
    QAction    _addPoint;
    QAction    _getLength;
    QAction    _getAngle;

    // Zoom toolbar.
    QToolBar   _zoomToolBar;
    QAction    _fitAction;
    QAction    _zoomInAction;
    QAction    _zoomOutAction;

    // Images toolbar.
    QToolBar   _imagesToolBar;
    QAction    _imagesVisibilityAction;
    QAction    _dxfVisibilityAction;

    // Selection toolbar.
    QToolBar   _stackToolBar;
    QAction    _frontAction;
    QAction    _backAction;
    QAction    _frontOneShotAction;
    QAction    _backOneShotAction;

    // Upgrade toolbar.
    QToolBar   _upgradeToolbar;
    QAction    _upgradeDownloadAction;

    // Computation.
    ComputationThread  *_computationThread;

    Preferences         _preferences;

    QMap<QAction*, QString> _exampleTitles;
    QMap<QMenu*, QString>   _menuTitles;
};

#endif
