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

#include <materials/MaterialCustomItem.h>
#include <actions/materials/ToogleMaterialFavorite.h>
#include <materials/MaterialProperties.h>
#include <actions/ActionsManager.h>
#include <databases/Materials.h>
#include <physics/Material.h>
#include <LinguistManager.h>
#include <StatesManager.h>
#include <QMouseEvent>
#include <QPainter>
#include <QLocale>

#define ItemWidth                       200
#define ClosedItemHeight                25

#ifdef _WIN64
#define ExpandedItemDefaultHeight       165
#endif

#ifdef __APPLE__
#define ExpandedItemDefaultHeight       105
#endif

#ifdef __linux__
#define ExpandedItemDefaultHeight       100
#endif

MaterialCustomItem::MaterialCustomItem():
    _conductivity(0.0),
    _properties(0),
    _expanded(false),
    _favorite(false),
    _hover(false),
    _favoriteEnabled(true),
    _emissivity(0.9),
    _extrusion(false),
    _mu(1.0)
{
    setMouseTracking(true);
    setSizeHint(0, QSize(ItemWidth, ClosedItemHeight));
    applyTheme();
    translate();
}

MaterialCustomItem::~MaterialCustomItem()
{
    Material *material=Materials::instance()->materialFromId(_id.toStdString());
    if (material)
        material->unregisterObserver(this);
}

void MaterialCustomItem::setMaterial(Material *material)
{
    material->registerObserver(this);
    _id=material->id().c_str();

    if (_id=="106D6720-211C-4090-8C60-7CED671C8580")
        _favoriteEnabled=false;
    if (_id=="0BB7BBC8-79A2-4E71-A92F-F8E9033A630B")
        _favoriteEnabled=false;
}

void MaterialCustomItem::paintEvent(QPaintEvent *event)
{
    QPainter qp(this);
    QRect rect=geometry();

    // Draw rectangle.
    QBrush brush(_color);
    qp.setBrush(brush);
    QRect r(4, 2, 30, 18);

    if (!_extrusion)
        qp.drawRect(r);

    // Draw image.
    int w=0;
    if (_favoriteEnabled && !_extrusion)
    {
        QString image="star_empty.png";
        if (_favorite)
            image="star_filled.png";
        if (_hover && !_favorite)
            image="star_hover.png";
        QPixmap pixmap(":/images/orange-blue/"+image);
        pixmap = pixmap.scaledToHeight(25, Qt::SmoothTransformation);
        QRect r2(width()-10-pixmap.width(), (25-pixmap.height())/2, pixmap.width(), pixmap.height());
        qp.drawPixmap(r2, pixmap);
        w=r2.width();
    }

    else if (_extrusion)
    {
        QPixmap pixmap(":/images/orange-blue/extrusion.png");
        pixmap = pixmap.scaledToHeight(25, Qt::SmoothTransformation);
        QRect r2(4, 2, pixmap.width(), pixmap.height());
        qp.drawPixmap(r2, pixmap);
        w=r2.width();
    }

    // Draw text.
    QFont f = qp.font();
    f.setPixelSize(12);
    qp.setFont(f);

    QFontMetrics metric(f);
    QString name = metric.elidedText(_translatedTitle, Qt::ElideRight, rect.width()-r.width()-w-24);

    int h = 25;
    if (_extrusion)
        h = 34;

    qp.drawText(r.width()+10, 0, rect.width()-r.width()-w-24, h, Qt::AlignVCenter, name);
}

void MaterialCustomItem::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p=event->pos();
    QRect r2(width()-10-24, (25-24)/2, 24, 24);
    bool hover=r2.contains(p);
    if (_hover!=hover)
    {
        _hover=hover;
        repaint();
    }
}

void MaterialCustomItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (!_hover)
        return;

    ToogleMaterialFavorite *action=new ToogleMaterialFavorite(_id.toStdString());
    ActionsManager::instance()->exec(action);

    emit sortListRequested();
    emit saveDatabaseRequested();
}

void MaterialCustomItem::leaveEvent(QEvent *event)
{
    _hover=false;
    repaint();
}

void MaterialCustomItem::translate()
{
    QString title    = _tr(_title);
    _translatedTitle = _title;
    _filterTitle     = escapeAccentedChar(_translatedTitle);

    if (!title.isEmpty())
        _translatedTitle=title;

    repaint();
}

void MaterialCustomItem::setTitle(const QString &title)
{
    _title=title;
    translate();
}

void MaterialCustomItem::setColor(const QColor &color)
{
    _color=color;
}

void MaterialCustomItem::setConductivity(double conductivity)
{
    _conductivity=conductivity;
    updateToolTip();
}

void MaterialCustomItem::updateToolTip()
{
    QLocale locale;
    QWidget::setToolTip("λ="+locale.toString(_conductivity)+" W/(m.°C)");
}

void MaterialCustomItem::setExpanded(bool expanded)
{
    if (expanded)
    {
        int n=0;
        QTreeWidgetItem *item = QTreeWidgetItem::parent();
        while (item)
        {
            n++;
            item=item->parent();
        }
        if (!_properties)
            _properties = new MaterialProperties(this);
        _properties->setGeometry(40, 26, width()-50, 76);
        _properties->setIndent(n);
        _properties->setMaterialId(_id);
        _properties->setName(_title);
        _properties->setConductivity(_conductivity);
        _properties->setColor(_color);
        _properties->setDescription(_description);
        _properties->setEmissivity(_emissivity);
        _properties->setMu(_mu);
        _properties->show();
    }
    else if (_properties)
    {
        delete _properties;
        _properties=0;
    }
}

void MaterialCustomItem::modelAltered()
{
    // Do nothing.
}

void MaterialCustomItem::modelResultsAltered()
{
    // Do nothing.
}

void MaterialCustomItem::changed()
{
    Material *material=Materials::instance()->materialFromId(_id.toStdString());
    if (!material)
        return;

    _title=material->name().c_str();
    _color=QColor(material->red(), material->green(), material->blue());
    _conductivity=material->conductivity();
    _description=material->description().c_str();
    _emissivity=material->emissivity();
    _mu = material->mu();
    _favorite=material->favorite();
    translate();

    updateToolTip();

    if (_properties)
    {
        _properties->setName(_title);
        _properties->setConductivity(_conductivity);
        _properties->setMu(_mu);
        _properties->setColor(_color);
        _properties->setDescription(_description);
        _properties->setEmissivity(_emissivity);
    }
    repaint();

    emit saveDatabaseRequested();
}

void MaterialCustomItem::expandItem(QTreeWidgetItem *item)
{
    if (item==this && _expanded)
        _expanded=false;
    else if (item==this)
        _expanded=true;
    else
        _expanded=false;

    if (_expanded)
    {
        int h = ExpandedItemDefaultHeight;

        setSizeHint(0, QSize(ItemWidth, h));
    }
    else
        setSizeHint(0, QSize(ItemWidth, ClosedItemHeight));

    setExpanded(_expanded);
}

QString MaterialCustomItem::id() const
{
    return _id;
}

bool MaterialCustomItem::operator<(const QTreeWidgetItem &other) const
{
    const MaterialCustomItem *material=dynamic_cast<const MaterialCustomItem*>(&other);
    if (!material)
        return false;
    if (_favorite && !material->_favorite)
        return true;
    if (!_favorite && material->_favorite)
        return false;
    return (_filterTitle.toLower()<material->_filterTitle.toLower());
}

QString MaterialCustomItem::escapeAccentedChar(const QString &text)
{
    QString str = text.toLower();

    str.replace("à", "a");
    str.replace("á", "a");
    str.replace("â", "a");
    str.replace("ã", "a");
    str.replace("ä", "a");
    str.replace("å", "a");
    str.replace("æ", "a");
    str.replace("ç", "c");
    str.replace("è", "e");
    str.replace("é", "e");
    str.replace("ê", "e");
    str.replace("ë", "e");
    str.replace("ì", "i");
    str.replace("í", "i");
    str.replace("î", "i");
    str.replace("ï", "i");
    str.replace("ð", "o");
    str.replace("ñ", "n");
    str.replace("ò", "o");
    str.replace("ó", "o");
    str.replace("ô", "o");
    str.replace("õ", "o");
    str.replace("ö", "o");
    str.replace("ø", "o");
    str.replace("ù", "u");
    str.replace("ú", "u");
    str.replace("û", "u");
    str.replace("ü", "u");
    str.replace("ý", "y");
    str.replace("ÿ", "y");

    return str;
}

QString MaterialCustomItem::title() const
{
    return _title;
}

void MaterialCustomItem::setEmissivity(double emissivity)
{
    _emissivity=emissivity;
}

void MaterialCustomItem::setMu(double mu)
{
    _mu = mu;
}

void MaterialCustomItem::setDescription(const QString &description)
{
    _description=description;
}

void MaterialCustomItem::setFavorite(bool favorite)
{
    _favorite=favorite;
}

void MaterialCustomItem::setExtrusion(bool extrusion)
{
    _extrusion = extrusion;

    if (extrusion)
        setSizeHint(0, QSize(ItemWidth, 34));
}

void MaterialCustomItem::applyFilter(const QString &text)
{
    if (text.isEmpty())
        return QTreeWidgetItem::setHidden(false);

    QList<QString> list=escapeAccentedChar(text).toLower().split(" ");
    bool hidden=false;
    for (int i=0 ; i<list.size() ; i++)
    {
        if (!_filterTitle.contains(list.at(i)))
        {
            hidden=true;
            break;
        }
    }
    
    QTreeWidgetItem::setHidden(hidden);
}
