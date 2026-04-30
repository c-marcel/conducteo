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

#include <ModelViewer.h>
#include <actions/model/SetBoundaryConditionFromSelection.h>
#include <actions/model/SetSurfaceBoundaryCondition.h>
#include <actions/model/UpdateTextAnnotationContent.h>
#include <actions/model/SetEnvironmentFromSelection.h>
#include <actions/model/SetMaterialFromSelection.h>
#include <actions/model/UpdateAnnotationPoints.h>
#include <actions/model/SetSurfaceEnvironment.h>
#include <actions/model/TranslateVolumeBorder.h>
#include <actions/model/TranslateVolumeCorner.h>
#include <actions/model/ImportVolumesFromXml.h>
#include <actions/model/SetVolumeMaterial.h>
#include <actions/model/TranslateVolumes.h>
#include <actions/model/RemoveAnnotation.h>
#include <databases/BoundaryConditions.h>
#include <actions/model/TranslateImage.h>
#include <actions/model/RotateVolumes.h>
#include <actions/model/AddAnnotation.h>
#include <actions/model/AddRectangle.h>
#include <actions/model/DeletePoint.h>
#include <actions/model/AddPolyline.h>
#include <physics/BoundaryCondition.h>
#include <actions/model/ResizeImage.h>
#include <actions/model/AddEllipse.h>
#include <actions/model/AddCircle.h>
#include <databases/Environments.h>
#include <actions/model/AddPoint.h>
#include <actions/ActionsManager.h>
#include <geometry/GeometryTools.h>
#include <databases/Materials.h>
#include <physics/Environment.h>
#include <images/ImageDataQt.h>
#include <physics/Material.h>
#include <model/DxfContent.h>
#include <model/Rectangle.h>
#include <LinguistManager.h>
#include <tools/ToolBox.h>
#include <dxf/DxfDataQt.h>
#include <model/Ellipse.h>
#include <geometry/Line.h>
#include <model/Surface.h>
#include <StatesManager.h>
#include <numeric/Mesh.h>
#include <numeric/Cell.h>
#include <model/Circle.h>
#include <geometry/Arc.h>
#include <model/Volume.h>
#include <model/Model.h>
#include <model/Image.h>
#include <QPainterPath>
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QToolTip>
#include <QPainter>

#include <Layer.h>
#include <Zoom.h>
#include <QMenu>
#include <Log.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#endif

using namespace std;

ModelViewer::ModelViewer(QWidget *parent):
    QWidget(parent),
    DxfViewer(StatesManager::instance()->zoom()),
    _model(0),
    _verticalTranslationBoard(TranslationBoard::Vertical, this),
    _horizontalTranslationBoard(TranslationBoard::Horizontal, this),
    _layout(this),
    _currentMode(Idle),
    _currentDrawnVolume(0),
    _dxTranslation(0.0),
    _dyTranslation(0.0),
    _hideRotation(false),
#ifdef DISPLAY_MATRIX_CONTENT
    _centeredCell(0),
#endif
    _mousePressed(false),
    _ctrlPressed(false),
    _mouseOverElementType(Nothing),
    _mouseOverPath(0),
    _mouseOverVolume(0),
    _dxPointTranslation(0.0),
    _dyPointTranslation(0.0),
    _lastDxPointTranslation(0.0),
    _lastDyPointTranslation(0.0),
    _majPressed(false),
    _displayNearPoint(false),
    _zoom(StatesManager::instance()->zoom()),
    _annotationActionType(AnnotationNoAction)
{
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    _layout.addWidget(&_verticalTranslationBoard, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    _layout.addWidget(&_horizontalTranslationBoard, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignBottom);

    _en13370Layout.addWidget(&_en13370Button, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    _en13370Layout.addWidget(&_en13370Area, 0, 1, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    _en13370Layout.addItem(new QSpacerItem(5000, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 2);
    _en13370Layout.addItem(new QSpacerItem(0, 5000, QSizePolicy::Expanding, QSizePolicy::Expanding), 2, 0);
    _layout.addLayout(&_en13370Layout, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    _en13370Area.setWidget(&_en13370Interface);
    _en13370Area.setAttribute(Qt::WA_NoMousePropagation);
    _en13370Area.hide();

    _en13370Button.setIconSize(QSize(32, 32));

    connect(&_verticalTranslationBoard,
            &TranslationBoard::decreaseRequested,
            this,
            &ModelViewer::downTranslation);
    connect(&_verticalTranslationBoard,
            &TranslationBoard::increaseRequested,
            this,
            &ModelViewer::upTranslation);
    connect(&_horizontalTranslationBoard,
            &TranslationBoard::decreaseRequested,
            this,
            &ModelViewer::leftTranslation);
    connect(&_horizontalTranslationBoard,
            &TranslationBoard::increaseRequested,
            this,
            &ModelViewer::rightTranslation);

    connect(&_en13370Button, &QAbstractButton::clicked, this, &ModelViewer::en13370Open);

    applyTheme();
}

ModelViewer::~ModelViewer()
{
}

void ModelViewer::resizeEvent(QResizeEvent *event)
{
    int w=std::min(width()-120, 850+15);
    _en13370Area.setFixedWidth(w);

    int h=std::min(height()-55, 650+15);
    _en13370Area.setFixedHeight(h);
}

void ModelViewer::setModel(Model *model)
{
    _model=model;

#ifdef DISPLAY_MATRIX_CONTENT
    _centeredCell=0;
#endif
}

void ModelViewer::mouseMoveEvent(QMouseEvent *event)
{
    _ctrlPressed=(event->modifiers() & Qt::CTRL);
    _majPressed=(event->modifiers() & Qt::SHIFT);
    _mousePosition=event->pos();

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    Point p=nearPoint(event->pos(), !(event->modifiers() & Qt::CTRL));

    emit mousePositionChanged(p.x(), p.y());
    emit mouseScreenPositionChanged(event->position().x(), event->position().y());

    if (StatesManager::instance()->interfaceMode()==StatesManager::AddPoint)
    {
        Point point;
        point.setX(_zoom->toPhysicalX(event->pos()));
        point.setY(_zoom->toPhysicalY(event->pos()));
        _displayNearPoint=false;

        Line *line=0;
        double distance=0.0;
        bool defined=false;
        for (unsigned int i=0 ; i<model->std::vector<Volume*>::size() ; i++)
        {
            Volume *v=model->std::vector<Volume*>::at(i);
            Line *near=v->nearLine(point);
            if (near)
            {
                double d=GeometryTools::pointDistance(near, point);
                if (d<distance || !defined)
                {
                    defined=true;
                    distance=d;
                    line=near;
                }
            }
        }

        if (line)
        {
            double d=GeometryTools::pointDistance(line, point);
            d=_zoom->toScreenLength(d);
            if (d<=30.0)
            {
                _addNearPoint=GeometryTools::orthogonalProjection(line, point);
                _displayNearPoint=true;
            }
        }
    }

    StatesManager::ViewType view=StatesManager::instance()->viewType();

    if (_mousePressed)
    {
        double x2=_zoom->toPhysicalX(event->pos());
        double y2=_zoom->toPhysicalY(event->pos());

        _currentSelectionEnd.setX(x2);
        _currentSelectionEnd.setY(y2);
    }

    // Mouse translation.
    if (_currentMode==ViewMouseTranslation)
    {
        double dx=_mouseClickPoint.x()-event->position().x();
        double dy=_mouseClickPoint.y()-event->position().y();

        dx=_zoom->toPhysicalLength(dx);
        dy=_zoom->toPhysicalLength(dy);

        _zoom->up(dy);
        _zoom->right(dx);

        _mouseClickPoint=event->pos();
    }

    // Selection translation.
    else if (_currentMode==SelectionTranslation)
    {
        double dx=event->position().x()-_mouseClickPoint.x();
        double dy=_mouseClickPoint.y()-event->position().y();

        dx=_zoom->toPhysicalLength(dx);
        dy=_zoom->toPhysicalLength(dy);

        _dxTranslation+=dx;
        _dyTranslation+=dy;
        
        _hideRotation=true;

        _mouseClickPoint=event->pos();
        repaint();
    }

    // Volume elements translation.
    else if (_currentMode==VolumeCornerTranslation || _currentMode==VolumeBorderTranslation)
    {
        double dx=event->position().x()-_mouseClickPoint.x();
        double dy=_mouseClickPoint.y()-event->position().y();

        dx=_zoom->toPhysicalLength(dx);
        dy=_zoom->toPhysicalLength(dy);

        _dxPointTranslation=dx;
        _dyPointTranslation=dy;
        
        repaint();
    }

    // Selection rotation.
    else if (_currentMode==SelectionRotation)
    {
        double x=_zoom->toPhysicalX(event->pos());
        double y=_zoom->toPhysicalY(event->pos());

        double dx=x-_rotation.center().x();
        double dy=y-_rotation.center().y();
        double angle=atan2(dy, dx)*180.0/M_PI;
        _rotation.setAngle(angle);
        repaint();
    }

    // Mouse cursor.
    else
    {
        // Image scale.
        if (_currentMode==ScaleImage)
        {
            setCursor(QCursor(Qt::CrossCursor));
        }

        // Mouse on rotation point.
        else if (mouseOnRotationPoint(event->pos()))
        {
            setCursor(_rotationCursor);
        }

        else if (_currentMode!=SelectionTranslation)
        {
            double x2=_zoom->toPhysicalX(event->pos());
            double y2=_zoom->toPhysicalY(event->pos());

            double precision=_zoom->toPhysicalLength(20.0);
            Volume *volume=model->getVolumeAtPosition(x2, y2);
            Image *image=model->getImageAtPosition(x2, y2, precision);
            if (volume && volume->selected() && (view==StatesManager::DisplayModel || view==StatesManager::Display1DModel))
            {
                setCursor(QCursor(Qt::SizeAllCursor));
            }

            else if (image && StatesManager::instance()->showImages() && image->selected() && (view==StatesManager::DisplayModel || view==StatesManager::Display1DModel))
            {
                // Translation or resize.
                ResizeType type=getResizeType(image, event->pos());
                setCursor(QCursor(cursorFromResizeType(type)));
            }

            else
            {
                setCursor(QCursor(Qt::ArrowCursor));
            }

            // Border volume selection.
            std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
            if (volumes.size()==1 && volumes.at(0)->selected() && !volumes.at(0)->containsArc() && (view==StatesManager::DisplayModel || view==StatesManager::Display1DModel))
            {
                Point p;
                p.setX(x2);
                p.setY(y2);

                double dist=_zoom->toPhysicalLength(10.0);
                _mouseOverElementType=Nothing;

                // Cursor near a point.
                if (1)
                {
                    for (unsigned i=0 ; i<volumes.at(0)->size() ; i++)
                    {
                        Path *path=volumes.at(0)->at(i);

                        Point b=path->begin();
                        if (GeometryTools::pointDistance(b, p)<=dist)
                        {
                            // Get other path.
                            Path *other=0;
                            for (unsigned int j=0 ; j<volumes.at(0)->size() ; j++)
                            {
                                if (i!=j)
                                {
                                    Path *path2=volumes.at(0)->at(j);
                                    if (GeometryTools::equivalentPoints(path2->begin(), b) || GeometryTools::equivalentPoints(path2->end(), b))
                                    {
                                        other=path2;
                                        break;
                                    }

                                }
                            }

                            if (other)
                            {
                                QTransform transform;
                                double angle=0.5*(path->angle()+other->angle());
                                transform=transform.rotate(-angle);
                                setCursor(QCursor(_resizeOriginalImage.transformed(transform, Qt::SmoothTransformation)));
                            }

                            _mouseOverPoint=b;
                            _mouseOverElementType=Corner;
                            _mouseOverVolume=volumes.at(0);
                            _mouseOverPath=path;
                            break;
                        }

                        Point e=path->end();
                        if (GeometryTools::pointDistance(e, p)<=dist)
                        {
                            // Get other path.
                            Path *other=0;
                            for (unsigned int j=0 ; j<volumes.at(0)->size() ; j++)
                            {
                                if (i!=j)
                                {
                                    Path *path2=volumes.at(0)->at(j);
                                    if (GeometryTools::equivalentPoints(path2->begin(), e) || GeometryTools::equivalentPoints(path2->end(), e))
                                    {
                                        other=path2;
                                        break;
                                    }

                                }
                            }

                            if (other)
                            {
                                QTransform transform;
                                double angle=0.5*(path->angle()+other->angle());
                                transform=transform.rotate(-angle);
                                setCursor(QCursor(_resizeOriginalImage.transformed(transform, Qt::SmoothTransformation)));
                            }

                            _mouseOverPoint=e;
                            _mouseOverPath=path;
                            _mouseOverVolume=volumes.at(0);
                            _mouseOverElementType=Corner;
                            break;
                        }
                    }
                }

                // Cursor near a border.
                if (_mouseOverElementType==Nothing)
                {
                    for (unsigned i=0 ; i<volumes.at(0)->size() ; i++)
                    {
                        Path *path=volumes.at(0)->at(i);
                        if (path->distance(p)<=dist)
                        {
                            QTransform transform;
                            double angle=path->angle();
                            transform=transform.rotate(-angle);
                            setCursor(QCursor(_resizeOriginalImage.transformed(transform)));

                            _mouseOverPath=path;
                            _mouseOverVolume=volumes.at(0);
                            _mouseOverElementType=Border;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Add rectangle.
    if (StatesManager::instance()->interfaceMode()==StatesManager::AddRectangle)
        updateRectangleGeometry(p.x(), p.y());
    
    // Add circle.
    else if (StatesManager::instance()->interfaceMode()==StatesManager::AddCircle)
        updateCircleGeometry(p.x(), p.y());

    // Add ellipse.
    else if (StatesManager::instance()->interfaceMode()==StatesManager::AddEllipse)
        updateEllipseGeometry(p.x(), p.y());

    // Add polyline.
    else if (StatesManager::instance()->interfaceMode()==StatesManager::AddPolyline)
        setCursor(Qt::CrossCursor);

    // Temperature under cursor.
    if (view==StatesManager::DisplayResults || view==StatesManager::Display1DResults)
    {
        double x2=_zoom->toPhysicalX(event->pos());
        double y2=_zoom->toPhysicalY(event->pos());

        bool success=false;
        double temperature=model->temperatureAtPosition(x2, y2, &success);

        if (!success)
            emit unSetTemperature();
        else
            emit setTemperature(temperature);

        // Display humidity.
        if (model->computationType() == Model::GlaserCondensation)
        {
            double humidity = model->humidityAtPosition(x2, y2, &success);

            if (!success)
                emit unSetHumidity();
            else
                emit setHumidity(temperature, humidity);
        }
    }

    // Humidity under cursor.
    else if (view == StatesManager::DisplayCondensationResults || view == StatesManager::DisplayHumidityResults)
    {
        double x2 = _zoom->toPhysicalX(event->pos());
        double y2 = _zoom->toPhysicalY(event->pos());

        bool success = false;
        double temperature = model->temperatureAtPosition(x2, y2, &success);

        if (!success)
        {
            emit unSetHumidity();
            emit unSetTemperature();
        }
        else
        {
            emit setTemperature(temperature);

            double humidity = model->humidityAtPosition(x2, y2, &success);

            if (!success)
                emit unSetHumidity();
            else
                emit setHumidity(temperature, humidity);
        }
    }

    repaint();
}

Qt::CursorShape ModelViewer::cursorFromResizeType(ResizeType type) const
{
    if (type==Left || type==Right)
        return Qt::SizeHorCursor;
    else if (type==Top || type==Bottom)
        return Qt::SizeVerCursor;
    else if (((type & Bottom) && (type & Left)) || ((type & Top) && (type & Right)))
        return Qt::SizeBDiagCursor;
    else if (((type & Bottom) && (type & Right)) || ((type & Top) && (type & Left)))
        return Qt::SizeFDiagCursor;
    return Qt::SizeAllCursor;
}

ModelViewer::ResizeType ModelViewer::getResizeType(Image *image, const QPoint &pos) const
{
    QPoint p=_zoom->toScreenPosition(image->x(), image->y());
    ResizeType type=Translation;

    // Left resize. (1)
    if (pos.x()<p.x()+20)
        type=Left;

    // Right resize. (2)
    else if (pos.x()>p.x()+_zoom->toScreenLength(image->width())-20)
        type=Right;

    // Top resize. (4)
    if (pos.y()<p.y()-_zoom->toScreenLength(image->height())+20)
        type=(ResizeType)(type+Top);

    // Bottom resize. (8)
    else if (pos.y()>p.y()-20)
        type=(ResizeType)(type+Bottom);

    return type;
}

void ModelViewer::updateRectangleGeometry(double x, double y)
{
    Rectangle *rectangle=dynamic_cast<Rectangle *>(_currentDrawnVolume);
    if (rectangle)
    {
        double width=x-rectangle->x();
        double height=y-rectangle->y();
        rectangle->setWidth(width);
        rectangle->setHeight(height);
        repaint();

        emit mouseDimensionChanged(std::fabs(width), std::fabs(height));
    }
}

void ModelViewer::updateCircleGeometry(double x, double y)
{
    Circle *circle=dynamic_cast<Circle *>(_currentDrawnVolume);
    if (circle)
    {
        double dx=x-circle->x();
        double dy=y-circle->y();
        double radius=sqrt(dx*dx+dy*dy);
        circle->setRadius(radius);
        repaint();

        emit mouseDimensionChanged(2.0*radius, 2.0*radius);
    }
}

void ModelViewer::leaveEvent(QEvent *event)
{
    emit hideMousePosition();
    emit mouseScreenPositionChanged(-1, -1);
}

void ModelViewer::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y()>0)
        _zoom->increase(event->position().toPoint());
    else
        _zoom->decrease(event->position().toPoint());
}

void ModelViewer::mousePressEvent(QMouseEvent *event)
{
    if (StatesManager::instance()->simulationState()==StatesManager::InProgress)
        return;

    _mouseClickPoint=event->pos();

    // Material selection by mouse.
    if (StatesManager::instance()->interfaceMode() == StatesManager::GetMaterial)
        return;

    // Selection.
    double x=_zoom->toPhysicalX(event->pos());
    double y=_zoom->toPhysicalY(event->pos());

    // Scale image.
    if (_currentMode==ScaleImage)
        return finalizeScaleDefinition();

    StatesManager::InterfaceMode mode=StatesManager::instance()->interfaceMode();

    // Translation mode.
    if (event->buttons() & Qt::MiddleButton)
    {
        _currentMode=ViewMouseTranslation;
        setCursor(QCursor(Qt::ClosedHandCursor));
    }

    // Left click.
    else if (event->buttons() & Qt::LeftButton)
    {
        StatesManager::ViewType view=StatesManager::instance()->viewType();
        Model *model=StatesManager::instance()->currentModelView();

        // Model 1d or 2d.
        if (view==StatesManager::DisplayModel || view==StatesManager::Display1DModel)
        {
            // Add rectangle mode.
            if (mode==StatesManager::AddRectangle)
                drawRectangle(_mouseClickPoint, !(event->modifiers() & Qt::CTRL));

            // Add circle.
            else if (mode==StatesManager::AddCircle)
                drawCircle(_mouseClickPoint, !(event->modifiers() & Qt::CTRL));

            // Add ellipse.
            else if (mode==StatesManager::AddEllipse)
                drawEllipse(_mouseClickPoint, !(event->modifiers() & Qt::CTRL));

            // Add polyline.
            else if (mode==StatesManager::AddPolyline)
                drawPolyline(_mouseClickPoint, !(event->modifiers() & Qt::CTRL), (event->modifiers() & Qt::SHIFT));

            // Get length.
            else if (mode==StatesManager::AddLengthCotation)
                drawLengthDataPoint(_mouseClickPoint, !(event->modifiers() & Qt::CTRL), (event->modifiers() & Qt::SHIFT));

            // Get angle.
            else if (mode==StatesManager::AddAngleCotation)
                drawAngleDataPoint(_mouseClickPoint, !(event->modifiers() & Qt::CTRL), (event->modifiers() & Qt::SHIFT));

            // Add point.
            else if (mode==StatesManager::AddPoint)
                addPoint();

            // Add text.
            else if (mode==StatesManager::AddText)
                addText(_mouseClickPoint);

            // Add arrow.
            else if (mode==StatesManager::AddArrow)
                addArrow(_mouseClickPoint, !(event->modifiers() & Qt::CTRL), (event->modifiers() & Qt::SHIFT));

            // Rotate selection.
            else if (mouseOnRotationPoint(event->pos()))
                _currentMode=SelectionRotation;

            // Translation.
            else if (model && mode==StatesManager::Selection)
            {
                Volume *volume=model->getVolumeAtPosition(x, y);

                double epsilon=_zoom->toPhysicalLength(20.0);

                Image *image=0;
                if (StatesManager::instance()->showImages())
                    image=model->getImageAtPosition(x, y, epsilon);

                Annotation *annotation = model->getAnnotationAtPosition(x, y, epsilon);

                if ((volume || image || annotation) && _mouseOverElementType==Nothing)
                {
                    // Special case when no volume has been selected before.
                    if (!(event->modifiers() & Qt::CTRL) && mode==StatesManager::Selection && volume && !volume->selected())
                        selectVolume(_mouseClickPoint, event->modifiers());
                    else if (!(event->modifiers() & Qt::CTRL) && mode==StatesManager::Selection && image && !image->selected())
                        selectVolume(_mouseClickPoint, event->modifiers());
                    else if (!(event->modifiers() & Qt::CTRL) && mode==StatesManager::Selection && annotation && annotation->selection() != Annotation::NoSelection)
                        selectVolume(_mouseClickPoint, event->modifiers());

                    // Annotation click type.
                    if (annotation && annotation->selection() != Annotation::NoSelection)
                    {
                        _annotationActionType = (AnnotationActionType) annotation->clickType(x, y, epsilon);
                        LOG_TRACE("AnnotationActionType set to: " << _annotationActionType);
                    }

                    _currentMode=SelectionTranslation;

                    if (image)
                    {
                        _resizeType=getResizeType(image, event->pos());
                        setCursor(QCursor(cursorFromResizeType(_resizeType)));
                    }
                    else
                        setCursor(QCursor(Qt::SizeAllCursor));
                }

                // Resize.
                else if(_mouseOverElementType!=Nothing)
                {
                    if (_mouseOverElementType==Border)
                        _currentMode=VolumeBorderTranslation;

                    else if (_mouseOverElementType==Corner)
                        _currentMode=VolumeCornerTranslation;

                    _dxPointTranslation=0.0;
                    _dyPointTranslation=0.0;
                    _lastDxPointTranslation=0.0;
                    _lastDyPointTranslation=0.0;
                }

                else
                {
                    model->unselectAllVolumes();
                    model->unselectAllImages();
                }
            }
    
        }

        if ((StatesManager::instance()->interfaceMode()==StatesManager::Selection || StatesManager::instance()->interfaceMode()==StatesManager::SetPhysics || StatesManager::instance()->interfaceMode()==StatesManager::SetEnvironments)
            && _currentMode==Idle
            && (StatesManager::instance()->viewType()==StatesManager::DisplayModel || StatesManager::instance()->viewType()==StatesManager::Display1DModel))
        {
            _currentSelectionBegin.setX(x);
            _currentSelectionBegin.setY(y);
            _currentSelectionEnd.setX(x);
            _currentSelectionEnd.setY(y);
            _mousePressed=true;
        }
        else
            _mousePressed=false;
    }

    // Right click.
    else if (event->buttons() & Qt::RightButton)
    {
        // Add polyline.
        if (mode==StatesManager::AddPolyline)
            removePolylinePoint();
        customContextMenuRequested(_mouseClickPoint);
    }

#ifdef DISPLAY_MATRIX_CONTENT
    Model *model=StatesManager::instance()->currentModel();
    StatesManager::ViewType view=StatesManager::instance()->viewType();
    if (model && view==StatesManager::DisplayMesh && (event->buttons() & Qt::LeftButton))
    {
        double x=_zoom->toPhysicalX(event->pos());
        double y=_zoom->toPhysicalY(event->pos());
        _centeredCell=model->cellAtPosition(x, y);
        if (_centeredCell)
        {
            _highlightCells=model->matrixNeighbors(_centeredCell->_matrixIdx);
            update();

            if (event->modifiers() & Qt::ControlModifier)
            {
                std::string data=model->matrixData(_centeredCell->_matrixIdx);
                QMessageBox dialog;
                dialog.setText(data.c_str());
                dialog.exec();
            }
        }
    }
#endif

    setFocus();
}

void ModelViewer::customContextMenuRequested(const QPoint &pos)
{
    if (StatesManager::instance()->simulationState()==StatesManager::InProgress)
        return;

    StatesManager::InterfaceMode mode=StatesManager::instance()->interfaceMode();
    if (mode!=StatesManager::Selection)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x=_zoom->toPhysicalX(pos);
    double y=_zoom->toPhysicalY(pos);
    double epsilon=_zoom->toPhysicalLength(4.0);

    Annotation *annotation = model->getAnnotationAtPosition(x, y, epsilon);
    std::vector<Volume*> volumes = StatesManager::instance()->selectedVolumes();
    Volume *volume=model->getVolumeAtPosition(x, y);

    // Annotation menu.
    if (annotation)
    {
        QMenu menu(this);
        QPoint globalPos = mapToGlobal(pos);

        QAction *update_text = 0;
        if (annotation->type() == Annotation::AnnotationText)
            update_text = menu.addAction(_tr("UpdateText"));
        QAction *delete_annotation = menu.addAction(_tr("DeleteAnnotation"));

        QAction *choice = menu.exec(globalPos);

        // Delete.
        if (choice == delete_annotation)
        {
            std::vector<Annotation*> annotations;
            annotations.push_back(annotation);

            RemoveAnnotation *action = new RemoveAnnotation(model, annotations);
            ActionsManager::instance()->exec(action);
        }

        // Update text.
        else if (update_text && choice == update_text)
        {
            updateText(annotation);
        }

        return;
    }

    // Right click on a volume corner.
    bool onVolumeCorner=false;
    Point corner;
    StatesManager::ViewType view=StatesManager::instance()->viewType();
    if (volumes.size()==1 && volumes.at(0)->selected() && !volumes.at(0)->containsArc() && (view==StatesManager::DisplayModel || view==StatesManager::Display1DModel))
    {
        double x2=_zoom->toPhysicalX(pos);
        double y2=_zoom->toPhysicalY(pos);

        Point p;
        p.setX(x2);
        p.setY(y2);

        double dist=_zoom->toPhysicalLength(10.0);

        // Cursor near a point.
        for (unsigned i=0 ; i<volumes.at(0)->size() ; i++)
        {
            Path *path=volumes.at(0)->at(i);
        
            Point b=path->begin();
            if (GeometryTools::pointDistance(b, p)<=dist)
            {
                corner=b;
                onVolumeCorner=true;
                break;
            }

            Point e=path->end();
            if (GeometryTools::pointDistance(e, p)<=dist)
            {
                corner=e;
                onVolumeCorner=true;
                break;
            }
        }
    }

    if (onVolumeCorner)
    {
        QMenu menu(this);
        QPoint globalPos=mapToGlobal(pos);
        QAction *delete_point=menu.addAction(_tr("DeletePoint"));
        QAction *choice=menu.exec(globalPos);
        if (choice==delete_point)
            deletePointRequested(volumes.at(0), corner);
    }

    // Volume selection.
    else if (!volumes.empty() && volume && !onVolumeCorner)
    {
        QMenu menu(this);
        QPoint globalPos=mapToGlobal(pos);
        QAction *copy=menu.addAction(_tr("Copy"));
        QAction *cut=menu.addAction(_tr("Paste"));
        menu.addSeparator();
        QAction *rotate = menu.addAction(_tr("RotateSelection"));
        menu.addSeparator();

        QAction *remove=menu.addAction(_tr("Delete"));

        QAction *choice=menu.exec(globalPos);
        if (choice==copy)
            emit copyRequested();
        else if (choice==cut)
            emit cutRequested();
        else if (choice==remove)
            emit deleteRequested();
        else if (choice==rotate)
            emit rotationRequested(_rotation.center().x(), _rotation.center().y());
    }

    else
    {
        std::vector<Image*> images=StatesManager::instance()->selectedImages();
        Image *image=model->getImageAtPosition(x, y);

        std::vector<DxfReader::Entity*> dxfEntities=selectedDxfEntities();
        DxfReader::Entity *entity=getDxfEntityAtPosition(x, y, epsilon);

        if (!images.empty() && image && !volume && !entity)
        {
            QMenu menu(this);
            QPoint globalPos=mapToGlobal(pos);
            QAction *set_scale=menu.addAction(_tr("DefineScale"));
            menu.addSeparator();
            QAction *remove=menu.addAction(_tr("Delete"));

            QAction *choice=menu.exec(globalPos);
            if (choice==remove)
                emit deleteRequested();
            else if (choice==set_scale)
                initScaleDefinition();
        }

        else if (!dxfEntities.empty() && entity)
        {
            bool selected_entity=false;
            for (unsigned int i=0 ; i<dxfEntities.size() ; i++)
            {
                if (dxfEntities.at(i)==entity)
                {
                    selected_entity=true;
                    break;
                }
            }
            
            if (selected_entity)
            {
                QMenu menu(this);
                QPoint globalPos=mapToGlobal(pos);
                QAction *import_elements=menu.addAction(_tr("ImportVolume"));

                QAction *choice=menu.exec(globalPos);
                if (choice==import_elements)
                    importSelectedElements();
            }
        }

        else
        {
            std::vector<DxfContent*> selectedDxf=StatesManager::instance()->selectedDxf();
            if (selectedDxf.size()==1)
            {
                DxfContent *content=selectedDxf.at(0);
                if (content->xMin()<=x && content->xMax()>=x && content->yMin()<=y && content->yMax()>=y)
                {
                    QMenu menu(this);
                    QPoint globalPos=mapToGlobal(pos);
                    QAction *remove=menu.addAction(_tr("Delete"));

                    QAction *choice=menu.exec(globalPos);
                    if (choice==remove)
                        emit deleteRequested();
                }
            }
        }
    }
}

std::vector<DxfReader::Entity*> ModelViewer::selectedDxfEntities() const
{
    std::vector<DxfReader::Entity*> entities;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return entities;

    for (unsigned int i=0 ; i<model->std::vector<DxfContent*>::size() ; i++)
    {
        DxfContent *content=model->std::vector<DxfContent*>::at(i);
        DxfReader::Dxf *dxf=dynamic_cast<DxfReader::Dxf*>(content->dxf());
        if (!dxf)
            continue;

        if (!dxf->visible())
            continue;

        for (unsigned int k=0; k<dxf->size(); k++)
        {
            DxfReader::Layer *layer=dxf->at(k);
            if (!layer->isVisible())
                continue;

            for (unsigned int j=0; j<layer->size(); j++)
            {
                DxfReader::Entity *entity=layer->at(j);
                if (entity->selected())
                    entities.push_back(entity);
            }
        }
    }
    return entities;
}

DxfReader::Entity *ModelViewer::getDxfEntityAtPosition(double x, double y, double epsilon) const
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return 0;

    for (unsigned int i=0 ; i<model->std::vector<DxfContent*>::size() ; i++)
    {
        DxfContent *content=model->std::vector<DxfContent*>::at(i);
        DxfReader::Dxf *dxf=dynamic_cast<DxfReader::Dxf*>(content->dxf());
        if (!dxf)
            continue;

        if (!dxf->visible())
            continue;

        for (unsigned int k=0; k<dxf->size(); k++)
        {
            DxfReader::Layer *layer=dxf->at(k);
            if (!layer->isVisible())
                continue;

            for (unsigned int j=0; j<layer->size(); j++)
            {
                DxfReader::Entity *entity=layer->at(j);
                if (entity->containsPoint(x, y, epsilon))
                    return entity;
            }
        }
    }
    return 0;
}

void ModelViewer::initScaleDefinition()
{
    _currentMode=ScaleImage;
    _imageScale.clear();

    QMessageBox dialog;
    dialog.setWindowTitle(_tr("DefineImageScale"));

    QString text=_tr("DefineImageScaleWith2Points");
    dialog.setText(text);
    dialog.setWindowModality(Qt::ApplicationModal);

    QIcon icon(":/images/icon.png");
    dialog.setWindowIcon(icon);
    dialog.setIconPixmap(QPixmap(":/images/icon.png"));

    dialog.exec();
}

void ModelViewer::finalizeScaleDefinition()
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x=_zoom->toPhysicalX(_mouseClickPoint);
    double y=_zoom->toPhysicalY(_mouseClickPoint);

    Point p;
    p.setX(x);
    p.setY(y);
    _imageScale.push_back(p);

    if (_imageScale.size()!=2)
        return;

    std::vector<Image*> images=StatesManager::instance()->selectedImages();
    if (images.size()!=1)
        return;

    Image *image=images.at(0);

    // Scale image.
    double dx=_imageScale.at(1).x()-_imageScale.at(0).x();
    double dy=_imageScale.at(1).y()-_imageScale.at(0).y();
    double length=sqrt(dx*dx+dy*dy);

    // The 2 points must be separated.
    if (length > 1.0e-8)
    {
        // Ask user to input physical distance.
        QString s_physicalLength = QInputDialog::getText(this, _tr("Length"), _tr("LengthMeters"), QLineEdit::Normal, "1,0");
        double physicalLength = ToolBox::convertStringToDouble(s_physicalLength.toStdString());

        if (physicalLength > 1.0e-6)
        {
            double ratio = physicalLength / length;
            double dw = image->width() * (ratio - 1.0);
            double dh = image->height() * (ratio - 1.0);

            ResizeImage *action=new ResizeImage(model, image, 0.0, 0.0, dw, dh);
            ActionsManager::instance()->exec(action);
        }
    }

    _imageScale.clear();
    _currentMode = Idle;
    repaint();
}

void ModelViewer::surfaceSelection(QPoint &pos)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x=_zoom->toPhysicalX(pos);
    double y=_zoom->toPhysicalY(pos);
    double precision=_zoom->toPhysicalLength(5.0);

    // Unselect all surfaces.
    for (unsigned int i=0 ; i<model->std::vector<Surface*>::size() ; i++)
        model->std::vector<Surface*>::at(i)->setSelected(false);

    // Look for a surface.
    Surface *surface=model->getSurfaceAtPosition(x, y, precision);
    if (surface)
        surface->setSelected(true);

    StatesManager::instance()->changed();
}

void ModelViewer::setPhysic(QPoint &pos)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x=_zoom->toPhysicalX(pos);
    double y=_zoom->toPhysicalY(pos);
    double precision=_zoom->toPhysicalLength(5.0);

    // Look for a surface.
    Surface *surface=model->getSurfaceAtPosition(x, y, precision);
    if (surface)
    {
        std::string id=StatesManager::instance()->currentBoundaryConditionId();
        SetSurfaceBoundaryCondition *action=new SetSurfaceBoundaryCondition(model, id, x, y, precision);
        ActionsManager::instance()->exec(action);
        repaint();
    }

    // Look for a volume.
    else
    {
        Volume *volume=model->getVolumeAtPosition(x, y);
        if (volume)
        {
            std::string id=StatesManager::instance()->currentMaterialId();
            SetVolumeMaterial *action=new SetVolumeMaterial(model, id, x, y);
            ActionsManager::instance()->exec(action);

            emit materialUsed(id.c_str());
            repaint();
        }
    }
}

void ModelViewer::setEnvironment(QPoint &pos)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x=_zoom->toPhysicalX(pos);
    double y=_zoom->toPhysicalY(pos);
    double precision=_zoom->toPhysicalLength(5.0);

    // Look for a surface.
    Surface *surface=model->getSurfaceAtPosition(x, y, precision);
    if (surface)
    {
        std::string id=StatesManager::instance()->currentEnvironmentId();
        SetSurfaceEnvironment *action=new SetSurfaceEnvironment(model, id, x, y, precision);
        ActionsManager::instance()->exec(action);
        repaint();
    }
}

bool ModelViewer::mouseOnRotationPoint(const QPoint &pos) const
{
    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (volumes.empty())
        return false;

    QPoint p=_zoom->toScreenPosition(_rotation.center().x(), _rotation.center().y());
    p.setX(p.x()+100);

    double r=(p.x()-pos.x())*(p.x()-pos.x())+(p.y()-pos.y())*(p.y()-pos.y());
    if (r>9.0)
        return false;
    return true;
}

void ModelViewer::selectVolume(const QPoint &point, Qt::KeyboardModifiers modifiers)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x=_zoom->toPhysicalX(point);
    double y=_zoom->toPhysicalY(point);

    // Set text dimensions for annotations.
    for (unsigned int i=0 ; i<model->std::vector<Annotation*>::size() ; i++)
    {
        Annotation *annotation=model->std::vector<Annotation*>::at(i);
        if (annotation->type()==Annotation::AnnotationArrow)
            continue;

        QString t=annotation->text().c_str();

        QFont f=font();
        f.setPixelSize(14);

        QFontMetrics m(f);
        int W=m.horizontalAdvance(t);
        int H=m.height();

        double w=_zoom->toPhysicalLength(W);
        double h=_zoom->toPhysicalLength(H);

        annotation->setTextDimensions(w, h);
    }

    // Get annotations under cursor.
    double precision=_zoom->toPhysicalLength(5.0);
    Annotation *annotation=model->getAnnotationAtPosition(x, y, precision);

    // Get volume under mouse cursor.
    Volume *volume=model->getVolumeAtPosition(x, y);

    // Annotation.
    if (annotation)
    {
        // Ctrl key pressed: add annotation to selection list.
        if (modifiers & Qt::ControlModifier)
        {
            if (annotation->selection() == Annotation::Whole)
                annotation->setSelection(Annotation::NoSelection);
            else
                annotation->select(x, y, precision);
        }

        // No Ctrl key pressed: select only one annotation.
        else
        {
            model->unselectAllAnnotations();
            annotation->select(x, y, precision);
        }

        model->unselectAllVolumes();
        model->unselectAllImages();
        model->unselectAllDxf();
    }

    // Volume.
    else if (volume)
    {
        // Ctrl key pressed: add volume to selection list.
        if (modifiers & Qt::ControlModifier)
            volume->setSelected(!volume->selected());

        // No Ctrl key pressed: select only one volume.
        else
        {
            model->unselectAllVolumes();
            volume->setSelected(true);
        }

        model->unselectAllAnnotations();
        model->unselectAllImages();
        model->unselectAllDxf();
        updateRotationCenter();
    }

    // No volume: test image.
    else if (StatesManager::instance()->showImages())
    {
        double epsilon=_zoom->toPhysicalLength(20.0);
        Image *image=model->getImageAtPosition(x, y, epsilon);
        if (image)
        {
            model->unselectAllImages();
            image->setSelected(true);
            _resizeType=getResizeType(image, point);
        }
        else
            model->unselectAllImages();
        model->unselectAllAnnotations();
        model->unselectAllVolumes();
        model->unselectAllDxf();

        // No image: test dxf.
        if (StatesManager::instance()->showDxf() && !image && !volume)
        {
            double epsilon=_zoom->toPhysicalLength(4.0);
            selectDxfEntities(x, y, epsilon, (modifiers & Qt::ControlModifier));
        }
    }
    
    StatesManager::instance()->changed();
    repaint();
}

void ModelViewer::updateRotationCenter()
{
    // Define rotation center.
    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (!volumes.empty())
    {
        double c_x=0.0;
        double c_y=0.0;
        for (unsigned int i=0 ; i<volumes.size() ; i++)
        {
            Point m=volumes.at(i)->middle();
            c_x+=m.x();
            c_y+=m.y();
        }
        c_x=c_x/double(volumes.size());
        c_y=c_y/double(volumes.size());

        Point c;
        c.setX(c_x);
        c.setY(c_y);

        _rotation.setCenter(c);
    }
}

void ModelViewer::deleteCurrentDrawnVolume()
{
    if (_currentDrawnVolume)
        delete _currentDrawnVolume;
    _currentDrawnVolume=0;
    repaint();
}

void ModelViewer::drawRectangle(const QPoint &point, bool snap)
{
    deleteCurrentDrawnVolume();

    Point p=nearPoint(point, snap);

    Rectangle *rectangle=new Rectangle;
    _currentDrawnVolume=rectangle;

    std::string materialId=StatesManager::instance()->currentMaterialId();
    Material *material=Materials::instance()->materialFromId(materialId);

    rectangle->create(p.x(), p.y(), 0.0, 0.0);
    rectangle->setMaterial(material);
}

void ModelViewer::drawCircle(const QPoint &point, bool snap)
{
    deleteCurrentDrawnVolume();

    Point p=nearPoint(point, snap);

    Circle *circle=new Circle;
    _currentDrawnVolume=circle;

    std::string materialId=StatesManager::instance()->currentMaterialId();
    Material *material=Materials::instance()->materialFromId(materialId);

    circle->create(p.x(), p.y(), 0.0);
    circle->setMaterial(material);
}

void ModelViewer::drawEllipse(const QPoint &point, bool snap)
{
    deleteCurrentDrawnVolume();

    Point p=nearPoint(point, snap);

    Ellipse *ellipse=new Ellipse;
    _currentDrawnVolume=ellipse;

    std::string materialId=StatesManager::instance()->currentMaterialId();
    Material *material=Materials::instance()->materialFromId(materialId);

    ellipse->create(p.x(), p.y(), 0.0, 0.0);
    ellipse->setMaterial(material);
}

void ModelViewer::removePolylinePoint()
{
    if (_polylinePoints.empty())
        return;

    _polylinePoints.erase(_polylinePoints.end()-1);
    update();
}

void ModelViewer::drawPolyline(const QPoint &point, bool snap, bool vh_align)
{
    Point p=nearPoint(point, snap);

    double x=p.x();
    double y=p.y();

    if (vh_align && !_polylinePoints.empty())
    {
        Point begin=_polylinePoints.at(_polylinePoints.size()-1);
        double dx=std::fabs(begin.x()-p.x());
        double dy=std::fabs(begin.y()-p.y());
        if (dx>dy)
            y=begin.y();
        else
            x=begin.x();
    }

    p.setX(x);
    p.setY(y);

    // If current point is equal to first point: finalize polyline.
    if (_polylinePoints.size()>2)
    {
        Point p1=_polylinePoints.at(0);
        if (GeometryTools::equivalentPoints(p1, p))
            return finalizePolyline();
    }

    bool add=true;
    if (_polylinePoints.size()>1 && GeometryTools::equivalentPoints(p, _polylinePoints.at(_polylinePoints.size()-1)))
        add=false;

    if (!polylineIntersection(x, y) && add)
        _polylinePoints.push_back(p);
    update();
}

void ModelViewer::drawLengthDataPoint(const QPoint &point, bool snap, bool vh_align)
{
    Point p=nearPoint(point, snap);

    _lengthDataPoint.push_back(p);
    finalizeLengthData();
}

void ModelViewer::finalizeLengthData()
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    if (_lengthDataPoint.size()<2)
        return;

    Annotation annotation;
    annotation.setType(Annotation::AnnotationCotation);

    Point begin;
    begin.setX(_lengthDataPoint.at(0).x());
    begin.setY(_lengthDataPoint.at(0).y());
    annotation.setBegin(begin);

    Point end;
    end.setX(_lengthDataPoint.at(1).x());
    end.setY(_lengthDataPoint.at(1).y());
    annotation.setEnd(end);

    double L=_zoom->toPhysicalLength(5.0);

    double x0=_lengthDataPoint.at(0).x();
    double y0=_lengthDataPoint.at(0).y();
    double x1=_lengthDataPoint.at(1).x();
    double y1=_lengthDataPoint.at(1).y();

    double dx=x1-x0;
    double dy=y1-y0;
    double l=sqrt(dx*dx+dy*dy);

    dx=dx/l;
    dy=dy/l;
            
    double n_ortho_x=dy;
    double n_ortho_y=-dx;

    Point t;
    t.setX(0.5*(x0+x1)-L*n_ortho_x);
    t.setY(0.5*(y0+y1)-L*n_ortho_y);
    annotation.setPosition(t);

    annotation.updateText();

    AddAnnotation *action=new AddAnnotation(model, annotation);
    ActionsManager::instance()->exec(action);

    _lengthDataPoint.clear();
}

void ModelViewer::drawAngleDataPoint(const QPoint &point, bool snap, bool vh_align)
{
    Point p=nearPoint(point, snap);

    _angleDataPoint.push_back(p);
    finalizeAngleData();
}

void ModelViewer::addArrow(const QPoint &point, bool snap, bool vh_align)
{
    Point p=nearPoint(point, snap);

    _arrowPoint.push_back(p);
    finalizeArrow();
}

void ModelViewer::finalizeArrow()
{
    if (_arrowPoint.size()<2)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    Annotation annotation;
    annotation.setType(Annotation::AnnotationArrow);
    annotation.setBegin(_arrowPoint.at(0));
    annotation.setEnd(_arrowPoint.at(1));

    AddAnnotation *action=new AddAnnotation(model, annotation);
    ActionsManager::instance()->exec(action);

    _arrowPoint.clear();
    repaint();
}

void ModelViewer::finalizeAngleData()
{
    if (_angleDataPoint.size()<3)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    Annotation annotation;
    annotation.setType(Annotation::AnnotationAngle);

    Point center;
    center.setX(_angleDataPoint.at(0).x());
    center.setY(_angleDataPoint.at(0).y());
    annotation.setAngleCenter(center);

    Point begin;
    begin.setX(_angleDataPoint.at(1).x());
    begin.setY(_angleDataPoint.at(1).y());
    annotation.setBegin(begin);

    Point end;
    end.setX(_angleDataPoint.at(2).x());
    end.setY(_angleDataPoint.at(2).y());
    annotation.setEnd(end);

    double xm=0.5*(begin.x()+end.x());
    double ym=0.5*(begin.y()+end.y());

    double nx=xm-center.x();
    double ny=ym-center.y();

    double n=sqrt(nx*nx+ny*ny);
    nx=nx/n;
    ny=ny/n;

    double L=_zoom->toPhysicalLength(30.0);

    double xp=center.x()+L*nx;
    double yp=center.y()+L*ny;

    Point t;
    t.setX(xp);
    t.setY(yp);
    annotation.setPosition(t);

    annotation.updateText();

    AddAnnotation *action=new AddAnnotation(model, annotation);
    ActionsManager::instance()->exec(action);

    _angleDataPoint.clear();
}

bool ModelViewer::polylineIntersection(double x, double y) const
{
    if (_polylinePoints.size()<2)
        return false;

    Point p;
    p.setX(x);
    p.setY(y);

    Line l2;
    l2.setBegin(_polylinePoints.at(_polylinePoints.size()-1), 0);
    l2.setEnd(p, 0);


    for (unsigned int i=0 ; i<_polylinePoints.size()-1 ; i++)
    {
        Point p1=_polylinePoints.at(i);
        Point p2=_polylinePoints.at(i+1);

        Line l1;
        l1.setBegin(p1, 0);
        l1.setEnd(p2, 0);

        if (GeometryTools::intersectsWithoutBounds(&l1, &l2))
            return true;
    }

    return false;
}

void ModelViewer::finalizePolyline()
{
    if (_polylinePoints.size()<3)
        return;

    // Close volume.
    double x=_polylinePoints.at(0).x();
    double y=_polylinePoints.at(0).y();

    if (polylineIntersection(x, y))
    {
        QMessageBox dialog;
        dialog.setWindowTitle(_tr("ClosedPathInsertion"));
        QString text=_tr("ClosedPathInterDetected");
        dialog.setText(text);
        dialog.setWindowModality(Qt::ApplicationModal);
        QIcon icon(":/images/icon.png");
        dialog.setWindowIcon(icon);
        dialog.setIconPixmap(QPixmap(":/images/orange-blue/warning.png"));
        dialog.exec();
        return;
    }

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::string materialId=StatesManager::instance()->currentMaterialId();

    AddPolyline *action=new AddPolyline(model, _polylinePoints, materialId);
    ActionsManager::instance()->exec(action);

    if (!materialId.empty())
        emit materialUsed(materialId.c_str());

    _polylinePoints.clear();
    update();
}

void ModelViewer::updateEllipseGeometry(double x, double y)
{
    Ellipse *ellipse=dynamic_cast<Ellipse *>(_currentDrawnVolume);
    if (ellipse)
    {
        double dx=x-ellipse->x();
        double dy=y-ellipse->y();
        ellipse->setHorizontalRadius(dx);
        ellipse->setVerticalRadius(dy);
        repaint();

        emit mouseDimensionChanged(std::fabs(dx), std::fabs(dy));
    }
}

void ModelViewer::finalizeEllipse()
{
    Ellipse *ellipse=dynamic_cast<Ellipse *>(_currentDrawnVolume);
    if (!ellipse)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    if (ellipse->horizontalRadius()>0.0 && ellipse->verticalRadius()>0.0)
    {
        AddEllipse *action=new AddEllipse(model, ellipse);
        ActionsManager::instance()->exec(action);

        std::string id=StatesManager::instance()->currentMaterialId();
        if (!id.empty())
            emit materialUsed(id.c_str());
    }

    repaint();
}

void ModelViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (StatesManager::instance()->simulationState()==StatesManager::InProgress)
        return;

    // Scale image.
    if (_currentMode==ScaleImage)
        return;

    // Material selection by mouse.
    if (StatesManager::instance()->interfaceMode() == StatesManager::GetMaterial)
        return selectMaterial(event);

    StatesManager::InterfaceMode mode=StatesManager::instance()->interfaceMode();
    StatesManager::ViewType view=StatesManager::instance()->viewType();

    // Mouse selection zone size.
    double x0 = _currentSelectionBegin.x();
    double y0 = _currentSelectionBegin.y();

    double x1 = _currentSelectionEnd.x();
    double y1 = _currentSelectionEnd.y();

    double w = std::fabs(x1 - x0);
    double h = std::fabs(y1 - y0);

    double W = _zoom->toScreenLength(w);
    double H = _zoom->toScreenLength(h);

    bool useSelection = false;
    if (W > 10 /* px */ && H > 10 /* px */)
        useSelection = true;

    // Left mouse click.
    if (event->button()==Qt::LeftButton)
    {
        if (view==StatesManager::DisplayModel || view==StatesManager::Display1DModel)
        {
            // Volume selection.
            if (mode==StatesManager::Selection && _dxTranslation==0.0 && _dyTranslation==0.0 && _rotation.angle()==0.0 && _dxPointTranslation==0.0 && _dyPointTranslation==0.0)
                selectVolume(_mouseClickPoint, event->modifiers());

            // Set material or boundary condition.
            else if (mode==StatesManager::SetPhysics && !useSelection)
                setPhysic(_mouseClickPoint);

            // Set environment.
            else if (mode==StatesManager::SetEnvironments && !useSelection)
                setEnvironment(_mouseClickPoint);
        }

        // Results.
        else if (view==StatesManager::DisplayResults || view==StatesManager::Display1DResults || view==StatesManager::DisplayCondensationResults || view==StatesManager::DisplayHumidityResults)
        {
            // Surface selection.
            surfaceSelection(_mouseClickPoint);
        }
    }

    // Add rectangle.
    if (StatesManager::instance()->interfaceMode()==StatesManager::AddRectangle)
        finalizeRectangle();

    // Add circle.
    else if (StatesManager::instance()->interfaceMode()==StatesManager::AddCircle)
        finalizeCircle();

    // Add ellipse.
    else if (StatesManager::instance()->interfaceMode()==StatesManager::AddEllipse)
        finalizeEllipse();

    // Translate selection.
    else if (_currentMode==SelectionTranslation && mode==StatesManager::Selection)
        finalizeSelectionTranslation();

    // Rotate selection.
    else if (_currentMode==SelectionRotation && mode==StatesManager::Selection)
        finalizeSelectionRotation();

    // Translation corner.
    else if (_currentMode==VolumeCornerTranslation && mode==StatesManager::Selection)
        finalizeCornerTranslation();

    // Translation border.
    else if (_currentMode==VolumeBorderTranslation && mode==StatesManager::Selection)
        finalizeBorderTranslation();

    deleteCurrentDrawnVolume();

    _currentMode          = Idle;
    _dxTranslation        = 0.0;
    _dyTranslation        = 0.0;
    _hideRotation         = false;
    _annotationActionType = AnnotationNoAction;

    // Mouse selection.
    if (_mousePressed && StatesManager::instance()->interfaceMode()==StatesManager::Selection)
    {
        selectVolumesUnderMouseSelection();
        emit hideMouseDimension();
        _mousePressed=false;
    }

    // Mouse environments selection.
    if (mode == StatesManager::SetEnvironments && (view == StatesManager::DisplayModel || view == StatesManager::Display1DModel))
    {
        setEnvironmentFromSelection();
        emit hideMouseDimension();
        _mousePressed = false;
    }

    // Mouse material/bc selection.
    if (useSelection && mode == StatesManager::SetPhysics && (view == StatesManager::DisplayModel || view == StatesManager::Display1DModel))
    {
        setPhysicFromSelection();
        emit hideMouseDimension();
        _mousePressed = false;
    }

    _mousePressed = false;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x=_zoom->toPhysicalX(event->pos());
    double y=_zoom->toPhysicalY(event->pos());

    Volume *volume=model->getVolumeAtPosition(x, y);
    if (volume && volume->selected())
    {
        setCursor(QCursor(Qt::SizeAllCursor));
    }
    else
    {
        setCursor(QCursor(Qt::ArrowCursor));
    }
    
    updateRotationCenter();
    repaint();
}

void ModelViewer::mouseDoubleClickEvent(QMouseEvent *event)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    // Alter text annotation content.
    if (StatesManager::instance()->interfaceMode() == StatesManager::Selection
        && _currentMode==Idle
        && (StatesManager::instance()->viewType()==StatesManager::DisplayModel || StatesManager::instance()->viewType()==StatesManager::Display1DModel))
    {
        // Get annotations under cursor.
        double precision=_zoom->toPhysicalLength(5.0);
        double x=_zoom->toPhysicalX(event->pos());
        double y=_zoom->toPhysicalY(event->pos());

        Annotation *annotation=model->getAnnotationAtPosition(x, y, precision);

        if (annotation && annotation->type() == Annotation::AnnotationText && annotation->selection() != Annotation::NoSelection)
        {
            updateText(annotation);
        }
    }
}

void ModelViewer::updateText(Annotation *annotation)
{
    if (!annotation)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    QString text=QInputDialog::getText(this, _tr("TextAnnotation"), _tr("TextTitle"), QLineEdit::Normal, annotation->text().c_str());
    if (text.isEmpty())
        return;

    UpdateTextAnnotationContent *action=new UpdateTextAnnotationContent(model, annotation->id(), text.toStdString());
    ActionsManager::instance()->exec(action);

    repaint();
}

void ModelViewer::selectVolumesUnderMouseSelection()
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    Rectangle selection;
    double x=std::min(_currentSelectionBegin.x(), _currentSelectionEnd.x());
    double y=std::min(_currentSelectionBegin.y(), _currentSelectionEnd.y());
    double w=std::fabs(_currentSelectionBegin.x()-_currentSelectionEnd.x());
    double h=std::fabs(_currentSelectionBegin.y()-_currentSelectionEnd.y());

    if (w<=1.0e-9)
        return;
    if (h<=1.0e-9)
        return;

    selection.create(x, y, w, h);
    
    for (unsigned int i=0 ; i<model->std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=model->std::vector<Volume*>::at(i);
        bool intersection=volume->intersectionWithVolume(&selection);
        if (intersection)
        {
            volume->setSelected(false);
        }
        else
        {
            if (!volume->empty())
            {
                Path *path=volume->at(0);
                Point p=path->begin();
                bool containPoint=selection.containsPoint(p);
                volume->setSelected(containPoint);
            }
        }
    }
    StatesManager::instance()->changed();
}

void ModelViewer::translateWithKeyboard(int key)
{
    if (StatesManager::instance()->selectedVolumes().empty() &&
        StatesManager::instance()->selectedImages().empty())
        return;

    if (key==Qt::Key_Left)
        _dxTranslation=-_zoom->gridStep();
    else if (key==Qt::Key_Right)
        _dxTranslation=_zoom->gridStep();
    else if (key==Qt::Key_Up)
        _dyTranslation=_zoom->gridStep();
    else if (key==Qt::Key_Down)
        _dyTranslation=-_zoom->gridStep();

    finalizeSelectionTranslation();
}

void ModelViewer::finalizeSelectionTranslation()
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    if (_dxTranslation==0.0 && _dyTranslation==0.0)
    {
        _currentMode=Idle;
        return;
    }

    UserAction *action=0;

    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (!volumes.empty())
    {
        double dx;
        double dy;
        if (_ctrlPressed)
        {
            dx=_dxTranslation;
            dy=_dyTranslation;
        }
        else
        {
            dx=int(_dxTranslation/_zoom->gridStep())*_zoom->gridStep();
            dy=int(_dyTranslation/_zoom->gridStep())*_zoom->gridStep();
        }

        if (_majPressed)
        {
            if (std::fabs(dx)>std::fabs(dy))
                dy=0.0;
            else
                dx=0.0;
        }
        action=new TranslateVolumes(model, dx, dy);
    }

    std::vector<Image*> images=StatesManager::instance()->selectedImages();
    if (!images.empty())
    {
        Image *image=images.at(0);
        if (_resizeType==Translation)
            action=new TranslateImage(model, image, _dxTranslation, _dyTranslation);
        else
        {
            double w=image->width();
            double h=image->height();
            double dx=0.0;
            double dy=0.0;
            double dw=0.0;
            double dh=0.0;

            if (_resizeType & Right)
                dw=_dxTranslation;

            if (_resizeType & Left)
                dw=-_dxTranslation;

            if (_resizeType & Top)
                dh=_dyTranslation;

            if (_resizeType & Bottom)
                dh=-_dyTranslation;

            // Min size.
            double min_distance=_zoom->toPhysicalLength(10.0);
            if (h+dh<min_distance)
                dh=min_distance-h;
            if (w+dw<min_distance)
                dw=min_distance-w;

            // Keep ratio.
            if (dw!=0.0 && dh!=0.0)
                dw=w/h*dh;

            if (_resizeType & Left)
                dx=-dw;

            if (_resizeType & Bottom)
                dy=-dh;

            action=new ResizeImage(model, image, dx, dy, dw, dh);
        }
    }

    std::vector<Annotation*> annotations = StatesManager::instance()->selectedAnnotations();
    if (!annotations.empty())
    {
        double dx;
        double dy;

        if (_ctrlPressed)
        {
            dx = _dxTranslation;
            dy = _dyTranslation;
        }
        else
        {
            dx = int(_dxTranslation / _zoom->gridStep()) * _zoom->gridStep();
            dy = int(_dyTranslation / _zoom->gridStep()) * _zoom->gridStep();
        }

        if (_majPressed)
        {
            if (std::fabs(dx) > std::fabs(dy))
                dy=0.0;
            else
                dx=0.0;
        }

        std::vector<std::string> ids;
        AnnotationSelections selections;
        for (unsigned int i=0 ; i<annotations.size() ; i++)
        {
            ids.push_back(annotations.at(i)->id());
            selections.push_back(annotations.at(i)->selection());
        }

        double begin_dx = 0.0;
        double begin_dy = 0.0;
        double end_dx   = 0.0;
        double end_dy   = 0.0;
        double mid_dx   = 0.0;
        double mid_dy   = 0.0;
        double ang_dx   = 0.0;
        double ang_dy   = 0.0;

        if (_annotationActionType == AnnotationTranslation)
        {
            begin_dx = dx;
            end_dx   = dx;
            mid_dx   = dx;
            ang_dx   = dx;

            begin_dy = dy;
            end_dy   = dy;
            mid_dy   = dy;
            ang_dy   = dy;
        }

        else if (_annotationActionType == AnnotationBeginTranslation)
        {
            begin_dx = dx;
            begin_dy = dy;
        }

        else if (_annotationActionType == AnnotationEndTranslation)
        {
            end_dx = dx;
            end_dy = dy;
        }

        else if (_annotationActionType == AnnotationCenterTranslation)
        {
            ang_dx = dx;
            ang_dy = dy;
        }

        action = new UpdateAnnotationPoints(model, selections, ids, begin_dx, begin_dy, end_dx, end_dy, mid_dx, mid_dy, ang_dx, ang_dy);
    }

    _currentMode=Idle;
    _dxTranslation=0.0;
    _dyTranslation=0.0;

    if (action)
    {
        ActionsManager::instance()->exec(action);
        updateRotationCenter();
    }
}

void ModelViewer::finalizeBorderTranslation()
{
    if (!_mouseOverVolume)
        return;
    if (!_mouseOverPath)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    if (_lastDxPointTranslation==0.0 && _lastDyPointTranslation==0.0)
    {
        _currentMode=Idle;
        _mouseOverPath=0;
        _mouseOverVolume=0;
        return;
    }

    double dx;
    double dy;
    if (_ctrlPressed)
    {
        dx=_lastDxPointTranslation;
        dy=_lastDyPointTranslation;
    }
    else
    {
        dx=int(_lastDxPointTranslation/_zoom->gridStep())*_zoom->gridStep();
        dy=int(_lastDyPointTranslation/_zoom->gridStep())*_zoom->gridStep();
    }

    if (_majPressed)
    {
        if (std::fabs(dx)>std::fabs(dy))
            dy=0.0;
        else
            dx=0.0;
    }

    TranslateVolumeBorder *action=new TranslateVolumeBorder(model, _mouseOverVolume, _mouseOverPath, dx, dy);

    _currentMode=Idle;
    _dxPointTranslation=0.0;
    _dyPointTranslation=0.0;
    _lastDxPointTranslation=0.0;
    _lastDyPointTranslation=0.0;
    _mouseOverElementType=Nothing;
    _mouseOverPath=0;
    _mouseOverVolume=0;

    if (action)
    {
        ActionsManager::instance()->exec(action);
        updateRotationCenter();
    }
}

void ModelViewer::finalizeCornerTranslation()
{
    if (!_mouseOverVolume)
        return;

     Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    if (_lastDxPointTranslation==0.0 && _lastDyPointTranslation==0.0)
    {
        _currentMode=Idle;
        _mouseOverPath=0;
        _mouseOverVolume=0;
        return;
    }

    double dx;
    double dy;
    if (_ctrlPressed)
    {
        dx=_lastDxPointTranslation;
        dy=_lastDyPointTranslation;
    }
    else
    {
        dx=int(_lastDxPointTranslation/_zoom->gridStep())*_zoom->gridStep();
        dy=int(_lastDyPointTranslation/_zoom->gridStep())*_zoom->gridStep();
    }

    if (_majPressed)
    {
        if (std::fabs(dx)>std::fabs(dy))
            dy=0.0;
        else
            dx=0.0;
    }

    TranslateVolumeCorner *action=new TranslateVolumeCorner(model, _mouseOverVolume, _mouseOverPoint, dx, dy);

    _currentMode=Idle;
    _dxPointTranslation=0.0;
    _dyPointTranslation=0.0;
    _lastDxPointTranslation=0.0;
    _lastDyPointTranslation=0.0;
    _mouseOverElementType=Nothing;
    _mouseOverPath=0;
    _mouseOverVolume=0;

    if (action)
    {
        ActionsManager::instance()->exec(action);
        updateRotationCenter();
    }
}

void ModelViewer::finalizeSelectionRotation()
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    RotateVolumes *action=new RotateVolumes(model, _rotation.center().x(), _rotation.center().y(), _rotation.angle());

    _currentMode=Idle;

    ActionsManager::instance()->exec(action);
    _rotation.setAngle(0.0);
    updateRotationCenter();
}

void ModelViewer::keyPressEvent(QKeyEvent *event)
{
    // Object translation.
    if (StatesManager::instance()->interfaceMode()==StatesManager::Selection &&
        (event->key()==Qt::Key_Left || event->key()==Qt::Key_Up || event->key()==Qt::Key_Right || event->key()==Qt::Key_Down))
        translateWithKeyboard(event->key());

    // Finalize polyline.
    if (StatesManager::instance()->interfaceMode()==StatesManager::AddPolyline && (event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return))
        finalizePolyline();

    // Cancel only.
    if (event->key() != Qt::Key_Escape)
        return;

    // Cancel selection translation.
    if (_currentMode==SelectionTranslation || _currentMode==VolumeCornerTranslation || _currentMode==VolumeBorderTranslation)
    {
        _currentMode=Idle;
        _dxTranslation=0.0;
        _dyTranslation=0.0;
        repaint();
    }

    // Cancel selection rotation.
    else if (_currentMode==SelectionRotation)
    {
        _currentMode=Idle;
        _rotation.setAngle(0.0);
        repaint();
    }

    // Cancel image scaling.
    else if (_currentMode==ScaleImage)
    {
        _currentMode=Idle;
        repaint();
    }

    // Annotations.
    _lengthDataPoint.clear();
    _angleDataPoint.clear();
    _arrowPoint.clear();

    Model *model=StatesManager::instance()->currentModelView();
    if (model)
    {
        model->unselectAllVolumes();
        model->unselectAllImages();
        model->unselectAllDxf();
    }

    deleteCurrentDrawnVolume();
    _polylinePoints.clear();
    _mouseOverElementType=Nothing;
    update();
}

void ModelViewer::finalizeRectangle()
{
    Rectangle *rectangle=dynamic_cast<Rectangle *>(_currentDrawnVolume);
    if (!rectangle)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    if (rectangle->width()!=0.0 && rectangle->height()!=0.0)
    {
        AddRectangle *action=new AddRectangle(model, rectangle);
        ActionsManager::instance()->exec(action);

        std::string id=StatesManager::instance()->currentMaterialId();
        if (!id.empty())
            emit materialUsed(id.c_str());
    }

    emit hideMouseDimension();

    repaint();
}

void ModelViewer::finalizeCircle()
{
    Circle *circle=dynamic_cast<Circle *>(_currentDrawnVolume);
    if (!circle)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    if (circle->radius()>0.0)
    {
        AddCircle *action=new AddCircle(model, circle);
        ActionsManager::instance()->exec(action);

        std::string id=StatesManager::instance()->currentMaterialId();
        if (!id.empty())
            emit materialUsed(id.c_str());
    }

    repaint();
}

void ModelViewer::paintEvent(QPaintEvent *event)
{
    QPainter qp(this);

    // Draw grid and origin.
    drawGridAndOrigin(qp, NoOptions);

    // Draw volumes & surfaces.
    Model *model=StatesManager::instance()->currentModelView();
    if (model)
    {
        drawDxf(model, qp);
        drawImages(model, qp);
        drawModel(model, qp);

        if (StatesManager::instance()->viewType()!=StatesManager::DisplayResults
            && StatesManager::instance()->viewType()!=StatesManager::Display1DResults
            && StatesManager::instance()->viewType()!=StatesManager::DisplayMesh)
            drawAnnotations(model, qp);
    }

    // Draw image scale line.
    if (_currentMode==ScaleImage && _imageScale.size()>=1)
    {
        QPen p=qp.pen();
        p.setColor(Qt::red);
        p.setWidth(3);
        p.setStyle(Qt::SolidLine);
        qp.setPen(p);
        
        QPoint p1=_zoom->toScreenPosition(_imageScale.at(0).x(), _imageScale.at(0).y());
        QPoint p2=mapFromGlobal(QCursor::pos());
        if (_imageScale.size()==2)
            p2=_zoom->toScreenPosition(_imageScale.at(1).x(), _imageScale.at(1).y());

        qp.drawLine(p1, p2);
    }

    // Draw length data.
    QPen p=qp.pen();
    p.setWidth(2);
    p.setColor(Qt::red);
    qp.setPen(p);

    if (_lengthDataPoint.size()==1)
    {
        Annotation annotation;
        annotation.setType(Annotation::AnnotationCotation);

        Point begin;
        begin.setX(_lengthDataPoint.at(0).x());
        begin.setY(_lengthDataPoint.at(0).y());
        annotation.setBegin(begin);

        double x0=_lengthDataPoint.at(0).x();
        double y0=_lengthDataPoint.at(0).y();

        Point p=nearPoint(_mousePosition, !_ctrlPressed);

        double x1=p.x();
        double y1=p.y();

        Point end;
        end.setX(x1);
        end.setY(y1);
        annotation.setEnd(end);

        double dx=x1-x0;
        double dy=y1-y0;
        double l=sqrt(dx*dx+dy*dy);

        if (std::fabs(l)>1.0e-6)
        {
            annotation.updateText();

            double L=_zoom->toPhysicalLength(5.0);

            dx=dx/l;
            dy=dy/l;
            
            double n_ortho_x=dy;
            double n_ortho_y=-dx;

            Point t;
            t.setX(0.5*(x0+x1)-L*n_ortho_x);
            t.setY(0.5*(y0+y1)-L*n_ortho_y);
            annotation.setPosition(t);

            drawAnnotation(qp, &annotation);
        }
    }

    // Draw angle data.
    if (_angleDataPoint.size()>0)
    {
        Point p=nearPoint(_mousePosition, !_ctrlPressed);

        Annotation annotation;
        annotation.setType(Annotation::AnnotationAngle);

        Point center;
        center.setX(_angleDataPoint.at(0).x());
        center.setY(_angleDataPoint.at(0).y());
        annotation.setAngleCenter(center);

        Point begin;
        if (_angleDataPoint.size()==2)
        {
            begin.setX(_angleDataPoint.at(1).x());
            begin.setY(_angleDataPoint.at(1).y());
        }
        else
        {
            begin.setX(p.x());
            begin.setY(p.y());
        }
        annotation.setBegin(begin);

        Point end;
        end.setX(p.x());
        end.setY(p.y());
        annotation.setEnd(end);

        double xm=0.5*(begin.x()+end.x());
        double ym=0.5*(begin.y()+end.y());

        double nx=xm-center.x();
        double ny=ym-center.y();

        double n=sqrt(nx*nx+ny*ny);
        nx=nx/n;
        ny=ny/n;

        double L=_zoom->toPhysicalLength(30.0);

        double xp=center.x()+L*nx;
        double yp=center.y()+L*ny;

        Point t;
        t.setX(xp);
        t.setY(yp);
        annotation.setPosition(t);

        annotation.updateText();

        drawAnnotation(qp, &annotation);
    }

    // Draw current drawn arrow.
    if (_arrowPoint.size()==1)
    {
        Annotation annotation;
        annotation.setType(Annotation::AnnotationArrow);
        annotation.setBegin(_arrowPoint.at(0));

        Point p=nearPoint(_mousePosition, !_ctrlPressed);
        annotation.setEnd(p);

        if (annotation.validArrow())
        {
            QPoint b=_zoom->toScreenPosition(_arrowPoint.at(0).x(), _arrowPoint.at(0).y());
            QPoint e=_zoom->toScreenPosition(p.x(), p.y());

            qp.drawLine(b, e);

            Point p1=annotation.secondArrowFirstPoint();
            Point p2=annotation.secondArrowSecondPoint();

            QPoint qp1=_zoom->toScreenPosition(p1.x(), p1.y());
            QPoint qp2=_zoom->toScreenPosition(p2.x(), p2.y());

            qp.drawLine(e, qp1);
            qp.drawLine(e, qp2);
        }
    }

    // Draw mouse selection.
    drawMouseSelection(qp);
}

void ModelViewer::drawMouseSelection(QPainter &painter)
{
    if (!_mousePressed)
        return;

    if (_currentMode!=Idle)
        return;

    if (StatesManager::instance()->interfaceMode()!=StatesManager::Selection
            && StatesManager::instance()->interfaceMode()==StatesManager::SetPhysics
            && StatesManager::instance()->interfaceMode()==StatesManager::SetEnvironments)
        return;

    QPen p=painter.pen();
    p.setColor(QColor(13, 255, 143));
    p.setWidth(1);

    painter.setPen(p);
    painter.setBrush(QColor(202, 255, 203, 50));

    QPoint b=_zoom->toScreenPosition(_currentSelectionBegin.x(), _currentSelectionBegin.y());
    QPoint e=_zoom->toScreenPosition(_currentSelectionEnd.x(), _currentSelectionEnd.y());
    painter.drawRect(QRect(b, e));

    double w=std::fabs(_currentSelectionEnd.x()-_currentSelectionBegin.x());
    double h=std::fabs(_currentSelectionEnd.y()-_currentSelectionBegin.y());

    emit mouseDimensionChanged(w, h);
}

void ModelViewer::drawGridAndOrigin(QPainter &qp, ImageOptions options)
{
    // Get (0, height) screen point physical values.
    double x=_zoom->toPhysicalX(QPoint(0, height()));
    double y=_zoom->toPhysicalY(QPoint(0, height()));
    double gridStep=_zoom->gridStep();

    // Round wrt grid step.
    int nx=int(x/gridStep)-1;
    int ny=int(y/gridStep)-1;

    double X=nx*gridStep;
    double Y=ny*gridStep;

    // Compute width & height.
    double w=_zoom->toPhysicalLength(width());
    double h=_zoom->toPhysicalLength(height());

    int NX=int(w/gridStep)+2;
    int NY=int(h/gridStep)+2;

    // Draw origin.
    QPoint origin=_zoom->toScreenPosition(0.0, 0.0);
    int l=50;
    int x0=origin.x()-l;
    if (x0<=0)
        x0=0;
    int x1=origin.x()+l;
    if (x1>=width())
        x1=width();

    int y0=origin.y()-l;
    if (y0<=0)
        y0=0;
    int y1=origin.y()+l;
    if (y1>=height())
        y1=height();

    QPen p=qp.pen();
    p.setWidth(1);
    p.setStyle(Qt::SolidLine);
    qp.setPen(p);

    // Paint grid.
    std::vector<double> x_marks;
    int di=(round(-X/gridStep)%5+5)%5;
    for (unsigned int i=0 ; i<NX ; i++)
    {
        if (i%5==di)
        {
            qp.setPen(QColor(159, 159, 159));
            double Xs=_zoom->toScreenPosition(X+i*gridStep, 0).x();
            if (Xs<width()-10 && Xs>0)
                x_marks.push_back(X+i*gridStep);
        }
        else
            qp.setPen(QColor(216, 216, 216));
        QPoint p=_zoom->toScreenPosition(X+i*gridStep, 0.0);
        qp.drawLine(p.x(), 0, p.x(), height());
    }

    int dj=(round(-Y/gridStep)%5+5)%5;
    std::vector<double> y_marks;
    for (unsigned int j=0 ; j<NY ; j++)
    {
        if (j%5==dj)
        {
            qp.setPen(QColor(159, 159, 159));
            double Ys=_zoom->toScreenPosition(0, Y+j*gridStep).y();
            if (Ys>0 && Ys<height()-10)
                y_marks.push_back(Y+j*gridStep);
        }
        else
            qp.setPen(QColor(216, 216, 216));
        QPoint p=_zoom->toScreenPosition(0.0, Y+j*gridStep);
        qp.drawLine(0, p.y(), width(), p.y());
    }

    // Origin.
    qp.setPen(Qt::black);
    qp.drawLine(x0, origin.y(), x1, origin.y());
    qp.drawLine(origin.x(), y0, origin.x(), y1);

    // Scale.
    if (options && x_marks.size()>=2 && y_marks.size()>=1)
    {
        double begin=x_marks.at(x_marks.size()-2);
        double end=x_marks.at(x_marks.size()-1);

        double y=y_marks.at(0);

        QPen p=qp.pen();
        p.setWidth(3);
        p.setColor(Qt::black);
        qp.setPen(p);

        QPoint b=_zoom->toScreenPosition(begin, y);
        QPoint e=_zoom->toScreenPosition(end, y);

        qp.drawLine(b, e);
        qp.drawText(0.5*(b.x()+e.x())-50, b.y()-25, 100, 20, Qt::AlignCenter, QString::number(gridStep*5000)+" mm");
    }
}

inline int ModelViewer::round(double v) const
{
    double s=1.0;
    if (v<0.0)
        s=-1.0;

    v=std::fabs(v);
    v=v+0.5;
    v=int(v);
    
    return s*v;
}

void ModelViewer::drawVolumes(Model *model, QPainter &painter, ImageOptions options, bool include_dxf, bool include_images)
{
    if (!model)
        return;

    StatesManager::ViewType view=StatesManager::instance()->viewType();

    QPalette qpalette=palette();
    QColor defaultColor=qpalette.window().color();

    std::vector<Volume*> volumes=*model;
    if (_currentDrawnVolume)
        volumes.push_back(_currentDrawnVolume);
    for (unsigned int i=0 ; i<volumes.size() ; i++)
    {
        // Get geometry.
        Volume *volume=volumes.at(i);
        
        // Get physics.
        Material *material=volume->material();
        QColor fill=defaultColor;
        if (material)
            fill=QColor(material->red(), material->green(), material->blue());

        QPen pen=painter.pen();
        if (volume->selected() && (view==StatesManager::DisplayModel || view==StatesManager::Display1DModel))
        {
            pen.setStyle(Qt::DashLine);
            pen.setWidth(2);
        }
        else
        {
            pen.setStyle(Qt::SolidLine);
            pen.setWidth(1);
        }

        if (volume==_currentDrawnVolume)
        {
            pen.setColor(Qt::red);
            pen.setWidth(2);
        }
        else
            pen.setColor(Qt::black);

        if ((options & DrawVolumes) && !material)
        {
            pen.setColor(Qt::black);
            fill=Qt::white;
        }

        double dx=0.0;
        double dy=0.0;
        if (volume->selected())
        {
            if (_ctrlPressed)
            {
                dx=_dxTranslation;
                dy=_dyTranslation;
            }
            else
            {
                dx=int(_dxTranslation/_zoom->gridStep())*_zoom->gridStep();
                dy=int(_dyTranslation/_zoom->gridStep())*_zoom->gridStep();
            }

            if (_majPressed)
            {
                if (std::fabs(dx)>std::fabs(dy))
                    dy=0.0;
                else
                    dx=0.0;
            }
        }

        bool apply_rotation=false;
        if (_currentMode==SelectionRotation && volume->selected())
            apply_rotation=true;

        bool selected_path=false;

        double ddx=_dxPointTranslation;
        double ddy=_dyPointTranslation;
        if (!_ctrlPressed)
        {
            ddx=int(_dxPointTranslation/_zoom->gridStep())*_zoom->gridStep();
            ddy=int(_dyPointTranslation/_zoom->gridStep())*_zoom->gridStep();
        }

        if (_majPressed)
        {
            if (std::fabs(ddx)>std::fabs(ddy))
                ddy=0.0;
            else
                ddx=0.0;
        }

        // If current volume is morphed by user test if volume is correct.
        if (volume==_mouseOverVolume && _currentMode==VolumeCornerTranslation)
        {
            Volume v=*volume;
            v.translatePoint(_mouseOverPoint, ddx, ddy);
            v.computeBounds();
            v.computeLines();
            if (v.isClosed() && !v.isSelfIntersected())
            {
                _lastDxPointTranslation=_dxPointTranslation;
                _lastDyPointTranslation=_dyPointTranslation;
            }
        }
        else if (volume==_mouseOverVolume && _currentMode==VolumeBorderTranslation && _mouseOverPath)
        {
            Volume v=*volume;
            v.translatePoint(_mouseOverPath->begin(), ddx, ddy);
            v.translatePoint(_mouseOverPath->end(), ddx, ddy);
            v.computeBounds();
            v.computeLines();
            if (v.isClosed() && !v.isSelfIntersected())
            {
                _lastDxPointTranslation=_dxPointTranslation;
                _lastDyPointTranslation=_dyPointTranslation;
            }
        }

        // Get point.
        Point replace_point;
        ddx=_lastDxPointTranslation;
        ddy=_lastDyPointTranslation;
        if (!_ctrlPressed)
        {
            ddx=int(_lastDxPointTranslation/_zoom->gridStep())*_zoom->gridStep();
            ddy=int(_lastDyPointTranslation/_zoom->gridStep())*_zoom->gridStep();
        }

        if (_majPressed)
        {
            if (std::fabs(ddx)>std::fabs(ddy))
                ddy=0.0;
            else
                ddx=0.0;
        }

        replace_point.setX(_mouseOverPoint.x()+ddx);
        replace_point.setY(_mouseOverPoint.y()+ddy);

        // Path point replacement.
        Point pt1;
        Point pt2;

        if (volume==_mouseOverVolume && _currentMode==VolumeBorderTranslation && _mouseOverPath)
        {
            pt1.setX(_mouseOverPath->begin().x()+ddx);
            pt1.setY(_mouseOverPath->begin().y()+ddy);
            pt2.setX(_mouseOverPath->end().x()+ddx);
            pt2.setY(_mouseOverPath->end().y()+ddy);
        }

        QPainterPath path;
        QList<QPoint> corners;
        for (unsigned int j=0 ; j<volume->size() ; j++)
        {
            Path *p=volume->at(j);
            if (!p)
                continue;

            Path::Type type=p->type();
            if (p==_mouseOverPath)
                selected_path=true;

            // Line.
            if (type==Path::LinePath)
            {
                Line *line=(Line *) p;
                Point begin=line->begin();
                Point end=line->end();

                if (apply_rotation)
                {
                    begin=_rotation.apply(begin);
                    end=_rotation.apply(end);
                }

                if (volume==_mouseOverVolume)
                {
                    if (_currentMode==VolumeCornerTranslation)
                    {
                        if (GeometryTools::equivalentPoints(begin, _mouseOverPoint))
                            begin=replace_point;
                        if (GeometryTools::equivalentPoints(end, _mouseOverPoint))
                            end=replace_point;
                    }
                    else if (_currentMode==VolumeBorderTranslation && _mouseOverPath)
                    {
                        if (GeometryTools::equivalentPoints(begin, _mouseOverPath->begin()))
                            begin=pt1;
                        if (GeometryTools::equivalentPoints(begin, _mouseOverPath->end()))
                            begin=pt2;
                        if (GeometryTools::equivalentPoints(end, _mouseOverPath->begin()))
                            end=pt1;
                        if (GeometryTools::equivalentPoints(end, _mouseOverPath->end()))
                            end=pt2;
                    }
                }

                if (volume->selected())
                    corners.append(_zoom->toScreenPosition(begin.x(), begin.y()));

                if (j==0)
                    path.moveTo(_zoom->toScreenPosition(begin.x()+dx, begin.y()+dy));
                path.lineTo(_zoom->toScreenPosition(end.x()+dx, end.y()+dy));
            }

            // Arc.
            else if(type==Path::ArcPath)
            {
                Arc *arc=(Arc *) p;

                double delta=_zoom->toPhysicalLength(0.5);
                double r=std::min(arc->horizontalRadius(), arc->verticalRadius());
                double ratio1=arc->horizontalRadius()/arc->verticalRadius();
                double ratio2=arc->verticalRadius()/arc->horizontalRadius();
                double ratio=std::min(ratio1, ratio2);
                double theta_max=delta/r*180.0/3.141592654*ratio;

                if (arc->horizontalRadius()<=0.0)
                    continue;
                if (arc->verticalRadius()<=0.0)
                    continue;

                for (unsigned int r=0 ; r<arc->size() ; r++)
                {
                    Line *line=arc->at(r);
                    Point begin=line->begin();
                    Point end=line->end();

                    if (apply_rotation)
                    {
                        begin=_rotation.apply(begin);
                        end=_rotation.apply(end);
                    }

                    if (volume==_mouseOverVolume)
                    {
                        if (_currentMode==VolumeCornerTranslation)
                        {
                            if (GeometryTools::equivalentPoints(begin, _mouseOverPoint))
                                begin=replace_point;
                            if (GeometryTools::equivalentPoints(end, _mouseOverPoint))
                                end=replace_point;
                        }
                        else if (_currentMode==VolumeBorderTranslation && _mouseOverPath)
                        {
                            if (GeometryTools::equivalentPoints(begin, _mouseOverPath->begin()))
                                begin=pt1;
                            if (GeometryTools::equivalentPoints(begin, _mouseOverPath->end()))
                                begin=pt2;
                            if (GeometryTools::equivalentPoints(end, _mouseOverPath->begin()))
                                end=pt1;
                            if (GeometryTools::equivalentPoints(end, _mouseOverPath->end()))
                                end=pt2;
                        }
                    }

                    if (r==0)
                        path.moveTo(_zoom->toScreenPosition(begin.x()+dx, begin.y()+dy));
                    path.lineTo(_zoom->toScreenPosition(end.x()+dx, end.y()+dy));
                }
            }
        }

        painter.setPen(pen);
        painter.setBrush(fill);

        // When drawing mesh don't display transparent volumes.
        if (view!=StatesManager::DisplayMesh || material)
            painter.drawPath(path);

        // Draw corners.
        painter.setPen(Qt::black);
        painter.setBrush(Qt::black);
        foreach(const QPoint &p, corners)
        {
            painter.drawEllipse(p, 3, 3);
        }

        // If volume has no material, draw grid again.
        if (!volume->material())
        {

            // Avoid transparent volumes to be clipped on borders.
            QPainterPath p1=path.translated(0.5, 0.0);
            QPainterPath p2=path.translated(-0.5, 0.0);
            QPainterPath p3=path.translated(0.0, 0.5);
            QPainterPath p4=path.translated(0.0, -0.5);
            QPainterPath p5=p1.intersected(p2);
            QPainterPath p6=p3.intersected(p4);

            painter.setClipPath(p5.intersected(p6));
            if (view==StatesManager::DisplayMesh)
            {
                painter.setBrush(defaultColor);
                painter.drawRect(rect());
            }
            drawGridAndOrigin(painter, options);
            
            if (include_images)
                drawImages(model, painter);

            if (include_dxf)
                drawDxf(model, painter);

            painter.setClipRect(rect());
        }

        // Selected point.
        if (selected_path && _mouseOverElementType==Corner && _currentMode!=VolumeCornerTranslation)
        {
            painter.setBrush(Qt::black);
            painter.drawEllipse(_zoom->toScreenPosition(_mouseOverPoint.x(), _mouseOverPoint.y()), 3, 3);
        }

        // Selected path.
        else if (selected_path && _mouseOverElementType==Border && _currentMode!=VolumeBorderTranslation)
        {
            Path::Type type=_mouseOverPath->type();

            QPen p=painter.pen();
            p.setWidth(3);
            p.setStyle(Qt::SolidLine);
            painter.setPen(p);

            // Line.
            if (type==Path::LinePath)
            {
                Line *line=(Line *) _mouseOverPath;
                Point begin=line->begin();
                Point end=line->end();
                    
                painter.drawLine(_zoom->toScreenPosition(begin.x(), begin.y()), _zoom->toScreenPosition(end.x(), end.y()));
            }

            // Arc.
            else if(type==Path::ArcPath)
            {
                Arc *arc=(Arc *) _mouseOverPath;

                double delta=_zoom->toPhysicalLength(0.5);
                double r=std::min(arc->horizontalRadius(), arc->verticalRadius());
                double ratio1=arc->horizontalRadius()/arc->verticalRadius();
                double ratio2=arc->verticalRadius()/arc->horizontalRadius();
                double ratio=std::min(ratio1, ratio2);
                double theta_max=delta/r*180.0/3.141592654*ratio;

                if (arc->horizontalRadius()<=0.0)
                    continue;
                if (arc->verticalRadius()<=0.0)
                    continue;

                for (unsigned int r=0 ; r<arc->size() ; r++)
                {
                    Line *line=arc->at(r);
                    Point begin=line->begin();
                    Point end=line->end();

                    painter.drawLine(_zoom->toScreenPosition(begin.x(), begin.y()), _zoom->toScreenPosition(end.x(), end.y()));
                }
            }
        }
    }
}

void ModelViewer::drawCellTemperatures(Model *model, QPainter &painter)
{
    if (!model)
        return;

    Mesh *mesh=model->mesh();

    if (mesh)
    {
        QPen p=painter.pen();
        p.setWidth(1);
        painter.setPen(p);

        for (unsigned int i=0 ; i<mesh->size() ; i++)
        {
            Cell *cell=mesh->at(i);
            double T=cell->temperature();

            QColor c=getColorByTemperature(0.0, 20.0, T);
            painter.setBrush(c);
            painter.setPen(c);

            QPainterPath path;
            for (unsigned int j=0 ; j<cell->size() ; j++)
            {
                Line *line=cell->at(j);
                Point begin=line->begin();
                Point end=line->end();

                if (j==0)
                    path.moveTo(_zoom->toScreenPosition(begin.x(), begin.y()));
                path.lineTo(_zoom->toScreenPosition(end.x(), end.y()));
            }
            painter.drawPath(path);
        }
    }
}

void ModelViewer::drawCellRh(Model *model, QPainter &painter)
{
    if (!model)
        return;

    Mesh *mesh = model->glaserMesh();

    if (mesh)
    {
        QPen p = painter.pen();
        p.setWidth(1);
        painter.setPen(p);

        for (unsigned int i=0 ; i<mesh->size() ; i++)
        {
            Cell *cell = mesh->at(i);
            double rh = cell->rh();

            QColor c = getColorByRh(rh);

            painter.setBrush(c);
            painter.setPen(c);

            QPainterPath path;
            for (unsigned int j=0 ; j<cell->size() ; j++)
            {
                Line *line=cell->at(j);
                Point begin=line->begin();
                Point end=line->end();

                if (j==0)
                    path.moveTo(_zoom->toScreenPosition(begin.x(), begin.y()));
                path.lineTo(_zoom->toScreenPosition(end.x(), end.y()));
            }
            painter.drawPath(path);
        }
    }
}

void ModelViewer::drawCellCondensation(Model *model, QPainter &painter)
{
    if (!model)
        return;

    Mesh *mesh = model->glaserMesh();

    if (mesh)
    {
        QPen p = painter.pen();
        p.setWidth(1);
        painter.setPen(p);

        for (unsigned int i=0 ; i<mesh->size() ; i++)
        {
            Cell *cell = mesh->at(i);
            double rh = cell->rh();

            QColor c = Qt::lightGray;
            if (rh >= 1.0)
                c = Qt::blue;

            painter.setBrush(c);
            painter.setPen(c);

            QPainterPath path;
            for (unsigned int j=0 ; j<cell->size() ; j++)
            {
                Line *line=cell->at(j);
                Point begin=line->begin();
                Point end=line->end();

                if (j==0)
                    path.moveTo(_zoom->toScreenPosition(begin.x(), begin.y()));
                path.lineTo(_zoom->toScreenPosition(end.x(), end.y()));
            }
            painter.drawPath(path);
        }
    }
}

void ModelViewer::drawSurfaces(Model *model, QPainter &painter, ImageOptions options, Surface *surface_under)
{
    if (!model)
        return;

    StatesManager::InterfaceMode mode=StatesManager::instance()->interfaceMode();
    StatesManager::ViewType view=StatesManager::instance()->viewType();

    QPen p=painter.pen();
    p.setStyle(Qt::SolidLine);
    p.setWidth(3);

    if (surface_under && mode==StatesManager::SetPhysics)
    {
        BoundaryCondition *bc=surface_under->boundaryCondition();
        if (bc)
        {
            QString text="<p style='white-space:pre'>";
            text+="<b>"+QString(bc->name().c_str())+"</b>";
            if (!bc->description().empty())
            {
				text+="<br/>";
                text+=bc->description().c_str();
            }
            double R=bc->resistance();
            double T=bc->temperature();

            if (R>0.0)
            {
                QLocale locale;

                QString condensation;
                if (model && model->computationType() == Model::GlaserCondensation)
                    condensation = " - RH=" + locale.toString(bc->rh() * 100.0, 'f', 2)+" %";

                QString data="R="+locale.toString(R, 'f', 2)+" (m².°C)/W - T="
                                +locale.toString(T, 'f', 2)+" °C" + condensation;
				text+="<br/>";
                text+=data;
            }
            text+="</p>";
            setToolTip(text);
        }
        else
        {
            setToolTip("<b>"+_tr("Adiabatic")+"</b>");
        }
    }
    else
    {
        setToolTip("");
        QToolTip::hideText();
    }

    for (unsigned int i=0 ; i<model->std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=model->std::vector<Surface*>::at(i);

        // Get physics.
        QColor color=Qt::black;

        // SetPhysics mode.
        BoundaryCondition *bc=surface->boundaryCondition();

        // Empty bc id is identical to no bc.
        if (bc && bc->id().empty())
            bc = 0;

        if (mode==StatesManager::SetPhysics && bc && options==NoOptions)
            color=QColor(bc->red(), bc->green(), bc->blue());
        else if ((options & DrawBoundaryConditions) && bc)
            color=QColor(bc->red(), bc->green(), bc->blue());

        if ((options & DrawBoundaryConditions) && !bc)
            continue;

        // SetEnvironments mode.
        if (mode==StatesManager::SetEnvironments && !bc && options==NoOptions)
            continue;
        if (!bc && (options & DrawEnvironments))
            continue;

        // Display results.
        if ((view==StatesManager::DisplayResults || view==StatesManager::Display1DResults || view==StatesManager::DisplayCondensationResults || view==StatesManager::DisplayHumidityResults) && !bc && options==NoOptions)
            continue;
        if ((view==StatesManager::DisplayResults || view==StatesManager::Display1DResults || view==StatesManager::DisplayCondensationResults || view==StatesManager::DisplayHumidityResults) && bc && bc->resistance()<=0.0 && options==NoOptions)
            continue;

        Environment *environment=surface->environment();
        if (mode==StatesManager::SetEnvironments && bc && environment && options==NoOptions)
            color=QColor(environment->red(), environment->green(), environment->blue());

        if ((view==StatesManager::DisplayResults || view==StatesManager::Display1DResults || view==StatesManager::DisplayCondensationResults || view==StatesManager::DisplayHumidityResults) && environment && options==NoOptions)
            color=QColor(environment->red(), environment->green(), environment->blue());

        if ((options & DrawEnvironments) && bc && environment)
            color=QColor(environment->red(), environment->green(), environment->blue());

        // Surface under cursor.
        if (surface==surface_under)
            color=Qt::red;

        // Selected surface in results mode.
        if ((view==StatesManager::DisplayResults || view==StatesManager::Display1DResults || view==StatesManager::DisplayCondensationResults || view==StatesManager::DisplayHumidityResults) && surface->selected() && options==NoOptions)
            color=Qt::red;

        p.setColor(color);
        painter.setPen(p);

        // Get geometry.
        for (unsigned int j=0 ; j<surface->size() ; j++)
        {
            Line *path=surface->at(j);
            Point begin=path->begin();
            Point end=path->end();

            painter.drawLine(_zoom->toScreenPosition(begin.x(), begin.y()), _zoom->toScreenPosition(end.x(), end.y()));
        }
    }
}

void ModelViewer::drawModel(Model *model, QPainter &painter, ImageOptions options, bool include_dxf, bool include_images)
{
    if (!model)
        return;

    StatesManager::InterfaceMode mode=StatesManager::instance()->interfaceMode();
    StatesManager::ViewType view=StatesManager::instance()->viewType();

    painter.setPen(Qt::black);

    QPalette qpalette=palette();
    QColor defaultColor=qpalette.window().color();

    // Draw volumes.
    if (view!=StatesManager::DisplayResults
            && view!=StatesManager::Display1DResults
            && view!=StatesManager::DisplayCondensationResults
            && view!=StatesManager::DisplayHumidityResults
            && options==NoOptions)
        drawVolumes(model, painter, options, include_dxf, include_images);
    else if (options & DrawVolumes)
        drawVolumes(model, painter, options, include_dxf, include_images);

    // Draw cell temperatures.
    if ((view==StatesManager::DisplayResults || view==StatesManager::Display1DResults) && options==NoOptions)
        drawCellTemperatures(model, painter);
    else if (options & DrawTemperatures)
        drawCellTemperatures(model, painter);

    // Draw cell humidity.
    if ((view == StatesManager::DisplayHumidityResults) && options==NoOptions)
        drawCellRh(model, painter);
    else if (options & DrawRhResults)
        drawCellRh(model, painter);

    // Draw cell condensation.
    if ((view == StatesManager::DisplayCondensationResults) && options==NoOptions)
        drawCellCondensation(model, painter);
    else if (options & DrawRhResults)
        drawCellCondensation(model, painter);

    // Draw mesh.
    if (view == StatesManager::DisplayMesh && options==NoOptions)
    {
        Mesh *mesh=model->mesh();
        if (mesh)
            drawMesh(mesh, painter);
        return;
    }

    // Surface under cursor.
    Surface *surface_under=0;
    if ((mode==StatesManager::SetPhysics || view==StatesManager::DisplayResults || view==StatesManager::Display1DResults || mode==StatesManager::SetEnvironments || view==StatesManager::DisplayCondensationResults || view==StatesManager::DisplayHumidityResults) && options==NoOptions)
    {
        QPoint pos=mapFromGlobal(QCursor::pos());
        double x=_zoom->toPhysicalX(pos);
        double y=_zoom->toPhysicalY(pos);
        double precision=_zoom->toPhysicalLength(5.0);
        surface_under=model->getSurfaceAtPosition(x, y, precision);
    }

    // Draw rotation center.
    std::vector<Volume*> volumes=StatesManager::instance()->selectedVolumes();
    if (!volumes.empty() && !_hideRotation && (view==StatesManager::DisplayModel || view==StatesManager::Display1DModel) && options==NoOptions)
    {
        QPoint p1=_zoom->toScreenPosition(_rotation.center().x(), _rotation.center().y());
        QPoint p2=p1;
        p2.setX(p2.x()+100);

        if (_currentMode==SelectionRotation)
            p2=mapFromGlobal(QCursor::pos());

        painter.setPen(Qt::black);
        painter.drawLine(p1, p2);

        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);

        painter.drawEllipse(p1, 3, 3);
        painter.drawEllipse(p2, 3, 3);
    }

    // Draw near point.
    if (StatesManager::instance()->interfaceMode()==StatesManager::AddPoint && options==NoOptions)
    {
        if (_displayNearPoint)
        {
            painter.setBrush(Qt::black);
            painter.drawEllipse(_zoom->toScreenPosition(_addNearPoint.x(), _addNearPoint.y()), 3, 3);
        }
        return;
    }

    // Display surface only in creation mode or results.
    if (mode==StatesManager::Selection && view!=StatesManager::DisplayResults && view!=StatesManager::Display1DResults && options==NoOptions && view!=StatesManager::DisplayCondensationResults && view!=StatesManager::DisplayHumidityResults)
        return;
    if (mode==StatesManager::AddLengthCotation && options==NoOptions)
        return;
    if (mode==StatesManager::AddAngleCotation && options==NoOptions)
        return;
    if (mode==StatesManager::AddArrow && options==NoOptions)
        return;
    if (mode==StatesManager::AddText && options==NoOptions)
        return;

    // Draw surfaces.
    if (options==NoOptions)
        drawSurfaces(model, painter, options, surface_under);
    else if ((options & DrawBoundaryConditions) || (options & DrawEnvironments))
        drawSurfaces(model, painter, options, surface_under);

    // Current drawn polyline.
    if (StatesManager::instance()->interfaceMode()==StatesManager::AddPolyline && !_polylinePoints.empty() && options==NoOptions)
    {
        QPen p=painter.pen();
        p.setColor(Qt::red);
        p.setWidth(2);
        p.setStyle(Qt::SolidLine);
        painter.setPen(p);

        // Defined lines.
        for (unsigned int i=0 ; i<_polylinePoints.size()-1 ; i++)
        {
            Point begin=_polylinePoints.at(i);
            Point end=_polylinePoints.at(i+1);
            painter.drawLine(_zoom->toScreenPosition(begin.x(), begin.y()), _zoom->toScreenPosition(end.x(), end.y()));
        }

        // Mouse line.
        Point begin=_polylinePoints.at(_polylinePoints.size()-1);

        Point point=nearPoint(_mousePosition, !_ctrlPressed);
        double x=point.x();
        double y=point.y();

        if (_majPressed)
        {
            double dx=std::fabs(begin.x()-x);
            double dy=std::fabs(begin.y()-y);
            if (dx>dy)
                y=begin.y();
            else
                x=begin.x();
        }

        QPoint end=_zoom->toScreenPosition(x, y);
        painter.drawLine(_zoom->toScreenPosition(begin.x(), begin.y()), end);
    }
}

void ModelViewer::drawImages(Model *model, QPainter &painter)
{
    if (!model)
        return;

    if (!StatesManager::instance()->showImages())
        return;

    for (unsigned int i=0 ; i<model->std::vector<Image*>::size() ; i++)
    {
        Image *image=model->std::vector<Image*>::at(i);
        createQtImageData(image);
        ImageData *im=image->imageData();
        if (!im)
            continue;

        ImageDataQt *imQt=(ImageDataQt*)im;
        
        double x=image->x();
        double y=image->y();
        double w=image->width();
        double h=image->height();

        double dx=0.0;
        double dy=0.0;
        double dw=0.0;
        double dh=0.0;
        if (image->selected())
        {
            if (_resizeType == Translation)
            {
                dx=_dxTranslation;
                dy=_dyTranslation;
            }

            else
            {
                if (_resizeType & Right)
                    dw=_dxTranslation;

                if (_resizeType & Left)
                    dw=-_dxTranslation;

                if (_resizeType & Top)
                    dh=_dyTranslation;

                if (_resizeType & Bottom)
                    dh=-_dyTranslation;

                // Min size.
                double min_distance=_zoom->toPhysicalLength(10.0);
                if (h+dh<min_distance)
                    dh=min_distance-h;
                if (w+dw<min_distance)
                    dw=min_distance-w;

                // Keep ratio.
                if (dw!=0.0 && dh!=0.0)
                    dw=w/h*dh;

                if (_resizeType & Left)
                    dx=-dw;

                if (_resizeType & Bottom)
                    dy=-dh;
            }
        }

        QPoint origin=_zoom->toScreenPosition(x+dx, y+dy);
        double W=_zoom->toScreenLength(w+dw);
        double H=_zoom->toScreenLength(h+dh);

        QRect rect;
        QPoint top_right;
        top_right.setX(origin.x()+W);
        top_right.setY(origin.y()-H);
        rect.setBottomLeft(origin);
        rect.setTopRight(top_right);

        painter.drawImage(rect, *imQt);

        // Draw selection.
        if (image->selected())
        {
            QPen p=painter.pen();
            p.setWidth(2);
            p.setStyle(Qt::DashLine);
            p.setColor(Qt::black);
            painter.setPen(p);
            painter.setBrush(Qt::transparent);

            painter.drawRect(rect);
        }
    }
}

void ModelViewer::drawDxf(Model *model, QPainter &painter)
{
    if (!model)
        return;

    if (!StatesManager::instance()->showDxf())
        return;

    for (unsigned int i=0 ; i<model->std::vector<DxfContent*>::size() ; i++)
    {
        DxfContent *dxf=model->std::vector<DxfContent*>::at(i);
        createQtDxfData(dxf);
        DxfData *d=dxf->dxf();
        if (!d)
            continue;

        DxfDataQt *dxfQt=(DxfDataQt*)d;
        DxfViewer::drawDxf(painter, dxfQt);
        
        // Draw selection.
        if (dxf->selected())
        {
            QPen p=painter.pen();
            p.setWidth(2);
            p.setStyle(Qt::DashLine);
            p.setColor(Qt::black);
            painter.setPen(p);
            painter.setBrush(Qt::transparent);

            QPoint bl=_zoom->toScreenPosition(d->xMin(), d->yMin());
            QPoint tr=_zoom->toScreenPosition(d->xMax(), d->yMax());

            QRect rect;
            rect.setBottomLeft(bl);
            rect.setTopRight(tr);

            painter.drawRect(rect);
        }
    }
}

void ModelViewer::drawMesh(Mesh *mesh, QPainter &painter)
{
    if (!mesh)
        return;

    painter.setPen(Qt::black);
    QPen p=painter.pen();
    p.setWidth(1);
    painter.setPen(p);

    for (unsigned int i=0 ; i<mesh->size() ; i++)
    {
        Cell *cell=mesh->at(i);

        for (unsigned int j=0 ; j<cell->size() ; j++)
        {
            Line *line=cell->at(j);
            Point begin=line->begin();
            Point end=line->end();

            painter.drawLine(_zoom->toScreenPosition(begin.x(), begin.y()), _zoom->toScreenPosition(end.x(), end.y()));

#ifdef MARK_BAD_CELLS
            Cell::CellState state=cell->cellState();
            if (state==Cell::CellOk)
                continue;

            if (state==Cell::CellBadSymetry)
                continue;
                // painter.setBrush(Qt::red);
            else if (state==Cell::CellBadSum)
                painter.setBrush(Qt::blue);
            else if (state==Cell::CellBadArea)
                painter.setBrush(Qt::yellow);
            else if (state==Cell::CellNotClosed)
                painter.setBrush(Qt::green);
            else if (state==Cell::CellHasNoSurface)
                painter.setBrush(Qt::magenta);

            Point m=cell->center();
            painter.drawEllipse(_zoom->toScreenPosition(m.x(), m.y()), 3, 3);
#endif
        }
    }

#ifdef DISPLAY_MATRIX_CONTENT
    painter.setBrush(Qt::darkRed);
    for (unsigned int i=0 ; i<_highlightCells.size() ; i++)
    {
        Cell *c=mesh->at(_highlightCells.at(i));
        Point begin=c->center();

        painter.drawEllipse(_zoom->toScreenPosition(begin.x(), begin.y()), 3, 3);
    }

    p.setWidth(3);
    painter.setPen(p);

    if (_centeredCell)
    {
        for (unsigned int j=0 ; j<_centeredCell->size() ; j++)
        {
            Line *line=_centeredCell->at(j);
            Point begin=line->begin();
            Point end=line->end();

            painter.drawLine(_zoom->toScreenPosition(begin.x(), begin.y()), _zoom->toScreenPosition(end.x(), end.y()));
        }
    }
#endif
}

void ModelViewer::downTranslation()
{
    int H=height()/5;
    double h=_zoom->toPhysicalLength(H);
    _zoom->down(h);
}

void ModelViewer::upTranslation()
{
    int H=height()/5;
    double h=_zoom->toPhysicalLength(H);
    _zoom->up(h);
}

void ModelViewer::leftTranslation()
{
    int W=width()/5;
    double w=_zoom->toPhysicalLength(W);
    _zoom->left(w);
}

void ModelViewer::rightTranslation()
{
    int W=width()/5;
    double w=_zoom->toPhysicalLength(W);
    _zoom->right(w);
}

void ModelViewer::statesChanged()
{
    setVisible(StatesManager::instance()->viewType()!=StatesManager::DisplayReport);
#ifdef DISPLAY_MATRIX_CONTENT
    if (StatesManager::instance()->simulationState()==StatesManager::InProgress)
    {
        _centeredCell=0;
        _highlightCells.clear();
    }
#endif

    bool display_en_13370=false;
    Model *model=StatesManager::instance()->currentModel();
    if (model && model->en13370Plugin() && StatesManager::instance()->viewType()==StatesManager::Display1DModel)
        display_en_13370=true;
    _en13370Button.setVisible(display_en_13370);

    if (!display_en_13370)
    {
        _en13370Interface.loadPlugin(0);
        _en13370Area.hide();
    }

    
    if (StatesManager::instance()->interfaceMode()!=StatesManager::AddLengthCotation)
    {
        _lengthDataPoint.clear();
    }

    if (StatesManager::instance()->interfaceMode()!=StatesManager::AddAngleCotation)
    {
        _angleDataPoint.clear();
    }

    if (StatesManager::instance()->interfaceMode()!=StatesManager::AddArrow)
    {
        _arrowPoint.clear();
    }

    // Remove mouse over elements.
    _mouseOverPath = 0;
    _mouseOverVolume = 0;

    updateRotationCenter();
    repaint();
}

void ModelViewer::en13370Open()
{
    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    En13370Plugin *plugin=model->en13370Plugin();
    if (!plugin)
        return;

    _en13370Interface.reloadMaterials();
    _en13370Interface.loadPlugin(plugin);
    _en13370Area.setVisible(!_en13370Area.isVisible());
}

void ModelViewer::zoomChanged()
{
    repaint();
}

void ModelViewer::applyTheme()
{
    _rotationCursor      = QCursor(QPixmap(":/images/orange-blue/rotate_cursor.png"));
    _resizeOriginalImage = QPixmap(":/images/orange-blue/resize.png");

    _en13370Button.setIcon(QPixmap(":/images/orange-blue/en13370.png"));
}

// RGB color arrays.
double temperatureColorRed[56]={1.0000,0.9843,0.9333,0.9373,0.9294,0.8902,0.8824,0.8627,0.8353,0.8078,0.7961,0.8039,0.8392,0.8549,0.8706,0.9255,0.9529,0.9804,0.9882,0.8863,0.7608,0.5922,0.4667,0.3451,0.2275,0.1333,0.0000,0.0039,0.0000,0.0000,0.0000,0.0118,0.0000,0.0157,0.0078,0.0000,0.0000,0.0118,0.0000,0.0235,0.0000,0.1137,0.2392,0.3725,0.5490,0.6706,0.8039,0.8980,0.9725,0.8000,0.6863,0.5098,0.4235,0.2941,0.1294,0.0118};
double temperatureColorGreen[56]={0.9922,0.9020,0.7686,0.7176,0.6314,0.5059,0.4118,0.3137,0.2275,0.1020,0.0353,0.1216,0.2392,0.3373,0.4706,0.6431,0.7608,0.8706,0.9961,0.9294,0.8667,0.7647,0.6980,0.6275,0.5490,0.5098,0.4235,0.5333,0.5922,0.6784,0.7529,0.8471,0.9176,1.0000,0.8706,0.7529,0.6235,0.4824,0.3098,0.1765,0.0118,0.0118,0.0118,0.0039,0.0000,0.0039,0.0078,0.0000,0.0078,0.0000,0.0118,0.0000,0.0039,0.0039,0.0000,0.0000};
double temperatureColorBlue[56]={0.9804,0.9020,0.7725,0.7216,0.6392,0.5098,0.4157,0.3098,0.2196,0.1059,0.0353,0.0000,0.0039,0.0039,0.0000,0.0000,0.0118,0.0039,0.0078,0.0000,0.0157,0.0000,0.0078,0.0000,0.0000,0.0275,0.0000,0.1725,0.3059,0.4471,0.5608,0.7294,0.8667,1.0000,0.9647,0.9176,0.8824,0.8549,0.7882,0.7765,0.7059,0.7451,0.7882,0.8196,0.8745,0.9020,0.9490,0.9765,0.9843,0.7922,0.6784,0.5059,0.4196,0.2980,0.1255,0.0000};

QColor ModelViewer::getColorByTemperature(const double xmin, const double xmax, double value)
{
    // Check for bounds.
    if (value<xmin)
        value=xmin;
    if (value>xmax)
        value=xmax;

    double v=55.0-(value-xmin)/(xmax-xmin)*55.0;
    int x=int(v);
    int xs=x;

    // Min value.
    double minRed=temperatureColorRed[x];
	double minGreen=temperatureColorGreen[x];
	double minBlue=temperatureColorBlue[x];

    // Max value.
    x++;
	if(x>55)
        x=55;
	double maxRed=temperatureColorRed[x];
	double maxGreen=temperatureColorGreen[x];
	double maxBlue=temperatureColorBlue[x];

    // Mean value.
    double r=minRed+(maxRed-minRed)*(v-xs);
	double g=minGreen+(maxGreen-minGreen)*(v-xs);
	double b=minBlue+(maxBlue-minBlue)*(v-xs);

    return QColor(r*255.0, g*255.0, b*255.0);
}

QColor ModelViewer::getColorByRh(double value)
{
    if (value <= 0.0)
        value = 0.0;

    if (value >= 1.0)
        value = 1.0;

    return QColor((1.0 - value) * 255.0, (1.0 - value) * 255.0, 255.0);
}

void ModelViewer::createQtImageData(Image *image)
{
    if (image->imageData())
        return;

    QByteArray base64=image->data().c_str();
    QByteArray content=QByteArray::fromBase64(base64);

    ImageDataQt *im=new ImageDataQt;
    im->loadFromData(content);
    image->setImageData(im);
}

void ModelViewer::createQtDxfData(DxfContent *dxf)
{
    if (!dxf->dxf())
        dxf->setDxf(new DxfDataQt);

    DxfDataQt *d=(DxfDataQt*)dxf->dxf();
    if (!d->empty())
        return;

    QByteArray base64=dxf->data().c_str();
    QByteArray content=QByteArray::fromBase64(base64);

    d->loadFromStream(content.constData());
    dxf->setDxf(d);
}

double ModelViewer::snapToGridX(const QPoint &pos) const
{
    double x=_zoom->toPhysicalX(pos);
    double s=1.0;
    if (x<0.0)
        s=-1.0;

    double gs=_zoom->gridStep();

    int nx=int((std::fabs(x)+0.5*gs)/gs);
    return nx*s*gs;
}

double ModelViewer::snapToGridY(const QPoint &pos) const
{
    double y=_zoom->toPhysicalY(pos);
    double s=1.0;
    if (y<0.0)
        s=-1.0;

    double gs=_zoom->gridStep();

    int ny=int((std::fabs(y)+0.5*gs)/gs);
    return ny*s*gs;
}

void ModelViewer::selectDxfEntities(double x, double y, double epsilon, bool ctrl_key)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    bool entities_selected=false;

    for (unsigned int i=0 ; i<model->std::vector<DxfContent*>::size() ; i++)
    {
        DxfContent *content=model->std::vector<DxfContent*>::at(i);
        DxfData *dxf=content->dxf();
        if (!dxf)
            continue;

        DxfDataQt *d=(DxfDataQt*) dxf;

        for (unsigned int i=0; i<d->size(); i++)
        {
            DxfReader::Layer *layer=d->at(i);
            if (!layer->isVisible())
                continue;

            for (unsigned int j=0; j<layer->size(); j++)
            {
                DxfReader::Entity *entity=layer->at(j);
                if (entity->containsPoint(x, y, epsilon))
                {
                    entities_selected=true;
                    if (!ctrl_key)
                    {
                        entity->setSelected(true);
                    }
                    else
                        entity->setSelected(!entity->selected());
                }
                else
                {
                    if (!ctrl_key)
                        entity->setSelected(false);
                }
            }
        }

        // Select whole Dxf.
        if (!entities_selected && !ctrl_key)
        {
            if (content->xMin()<=x && content->xMax()>=x && content->yMin()<=y && content->yMax()>=y)
            {
                content->setSelected(!content->selected());
                entities_selected=true;
            }
        }
    }
}

void ModelViewer::importSelectedElements()
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    std::vector<DxfReader::Entity*> entities=selectedDxfEntities();
    if (entities.empty())
        return;

    std::string xml=dxfElementsToXml(entities);
    ImportVolumesFromXml *action=new ImportVolumesFromXml(model, xml, true);
    ActionsManager::instance()->exec(action);

    // If no volume has been imported: inform user & delete action.
    if (action->canceledAction())
    {
        QMessageBox dialog;
        dialog.setWindowTitle(_tr("VolumeImportFromDxf"));
        QString text=_tr("VolumeImportFromDxfErrorText");
        dialog.setText(text);
        dialog.setWindowModality(Qt::ApplicationModal);
        QIcon icon(":/images/icon.png");
        dialog.setWindowIcon(icon);
        dialog.setIconPixmap(QPixmap(":/images/orange-blue/warning.png"));
        dialog.exec();

        ActionsManager::instance()->removeAction(action);
    }
}

std::string ModelViewer::dxfElementsToXml(const std::vector<DxfReader::Entity*> &elements) const
{
    std::string xml="<volumes>";
    xml+="<volume>";

    std::string material=StatesManager::instance()->currentMaterialId();
    if (!material.empty())
        xml+="<material ref=\"" + material + "\" />";

    xml+="<cells>";
    xml+="<cell-size>0.025</cell-size>";
    xml+="</cells>";

    xml+="<paths>";

    for (unsigned int i=0 ; i<elements.size() ; i++)
    {
        DxfReader::Entity *entity=elements.at(i);
        DxfReader::Entity::Type t=entity->getType();
        xml+=entity->toConducteoElement();
    }

    xml+="</paths>";
    xml+="</volume>";
    xml+="</volumes>";
    return xml;
}

void ModelViewer::addPoint()
{
    if (!_displayNearPoint)
        return;

    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    AddPoint *action=new AddPoint(model, _addNearPoint);
    ActionsManager::instance()->exec(action);
}

void ModelViewer::setEnvironmentFromSelection()
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x = std::min(_currentSelectionBegin.x(), _currentSelectionEnd.x());
    double y = std::min(_currentSelectionBegin.y(), _currentSelectionEnd.y());
    double w = std::fabs(_currentSelectionBegin.x()-_currentSelectionEnd.x());
    double h = std::fabs(_currentSelectionBegin.y()-_currentSelectionEnd.y());

    std::string id = StatesManager::instance()->currentEnvironmentId();

    SetEnvironmentFromSelection *action = new SetEnvironmentFromSelection(model, id, x, y, w, h);

    // This action can be invalid if no surface is caugth.
    if (action->isValid())
        ActionsManager::instance()->exec(action);
    else
        delete action;
}

void ModelViewer::setPhysicFromSelection()
{
    QMenu menu(this);

    QPoint pos;
    pos.setX(_zoom->toScreenPosition(_currentSelectionEnd.x(), _currentSelectionEnd.y()).x());
    pos.setY(_zoom->toScreenPosition(_currentSelectionEnd.x(), _currentSelectionEnd.y()).y());

    QPoint globalPos = mapToGlobal(pos);

    QAction *applyMaterial = menu.addAction(_tr("ApplyVolumeMaterial"));
    QAction *applyBoundary = menu.addAction(_tr("ApplyVolumeBC"));

    QAction *choice = menu.exec(globalPos);

    if (choice == applyMaterial)
        setMaterialFromSelection();
    else if (choice == applyBoundary)
        setBoundaryConditionFromSelection();
}

void ModelViewer::setMaterialFromSelection()
{
    Model *model = StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x = std::min(_currentSelectionBegin.x(), _currentSelectionEnd.x());
    double y = std::min(_currentSelectionBegin.y(), _currentSelectionEnd.y());
    double w = std::fabs(_currentSelectionBegin.x()-_currentSelectionEnd.x());
    double h = std::fabs(_currentSelectionBegin.y()-_currentSelectionEnd.y());

    std::string id = StatesManager::instance()->currentMaterialId();

    SetMaterialFromSelection *action = new SetMaterialFromSelection(model, id, x, y, w, h);

    // This action can be invalid if no surface is caugth.
    if (action->isValid())
        ActionsManager::instance()->exec(action);
    else
        delete action;
}

void ModelViewer::setBoundaryConditionFromSelection()
{
    Model *model = StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x = std::min(_currentSelectionBegin.x(), _currentSelectionEnd.x());
    double y = std::min(_currentSelectionBegin.y(), _currentSelectionEnd.y());
    double w = std::fabs(_currentSelectionBegin.x()-_currentSelectionEnd.x());
    double h = std::fabs(_currentSelectionBegin.y()-_currentSelectionEnd.y());

    std::string id = StatesManager::instance()->currentBoundaryConditionId();

    SetBoundaryConditionFromSelection *action = new SetBoundaryConditionFromSelection(model, id, x, y, w, h);

    // This action can be invalid if no surface is caugth.
    if (action->isValid())
        ActionsManager::instance()->exec(action);
    else
        delete action;
}

Point ModelViewer::nearPoint(const QPoint &position, bool snap) const
{
    Point p;

    p.setX(_zoom->toPhysicalX(position));
    p.setY(_zoom->toPhysicalY(position));

    // No snap to grid & points.
    if (!snap)
        return p;

    // Snap to grid or points.
    std::vector<Point> points;
    Model *model=StatesManager::instance()->currentModelView();
    if (model)
        points=model->points();
    
    Point s;
    s.setX(snapToGridX(position));
    s.setY(snapToGridY(position));

    points.push_back(s);

    double distance=0.0;
    unsigned int idx=0;
    for (unsigned int i=0 ; i<points.size() ; i++)
    {
        Point &p2=points.at(i);
        double d=GeometryTools::pointDistance(p, p2);

        if (d<distance || i==0)
        {
            distance=d;
            idx=i;
        }
    }

    return points.at(idx);
}

void ModelViewer::setCustomZoom(Zoom *zoom)
{
    _zoom=zoom;
    _viewZoom=zoom;
}

QImage ModelViewer::toImage(Model *model, int width, int height, ImageOptions options, bool include_images, bool include_dxf, bool include_annotations, Model *model2d)
{
    resize(width, height);
    setModel(model);

    // Create empty image.
    QImage picture(width, height, QImage::Format_ARGB32);

    QPainter painter;
    painter.begin(&picture);

    QFont f = painter.font();
    f.setPixelSize(11);
    painter.setFont(f);

    QBrush white_brush(Qt::white);
    QPen pen(Qt::white);
    painter.setBrush(white_brush);
    painter.setPen(pen);

    painter.drawRect(0, 0, width, height);

    // Draw grid.
    drawGridAndOrigin(painter, options);

    // Draw Dxf.
    if (include_dxf)
        drawDxf(model, painter);

    // Draw images;
    if (include_images)
        drawImages(model, painter);

    // Draw model.
    drawModel(model, painter, options, include_dxf, include_images);

    // Draw annotations.
    if (include_annotations)
        drawAnnotations(model, painter);

    // Draw superficial temperatures.
    if (options & DrawSupTemp)
        drawSuperficialTemperatures(model, painter);

    // Draw condensation.
    if (options & DrawCondensationResults)
        drawCellCondensation(model, painter);

    if (options & DrawRhResults)
        drawCellRh(model, painter);

    // Draw EN 13370 results.
    if (options & DrawEn13370Results)
        drawEn13370Results(model, painter, height, width, model2d);
    
    painter.end();
    return picture;
}

void ModelViewer::drawEn13370Results(Model *model, QPainter &painter, int height, int width, Model *model2d)
{
    if (!model)
        return;

    if (!model2d)
        return;

    En13370Plugin *plugin=model2d->en13370Plugin();
    if (!plugin)
        return;

    double flux=plugin->flux();
    if (isnan(flux) || isinf(flux))
        return;

    if (flux==0.0)
        return;

    QLocale locale;
    painter.setPen(Qt::black);

    QString string=_tr("EN13370GndFlux")
        +locale.toString(flux, 'f', 3)+" W/m";
    painter.drawText(10, height-20, width-10, 20, 0, string);
}

QImage ModelViewer::getMaterialLegend() const 
{
    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return QImage();

    int w=40;
    int h_ref=30;
    int m=10;

    int width=200;

    // Create material list.
    std::vector<std::string> materials;
    for (unsigned int i=0 ; i<model->std::vector<Volume*>::size() ; i++)
    {
        Volume *volume=model->std::vector<Volume*>::at(i);
        Material *material=volume->material();
        if (!material)
            continue;
        materials.push_back(material->id());
    }

    // Remove material defined twice.
    std::sort(materials.begin(), materials.end());
    std::vector<std::string>::iterator it1=std::unique(materials.begin(), materials.end());
    materials.resize(std::distance(materials.begin(),it1));

    QImage picture(width, m+materials.size()*(h_ref+m), QImage::Format_ARGB32);

    QPainter painter;
    painter.begin(&picture);

    QFont f = painter.font();
    f.setPixelSize(11);
    painter.setFont(f);

    QBrush white_brush(Qt::white);
    QPen pen(Qt::white);
    painter.setBrush(white_brush);
    painter.setPen(pen);

    painter.drawRect(0, 0, picture.width(), picture.height());

    QLocale locale;

    bool condensationMode = false;
    if (model && model->computationType() == Model::GlaserCondensation)
        condensationMode = true;

    for (unsigned int i=0 ; i<materials.size() ; i++)
    {
        std::string material_id=materials.at(i);
        Material *material=Materials::instance()->materialFromId(material_id);
        if (!material)
            continue;

        int r=material->red();
        int g=material->green();
        int b=material->blue();
        QColor color(r, g, b);

        QString title=material->name().c_str();
        QString description=material->description().c_str();
        double conductivity=material->conductivity();
        QString data="λ="+locale.toString(conductivity, 'f', 3)+" W/(m.°C)";

        QRect rect(m, m+i*(h_ref+m), w, h_ref);

        QPen pen;
        pen.setColor(color.darker());
        painter.setPen(pen);
        painter.setBrush(QBrush(color));

        painter.drawRect(rect);

        int offset_h=0;
        if (!description.isEmpty() && !condensationMode)
            offset_h=6;

        int dh=h_ref/3+offset_h/3;
        if (description.isEmpty() && !condensationMode)
            dh=h_ref/2;

        // Name.
        QRect title_rect(2*m+w, m+i*(h_ref+m)-offset_h/2, picture.width()-(2*m+w), dh);
        painter.setPen(Qt::black);
        QTextOption o;
        o.setAlignment(Qt::AlignVCenter);

        // Elide text if needed.
        QFontMetrics metric(painter.font());
        if (material_id=="106D6720-211C-4090-8C60-7CED671C8580" || material_id=="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
            title_rect.setHeight(title_rect.height()*2);
        title=metric.elidedText(title, Qt::ElideRight, title_rect.width());

        painter.drawText(title_rect, title, o);

        // Description (disabled if condensation mode is set on).
        if (!description.isEmpty() && !condensationMode)
        {
            QRect desc_rect(2*m+w, m+i*(h_ref+m)+dh-offset_h/2, picture.width()-(2*m+w), dh);
            description=metric.elidedText(description, Qt::ElideRight, desc_rect.width());
            painter.drawText(desc_rect, description, o);
        }

        // Physical data.
        int pos=m+i*(h_ref+m)+2*dh-offset_h/2;
        if (description.isEmpty())
            pos=m+i*(h_ref+m)+dh;
        if (condensationMode)
            pos = m + i * (h_ref + m) + dh - offset_h / 2;

        QRect data_rect(2*m+w, pos, picture.width()-(2*m+w), dh);
        if (material_id!="106D6720-211C-4090-8C60-7CED671C8580" && material_id!="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
            painter.drawText(data_rect, data, o);

        // Condensation data.
        if (condensationMode)
        {
            double mu = material->mu();
            data = "μ=" + locale.toString(mu, 'f', 0);

            pos = m + i * (h_ref + m) + 2 * dh - offset_h / 2;

            QRect mu_rect(2 * m + w, pos, picture.width()-(2 * m + w), dh + 5);
            painter.drawText(mu_rect, data, o);
        }
    }

    painter.end();
    return picture;
}

QImage ModelViewer::getBoundaryConditionLegend() const
{
    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return QImage();

    int w=40;
    int h=30;
    int m=10;
    int width=250;

    // Create boundary conditions list.
    std::vector<std::string> boundaries;
    for (unsigned int i=0 ; i<model->std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=model->std::vector<Surface*>::at(i);
        BoundaryCondition *boundary=surface->boundaryCondition();
        if (!boundary)
            continue;

        if (boundary->resistance()<=0.0)
            continue;

        boundaries.push_back(boundary->id());
    }

    // Remove boundary defined twice.
    std::sort(boundaries.begin(), boundaries.end());
    std::vector<std::string>::iterator it1=std::unique(boundaries.begin(), boundaries.end());
    boundaries.resize(std::distance(boundaries.begin(),it1));

    QImage picture(width, m+boundaries.size()*(h+m), QImage::Format_ARGB32);

    QPainter painter;
    painter.begin(&picture);

    QFont f = painter.font();
    f.setPixelSize(11);
    painter.setFont(f);

    QBrush white_brush(Qt::white);
    QPen pen(Qt::white);
    painter.setBrush(white_brush);
    painter.setPen(pen);

    painter.drawRect(0, 0, picture.width(), picture.height());

    QLocale locale;

    for (unsigned int i=0 ; i<boundaries.size() ; i++)
    {
        std::string boundary_id=boundaries.at(i);
        BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(boundary_id);
        if (!bc)
            continue;
        
        double R=bc->resistance();
        double T=bc->temperature();

        int r=bc->red();
        int g=bc->green();
        int b=bc->blue();

        QColor color(r, g, b);
        QString name=bc->name().c_str();

        if (!LinguistManager::instance()->translate(name).isEmpty())
            name=LinguistManager::instance()->translate(name);

        QString data="R="+locale.toString(R, 'f', 2)+" (m².°C)/W - T="
                      +locale.toString(T, 'f', 2)+" °C";

        QRect rect(m, m+i*(h+m), w, h);

        QPen pen;
        pen.setColor(color.darker());
        painter.setPen(pen);
        painter.setBrush(QBrush(color));

        painter.drawRect(rect);

        QRect title_rect(2*m+w, m+i*(h+m), picture.width()-(2*m+w), h/2);
        painter.setPen(Qt::black);
        QTextOption o;
        o.setAlignment(Qt::AlignVCenter);

        // Elide text if needed.
        QFontMetrics metric(painter.font());
        name=metric.elidedText(name, Qt::ElideRight, title_rect.width());

        painter.drawText(title_rect, name, o);

        QRect data_rect(2*m+w, m+i*(h+m)+h/2, picture.width()-(2*m+w), h/2);
        painter.drawText(data_rect, data, o);
    }

    painter.end();
    return picture;
}

QImage ModelViewer::getEnvironmentsLegend() const
{
    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return QImage();

    int w=40;
    int h=30;
    int m=10;

    int width=250;

    // Create environments list.
    std::vector<std::string> environments;
    for (unsigned int i=0 ; i<model->std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=model->std::vector<Surface*>::at(i);
        Environment *environment=surface->environment();
        if (!environment)
            continue;
        environments.push_back(environment->name());
    }

    // Remove boundary defined twice.
    std::sort(environments.begin(), environments.end());
    std::vector<std::string>::iterator it1=std::unique(environments.begin(), environments.end());
    environments.resize(std::distance(environments.begin(),it1));

    // HACK: sort by name.
    std::sort(environments.begin(), environments.end());

    QImage picture(width, m+environments.size()*(h+m), QImage::Format_ARGB32);

    QPainter painter;
    painter.begin(&picture);

    QFont f = painter.font();
    f.setPixelSize(11);
    painter.setFont(f);

    QBrush white_brush(Qt::white);
    QPen pen(Qt::white);
    painter.setBrush(white_brush);
    painter.setPen(pen);

    painter.drawRect(0, 0, picture.width(), picture.height());

    for (unsigned int i=0 ; i<environments.size() ; i++)
    {
        std::string env_id=environments.at(i);
        Environment *environment=Environments::instance()->environmentFromName(env_id);
        if (!environment)
            continue;
        
        QString name=environment->description().c_str();

        if (!LinguistManager::instance()->translate(name).isEmpty())
            name=LinguistManager::instance()->translate(name);

        int r=environment->red();
        int g=environment->green();
        int b=environment->blue();

        QColor color(r, g, b);
        QRect rect(m, m+i*(h+m), w, h);

        QPen pen;
        pen.setColor(color.darker());
        painter.setPen(pen);
        painter.setBrush(QBrush(color));

        painter.drawRect(rect);

        QRect title_rect(2*m+w, m+i*(h+m), picture.width()-(2*m+w), h);
        painter.setPen(Qt::black);
        QTextOption o;
        o.setAlignment(Qt::AlignVCenter);

        // Elide text if needed.
        QFontMetrics metric(painter.font());
        name=metric.elidedText(name, Qt::ElideRight, title_rect.width());

        painter.drawText(title_rect, name, o);
    }

    painter.end();
    return picture;
}

BoundaryConditionData::BoundaryConditionData():
    _useFactor(false)
{
}

BoundaryConditionData::~BoundaryConditionData()
{
}

std::vector<BoundaryConditionData> ModelViewer::getBCSuperficialTemperatures() const
{
    std::vector<BoundaryConditionData> data;

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return data;

    // Count number of boundary temperatures.
    std::vector<std::string> bcs;
    std::vector<double> temperatures;

    for (unsigned int i=0 ; i<model->std::vector<Surface*>::size() ; i++)
    {
        Surface *surface=model->std::vector<Surface*>::at(i);
        BoundaryCondition *bc=surface->boundaryCondition();
        if (!bc)
            continue;

        if (bc->resistance()<=0.0)
            continue;

        bcs.push_back(bc->id());
        temperatures.push_back(bc->temperature());
    }

    // Make unique list of boundary temperatures.
    std::sort(bcs.begin(), bcs.end());
    std::vector<std::string>::iterator it=std::unique(bcs.begin(), bcs.end());
    bcs.resize(std::distance(bcs.begin(),it));

    std::sort(temperatures.begin(), temperatures.end());
    std::vector<double>::iterator it2=std::unique(temperatures.begin(), temperatures.end());
    temperatures.resize(std::distance(temperatures.begin(),it2));

    // Create physic data list.
    for (unsigned int j=0 ; j<bcs.size() ; j++)
    {
        std::string id=bcs.at(j);
        BoundaryCondition *bc=BoundaryConditions::instance()->bcFromId(id);
        if (!bc)
            continue;

        BoundaryConditionData d;
        d._id=id;
        d._name=bc->name().c_str();
        d._resistance=bc->resistance();
        d._temperature=bc->temperature();
        d._useFactor=(temperatures.size()==2);
        d._color=QColor(bc->red(), bc->green(), bc->blue());

        if (!LinguistManager::instance()->translate(d._name).isEmpty())
            d._name=LinguistManager::instance()->translate(d._name);

        bool min_defined=false;
        bool max_defined=false;
        for (unsigned int i=0 ; i<model->std::vector<Surface*>::size() ; i++)
        {
            Surface *surface=model->std::vector<Surface*>::at(i);
            BoundaryCondition *boundary=surface->boundaryCondition();
            if (!boundary)
                continue;

            if (boundary!=bc)
                continue;

            double tmin=surface->minimalTemperature();
            double tmax=surface->maximalTemperature();

            if (!min_defined || tmin<d._tmin)
            {
                d._tmin=tmin;
                d._xmin=surface->minimalTemperatureLocation().x();
                d._ymin=surface->minimalTemperatureLocation().y();
                min_defined=true;
            }

            if (!max_defined || tmax>d._tmax)
            {
                d._tmax=tmax;
                d._xmax=surface->maximalTemperatureLocation().x();
                d._ymax=surface->maximalTemperatureLocation().y();
                max_defined=true;
            }
        }
        data.push_back(d);
    }

    // Set factors.
    double delta_temperature=0.0;
    double min_temperature=0.0;
    if (temperatures.size()==2)
    {
        delta_temperature=std::fabs(temperatures.at(1)-temperatures.at(0));
        min_temperature=std::min(temperatures.at(0), temperatures.at(1));

        for (unsigned int j=0 ; j<data.size() ; j++)
        {
            BoundaryConditionData &d=data.at(j);
            d._minFactor=(d._tmin-min_temperature)/delta_temperature;
            d._maxFactor=(d._tmax-min_temperature)/delta_temperature;
        }
    }

    return data;
}

void ModelViewer::drawSuperficialTemperatures(Model *model, QPainter &painter)
{
    if (!model)
        return;

    std::vector<BoundaryConditionData> bc=getBCSuperficialTemperatures();

    for (unsigned int i=0 ; i<bc.size() ; i++)
    {
        BoundaryConditionData &b=bc.at(i);

        int diameter=10;

        int X0=_zoom->toScreenPosition(b._xmin, b._ymin).x()-diameter/2;
        int Y0=_zoom->toScreenPosition(b._xmin, b._ymin).y()-diameter/2;

        QPen blue(Qt::blue);
        painter.setPen(blue);
        painter.setBrush(Qt::blue);
        painter.drawEllipse(X0, Y0, diameter, diameter);

        int X1=_zoom->toScreenPosition(b._xmax, b._ymax).x()-diameter/2;
        int Y1=_zoom->toScreenPosition(b._xmax, b._ymax).y()-diameter/2;

        QPen red(Qt::red);
        painter.setPen(red);
        painter.setBrush(Qt::red);
        painter.drawEllipse(X1, Y1, diameter, diameter);
    }
}

void ModelViewer::deletePointRequested(Volume *volume, Point &point)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    DeletePoint *action=new DeletePoint(model, volume, point);
    ActionsManager::instance()->exec(action);

    if (action->canceledAction())
        ActionsManager::instance()->removeAction(action);
}

void ModelViewer::selectMaterial(QMouseEvent *event)
{
    StatesManager::ViewType view = StatesManager::instance()->viewType();
    if (view != StatesManager::DisplayModel && view != StatesManager::Display1DModel)
        return;

    Model *model = StatesManager::instance()->currentModelView();
    if (!model)
        return;

    double x = _zoom->toPhysicalX(event->pos());
    double y = _zoom->toPhysicalY(event->pos());

    Volume *volume = model->getVolumeAtPosition(x, y);
    if (!volume)
        return;

    Material *material = volume->material();
    if (!material)
        return;

    emit selectMaterialByIdRequested(material->id().c_str());

    // After selecting a material with mouse we come back
    // into SetPhysics mode.
    StatesManager::instance()->setInterfaceMode(StatesManager::SetPhysics);
}

void ModelViewer::addText(const QPoint &point)
{
    Model *model=StatesManager::instance()->currentModelView();
    if (!model)
        return;

    QString text=QInputDialog::getText(this, _tr("TextAnnotation"), _tr("TextTitle"));
    if (text.isEmpty())
        return;

    Annotation annotation;
    annotation.setType(Annotation::AnnotationText);
    annotation.setText(text.toStdString());
    
    Point position;
    position.setX(_zoom->toPhysicalX(point));
    position.setY(_zoom->toPhysicalY(point));
    annotation.setPosition(position);

    AddAnnotation *action=new AddAnnotation(model, annotation);
    ActionsManager::instance()->exec(action);

    repaint();
}

void ModelViewer::drawAnnotation(QPainter &painter, Annotation *annotation)
{
    QPen p(Qt::red);
    if (annotation->selection() != Annotation::NoSelection)
        p = QPen(Qt::gray);
    p.setWidth(2);
    painter.setPen(p);

    // Annotation translation.
    double dx = 0.0;
    double dy = 0.0;
    if (_currentMode == SelectionTranslation && annotation->selection() != Annotation::NoSelection)
    {
        dx = _dxTranslation;
        dy = _dyTranslation;

        if (!_ctrlPressed)
        {
            dx = int(_dxTranslation / _zoom->gridStep()) * _zoom->gridStep();
            dy = int(_dyTranslation / _zoom->gridStep()) * _zoom->gridStep();
        }

        if (_majPressed)
        {
            if (std::fabs(dx) > std::fabs(dy))
                dy=0.0;
            else
                dx=0.0;
        }
    }

    // Draw text.
    if (annotation->type()==Annotation::AnnotationText)
    {
        Point p=annotation->position();

        if (_annotationActionType != AnnotationTranslation)
        {
            dx = 0.0;
            dy = 0.0;
        }

        QPoint pos=_zoom->toScreenPosition(p.x() + dx, p.y() + dy);

        painter.drawText(pos.x(), pos.y(), annotation->text().c_str());

        // Text selection.
        if (annotation->selection() == Annotation::Whole)
        {
            // Around rectangle.
            QPen p(Qt::DashLine);
            p.setColor(Qt::black);

            QFontMetrics metrics(painter.font());

            int w = metrics.horizontalAdvance(annotation->text().c_str());
            int h = metrics.ascent();

            int margin = 2;

            QRect r(pos.x() - margin, pos.y() - h - margin, w + 2 * margin, metrics.height() + 2 * margin);

            painter.setPen(p);
            painter.setBrush(Qt::transparent);
            painter.drawRect(r);
        }
    }

    // Draw arrows.
    if (annotation->type()==Annotation::AnnotationArrow && annotation->validArrow())
    {
        Point begin=annotation->begin();
        Point end=annotation->end();

        double bdx = 0.0;
        double edx = 0.0;
        double bdy = 0.0;
        double edy = 0.0;

        if (_annotationActionType == AnnotationTranslation)
        {
            bdx = dx;
            bdy = dy;
            edx = dx;
            edy = dy;
        }

        else if (_annotationActionType == AnnotationBeginTranslation)
        {
            bdx = dx;
            bdy = dy;
        }

        else if (_annotationActionType == AnnotationEndTranslation)
        {
            edx = dx;
            edy = dy;
        }

        // Copy annotation
        Annotation copied = *annotation;

        Point begin2;
        begin2.setX(begin.x() + bdx);
        begin2.setY(begin.y() + bdy);
        copied.setBegin(begin2);

        Point end2;
        end2.setX(end.x() + edx);
        end2.setY(end.y() + edy);
        copied.setEnd(end2);

        QPoint b=_zoom->toScreenPosition(copied.begin().x(), copied.begin().y());
        QPoint e=_zoom->toScreenPosition(copied.end().x(), copied.end().y());

        if (e == b)
            return;

        painter.drawLine(b, e);

        Point p1=copied.secondArrowFirstPoint();
        Point p2=copied.secondArrowSecondPoint();

        QPoint qp1=_zoom->toScreenPosition(p1.x(), p1.y());
        QPoint qp2=_zoom->toScreenPosition(p2.x(), p2.y());

        painter.drawLine(e, qp1);
        painter.drawLine(e, qp2);

        // Arrow selection.
        if (annotation->selection() == Annotation::Whole)
        {
            painter.setPen(Qt::black);
            painter.setBrush(Qt::black);

            painter.drawEllipse(b, 3, 3);
            painter.drawEllipse(e, 3, 3);
        }
    }

    // Cotation.
    else if (annotation->type()==Annotation::AnnotationCotation && annotation->validArrow())
    {
        Point begin=annotation->begin();
        Point end=annotation->end();

        double bdx = 0.0;
        double edx = 0.0;
        double bdy = 0.0;
        double edy = 0.0;

        if (_annotationActionType == AnnotationTranslation && annotation->selection() & Annotation::Lines)
        {
            bdx = dx;
            bdy = dy;
            edx = dx;
            edy = dy;
        }

        else if (_annotationActionType == AnnotationBeginTranslation)
        {
            bdx = dx;
            bdy = dy;
        }

        else if (_annotationActionType == AnnotationEndTranslation)
        {
            edx = dx;
            edy = dy;
        }

        // Copy annotation
        Annotation copied = *annotation;

        Point begin2;
        begin2.setX(begin.x() + bdx);
        begin2.setY(begin.y() + bdy);
        copied.setBegin(begin2);

        Point end2;
        end2.setX(end.x() + edx);
        end2.setY(end.y() + edy);
        copied.setEnd(end2);

        QPoint b=_zoom->toScreenPosition(copied.begin().x(), copied.begin().y());
        QPoint e=_zoom->toScreenPosition(copied.end().x(), copied.end().y());

        if (e == b)
            return;

        QPen pen = painter.pen();
        if (annotation->selection() & Annotation::Lines)
            pen.setColor(Qt::gray);
        else
            pen.setColor(Qt::red);
        painter.setPen(pen);

        painter.drawLine(b, e);

        Point p1=copied.secondArrowFirstPoint();
        Point p2=copied.secondArrowSecondPoint();

        QPoint qp1=_zoom->toScreenPosition(p1.x(), p1.y());
        QPoint qp2=_zoom->toScreenPosition(p2.x(), p2.y());

        painter.drawLine(e, qp1);
        painter.drawLine(e, qp2);

        p1=copied.firstArrowFirstPoint();
        p2=copied.firstArrowSecondPoint();

        qp1=_zoom->toScreenPosition(p1.x(), p1.y());
        qp2=_zoom->toScreenPosition(p2.x(), p2.y());

        painter.drawLine(b, qp1);
        painter.drawLine(b, qp2);

        double dtx = 0.0;
        double dty = 0.0;

        if (_annotationActionType == AnnotationTranslation && annotation->selection() & Annotation::Text)
        {
            dtx = dx;
            dty = dy;
        }

        Point p=annotation->position();
        QPoint pos=_zoom->toScreenPosition(p.x() + dtx, p.y() + dty);

        if (annotation->selection() & Annotation::Text)
            pen.setColor(Qt::gray);
        else
            pen.setColor(Qt::red);
        painter.setPen(pen);

        if (_annotationActionType == AnnotationBeginTranslation || _annotationActionType == AnnotationEndTranslation)
            copied.updateText();

        painter.drawText(pos.x(), pos.y(), copied.text().c_str());

        pen.setColor(Qt::black);
        painter.setPen(pen);

        // Cotation selection.
        if (annotation->selection() & Annotation::Text)
        {
            // Text part.
            QPen p(Qt::DashLine);
            p.setColor(Qt::black);

            QFontMetrics metrics(painter.font());

            int w = metrics.horizontalAdvance(annotation->text().c_str());
            int h = metrics.ascent();

            int margin = 2;

            QRect r(pos.x() - margin, pos.y() - h - margin, w + 2 * margin, metrics.height() + 2 * margin);

            painter.setPen(p);
            painter.setBrush(Qt::transparent);
            painter.drawRect(r);
        }

        if (annotation->selection() & Annotation::Lines)
        {
            // Arrow part.
            painter.setPen(Qt::black);
            painter.setBrush(Qt::black);

            painter.drawEllipse(b, 3, 3);
            painter.drawEllipse(e, 3, 3);
        }
    }

    // Angle.
    else if (annotation->type()==Annotation::AnnotationAngle)
    {
        Point begin=annotation->begin();
        Point end=annotation->end();
        Point center=annotation->angleCenter();

        double bdx = 0.0;
        double edx = 0.0;
        double pdx = 0.0;
        double bdy = 0.0;
        double edy = 0.0;
        double pdy = 0.0;

        if (_annotationActionType == AnnotationTranslation && annotation->selection() & Annotation::Lines)
        {
            bdx = dx;
            bdy = dy;
            edx = dx;
            edy = dy;
            pdx = dx;
            pdy = dy;
        }

        else if (_annotationActionType == AnnotationBeginTranslation)
        {
            bdx = dx;
            bdy = dy;
        }

        else if (_annotationActionType == AnnotationEndTranslation)
        {
            edx = dx;
            edy = dy;
        }

        else if (_annotationActionType == AnnotationCenterTranslation)
        {
            pdx = dx;
            pdy = dy;
        }

        // Copy annotation
        Annotation copied = *annotation;

        Point begin2;
        begin2.setX(begin.x() + bdx);
        begin2.setY(begin.y() + bdy);
        copied.setBegin(begin2);

        Point end2;
        end2.setX(end.x() + edx);
        end2.setY(end.y() + edy);
        copied.setEnd(end2);

        Point position2;
        position2.setX(center.x() + pdx);
        position2.setY(center.y() + pdy);
        copied.setAngleCenter(position2);

        QPoint b=_zoom->toScreenPosition(copied.begin().x(), copied.begin().y());
        QPoint e=_zoom->toScreenPosition(copied.end().x(), copied.end().y());
        QPoint c=_zoom->toScreenPosition(copied.angleCenter().x(), copied.angleCenter().y());

        if (e == c)
            return;

        if (b == c)
            return;

        QPen pen = painter.pen();
        if (annotation->selection() & Annotation::Lines)
            pen.setColor(Qt::gray);
        else
            pen.setColor(Qt::red);
        painter.setPen(pen);

        painter.drawLine(c, b);
        painter.drawLine(c, e);

        double dtx = 0.0;
        double dty = 0.0;

        if (_annotationActionType == AnnotationTranslation && annotation->selection() & Annotation::Text)
        {
            dtx = dx;
            dty = dy;
        }

        Point p=annotation->position();
        QPoint pos=_zoom->toScreenPosition(p.x() + dtx, p.y() + dty);

        if (annotation->selection() & Annotation::Text)
            pen.setColor(Qt::gray);
        else
            pen.setColor(Qt::red);
        painter.setPen(pen);

        if (_annotationActionType == AnnotationBeginTranslation || _annotationActionType == AnnotationEndTranslation || _annotationActionType == AnnotationCenterTranslation)
            copied.updateText();

        if (b != e)
            painter.drawText(pos.x(), pos.y(), copied.text().c_str());

        pen.setColor(Qt::black);
        painter.setPen(pen);

        // Angle selection.
        if (annotation->selection() & Annotation::Text)
        {
            // Text part.
            QPen p(Qt::DashLine);
            p.setColor(Qt::black);

            QFontMetrics metrics(painter.font());

            int w = metrics.horizontalAdvance(annotation->text().c_str());
            int h = metrics.ascent();

            int margin = 2;

            QRect r(pos.x() - margin, pos.y() - h - margin, w + 2 * margin, metrics.height() + 2 * margin);

            painter.setPen(p);
            painter.setBrush(Qt::transparent);
            painter.drawRect(r);
        }

        if (annotation->selection() & Annotation::Lines)
        {
            // Points part.
            painter.setPen(Qt::black);
            painter.setBrush(Qt::black);

            painter.drawEllipse(b, 3, 3);
            painter.drawEllipse(e, 3, 3);
            painter.drawEllipse(c, 3, 3);
        }
    }
}

void ModelViewer::drawAnnotations(Model *model, QPainter &painter)
{
    if (!model)
        return;

    QFont font=painter.font();
    font.setPixelSize(14);
    painter.setFont(font);
	
    for (unsigned int i=0 ; i<model->std::vector<Annotation*>::size() ; i++)
    {
        Annotation *annotation=model->std::vector<Annotation*>::at(i);
        drawAnnotation(painter, annotation);
    }
}
