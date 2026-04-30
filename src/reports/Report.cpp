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

#include <reports/Report.h>
#include <LinguistManager.h>
#include <QDesktopServices>
#include <tools/ToolBox.h>
#include <StatesManager.h>
#include <LocalData.h>
#include <QFileDialog>
#include <QPainter>
#include <Log.h>
#include <QUrl>

Report::Report(QWidget *parent):
    QWidget(parent),
    _layout(this),
    _pdfExport(0),
    _exportMenu(0)
{
    _toolbar.addWidget(&_export);
    QSpacerItem *hS1=new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    _toolbar.addItem(hS1);
    _layout.addLayout(&_toolbar);

    _scrollArea.setWidget(&_report);
    _scrollArea.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _layout.addWidget(&_scrollArea);

    _export.installMenu(&_exportMenu);
    _export.setStyleSheet ("text-align: left;");
    _export.useParentWidth(false);

#ifndef _WIN32
    _export.setMargins(0, -15);
#endif

    _pdfExport=_exportMenu.addAction(_tr("PdfReport"));
    _pdfExport->setIconVisibleInMenu(true);

    _exportMenuAction=_exportMenu.addMenu(&_docxExportMenu);
    _exportMenuAction->setIconVisibleInMenu(true);
    
    connect(_pdfExport, &QAction::triggered, &_report, &ReportContent::onPdfExport);

    translate();
    applyTheme();
    setTexts();
}

Report::~Report()
{
}

void Report::setIcons()
{
    _export.setIcon(QIcon(":/images/orange-blue/export.png"));
    _pdfExport->setIcon(QIcon(":/images/orange-blue/pdf.png"));
    _exportMenuAction->setIcon(QIcon(":/images/orange-blue/docx.png"));
    _export.setIconSize(QSize(24, 24));
}

void Report::setTexts()
{
    _export.setTitle(_tr("ExportReport"));
}

void Report::setMaterialLegend(const QImage &image)
{
    _report.setMaterialLegend(image);
}

void Report::setBoundaryConditionLegend(const QImage &image)
{
    _report.setBoundaryConditionLegend(image);
}

void Report::setEnvironmentsLegend(const QImage &image)
{
    _report.setEnvironmentsLegend(image);
}

void Report::setModel(const QImage &image)
{
    _report.setModel(image);
}

void Report::set1DModel(const QImage &image)
{
    _report.set1DModel(image);
}

void Report::setResultsView(const QImage &image)
{
    _report.setResultsView(image);
}

void Report::setEnvironmentsView(const QImage &image)
{
    _report.setEnvironmentsView(image);
}

void Report::setNodesNumber(const double nodes)
{
    _report.setNodesNumber(nodes);
}

void Report::setFluxVariation(const double fluxVariation)
{
    _report.setFluxVariation(fluxVariation);
}

void Report::set1DFlux(const double flux)
{
    _report.set1DFlux(flux);
}

void Report::set2DFlux(const double flux)
{
    _report.set2DFlux(flux);
}

void Report::setPsiCoefficient(const QString &psi)
{
    _report.setPsiCoefficient(psi);
}

void Report::setRatio(const double ratio)
{
    _report.setRatio(ratio);
}

void Report::setBCTemperatures(const std::vector<BoundaryConditionData> &bc)
{
    _report.setBCTemperatures(bc);
}

void Report::setDocxModelList(QList<QString> &list)
{
    _docxExportMenu.clear();

    // Add default docx template.
    QAction *default_template=_docxExportMenu.addAction(_tr("Defaultc2dReport"));
    connect(default_template, &QAction::triggered, &_report, &ReportContent::onDocxExport, Qt::UniqueConnection);

    if (list.isEmpty())
        return;

    _docxExportMenu.addSeparator();
    for (int i=0 ; i<list.size() ; i++)
    {
        QAction *action=_docxExportMenu.addAction(list.at(i));
        connect(action, &QAction::triggered, &_report, &ReportContent::onCustomDocxExport, Qt::UniqueConnection);
    }
}

void Report::translate()
{
    _docxExportMenu.setTitle(_tr("DocxReport"));
    _pdfExport->setText(_tr("PdfReport"));
    setTexts();
    if (!_docxExportMenu.actions().isEmpty())
        _docxExportMenu.actions().at(0)->setText(tr("Defaultc2dReport"));
    update();
}

void Report::updateExportedData()
{
    _report.updateExportedData();
}

void Report::applyTheme()
{
    setIcons();
    update();
}

void Report::statesChanged()
{
    LOG_INFO("States changed caught.");

    setVisible(StatesManager::instance()->viewType()==StatesManager::DisplayReport);

    if (StatesManager::instance()->viewType()==StatesManager::DisplayReport)
    {
        _docxExportMenu.clear();

        // Add default docx template.
        QAction *default_template=_docxExportMenu.addAction(_tr("Defaultc2dReport"));
        connect(default_template,&QAction::triggered, &_report, &ReportContent::onDocxExport, Qt::UniqueConnection);

        std::string docx=LocalData::instance()->getParameter("docx-templates");
        std::vector<std::string> files=ToolBox::split(docx, '|');
        
        QList<QString> list;
        for (unsigned int i=0 ; i<files.size() ; i++)
        {
            QString file_name=files.at(i).c_str();
#ifdef WIN32
            file_name.replace("/", "\\");
#endif

            if (!QFile(file_name).exists())
                continue;

            list.append(file_name);
        }

        if (list.isEmpty())
            return;

        _docxExportMenu.addSeparator();
        for (int i=0 ; i<list.size() ; i++)
        {
            QAction *action=_docxExportMenu.addAction(list.at(i));
            connect(action, &QAction::triggered, &_report, &ReportContent::onCustomDocxExport, Qt::UniqueConnection);
        }
    }
}
