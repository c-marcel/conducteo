// conducteö - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2019, Clément MARCEL.
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

#ifndef _STATES_MANAGER_H
#define _STATES_MANAGER_H

#include <actions/Observer.h>
#include <tools/Singleton.h>
#include <QObject>
#include <vector>
#include <string>

class StatesObserver;
class Annotation;
class DxfContent;
class Surface;
class Volume;
class Model;
class Image;
class Zoom;	

class StatesManager: public QObject,
                     public Singleton<StatesManager>,
                     public Observer
{
    Q_OBJECT
public:
    enum InterfaceMode
    {
        Selection,
        AddRectangle,
        AddPolyline,
        AddCircle,
        AddEllipse,
        GetMaterial,
        SetPhysics,
        SetEnvironments,
        AddPoint,
        AddLengthCotation,
        AddAngleCotation,
        AddText,
        AddArrow
    };

    enum ViewType
    {
        DisplayModel,
        Display1DModel,
        DisplayMesh,
        DisplayResults,
        Display1DResults,
        DisplayReport,
        DisplayCondensationResults,
        DisplayHumidityResults,
    };

    enum SimulationState
    {
        Idle,
        InProgress,
        Done
    };

    enum UpgradeState
    {
        Unknown,
        UpOfDate,
        OutOfDate,
        Downloading,
        DownloadDone,
        UpgradeInProgress,
        UpgradeError
    };

    StatesManager(QObject *parent=0);
    virtual ~StatesManager();

    void registerObject(StatesObserver *object);
    void unregisterObject(StatesObserver *object);

    void setSavedModel(bool saved);
    void setModelFilename(const std::string &filename);
    void setInterfaceMode(InterfaceMode mode);
    void setViewType(ViewType type);
    void setSimulationState(SimulationState state);
    void setUpgradeState(UpgradeState state);
    void setCurrentMaterialId(const std::string &id);
    void setCurrentBoundaryConditionId(const std::string &id);
    void setCurrentEnvironmentId(const std::string &id);
    void setUpgradePercent(int percent);
    void showImages(bool show);
    void showDxf(bool show);

    void deleteCurrentModel();
    void createModel();
    void repaintView();

    Model *currentModel() const;
    Model *currentModelView() const;
    bool saved() const;
    std::string modelFilename() const;
    std::vector<Volume*> selectedVolumes() const;
    std::vector<Surface*> selectedSurfaces() const;
    std::vector<Image*> selectedImages() const;
    std::vector<DxfContent*> selectedDxf() const;
    std::vector<Annotation*> selectedAnnotations() const;
    InterfaceMode interfaceMode() const;
    ViewType viewType() const;
    SimulationState simulationState() const;
    Zoom *zoom();
    bool clipboardHasContent() const;
    UpgradeState upgradeState() const;
    std::string currentMaterialId() const;
    std::string currentBoundaryConditionId() const;
    std::string currentEnvironmentId() const;
    int upgradePercent() const;
    bool showImages() const;
    bool showDxf() const;

    void changed();
    void modelAltered();
    void modelResultsAltered();

    bool includeAnnotationsIn2dView() const;
    bool includeImagesIn2dView() const;
    bool includeDxfIn2dView() const;

    bool includeAnnotationsIn1dView() const;
    bool includeImagesIn1dView() const;
    bool includeDxfIn1dView() const;

    bool includeAnnotationsInResultsView() const;
    bool includeImagesInResultsView() const;
    bool includeDxfInResultsView() const;

    bool includeAnnotationsInEnvsView() const;
    bool includeImagesInEnvsView() const;
    bool includeDxfInEnvsView() const;

    void includeAnnotationsIn2dView(bool include);
    void includeImagesIn2dView(bool include);
    void includeDxfIn2dView(bool include);

    void includeAnnotationsIn1dView(bool include);
    void includeImagesIn1dView(bool include);
    void includeDxfIn1dView(bool include);

    void includeAnnotationsInResultsView(bool include);
    void includeImagesInResultsView(bool include);
    void includeDxfInResultsView(bool include);

    void includeAnnotationsInEnvsView(bool include);
    void includeImagesInEnvsView(bool include);
    void includeDxfInEnvsView(bool include);

    bool includeAnnotationsInCondensationView() const;
    void setIncludeAnnotationsInCondensationView(bool includeAnnotationsInCondensationView);

    bool includeImagesInCondensationView() const;
    void setIncludeImagesInCondensationView(bool includeImagesInCondensationView);

    bool includeDxfInCondensationView() const;
    void setIncludeDxfInCondensationView(bool includeDxfInCondensationView);

    bool includeAnnotationsInRhView() const;
    void setIncludeAnnotationsInRhView(bool includeAnnotationsInRhView);

    bool includeImagesInRhView() const;
    void setIncludeImagesInRhView(bool includeImagesInRhView);

    bool includeDxfInRhView() const;
    void setIncludeDxfInRhView(bool includeDxfInRhView);

public slots:
    void zoomChanged();

private:
    void statesChanged();
    void upgradeStateChanged();
    void clearModelData();

    std::vector<StatesObserver*>    _objects;

    Model                          *_currentModel;
    std::string                     _modelFilename;
    
    InterfaceMode                   _interfaceMode;
    ViewType                        _viewType;
    SimulationState                 _simulationState;
    Zoom                           *_zoom;
    
    std::string                     _currentMaterialId;
    std::string                     _currentBoundaryConditionId;
    std::string                     _currentEnvironmentId;

    UpgradeState                    _upgradeState;
    int                             _upgradePercent;

    bool                            _showImages;
    bool                            _showDxf;

    bool                            _includeAnnotationsIn2dView;
    bool                            _includeImagesIn2dView;
    bool                            _includeDxfIn2dView;

    bool                            _includeAnnotationsIn1dView;
    bool                            _includeImagesIn1dView;
    bool                            _includeDxfIn1dView;

    bool                            _includeAnnotationsInResultsView;
    bool                            _includeImagesInResultsView;
    bool                            _includeDxfInResultsView;

    bool                            _includeAnnotationsInEnvsView;
    bool                            _includeImagesInEnvsView;
    bool                            _includeDxfInEnvsView;

    bool                            _includeAnnotationsInCondensationView;
    bool                            _includeImagesInCondensationView;
    bool                            _includeDxfInCondensationView;

    bool                            _includeAnnotationsInRhView;
    bool                            _includeImagesInRhView;
    bool                            _includeDxfInRhView;
};

#endif
