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

#ifndef _ZOOM_H
#define _ZOOM_H

#include <geometry/Point.h>
#include <QObject>
#include <QPoint>

class Zoom: public QObject
{

    Q_OBJECT

public:

    Zoom(QObject *parent=0);
    virtual ~Zoom();

    void increase(const QPoint &center);
    void decrease(const QPoint &center);

    void up(double dy);
    void down(double dy);
    void left(double dx);
    void right(double dx);

    void fitTo(double xmin, double ymin, double xmax, double ymax, double width, double height, bool force=false);

    QPoint toScreenPosition(double x, double y) const;

    double toPhysicalX(const QPoint &position) const;
    double toPhysicalY(const QPoint &position) const;
    double toPhysicalLength(double distance) const;
    double toScreenLength(double distance) const;

    const double gridStep() const;
    bool isZoomAtMax() const;
    bool isZoomAtMin() const;

    void setMaxZoom(double zoom);

signals:

    void zoomChanged();

private:

    double increasedZoom() const;
    double decreasedZoom() const;
    void updateOrigin(double zoom, const QPoint &center);
    void updateGridStep();

    // Origin.
    Point           _origin;            ///< Physical coordinates of screen origin point.

    // Zoom.
    double          _zoom;              ///< Zoom factor.
    double          _maxZoom;           ///< Max zoom factor.
    double          _minZoom;           ///< Min zoom factor.
    double          _zoomMultiplier;    ///< Zoom multiplier to increase/decrease zoom factor.

    double          _gridStep;
    double          _minScreenGridStep;
    
};

#endif
