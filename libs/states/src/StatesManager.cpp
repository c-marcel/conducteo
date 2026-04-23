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

#include <StatesManager.h>
#include <actions/ActionsManager.h>
#include <databases/Environments.h>
#include <model/Annotation.h>
#include <model/DxfContent.h>
#include <StatesObserver.h>
#include <model/Surface.h>
#include <model/Volume.h>
#include <model/Model.h>
#include <model/Image.h>
#include <QApplication>
#include <LocalData.h>
#include <QClipboard>
#include <QMimeData>
#include <Zoom.h>
#include <Log.h>

StatesManager::StatesManager(QObject *parent):
    QObject(parent),
    _currentModel(0),
    _interfaceMode(Selection),
    _viewType(DisplayModel),
    _simulationState(Idle),
    _upgradeState(Unknown),
    _upgradePercent(0),
    _showImages(true),
    _showDxf(true),
	_zoom(new Zoom),
    _includeAnnotationsIn2dView(true),
    _includeImagesIn2dView(true),
    _includeDxfIn2dView(true),
    _includeAnnotationsIn1dView(true),
    _includeImagesIn1dView(true),
    _includeDxfIn1dView(true),
    _includeAnnotationsInResultsView(false),
    _includeImagesInResultsView(false),
    _includeDxfInResultsView(false),
    _includeAnnotationsInEnvsView(false),
    _includeImagesInEnvsView(false),
    _includeDxfInEnvsView(false),
    _includeAnnotationsInCondensationView(false),
    _includeImagesInCondensationView(false),
    _includeDxfInCondensationView(false),
    _includeAnnotationsInRhView(false),
    _includeImagesInRhView(false),
    _includeDxfInRhView(false)
{
    connect(_zoom, &Zoom::zoomChanged, this, &StatesManager::zoomChanged, Qt::UniqueConnection);
    ActionsManager::instance()->registerObserver(this);

    // Initialize images exports options.
    std::string s_includeAnnotationsIn2dView=LocalData::instance()->getParameter("include-annotations-in-2d-view");
    std::string s_includeImagesIn2dView=LocalData::instance()->getParameter("include-images-in-2d-view");
    std::string s_includeDxfIn2dView=LocalData::instance()->getParameter("include-dxf-in-2d-view");

    if (s_includeAnnotationsIn2dView=="false")
        _includeAnnotationsIn2dView=false;
    if (s_includeImagesIn2dView=="false")
        _includeImagesIn2dView=false;
    if (s_includeDxfIn2dView=="false")
        _includeDxfIn2dView=false;

    std::string s_includeAnnotationsIn1dView=LocalData::instance()->getParameter("include-annotations-in-1d-view");
    std::string s_includeImagesIn1dView=LocalData::instance()->getParameter("include-images-in-1d-view");
    std::string s_includeDxfIn1dView=LocalData::instance()->getParameter("include-dxf-in-1d-view");

    if (s_includeAnnotationsIn1dView=="false")
        _includeAnnotationsIn1dView=false;
    if (s_includeImagesIn1dView=="false")
        _includeImagesIn1dView=false;
    if (s_includeDxfIn1dView=="false")
        _includeDxfIn1dView=false;

    std::string s_includeAnnotationsInResultsView=LocalData::instance()->getParameter("include-annotations-in-results-view");
    std::string s_includeImagesInResultsView=LocalData::instance()->getParameter("include-images-in-results-view");
    std::string s_includeDxfInResultsView=LocalData::instance()->getParameter("include-dxf-in-results-view");

    if (s_includeAnnotationsInResultsView=="true")
        _includeAnnotationsInResultsView=true;
    if (s_includeImagesInResultsView=="true")
        _includeImagesInResultsView=true;
    if (s_includeDxfInResultsView=="true")
        _includeDxfInResultsView=true;

    std::string s_includeAnnotationsInEnvsView=LocalData::instance()->getParameter("include-annotations-in-environments-view");
    std::string s_includeImagesInEnvsView=LocalData::instance()->getParameter("include-images-in-environments-view");
    std::string s_includeDxfInEnvsView=LocalData::instance()->getParameter("include-dxf-in-environments-view");

    if (s_includeAnnotationsInEnvsView=="true")
        _includeAnnotationsInEnvsView=true;
    if (s_includeImagesInEnvsView=="true")
        _includeImagesInEnvsView=true;
    if (s_includeDxfInEnvsView=="true")
        _includeDxfInEnvsView=true;
}

StatesManager::~StatesManager()
{
    if (_currentModel)
    {
        delete _currentModel;
        _currentModel=0;
    }

    ActionsManager::instance()->unregisterObserver(this);
	delete _zoom;
}

void StatesManager::registerObject(StatesObserver *object)
{
    _objects.push_back(object);
}

void StatesManager::unregisterObject(StatesObserver *object)
{
    for (unsigned int i=0 ; i<_objects.size() ; i++)
    {
        if (_objects.at(i)==object)
        {
            _objects.erase(_objects.begin()+i);
            return;
        }
    }
}

void StatesManager::statesChanged()
{
    LOG_INFO("States changed (signal begin).");
    for (unsigned int i=0 ; i<_objects.size() ; i++)
        _objects.at(i)->statesChanged();
    LOG_INFO("States changed (signal end).");
}

void StatesManager::upgradeStateChanged()
{
    LOG_INFO("Upgrade state changed (signal begin).");
    for (unsigned int i=0 ; i<_objects.size() ; i++)
        _objects.at(i)->upgradeStateChanged();
    LOG_INFO("Upgrade state changed (signal end).");
}

bool StatesManager::includeDxfInRhView() const
{
    return _includeDxfInRhView;
}

void StatesManager::setIncludeDxfInRhView(bool includeDxfInRhView)
{
    _includeDxfInRhView = includeDxfInRhView;
}

bool StatesManager::includeImagesInRhView() const
{
    return _includeImagesInRhView;
}

void StatesManager::setIncludeImagesInRhView(bool includeImagesInRhView)
{
    _includeImagesInRhView = includeImagesInRhView;
}

bool StatesManager::includeAnnotationsInRhView() const
{
    return _includeAnnotationsInRhView;
}

void StatesManager::setIncludeAnnotationsInRhView(bool includeAnnotationsInRhView)
{
    _includeAnnotationsInRhView = includeAnnotationsInRhView;
}

bool StatesManager::includeDxfInCondensationView() const
{
    return _includeDxfInCondensationView;
}

void StatesManager::setIncludeDxfInCondensationView(bool includeDxfInCondensationView)
{
    _includeDxfInCondensationView = includeDxfInCondensationView;
}

bool StatesManager::includeImagesInCondensationView() const
{
    return _includeImagesInCondensationView;
}

void StatesManager::setIncludeImagesInCondensationView(bool includeImagesInCondensationView)
{
    _includeImagesInCondensationView = includeImagesInCondensationView;
}

bool StatesManager::includeAnnotationsInCondensationView() const
{
    return _includeAnnotationsInCondensationView;
}

void StatesManager::setIncludeAnnotationsInCondensationView(bool includeAnnotationsInCondensationView)
{
    _includeAnnotationsInCondensationView = includeAnnotationsInCondensationView;
}

void StatesManager::deleteCurrentModel()
{
    LOG_INFO("Delete current model: " << _currentModel);
    if (_currentModel)
    {
        if (!_currentModel->running())
        {
            LOG_INFO("Execute deletion (model is not running).");
            delete _currentModel;
        }
        else
        {
            LOG_INFO("Don't execute deletion (model is running).");
            _currentModel->deleteObject(true);
        }
        _currentModel=0;
        clearModelData();
        statesChanged();

        // Delete stored actions into actions manager.
        ActionsManager::instance()->clear();
    }
}

void StatesManager::createModel()
{
    LOG_INFO("Create model.");

    if (_currentModel)
        deleteCurrentModel();
    _currentModel=new Model;
    _currentModel->setModel1d(new Model);
    clearModelData();
    statesChanged();
}

void StatesManager::clearModelData()
{
    LOG_INFO("Clear model data.");

    ActionsManager::instance()->setSavedModel(true);
    Environments::instance()->initializeDescriptions();
    _modelFilename="";
    _interfaceMode=Selection;
    _viewType=DisplayModel;
    _simulationState=Idle;
}

Model *StatesManager::currentModel() const
{
    return _currentModel;
}

Model *StatesManager::currentModelView() const
{
    if (!_currentModel)
        return 0;
    if (_viewType!=Display1DModel && _viewType!=Display1DResults)
        return _currentModel;
    return _currentModel->model1d();
}

void StatesManager::setSavedModel(bool saved)
{
    LOG_INFO("Set model.saved=" << saved);

    if (ActionsManager::instance()->saved()==saved)
        return;

    ActionsManager::instance()->setSavedModel(saved);
    statesChanged();
}

bool StatesManager::saved() const
{
    return ActionsManager::instance()->saved();
}

void StatesManager::setModelFilename(const std::string &filename)
{
    LOG_INFO("Set model filename.");

    _modelFilename=filename;
    statesChanged();
}

std::string StatesManager::modelFilename() const
{
    return _modelFilename;
}

std::vector<Volume*> StatesManager::selectedVolumes() const
{
    std::vector<Volume*> volumes;

    Model *model=currentModelView();

    if (!model)
        return volumes;

    for (unsigned int i=0 ; i<model->std::vector<Volume*>::size() ; i++)
        if (model->std::vector<Volume*>::at(i)->selected())
            volumes.push_back(model->std::vector<Volume*>::at(i));

    return volumes;
}

std::vector<Surface*> StatesManager::selectedSurfaces() const
{
    std::vector<Surface*> surfaces;

    Model *model=currentModelView();

    if (!model)
        return surfaces;

    for (unsigned int i=0 ; i<model->std::vector<Surface*>::size() ; i++)
        if (model->std::vector<Surface*>::at(i)->selected())
            surfaces.push_back(model->std::vector<Surface*>::at(i));

    return surfaces;
}

std::vector<Image*> StatesManager::selectedImages() const
{
    std::vector<Image*> images;

    Model *model=currentModelView();

    if (!model)
        return images;

    for (unsigned int i=0 ; i<model->std::vector<Image*>::size() ; i++)
        if (model->std::vector<Image*>::at(i)->selected())
            images.push_back(model->std::vector<Image*>::at(i));

    return images;
}

std::vector<DxfContent*> StatesManager::selectedDxf() const
{
    std::vector<DxfContent*> dxf;

    Model *model=currentModelView();

    if (!model)
        return dxf;

    for (unsigned int i=0 ; i<model->std::vector<DxfContent*>::size() ; i++)
        if (model->std::vector<DxfContent*>::at(i)->selected())
            dxf.push_back(model->std::vector<DxfContent*>::at(i));

    return dxf;
}

std::vector<Annotation*> StatesManager::selectedAnnotations() const
{
    std::vector<Annotation*> annotations;

    Model *model=currentModelView();

    if (!model)
        return annotations;

    for (unsigned int i=0 ; i<model->std::vector<Annotation*>::size() ; i++)
        if (model->std::vector<Annotation*>::at(i)->selection() != Annotation::NoSelection)
            annotations.push_back(model->std::vector<Annotation*>::at(i));

    return annotations;
}

void StatesManager::setInterfaceMode(InterfaceMode mode)
{
    LOG_INFO("Set interface mode to: " << mode);

    _interfaceMode=mode;
    statesChanged();
}

StatesManager::InterfaceMode StatesManager::interfaceMode() const
{
    return _interfaceMode;
}

void StatesManager::setViewType(ViewType type)
{
    LOG_INFO("Set view type to: " << type);

    _viewType=type;
    if (_viewType!=DisplayModel && _viewType!=Display1DModel)
        _interfaceMode=Selection;
    statesChanged();
}

StatesManager::ViewType StatesManager::viewType() const
{
    return _viewType;
}

void StatesManager::setSimulationState(SimulationState state)
{
    LOG_INFO("Set simulation state to: " << state);

    _simulationState=state;
    statesChanged();
}

void StatesManager::setUpgradePercent(int percent)
{
    _upgradePercent=percent;
    upgradeStateChanged();
}

StatesManager::SimulationState StatesManager::simulationState() const
{
    return _simulationState;
}

Zoom *StatesManager::zoom()
{
    return _zoom;
}

void StatesManager::zoomChanged()
{
    for (unsigned int i=0 ; i<_objects.size() ; i++)
        _objects.at(i)->zoomChanged();
}

bool StatesManager::clipboardHasContent() const
{
    QClipboard *clipboard=QApplication::clipboard();
    if (!clipboard)
        return false;

    const QMimeData *mimedata=clipboard->mimeData();
    if (!mimedata)
        return false;

    if (!mimedata->hasFormat("conducteo/volumes"))
        return false;

    return true;
}

void StatesManager::setUpgradeState(UpgradeState state)
{
    LOG_INFO("Set upgrade state to: " << state);

    _upgradeState=state;
    upgradeStateChanged();
}

StatesManager::UpgradeState StatesManager::upgradeState() const
{
    return _upgradeState;
}

void StatesManager::setCurrentMaterialId(const std::string &id)
{
    LOG_INFO("Set current material id to: " << id);

    _currentMaterialId=id;
}

std::string StatesManager::currentMaterialId() const
{
    return _currentMaterialId;
}

void StatesManager::setCurrentBoundaryConditionId(const std::string &id)
{
    LOG_INFO("Set current boundary condition id to: " << id);

    _currentBoundaryConditionId=id;
}

std::string StatesManager::currentBoundaryConditionId() const
{
    return _currentBoundaryConditionId;
}

void StatesManager::setCurrentEnvironmentId(const std::string &id)
{
    LOG_INFO("Set current environment id to: " << id);

    _currentEnvironmentId=id;
}

std::string StatesManager::currentEnvironmentId() const
{
    return _currentEnvironmentId;
}

void StatesManager::changed()
{
    statesChanged();
}

void StatesManager::repaintView()
{
    LOG_INFO("Repaint view requested.");
    statesChanged();
}

int StatesManager::upgradePercent() const
{
    return _upgradePercent;
}

void StatesManager::showImages(bool show)
{
    LOG_INFO("Set show images to: " << show);

    _showImages=show;
    changed();
}

bool StatesManager::showImages() const
{
    return _showImages;
}

void StatesManager::modelResultsAltered()
{
    LOG_INFO("Set model results as altered.");

    if (_simulationState==Done)
    {
        if (_viewType!=DisplayModel && _viewType!=Display1DModel)
            setViewType(DisplayModel);
        setSimulationState(Idle);
    }
}

void StatesManager::modelAltered()
{
    // Do nothing.
}

void StatesManager::showDxf(bool show)
{
    LOG_INFO("Set show Dxf to: " << show);

    _showDxf=show;
    changed();
}

bool StatesManager::showDxf() const
{
    return _showDxf;
}

bool StatesManager::includeAnnotationsIn2dView() const
{
    return _includeAnnotationsIn2dView;
}

bool StatesManager::includeImagesIn2dView() const
{
    return _includeImagesIn2dView;
}

bool StatesManager::includeDxfIn2dView() const
{
    return _includeDxfIn2dView;
}

bool StatesManager::includeAnnotationsIn1dView() const
{
    return _includeAnnotationsIn1dView;
}

bool StatesManager::includeImagesIn1dView() const
{
    return _includeImagesIn1dView;
}

bool StatesManager::includeDxfIn1dView() const
{
    return _includeDxfIn1dView;
}

bool StatesManager::includeAnnotationsInResultsView() const
{
    return _includeAnnotationsInResultsView;
}

bool StatesManager::includeImagesInResultsView() const
{
    return _includeImagesInResultsView;
}

bool StatesManager::includeDxfInResultsView() const
{
    return _includeDxfInResultsView;
}

bool StatesManager::includeAnnotationsInEnvsView() const
{
    return _includeAnnotationsInEnvsView;
}

bool StatesManager::includeImagesInEnvsView() const
{
    return _includeImagesInEnvsView;
}

bool StatesManager::includeDxfInEnvsView() const
{
    return _includeDxfInEnvsView;
}

void StatesManager::includeAnnotationsIn2dView(bool include)
{
    _includeAnnotationsIn2dView=include;
}

void StatesManager::includeImagesIn2dView(bool include)
{
    _includeImagesIn2dView=include;
}

void StatesManager::includeDxfIn2dView(bool include)
{
    _includeDxfIn2dView=include;
}

void StatesManager::includeAnnotationsIn1dView(bool include)
{
    _includeAnnotationsIn1dView=include;
}

void StatesManager::includeImagesIn1dView(bool include)
{
    _includeImagesIn1dView=include;
}

void StatesManager::includeDxfIn1dView(bool include)
{
    _includeDxfIn1dView=include;
}

void StatesManager::includeAnnotationsInResultsView(bool include)
{
    _includeAnnotationsInResultsView=include;
}

void StatesManager::includeImagesInResultsView(bool include)
{
    _includeImagesInResultsView=include;
}

void StatesManager::includeDxfInResultsView(bool include)
{
    _includeDxfInResultsView=include;
}

void StatesManager::includeAnnotationsInEnvsView(bool include)
{
    _includeAnnotationsInEnvsView=include;
}

void StatesManager::includeImagesInEnvsView(bool include)
{
    _includeImagesInEnvsView=include;
}

void StatesManager::includeDxfInEnvsView(bool include)
{
    _includeDxfInEnvsView=include;
}
