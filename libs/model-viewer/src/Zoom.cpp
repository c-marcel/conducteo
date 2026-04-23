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

#include <Zoom.h>
#include <cmath>

Zoom::Zoom(QObject *parent):
    QObject(parent),
    _zoom(500.0),
    _zoomMultiplier(1.3),
    _maxZoom(50000.0),
    _minZoom(10.0),
    _minScreenGridStep(20.0)
{
    updateGridStep();
}

Zoom::~Zoom()
{
}

void Zoom::increase(const QPoint &center)
{
    updateOrigin(increasedZoom(), center);
    _zoom=increasedZoom();
    updateGridStep();
    emit zoomChanged();
}

void Zoom::decrease(const QPoint &center)
{
    updateOrigin(decreasedZoom(), center);
    _zoom=decreasedZoom();
    updateGridStep();
    emit zoomChanged();
}

QPoint Zoom::toScreenPosition(double x, double y) const
{
    return QPoint(_zoom*(x-_origin.x()), -_zoom*(y-_origin.y()));
}

double Zoom::toPhysicalX(const QPoint &position) const
{
    return _origin.x()+position.x()/double(_zoom);
}

double Zoom::toPhysicalY(const QPoint &position) const
{
    return _origin.y()-position.y()/double(_zoom);
}

void Zoom::up(double dy)
{
    _origin.setY(_origin.y()-dy);
    emit zoomChanged();
}

void Zoom::down(double dy)
{
    _origin.setY(_origin.y()+dy);
    emit zoomChanged();
}

void Zoom::left(double dx)
{
    _origin.setX(_origin.x()-dx);
    emit zoomChanged();
}

void Zoom::right(double dx)
{
    _origin.setX(_origin.x()+dx);
    emit zoomChanged();
}

double Zoom::toPhysicalLength(double distance) const
{
    return distance/_zoom;
}

double Zoom::toScreenLength(double distance) const
{
    return distance*_zoom;
}

bool Zoom::isZoomAtMax() const
{
    return (increasedZoom()==_zoom);
}

bool Zoom::isZoomAtMin() const
{
    return (decreasedZoom()==_zoom);
}

double Zoom::increasedZoom() const
{
    double zoom=_zoom*_zoomMultiplier;
    if (zoom>_maxZoom)
        zoom=_zoom;
    return zoom;
}

double Zoom::decreasedZoom() const
{
    double zoom=_zoom/_zoomMultiplier;
    if (zoom<_minZoom)
        zoom=_zoom;
    return zoom;
}

void Zoom::updateOrigin(double zoom, const QPoint &center)
{
    // compute model fixed point.
    double x=toPhysicalX(center);
    double y=toPhysicalY(center);
    // compute origin to keep screen & model points fixed.
    double xOrigin=x-double(center.x())/zoom;
    double yOrigin=y+double(center.y())/zoom;
    _origin.setX(xOrigin);
    _origin.setY(yOrigin);
}

void Zoom::updateGridStep()
{
    // Get physical minimal grid step.
    double min_step=toPhysicalLength(_minScreenGridStep);
    
    // Get power.
    int p=int(-log10(min_step));

    double ratio=min_step/pow(0.1, p+1);
    if (ratio<=1.5)
        ratio=1.0;
    else if (ratio<=3.5)
        ratio=2.0;
    else if (ratio<=7.5)
        ratio=5.0;
    else
        ratio=10.0;

    // Compute dx.
    _gridStep=ratio*pow(0.1, p+1);
}

const double Zoom::gridStep() const
{
    return _gridStep;
}

void Zoom::fitTo(double xmin, double ymin, double xmax, double ymax, double width, double height, bool force)
{
    if (xmin==xmax)
    {
        xmin-=1.0;
        xmax+=1.0;
    }

    if (ymin==ymax)
    {
        ymin-=1.0;
        ymax+=1.0;
    }

    double zoom_x=width/(xmax-xmin);
    double zoom_y=height/(ymax-ymin);

    double zoom=std::min(zoom_x, zoom_y);

    _zoom=_maxZoom;
    while (_zoom>zoom && _zoom!=decreasedZoom() && !force)
        _zoom=decreasedZoom();
    if (force)
        _zoom=zoom;

    // Origin.
    double w=toPhysicalLength(width);
    double h=toPhysicalLength(height);

    _origin.setX(xmin-(w-(xmax-xmin))/2.0);
    _origin.setY(ymax+(h-(ymax-ymin))/2.0);
    
    updateGridStep();

    emit zoomChanged();
}

void Zoom::setMaxZoom(double zoom)
{
    _maxZoom=zoom;
}
