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

#include <MainMenu.h>
#include <actions/boundary-conditions/UnsetAllBoundaryConditions.h>
#include <actions/environments/UnsetAllEnvironments.h>
#include <actions/model/ImportVolumesFromXml.h>
#include <actions/model/Copy2dModelTo1dModel.h>
#include <computation/ComputationReporter.h>
#include <actions/model/SendToFrontVolume.h>
#include <actions/model/RemoveAnnotation.h>
#include <actions/model/SendToBackVolume.h>
#include <actions/model/SendToFrontImage.h>
#include <actions/model/SendToBackImage.h>
#include <actions/model/BringDownVolume.h>
#include <actions/model/BringDownImage.h>
#include <actions/model/BringUpVolume.h>
#include <actions/model/DeleteVolumes.h>
#include <actions/model/RotateVolumes.h>
#include <actions/model/BringUpImage.h>
#include <actions/model/DeleteImage.h>
#include <actions/model/DeleteDxf.h>
#include <actions/ActionsManager.h>
#include <actions/model/AddImage.h>
#include <actions/model/AddDxf.h>
#include <images/ImageDataQt.h>
#include <LinguistManager.h>
#include <QDesktopServices>
#include <StatesManager.h>
#include <tools/ToolBox.h>
#include <tools/UiTools.h>
#include <ModelViewer.h>
#include <model/Model.h>
#include <model/Image.h>
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QMainWindow>
#include <LocalData.h>
#include <QClipboard>
#include <QMimeData>
#include <QPainter>
#include <Zoom.h>
#include <QImage>
#include <QIcon>
#include <QFile>
#include <Log.h>
#include <QDir>
#include <QUrl>

MainMenu::MainMenu(QWidget *parent):
    QMenuBar(parent),
    _newDocumentAction(this),
    _openDocumentAction(this),
    _openLastAction(this),
    _openExampleAction(this),
    _saveDocumentAction(this),
    _saveDocumentAsAction(this),
    _cutAction(this),
    _copyAction(this),
    _pasteAction(this),
    _undoAction(this),
    _redoAction(this),
    _aboutAction(this),
    _quitAction(this),
    _selectAll(this),
    _preferencesAction(this),
    _delete(this),
    _help(this),
    _tutorial(this),
    _quickstart(this),
    _documentationAction(this),
    _validationsReport(this),
    _selectionModeAction(this),
    _addRectangleModeAction(this),
    _getMaterialModeAction(this),
    _setPhysicModeAction(this),
    _setEnvironmentModeAction(this),
    _computeAction(this),
    _fitAction(this),
    _zoomInAction(this),
    _zoomOutAction(this),
    _copy2DModelTo1DModel(this),
    _toogleEn13370(this),
    _modelTypeThermalBridgeAction(this),
    _modelTypeThermalFluxAction(this),
    _modelTypeThermalTransmissionAction(this),
    _modelTypeThermalConductivityAction(this),
    _modelTypeGlaserCondensationAction(this),
    _modelTypeAction(this),
    _modelLengthAction(this),
    _modelEqConductivityAction(this),
    _modelLengthFullAction(this),
    _modelLengthXAction(this),
    _modelLengthYAction(this),
    _modelLengthCustomAction(this),
    _modelConductivityWidth(this),
    _modelConductivityThickness(this),
    _modelConductivityResistance(this),
    _frontAction(this),
    _backAction(this),
    _frontOneShotAction(this),
    _backOneShotAction(this),
    _upgradeDownloadAction(this),
    _computationThread(new ComputationThread(this)),
    _showHideImagesAndDxf(Qt::Key_F5, this),
    _dxfVisibilityAction(this),
    _addDxfAction(this),
    _addImageAction(this),
    _imagesVisibilityAction(this),
    _addPoint(this),
    _getLength(this),
    _getAngle(this),
    _addText(this),
    _addArrow(this),
    _addPolyline(this),
    _addCircleModeAction(this),
    _addEllipseModeAction(this),
    _preferences(parent),
    _modelClearAllBc(this),
    _modelClearAllEnv(this)
{
    addAction(_fileMenu.menuAction());
    addAction(_editMenu.menuAction());
    addAction(_viewMenu.menuAction());
    addAction(_projectMenu.menuAction());
    addAction(_helpMenu.menuAction());

    // Shortcuts.
    _newDocumentAction.setShortcuts(QKeySequence::New);
    _openDocumentAction.setShortcuts(QKeySequence::Open);
    _saveDocumentAction.setShortcuts(QKeySequence::Save);
    _saveDocumentAsAction.setShortcuts(QKeySequence::SaveAs);
    _cutAction.setShortcuts(QKeySequence::Cut);
    _copyAction.setShortcuts(QKeySequence::Copy);
    _pasteAction.setShortcuts(QKeySequence::Paste);
    _undoAction.setShortcuts(QKeySequence::Undo);
    _redoAction.setShortcuts(QKeySequence::Redo);
    _quitAction.setShortcuts(QKeySequence::Quit);
    _selectAll.setShortcuts(QKeySequence::SelectAll);
    _delete.setShortcuts(QKeySequence::Delete);
    _zoomInAction.setShortcut(QKeySequence::ZoomIn);
    _zoomOutAction.setShortcut(QKeySequence::ZoomOut);

    _selectionModeAction.setShortcut(Qt::Key_F1);
    _addRectangleModeAction.setShortcut(Qt::Key_F2);
    _addPolyline.setShortcut(Qt::Key_F3);
    _setPhysicModeAction.setShortcut(Qt::Key_F4);

    // Checkable.
    _selectionModeAction.setCheckable(true);
    _addRectangleModeAction.setCheckable(true);

    _addPolyline.setCheckable(true);
    _addPoint.setCheckable(true);
    _addCircleModeAction.setCheckable(true);
    _addEllipseModeAction.setCheckable(true);

    _getMaterialModeAction.setCheckable(true);
    _setPhysicModeAction.setCheckable(true);
    _setEnvironmentModeAction.setCheckable(true);
    _toogleEn13370.setCheckable(true);

    _getLength.setCheckable(true);
    _getAngle.setCheckable(true);
    _addText.setCheckable(true);
    _addArrow.setCheckable(true);

    _modelTypeThermalBridgeAction.setCheckable(true);
    _modelTypeThermalFluxAction.setCheckable(true);
    _modelTypeThermalTransmissionAction.setCheckable(true);
    _modelTypeThermalConductivityAction.setCheckable(true);
    _modelTypeGlaserCondensationAction.setCheckable(true);

    _modelLengthFullAction.setCheckable(true);
    _modelLengthXAction.setCheckable(true);
    _modelLengthYAction.setCheckable(true);
    _modelLengthCustomAction.setCheckable(true);

    // Sub-menus.
    _openLastAction.setMenu(&_lastProjectsMenu);
    _openExampleAction.setMenu(&_examplesMenu);
    _documentationAction.setMenu(&_documentationMenu);

    // Fill menus.
    _fileMenu.addAction(&_newDocumentAction);
    _fileMenu.addAction(&_openDocumentAction);
    _fileMenu.addAction(&_saveDocumentAction);
    _fileMenu.addAction(&_saveDocumentAsAction);
    _fileMenu.addSeparator();
    _fileMenu.addAction(&_openLastAction);
    _fileMenu.addAction(&_openExampleAction);
    _fileMenu.addSeparator();
    _fileMenu.addAction(&_quitAction);

    _editMenu.addAction(&_undoAction);
    _editMenu.addAction(&_redoAction);
    _editMenu.addSeparator();
    _editMenu.addAction(&_cutAction);
    _editMenu.addAction(&_copyAction);
    _editMenu.addAction(&_pasteAction);
    _editMenu.addSeparator();
    _editMenu.addAction(&_selectAll);
    _editMenu.addAction(&_delete);
    _editMenu.addSeparator();
    _editMenu.addAction(&_preferencesAction);

    _lengthTypeMenu.addAction(&_modelLengthFullAction);
    _lengthTypeMenu.addAction(&_modelLengthXAction);
    _lengthTypeMenu.addAction(&_modelLengthYAction);
    _lengthTypeMenu.addAction(&_modelLengthCustomAction);

    _equivalentConductivityMenu.addAction(&_modelConductivityWidth);
    _equivalentConductivityMenu.addAction(&_modelConductivityThickness);
    _equivalentConductivityMenu.addAction(&_modelConductivityResistance);

    _modelLengthAction.setMenu(&_lengthTypeMenu);
    _modelEqConductivityAction.setMenu(&_equivalentConductivityMenu);

    _projectMenu.addSeparator();
    _projectMenu.addAction(&_copy2DModelTo1DModel);
    _projectMenu.addAction(&_toogleEn13370);
    _projectMenu.addAction(&_computeAction);

    _helpMenu.addAction(&_documentationAction);
    _helpMenu.addSeparator();
    _helpMenu.addAction(&_aboutAction);

    _documentationMenu.addAction(&_help);
    _documentationMenu.addAction(&_quickstart);
    _documentationMenu.addAction(&_tutorial);
    _documentationMenu.addAction(&_validationsReport);

    // Toolbars.
    _menuToolBar.setIconSize(QSize(32, 32));
    _menuToolBar.setMovable(false);
    _menuToolBar.addAction(&_newDocumentAction);
    _menuToolBar.addAction(&_openDocumentAction);
    _menuToolBar.addAction(&_saveDocumentAction);
    _menuToolBar.addAction(&_saveDocumentAsAction);
    _menuToolBar.addSeparator();
    _menuToolBar.addAction(&_cutAction);
    _menuToolBar.addAction(&_copyAction);
    _menuToolBar.addAction(&_pasteAction);
    _menuToolBar.addSeparator();
    _menuToolBar.addAction(&_computeAction);
    _menuToolBar.addAction(&_copy2DModelTo1DModel);
    _menuToolBar.addAction(&_toogleEn13370);
    _menuToolBar.addSeparator();
    _menuToolBar.addAction(&_undoAction);
    _menuToolBar.addAction(&_redoAction);
    _menuToolBar.addSeparator();
    _menuToolBar.addAction(&_aboutAction);

    _zoomToolBar.setIconSize(QSize(32, 32));
    _zoomToolBar.setMovable(false);
    _zoomToolBar.addAction(&_fitAction);
    _zoomToolBar.addAction(&_zoomInAction);
    _zoomToolBar.addAction(&_zoomOutAction);

    _stackToolBar.setIconSize(QSize(32, 32));
    _stackToolBar.setMovable(false);
    _stackToolBar.addAction(&_frontAction);
    _stackToolBar.addAction(&_backAction);
    _stackToolBar.addAction(&_frontOneShotAction);
    _stackToolBar.addAction(&_backOneShotAction);

    _upgradeToolbar.setIconSize(QSize(32, 32));
    _upgradeToolbar.setMovable(false);
    _upgradeToolbar.addAction(&_upgradeDownloadAction);
    _upgradeToolbar.hide();

    // Update dynamic contents.
    _preferences.updateDocxTemplateLists();

    // Install on parent window.
    installOnWindow();

    connect(&_newDocumentAction, &QAction::triggered, this, &MainMenu::newRequested);
    connect(&_openDocumentAction, &QAction::triggered, this, &MainMenu::openRequested);
    connect(&_saveDocumentAction, &QAction::triggered, this, &MainMenu::saveRequested);
    connect(&_saveDocumentAsAction, &QAction::triggered, this, &MainMenu::saveAsRequested);
    connect(&_aboutAction, &QAction::triggered, this, &MainMenu::aboutRequested);
    connect(&_quitAction, &QAction::triggered, this, &MainMenu::exitRequested);
    connect(&_help, &QAction::triggered, this, &MainMenu::helpRequested);
    connect(&_tutorial, &QAction::triggered, this, &MainMenu::tutorialRequested);
    connect(&_quickstart, &QAction::triggered, this, &MainMenu::quickStartRequested);

    connect(&_selectAll, &QAction::triggered, this, &MainMenu::selectAllRequested);
    connect(&_delete, &QAction::triggered, this, &MainMenu::deleteRequested);

    connect(&_preferencesAction, &QAction::triggered, this, &MainMenu::openPreferencesRequested);

    connect(&_cutAction, &QAction::triggered, this, &MainMenu::cutRequested);
    connect(&_copyAction, &QAction::triggered, this, &MainMenu::copyRequested);
    connect(&_pasteAction, &QAction::triggered, this, &MainMenu::pasteRequested);

    connect(&_validationsReport, &QAction::triggered, this, &MainMenu::validationsReportRequested);

    connect(&_addText, &QAction::triggered, this, &MainMenu::addTextRequested);
    connect(&_addArrow, &QAction::triggered, this, &MainMenu::addArrowRequested);

    connect(&_undoAction, &QAction::triggered, this, &MainMenu::undo);
    connect(&_redoAction, &QAction::triggered, this, &MainMenu::redo);

    connect(&_selectionModeAction, &QAction::triggered, this, &MainMenu::selectionModeRequested);
    connect(&_addRectangleModeAction,
            &QAction::triggered,
            this,
            &MainMenu::addRectangleModeRequested);

    connect(&_addPolyline, &QAction::triggered, this, &MainMenu::addPolylineModeRequested);
    connect(&_addPoint, &QAction::triggered, this, &MainMenu::addPointModeRequested);
    connect(&_addCircleModeAction, &QAction::triggered, this, &MainMenu::addCircleModeRequested);
    connect(&_addEllipseModeAction, &QAction::triggered, this, &MainMenu::addEllipseModeRequested);

    connect(&_getMaterialModeAction, &QAction::triggered, this, &MainMenu::getMaterialModeRequested);
    connect(&_setPhysicModeAction, &QAction::triggered, this, &MainMenu::setPhysicsModeRequested);
    connect(&_setEnvironmentModeAction,
            &QAction::triggered,
            this,
            &MainMenu::setEnvironmentsModeRequested);

    connect(&_getLength, &QAction::triggered, this, &MainMenu::getLengthRequested);
    connect(&_getAngle, &QAction::triggered, this, &MainMenu::getAngleRequested);

    connect(&_fitAction, &QAction::triggered, this, &MainMenu::fitViewRequested);
    connect(&_zoomInAction, &QAction::triggered, this, &MainMenu::zoomInRequested);
    connect(&_zoomOutAction, &QAction::triggered, this, &MainMenu::zoomOutRequested);

    connect(&_upgradeDownloadAction, &QAction::triggered, this, &MainMenu::downloadOrInstallUpgrade);

    connect(&_computeAction, &QAction::triggered, this, &MainMenu::computationRequested);
    connect(&_copy2DModelTo1DModel, &QAction::triggered, this, &MainMenu::copy2DModelTo1DModel);
    connect(&_toogleEn13370, &QAction::triggered, this, &MainMenu::toogleEn13370);

    connect(&_modelTypeThermalBridgeAction,
            &QAction::triggered,
            this,
            &MainMenu::setThermalBridgeType);
    connect(&_modelTypeThermalFluxAction, &QAction::triggered, this, &MainMenu::setThermalFluxType);
    connect(&_modelTypeThermalTransmissionAction,
            &QAction::triggered,
            this,
            &MainMenu::setThermalTransmissionType);
    connect(&_modelTypeThermalConductivityAction,
            &QAction::triggered,
            this,
            &MainMenu::setThermalConductivityType);
    connect(&_modelTypeGlaserCondensationAction,
            &QAction::triggered,
            this,
            &MainMenu::setCondensationType);

    connect(&_modelLengthFullAction, &QAction::triggered, this, &MainMenu::setLengthFull);
    connect(&_modelLengthXAction, &QAction::triggered, this, &MainMenu::setLengthX);
    connect(&_modelLengthYAction, &QAction::triggered, this, &MainMenu::setLengthY);
    connect(&_modelLengthCustomAction, &QAction::triggered, this, &MainMenu::setLengthCustom);

    connect(&_modelConductivityWidth, &QAction::triggered, this, &MainMenu::setCustomWidth);
    connect(&_modelConductivityThickness, &QAction::triggered, this, &MainMenu::setCustomThickness);
    connect(&_modelConductivityResistance,
            &QAction::triggered,
            this,
            &MainMenu::setCustomResistances);

    connect(&_addImageAction, &QAction::triggered, this, &MainMenu::onAddImageRequested);
    connect(&_addDxfAction, &QAction::triggered, this, &MainMenu::onAddDxfRequested);
    connect(&_imagesVisibilityAction, &QAction::triggered, this, &MainMenu::onToogleImageVisibility);
    connect(&_dxfVisibilityAction, &QAction::triggered, this, &MainMenu::onToogleDxfVisibility);

    connect(&_modelClearAllBc, &QAction::triggered, this, &MainMenu::clearAllBoundaryConditions);
    connect(&_modelClearAllEnv, &QAction::triggered, this, &MainMenu::clearAllEnvironments);

    connect(&_frontAction, &QAction::triggered, this, &MainMenu::frontSelectedRequested);
    connect(&_backAction, &QAction::triggered, this, &MainMenu::backSelectedRequested);

    connect(&_frontOneShotAction,
            &QAction::triggered,
            this,
            &MainMenu::frontOneShotSelectedRequested);
    connect(&_backOneShotAction, &QAction::triggered, this, &MainMenu::backOneShotSelectedRequested);

    connect(&_showHideImagesAndDxf,
            &QShortcut::activated,
            this,
            &MainMenu::showHideImagesAndDxfRequested);

    connect(_computationThread,
            &ComputationThread::exceptionCaught,
            this,
            &MainMenu::exceptionCaught,
            Qt::BlockingQueuedConnection);

    translate();
    applyTheme();
    statesChanged();

    updateSubMenus();

    loadExamples();
    loadLastProjects();
    _preferences.updateDocxTemplateLists();
}

MainMenu::~MainMenu()
{
    _computationThread->deleteLater();
}

void MainMenu::translate()
{
    _fileMenu.setTitle(_tr("MenuFile"));
    _editMenu.setTitle(_tr("MenuEdit"));
    _viewMenu.setTitle(_tr("MenuDisplay"));
    _projectMenu.setTitle(_tr("MenuProject"));
    _helpMenu.setTitle(_tr("MenuHelp"));

    _newDocumentAction.setText(_tr("MenuFileNew"));
    _openDocumentAction.setText(_tr("MenuFileOpen"));
    _openLastAction.setText(_tr("MenuFileRecents"));
    _openExampleAction.setText(_tr("MenuFileExamples"));
    _saveDocumentAction.setText(_tr("MenuFileSave"));
    _saveDocumentAsAction.setText(_tr("MenuFileSaveAs"));
    _quitAction.setText(_tr("MenuFileQuit"));

    _cutAction.setText(_tr("MenuEditCut"));
    _copyAction.setText(_tr("MenuEditCopy"));
    _pasteAction.setText(_tr("MenuEditPaste"));
    _undoAction.setText(_tr("MenuEditUndo"));
    _redoAction.setText(_tr("MenuEditRedo"));
    _selectAll.setText(_tr("MenuEditSelectAll"));
    _delete.setText(_tr("MenuEditDelete"));
    _preferencesAction.setText(_tr("MenuEditSettings"));

    _selectionModeAction.setText(_tr("ActionSelection"));
    _addRectangleModeAction.setText(_tr("ActionDrawRect"));

    _getMaterialModeAction.setText(_tr("ActionSelectMaterial"));
    _setPhysicModeAction.setText(_tr("ActionSetPhysic"));
    _setEnvironmentModeAction.setText(_tr("ActionSetEnv"));
    _fitAction.setText(_tr("ActionFitZoom"));
    _zoomInAction.setText(_tr("ActionZoomIn"));
    _zoomOutAction.setText(_tr("ActionZoomOut"));

    _computeAction.setText(_tr("ActionStartSimulation"));
    
    _help.setText(_tr("HelpManual"));
    _tutorial.setText(_tr("HelpTutorial"));
    _quickstart.setText(_tr("HelpQuickStart"));
    _aboutAction.setText(_tr("ActionAbout"));
    _documentationAction.setText(_tr("ActionDocumentation"));
    _validationsReport.setText(_tr("ActionValidationReport"));

    _copy2DModelTo1DModel.setText(_tr("DuplicateModel"));
    _toogleEn13370.setText(_tr("ToogleEN13370"));

    _modelTypeAction.setText(_tr("ComputationType"));
    _modelTypeThermalBridgeAction.setText(_tr("ThermBridgeCompType"));
    _modelTypeThermalFluxAction.setText(_tr("ThermFluxCompType"));
    _modelTypeThermalTransmissionAction.setText(_tr("ThermUCompType"));
    _modelTypeThermalConductivityAction.setText(_tr("EquivThermCondCompType"));
    _modelTypeGlaserCondensationAction.setText(_tr("CondGlaserCompType"));

    _modelLengthAction.setText(_tr("ModelLength"));
    _modelLengthFullAction.setText(_tr("TotalLength"));
    _modelLengthXAction.setText(_tr("LengthOnX"));
    _modelLengthYAction.setText(_tr("LengthOnY"));
    _modelEqConductivityAction.setText(_tr("EquivCond"));

    _addImageAction.setText(_tr("ImportImage"));
    _addDxfAction.setText(_tr("ImportDxf"));
    _addPolyline.setText(_tr("DrawClosedPath"));
    _addPoint.setText(_tr("AddPoint"));
    _addCircleModeAction.setText(_tr("DrawCircle"));
    _addEllipseModeAction.setText(_tr("DrawEllipse"));
    _getLength.setText(_tr("LengthMeasure"));
    _getAngle.setText(_tr("AngleMeasure"));
    _addText.setText(_tr("AddTextAnnotation"));
    _addArrow.setText(_tr("AddArrowAnnotation"));

    _modelClearAllBc.setText(_tr("ClearBCs"));
    _modelClearAllEnv.setText(_tr("ClearThermEnvs"));

    updateImagesVisibility();
    updateUpgradeAction();
    updateFrontBack();
    updateCustomLength();
    updateConductivityData();

    for (QMap<QAction*, QString>::iterator it=_exampleTitles.begin() ; it!=_exampleTitles.end() ; it++)
    {
        QString t = _tr(it.value());
        if (t.isEmpty())
            t = it.value();
        it.key()->setText(t);
    }

    for (QMap<QMenu*, QString>::iterator it=_menuTitles.begin() ; it!=_menuTitles.end() ; it++)
    {
        QString t = _tr(it.value());
        if (t.isEmpty())
            t = it.value();
        it.key()->setTitle(t);
    }
}

void MainMenu::updateUpgradeAction()
{
    LOG_INFO("Update upgrade state.");

    if (StatesManager::instance()->upgradeState()==StatesManager::DownloadDone)
        _upgradeDownloadAction.setText(_tr("InstallUpgrade"));
    else if (StatesManager::instance()->upgradeState()==StatesManager::Downloading)
        _upgradeDownloadAction.setText(_tr("DownloadingUpgrade")+QString(" (")+ToolBox::intToString(StatesManager::instance()->upgradePercent()).c_str()+QString("%)"));
    else
        _upgradeDownloadAction.setText(_tr("DownloadUpgrade"));

    // Update icon.
    if (StatesManager::instance()->upgradeState()==StatesManager::Downloading)
        _upgradeDownloadAction.setIcon(getProgressIcon(StatesManager::instance()->upgradePercent()));
    else if (StatesManager::instance()->upgradeState()==StatesManager::DownloadDone)
        _upgradeDownloadAction.setIcon(QIcon(":/images/orange-blue/install.png"));
    else
        _upgradeDownloadAction.setIcon(QIcon(":/images/orange-blue/upgrade.png"));

    // Visibility.
    bool show_button=false;
    if (StatesManager::instance()->upgradeState()==StatesManager::OutOfDate)
        show_button=true;
    else if (StatesManager::instance()->upgradeState()==StatesManager::Downloading)
        show_button=true;
    else if (StatesManager::instance()->upgradeState()==StatesManager::DownloadDone)
        show_button=true;
    else if (StatesManager::instance()->upgradeState()==StatesManager::UpgradeInProgress)
        show_button=true;

    _upgradeToolbar.setVisible(show_button);
    _upgradeToolbar.setEnabled(StatesManager::instance()->simulationState()!=StatesManager::InProgress);
}

void MainMenu::applyTheme()
{
    _newDocumentAction.setIcon(QIcon(":/images/orange-blue/new_document.png"));
    _openDocumentAction.setIcon(QIcon(":/images/orange-blue/open_document.png"));
    _saveDocumentAction.setIcon(QIcon(":/images/orange-blue/save_document.png"));
    _saveDocumentAsAction.setIcon(QIcon(":/images/orange-blue/save_document_as.png"));
    _cutAction.setIcon(QIcon(":/images/orange-blue/cut.png"));
    _copyAction.setIcon(QIcon(":/images/orange-blue/copy.png"));
    _pasteAction.setIcon(QIcon(":/images/orange-blue/paste.png"));
    _undoAction.setIcon(QIcon(":/images/orange-blue/undo.png"));
    _redoAction.setIcon(QIcon(":/images/orange-blue/redo.png"));
    _aboutAction.setIcon(QIcon(":/images/orange-blue/about.png"));

    _selectionModeAction.setIcon(QIcon(":/images/orange-blue/selection.png"));
    _addRectangleModeAction.setIcon(QIcon(":/images/orange-blue/add_rectangle.png"));

    _getMaterialModeAction.setIcon(QIcon(":/images/orange-blue/pipette.png"));
    _setPhysicModeAction.setIcon(QIcon(":/images/orange-blue/set.png"));
    _setEnvironmentModeAction.setIcon(QIcon(":/images/orange-blue/environments.png"));

    _computeAction.setIcon(QIcon(":/images/orange-blue/play.png"));

    _frontAction.setIcon(QIcon(":/images/orange-blue/front.png"));
    _backAction.setIcon(QIcon(":/images/orange-blue/back.png"));

    _frontOneShotAction.setIcon(QIcon(":/images/orange-blue/front_one_shot.png"));
    _backOneShotAction.setIcon(QIcon(":/images/orange-blue/back_one_shot.png"));

    _fitAction.setIcon(QIcon(":/images/orange-blue/fit.png"));
    _zoomInAction.setIcon(QIcon(":/images/orange-blue/zoom_in.png"));
    _zoomOutAction.setIcon(QIcon(":/images/orange-blue/zoom_out.png"));

    _copy2DModelTo1DModel.setIcon(QIcon(":/images/orange-blue/from2d.png"));
    _toogleEn13370.setIcon(QIcon(":/images/orange-blue/en13370.png"));
    
    _addPolyline.setIcon(QIcon(":/images/orange-blue/polyline.png"));
    _addPoint.setIcon(QIcon(":/images/orange-blue/add_point.png"));
    _addCircleModeAction.setIcon(QIcon(":/images/orange-blue/add_circle.png"));
    _addEllipseModeAction.setIcon(QIcon(":/images/orange-blue/add_ellipse.png"));

    _addImageAction.setIcon(QIcon(":/images/orange-blue/add_image.png"));
    _addDxfAction.setIcon(QIcon(":/images/orange-blue/add_dxf.png"));

    _getLength.setIcon(QIcon(":/images/orange-blue/get_length.png"));
    _getAngle.setIcon(QIcon(":/images/orange-blue/get_angle.png"));

    _addText.setIcon(QIcon(":/images/orange-blue/text.png"));
    _addArrow.setIcon(QIcon(":/images/orange-blue/arrow.png"));

    updateImagesVisibility();
    updateUpgradeAction();
}

void MainMenu::updateImagesVisibility()
{
    if (StatesManager::instance()->showImages())
        _imagesVisibilityAction.setIcon(QIcon(":/images/orange-blue/image_visibility_on.png"));
    else
        _imagesVisibilityAction.setIcon(QIcon(":/images/orange-blue/image_visibility_off.png"));

    if (StatesManager::instance()->showImages())
        _imagesVisibilityAction.setText(_tr("HideImages"));
    else
        _imagesVisibilityAction.setText(_tr("ShowImages"));

    if (StatesManager::instance()->showDxf())
        _dxfVisibilityAction.setIcon(QIcon(":/images/orange-blue/dxf_visibility_on.png"));
    else
        _dxfVisibilityAction.setIcon(QIcon(":/images/orange-blue/dxf_visibility_off.png"));

    if (StatesManager::instance()->showDxf())
        _dxfVisibilityAction.setText(_tr("HideDxf"));
    else
        _dxfVisibilityAction.setText(_tr("ShowDxf"));
}

void MainMenu::installOnWindow()
{
    QMainWindow *window=dynamic_cast<QMainWindow*>(parent());
    if (!window)
        return;
    window->setMenuBar(this);

    // Top.
    window->addToolBar(Qt::TopToolBarArea, &_menuToolBar);
    window->addToolBar(Qt::TopToolBarArea, &_upgradeToolbar);

    // Right.
    window->addToolBar(Qt::RightToolBarArea, &_zoomToolBar);
    window->addToolBar(Qt::RightToolBarArea, &_imagesToolBar);

    window->addToolBar(Qt::RightToolBarArea, &_stackToolBar);
    window->addToolBar(Qt::RightToolBarArea, &_modelToolBar);
    window->insertToolBarBreak(&_modelToolBar);
}

void MainMenu::loadLastProjects()
{
    LOG_INFO("Load last projects.");

    // Remove items.
    _lastProjectsMenu.clear();

    // Get last projects.
    std::string last=LocalData::instance()->getParameter("last-projects");
    std::vector<std::string> files=ToolBox::split(last, '|');

    // Add to menu.
    int n=0;
    for (unsigned int i=0 ; i<files.size() ; i++)
    {
        // Max 10 entries.
        if (n>10)
            break;

        // Test file existency.
        QString f=files.at(i).c_str();
        if (!QFile::exists(f))
            continue;

        QAction *action=_lastProjectsMenu.addAction(f);
        connect(action, qOverload<bool>(&QAction::triggered), this, &MainMenu::onOpenProject);
        n++;
    }

    _openLastAction.setEnabled(n>0);
}

void MainMenu::addLastProject(QString filename)
{
    // Use \ instead of / (Windows only).
#ifdef WIN32
    filename.replace("/", "\\");
#endif

    // Get last projects.
    std::string last=LocalData::instance()->getParameter("last-projects");
    std::vector<std::string> files=ToolBox::split(last, '|');

    // Check file is not already defined.
    std::string new_file=filename.toStdString();
    for (unsigned int i=0 ; i<files.size() ; i++)
    {
        if (files.at(i)==new_file)
        {
            files.erase(files.begin()+i);
            break;
        }
    }

    // Insert at first position.
    files.insert(files.begin(), new_file);

    // Max 10 entries.
    if (files.size()>10)
        files.resize(10);

    // Save.
    std::string f=ToolBox::merge(files, '|');
    LocalData::instance()->setParameter("last-projects", f);

    loadLastProjects();
}

void MainMenu::addExample(QMenu *menu, const QString& title, const QString& filename)
{
    QAction *action = menu->addAction(_tr(title));
    action->setData(filename);
    connect(action, &QAction::triggered, this, &MainMenu::onOpenExample);

    _exampleTitles[action] = title;
}

QMenu *MainMenu::addMenu(QMenu *menu, const QString &title)
{
    QMenu *m = menu->addMenu(_tr(title));
    _menuTitles[m] = title;

    return m;
}

void MainMenu::loadExamples()
{
    LOG_INFO("Load examples.");

    _examplesMenu.clear();

    // Add sub-menus.
    QMenu *simple       = addMenu(&_examplesMenu, "SimpleExamples");
    QMenu *repartition  = addMenu(&_examplesMenu, "SplittedExamples");
    QMenu *ground_floor = addMenu(&_examplesMenu, "GroundExamples");

    QMenu *windows = addMenu(&_examplesMenu, "WindowsExamples");
    QMenu *tutorial = addMenu(&_examplesMenu, "TutorialExamples");

    QMenu *validations  = addMenu(&_examplesMenu, "ValidationsExamples");

    // Simple
    addExample(simple, "Example_1", "Exemple_2.c2d");
    addExample(simple, "Example_2", "Exemple_4.c2d");
    addExample(simple, "Example_3", "Exemple_3.c2d");
    addExample(simple, "Example_4", "Exemple_1.c2d");

    // Repartition.
    addExample(repartition, "Example_8",   "Exemple_8.c2d");
    addExample(repartition, "Example_9",   "Exemple_9.c2d");
    addExample(repartition, "Example_10",  "Exemple_10.c2d");

    // Ground floor.
    addExample(ground_floor, "Example_6", "Exemple_6.c2d");
    addExample(ground_floor, "Example_7", "Exemple_7.c2d");

    // Windows.
    addExample(windows, "Example_5", "Exemple_5.c2d");

    // Tutorial.
    QMenu *cat1  = addMenu(tutorial, "ExamplesCat1");
    QMenu *cat2  = addMenu(tutorial, "ExamplesCat2");
    QMenu *cat3  = addMenu(tutorial, "ExamplesCat3");
    QMenu *cat4  = addMenu(tutorial, "ExamplesCat4");
    QMenu *cat5  = addMenu(tutorial, "ExamplesCat5");
    QMenu *cat6  = addMenu(tutorial, "ExamplesCat6");
    QMenu *cat7  = addMenu(tutorial, "ExamplesCat7");
    QMenu *cat8  = addMenu(tutorial, "ExamplesCat8");
    QMenu *cat9  = addMenu(tutorial, "ExamplesCat9");
    QMenu *cat10 = addMenu(tutorial, "ExamplesCat10");
    QMenu *cat11 = addMenu(tutorial, "ExamplesCat11");
    QMenu *cat12 = addMenu(tutorial, "ExamplesCat12");
    QMenu *cat13 = addMenu(tutorial, "ExamplesCat13");
    QMenu *cat14 = addMenu(tutorial, "ExamplesCat14");
    QMenu *cat15 = addMenu(tutorial, "ExamplesCat15");
    QMenu *cat16 = addMenu(tutorial, "ExamplesCat16");

    // Cat 1.
    addExample(cat1, "Tutorial_4", "Tutorial_4.c2d");
    addExample(cat1, "Tutorial_3", "Tutorial_3.c2d");

    // Cat 2.
    addExample(cat2, "Tutorial_9", "Tutorial_9.c2d");
    addExample(cat2, "Tutorial_10", "Tutorial_10.c2d");

    // Cat 3.
    addExample(cat3, "Tutorial_6", "Tutorial_6.c2d");
    addExample(cat3, "Tutorial_7", "Tutorial_7.c2d");
    addExample(cat3, "Tutorial_8", "Tutorial_8.c2d");

    // Cat 4.
    addExample(cat4, "Tutorial_25", "Tutorial_25.c2d");
    addExample(cat4, "Tutorial_24", "Tutorial_24.c2d");

    // Cat 5.
    addExample(cat5, "Tutorial_26", "Tutorial_26.c2d");

    // Cat 6.
    addExample(cat6, "Tutorial_2", "Tutorial_2.c2d");
    addExample(cat6, "Tutorial_1", "Tutorial_1.c2d");

    // Cat 7.
    addExample(cat7, "Tutorial_12", "Tutorial_12.c2d");
    addExample(cat7, "Tutorial_11", "Tutorial_11.c2d");

    // Cat 8.
    addExample(cat8, "Tutorial_18", "Tutorial_18.c2d");
    addExample(cat8, "Tutorial_19", "Tutorial_19.c2d");

    // Cat 9.
    addExample(cat9, "Tutorial_14", "Tutorial_14.c2d");
    addExample(cat9, "Tutorial_13", "Tutorial_13.c2d");
    addExample(cat9, "Tutorial_15", "Tutorial_15.c2d");

    // Cat 10.
    addExample(cat10, "Tutorial_20", "Tutorial_20.c2d");
    addExample(cat10, "Tutorial_21", "Tutorial_21.c2d");

    // Cat 11.
    addExample(cat11, "Tutorial_5", "Tutorial_5.c2d");

    // Cat 12.
    addExample(cat12, "Tutorial_16", "Tutorial_16.c2d");
    addExample(cat12, "Tutorial_17", "Tutorial_17.c2d");

    // Cat 13.
    addExample(cat13, "Tutorial_22", "Tutorial_22.c2d");

    // Cat 14.
    addExample(cat14, "Tutorial_28", "Tutorial_28.c2d");

    // Cat 15.
    addExample(cat15, "Tutorial_29", "Tutorial_29.c2d");

    // Cat 16.
    addExample(cat16, "Tutorial_30", "Tutorial_30.c2d");

    // Validations.
    QMenu *en10211  = addMenu(validations, "ExamplesCat10211");
    QMenu *en10077 = addMenu(validations, "ExamplesCat10077");
    QMenu *internal = addMenu(validations, "ExamplesCatInternal");

    // EN 10211.
    addExample(en10211, "cas_1", "cas_1.c2d");
    addExample(en10211, "cas_2", "cas_2.c2d");

    // EN 10077.
    addExample(en10077, "cas_D1", "cas_D1.c2d");
    addExample(en10077, "cas_D2", "cas_D2.c2d");
    addExample(en10077, "cas_D3", "cas_D3.c2d");
    addExample(en10077, "cas_D4", "cas_D4.c2d");
    addExample(en10077, "cas_D5", "cas_D5.c2d");
    addExample(en10077, "cas_D6", "cas_D6.c2d");
    addExample(en10077, "cas_D7", "cas_D7.c2d");
    addExample(en10077, "cas_D8", "cas_D8.c2d");
    addExample(en10077, "cas_D9", "cas_D9.c2d");
    addExample(en10077, "cas_D10", "cas_D10.c2d");

    // Internal.
    for (unsigned int i=1 ; i<=32 ; i++)
    {
        QString title    = QString("Test %1").arg(i);
        QString filename = QString("test_%1.c2d").arg(i);
        addExample(internal, title, filename);
    }
}

void MainMenu::upgradeStateChanged()
{
    updateUpgradeAction();
}

void MainMenu::statesChanged()
{
    LOG_INFO("State has changed.");

    Model *model=StatesManager::instance()->currentModel();

    _modelLengthAction.setVisible(model && (model->computationType()==Model::ThermalTransmission));
    _modelEqConductivityAction.setVisible(model && (model->computationType()==Model::EquivalentThermalConductivity));

    if (StatesManager::instance()->simulationState()==StatesManager::InProgress)
    {
        _selectionModeAction.setEnabled(false);
        _addRectangleModeAction.setEnabled(false);

        _addPolyline.setEnabled(false);
        _addPoint.setEnabled(false);
        _addCircleModeAction.setEnabled(false);
        _addEllipseModeAction.setEnabled(false);

        _getMaterialModeAction.setEnabled(false);
        _setPhysicModeAction.setEnabled(false);
        _setEnvironmentModeAction.setEnabled(false);
        _copy2DModelTo1DModel.setEnabled(false);
        _toogleEn13370.setEnabled(false);
        _backAction.setEnabled(false);
        _frontAction.setEnabled(false);
        _frontOneShotAction.setEnabled(false);
        _backOneShotAction.setEnabled(false);
        _cutAction.setEnabled(false);
        _copyAction.setEnabled(false);
        _delete.setEnabled(false);
        _pasteAction.setEnabled(false);
        _undoAction.setEnabled(false);
        _redoAction.setEnabled(false);
        _selectAll.setEnabled(false);

        _addImageAction.setEnabled(false);
        _addDxfAction.setEnabled(false);
        _getLength.setEnabled(false);
        _getAngle.setEnabled(false);

        _preferencesAction.setEnabled(false);
        _addText.setEnabled(false);
        _addArrow.setEnabled(false);

        _modelClearAllBc.setEnabled(false);
        _modelClearAllEnv.setEnabled(false);

        _modelTypeThermalBridgeAction.setEnabled(false);
        _modelTypeThermalFluxAction.setEnabled(false);
        _modelTypeThermalTransmissionAction.setEnabled(false);
        _modelTypeThermalConductivityAction.setEnabled(false);
        _modelTypeGlaserCondensationAction.setEnabled(false);

        _lengthTypeMenu.setEnabled(false);
        _modelLengthFullAction.setEnabled(false);
        _modelLengthXAction.setEnabled(false);
        _modelLengthYAction.setEnabled(false);
        _modelLengthCustomAction.setEnabled(false);

        _equivalentConductivityMenu.setEnabled(false);
        _modelConductivityWidth.setEnabled(false);
        _modelConductivityThickness.setEnabled(false);
        _modelConductivityResistance.setEnabled(false);
    }
    else
    {
        _selectionModeAction.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::Selection);
        _addRectangleModeAction.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::AddRectangle);

        _addPolyline.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::AddPolyline);
        _addPoint.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::AddPoint);
        _addCircleModeAction.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::AddCircle);
        _addEllipseModeAction.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::AddEllipse);

        _getMaterialModeAction.setChecked(StatesManager::instance()->interfaceMode() == StatesManager::GetMaterial);
        _setPhysicModeAction.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::SetPhysics);
        _setEnvironmentModeAction.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::SetEnvironments);

        _getLength.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::AddLengthCotation);
        _getAngle.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::AddAngleCotation);
        _addText.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::AddText);
        _addArrow.setChecked(StatesManager::instance()->interfaceMode()==StatesManager::AddArrow);

        _preferencesAction.setEnabled(true);
        _modelTypeThermalBridgeAction.setEnabled(true);
        _modelTypeThermalFluxAction.setEnabled(true);
        _modelTypeThermalTransmissionAction.setEnabled(true);
        _modelTypeThermalConductivityAction.setEnabled(true);
        _modelTypeGlaserCondensationAction.setEnabled(true);

        _lengthTypeMenu.setEnabled(true);
        _modelLengthFullAction.setEnabled(true);
        _modelLengthXAction.setEnabled(true);
        _modelLengthYAction.setEnabled(true);
        _modelLengthCustomAction.setEnabled(true);

        _equivalentConductivityMenu.setEnabled(true);
        _modelConductivityWidth.setEnabled(true);
        _modelConductivityThickness.setEnabled(true);
        _modelConductivityResistance.setEnabled(true);

        if (StatesManager::instance()->viewType()==StatesManager::DisplayModel || StatesManager::instance()->viewType()==StatesManager::Display1DModel)
        {
            _selectionModeAction.setEnabled(true);
            _addRectangleModeAction.setEnabled(true);

            _addPolyline.setEnabled(true);
            _addPoint.setEnabled(true);
            _addCircleModeAction.setEnabled(true);
            _addEllipseModeAction.setEnabled(true);

            _getMaterialModeAction.setEnabled(true);
            _setPhysicModeAction.setEnabled(true);
            _setEnvironmentModeAction.setEnabled(model && (model->computationType()==Model::ThermalBridge));

            _addImageAction.setEnabled(true);
            _addDxfAction.setEnabled(true);
            _getLength.setEnabled(true);
            _getAngle.setEnabled(true);
            _addText.setEnabled(true);
            _addArrow.setEnabled(true);
        }
        else
        {
            _selectionModeAction.setEnabled(true);
            _addRectangleModeAction.setEnabled(false);
            _addPolyline.setEnabled(false);
            _addCircleModeAction.setEnabled(false);
            _addEllipseModeAction.setEnabled(false);

            _getMaterialModeAction.setEnabled(false);
            _setPhysicModeAction.setEnabled(false);
            _setEnvironmentModeAction.setEnabled(false);

            _addImageAction.setEnabled(false);
            _addDxfAction.setEnabled(false);
            _addPoint.setEnabled(false);
            _getLength.setEnabled(false);
            _getAngle.setEnabled(false);
            _addText.setEnabled(false);
            _addArrow.setEnabled(false);
        }

        _copy2DModelTo1DModel.setEnabled(StatesManager::instance()->viewType()==StatesManager::Display1DModel);
        _toogleEn13370.setEnabled(StatesManager::instance()->viewType()==StatesManager::Display1DModel);
        _toogleEn13370.setChecked(model && model->en13370Plugin());

        _modelClearAllBc.setEnabled(model && StatesManager::instance()->interfaceMode() == StatesManager::SetPhysics);
        _modelClearAllEnv.setEnabled(model && StatesManager::instance()->interfaceMode() == StatesManager::SetEnvironments);

        _modelTypeThermalBridgeAction.setChecked(model && (model->computationType()==Model::ThermalBridge));
        _modelTypeThermalFluxAction.setChecked(model && (model->computationType()==Model::Flux));
        _modelTypeThermalTransmissionAction.setChecked(model && (model->computationType()==Model::ThermalTransmission));
        _modelTypeThermalConductivityAction.setChecked(model && (model->computationType()==Model::EquivalentThermalConductivity));
        _modelTypeGlaserCondensationAction.setChecked(model && (model->computationType() == Model::GlaserCondensation));

        _modelLengthFullAction.setChecked(model && (model->transmissionCoefficientLengthType()==Model::FullLength));
        _modelLengthXAction.setChecked(model && (model->transmissionCoefficientLengthType()==Model::ProjectedXLength));
        _modelLengthYAction.setChecked(model && (model->transmissionCoefficientLengthType()==Model::ProjectedYLength));
        _modelLengthCustomAction.setChecked(model && (model->transmissionCoefficientLengthType()==Model::CustomLength));

        _cutAction.setEnabled(StatesManager::instance()->selectedVolumes().size()>=1 && (StatesManager::instance()->viewType()==StatesManager::DisplayModel || StatesManager::instance()->viewType()==StatesManager::Display1DModel));
        _copyAction.setEnabled(StatesManager::instance()->selectedVolumes().size()>=1 && (StatesManager::instance()->viewType()==StatesManager::DisplayModel || StatesManager::instance()->viewType()==StatesManager::Display1DModel));
        _delete.setEnabled((StatesManager::instance()->selectedAnnotations().size()>=1 || StatesManager::instance()->selectedVolumes().size()>=1 || StatesManager::instance()->selectedImages().size()>=1 || StatesManager::instance()->selectedDxf().size()>=1) && (StatesManager::instance()->viewType()==StatesManager::DisplayModel || StatesManager::instance()->viewType()==StatesManager::Display1DModel));

        _pasteAction.setEnabled(StatesManager::instance()->clipboardHasContent() && (StatesManager::instance()->viewType()==StatesManager::DisplayModel || StatesManager::instance()->viewType()==StatesManager::Display1DModel));

        _undoAction.setEnabled(ActionsManager::instance()->canUndo());
        _redoAction.setEnabled(ActionsManager::instance()->canRedo());

        // Delete selection on non-selection mode.
        _selectAll.setEnabled(StatesManager::instance()->interfaceMode()==StatesManager::Selection);
        if (StatesManager::instance()->interfaceMode()!=StatesManager::Selection && model)
        {
            model->unselectAllVolumes();
            model->unselectAllImages();
            model->unselectAllDxf();

            Model *model1d=model->model1d();
            if (model1d)
            {
                model1d->unselectAllVolumes();
                model1d->unselectAllImages();
                model1d->unselectAllDxf();
            }
        }
    }

    _saveDocumentAction.setEnabled(!StatesManager::instance()->saved());
    _computeAction.setEnabled(StatesManager::instance()->simulationState()!=StatesManager::InProgress);

    updateUpgradeAction();

    updateImagesVisibility();
    updateFrontBack();
    updatePasteIcon();
    updateCustomLength();
    updateConductivityData();
}

void MainMenu::zoomChanged()
{
    Zoom *zoom=StatesManager::instance()->zoom();
    if (!zoom)
        return;

    _zoomInAction.setEnabled(!zoom->isZoomAtMax());
    _zoomOutAction.setEnabled(!zoom->isZoomAtMin());
}

void MainMenu::selectionModeRequested()
{
    LOG_INFO("Selection mode selected.");
    StatesManager::instance()->setInterfaceMode(StatesManager::Selection);
}

void MainMenu::addRectangleModeRequested()
{
    LOG_INFO("Add rectangle mode selected.");
    StatesManager::instance()->setInterfaceMode(StatesManager::AddRectangle);
}

void MainMenu::addPolylineModeRequested()
{
    LOG_INFO("Add polyline mode selected.");
    StatesManager::instance()->setInterfaceMode(StatesManager::AddPolyline);
}

void MainMenu::addCircleModeRequested()
{
    LOG_INFO("Add circle mode selected.");
    StatesManager::instance()->setInterfaceMode(StatesManager::AddCircle);
}

void MainMenu::addEllipseModeRequested()
{
    LOG_INFO("Add ellipse mode selected.");
    StatesManager::instance()->setInterfaceMode(StatesManager::AddEllipse);
}

void MainMenu::getMaterialModeRequested()
{
    LOG_INFO("Get material mode selected.");
    StatesManager::instance()->setInterfaceMode(StatesManager::GetMaterial);
}

void MainMenu::setPhysicsModeRequested()
{
    LOG_INFO("Set physics mode selected.");
    StatesManager::instance()->setInterfaceMode(StatesManager::SetPhysics);
}

void MainMenu::setEnvironmentsModeRequested()
{
    LOG_INFO("Set environments mode selected.");
    StatesManager::instance()->setInterfaceMode(StatesManager::SetEnvironments);
}

void MainMenu::undo()
{
    LOG_INFO("Undo action.");
    ActionsManager::instance()->undo();
}

void MainMenu::redo()
{
    LOG_INFO("Redo action.");
    ActionsManager::instance()->redo();
}

void MainMenu::selectAllRequested()
{
    LOG_INFO("Select all.");
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;
    model->selectAllVolumes();
    
    StatesManager::instance()->repaintView();
}

void MainMenu::deleteRequested()
{
    LOG_INFO("Delete requested.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (!volumes.empty())
    {
        DeleteVolumes *action=new DeleteVolumes(model, volumes);
        ActionsManager::instance()->exec(action);
        return;
    }
        
    std::vector<Image*> images=StatesManager::instance()->selectedImages();
    if (images.size()==1)
    {
        DeleteImage *action=new DeleteImage(model, images.at(0));
        ActionsManager::instance()->exec(action);
        return;
    }

    std::vector<DxfContent*> dxf=StatesManager::instance()->selectedDxf();
    if (dxf.size()==1)
    {
        DeleteDxf *action=new DeleteDxf(model, dxf.at(0));
        ActionsManager::instance()->exec(action);
        return;
    }

    std::vector<Annotation*> annotations=StatesManager::instance()->selectedAnnotations();
    if (annotations.size()>=1)
    {
        RemoveAnnotation *action=new RemoveAnnotation(model, annotations);
        ActionsManager::instance()->exec(action);
        return;
    }
}

void MainMenu::rotationRequested(double x, double y)
{
    LOG_INFO("Rotation requested.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.empty())
        return;

    bool ok;
    double angle = QInputDialog::getDouble(this, _tr("RotateSelection"), _tr("RotationAngleDegrees"), 0.0, -360.0, 360.0, 2, &ok);

    if (!ok)
        return;

    if (fabs(angle) < 1.0e-3)
        return;

    RotateVolumes *action = new RotateVolumes(model, x, y, angle);
    ActionsManager::instance()->exec(action);
}

QIcon MainMenu::getProgressIcon(int percent) const
{
    QImage base(":/images/orange-blue/progress_earth.png");
    QPainter painter;
    painter.begin(&base);

    QColor themedColor = QColor(236, 145, 16);

    QBrush brush(themedColor);
    QPen pen(themedColor);
    painter.setBrush(brush);
    painter.setPen(pen);

    QRect r(23, 30, 19*percent/100.0, 5);
    painter.drawRect(r);
    painter.end();
    QPixmap p=QPixmap::fromImage(base);
    return QIcon(p);
}

void MainMenu::computationRequested()
{
    LOG_INFO("Computation requested.");

    StatesManager::instance()->setInterfaceMode(StatesManager::Selection);
    StatesManager::instance()->setViewType(StatesManager::DisplayModel);
    StatesManager::instance()->setSavedModel(false);
    
    // Start simulation.
    Model *model=StatesManager::instance()->currentModel();
    if (model)
    {
        LOG_INFO("Start computation.");
        model->unselectAllImages();
        model->unselectAllVolumes();
        model->unselectAllDxf();
        StatesManager::instance()->changed();
        _computationThread->start();
    }
}

void MainMenu::abortComputation()
{
    LOG_INFO("Abort compuation requested.");
    _computationThread->abort();
}

void MainMenu::connectReporter(ComputationReporter *reporter)
{
    connect(_computationThread,
            &ComputationThread::simulationTypeChanged,
            reporter,
            &ComputationReporter::setRefinementType,
            Qt::BlockingQueuedConnection);
    connect(_computationThread,
            &ComputationThread::simulationLevelChanged,
            reporter,
            &ComputationReporter::setLevel,
            Qt::BlockingQueuedConnection);
}

void MainMenu::onAddImageRequested()
{
    LOG_INFO("Add image requested.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    QString path=LocalData::instance()->getParameter("last-image-path").c_str();

    QString filename=QFileDialog::getOpenFileName(this, _tr("ImportImage"), path, _tr("ImageImportPattern") );
    if (filename.isEmpty())
        return;

    addImage(filename);
}

void MainMenu::addImage(const QString &filename)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    QFileInfo info(filename);
    LocalData::instance()->setParameter("last-image-path", info.absolutePath().toStdString());

    // Test image.
    QImage image(filename);
    if (image.width()==0 || image.height()==0)
    {
        return;
    }

    // Create image.
    QFile file(filename);
    bool success = file.open(QIODevice::ReadOnly);
    if (!success)
        return;

    QByteArray data=file.readAll();
    file.close();
    QString content=data.toBase64();

    AddImage *action=new AddImage(model, content.toStdString(), 0.0, 0.0, image.width()/1000.0, image.height()/1000.0);
    ActionsManager::instance()->exec(action);

    // Create Qt image data.
    Image *im=action->image();
    if (im)
        ModelViewer::createQtImageData(im);
}

void MainMenu::onAddDxfRequested()
{
    LOG_INFO("Add Dxf requested.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    QString path=LocalData::instance()->getParameter("last-dxf-path").c_str();

    QString filename=QFileDialog::getOpenFileName(this, _tr("ImportDxf"), path, _tr("DxfImportPattern") );
    if (filename.isEmpty())
        return;

    addDxf(filename);
}

void MainMenu::addDxf(const QString &filename)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    QFileInfo info(filename);
    LocalData::instance()->setParameter("last-dxf-path", info.absolutePath().toStdString());

    // Create Dxf.
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray data=file.readAll();
    file.close();
    QString content=data.toBase64();

    AddDxf *action=new AddDxf(model, content.toStdString());
    ActionsManager::instance()->exec(action);

    // Create Qt dxf data.
    DxfContent *d=action->dxf();
    if (d)
        ModelViewer::createQtDxfData(d);
}

void MainMenu::onToogleImageVisibility()
{
    LOG_INFO("Toogle images visibility.");
    StatesManager::instance()->showImages(!StatesManager::instance()->showImages());
}

void MainMenu::onToogleDxfVisibility()
{
    LOG_INFO("Toogle Dxf visibility.");
    StatesManager::instance()->showDxf(!StatesManager::instance()->showDxf());
}

void MainMenu::frontSelectedRequested()
{
    LOG_INFO("Bring up volume requested.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    std::vector<Image*> images=StatesManager::instance()->selectedImages();

    if (volumes.size()==1)
    {
        Volume *volume=volumes.at(0);
        BringUpVolume *action=new BringUpVolume(model, volume);
        ActionsManager::instance()->exec(action);
    }

    else if (images.size()==1)
    {
        Image *image=images.at(0);
        BringUpImage *action=new BringUpImage(model, image);
        ActionsManager::instance()->exec(action);
    }

    StatesManager::instance()->repaintView();
}

void MainMenu::backSelectedRequested()
{
    LOG_INFO("Bring down volume requested.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    std::vector<Image*> images=StatesManager::instance()->selectedImages();

    if (volumes.size()==1)
    {
        Volume *volume=volumes.at(0);
        BringDownVolume *action=new BringDownVolume(model, volume);
        ActionsManager::instance()->exec(action);
    }

    else if (images.size()==1)
    {
        Image *image=images.at(0);
        BringDownImage *action=new BringDownImage(model, image);
        ActionsManager::instance()->exec(action);
    }

    StatesManager::instance()->repaintView();
}

void MainMenu::frontOneShotSelectedRequested()
{
    LOG_INFO("Send to front requested.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    std::vector<Image*> images=StatesManager::instance()->selectedImages();

    if (volumes.size()==1)
    {
        Volume *volume=volumes.at(0);
        SendToFrontVolume *action=new SendToFrontVolume(model, volume);
        ActionsManager::instance()->exec(action);
    }

    else if (images.size()==1)
    {
        Image *image=images.at(0);
        SendToFrontImage *action=new SendToFrontImage(model, image);
        ActionsManager::instance()->exec(action);
    }

    StatesManager::instance()->repaintView();
}

void MainMenu::backOneShotSelectedRequested()
{
    LOG_INFO("Send to back requested.");

     Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    std::vector<Image*> images=StatesManager::instance()->selectedImages();

    if (volumes.size()==1)
    {
        Volume *volume=volumes.at(0);
        SendToBackVolume *action=new SendToBackVolume(model, volume);
        ActionsManager::instance()->exec(action);
    }

    else if (images.size()==1)
    {
        Image *image=images.at(0);
        SendToBackImage *action=new SendToBackImage(model, image);
        ActionsManager::instance()->exec(action);
    }

    StatesManager::instance()->repaintView();
}

void MainMenu::updateCustomLength()
{
    LOG_INFO("Update custom length.");

    _modelLengthCustomAction.setText(_tr("FixedLength"));

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    if (model->computationType()!=Model::ThermalTransmission)
        return;
    if (model->transmissionCoefficientLengthType()!=Model::CustomLength)
        return;

    QLocale locale;
    QString customLength=locale.toString(model->transmissionCoefficientLength()*1000.0, 'f', 1);
    _modelLengthCustomAction.setText(_tr("FixedLengthTitle")+" "+customLength+_tr("LengthMM"));
}

void MainMenu::updateConductivityData()
{
    LOG_INFO("Update equivalent conductivity data.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    if (model->computationType() != Model::EquivalentThermalConductivity)
        return;

    QLocale locale;

    QString width = locale.toString(model->getEquivalentConductivityWidth() * 1000.0, 'f', 1);
    QString thick = locale.toString(model->getEquivalentConductivityThickness() * 1000.0, 'f', 1);
    QString res   = locale.toString(model->getEquivalentConductivityResistanceSum(), 'f', 3);

    _modelConductivityWidth.setText(_tr("WidthTitle") + " " + width + _tr("LengthMM"));
    _modelConductivityThickness.setText(_tr("ThicknessTitle") + " " + thick + _tr("LengthMM"));
    _modelConductivityResistance.setText(_tr("SuperResistancesTitle") + " " + res + _tr("ResistancesUnits"));
}

void MainMenu::updateFrontBack()
{
    LOG_INFO("Update front/back icons.");

    Model *model=StatesManager::instance()->currentModelView();
    if(StatesManager::instance()->simulationState()==StatesManager::InProgress || !model)
    {
        _frontAction.setEnabled(false);
        _backAction.setEnabled(false);
        _frontOneShotAction.setEnabled(false);
        _backOneShotAction.setEnabled(false);
        return;
    }

    if (StatesManager::instance()->selectedVolumes().size()==1)
    {
        _frontAction.setText(_tr("FrontVolume"));
        _backAction.setText(_tr("BackVolume"));
        _frontOneShotAction.setText(_tr("VolumeOnTop"));
        _backOneShotAction.setText(_tr("VolumeOnBottom"));

        if (model->std::vector<Volume*>::size()>1)
        {
            int n=model->volumeStackPosition(StatesManager::instance()->selectedVolumes().at(0));
            _backAction.setEnabled(n!=0);
            _frontAction.setEnabled(n!=(model->std::vector<Volume*>::size()-1));
            _backOneShotAction.setEnabled(n!=0);
            _frontOneShotAction.setEnabled(n!=(model->std::vector<Volume*>::size()-1));
        }
        else
        {
            _frontAction.setEnabled(false);
            _backAction.setEnabled(false);
            _frontOneShotAction.setEnabled(false);
            _backOneShotAction.setEnabled(false);
        }
    }

    else if (StatesManager::instance()->selectedImages().size()==1)
    {
        _frontAction.setText(_tr("FrontImage"));
        _backAction.setText(_tr("BackImage"));
        _frontOneShotAction.setText(_tr("ImageOnTop"));
        _backOneShotAction.setText(_tr("ImageOnBottom"));

        if (model->std::vector<Image*>::size()>1)
        {
            int n=model->imageStackPosition(StatesManager::instance()->selectedImages().at(0));
            _backAction.setEnabled(n!=0);
            _frontAction.setEnabled(n!=(model->std::vector<Image*>::size()-1));
            _backOneShotAction.setEnabled(n!=0);
            _frontOneShotAction.setEnabled(n!=(model->std::vector<Image*>::size()-1));
        }
        else
        {
            _frontAction.setEnabled(false);
            _backAction.setEnabled(false);
            _frontOneShotAction.setEnabled(false);
            _backOneShotAction.setEnabled(false);
        }
    }

    else
    {
        _frontAction.setEnabled(false);
        _backAction.setEnabled(false);
        _frontOneShotAction.setEnabled(false);
        _backOneShotAction.setEnabled(false);
        _frontAction.setText(_tr("ToFront"));
        _backAction.setText(_tr("ToBack"));
        _frontOneShotAction.setText(_tr("OnTop"));
        _backOneShotAction.setText(_tr("OnBottom"));
    }
}

void MainMenu::validationsReportRequested()
{
    LOG_INFO("Validations report requested.");
    QDesktopServices::openUrl(QUrl("file:///" + UiTools::getDocumentationDir() + "/validations.pdf"));
}

void MainMenu::helpRequested()
{
    LOG_INFO("Help requested.");
    QDesktopServices::openUrl(QUrl("file:///" + UiTools::getDocumentationDir() + "/documentation.pdf"));
}

void MainMenu::tutorialRequested()
{
    LOG_INFO("Tutorial requested.");
    QDesktopServices::openUrl(QUrl("file:///" + UiTools::getDocumentationDir() + "/tutorial.pdf"));
}

void MainMenu::quickStartRequested()
{
    LOG_INFO("Quickstart requested.");
    QDesktopServices::openUrl(QUrl("file:///" + UiTools::getDocumentationDir() + "/quickstart.pdf"));
}

void MainMenu::onOpenExample()
{
    QAction *action=dynamic_cast<QAction*>(sender());
    if (!action)
        return;

    // Get action text.
    QString filename=action->data().toString();
    if (filename.isEmpty())
        return;

    LOG_INFO("Open example: " << filename.toStdString());

    emit openExample(filename);
}

void MainMenu::onOpenProject()
{
    QAction *action=dynamic_cast<QAction*>(sender());
    if (!action)
        return;

    // Get action text.
    QString filename=action->text();
    if (filename.isEmpty())
        return;

    LOG_INFO("Open example: " << filename.toStdString());

    emit openProject(filename);
}

void MainMenu::updatePasteIcon()
{
    LOG_INFO("Update copy/paste icon.");

    _pasteAction.setEnabled(false);

    QClipboard *clipboard=QApplication::clipboard();
    if (!clipboard)
        return;

    const QMimeData *mimedata=clipboard->mimeData();
    if (!mimedata)
        return;

    if (!mimedata->hasFormat("conducteo/volumes"))
        return;

    if (StatesManager::instance()->simulationState()==StatesManager::InProgress)
        return;

    _pasteAction.setEnabled(true);
}

void MainMenu::cutRequested()
{
    LOG_INFO("Cut requested.");

    copyRequested();
    deleteSelectedVolumes();
}

void MainMenu::deleteSelectedVolumes()
{
    LOG_INFO("Delete selected volume.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.empty())
        return;

    DeleteVolumes *action=new DeleteVolumes(model, volumes);
    ActionsManager::instance()->exec(action);
}

void MainMenu::copyRequested()
{
    LOG_INFO("Copy requested.");

    QString serialization=serializeSelection();
    QMimeData *mimedata=new QMimeData;
    mimedata->setData("conducteo/volumes", serialization.toLatin1());
    QApplication::clipboard()->setMimeData(mimedata);
}

QString MainMenu::serializeSelection() const
{
    QString xml="<volumes>";
    
    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    for (unsigned int i=0 ; i<volumes.size() ; i++)
        xml+=volumes.at(i)->serialize().c_str();

    xml+="</volumes>";
    
    return xml;
}

void MainMenu::pasteRequested()
{
    LOG_INFO("Paste requested.");

    QClipboard *clipboard=QApplication::clipboard();
    if (!clipboard)
        return;

    const QMimeData *mimedata=clipboard->mimeData();
    if (!mimedata)
        return;

    if (!mimedata->hasFormat("conducteo/volumes"))
        return;

    QString data=mimedata->data("conducteo/volumes");
    importVolumesFromXml(data.toStdString());
}

void MainMenu::importVolumesFromXml(const std::string &xml)
{
    LOG_INFO("Import volume from Xml.");

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    // Unselect all previous selection.
    model->unselectAllVolumes();
    model->unselectAllAnnotations();
    model->unselectAllDxf();
    model->unselectAllImages();

    ImportVolumesFromXml *action=new ImportVolumesFromXml(model, xml);
    ActionsManager::instance()->exec(action);
}

void MainMenu::copy2DModelTo1DModel()
{
    LOG_INFO("Copy 2d model to 1d model.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    Copy2dModelTo1dModel *action=new Copy2dModelTo1dModel(model);
    ActionsManager::instance()->exec(action);

    StatesManager::instance()->repaintView();
}

void MainMenu::exceptionCaught(ErrorHandler::ErrorCode code)
{
    LOG_INFO("Exception caught.");

    QMessageBox dialog;
    dialog.setWindowTitle(_tr("SimulationError"));
    QString text=_tr("SimulationErrorText");

    if (code!=ErrorHandler::NoError)
        text+="<br/><br/><b>"+_tr("SimulationErrorDetails")+"</b><br/> "+QString(ErrorHandler::codeToString(code, LinguistManager::instance()->languageToString()).c_str())+".";
    dialog.setText(text);
    dialog.setWindowModality(Qt::ApplicationModal);
    QIcon icon(":/images/icon.png");
    dialog.setWindowIcon(icon);
    dialog.setIconPixmap(QPixmap(":/images/orange-blue/warning.png"));
    dialog.exec();
}

void MainMenu::showHideImagesAndDxfRequested()
{
    LOG_INFO("Show/hide images & Dxf requested.");

    if (StatesManager::instance()->showImages() || StatesManager::instance()->showDxf())
    {
        StatesManager::instance()->showImages(false);
        StatesManager::instance()->showDxf(false);
    }

    else
    {
        StatesManager::instance()->showImages(true);
        StatesManager::instance()->showDxf(true);
    }
}

void MainMenu::addPointModeRequested()
{
    LOG_INFO("Add point mode requested.");
    StatesManager::instance()->setInterfaceMode(StatesManager::AddPoint);
}

void MainMenu::toogleEn13370()
{
    LOG_INFO("Toogle EN 13370.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    if (model->en13370Plugin())
        model->removeEn13370Plugin();
    else
        model->createEn13370Plugin();

    StatesManager::instance()->changed();
}

void MainMenu::getLengthRequested()
{
    LOG_INFO("Get length requested.");
    StatesManager::instance()->setInterfaceMode(StatesManager::AddLengthCotation);
}

void MainMenu::getAngleRequested()
{
    LOG_INFO("Get angle requested.");
    StatesManager::instance()->setInterfaceMode(StatesManager::AddAngleCotation);
}

void MainMenu::openPreferencesRequested()
{
    LOG_INFO("On preferences requested.");

    _preferences.updateContent();
    _preferences.updateCategoriesList();
    _preferences.setVisible(true);
}

void MainMenu::updateLicense()
{
    LOG_INFO("Update license.");
    _preferences.updateContent();
}

void MainMenu::addTextRequested()
{
    LOG_INFO("Add text requested.");
    StatesManager::instance()->setInterfaceMode(StatesManager::AddText);
}

void MainMenu::addArrowRequested()
{
    LOG_INFO("Add arrow requested.");
    StatesManager::instance()->setInterfaceMode(StatesManager::AddArrow);
}

void MainMenu::setThermalBridgeType()
{
    LOG_INFO("Set thermal bridge type.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;
    model->setComputationType(Model::ThermalBridge);
    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();
    ActionsManager::instance()->modelAltered();
    ActionsManager::instance()->modelResultsAltered();
}

void MainMenu::setThermalFluxType()
{
    LOG_INFO("Set thermal flux type.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;
    model->setComputationType(Model::Flux);
    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->setViewType(StatesManager::DisplayModel);
    StatesManager::instance()->setInterfaceMode(StatesManager::Selection);
    StatesManager::instance()->changed();
    ActionsManager::instance()->modelAltered();
    ActionsManager::instance()->modelResultsAltered();
}

void MainMenu::setThermalTransmissionType()
{
    LOG_INFO("Set thermal transmission type.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;
    model->setComputationType(Model::ThermalTransmission);
    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();
    ActionsManager::instance()->modelAltered();
    ActionsManager::instance()->modelResultsAltered();
}

void MainMenu::setThermalConductivityType()
{
    LOG_INFO("Set thermal conductivity type.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    model->setComputationType(Model::EquivalentThermalConductivity);

    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();

    StatesManager::instance()->setViewType(StatesManager::DisplayModel);
    StatesManager::instance()->setInterfaceMode(StatesManager::Selection);

    ActionsManager::instance()->modelAltered();
    ActionsManager::instance()->modelResultsAltered();
}

void MainMenu::setCondensationType()
{
    LOG_INFO("Set Glaser condensation type.");

    Model *model = StatesManager::instance()->currentModel();
    if (!model)
        return;

    model->setComputationType(Model::GlaserCondensation);

    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();

    StatesManager::instance()->setViewType(StatesManager::DisplayModel);
    StatesManager::instance()->setInterfaceMode(StatesManager::Selection);

    ActionsManager::instance()->modelAltered();
    ActionsManager::instance()->modelResultsAltered();
}

void MainMenu::setLengthFull()
{
    LOG_INFO("Set length full.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;
    model->setTransmissionCoefficientLengthType(Model::FullLength);
    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();
}

void MainMenu::setLengthX()
{
    LOG_INFO("Set length X.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;
    model->setTransmissionCoefficientLengthType(Model::ProjectedXLength);
    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();
}

void MainMenu::setLengthY()
{
    LOG_INFO("Set length Y.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;
    model->setTransmissionCoefficientLengthType(Model::ProjectedYLength);
    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();
}

void MainMenu::setLengthCustom()
{
    LOG_INFO("Set length custom.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    bool ok;
    double length=QInputDialog::getDouble(this, _tr("Length"), _tr("LengthMM"), model->transmissionCoefficientLength()*1000.0, 0.0, 10000.0, 3, &ok);

    if (ok && length>0.0)
        model->setTransmissionCoefficientLength(length/1000.0);

    model->setTransmissionCoefficientLengthType(Model::CustomLength);
    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();
}

void MainMenu::setCustomWidth()
{
    LOG_INFO("Set custom width.");

    Model *model = StatesManager::instance()->currentModel();
    if (!model)
        return;

    bool ok;
    double width = QInputDialog::getDouble(this, _tr("Width"), _tr("WidthMM"), model->getEquivalentConductivityWidth() * 1000.0, 0.0, 10000.0, 3, &ok);

    if (ok && width > 0.0)
        model->setEquivalentConductivityWidth(width / 1000.0);

    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();
}

void MainMenu::setCustomThickness()
{
    LOG_INFO("Set custom thickness.");

    Model *model = StatesManager::instance()->currentModel();
    if (!model)
        return;

    bool ok;
    double thickness = QInputDialog::getDouble(this, _tr("Thickness"), _tr("ThicknessMM"), model->getEquivalentConductivityThickness() * 1000.0, 0.0, 10000.0, 3, &ok);

    if (ok && thickness > 0.0)
        model->setEquivalentConductivityThickness(thickness / 1000.0);

    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();
}

void MainMenu::setCustomResistances()
{
    LOG_INFO("Set custom superficial resistances.");

    Model *model = StatesManager::instance()->currentModel();
    if (!model)
        return;

    bool ok;
    double res = QInputDialog::getDouble(this, _tr("SuperResistances"), _tr("SuperResistancesWithUnit"), model->getEquivalentConductivityResistanceSum(), 0.0, 10000.0, 3, &ok);

    if (ok && res >= 0.0)
        model->setEquivalentConductivityResistanceSum(res);

    StatesManager::instance()->setSavedModel(false);
    StatesManager::instance()->changed();
}

void MainMenu::clearAllBoundaryConditions()
{
    LOG_INFO("Unset all boundary conditions requested.");

    Model *model = StatesManager::instance()->currentModelView();
    if (!model)
        return;

    UnsetAllBoundaryConditions *action = new UnsetAllBoundaryConditions(model);
    ActionsManager::instance()->exec(action);
}

void MainMenu::clearAllEnvironments()
{
    LOG_INFO("Unset all environments requested.");

    Model *model = StatesManager::instance()->currentModelView();
    if (!model)
        return;

    UnsetAllEnvironments *action = new UnsetAllEnvironments(model);
    ActionsManager::instance()->exec(action);
}

void MainMenu::updateSubMenus()
{
    // View menu.
    _viewMenu.clear();
    _viewMenu.addAction(&_fitAction);
    _viewMenu.addAction(&_zoomInAction);
    _viewMenu.addAction(&_zoomOutAction);
    _viewMenu.addSeparator();
    _viewMenu.addAction(&_imagesVisibilityAction);
    _viewMenu.addAction(&_dxfVisibilityAction);

    // Project menu.
    _projectMenu.clear();

    // Projet type.
    _modelTypeMenu.clear();
    _modelTypeMenu.addAction(&_modelTypeThermalBridgeAction);
    _modelTypeMenu.addAction(&_modelTypeThermalFluxAction);
    _modelTypeMenu.addAction(&_modelTypeThermalTransmissionAction);
    _modelTypeMenu.addAction(&_modelTypeThermalConductivityAction);
    _modelTypeMenu.addSeparator();
    _modelTypeMenu.addAction(&_modelTypeGlaserCondensationAction);

    _modelTypeAction.setMenu(&_modelTypeMenu);

    _projectMenu.addAction(&_modelTypeAction);
    _projectMenu.addAction(&_modelLengthAction);
    _projectMenu.addAction(&_modelEqConductivityAction);
    _projectMenu.addSeparator();
    _projectMenu.addAction(&_addImageAction);
    _projectMenu.addAction(&_addDxfAction);

    _projectMenu.addSeparator();
    _projectMenu.addAction(&_modelClearAllBc);
    _projectMenu.addAction(&_modelClearAllEnv);

    // Model toolbar.
    _modelToolBar.clear();
    _modelToolBar.setIconSize(QSize(32, 32));
    _modelToolBar.setMovable(false);
    _modelToolBar.addAction(&_selectionModeAction);
    _modelToolBar.addAction(&_addRectangleModeAction);

    _modelToolBar.addAction(&_addPolyline);
    _modelToolBar.addAction(&_addCircleModeAction);
    _modelToolBar.addAction(&_addEllipseModeAction);
    _modelToolBar.addSeparator();
    _modelToolBar.addAction(&_addPoint);
    _modelToolBar.addSeparator();
    _modelToolBar.addAction(&_addImageAction);
    _modelToolBar.addAction(&_addDxfAction);

    _modelToolBar.addSeparator();
    _modelToolBar.addAction(&_getMaterialModeAction);
    _modelToolBar.addAction(&_setPhysicModeAction);
    _modelToolBar.addAction(&_setEnvironmentModeAction);

    _modelToolBar.addSeparator();
    _modelToolBar.addAction(&_addText);
    _modelToolBar.addAction(&_addArrow);
    _modelToolBar.addAction(&_getLength);
    _modelToolBar.addAction(&_getAngle);

    // Image toolbar.
    _imagesToolBar.clear();
    _imagesToolBar.setIconSize(QSize(32, 32));
    _imagesToolBar.setMovable(false);
    _imagesToolBar.addAction(&_imagesVisibilityAction);
    _imagesToolBar.addAction(&_dxfVisibilityAction);

    _imagesToolBar.setVisible(true);

    loadExamples();
    statesChanged();
}
