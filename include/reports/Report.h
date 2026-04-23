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

#ifndef _REPORT_H
#define _REPORT_H

#include <reports/ReportContent.h>
#include <StatesObserver.h>
#include <Translatable.h>
#include <MenuButton.h>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QWidget>
#include <QString>
#include <QMenu>

class Report: public QWidget,
              public Translatable,
              public StatesObserver
{
    Q_OBJECT
public:
    Report(QWidget *parent=0);
    virtual ~Report();

    void setMaterialLegend(const QImage &image);
    void setBoundaryConditionLegend(const QImage &image);
    void setEnvironmentsLegend(const QImage &image);
    void setModel(const QImage &image);
    void set1DModel(const QImage &image);
    void setResultsView(const QImage &image);
    void setEnvironmentsView(const QImage &image);
    void setNodesNumber(const double convergency);
    void setFluxVariation(const double fluxVariation);
    void set1DFlux(const double flux);
    void set2DFlux(const double flux);
    void setPsiCoefficient(const QString &psi);
    void setRatio(const double ratio);
    void setBCTemperatures(const std::vector<BoundaryConditionData> &bc);

    void setDocxModelList(QList<QString> &list);

    void translate();
    void statesChanged();
    void updateExportedData();
    void applyTheme();

private:
    void setIcons();
    void setTexts();

    QVBoxLayout     _layout;
    QHBoxLayout     _toolbar;
    MenuButton      _export;
    QScrollArea     _scrollArea;
    ReportContent   _report;
    QMenu           _exportMenu;
    QMenu           _docxExportMenu;
    QAction        *_pdfExport;
    QAction        *_exportMenuAction;
};

#endif
