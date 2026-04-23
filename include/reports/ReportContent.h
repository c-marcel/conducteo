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

#ifndef _REPORT_CONTENT_H
#define _REPORT_CONTENT_H

#include <StatesObserver.h>
#include <Translatable.h>
#include <ModelViewer.h>
#include <model/Model.h>
#include <QPushButton>
#include <QWidget>

class DocxImage;

class ReportContent: public QWidget,
                     public Translatable,
                     public StatesObserver
{
    Q_OBJECT
public:
    ReportContent(QWidget *parent=0);
    virtual ~ReportContent();

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
    void setLambda(const double lambda);
    void setRatio(const double ratio);
    void setPsiCoefficient(const QString &psi);
    void setBCTemperatures(const std::vector<BoundaryConditionData> &bc);
    
    void statesChanged();
    void translate();
    void updateExportedData();

    bool getCondensation() const;
    void setCondensation(bool condensation);

protected:
    virtual void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent * event);


private slots:
    void onPdfExport();
    void onDocxExport();
    void onCustomDocxExport();

private:
    void updateContent();
    void drawReport(QPainter *painter, int width);
    void drawReport_page2(QPainter *painter, int width, int offset=0);
    void drawReport_page3(QPainter *painter, int width, int offset=0);

    std::string createXmlData() const;
    std::string getDictionaryContent() const;
    double getCouplingCoefficient(const std::string &env1, const std::string &env2) const;
    std::vector<DocxImage*> getImageContents() const;
    void exportDocx(const std::string &model);
    std::string imageData(const QImage &image) const;
    DocxImage *qimageToDocxImage(const QImage &image, const std::string &name) const;
    QImage boundaryConditionImage(const BoundaryConditionData &bc) const;
    std::string imageToPngStream(const QImage &image) const;

    QImage      _materialLegend;
    QImage      _boundaryConditionLegend;
    QImage      _environmentsLegend;

    QImage      _model;
    QImage      _1Dmodel;
    QImage      _results;
    QImage      _1Dresults;
    QImage      _environments;
    QImage      _condensation;
    QImage      _rh;

    double      _nodes;
    double      _fluxVariation;
    double      _1DFlux;
    double      _2DFlux;
    double      _ratio;
    double      _lambda;
    QString     _psi;
    bool        _hasCondensation;

    int         _height;
    int         _margins;
    int         _view_height;
    int         _header_height;
    int         _legend_width;

    std::vector<BoundaryConditionData>  _bc;
    std::vector<EnvironmentResult>      _environmentsResults;

    bool        _translate2dView;
    bool        _translate1dView;
    bool        _translateResultsView;
    bool        _translateEnvsView;
    bool        _translateCondensationView;
    QPoint      _mousePoint;

    mutable QImage  _model_0;
    mutable QImage  _model_1;
    mutable QImage  _model_2;
    mutable QImage  _model_3;
    mutable QImage  _model_4;
    mutable QImage  _model_5;
    mutable QImage  _model_6;
    mutable QImage  _model_7;

    mutable QImage  _model_1d_0;
    mutable QImage  _model_1d_1;
    mutable QImage  _model_1d_2;
    mutable QImage  _model_1d_3;
    mutable QImage  _model_1d_4;
    mutable QImage  _model_1d_5;
    mutable QImage  _model_1d_6;
    mutable QImage  _model_1d_7;

    mutable QImage  _results_0;
    mutable QImage  _results_1;
    mutable QImage  _results_2;
    mutable QImage  _results_3;
    mutable QImage  _results_4;
    mutable QImage  _results_5;
    mutable QImage  _results_6;
    mutable QImage  _results_7;

    mutable QImage  _envs_0;
    mutable QImage  _envs_1;
    mutable QImage  _envs_2;
    mutable QImage  _envs_3;
    mutable QImage  _envs_4;
    mutable QImage  _envs_5;
    mutable QImage  _envs_6;
    mutable QImage  _envs_7;

    mutable QImage  _condensation_0;
    mutable QImage  _condensation_1;
    mutable QImage  _condensation_2;
    mutable QImage  _condensation_3;
    mutable QImage  _condensation_4;
    mutable QImage  _condensation_5;
    mutable QImage  _condensation_6;
    mutable QImage  _condensation_7;

    mutable QImage  _rh_0;
    mutable QImage  _rh_1;
    mutable QImage  _rh_2;
    mutable QImage  _rh_3;
    mutable QImage  _rh_4;
    mutable QImage  _rh_5;
    mutable QImage  _rh_6;
    mutable QImage  _rh_7;

    friend class Report;
};

#endif
