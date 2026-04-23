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

#ifndef _MATERIAL_CUSTOM_ITEM_H
#define _MATERIAL_CUSTOM_ITEM_H

#include <actions/Observer.h>
#include <QTreeWidgetItem>
#include <Translatable.h>
#include <QWidget>

class MaterialProperties;
class Material;

class MaterialCustomItem: public QWidget,
                          public QTreeWidgetItem,
                          public Observer,
                          public Translatable
{
    Q_OBJECT
public:
    MaterialCustomItem();
    virtual ~MaterialCustomItem();

    void setMaterial(Material *material);
    void setTitle(const QString &title);
    void setColor(const QColor &color);
    void setConductivity(double conductivity);
    void setEmissivity(double emissivity);
    void setMu(double mu);
    void setDescription(const QString &description);
    void setFavorite(bool favorite);
    void setExtrusion(bool extrusion);

    QString id() const;
    QString title() const;

    void expandItem(QTreeWidgetItem *item);

    void changed();
    void modelAltered();
    void modelResultsAltered();

    void translate();

    bool operator<(const QTreeWidgetItem &other) const;

    static QString escapeAccentedChar(const QString& text);

signals:
    void saveDatabaseRequested();
    void sortListRequested();

public slots:
    void applyFilter(const QString &text);

protected:
    virtual void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void setExpanded(bool expanded);
    void updateToolTip();

    QString                 _id;
    QString                 _title;
    QString                 _filterTitle;
    QString                 _translatedTitle;
    QColor                  _color;
    double                  _conductivity;
    double                  _emissivity;
    double                  _mu;
    QString                 _description;
    MaterialProperties     *_properties;
    bool                    _expanded;
    bool                    _favorite;
    bool                    _hover;
    bool                    _favoriteEnabled;
    bool                    _extrusion;
};

#endif
