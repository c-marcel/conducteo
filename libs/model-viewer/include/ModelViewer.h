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

#ifndef MODEL_VIEWER_H
#define MODEL_VIEWER_H

#include <geometry/transformations/Rotation.h>
#include <plugins/En13370Interface.h>
#include <TranslationBoard.h>
#include <StatesObserver.h>
#include <Translatable.h>
#include <QGridLayout>
#include <DxfViewer.h>
#include <QScrollArea>
#include <QWidget>
#include <QColor>

class Annotation;
class DxfContent;
class Surface;
class Volume;
class Model;
class Image;
class Mesh;
class Cell;
class Path;
class Zoom;

class BoundaryConditionData
{
public:
    BoundaryConditionData();
    virtual ~BoundaryConditionData();

    double      _resistance;
    double      _temperature;

    double      _tmin;
    double      _tmax;
    double      _xmin;
    double      _ymin;
    double      _xmax;
    double      _ymax;

    bool        _useFactor;
    double      _minFactor;
    double      _maxFactor;

    QColor      _color;
    QString     _name;

    std::string _id;
};

class ModelViewer: public QWidget,
                   public StatesObserver,
                   public Translatable,
                   public DxfViewer
{
    Q_OBJECT
public:
    enum Mode
    {
        Idle,
        ViewMouseTranslation,
        SelectionTranslation,
        SelectionRotation,
        ScaleImage,
        VolumeCornerTranslation,
        VolumeBorderTranslation
    };

    enum ImageOptions
    {
        NoOptions               = 0,
        DrawVolumes             = 1,
        DrawBoundaryConditions  = 2,
        DrawTemperatures        = 4,
        DrawSupTemp             = 8,
        DrawEnvironments        = 16,
        DrawEn13370Results      = 32,
        DrawRhResults           = 64,
        DrawCondensationResults = 128
    };

    ModelViewer(QWidget *parent=0);
    virtual ~ModelViewer();

    void statesChanged();
    void zoomChanged();
    void applyTheme();

    void setModel(Model *model);

    static QColor getColorByTemperature(const double xmin, const double xmax, double value);
    static QColor getColorByRh(double value);

    static void createQtImageData(Image *image);
    static void createQtDxfData(DxfContent *dxf);

    void setCustomZoom(Zoom *zoom);
    QImage toImage(Model *model, int width, int height, ImageOptions options, bool include_images, bool include_dxf, bool include_annotations, Model *model2d=0);
    QImage getMaterialLegend() const;
    QImage getBoundaryConditionLegend() const;
    QImage getEnvironmentsLegend() const;
    std::vector<BoundaryConditionData> getBCSuperficialTemperatures() const;

signals:
    void hideMousePosition();
    void mousePositionChanged(double x, double y);
    void hideMouseDimension();
    void mouseDimensionChanged(double w, double h);

    void mouseScreenPositionChanged(int x, int y);
    void setTemperature(double temperature);
    void unSetTemperature();

    void setHumidity(double temperature, double humidity);
    void unSetHumidity();

    void materialUsed(const QString &id);
    void selectMaterialByIdRequested(const QString &id);

    void copyRequested();
    void cutRequested();
    void deleteRequested();
    void rotationRequested(double x, double y);

public slots:
    void downTranslation();
    void upTranslation();
    void leftTranslation();
    void rightTranslation();
    void en13370Open();

protected:
    void mouseMoveEvent(QMouseEvent * event);
    void leaveEvent(QEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void customContextMenuRequested(const QPoint &pos);
    void resizeEvent(QResizeEvent * event);

private:
    enum ResizeType
    {
        Translation = 0,
        Left = 1,
        Right = 2,
        Top = 4,
        Bottom =8
    };

    void drawGridAndOrigin(QPainter &painter, ImageOptions options);
    void drawModel(Model *model, QPainter &painter, ImageOptions options=NoOptions, bool include_dxf=true, bool include_images=true);
    void drawImages(Model *model, QPainter &painter);
    void drawAnnotations(Model *model, QPainter &painter);
    void drawAnnotation(QPainter &painter, Annotation *annotation);
    void drawDxf(Model *model, QPainter &painter);
    void drawMesh(Mesh *mesh, QPainter &painter);
    void drawMouseSelection(QPainter &painter);
    void deleteCurrentDrawnVolume();
    void selectVolumesUnderMouseSelection();
    void selectDxfEntities(double x, double y, double epsilon, bool ctrl_key);

    void drawVolumes(Model *model, QPainter &painter, ImageOptions options, bool include_dxf, bool include_images);

    void drawCellTemperatures(Model *model, QPainter &painter);
    void drawCellRh(Model *model, QPainter &painter);
    void drawCellCondensation(Model *model, QPainter &painter);

    void drawSurfaces(Model *model, QPainter &painter, ImageOptions options, Surface *surface_under);
    void drawSuperficialTemperatures(Model *model, QPainter &painter);
    void drawEn13370Results(Model *model, QPainter &painter, int height, int width, Model *model2d=0);
    
    void drawRectangle(const QPoint &point, bool snap);
    void updateRectangleGeometry(double x, double y);
    void finalizeRectangle();

    void drawCircle(const QPoint &point, bool snap);
    void updateCircleGeometry(double x, double y);
    void finalizeCircle();

    void drawEllipse(const QPoint &point, bool snap);
    void updateEllipseGeometry(double x, double y);
    void finalizeEllipse();

    void drawPolyline(const QPoint &point, bool snap, bool vh_align);
    void removePolylinePoint();
    bool polylineIntersection(double x, double y) const;
    void finalizePolyline();

    void drawLengthDataPoint(const QPoint &point, bool snap, bool vh_align);
    void finalizeLengthData();

    void drawAngleDataPoint(const QPoint &point, bool snap, bool vh_align);
    void finalizeAngleData();

    void addText(const QPoint &point);
    void updateText(Annotation *annotation);
    
    void addArrow(const QPoint &point, bool snap, bool vh_align);
    void finalizeArrow();

    void selectVolume(const QPoint &point, Qt::KeyboardModifiers modifiers);
    void translateWithKeyboard(int key);
    void finalizeSelectionTranslation();
    void finalizeSelectionRotation();
    void updateRotationCenter();
    bool mouseOnRotationPoint(const QPoint &pos) const;

    void setPhysic(QPoint &pos);
    void setEnvironment(QPoint &pos);
    void surfaceSelection(QPoint &pos);

    double snapToGridX(const QPoint &pos) const;
    double snapToGridY(const QPoint &pos) const;

    void initScaleDefinition();
    void finalizeScaleDefinition();
    void finalizeCornerTranslation();
    void finalizeBorderTranslation();
    void addPoint();

    void setEnvironmentFromSelection();
    void setPhysicFromSelection();
    void setMaterialFromSelection();
    void setBoundaryConditionFromSelection();

    Point nearPoint(const QPoint &position, bool snap) const;

    std::vector<DxfReader::Entity*> selectedDxfEntities() const;
    DxfReader::Entity *getDxfEntityAtPosition(double x, double y, double epsilon) const;
    void importSelectedElements();
    std::string dxfElementsToXml(const std::vector<DxfReader::Entity*> &elements) const;

    inline int round(double v) const;

    ResizeType getResizeType(Image *image, const QPoint &pos) const;
    Qt::CursorShape cursorFromResizeType(ResizeType type) const;

    void deletePointRequested(Volume *volume, Point &point);

    void selectMaterial(QMouseEvent *event);

    Model              *_model;
    Mode                _currentMode;
    QGridLayout         _layout;
    TranslationBoard    _verticalTranslationBoard;
    TranslationBoard    _horizontalTranslationBoard;
    QPoint              _mouseClickPoint;
    QGridLayout         _en13370Layout;
    QPushButton         _en13370Button;
    QScrollArea         _en13370Area;
    En13370Interface    _en13370Interface;

    Volume             *_currentDrawnVolume;

    double              _dxTranslation;
    double              _dyTranslation;
    Rotation            _rotation;
    bool                _hideRotation;
    QCursor             _rotationCursor;

    ResizeType          _resizeType;
    std::vector<Point>  _imageScale;

    bool                _mousePressed;
    Point               _currentSelectionBegin;
    Point               _currentSelectionEnd;
    bool                _ctrlPressed;
    bool                _majPressed;

    enum MouseOverElement
    {
        Nothing,
        Border,
        Corner
    };

    MouseOverElement    _mouseOverElementType;
    Point               _mouseOverPoint;
    Path               *_mouseOverPath;
    Volume             *_mouseOverVolume;
    double              _dxPointTranslation;
    double              _dyPointTranslation;
    double              _lastDxPointTranslation;
    double              _lastDyPointTranslation;
    QCursor             _resizeCursor;
    QPixmap             _resizeOriginalImage;

    std::vector<Point>  _polylinePoints;
    QPoint              _mousePosition;

    Point               _addNearPoint;
    bool                _displayNearPoint;

#ifdef DISPLAY_MATRIX_CONTENT
    std::vector<unsigned int>   _highlightCells;
    Cell                       *_centeredCell;
#endif

    std::vector<Point>          _lengthDataPoint;
    std::vector<Point>          _angleDataPoint;
    std::vector<Point>          _arrowPoint;

    Zoom                       *_zoom;

    enum AnnotationActionType
    {
        AnnotationNoAction          = 0,
        AnnotationTranslation       = 1,
        AnnotationBeginTranslation  = 2,
        AnnotationEndTranslation    = 4,
        AnnotationCenterTranslation = 8
    };

    AnnotationActionType    _annotationActionType;
};

#endif
