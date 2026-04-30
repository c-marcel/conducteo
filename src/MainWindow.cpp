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

#include <MainWindow.h>
#include <actions/model/ImportVolumesFromXml.h>
#include <actions/ActionsManager.h>
#include <errors/ErrorHandler.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <LinguistManager.h>
#include <QDesktopServices>
#include <StatesManager.h>
#include <tools/UiTools.h>
#include <model/Model.h>
#include <QApplication>
#include <LocalData.h>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <Polyline.h>
#include <QMimeData>
#include <Layer.h>
#include <Zoom.h>
#include <Log.h>
#include <Dxf.h>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    _menu(this),
    _centralWidget(new QWidget(this)),
    _grid(_centralWidget),
    _mouseCursor(this),
    _temperatures(this),
    _humidity(this),
    _surfaceProperties(this),
    _environmentsList(this),
    _volumeProperties(this),
    _materialsList(this),
    _boundaryConditionsList(this),
    _tabs(this),
    _horizontalScale(this),
    _verticalScale(this),
    _modelViewer(this),
    _report(this),
    _computationReporter(this),
    _validationsShortCut(this)
{
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowIcon(QIcon(":/images/icon.png"));

    // Initialize linguist manager.
    std::string current_language=LocalData::instance()->getParameter("language");
    if (current_language=="english")
        LinguistManager::instance()->changeLanguage(LinguistManager::English);

    // Layouts.
    _modelGrid.setContentsMargins(0, 0, 0, 0);
    _modelGrid.addWidget(&_horizontalScale, 0, 1, 1, 1);
    _modelGrid.addWidget(&_verticalScale, 1, 0, 1, 1);
    _modelGrid.addWidget(&_modelViewer, 1, 1, 1, 1);
    _modelGrid.addWidget(&_report, 1, 1, 1, 1);

    _listLayout.setContentsMargins(0, 0, 0, 0);
    _listLayout.addWidget(&_mouseCursor);
    _listLayout.addWidget(&_materialsList);

    _listLayout.setStretchFactor(&_materialsList, 10);

    _listLayout.addWidget(&_boundaryConditionsList);
    _listLayout.addWidget(&_environmentsList);
    _listLayout.addWidget(&_temperatures);
    _listLayout.addWidget(&_humidity);
    _listLayout.addWidget(&_surfaceProperties);
    _listLayout.addWidget(&_volumeProperties);
    QSpacerItem *vS1=new QSpacerItem(300, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    _listLayout.addItem(vS1);

    _tabsLayout.addWidget(&_tabs);
    _tabsLayout.addLayout(&_modelGrid);

    _grid.setSpacing(0);
    _grid.setContentsMargins(0, 0, 0, 0);
    _grid.addLayout(&_listLayout, 0, 0, 1, 1);
    _grid.addLayout(&_tabsLayout, 0, 1, 1, 1);
    _grid.addWidget(&_computationReporter, 0, 1, 1, 1);
    _grid.addWidget(&_psiReporter, 0, 1, 1, 1);
    _grid.setAlignment(&_computationReporter, Qt::AlignCenter);
    _grid.setAlignment(&_psiReporter, Qt::AlignLeft | Qt::AlignBottom);

    setCentralWidget(_centralWidget);

    // Mouse position.
    connect(&_modelViewer,
            &ModelViewer::mousePositionChanged,
            &_mouseCursor,
            &MouseCursorInfos::setPosition,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::hideMousePosition,
            &_mouseCursor,
            &MouseCursorInfos::unSetPosition,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::mouseDimensionChanged,
            &_mouseCursor,
            &MouseCursorInfos::setDimension,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::hideMouseDimension,
            &_mouseCursor,
            &MouseCursorInfos::unSetDimension,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::mouseScreenPositionChanged,
            &_verticalScale,
            &Scale::mouseScreenPositionChanged,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::mouseScreenPositionChanged,
            &_horizontalScale,
            &Scale::mouseScreenPositionChanged,
            Qt::UniqueConnection);

    // Temperature update.
    connect(&_modelViewer,
            &ModelViewer::unSetTemperature,
            &_temperatures,
            &TemperatureDisplayer::unSetTemperature,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::setTemperature,
            &_temperatures,
            &TemperatureDisplayer::setTemperature,
            Qt::UniqueConnection);

    // Humidity update.
    connect(&_modelViewer,
            &ModelViewer::unSetHumidity,
            &_humidity,
            &HumidityDisplayer::unSetHumidity,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::setHumidity,
            &_humidity,
            &HumidityDisplayer::setHumidity,
            Qt::UniqueConnection);

    // Model changes.
    connect(&_modelViewer,
            &ModelViewer::materialUsed,
            &_materialsList,
            &MaterialList::setMaterialUsed,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::selectMaterialByIdRequested,
            &_materialsList,
            &MaterialList::selectMaterialById,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::copyRequested,
            &_menu,
            &MainMenu::copyRequested,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::cutRequested,
            &_menu,
            &MainMenu::cutRequested,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::deleteRequested,
            &_menu,
            &MainMenu::deleteRequested,
            Qt::UniqueConnection);
    connect(&_modelViewer,
            &ModelViewer::rotationRequested,
            &_menu,
            &MainMenu::rotationRequested,
            Qt::UniqueConnection);

    // Computation reporter.
    _menu.connectReporter(&_computationReporter);
    connect(&_computationReporter,
            &ComputationReporter::computationAbortRequested,
            &_menu,
            &MainMenu::abortComputation,
            Qt::UniqueConnection);

    // Validations.
    _validationsShortCut.setKey(Qt::CTRL|Qt::SHIFT|Qt::Key_V);
    connect(&_validationsShortCut,
            &QShortcut::activated,
            this,
            &MainWindow::validateSoftware,
            Qt::UniqueConnection);

    // Menus.
    connect(&_menu, &MainMenu::newRequested, this, &MainWindow::newRequested, Qt::UniqueConnection);
    connect(&_menu, &MainMenu::openRequested, this, &MainWindow::openRequested, Qt::UniqueConnection);
    connect(&_menu, &MainMenu::saveRequested, this, &MainWindow::saveRequested, Qt::UniqueConnection);
    connect(&_menu,
            &MainMenu::aboutRequested,
            this,
            &MainWindow::aboutRequested,
            Qt::UniqueConnection);
    connect(&_menu,
            &MainMenu::saveAsRequested,
            this,
            &MainWindow::saveAsRequested,
            Qt::UniqueConnection);
    connect(&_menu, &MainMenu::exitRequested, this, &MainWindow::exitRequested, Qt::UniqueConnection);
    connect(&_menu,
            &MainMenu::fitViewRequested,
            this,
            &MainWindow::fitViewRequested,
            Qt::UniqueConnection);
    connect(&_menu,
            &MainMenu::zoomInRequested,
            this,
            &MainWindow::zoomInRequested,
            Qt::UniqueConnection);
    connect(&_menu,
            &MainMenu::zoomOutRequested,
            this,
            &MainWindow::zoomOutRequested,
            Qt::UniqueConnection);
    connect(&_menu, &MainMenu::openExample, this, &MainWindow::openExample, Qt::UniqueConnection);
    connect(&_menu, &MainMenu::openProject, this, &MainWindow::openProject, Qt::UniqueConnection);

    // Show window.
    updateWindowTitle();

    // Open new project by default.
    showMaximized();
    newRequested();

    // Show quick start if needed.
    if (LocalData::instance()->getParameter("show-quickstart-tutorial") == "true")
        showQuickStart();
}

MainWindow::~MainWindow()
{
}

void MainWindow::upgradeStateChanged()
{
    LOG_INFO("Upgrade state changed.");

    if (StatesManager::instance()->upgradeState()!=StatesManager::UpgradeInProgress)
        return;

    if (exitRequested())
        return;

    // User wont to exit.
    StatesManager::instance()->setUpgradeState(StatesManager::DownloadDone);
}

void MainWindow::translate()
{
    updateWindowTitle();
}

QString MainWindow::escapeString(const QString &text) const
{
    QString out = text.toLower();

    out = out.replace( QRegularExpression("[éèëê]"), "e");
    out = out.replace( QRegularExpression("[âàäáãå]"), "a");
    out = out.replace( QRegularExpression("[ç]"), "c");
    out = out.replace( QRegularExpression("[îïíì]"), "i");
    out = out.replace( QRegularExpression("[ñ]"), "n");
    out = out.replace( QRegularExpression("[ôöóòõø]"), "o");
    out = out.replace( QRegularExpression("[ß]"), "ss");
    out = out.replace( QRegularExpression("[ûüúù]"), "u");
    out = out.replace( QRegularExpression("[æ]"), "ae");

    return out;
}

bool polyline_sort(DxfReader::Polyline *line1, DxfReader::Polyline *line2)
{
    return line1->zOrder() < line2->zOrder();
}

void MainWindow::importDxfContent(const std::string &filename)
{
    // Get category id.
    std::string categoryId = LocalData::instance()->getParameter("material-dxf-category");

    if (categoryId == "")
    {
        StatesManager::instance()->deleteCurrentModel();
        StatesManager::instance()->createModel();
        displayErrorMessage(_tr("DxfImportError"), _tr("NoMaterialCategoryForDxf"), 0);

        return;
    }

    // Get materials.
    std::vector<Material*> materials = Materials::instance()->materialListFromCategory(categoryId);

    if (materials.size() == 0)
    {
        StatesManager::instance()->deleteCurrentModel();
        StatesManager::instance()->createModel();
        displayErrorMessage(_tr("DxfImportError"), _tr("EmptyMatCatForDxf"), 0);

        return;
    }

    // Create internal table: RGB as long => material id.
    std::map<unsigned long, std::string> internalMaterialsMap;

    std::string defaultMaterialId   = "";
    QString     defaultMaterialName = "";

    for (unsigned int i = 0; i < materials.size(); i++)
    {
        Material *material = materials[i];
        if (!material)
            continue;

        unsigned long red = material->red();
        unsigned long green = material->green();
        unsigned long blue = material->blue();

        red = red << 16;
        green = green << 8;

        unsigned long color = red + green + blue;
        
        internalMaterialsMap[color] = material->id();

        // Store first material.
        QString materialName = escapeString(material->name().c_str());
        if (materialName < defaultMaterialName || defaultMaterialName == "")
        {
            defaultMaterialName = materialName;
            defaultMaterialId   = material->id();
        }
    }

    // Load DXF file content.
    DxfReader::Dxf dxf;
    dxf.loadFromFile(filename);

    if (dxf.size() == 0)
    {
        StatesManager::instance()->deleteCurrentModel();
        StatesManager::instance()->createModel();
        displayErrorMessage(_tr("DxfImportError"), _tr("DxfHasNoLayer"), 0);

        return;
    }

    int nbOfVolumesImported = 0;

    // Create polyline list.
    std::vector<DxfReader::Polyline *> polylines;

    for (unsigned int i = 0; i < dxf.size(); i++)
    {
        DxfReader::Layer *layer = dxf[i];

        for (unsigned int j = 0; j < layer->size(); j++)
        {
            DxfReader::Entity *entity = layer->at(j);
            if (!entity)
                continue;

            DxfReader::Polyline *line = dynamic_cast<DxfReader::Polyline *>(entity);
            if (!line)
                continue;

            polylines.push_back(line);
        }
    }

    // Sort by z-order.
    std::sort(polylines.begin(), polylines.end(), polyline_sort);


    // Import all volumes.
    for (unsigned int i = 0; i < polylines.size(); i++)
    {
        DxfReader::Polyline *line = polylines.at(i);

        // Read color.
        unsigned long red = line->redColor();
        unsigned long green = line->greenColor();
        unsigned long blue = line->blueColor();

        red = red << 16;
        green = green << 8;

        unsigned long color = red + green + blue;

        // Get material id.
        std::string materialId = internalMaterialsMap[color];
        if (materialId == "")
            materialId = defaultMaterialId;

        // Create dxf volume XML stream.
        std::string xml = "<volumes>";
        xml += "<volume>";
        xml += "<material ref=\"" + materialId + "\" />";
        xml += "<cells>";
        xml += "<cell-size>0.025</cell-size>";
        xml += "</cells>";
        xml += "<paths>";
        xml += line->toConducteoElement();
        xml += "</paths>";
        xml += "</volume>";
        xml += "</volumes>";

        // Import volume.
        ImportVolumesFromXml *action = new ImportVolumesFromXml(StatesManager::instance()->currentModel(), xml, false);
        ActionsManager::instance()->exec(action);

        if (!action->canceledAction())
            nbOfVolumesImported++;
    }

    if (nbOfVolumesImported == 0)
    {
        StatesManager::instance()->deleteCurrentModel();
        StatesManager::instance()->createModel();
        displayErrorMessage(_tr("DxfImportError"), _tr("DxfHasNoValidPolyline"), 0);

        return;
    }
}

void MainWindow::loadModel(const std::string &filename)
{
    LOG_INFO("Load model: " << filename);

    // Release current model.
    StatesManager::instance()->deleteCurrentModel();

    _menu.addLastProject(filename.c_str());

    // Create new model.
    StatesManager::instance()->createModel();
    Model *model = StatesManager::instance()->currentModel();
    if (!model)
        return;

    // Check for DXF file import.
    QFileInfo info(filename.c_str());
    QString suffix = info.suffix().toLower();

    if (suffix == "dxf")
    {
        QString f = QString::fromUtf8(filename.c_str());

        // Windows support only.
        importDxfContent(f.toLatin1().constData());
    }

    // Default: c2d or THM.
    else
    {
        // Read model content.
        QFile file(filename.c_str());
        if (!file.open(QIODevice::ReadOnly))
            return;

        QByteArray data = file.readAll();
        file.close();

        std::string xml;
        for (int i = 0; i < data.size(); i++)
            xml.push_back(data.at(i));

        ErrorHandler error;
        model->setMaterialImportCategory(LocalData::instance()->getParameter("material-import-category"));
        model->fromXml(xml, &error);

        // Catch for errors.
        if (error.code() != ErrorHandler::NoError)
        {
            LOG_INFO("c2d file contains some errors.");

            StatesManager::instance()->deleteCurrentModel();
            StatesManager::instance()->createModel();
            displayErrorMessage(_tr("BadFile"), _tr("BadFileDetails"), &error);
        }
        else
        {
            if (model->modelType() == Model::ConducteoFile)
                StatesManager::instance()->setModelFilename(filename);
        }

        _environmentsList.updateDescriptions();
    }

    updateWindowTitle();
    fitViewRequested();
}

void MainWindow::displayErrorMessage(const QString &title, const QString &message, ErrorHandler *error)
{
    LOG_INFO("Error message: " << message.toStdString());

    QMessageBox dialog;
    dialog.setWindowTitle(title);
    QString text=message;

    if (error && error->code()!=ErrorHandler::NoError)
        text+="<br/><br/><b>"+_tr("DetailsError")+"</b><br/> "+QString(ErrorHandler::codeToString(error->code(), LinguistManager::instance()->languageToString()).c_str())+".";
    dialog.setText(text);
    dialog.setWindowModality(Qt::ApplicationModal);
    QIcon icon(":/images/icon.png");
    dialog.setWindowIcon(icon);
    dialog.setIconPixmap(QPixmap(":/images/icon.png"));
    dialog.exec();
}

void MainWindow::aboutRequested()
{
    QMessageBox dialog;
    dialog.setWindowTitle(_tr("AboutSoftware"));
    QString text="<b>conducteo version "+UiTools::getSoftwareVersion()+"</b><br/><br/>";
    text+=_tr("AboutText1")+"<br/><br/>";
    text+=_tr("AboutText2")+"<br/><br/>";
    text+="&nbsp;&nbsp;&nbsp;• "+_tr("AboutText3")+"<br/>";
    text+="&nbsp;&nbsp;&nbsp;• "+_tr("AboutText4")+"<br/>";
    text+="&nbsp;&nbsp;&nbsp;• "+_tr("AboutText5")+"<br/>";
    text+="&nbsp;&nbsp;&nbsp;• "+_tr("AboutText6")+"<br/>";
    text+="&nbsp;&nbsp;&nbsp;• "+_tr("AboutText7")+"<br/><br/>";
    text+=_tr("AboutText8");
    text+=_tr("AboutText9");
    text+="<br/>Copyright 2009-2026";
    dialog.setText(text);
    dialog.setWindowModality(Qt::ApplicationModal);
    QIcon icon(":/images/icon.png");
    dialog.setWindowIcon(icon);
    dialog.setIconPixmap(QPixmap(":/images/icon.png"));
    dialog.exec();
}

void MainWindow::updateWindowTitle()
{
    QString title = _tr("SoftwareTitle") + UiTools::getSoftwareVersion();
    std::string filename=StatesManager::instance()->modelFilename();
    if (!filename.empty())
    {
        QString file=filename.c_str();
#ifdef WIN32
        file.replace("/", "\\");
#endif
        title+=" - "+file;
    }
    setWindowTitle(title);
}

void MainWindow::newRequested()
{
    LOG_INFO("New model requested.");

    if (!userAcceptToStopSimulation())
        return;

    if (!userAcceptToCloseCurrentModel())
        return;

    if (userWantToRecalculateModel())
        return _menu.computationRequested();

    // Release current model & create new empty model.
    LOG_INFO("Release current model & create new one.");

    StatesManager::instance()->deleteCurrentModel();
    StatesManager::instance()->createModel();
    StatesManager::instance()->setModelFilename("");
    updateWindowTitle();
    fitViewRequested();
    _environmentsList.updateDescriptions();
}

void MainWindow::openRequested()
{
    LOG_INFO("Open requested.");

    if (!userAcceptToStopSimulation())
        return;

    if (!userAcceptToCloseCurrentModel())
        return;

    if (userWantToRecalculateModel())
        return _menu.computationRequested();

    // Get last used path.
    QString path=LocalData::instance()->getParameter("last-path").c_str();

    QString filetype=_tr("OpenModelFilePattern");

    QString filename=QFileDialog::getOpenFileName(this, _tr("OpenModel"), path, filetype);
    if (filename.isEmpty())
        return;

    // Save choosen directory.
    QFileInfo info(filename);
    LocalData::instance()->setParameter("last-path", info.absolutePath().toStdString());
    
    loadModel(filename.toStdString());
}

bool MainWindow::userAcceptToStopSimulation()
{
    if (StatesManager::instance()->simulationState()!=StatesManager::InProgress)
        return true;

    QMessageBox dialog;
    dialog.setWindowTitle(_tr("ComputationInProgress"));
    dialog.setText(_tr("ComputationStop"));
    dialog.setWindowModality(Qt::ApplicationModal);
    dialog.setIcon(QMessageBox::Question);
    QAbstractButton *yes=dialog.addButton(_tr("Yes"), QMessageBox::YesRole);
    QAbstractButton *no=dialog.addButton(_tr("No"), QMessageBox::NoRole);
    QIcon icon(":/images/icon.png");
    dialog.setWindowIcon(icon);
    dialog.exec();

    if (dialog.clickedButton()==yes)
    {
        LOG_INFO("User want to abort computation.");
        _menu.abortComputation();
        return true;
    }
    if (dialog.clickedButton()==no)
    {
        LOG_INFO("User wont to abort comutation.");
        return false;
    }

    LOG_INFO("User wont to abort comutation.");
    return false;
}

bool MainWindow::userWantToRecalculateModel()
{
#ifdef __APPLE__
    return false;
#endif

    if (LocalData::instance()->getParameter("compute-when-exiting") == "false")
        return false;

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return false;

    if (model->isEmpty())
        return false;

    if (model->validResults())
        return false;

    QMessageBox dialog;
    dialog.setWindowTitle(_tr("NotComputedModel"));
    dialog.setText(_tr("RecomputeModel"));
    dialog.setWindowModality(Qt::ApplicationModal);
    dialog.setIcon(QMessageBox::Question);
    QAbstractButton *yes=dialog.addButton(_tr("Yes"), QMessageBox::YesRole);
    QAbstractButton *no=dialog.addButton(_tr("No"), QMessageBox::NoRole);
    QIcon icon(":/images/icon.png");
    dialog.setWindowIcon(icon);
    dialog.exec();

    if (dialog.clickedButton()==no)
    {
        LOG_INFO("User want to recalculate comutation.");
        return false;
    }

    LOG_INFO("User wont to recalculate comutation.");
    return true;
}

bool MainWindow::userAcceptToCloseCurrentModel()
{
    if (StatesManager::instance()->saved())
        return true;

    QMessageBox dialog;
    dialog.setWindowTitle(_tr("UnsavedModel"));
    dialog.setText(_tr("SaveModel"));
    dialog.setWindowModality(Qt::ApplicationModal);
    dialog.setIcon(QMessageBox::Question);
    QAbstractButton *yes=dialog.addButton(_tr("Yes"), QMessageBox::YesRole);
    QAbstractButton *no=dialog.addButton(_tr("No"), QMessageBox::NoRole);
    QAbstractButton *cancel=dialog.addButton(_tr("Cancel"), QMessageBox::RejectRole);
    QIcon icon(":/images/icon.png");
    dialog.setWindowIcon(icon);
    dialog.exec();

    if (dialog.clickedButton()==cancel)
    {
        LOG_INFO("User wont to close current model.");
        return false;
    }
    if (dialog.clickedButton()==yes)
        saveRequested();

    LOG_INFO("User want to close current model.");
    return true;
}

void MainWindow::saveRequested()
{
    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    _report.updateExportedData();

    ErrorHandler error;
    std::string xml = model->toXml_V3(&error);

    if (error.code()!=ErrorHandler::NoError)
    {
        LOG_INFO("Error while saving model. Error code: " << error.code());
        StatesManager::instance()->deleteCurrentModel();
        displayErrorMessage(_tr("SaveError"), _tr("SaveErrorDetails"), &error);
        return;
    }

    if (StatesManager::instance()->modelFilename().empty())
        return saveAsRequested();

    std::string filename=StatesManager::instance()->modelFilename();
    QFile file(filename.c_str());
    if (!file.open(QIODevice::WriteOnly))
        return;

    file.write(xml.c_str());
    file.close();

    StatesManager::instance()->setSavedModel(true);
}

void MainWindow::saveAsRequested()
{
    // Get last used path.
    QString path=LocalData::instance()->getParameter("last-path").c_str();

    QFileDialog fileDialog(this, _tr("SaveAs"));
    fileDialog.setNameFilter("conducteo (*.c2d)");
    fileDialog.setDefaultSuffix("c2d");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    if(!fileDialog.exec())
        return;

    QString f=fileDialog.selectedFiles().first();
    if (f.isEmpty())
        return;

    // Save choosen directory.
    QFileInfo info(f);
    LocalData::instance()->setParameter("last-path", info.absolutePath().toStdString());

    // conducteö file format.
    if (info.suffix().toLower()=="c2d")
    {
        // Add into last project list.
        _menu.addLastProject(f);
        StatesManager::instance()->setModelFilename(f.toStdString());
        updateWindowTitle();
        saveRequested();
    }

    // THERM 7.3 file format.
    else if (info.suffix().toLower()=="thm")
    {
        Model *model=StatesManager::instance()->currentModel();
        if (!model)
            return;

        std::string content=model->toThermFile(*model, *model);

        QFile file(f);
        if (!file.open(QIODevice::WriteOnly))
            return;

        file.write(content.c_str(), content.length());
        file.close();
    }
}

bool MainWindow::exitRequested()
{
    LOG_INFO("Exit requested.");

    if (!userAcceptToStopSimulation())
        return false;
    if (!userAcceptToCloseCurrentModel())
        return false;
    if (userWantToRecalculateModel())
    {
        _menu.computationRequested();
        return false;
    }

    LOG_INFO("Quit application.");
    QApplication::instance()->quit();
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    LOG_INFO("Close event caught.");

    exitRequested();
    event->ignore();
}

void MainWindow::fitViewRequested()
{
    LOG_INFO("Fit view requested.");


    // Process events to ensure widgets has been drawn at least one time
    // because fitted zoom is computed with widget size.
    QApplication::instance()->processEvents();

    Zoom *zoom=StatesManager::instance()->zoom();
    if (!zoom)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    int w=_modelViewer.width();
    int h=_modelViewer.height();

    double xmin=model->xMin(StatesManager::instance()->showImages(), StatesManager::instance()->showDxf());
    double xmax=model->xMax(StatesManager::instance()->showImages(), StatesManager::instance()->showDxf());
    double ymin=model->yMin(StatesManager::instance()->showImages(), StatesManager::instance()->showDxf());
    double ymax=model->yMax(StatesManager::instance()->showImages(), StatesManager::instance()->showDxf());

    zoom->fitTo(xmin, ymin, xmax, ymax, w, h);
}

void MainWindow::zoomInRequested()
{
    Zoom *zoom=StatesManager::instance()->zoom();
    if (!zoom)
        return;

    QPoint center;
    int w=_modelViewer.width();
    int h=_modelViewer.height();
    center.setX(w/2.0);
    center.setY(h/2.0);

    zoom->increase(center);
}

void MainWindow::zoomOutRequested()
{
    Zoom *zoom=StatesManager::instance()->zoom();
    if (!zoom)
        return;

    QPoint center;
    int w=_modelViewer.width();
    int h=_modelViewer.height();
    center.setX(w/2.0);
    center.setY(h/2.0);

    zoom->decrease(center);
}

void MainWindow::validateSoftware()
{
    LOG_INFO("Start software validation thread.");
    _validationsThread.start();
}

void MainWindow::showQuickStart()
{
    QMessageBox dialog(this);
    dialog.setWindowTitle(_tr("FirstLaunch"));
    dialog.setText(_tr("FirstLaunchQuestion"));

    QPushButton *yes = dialog.addButton(_tr("Yes"), QMessageBox::AcceptRole);
    QPushButton *no  = dialog.addButton(_tr("No"), QMessageBox::RejectRole);

    QCheckBox *checkbox = new QCheckBox;
    checkbox->setText(_tr("FirstLaunchQuestionHide"));
    dialog.setCheckBox(checkbox);

    int choice = dialog.exec();

    // Store parameter.
    if (checkbox->isChecked())
        LocalData::instance()->setParameter("show-quickstart-tutorial", "false");

    if (choice == QMessageBox::AcceptRole)
    {
        LOG_INFO("Quick start guide requested.");
        QDesktopServices::openUrl(QUrl("file:///" + UiTools::getDocumentationDir() +"/quickstart.pdf"));
    }
}

void MainWindow::openExample(const QString &filename)
{
    LOG_INFO("Open example requested.");

    if (!userAcceptToStopSimulation())
        return;

    if (!userAcceptToCloseCurrentModel())
        return;

    if (userWantToRecalculateModel())
        return _menu.computationRequested();

    std::string f = UiTools::getExamplesDir().toStdString() + "/" + filename.toStdString();
    loadModel(f);
}

void MainWindow::openProject(const QString &filename)
{
    LOG_INFO("Open project requested.");

    if (!userAcceptToStopSimulation())
        return;

    if (!userAcceptToCloseCurrentModel())
        return;

    if (userWantToRecalculateModel())
        return _menu.computationRequested();

    std::string f=filename.toStdString();
    loadModel(f);
}

MainWindow::DropFileType MainWindow::acceptDrop(QDropEvent *event) const
{
    const QMimeData *mimedata=event->mimeData();
    if (!mimedata)
        return UnknownType;

    if (!mimedata->hasUrls())
        return UnknownType;

    QList<QUrl> urls=mimedata->urls();
    if (urls.size()!=1)
        return UnknownType;

    QString data=urls.at(0).toString();
    data=data.toLower();

    if (data.contains(".c2d"))
        return ModelType;
    if (data.contains(".thm"))
        return ImportModelType;
    if (data.contains(".db"))
        return MaterialDbType;
    if (data.contains(".dxf"))
        return DxfFileType;
    if (data.contains(".bmp") || data.contains(".png") || data.contains(".jpg") || data.contains(".jpeg") || data.contains(".gif"))
        return ImageType;

    return UnknownType;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (acceptDrop(event)==UnknownType)
        return;

    event->setDropAction(Qt::ActionMask);
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    DropFileType type=acceptDrop(event);
    if (type==UnknownType)
        return;

    const QMimeData *mimedata=event->mimeData();
    QList<QUrl> urls=mimedata->urls();
    QString file=urls.at(0).toLocalFile();

    // Load model.
    if (type==ModelType || type==ImportModelType)
    {
        if (!userAcceptToStopSimulation())
            return;

        if (!userAcceptToCloseCurrentModel())
            return;

        if (userWantToRecalculateModel())
            return _menu.computationRequested();
    
        loadModel(file.toStdString());
    }

    // Import materials db.
    else if (type==MaterialDbType)
    {
        _materialsList.importMaterialsDatabase(file);
    }

    // Import image.
    else if (type==ImageType)
    {
        _menu.addImage(file);
    }

    // Import Dxf file.
    else if (type==DxfFileType)
    {
        _menu.addDxf(file);
    }
}

std::vector<MaterialCategory *> MainWindow::categories() const
{
    return _materialsList.categories();
}
