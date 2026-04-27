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

#include <reports/ReportContent.h>
#include <LinguistManager.h>
#include <QDesktopServices>
#include <StatesManager.h>
#include <tools/ToolBox.h>
#include <tools/UiTools.h>
#include <DocxConverter.h>
#include <tools/UiTools.h>
#include <XMLInputData.h>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QLocale>
#include <QAction>
#include <Log.h>
#include <QUrl>

void ReportContent::onCustomDocxExport()
{
    QAction *action=dynamic_cast<QAction*>(sender());
    if (!action)
        return;
    QString file=action->text();
    if (file.isEmpty())
        return;
#ifdef WIN32
    std::string model=file.toLatin1().constData();
#else
	std::string model=file.toStdString();
#endif
    exportDocx(model);
}

void ReportContent::exportDocx(const std::string &model)
{
	std::string modelFilename = StatesManager::instance()->modelFilename();

    QFileDialog fileDialog(this, _tr("Exporter un rapport DocX"));
    fileDialog.setNameFilter(_tr("document DocX (*.docx)"));
    fileDialog.setDefaultSuffix("docx");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);

	// Default name & location.
	if (modelFilename != "")
	{
		QFileInfo info(modelFilename.c_str());
		fileDialog.setDirectory(info.absolutePath());
		fileDialog.selectFile(info.fileName().replace("." + info.suffix(), ".docx"));
	}

    if(!fileDialog.exec())
        return;

    QString filename=fileDialog.selectedFiles().first();
    if (filename.isEmpty())
        return;

    LOG_INFO("Export DocX to: " << filename.toStdString());

    // Create Xml data.
    std::string xml=createXmlData();

    // Create Xml dictionary.
    std::string dictionary=getDictionaryContent();

    // Create scalar images contents.
    std::vector<DocxImage*> images=getImageContents();

    // Load dictionary & report content.
    Import::Data::XMLInputData data;
    data.setDictionary(dictionary);
    data.load(xml);
    data.addImages(images);

    // Use template docx & input data.
    Export::Converter::DocxConverter converter;
    Errors::Code code=converter.load(model);
    converter.setInputData(&data);

    // Check for template import.
    if (code!=Errors::NoError)
    {
        QMessageBox dialog;
        dialog.setWindowTitle(_tr("Export du rapport DocX"));
        dialog.setText(_tr("Le modèle de rapport DocX n'est pas supporté conducteö."));
        dialog.setWindowModality(Qt::ApplicationModal);
        QIcon icon(":/images/icon.png");
        dialog.setWindowIcon(icon);
        dialog.setIcon(QMessageBox::Critical);
        dialog.exec();
        return;
    }

#ifdef WIN32
    code=converter.exportDocument(filename.toLatin1().constData());
#else
	code=converter.exportDocument(filename.toStdString());
#endif
    if (code!=Errors::NoError)
    {
        QMessageBox dialog;
        dialog.setWindowTitle(_tr("Export du rapport DocX"));
        dialog.setText(_tr("L'export du rapport DocX a échoué."));
        dialog.setWindowModality(Qt::ApplicationModal);
        QIcon icon(":/images/icon.png");
        dialog.setWindowIcon(icon);
        dialog.setIcon(QMessageBox::Critical);
        dialog.exec();
        return;
    }

    // Open file.
    QDesktopServices::openUrl(QUrl("file:///"+filename));
}

void ReportContent::onDocxExport()
{
#ifdef WIN32
    std::string model = UiTools::getTemplatesDir().toLatin1().constData();
#else
    std::string model = UiTools::getTemplatesDir().toStdString();
#endif

    model+="/model_";

    // Thermal environments.
    if (!_environmentsResults.empty())
        model+="2_";

    // Language.
    if (LinguistManager::instance()->currentLanguage()==LinguistManager::French)
        model+="fr";
    else if (LinguistManager::instance()->currentLanguage()==LinguistManager::English)
        model+="en";

    model+=".docx";

    exportDocx(model);
}

double ReportContent::getCouplingCoefficient(const std::string &env1, const std::string &env2) const
{
    for (unsigned int i=0 ; i<_environmentsResults.size() ; i++)
    {
        const EnvironmentResult &r=_environmentsResults.at(i);
        if (r._environment1Id==env1 && r._environment2Id==env2)
            return r._couplingCoefficient;
        if (r._environment1Id==env2 && r._environment2Id==env1)
            return r._couplingCoefficient;
    }
    return 0.0;
}

std::string ReportContent::getDictionaryContent() const
{
    std::string xml="<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    xml+="<dictionary>";
    xml+="<parameter name=\"owner\" xpath=\"results/owner\" />";
    xml+="<parameter name=\"address\" xpath=\"results/address\" />";
    xml+="<parameter name=\"version\" xpath=\"results/version\" />";
    xml+="<parameter name=\"date\" xpath=\"results/date\" />";
    xml+="<parameter name=\"flux2d\" xpath=\"results/flux2d\" />";
    xml+="<parameter name=\"flux1d\" xpath=\"results/flux1d\" />";
    xml+="<parameter name=\"psi\" xpath=\"results/psi\" />";
    xml+="<parameter name=\"nodes\" xpath=\"results/nodes\" />";
    xml+="<parameter name=\"fluxvariation\" xpath=\"results/fluxvariation\" />";
    xml+="<parameter name=\"error\" xpath=\"results/error\" />";
    xml+="<parameter name=\"psi_1_2\" xpath=\"results/psi_1_2\" />";
    xml+="<parameter name=\"psi_1_3\" xpath=\"results/psi_1_3\" />";
    xml+="<parameter name=\"psi_1_4\" xpath=\"results/psi_1_4\" />";
    xml+="<parameter name=\"psi_2_3\" xpath=\"results/psi_2_3\" />";
    xml+="<parameter name=\"psi_2_4\" xpath=\"results/psi_2_4\" />";
    xml+="<parameter name=\"psi_3_4\" xpath=\"results/psi_3_4\" />";
    xml+="<parameter name=\"psi_1\" xpath=\"results/psi_1\" />";
    xml+="<parameter name=\"psi_2\" xpath=\"results/psi_2\" />";
    xml+="<parameter name=\"psi_3\" xpath=\"results/psi_3\" />";
    xml+="<parameter name=\"psi_4\" xpath=\"results/psi_4\" />";
    xml+="<parameter name=\"condensation\" xpath=\"results/condensation\" />";
    xml+="<array name=\"bc\" xpath=\"results/bc/entry\">";
    xml+="<parameter name=\"legend\" xpath=\"entry/legend\" />";
    xml+="<parameter name=\"mintemp\" xpath=\"entry/mintemp\" />";
    xml+="<parameter name=\"maxtemp\" xpath=\"entry/maxtemp\" />";
    xml+="<parameter name=\"minftemp\" xpath=\"entry/minftemp\" />";
    xml+="<parameter name=\"maxftemp\" xpath=\"entry/maxftemp\" />";
    xml+="</array>";
    xml+="</dictionary>";
    return xml;
}

std::string ReportContent::createXmlData() const
{
    QLocale locale;
    std::string xml="<results>";
    xml+="<version>"+UiTools::getSoftwareVersion().toStdString()+"</version>";
    xml+="<date>"+UiTools::getCurrentDate().toStdString()+"</date>";
    xml+="<flux2d>"+locale.toString(_2DFlux, 'f', 3).toStdString()+"</flux2d>";
    xml+="<flux1d>"+locale.toString(_1DFlux, 'f', 3).toStdString()+"</flux1d>";
    xml+="<psi>"+_psi.toStdString()+"</psi>";
    xml+="<nodes>"+ToolBox::intToString(_nodes)+"</nodes>";
    xml+="<fluxvariation>"+locale.toString(_fluxVariation, 'f', 3).toStdString()+"</fluxvariation>";
    xml+="<error>"+locale.toString(_ratio, 'f', 6).toStdString()+"</error>";
    xml+="<psi_1_2>"+locale.toString(getCouplingCoefficient("1", "2"), 'f', 3).toStdString()+"</psi_1_2>";
    xml+="<psi_1_3>"+locale.toString(getCouplingCoefficient("1", "3"), 'f', 3).toStdString()+"</psi_1_3>";
    xml+="<psi_1_4>"+locale.toString(getCouplingCoefficient("1", "4"), 'f', 3).toStdString()+"</psi_1_4>";
    xml+="<psi_2_3>"+locale.toString(getCouplingCoefficient("2", "3"), 'f', 3).toStdString()+"</psi_2_3>";
    xml+="<psi_2_4>"+locale.toString(getCouplingCoefficient("2", "4"), 'f', 3).toStdString()+"</psi_2_4>";
    xml+="<psi_3_4>"+locale.toString(getCouplingCoefficient("3", "4"), 'f', 3).toStdString()+"</psi_3_4>";

    // Global coupling coefficients.
    double c1 = ToolBox::globalCouplingCoefficient(_environmentsResults, "1");
    double c2 = ToolBox::globalCouplingCoefficient(_environmentsResults, "2");
    double c3 = ToolBox::globalCouplingCoefficient(_environmentsResults, "3");
    double c4 = ToolBox::globalCouplingCoefficient(_environmentsResults, "4");

    xml+="<psi_1>"+locale.toString(c1, 'f', 3).toStdString()+"</psi_1>";
    xml+="<psi_2>"+locale.toString(c2, 'f', 3).toStdString()+"</psi_2>";
    xml+="<psi_3>"+locale.toString(c3, 'f', 3).toStdString()+"</psi_3>";
    xml+="<psi_4>"+locale.toString(c4, 'f', 3).toStdString()+"</psi_4>";

    if (_hasCondensation)
        xml += "<condensation>Oui</condensation>";
    else
        xml += "<condensation>Non</condensation>";

    xml+="<bc>";
    for (unsigned int i=0 ; i<_bc.size() ; i++)
    {
        xml+="<entry>";

        // Dummy parameter for image id.
        xml+="<legend>${imagebcnb"+ToolBox::intToString(i)+";}</legend>";

        // Minimal temperature.
        double T=_bc.at(i)._tmin;
        QString data=locale.toString(T, 'f', 2)+" °C";
        xml+="<mintemp>"+data.toStdString()+"</mintemp>";

        // Maximal temperature.
        T=_bc.at(i)._tmax;
        data=locale.toString(T, 'f', 2)+" °C";
        xml+="<maxtemp>"+data.toStdString()+"</maxtemp>";

        // Minimal temperature factor.
        data="-";
        if (_bc.at(i)._useFactor)
            data=locale.toString(_bc.at(i)._minFactor, 'f', 4);
        xml+="<minftemp>"+data.toStdString()+"</minftemp>";

        // Maximal temperature factor.
        data="-";
        if (_bc.at(i)._useFactor)
            data=locale.toString(_bc.at(i)._maxFactor, 'f', 4);
        xml+="<maxftemp>"+data.toStdString()+"</maxftemp>";
        
        xml+="</entry>";
    }
    xml+="</bc>";
    xml+="</results>";
    return xml;
}
