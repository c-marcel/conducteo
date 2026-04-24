// conducte� - free software for 2d thermal bridges computation.
// Copyright (C) 2009-2020, Cl�ment MARCEL.
//
// This file is part of conducte� software.
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

#include <reports/ReportContent.h>
#include <QtPrintSupport/QPrinter>
#include <QDesktopServices>
#include <StatesManager.h>
#include <tools/ToolBox.h>
#include <tools/UiTools.h>
#include <DocxConverter.h>
#include <XMLInputData.h>
#include <model/Model.h>
#include <ModelViewer.h>
#include <DocxImage.h>
#include <QFileDialog>
#include <QWheelEvent>
#include <QPdfWriter>
#include <QPainter>
#include <QLocale>
#include <QBuffer>
#include <Zoom.h>
#include <Log.h>
#include <QUrl>

ReportContent::ReportContent(QWidget *parent):
    QWidget(parent),
    _translate2dView(false),
    _translate1dView(false),
    _translateResultsView(false),
    _translateEnvsView(false),
    _translateCondensationView(false),
    _hasCondensation(false)
{
    _height=1485*3;
    _margins=30;
    _view_height=500;
    _header_height=300;
    _legend_width=250;

    resize(1015, _height);
}

ReportContent::~ReportContent()
{
}

void ReportContent::statesChanged()
{
    if (StatesManager::instance()->viewType() != StatesManager::DisplayReport)
        return;

    updateContent();
}

void ReportContent::updateExportedData()
{
    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    updateContent();

    // Save images into model.
    if (model)
    {
        model->set2dPngView(imageToPngStream(_model));
        model->set1dPngView(imageToPngStream(_1Dmodel));

        model->set2dResultsPngView(imageToPngStream(_results));
        model->set1dResultsPngView(imageToPngStream(_1Dresults));
    }
}

void ReportContent::updateContent()
{
    LOG_INFO("States changed caught.");

    LOG_INFO("Getting model.");

    Model *model=StatesManager::instance()->currentModel();
    if (!model)
    {
        LOG_WARNING("No model defined.");
        return;
    }

    QLocale locale;

    // Recreate report.
    LOG_INFO("Creating model viewer.");

    ModelViewer viewer;

    LOG_INFO("Creating legends.");

    _materialLegend=viewer.getMaterialLegend();
    _boundaryConditionLegend=viewer.getBoundaryConditionLegend();
    _environmentsLegend=viewer.getEnvironmentsLegend();

    LOG_INFO("Creating zoom object.");

    Zoom zoom;
    viewer.setCustomZoom(&zoom);

    LOG_INFO("Getting export options.");

    bool include_annotations=StatesManager::instance()->includeAnnotationsIn2dView();
    bool include_images=StatesManager::instance()->includeImagesIn2dView();
    bool include_dxf=StatesManager::instance()->includeDxfIn2dView();

    LOG_INFO("Setting zoom value.");
    
    zoom.fitTo(model->xMin(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->yMin(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->xMax(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->yMax(Model::UserView2dModel, include_images, include_dxf, include_annotations), 1015-250-3*30, 500, true);

    LOG_INFO("Creating model image.");

    _model=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), include_images, include_dxf, include_annotations);

    LOG_INFO("Getting model includes.");

    include_annotations=StatesManager::instance()->includeAnnotationsIn1dView();
    include_images=StatesManager::instance()->includeImagesIn1dView();
    include_dxf=StatesManager::instance()->includeDxfIn1dView();

    LOG_INFO("1d model image.");

    if (model->computationType()!=Model::Flux && model->computationType()!=Model::EquivalentThermalConductivity && model->computationType() != Model::GlaserCondensation)
    {
        LOG_INFO("Setting 1d model zoom.");

        zoom.fitTo(model->xMin(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->yMin(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->xMax(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->yMax(Model::UserView1dModel, include_images, include_dxf, include_annotations), 1015-250-3*30, 500, true);

        LOG_INFO("Creating 1d model image.");

        _1Dmodel=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), include_images, include_dxf, include_annotations, model);
    }
    else
    {
        LOG_INFO("Creating null 1d model image.");

        _1Dmodel=QImage();
    }
    
    if (model->computationType() == Model::GlaserCondensation)
    {
        LOG_INFO("Setting condensation zoom.");

        zoom.fitTo(model->xMin(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->yMin(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->xMax(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->yMax(Model::UserView2dModel, include_images, include_dxf, include_annotations), 1015-250-3*30, 500, true);
        
        LOG_INFO("Creating condensation image.");

        _condensation = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes | ModelViewer::DrawCondensationResults), include_images, include_dxf, include_annotations, model);

        LOG_INFO("Creating rh image.");

        _rh = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes | ModelViewer::DrawRhResults), include_images, include_dxf, include_annotations, model);
    }
    else
    {
        LOG_INFO("No condensation: null images for condensation and rh.");

        _condensation = QImage();
        _rh           = QImage();
    }

    LOG_INFO("Getting results view includes.");

    include_annotations=StatesManager::instance()->includeAnnotationsInResultsView();
    include_images=StatesManager::instance()->includeImagesInResultsView();
    include_dxf=StatesManager::instance()->includeDxfInResultsView();

    LOG_INFO("Setting zoom for results image.");

    zoom.fitTo(model->xMin(Model::UserViewResults, include_images, include_dxf, include_annotations), model->yMin(Model::UserViewResults, include_images, include_dxf, include_annotations), model->xMax(Model::UserViewResults, include_images, include_dxf, include_annotations), model->yMax(Model::UserViewResults, include_images, include_dxf, include_annotations), 1015-250-3*30, 500, true);

    LOG_INFO("Creating image for results.");

    _results=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), include_images, include_dxf, include_annotations);

    if (model->computationType()!=Model::Flux && model->computationType()!=Model::EquivalentThermalConductivity)
    {
        LOG_INFO("Setting zoom for 1d results.");

        zoom.fitTo(model->xMin(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->yMin(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->xMax(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->yMax(Model::UserView1dModel, include_images, include_dxf, include_annotations), 1015-250-3*30, 500, true);

        LOG_INFO("Creating image for 1d results.");

        _1Dresults=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), include_images, include_dxf, include_annotations);
    }
    else
    {
        LOG_INFO("No 1d results: null image.");

        _1Dresults=QImage();
    }

    LOG_INFO("Getting envs includes.");

    include_annotations=StatesManager::instance()->includeAnnotationsInEnvsView();
    include_images=StatesManager::instance()->includeImagesInEnvsView();
    include_dxf=StatesManager::instance()->includeDxfInEnvsView();

    LOG_INFO("Setting zoom for envs image.");

    zoom.fitTo(model->xMin(Model::UserViewEnvs, include_images, include_dxf, include_annotations), model->yMin(Model::UserViewEnvs, include_images, include_dxf, include_annotations), model->xMax(Model::UserViewEnvs, include_images, include_dxf, include_annotations), model->yMax(Model::UserViewEnvs, include_images, include_dxf, include_annotations), 1015-250-3*30, 500, true);

    LOG_INFO("Creating envs image.");

    _environments=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), include_images, include_dxf, include_annotations);

    LOG_INFO("Getting number of nodes.");

    _nodes=double(model->numberOfNodes());

    LOG_INFO("Getting flux variation.");

    _fluxVariation=model->fluxRelativeVariation();

    LOG_INFO("Getting 1d flux.");

    _1DFlux=model->flux1d();

    LOG_INFO("Getting 2d flux.");

    _2DFlux=model->flux2d();

    LOG_INFO("Getting flux sum.");

    _ratio=model->fluxSum();

    LOG_INFO("Getting lambda value.");

    _lambda=model->equivalentThermalConductivity();

    LOG_INFO("Getting has condensation.");

    _hasCondensation = model->getCondensation();

    LOG_INFO("Setting psi value.");

    if (!model->psiCoefficientComputed())
        _psi="-";
    else if (model->computationType()==Model::ThermalBridge)
        _psi=locale.toString(model->psiCoefficient(), 'f', 3);
    else if (model->computationType()==Model::ThermalTransmission)
        _psi=locale.toString(model->transmissionCoefficient(), 'f', 3);

    LOG_INFO("Getting bcs.");

    _bc=viewer.getBCSuperficialTemperatures();

    LOG_INFO("Getting envs.");

    _environmentsResults=model->environmentResults();

    // Update page number.

    LOG_INFO("Update page number.");

    unsigned int n_page=2;
    if (!_environmentsResults.empty())
        n_page=3;
    
    if (model && model->computationType() == Model::GlaserCondensation)
        n_page = 1;

    LOG_INFO("Nb of page(s): " << n_page);
    
    _height=1485*n_page;

    LOG_INFO("Resizing report.");

    resize(1015, _height);

    LOG_INFO("End of call.");
}

std::string ReportContent::imageToPngStream(const QImage &image) const
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return QString(ba.toBase64()).toStdString();
}

void ReportContent::paintEvent(QPaintEvent *event)
{
    LOG_INFO("paintEvent() requested.");

    QPainter qp(this);
    
    Model *model = StatesManager::instance()->currentModel();
    
    // Initialize font size.
    QFont f=qp.font();
    f.setPixelSize(12);
    qp.setFont(f);

    drawReport(&qp, 1015);

    if (!_environmentsResults.empty())
    {
        drawReport_page2(&qp, 1015, _height/3.0);
        drawReport_page3(&qp, 1015, 2.0*_height/3.0);
    }
    else if (model && model->computationType() != Model::GlaserCondensation)
        drawReport_page2(&qp, 1015, _height/2.0);
}

void ReportContent::onPdfExport()
{
	Model *model = StatesManager::instance()->currentModel();
	if (!model)
		return;

	std::string modelFilename = StatesManager::instance()->modelFilename();

    QFileDialog fileDialog(this, _tr("Exporter un rapport PDF"));
    fileDialog.setNameFilter(_tr("document PDF (*.pdf)"));
    fileDialog.setDefaultSuffix("pdf");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);

	// Default name & location.
	if (modelFilename != "")
	{
		QFileInfo info(modelFilename.c_str());
		fileDialog.setDirectory(info.absolutePath());
		fileDialog.selectFile(info.fileName().replace("." + info.suffix(), ".pdf"));
	}

    if(!fileDialog.exec())
        return;

    QString filename=fileDialog.selectedFiles().first();
    if (filename.isEmpty())
        return;
    
    QPdfWriter pdfWriter(filename);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageOrientation(QPageLayout::Portrait);
    pdfWriter.setResolution(150);

    pdfWriter.setTitle(tr("Report"));
    pdfWriter.setCreator("conducte\u00F6");
#ifdef _WIN32
    pdfWriter.setAuthor("conducte\u00F6");
#endif
    QPainter painter(&pdfWriter);

    QFont f=font();
    f.setPixelSize(12);
    painter.setFont(f);
    drawReport(&painter, pdfWriter.width());
    
    if (model && model->computationType() != Model::GlaserCondensation)
    {
        pdfWriter.newPage();
        drawReport_page2(&painter, pdfWriter.width());
    }

    if (!_environmentsResults.empty())
    {
        pdfWriter.newPage();
        drawReport_page3(&painter, pdfWriter.width());
    }

    painter.end();


    QDesktopServices::openUrl(QUrl("file:///"+filename));
}

void ReportContent::drawReport(QPainter *painter, int width)
{
    LOG_INFO("Draw report (page 1).");

    // Create report.
    QLocale locale;

    QTextOption o;
    o.setAlignment(Qt::AlignVCenter);

    QTextOption centered_text;
    centered_text.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QTextOption justified_text;
    justified_text.setAlignment(Qt::AlignJustify);

    // Fonts.
    QFont f=painter->font();
    QFont title_2_font=f;
    title_2_font.setBold(true);
    title_2_font.setUnderline(true);
    title_2_font.setPixelSize(title_2_font.pixelSize()+3);

    QFont title_1_font=f;
    title_1_font.setBold(true);
    title_1_font.setUnderline(true);
    title_1_font.setPixelSize(title_1_font.pixelSize()+10);

    QFont big_font=f;
    big_font.setPixelSize(big_font.pixelSize()+3);

    QFont red_big_font=big_font;
    red_big_font.setBold(true);

    // Create empty image.
    LOG_INFO("Creating new page.");

    QBrush white_brush(Qt::white);
    QPen pen(Qt::white);
    painter->setBrush(white_brush);
    painter->setPen(pen);
    painter->drawRect(0, 0, width, _height);
    
    // Software logo.
    LOG_INFO("Adding logo.");
    QImage logo(":/images/logo.png");
    painter->drawImage(width - _margins - logo.width(), _margins, logo);

    // Report title & subtitle.
    LOG_INFO("Adding title & subtitle.");
    painter->setPen(Qt::black);
    painter->setFont(title_1_font);
    QRect title_rec(_margins, _margins, width-2*_margins, 40);
    painter->drawText(title_rec, _tr("Note de calcul"), centered_text);
    
    QString subtitle = _tr("pont thermique linéique");
    
    Model *model = StatesManager::instance()->currentModel();
    if (model && model->computationType() == Model::Flux)
        subtitle = _tr("flux de chaleur");
    else if (model && model->computationType() == Model::ThermalTransmission)
        subtitle = _tr("transmission thermique");
    else if (model && model->computationType() == Model::EquivalentThermalConductivity)
        subtitle = _tr("conductivité thermique équivalente");
    else if (model && model->computationType() == Model::GlaserCondensation)
        subtitle = _tr("risques de condensation");

    QRect subtitle_rec(_margins, _margins+title_rec.height(), width-2*_margins, 40);
    painter->drawText(subtitle_rec, subtitle, centered_text);

    // Report owner.
    LOG_INFO("Adding owner.");

    painter->setFont(big_font);
    QRect owner_rec(_margins, subtitle_rec.y()+subtitle_rec.height()+30, width-2*_margins, 20);
    painter->drawText(owner_rec, _tr("Note de calcul réalisée par : "), o);

    // Date.
    LOG_INFO("Adding date.");

    QRect date_rec(_margins+185, owner_rec.y()+25, width-2*_margins, 50);
    painter->drawText(date_rec, _tr("le ")+UiTools::getCurrentDate()+_tr(" - conducteö version ")+UiTools::getSoftwareVersion(), justified_text);

    // Software 10211 standart compliance.
    LOG_INFO("Adding compliance.");

    painter->setFont(f);
    QRect compliance_rec(_margins, owner_rec.y()+owner_rec.height()+40, width-2*_margins, 50);
    painter->drawText(compliance_rec, _tr("Le logiciel conducteö est conforme à la norme EN 10211 et aux tests de validation de cette même norme et est ainsi classé comme méthode bidimensionnelle en régime permanent de haute précision. Le logiciel conducteö est également conforme aux hypothèses complémentaires de la RT 2005, RT 2012 et RE 2020. Le logiciel conducteö est conforme à la norme 10077-2 et aux tests de validation de cette même norme et peut ainsi être utilisé à des fins de calcul concernant les menuiseries."), justified_text);

    // Create legends.
    LOG_INFO("Adding legends.");

    QRect material_legend_title(_margins, _header_height, _legend_width, 20);
    painter->setFont(title_2_font);
    painter->drawText(material_legend_title, _tr("Matériaux"), o);
    painter->drawImage(_margins, _header_height+material_legend_title.height(), _materialLegend);

    QRect bc_legend_title(_margins, _header_height+material_legend_title.height()+_materialLegend.height()+_margins, _legend_width, 20);
    painter->drawText(bc_legend_title, _tr("Conditions aux limites"), o);
    painter->drawImage(_margins, _header_height+material_legend_title.height()+_materialLegend.height()+_margins+bc_legend_title.height(), _boundaryConditionLegend);

    // Create 2D model view.
    LOG_INFO("Adding 2d view.");

    QSize size(width-_boundaryConditionLegend.width()-3*_margins, _view_height);
    painter->drawImage(2*_margins+_boundaryConditionLegend.width(), _header_height, _model);
    
    if (model && (model->computationType() == Model::ThermalBridge || model->computationType() == Model::ThermalTransmission))
    {
        // Create 1D model view.
        painter->drawImage(2*_margins+_boundaryConditionLegend.width(), _header_height+_model.height()+10, _1Dmodel);

        // 1D model title.
        QString text1D=_tr("Modélisation sans pont thermique");
        QRect rect_1D_title(_margins, _header_height+_model.height(), _legend_width, _1Dmodel.height());
        painter->drawText(rect_1D_title, text1D, o);
    }
    
    else if (model && (model->computationType() == Model::GlaserCondensation))
    {
        // Create 1D model view.
        painter->drawImage(2*_margins+_boundaryConditionLegend.width(), _header_height+_model.height()+10, _condensation);

        // 1D model title.
        QString text1D=_tr("Zones de condensation");
        QRect rect_1D_title(_margins, _header_height+_model.height(), _legend_width, _condensation.height());
        painter->drawText(rect_1D_title, text1D, o);
    }

    // Convergency informations.
    LOG_INFO("Adding convergency.");

    painter->setFont(title_2_font);
    QRect convergency_rec(_margins, _header_height+_model.height()+_1Dmodel.height()+_condensation.height()+30, width-2*_margins, 25);
    painter->drawText(convergency_rec, _tr("Convergence de la simulation"), o);

    painter->setFont(big_font);
    QRect convergency_rec_1(_margins, convergency_rec.y()+convergency_rec.height()+20, width-2*_margins, 20);
    painter->drawText(convergency_rec_1, _tr("Nombre de noeuds : ")+QString::number(_nodes), o);

    QRect convergency_rec_2(_margins, convergency_rec_1.y()+convergency_rec_1.height(), width-2*_margins, 20);
    painter->drawText(convergency_rec_2, _tr("Variation relative des flux : ")+locale.toString(_fluxVariation, 'f', 4)+" %", o);

    QRect convergency_rec_3(_margins, convergency_rec_2.y()+convergency_rec_2.height(), width-2*_margins, 20);
    painter->drawText(convergency_rec_3, _tr("Somme des flux / Flux total : ")+locale.toString(_ratio, 'f', 6), o);

    // Fluxes & psi coefficient outputs.
    LOG_INFO("Adding fluxes & coefficients outputs.");

    painter->setFont(title_2_font);
    QRect results_rec(width/2, _header_height+_model.height()+_1Dmodel.height()+_condensation.height()+30, width/2-_margins, 25);

    if (model)
    {
        if (model->computationType()==Model::ThermalBridge)
            painter->drawText(results_rec, _tr("Coefficient ψ"), o);
        else if (model->computationType()==Model::Flux)
            painter->drawText(results_rec, _tr("Flux de chaleur"), o);
        else if (model->computationType()==Model::ThermalTransmission)
            painter->drawText(results_rec, _tr("Coefficient U"), o);
        else if (model->computationType()==Model::EquivalentThermalConductivity)
            painter->drawText(results_rec, _tr("Conductivité équivalente"), o);
        else if (model->computationType()==Model::GlaserCondensation)
            painter->drawText(results_rec, _tr("Condensation"), o);
    }

    painter->setFont(big_font);
    QRect results_rec_1(width/2, results_rec.y()+results_rec.height()+20, width/2-_margins, 20);
    if (model && model->computationType()!=Model::Flux && model->computationType()!=Model::EquivalentThermalConductivity)
        painter->drawText(results_rec_1, _tr("Flux 2D : ")+locale.toString(_2DFlux, 'f', 3)+" W/m", o);
    else if(model && model->computationType() == Model::Flux)
        painter->drawText(results_rec_1, _tr("Flux : ")+locale.toString(_2DFlux, 'f', 3)+" W/m", o);
    else if(model && model->computationType() == Model::EquivalentThermalConductivity)
        painter->drawText(results_rec_1, _tr("λ équivalent : ")+locale.toString(_lambda, 'f', 3)+" W/(m.°C)", o);

    int dy=results_rec_1.y()+results_rec_1.height();
    if (model && model->computationType()!=Model::Flux && model->computationType() != Model::EquivalentThermalConductivity && model->computationType() != Model::GlaserCondensation)
    {
        QRect results_rec_2(width/2, results_rec_1.y()+results_rec_1.height(), width/2-_margins, 20);
        dy=results_rec_2.y()+results_rec_2.height();
        painter->drawText(results_rec_2, _tr("Flux 1D : ")+locale.toString(_1DFlux, 'f', 3)+" W/m", o);
    }
    
    else if (model && model->computationType() == Model::GlaserCondensation)
    {
        QRect results_rec_2(width/2, results_rec_1.y()+results_rec_1.height(), width/2-_margins, 20);
        dy = results_rec_2.y()+results_rec_2.height();
        
        QString value = _tr("Non");
        if (model->getCondensation())
            value = _tr("Oui");
        
        painter->drawText(results_rec_2, _tr("Condensation :") + " " + value, o);
    }

    painter->setFont(red_big_font);
    painter->setPen(Qt::red);
    QRect results_rec_3(width/2, dy, width/2-_margins, 20);
    if (model)
    {
        if (model->computationType()==Model::ThermalBridge)
            painter->drawText(results_rec_3, _tr("Coefficient ψ : ")+_psi+" W/(m.°C)", o);
        else if (model->computationType()==Model::ThermalTransmission)
            painter->drawText(results_rec_3, _tr("Coefficient U : ")+_psi+" W/(m².°C)", o);
    }

    // Reset default fonts.
    painter->setFont(f);
    painter->setPen(Qt::black);
}

void ReportContent::drawReport_page2(QPainter *painter, int width, int offset)
{
    LOG_INFO("Adding page 2.");

    // Create page 2.
    QLocale locale;

    QTextOption o;
    o.setAlignment(Qt::AlignVCenter);

    QTextOption centered_text;
    centered_text.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QTextOption justified_text;
    justified_text.setAlignment(Qt::AlignJustify);

    // Fonts.
    QFont f=painter->font();
    painter->setFont(f);

    QFont big_font=f;
    big_font.setPixelSize(big_font.pixelSize() + 3);

    QFont bold_big_font=big_font;
    bold_big_font.setBold(true);

    // Draw results view.
    QSize size(width-_boundaryConditionLegend.width()-3*_margins, _view_height);
    painter->drawImage(2*_margins+_boundaryConditionLegend.width(), _header_height/3+offset, _results);

    int w=40;
    int h=30;
    int m=30;
    int legend_with=250;
    int title_offset=_header_height/3+offset+_view_height+50;
    int array_offset=title_offset+50;
    int left=m+w+10+legend_with+10;

    // Array titles.
    painter->setFont(bold_big_font);

    QRect min_rect_title(left, title_offset, (width-left-m)/4, 2*h);
    painter->drawText(min_rect_title, _tr("Température\nminimale"), centered_text);

    QRect max_rect_title(left+(width-left-m)/4, title_offset, (width-left-m)/4, 2*h);
    painter->drawText(max_rect_title, _tr("Température\nmaximale"), centered_text);

    QRect minf_rect_title(left+2*(width-left-m)/4, title_offset, (width-left-m)/4, 2*h);
    painter->drawText(minf_rect_title, _tr("Facteur de\ntempérature\nminimale"), centered_text);

    QRect maxf_rect_title(left+3*(width-left-m)/4, title_offset, (width-left-m)/4, 2*h);
    painter->drawText(maxf_rect_title, _tr("Facteur de\ntempérature\nmaximale"), centered_text);

    // Draw boundary conditions data.
    for (unsigned int i=0 ; i<_bc.size() ; i++)
    {
        painter->setFont(f);

        double R=_bc.at(i)._resistance;
        double T=_bc.at(i)._temperature;

        QColor color(_bc.at(i)._color);
        QString name=_bc.at(i)._name;

        QString data="R="+locale.toString(R, 'f', 2)+" (m².°C)/W - T="
                      +locale.toString(T, 'f', 2)+" °C";

        QRect rect(m, array_offset+m+i*(h+m), w, h);

        QPen pen;
        pen.setColor(color.darker());
        painter->setPen(pen);
        painter->setBrush(QBrush(color));

        painter->drawRect(rect);

        QRect title_rect(m+w+10, array_offset+m+i*(h+m), legend_with, h/2);
        painter->setPen(Qt::black);
        QTextOption o;
        o.setAlignment(Qt::AlignVCenter);

        // Elide text if needed.
        QFontMetrics metric(painter->font());
        name=metric.elidedText(name, Qt::ElideRight, title_rect.width());

        painter->drawText(title_rect, name, o);

        QRect data_rect(m+w+10, array_offset+m+i*(h+m)+h/2, legend_with, h/2);
        painter->drawText(data_rect, data, o);

        painter->setFont(big_font);

        // Minimal temperature.
        T=_bc.at(i)._tmin;
        data=locale.toString(T, 'f', 2)+" °C";

        QRect min_rect(left, array_offset+m+i*(h+m), (width-left-m)/4, h);
        painter->drawText(min_rect, data, centered_text);

        // Maximal temperature.
        T=_bc.at(i)._tmax;
        data=locale.toString(T, 'f', 2)+" °C";

        QRect max_rect(left+(width-left-m)/4, array_offset+m+i*(h+m), (width-left-m)/4, h);
        painter->drawText(max_rect, data, centered_text);

        // Minimal temperature factor.
        data="-";
        if (_bc.at(i)._useFactor)
            data=locale.toString(_bc.at(i)._minFactor, 'f', 4);

        QRect minf_rect(left+2*(width-left-m)/4, array_offset+m+i*(h+m), (width-left-m)/4, h);
        painter->drawText(minf_rect, data, centered_text);

        // Maximal temperature factor.
        data="-";
        if (_bc.at(i)._useFactor)
            data=locale.toString(_bc.at(i)._maxFactor, 'f', 4);

        QRect maxf_rect(left+3*(width-left-m)/4, array_offset+m+i*(h+m), (width-left-m)/4, h);
        painter->drawText(maxf_rect, data, centered_text);
    } 
}

void ReportContent::drawReport_page3(QPainter *painter, int width, int offset)
{
    LOG_INFO("Adding page 3.");

    // Create page 3.
    QLocale locale;

    QTextOption o;
    o.setAlignment(Qt::AlignVCenter);

    QTextOption centered_text;
    centered_text.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QTextOption justified_text;
    justified_text.setAlignment(Qt::AlignJustify);

    // Fonts.
    QFont f=painter->font();
    painter->setFont(f);

    QFont big_font=f;
    big_font.setPixelSize(big_font.pixelSize() + 3);

    QFont bold_big_font=big_font;
    bold_big_font.setBold(true);

    QFont small_font=f;
    small_font.setPixelSize(f.pixelSize() - 3);

    // Environments legend.
    QRect env_legend_title(_margins, _header_height+_margins+offset, _legend_width, 20);
    painter->drawText(env_legend_title, _tr("Ambiances thermiques"), o);
    painter->drawImage(_margins, _header_height+env_legend_title.height()+_margins+offset, _environmentsLegend);

    // Draw environments view.
    QSize size(width-_environmentsLegend.width()-3*_margins, _view_height);
    painter->drawImage(2*_margins+_environmentsLegend.width(), _header_height/3+offset, _environments);

    // Results.
    painter->setFont(bold_big_font);
    QRect env_results_title(_margins, _header_height/3+offset+size.height(), width, 25);
    painter->drawText(env_results_title, _tr("Répartition du pont thermique par ambiance thermique"), o);

    for (unsigned int i=0 ; i<_environmentsResults.size() ; i++)
    {
        EnvironmentResult &r=_environmentsResults.at(i);

        painter->setFont(f);
        QRect entry_rect(2*_margins, _header_height/3+offset+size.height()+env_results_title.height()+(i+1)*30, width, 30);
        QString title="ψ   : "+locale.toString(r._couplingCoefficient, 'f', 3)+" W/(m.°C)";
        painter->drawText(entry_rect, title, o);

        painter->setFont(small_font);
        QRect entry_rect_indice(2*_margins+10, 17+_header_height/3+offset+size.height()+env_results_title.height()+(i+1)*30, width, 20);
        QString indice=QString(r._environment1Id.c_str())+QString(r._environment2Id.c_str());
        painter->drawText(entry_rect_indice, indice, o);
    }
    
    int n = 0;
    int y_offset = _header_height/3+offset+size.height()+env_results_title.height()+(_environmentsResults.size())*30;
    
    for (int i=1 ; i<=4; i++)
    {
        std::string id = ToolBox::intToString(i);

        if (!ToolBox::useEnvironment(_environmentsResults, id))
            continue;

        double value = ToolBox::globalCouplingCoefficient(_environmentsResults, id);

        painter->setFont(f);
        QRect entry_rect(2*_margins, y_offset + (n + 1) * 30, width, 30);
        QString title="ψ   : "+locale.toString(value, 'f', 3)+" W/(m.°C)";
        painter->drawText(entry_rect, title, o);

        painter->setFont(small_font);
        QRect entry_rect_indice(2*_margins+10, 17 + y_offset + (n + 1) * 30, width, 20);
        QString indice=QString(id.c_str());
        painter->drawText(entry_rect_indice, indice, o);

        n++;
    }
}

void ReportContent::setMaterialLegend(const QImage &image)
{
    _materialLegend=image;
}

void ReportContent::setBoundaryConditionLegend(const QImage &image)
{
    _boundaryConditionLegend=image;
}

void ReportContent::setEnvironmentsLegend(const QImage &image)
{
    _environmentsLegend=image;
}

void ReportContent::setModel(const QImage &image)
{
    _model=image;
}

void ReportContent::set1DModel(const QImage &image)
{
    _1Dmodel=image;
}

void ReportContent::setResultsView(const QImage &image)
{
    _results=image;
}

void ReportContent::setEnvironmentsView(const QImage &image)
{
    _environments=image;
}

void ReportContent::setNodesNumber(const double nodes)
{
    _nodes=nodes;
}

void ReportContent::setFluxVariation(const double fluxVariation)
{
    _fluxVariation=fluxVariation;
}

void ReportContent::set1DFlux(const double flux)
{
    _1DFlux=flux;
}

void ReportContent::set2DFlux(const double flux)
{
    _2DFlux=flux;
}

void ReportContent::setRatio(const double ratio)
{
    _ratio=ratio;
}

void ReportContent::setPsiCoefficient(const QString &psi)
{
    _psi=psi;
}

void ReportContent::setBCTemperatures(const std::vector<BoundaryConditionData> &bc)
{
    _bc=bc;
}

std::string ReportContent::imageData(const QImage &image) const
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG", 200);
    return std::string(ba.constData(), ba.size());
}

DocxImage *ReportContent::qimageToDocxImage(const QImage &image, const std::string &name) const
{
    DocxImage *im=new DocxImage(name, imageData(image), image.size().width(), image.size().height());
    return im;
}

std::vector<DocxImage*> ReportContent::getImageContents() const
{
    std::vector<DocxImage*> images;
    images.push_back(qimageToDocxImage(_materialLegend, "conducteo_material_legend"));
    images.push_back(qimageToDocxImage(_boundaryConditionLegend, "conducteo_bc_legend"));
    images.push_back(qimageToDocxImage(_model, "conducteo_model"));
    images.push_back(qimageToDocxImage(_condensation, "conducteo_condensation"));
    images.push_back(qimageToDocxImage(_rh, "conducteo_humidity"));
    images.push_back(qimageToDocxImage(_1Dmodel, "conducteo_1d_model"));
    images.push_back(qimageToDocxImage(_results, "conducteo_results_view"));
    images.push_back(qimageToDocxImage(_environments, "conducteo_environments"));
    images.push_back(qimageToDocxImage(_environmentsLegend, "conducteo_environments_legend"));

    // Create images with export options.
    ModelViewer viewer;
    Zoom zoom;
    viewer.setCustomZoom(&zoom);

    Model *model=StatesManager::instance()->currentModel();
    if (model)
    {
        // 2d model.
        zoom.fitTo(model->xMin(Model::UserView2dModel, false, false, false), model->yMin(Model::UserView2dModel, false, false, false), model->xMax(Model::UserView2dModel, false, false, false), model->yMax(Model::UserView2dModel, false, false, false), 1015-250-3*30, 500, true);
        _model_0=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), false, false, false);
        images.push_back(qimageToDocxImage(_model_0, "conducteo_model_0"));

        zoom.fitTo(model->xMin(Model::UserView2dModel, true, false, false), model->yMin(Model::UserView2dModel, true, false, false), model->xMax(Model::UserView2dModel, true, false, false), model->yMax(Model::UserView2dModel, true, false, false), 1015-250-3*30, 500, true);
        _model_1=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), true, false, false);
        images.push_back(qimageToDocxImage(_model_1, "conducteo_model_1"));

        zoom.fitTo(model->xMin(Model::UserView2dModel, false, true, false), model->yMin(Model::UserView2dModel, false, true, false), model->xMax(Model::UserView2dModel, false, true, false), model->yMax(Model::UserView2dModel, false, true, false), 1015-250-3*30, 500, true);
        _model_2=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), false, true, false);
        images.push_back(qimageToDocxImage(_model_2, "conducteo_model_2"));

        zoom.fitTo(model->xMin(Model::UserView2dModel, false, false, true), model->yMin(Model::UserView2dModel, false, false, true), model->xMax(Model::UserView2dModel, false, false, true), model->yMax(Model::UserView2dModel, false, false, true), 1015-250-3*30, 500, true);
        _model_4=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), false, false, true);
        images.push_back(qimageToDocxImage(_model_4, "conducteo_model_4"));

        zoom.fitTo(model->xMin(Model::UserView2dModel, true, true, false), model->yMin(Model::UserView2dModel, true, true, false), model->xMax(Model::UserView2dModel, true, true, false), model->yMax(Model::UserView2dModel, true, true, false), 1015-250-3*30, 500, true);
        _model_3=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), true, true, false);
        images.push_back(qimageToDocxImage(_model_3, "conducteo_model_3"));

        zoom.fitTo(model->xMin(Model::UserView2dModel, true, false, true), model->yMin(Model::UserView2dModel, true, false, true), model->xMax(Model::UserView2dModel, true, false, true), model->yMax(Model::UserView2dModel, true, false, true), 1015-250-3*30, 500, true);
        _model_5=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), true, false, true);
        images.push_back(qimageToDocxImage(_model_5, "conducteo_model_5"));

        zoom.fitTo(model->xMin(Model::UserView2dModel, false, true, true), model->yMin(Model::UserView2dModel, false, true, true), model->xMax(Model::UserView2dModel, false, true, true), model->yMax(Model::UserView2dModel, false, true, true), 1015-250-3*30, 500, true);
        _model_6=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), false, true, true);
        images.push_back(qimageToDocxImage(_model_6, "conducteo_model_6"));

        zoom.fitTo(model->xMin(Model::UserView2dModel, true, true, true), model->yMin(Model::UserView2dModel, true, true, true), model->xMax(Model::UserView2dModel, true, true, true), model->yMax(Model::UserView2dModel, true, true, true), 1015-250-3*30, 500, true);
        _model_7=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), true, true, true);
        images.push_back(qimageToDocxImage(_model_7, "conducteo_model_7"));

        // 1d model.
        zoom.fitTo(model->xMin(Model::UserView1dModel, false, false, false), model->yMin(Model::UserView1dModel, false, false, false), model->xMax(Model::UserView1dModel, false, false, false), model->yMax(Model::UserView1dModel, false, false, false), 1015-250-3*30, 500, true);
        _model_1d_0=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), false, false, false, model);
        images.push_back(qimageToDocxImage(_model_1d_0, "conducteo_1d_model_0"));

        zoom.fitTo(model->xMin(Model::UserView1dModel, true, false, false), model->yMin(Model::UserView1dModel, true, false, false), model->xMax(Model::UserView1dModel, true, false, false), model->yMax(Model::UserView1dModel, true, false, false), 1015-250-3*30, 500, true);
        _model_1d_1=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), true, false, false, model);
        images.push_back(qimageToDocxImage(_model_1d_1, "conducteo_1d_model_1"));

        zoom.fitTo(model->xMin(Model::UserView1dModel, false, true, false), model->yMin(Model::UserView1dModel, false, true, false), model->xMax(Model::UserView1dModel, false, true, false), model->yMax(Model::UserView1dModel, false, true, false), 1015-250-3*30, 500, true);
        _model_1d_2=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), false, true, false, model);
        images.push_back(qimageToDocxImage(_model_1d_2, "conducteo_1d_model_2"));

        zoom.fitTo(model->xMin(Model::UserView1dModel, false, false, true), model->yMin(Model::UserView1dModel, false, false, true), model->xMax(Model::UserView1dModel, false, false, true), model->yMax(Model::UserView1dModel, false, false, true), 1015-250-3*30, 500, true);
        _model_1d_4=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), false, false, true, model);
        images.push_back(qimageToDocxImage(_model_1d_4, "conducteo_1d_model_4"));

        zoom.fitTo(model->xMin(Model::UserView1dModel, true, true, false), model->yMin(Model::UserView1dModel, true, true, false), model->xMax(Model::UserView1dModel, true, true, false), model->yMax(Model::UserView1dModel, true, true, false), 1015-250-3*30, 500, true);
        _model_1d_3=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), true, true, false, model);
        images.push_back(qimageToDocxImage(_model_1d_3, "conducteo_1d_model_3"));

        zoom.fitTo(model->xMin(Model::UserView1dModel, true, false, true), model->yMin(Model::UserView1dModel, true, false, true), model->xMax(Model::UserView1dModel, true, false, true), model->yMax(Model::UserView1dModel, true, false, true), 1015-250-3*30, 500, true);
        _model_1d_5=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), true, false, true, model);
        images.push_back(qimageToDocxImage(_model_1d_5, "conducteo_1d_model_5"));

        zoom.fitTo(model->xMin(Model::UserView1dModel, false, true, true), model->yMin(Model::UserView1dModel, false, true, true), model->xMax(Model::UserView1dModel, false, true, true), model->yMax(Model::UserView1dModel, false, true, true), 1015-250-3*30, 500, true);
        _model_1d_6=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), false, true, true, model);
        images.push_back(qimageToDocxImage(_model_1d_6, "conducteo_1d_model_6"));

        zoom.fitTo(model->xMin(Model::UserView1dModel, true, true, true), model->yMin(Model::UserView1dModel, true, true, true), model->xMax(Model::UserView1dModel, true, true, true), model->yMax(Model::UserView1dModel, true, true, true), 1015-250-3*30, 500, true);
        _model_1d_7=viewer.toImage(model->model1d(), 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), true, true, true, model);
        images.push_back(qimageToDocxImage(_model_1d_7, "conducteo_1d_model_7"));

        // results.
        zoom.fitTo(model->xMin(Model::UserViewResults, false, false, false), model->yMin(Model::UserViewResults, false, false, false), model->xMax(Model::UserViewResults, false, false, false), model->yMax(Model::UserViewResults, false, false, false), 1015-250-3*30, 500, true);
        _results_0=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), false, false, false);
        images.push_back(qimageToDocxImage(_results_0, "conducteo_results_view_0"));

        zoom.fitTo(model->xMin(Model::UserViewResults, true, false, false), model->yMin(Model::UserViewResults, true, false, false), model->xMax(Model::UserViewResults, true, false, false), model->yMax(Model::UserViewResults, true, false, false), 1015-250-3*30, 500, true);
        _results_1=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), true, false, false);
        images.push_back(qimageToDocxImage(_results_1, "conducteo_results_view_1"));

        zoom.fitTo(model->xMin(Model::UserViewResults, false, true, false), model->yMin(Model::UserViewResults, false, true, false), model->xMax(Model::UserViewResults, false, true, false), model->yMax(Model::UserViewResults, false, true, false), 1015-250-3*30, 500, true);
        _results_2=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), false, true, false);
        images.push_back(qimageToDocxImage(_results_2, "conducteo_results_view_2"));

        zoom.fitTo(model->xMin(Model::UserViewResults, false, false, true), model->yMin(Model::UserViewResults, false, false, true), model->xMax(Model::UserViewResults, false, false, true), model->yMax(Model::UserViewResults, false, false, true), 1015-250-3*30, 500, true);
        _results_4=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), false, false, true);
        images.push_back(qimageToDocxImage(_results_4, "conducteo_results_view_4"));

        zoom.fitTo(model->xMin(Model::UserViewResults, true, true, false), model->yMin(Model::UserViewResults, true, true, false), model->xMax(Model::UserViewResults, true, true, false), model->yMax(Model::UserViewResults, true, true, false), 1015-250-3*30, 500, true);
        _results_3=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), true, true, false);
        images.push_back(qimageToDocxImage(_results_3, "conducteo_results_view_3"));

        zoom.fitTo(model->xMin(Model::UserViewResults, true, false, true), model->yMin(Model::UserViewResults, true, false, true), model->xMax(Model::UserViewResults, true, false, true), model->yMax(Model::UserViewResults, true, false, true), 1015-250-3*30, 500, true);
        _results_5=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), true, false, true);
        images.push_back(qimageToDocxImage(_results_5, "conducteo_results_view_5"));

        zoom.fitTo(model->xMin(Model::UserViewResults, false, true, true), model->yMin(Model::UserViewResults, false, true, true), model->xMax(Model::UserViewResults, false, true, true), model->yMax(Model::UserViewResults, false, true, true), 1015-250-3*30, 500, true);
        _results_6=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), false, true, true);
        images.push_back(qimageToDocxImage(_results_6, "conducteo_results_view_6"));

        zoom.fitTo(model->xMin(Model::UserViewResults, true, true, true), model->yMin(Model::UserViewResults, true, true, true), model->xMax(Model::UserViewResults, true, true, true), model->yMax(Model::UserViewResults, true, true, true), 1015-250-3*30, 500, true);
        _results_7=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), true, true, true);
        images.push_back(qimageToDocxImage(_results_7, "conducteo_results_view_7"));

        // environments.
        zoom.fitTo(model->xMin(Model::UserViewEnvs, false, false, false), model->yMin(Model::UserViewEnvs, false, false, false), model->xMax(Model::UserViewEnvs, false, false, false), model->yMax(Model::UserViewEnvs, false, false, false), 1015-250-3*30, 500, true);
        _envs_0=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), false, false, false);
        images.push_back(qimageToDocxImage(_envs_0, "conducteo_environments_0"));

        zoom.fitTo(model->xMin(Model::UserViewEnvs, true, false, false), model->yMin(Model::UserViewEnvs, true, false, false), model->xMax(Model::UserViewEnvs, true, false, false), model->yMax(Model::UserViewEnvs, true, false, false), 1015-250-3*30, 500, true);
        _envs_1=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), true, false, false);
        images.push_back(qimageToDocxImage(_envs_1, "conducteo_environments_1"));

        zoom.fitTo(model->xMin(Model::UserViewEnvs, false, true, false), model->yMin(Model::UserViewEnvs, false, true, false), model->xMax(Model::UserViewEnvs, false, true, false), model->yMax(Model::UserViewEnvs, false, true, false), 1015-250-3*30, 500, true);
        _envs_2=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), false, true, false);
        images.push_back(qimageToDocxImage(_envs_2, "conducteo_environments_2"));

        zoom.fitTo(model->xMin(Model::UserViewEnvs, false, false, true), model->yMin(Model::UserViewEnvs, false, false, true), model->xMax(Model::UserViewEnvs, false, false, true), model->yMax(Model::UserViewEnvs, false, false, true), 1015-250-3*30, 500, true);
        _envs_4=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), false, false, true);
        images.push_back(qimageToDocxImage(_envs_4, "conducteo_environments_4"));

        zoom.fitTo(model->xMin(Model::UserViewEnvs, true, true, false), model->yMin(Model::UserViewEnvs, true, true, false), model->xMax(Model::UserViewEnvs, true, true, false), model->yMax(Model::UserViewEnvs, true, true, false), 1015-250-3*30, 500, true);
        _envs_3=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), true, true, false);
        images.push_back(qimageToDocxImage(_envs_3, "conducteo_environments_3"));

        zoom.fitTo(model->xMin(Model::UserViewEnvs, true, false, true), model->yMin(Model::UserViewEnvs, true, false, true), model->xMax(Model::UserViewEnvs, true, false, true), model->yMax(Model::UserViewEnvs, true, false, true), 1015-250-3*30, 500, true);
        _envs_5=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), true, false, true);
        images.push_back(qimageToDocxImage(_envs_5, "conducteo_environments_5"));

        zoom.fitTo(model->xMin(Model::UserViewEnvs, false, true, true), model->yMin(Model::UserViewEnvs, false, true, true), model->xMax(Model::UserViewEnvs, false, true, true), model->yMax(Model::UserViewEnvs, false, true, true), 1015-250-3*30, 500, true);
        _envs_6=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), false, true, true);
        images.push_back(qimageToDocxImage(_envs_6, "conducteo_environments_6"));

        zoom.fitTo(model->xMin(Model::UserViewEnvs, true, true, true), model->yMin(Model::UserViewEnvs, true, true, true), model->xMax(Model::UserViewEnvs, true, true, true), model->yMax(Model::UserViewEnvs, true, true, true), 1015-250-3*30, 500, true);
        _envs_7=viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), true, true, true);
        images.push_back(qimageToDocxImage(_envs_7, "conducteo_environments_7"));

        // Condensation.
        if (model->computationType() == Model::GlaserCondensation)
        {
            zoom.fitTo(model->xMin(Model::UserViewCondensation, false, false, false), model->yMin(Model::UserViewCondensation, false, false, false), model->xMax(Model::UserViewCondensation, false, false, false), model->yMax(Model::UserViewCondensation, false, false, false), 1015-250-3*30, 500, true);
            _condensation_0 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), false, false, false);
            images.push_back(qimageToDocxImage(_condensation_0, "conducteo_condensation_0"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, true, false, false), model->yMin(Model::UserViewCondensation, true, false, false), model->xMax(Model::UserViewCondensation, true, false, false), model->yMax(Model::UserViewCondensation, true, false, false), 1015-250-3*30, 500, true);
            _condensation_1 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), true, false, false);
            images.push_back(qimageToDocxImage(_condensation_1, "conducteo_condensation_1"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, false, true, false), model->yMin(Model::UserViewCondensation, false, true, false), model->xMax(Model::UserViewCondensation, false, true, false), model->yMax(Model::UserViewCondensation, false, true, false), 1015-250-3*30, 500, true);
            _condensation_2 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), false, true, false);
            images.push_back(qimageToDocxImage(_condensation_2, "conducteo_condensation_2"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, false, false, true), model->yMin(Model::UserViewCondensation, false, false, true), model->xMax(Model::UserViewCondensation, false, false, true), model->yMax(Model::UserViewCondensation, false, false, true), 1015-250-3*30, 500, true);
            _condensation_3 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), false, false, true);
            images.push_back(qimageToDocxImage(_condensation_3, "conducteo_condensation_3"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, true, true, false), model->yMin(Model::UserViewCondensation, true, true, false), model->xMax(Model::UserViewCondensation, true, true, false), model->yMax(Model::UserViewCondensation, true, true, false), 1015-250-3*30, 500, true);
            _condensation_4 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), true, true, false);
            images.push_back(qimageToDocxImage(_condensation_4, "conducteo_condensation_4"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, true, false, true), model->yMin(Model::UserViewCondensation, true, false, true), model->xMax(Model::UserViewCondensation, true, false, true), model->yMax(Model::UserViewCondensation, true, false, true), 1015-250-3*30, 500, true);
            _condensation_5 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), true, false, true);
            images.push_back(qimageToDocxImage(_condensation_5, "conducteo_condensation_5"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, false, true, true), model->yMin(Model::UserViewCondensation, false, true, true), model->xMax(Model::UserViewCondensation, false, true, true), model->yMax(Model::UserViewCondensation, false, true, true), 1015-250-3*30, 500, true);
            _condensation_6 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), false, true, true);
            images.push_back(qimageToDocxImage(_condensation_6, "conducteo_condensation_6"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, true, true, true), model->yMin(Model::UserViewCondensation, true, true, true), model->xMax(Model::UserViewCondensation, true, true, true), model->yMax(Model::UserViewCondensation, true, true, true), 1015-250-3*30, 500, true);
            _condensation_7 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), true, true, true);
            images.push_back(qimageToDocxImage(_condensation_7, "conducteo_condensation_7"));

            // RH.
            zoom.fitTo(model->xMin(Model::UserViewCondensation, false, false, false), model->yMin(Model::UserViewCondensation, false, false, false), model->xMax(Model::UserViewCondensation, false, false, false), model->yMax(Model::UserViewCondensation, false, false, false), 1015-250-3*30, 500, true);
            _rh_0 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), false, false, false);
            images.push_back(qimageToDocxImage(_rh_0, "conducteo_rh_0"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, true, false, false), model->yMin(Model::UserViewCondensation, true, false, false), model->xMax(Model::UserViewCondensation, true, false, false), model->yMax(Model::UserViewCondensation, true, false, false), 1015-250-3*30, 500, true);
            _rh_1 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), true, false, false);
            images.push_back(qimageToDocxImage(_rh_1, "conducteo_rh_1"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, false, true, false), model->yMin(Model::UserViewCondensation, false, true, false), model->xMax(Model::UserViewCondensation, false, true, false), model->yMax(Model::UserViewCondensation, false, true, false), 1015-250-3*30, 500, true);
            _rh_2 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), false, true, false);
            images.push_back(qimageToDocxImage(_rh_2, "conducteo_rh_2"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, false, false, true), model->yMin(Model::UserViewCondensation, false, false, true), model->xMax(Model::UserViewCondensation, false, false, true), model->yMax(Model::UserViewCondensation, false, false, true), 1015-250-3*30, 500, true);
            _rh_3 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), false, false, true);
            images.push_back(qimageToDocxImage(_rh_3, "conducteo_rh_3"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, true, true, false), model->yMin(Model::UserViewCondensation, true, true, false), model->xMax(Model::UserViewCondensation, true, true, false), model->yMax(Model::UserViewCondensation, true, true, false), 1015-250-3*30, 500, true);
            _rh_4 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), true, true, false);
            images.push_back(qimageToDocxImage(_rh_4, "conducteo_rh_4"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, true, false, true), model->yMin(Model::UserViewCondensation, true, false, true), model->xMax(Model::UserViewCondensation, true, false, true), model->yMax(Model::UserViewCondensation, true, false, true), 1015-250-3*30, 500, true);
            _rh_5 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), true, false, true);
            images.push_back(qimageToDocxImage(_rh_5, "conducteo_rh_5"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, false, true, true), model->yMin(Model::UserViewCondensation, false, true, true), model->xMax(Model::UserViewCondensation, false, true, true), model->yMax(Model::UserViewCondensation, false, true, true), 1015-250-3*30, 500, true);
            _rh_6 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), false, true, true);
            images.push_back(qimageToDocxImage(_rh_6, "conducteo_rh_6"));

            zoom.fitTo(model->xMin(Model::UserViewCondensation, true, true, true), model->yMin(Model::UserViewCondensation, true, true, true), model->xMax(Model::UserViewCondensation, true, true, true), model->yMax(Model::UserViewCondensation, true, true, true), 1015-250-3*30, 500, true);
            _rh_7 = viewer.toImage(model, 1015-250-3*30, 500, (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawCondensationResults), true, true, true);
            images.push_back(qimageToDocxImage(_rh_7, "conducteo_rh_7"));
        }
    }

    for (unsigned int i=0 ; i<_bc.size() ; i++)
        images.push_back(qimageToDocxImage(boundaryConditionImage(_bc.at(i)), "imagebcnb"+ToolBox::intToString(i)));
    return images;
}

QImage ReportContent::boundaryConditionImage(const BoundaryConditionData &bc) const
{
    int w=40;
    int h=30;
    int bc_width=250;
    int bc_height=50;
    int m=(bc_height-h)/2;

    QPainter painter;
    QLocale locale;
    QImage image(bc_width, bc_height, QImage::Format_RGB32);
    painter.begin(&image);

    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawRect(image.rect());
    
    double R=bc._resistance;
    double T=bc._temperature;

    QColor color(bc._color);
    QString name=bc._name;

    QString data="R="+locale.toString(R, 'f', 2)+" (m².°C)/W - T="
                    +locale.toString(T, 'f', 2)+" °C";

    QRect rect(0, m, w, h);

    QPen pen;
    pen.setColor(color.darker());
    painter.setPen(pen);
    painter.setBrush(QBrush(color));

    painter.drawRect(rect);

    QRect title_rect(w+10, m, bc_width, h/2);
    painter.setPen(Qt::black);
    QTextOption o;
    o.setAlignment(Qt::AlignVCenter);

    // Elide text if needed.
    QFontMetrics metric(painter.font());
    name=metric.elidedText(name, Qt::ElideRight, title_rect.width());

    painter.drawText(title_rect, name, o);

    QRect data_rect(w+10, h/2+m, bc_width, h/2);
    painter.drawText(data_rect, data, o);

    painter.end();
    return image;
}

void ReportContent::translate()
{
    statesChanged();
    update();
}

void ReportContent::wheelEvent(QWheelEvent *event)
{
    Model *model = StatesManager::instance()->currentModel();
    if (!model)
        return;

    int offset1=_height/3.0;
    if (_environmentsResults.empty())
        offset1=_height/2.0;
    int offset2=2.0*_height/3.0;

    double x=event->position().x();
    double y=event->position().y();

    ModelViewer viewer;

    // 2d model.
    if (x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_model.width()
        && y>_header_height && y<_header_height+_model.height())
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsIn2dView();
        bool include_images=StatesManager::instance()->includeImagesIn2dView();
        bool include_dxf=StatesManager::instance()->includeDxfIn2dView();
    
        zoom.fitTo(model->xMin(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->yMin(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->xMax(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->yMax(Model::UserView2dModel, include_images, include_dxf, include_annotations), _model.width(), _model.height(), true);

        QPoint p;
        p.setX(event->position().x()-2*_margins-_boundaryConditionLegend.width());
        p.setY(event->position().y()-_header_height);

        if (event->angleDelta().y()>0)
            zoom.increase(p);
        else
            zoom.decrease(p);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_model.width(), _model.height()));
        double ymin=zoom.toPhysicalY(QPoint(_model.width(), _model.height()));
        model->use2dModelUserView(xmin, ymin, xmax, ymax);

        _model=viewer.toImage(model, _model.width(), _model.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), include_images, include_dxf, include_annotations);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    }

    // 1d model.
    else if (model->computationType() != Model::GlaserCondensation && x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_1Dmodel.width()
        && y>_header_height+_model.height()+10 && y<_header_height+_model.height()+10+_1Dmodel.height())
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsIn1dView();
        bool include_images=StatesManager::instance()->includeImagesIn1dView();
        bool include_dxf=StatesManager::instance()->includeDxfIn1dView();
    
        zoom.fitTo(model->xMin(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->yMin(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->xMax(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->yMax(Model::UserView1dModel, include_images, include_dxf, include_annotations), _1Dmodel.width(), _1Dmodel.height(), true);

        QPoint p;
        p.setX(event->position().x()-2*_margins-_boundaryConditionLegend.width());
        p.setY(event->position().y()-_header_height-_model.height());

        if (event->angleDelta().y()>0)
            zoom.increase(p);
        else
            zoom.decrease(p);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_1Dmodel.width(), _1Dmodel.height()));
        double ymin=zoom.toPhysicalY(QPoint(_1Dmodel.width(), _1Dmodel.height()));
        model->use1dModelUserView(xmin, ymin, xmax, ymax);

        _1Dmodel=viewer.toImage(model->model1d(), _1Dmodel.width(), _1Dmodel.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), include_images, include_dxf, include_annotations);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    }

    // Results.
    else if(model->computationType() != Model::GlaserCondensation && x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_results.width()
        && y>_header_height/3+offset1 && y<_header_height/3+offset1+_results.height())
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsInResultsView();
        bool include_images=StatesManager::instance()->includeImagesInResultsView();
        bool include_dxf=StatesManager::instance()->includeDxfInResultsView();
    
        zoom.fitTo(model->xMin(Model::UserViewResults, include_images, include_dxf, include_annotations), model->yMin(Model::UserViewResults, include_images, include_dxf, include_annotations), model->xMax(Model::UserViewResults, include_images, include_dxf, include_annotations), model->yMax(Model::UserViewResults, include_images, include_dxf, include_annotations), _results.width(), _results.height(), true);

        QPoint p;
        p.setX(event->position().x()-2*_margins-_boundaryConditionLegend.width());
        p.setY(event->position().y()-(_header_height/3+offset1));

        if (event->angleDelta().y()>0)
            zoom.increase(p);
        else
            zoom.decrease(p);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_results.width(), _results.height()));
        double ymin=zoom.toPhysicalY(QPoint(_results.width(), _results.height()));
        model->useResultsUserView(xmin, ymin, xmax, ymax);

        _results=viewer.toImage(model, _results.width(), _results.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), include_images, include_dxf, include_annotations, model);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    }

    // Environments.
    else if(model->computationType() != Model::GlaserCondensation && x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_environments.width()
        && y>_header_height/3+offset2 && y<_header_height/3+offset2+_environments.height() && !_environmentsResults.empty())
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsInEnvsView();
        bool include_images=StatesManager::instance()->includeImagesInEnvsView();
        bool include_dxf=StatesManager::instance()->includeDxfInEnvsView();
    
        zoom.fitTo(model->xMin(Model::UserViewEnvs, include_images, include_dxf, include_annotations), model->yMin(Model::UserViewEnvs, include_images, include_dxf, include_annotations), model->xMax(Model::UserViewEnvs, include_images, include_dxf, include_annotations), model->yMax(Model::UserViewEnvs, include_images, include_dxf, include_annotations), _environments.width(), _environments.height(), true);

        QPoint p;
        p.setX(event->position().x()-2*_margins-_boundaryConditionLegend.width());
        p.setY(event->position().y()-(_header_height/3+offset2));

        if (event->angleDelta().y()>0)
            zoom.increase(p);
        else
            zoom.decrease(p);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_environments.width(), _environments.height()));
        double ymin=zoom.toPhysicalY(QPoint(_environments.width(), _environments.height()));
        model->useEnvsUserView(xmin, ymin, xmax, ymax);

        _environments=viewer.toImage(model, _environments.width(), _environments.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), include_images, include_dxf, include_annotations);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    }
    
    // Condensation zones.
    else if (model->computationType() == Model::GlaserCondensation && x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_condensation.width()
        && y>_header_height+_model.height()+10 && y<_header_height+_model.height()+10+_condensation.height())
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsInCondensationView();
        bool include_images=StatesManager::instance()->includeImagesInCondensationView();
        bool include_dxf=StatesManager::instance()->includeDxfInCondensationView();
    
        zoom.fitTo(model->xMin(Model::UserViewCondensation, include_images, include_dxf, include_annotations), model->yMin(Model::UserViewCondensation, include_images, include_dxf, include_annotations), model->xMax(Model::UserViewCondensation, include_images, include_dxf, include_annotations), model->yMax(Model::UserViewCondensation, include_images, include_dxf, include_annotations), _condensation.width(), _condensation.height(), true);

        QPoint p;
        p.setX(event->position().x()-2*_margins-_boundaryConditionLegend.width());
        p.setY(event->position().y()-_header_height-_model.height());

        if (event->angleDelta().y()>0)
            zoom.increase(p);
        else
            zoom.decrease(p);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_condensation.width(), _condensation.height()));
        double ymin=zoom.toPhysicalY(QPoint(_condensation.width(), _condensation.height()));
        model->useCondensationUserView(xmin, ymin, xmax, ymax);

        _condensation = viewer.toImage(model, _condensation.width(), _condensation.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes | ModelViewer::DrawCondensationResults), include_images, include_dxf, include_annotations);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    } 

    else
        QWidget::wheelEvent(event);
}

void ReportContent::mousePressEvent(QMouseEvent *event)
{
    _translate2dView=false;
    _translate1dView=false;
    _translateResultsView=false;
    _translateEnvsView=false;
    _translateCondensationView=false;

    Model *model = StatesManager::instance()->currentModel();
    if (!model)
        return;

    if (event->buttons() & Qt::MiddleButton)
    {

        int offset1=_height/3.0;
        if (_environmentsResults.empty())
            offset1=_height/2.0;
        int offset2=2.0*_height/3.0;

        double x=event->pos().x();
        double y=event->pos().y();

        if (x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_model.width()
            && y>_header_height && y<_header_height+_model.height())
            _translate2dView=true;

        else if (model->computationType() != Model::GlaserCondensation && x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_1Dmodel.width()
            && y>_header_height+_model.height()+10 && y<_header_height+_model.height()+10+_1Dmodel.height())
            _translate1dView=true;

        else if(model->computationType() != Model::GlaserCondensation && x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_results.width()
            && y>_header_height/3+offset1 && y<_header_height/3+offset1+_results.height())
            _translateResultsView=true;

        else if(model->computationType() != Model::GlaserCondensation && x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_environments.width()
            && y>_header_height/3+offset2 && y<_header_height/3+offset2+_environments.height() && !_environmentsResults.empty())
            _translateEnvsView=true;

        else if (model->computationType() == Model::GlaserCondensation && x>2*_margins+_boundaryConditionLegend.width() && x<2*_margins+_boundaryConditionLegend.width()+_condensation.width()
                 && y>_header_height+_model.height()+10 && y<_header_height+_model.height()+10+_condensation.height())
            _translateCondensationView = true;

        if (_translate2dView || _translate1dView || _translateResultsView || _translateEnvsView || _translateCondensationView)
            setCursor(QCursor(Qt::ClosedHandCursor));

        _mousePoint=event->pos();
    }
}

void ReportContent::mouseReleaseEvent(QMouseEvent *event)
{
    _translate2dView=false;
    _translate1dView=false;
    _translateResultsView=false;
    _translateEnvsView=false;
    _translateCondensationView=false;

    setCursor(QCursor(Qt::ArrowCursor));
}

void ReportContent::mouseMoveEvent(QMouseEvent * event)
{
    Model *model=StatesManager::instance()->currentModel();
    if (!model)
        return;

    ModelViewer viewer;

    double dx=_mousePoint.x()-event->position().x();
    double dy=_mousePoint.y()-event->position().y();

    if (_translate2dView)
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsIn2dView();
        bool include_images=StatesManager::instance()->includeImagesIn2dView();
        bool include_dxf=StatesManager::instance()->includeDxfIn2dView();
    
        zoom.fitTo(model->xMin(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->yMin(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->xMax(Model::UserView2dModel, include_images, include_dxf, include_annotations), model->yMax(Model::UserView2dModel, include_images, include_dxf, include_annotations), _model.width(), _model.height(), true);

        dx=zoom.toPhysicalLength(dx);
        dy=zoom.toPhysicalLength(dy);

        zoom.up(dy);
        zoom.right(dx);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_model.width(), _model.height()));
        double ymin=zoom.toPhysicalY(QPoint(_model.width(), _model.height()));
        model->use2dModelUserView(xmin, ymin, xmax, ymax);

        _model=viewer.toImage(model, _model.width(), _model.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions), include_images, include_dxf, include_annotations);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    }

    else if(_translate1dView)
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsIn1dView();
        bool include_images=StatesManager::instance()->includeImagesIn1dView();
        bool include_dxf=StatesManager::instance()->includeDxfIn1dView();

        zoom.fitTo(model->xMin(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->yMin(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->xMax(Model::UserView1dModel, include_images, include_dxf, include_annotations), model->yMax(Model::UserView1dModel, include_images, include_dxf, include_annotations), _1Dmodel.width(), _1Dmodel.height(), true);

        dx=zoom.toPhysicalLength(dx);
        dy=zoom.toPhysicalLength(dy);

        zoom.up(dy);
        zoom.right(dx);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_1Dmodel.width(), _1Dmodel.height()));
        double ymin=zoom.toPhysicalY(QPoint(_1Dmodel.width(), _1Dmodel.height()));
        model->use1dModelUserView(xmin, ymin, xmax, ymax);

        _1Dmodel=viewer.toImage(model->model1d(), _1Dmodel.width(), _1Dmodel.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawBoundaryConditions|ModelViewer::DrawEn13370Results), include_images, include_dxf, include_annotations, model);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    }

    else if(_translateCondensationView)
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsIn1dView();
        bool include_images=StatesManager::instance()->includeImagesIn1dView();
        bool include_dxf=StatesManager::instance()->includeDxfIn1dView();

        zoom.fitTo(model->xMin(Model::UserViewCondensation, include_images, include_dxf, include_annotations), model->yMin(Model::UserViewCondensation, include_images, include_dxf, include_annotations), model->xMax(Model::UserViewCondensation, include_images, include_dxf, include_annotations), model->yMax(Model::UserViewCondensation, include_images, include_dxf, include_annotations), _condensation.width(), _condensation.height(), true);

        dx=zoom.toPhysicalLength(dx);
        dy=zoom.toPhysicalLength(dy);

        zoom.up(dy);
        zoom.right(dx);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_condensation.width(), _condensation.height()));
        double ymin=zoom.toPhysicalY(QPoint(_condensation.width(), _condensation.height()));
        model->useCondensationUserView(xmin, ymin, xmax, ymax);

        _condensation = viewer.toImage(model, _condensation.width(), _condensation.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes | ModelViewer::DrawCondensationResults), include_images, include_dxf, include_annotations);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    }
    
    else if(_translateResultsView)
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsInResultsView();
        bool include_images=StatesManager::instance()->includeImagesInResultsView();
        bool include_dxf=StatesManager::instance()->includeDxfInResultsView();
    
        zoom.fitTo(model->xMin(Model::UserViewResults, include_images, include_dxf, include_annotations), model->yMin(Model::UserViewResults, include_images, include_dxf, include_annotations), model->xMax(Model::UserViewResults, include_images, include_dxf, include_annotations), model->yMax(Model::UserViewResults, include_images, include_dxf, include_annotations), _results.width(), _results.height(), true);

        dx=zoom.toPhysicalLength(dx);
        dy=zoom.toPhysicalLength(dy);

        zoom.up(dy);
        zoom.right(dx);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_results.width(), _results.height()));
        double ymin=zoom.toPhysicalY(QPoint(_results.width(), _results.height()));
        model->useResultsUserView(xmin, ymin, xmax, ymax);

        _results=viewer.toImage(model, _results.width(), _results.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawTemperatures|ModelViewer::DrawSupTemp), include_images, include_dxf, include_annotations);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    }
    
    else if (_translateEnvsView)
    {
        Zoom zoom;
        viewer.setCustomZoom(&zoom);

        bool include_annotations=StatesManager::instance()->includeAnnotationsInEnvsView();
        bool include_images=StatesManager::instance()->includeImagesInEnvsView();
        bool include_dxf=StatesManager::instance()->includeDxfInEnvsView();
    
        zoom.fitTo(model->xMin(Model::UserViewEnvs, include_images, include_dxf, include_annotations), model->yMin(Model::UserViewEnvs, include_images, include_dxf, include_annotations), model->xMax(Model::UserViewEnvs, include_images, include_dxf, include_annotations), model->yMax(Model::UserViewEnvs, include_images, include_dxf, include_annotations), _environments.width(), _environments.height(), true);

        dx=zoom.toPhysicalLength(dx);
        dy=zoom.toPhysicalLength(dy);

        zoom.up(dy);
        zoom.right(dx);

        double xmin=zoom.toPhysicalX(QPoint(0, 0));
        double ymax=zoom.toPhysicalY(QPoint(0, 0));
        double xmax=zoom.toPhysicalX(QPoint(_environments.width(), _environments.height()));
        double ymin=zoom.toPhysicalY(QPoint(_environments.width(), _environments.height()));
        model->useEnvsUserView(xmin, ymin, xmax, ymax);

        _environments=viewer.toImage(model, _environments.width(), _environments.height(), (ModelViewer::ImageOptions) (ModelViewer::DrawVolumes|ModelViewer::DrawEnvironments), include_images, include_dxf, include_annotations);

        repaint();
        StatesManager::instance()->setSavedModel(false);
    }

    _mousePoint=event->pos();
}

void ReportContent::setLambda(const double lambda)
{
    _lambda = lambda;
}
