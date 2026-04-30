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

#include <LinguistManager.h>
#include <tools/ToolBox.h>
#include <Translatable.h>

LinguistManager::LinguistManager():
#ifdef __linux__
    _currentLanguage(English)   //< English as default language for Linux.
#else
    _currentLanguage(French)
#endif
{
    loadEnglish();
}

LinguistManager::~LinguistManager()
{
}

void LinguistManager::registerObject(Translatable *object)
{
    _objects.push_back(object);
}

void LinguistManager::unregisterObject(Translatable *object)
{
    for (unsigned int i=0 ; i<_objects.size() ; i++)
    {
        if (_objects.at(i)==object)
        {
            _objects.erase(_objects.begin()+i);
            return;
        }
    }
}

LinguistManager::Language LinguistManager::currentLanguage() const
{
    return _currentLanguage;
}

std::string LinguistManager::languageToString() const
{
    if (_currentLanguage==English)
        return "en";
    return "fr";
}

void LinguistManager::changeLanguage(Language language)
{
    _currentLanguage=language;
    for (unsigned int i=0 ; i<_objects.size() ; i++)
        _objects.at(i)->translate();

    // Update decimal separator.
    if (language == French)
        ToolBox::setDecimalSeparator(',');
    else
        ToolBox::setDecimalSeparator('.');
}

QString LinguistManager::translate(const QString &text)
{
    // French.
    if (_currentLanguage==French)
        return text;

    // English.
    return _english[text];
}

void LinguistManager::loadEnglish()
{
    _english["Fichier"]="File";
    _english["Edition"]="Edit";
    _english["Affichage"]="View";
    _english["Projet"]="Project";
    _english["Aide"]="Help";
    _english["Préférences"]="Preferences";
    _english["Modifier"]="Modify";
    _english["Supprimer"]="Delete";
    _english["Langue"]="Language";
    _english["Nouveau"]="New";
    _english["Ouvrir..."]="Open...";
    _english["Fichiers récents"]="Recent files";
    _english["Exemples"]="Examples";
    _english["Enregistrer"]="Save";
    _english["Enregistrer sous..."]="Save as...";
    _english["Couper"]="Cut";
    _english["Copier"]="Copy";
    _english["Coller"]="Paste";
    _english["Annuler"]="Undo";
    _english["Refaire"]="Redo";
    _english["A propos"]="About";
    _english["Quitter"]="Quit";
    _english["Sélectionner tout"]="Select all";
    _english["Supprimer"]="Delete";
    _english["Installer une licence"]="Install a license";
    _english["Français"]="French";
    _english["Anglais"]="English";
    _english["Documentation"]="Documentation";
    _english["Dossier de validation"]="Validation report";
    _english["Présentation du moteur de calcul"]="Calculation engine presentation";
    _english["Méthode de calcul automatique du coefficient ψ"]="ψ-coefficient calcultion method";
    _english["Tableurs"]="Spreadsheets";
    _english["Coefficient ψ plancher bas (OpenOffice)"]="Low floor ψ-coefficient (OpenOffice)";
    _english["Coefficient ψ plancher bas (Office)"]="Low floor ψ-coefficient (Office)";
    _english["Sélection"]="Selection";
    _english["Ajout de volumes"]="Add volume";
    _english["Dessiner un rectangle"]="Draw a rectangle";
    _english["Dessiner un cercle"]="Draw a circle";
    _english["Dessiner une ellipse"]="Draw an ellipse";
    _english["Matériaux et conditions aux limites"]="Materials and boundary conditions";
    _english["Ambiances thermiques"]="Thermal environments";
    _english["Lancer la simulation"]="Start simulation";
    _english["Avancer le volume"]="Bring up volume in the stack";
    _english["Reculer le volume"]="Bring down volume in the stack";
    _english["Avancer l'image"]="Bring up picture in the stack";
    _english["Reculer l'image"]="Bring down picture in the stack";
    _english["Télécharger la mise à jour"]="Download upgrade";
    _english["Ajuster le zoom"]="Fit view";
    _english["Augmenter le zoom"]="Zoom in";
    _english["Réduire le zoom"]="Zoom out";
    _english["Recréer la modélisation sans pont thermique"]="Recreate model without thermal bridge";
    _english["Dupliquer la modélisation"]="Duplicate model";
    _english["Ajouter un modèle"]="Add model";
    _english["conducteö - calcul détaillé des ponts thermiques linéiques - version "]="conducteö - linear thermal bridges calculation - version ";
    _english["Erreur API conducteö"]="conducteö API error";
    _english["Erreur inconnue"]="Unknown error";
    _english["Ouvrir une modélisation"]="Open project";
    _english["A propos de conducteö"]="About conducteö";
    _english["Logiciel de simulation numérique des ponts thermiques linéiques, conforme à la norme européenne 10211 (version de juillet 2017) et aux hypothèses complémentaires de la règlementation thermique RE 2020."]="Software for numerical simulation of linear thermal bridges, according to the European Standard 10211 (version of July 2017) and additional assumptions of french thermal regulation RE 2020.";
    _english["Logiciel de simulation numérique des ponts thermiques linéiques."]="Software for numerical simulation of linear thermal bridges.";
    _english["<b>Référentiel normatif :</b>"]="<b> Standard compliance:</b>";
    _english["norme <b>EN 10211:2018</b> - simulation numérique des ponts thermiques"]="<b>EN 10211:2018</b> standard - numerical calculation of thermal bridges";
    _english["norme <b>EN 10077-2:2012</b> - simulation numérique des menuiseries"]="<b>EN 10077-2:2012</b> standard - numerical calculation of window frames";
    _english["norme <b>EN 6946:2007</b> - coefficients de transmission thermique"]="<b>EN 6946:2007</b> standard - thermal transmission coefficient";
    _english["<b>RT 2005, 2012 et RE 2020</b> - hypothèses complémentaires de modélisation"]="<b>RT 2005, 2012 and RE 2020</b> - additional hypothesis";
    _english["<b>Editeur :</b> Clément MARCEL<br/>"]="<b>Editor:</b> Clément MARCEL<br/>";
    _english["<b>Site internet :</b> <a href=\"https://conducteo.sourceforge.io\">https://conducteo.sourceforge.io</a><br/>"]="<b>Website:</b> <a href=\"https://conducteo.sourceforge.io\">https://conducteo.sourceforge.io</a><br/>";
    _english["Modélisation non enregistrée"]="Unregistered project";
    _english["Souhaitez-vous enregistrer la modélisation ?"]="Would you like to save modeling?";
    _english["Oui"]="Yes";
    _english["Non"]="No";
    _english["Annuler"]="Cancel";
    _english["Simulation en cours"]="Calculation in progress";
    _english["Souhaitez-vous annuler la simulation ?"]="Would you like to cancel the simulation?";
    _english["Version de démonstration"]="Demo version";
    _english["Erreur de modélisation"]="Model error";
    _english["Tous les volumes doivent définir un matériau. Vérifiez la modélisation."]="All volumes must own a material. Please check your model.";
    _english["OK"]="OK";
    _english["document DocX (*.docx)"]="DocX document (*.docx)";
    _english["Nouvelle condition à la limite"]="New boundary condition";
    _english["Nouveau matériau"]="New material";
    _english["Calcul des flux thermiques"]="Heat flows calculation";
    _english["Calcul des coefficients de couplage"]="Coupling coefficients calculation";
    _english["Niveau de raffinement :"]="Level refinement:";
    _english["<a href=\"conducteo:computation/cancel\" style=\"color: #93b543;\">Annuler</a>"]="<a href=\"conducteo:computation/cancel\" style=\"color: #93b543;\">Cancel</a>";
    _english["Flux sortant"]="Outgoing heat flow";
    _english["Sans pont thermique :"]="Without thermal bridge:";
    _english["Avec pont thermique :"]="With thermal bridge:";
    _english["Modélisation 2D :"]="2D model:";
    _english["Coefficient ψ :"]="ψ-coefficient:";
    _english["Couleur de la condition à la limite"]="Boundary condition color";
    _english["Nom"]="Name";
    _english["Température"]="Temperature";
    _english["Résistance"]="Resistance";
    _english["Couleur"]="Color";
    _english["Modélisation"]="Model";
    _english["Modélisation sans pont thermique"]="Model without thermal bridge";
    _english["Modélisation avec pont thermique"]="Model with thermal bridge";
    _english["Températures"]="Temperatures";
    _english["Note de calcul"]="Calculation note";
    _english["Mode Debug"]="Debug mode";
    _english["Ambiance thermique"]="Thermal environment";
    _english["Ambiance thermique 1"]="Thermal environment 1";
    _english["Ambiance thermique 2"]="Thermal environment 2";
    _english["Ambiance thermique 3"]="Thermal environment 3";
    _english["Ambiance thermique 4"]="Thermal environment 4";
    _english["Ajouter un matériau"]="Add material";
    _english["Exporter la bibliothèque personnelle"]="Export material database";
    _english["Importer des matériaux"]="Import materials";
    _english["Supprimer le matériau"]="Delete material";
    _english["Modifier le matériau"]="Modify material";
    _english["Matériaux"]="Materials";
    _english["Derniers utilisés"]="Last used";
    _english["Bibliothèque personnelle"]="User-defined materials";
    _english["Bibliothèque RE 2020"]="RE 2020 materials";
    _english["Exporter des matériaux"]="Export materials";
    _english["Matériaux conducteö (*.db)"]="conducteö materials";
    _english["Importer des matériaux"]="Import materials";
    _english["Couleur du matériau"]="Material color";
    _english["par défaut (25 mm)"]="default (25 mm)";
    _english["sol (200 mm)"]="ground (200 mm)";
    _english["personnalisé"]="user-defined";
    _english["Largeur"]="Height";
    _english["Hauteur"]="Width";
    _english["Souris"]="Mouse";
    _english["Position"]="Position";
    _english["Dimension"]="Dimension";
    _english["Rapport DocX"]="DocX report";
    _english["Rapport PDF"]="PDF report";
    _english["Modèle conducteö par défaut"]="Default conduteö model";
    _english["Exporter le rapport"]="Export report";
    _english["Flux de chaleur :"]="Heat flow:";
    _english["Température min. :"]="Min. temperature:";
    _english["Température max. :"]="Max. temperature:";
    _english["Condition à la limite"]="Boundary condition";
    _english["Mémoire insuffisante"]="Not enough memory";
    _english["Mémoire insuffisante pour pouvoir poursuivre l'opération."]="Not enough memory to perform calculation.";
    _english["Géométrie"]="Geometry";
    _english["Maillage"]="Mesh";
    _english["Volume"]="Volume";
    _english["Matériau"]="Material";
    _english["Ajouter une condition à la limite"]="Add boundary condition";
    _english["Supprimer la condition à la limite"]="Delete boundary condition";
    _english["Modifier la condition à la limite"]="Modify boundary condition";
    _english["Conditions limites"]="Boundary conditions";
    _english["Bibliothèque standard"]="Standard database";
    _english["Bibliothèque personnelle"]="User-defined database";
    _english["Adiabatique"]="Adiabatic";
    _english["Extérieur"]="External";
    _english["Intérieur (flux horizontal)"]="Internal (horizontal heat flow)";
    _english["Intérieur (flux ascendant)"]="Internal (upflow)";
    _english["Intérieur (flux descendant)"]="Internal (downflow)";
    _english["Intérieur (flux horizontal réduit)"]="Internal (horizontal reduced heat flow)";
    _english["Ambiance non définie"]="No defined environment";
    _english["Exporter un rapport PDF"]="Export PDF report";
    _english["document PDF (*.pdf)"]="PDF document (*.pdf)";
    _english["pont thermique linéique"]="linear thermal bridge";
    _english["Note de calcul réalisée par : "]="Calculation note created by: ";
    _english["le "]="on ";
    _english[" - conducteö version "]=" - conducteö version ";
    _english["Le logiciel conducteö est conforme à la norme EN 10211 et aux tests de validation de cette même norme et est ainsi classé comme méthode bidimensionnelle en régime permanent de haute précision. Le logiciel conducteö est également conforme aux hypothèses complémentaires de la RT 2005, RT 2012 et RE 2020. Le logiciel conducteö est conforme à la norme 10077-2 et aux tests de validation de cette même norme et peut ainsi être utilisé à des fins de calcul concernant les menuiseries."]="conducteö software is compliant with European Standard EN 10211. Validation has been done against EN 10211 validation tests. This software is classified as high precision two-dimensional method in steady state. conducteö is compliant with assumptions of French Thermal Law RT 2005, RT 2012 and RE 2020. conducteö software is compliant with European Standard EN 10077-2. Validation has been done against EN 10077-2 validation tests. This software can be used to perform calculation of windows frames.";
    _english["Conditions aux limites"]="Boundary conditions";
    _english["Modélisation sans pont thermique :"]="Model without thermal bridge:";
    _english["Convergence de la simulation"]="Calculation convergence";
    _english["Nombre de noeuds : "]="Number of nodes: ";
    _english["Variation relative des flux : "]="Relative heat flows variation: ";
    _english["Somme des flux / Flux total : "]="Heat flows sum / Global heat flow: ";
    _english["Coefficient ψ"]="ψ-coefficient";
    _english["Flux 2D : "]="2D heat flow: ";
    _english["Flux 1D : "]="1D heat flow: ";
    _english["Coefficient ψ : "]="ψ-coefficient: ";
    _english["Température\nminimale"]="Minimal\ntemperature";
    _english["Température\nmaximale"]="Maximal\ntemperature";
    _english["Facteur de\ntempérature\nminimale"]="Minimal\ntemperature\nfactor";
    _english["Facteur de\ntempérature\nmaximale"]="Maximal\ntemperature\nfactor";
    _english["Répartition du pont thermique par ambiance thermique"]="Environments thermal bridge repartition";
    _english["Exporter un rapport DocX"]="Export DocX report";
    _english["document DocX (*.docx)"]="DocX document (*.docx)";
    _english["Export du rapport DocX"]="DocX report export";
    _english["Le modèle de rapport DocX n'est pas supporté conducteö."]="This DocX report model is not yet supported by conducteö.";
    _english["L'export du rapport DocX a échoué."]="DocX report export has failed.";
    _english["à"]="at";
    _english["Validation de conducteö"]="conducteö validations";
    _english["Nombre de validations réussies : "]="Number of passed tests: ";
    _english["Nombre de validations échouées : "]="Number of failed tests: ";
    _english["Fichier incorrect"]="Bad file";
    _english["Le fichier sélectionné comporte des erreurs qui empêchent son ouverture."]="Selected file contains some errors. Unable to open it.";
    _english["Détails de l'erreur :"]="Error details:";
    _english["Erreur d'enregistrement"]="Writing error";
    _english["Une erreur dans la modélisation empêche son enregistrement."]="Writing error due to some errors in model";
    _english["Importer une image"]="Import image";
    _english["Afficher les images"]="Show images";
    _english["Masquer les images"]="Hide images";
    _english["Avancer"]="Bring to front";
    _english["Reculer"]="Bring to back";
    _english["Mettre au premier plan"]="Send to front";
    _english["Mettre à l'arrière-plan"]="Send to back";
    _english["Mettre l'image au premier plan"]="Send image to front";
    _english["Mettre l'image à l'arrière-plan"]="Send image to back";
    _english["Mettre le volume au premier plan"]="Send volume to front";
    _english["Mettre le volume à l'arrière-plan"]="Send volume to back";
    _english["Description"]="Description";
    _english["Cavités d'air"]="Air cavities";
    _english["Ajouter une catégorie"]="Add a category";
    _english["Supprimer la catégorie"]="Delete category";
    _english["Renommer la catégorie"]="Rename category";
    _english["Nouvelle catégorie"]="New category";
    _english["Vide d'air non ventilé (encadrements et coffres de volets roulants)"]="Unventilated air cavity (frames & shutter boxes)";
    _english["Vide d'air partiellement ventilé (encadrements et coffres de volets roulants)"]="Partially ventilated air cavity (frames & shutter boxes)";
    _english["Définir l'échelle"]="Define image scale";
    _english["Définition de l'échelle de l'image"]="Image scaling";
    _english["Définissez une longueur connue sur l'image en sélectionnant 2 points."]="Please select 2 points on image with known distance.";
    _english["Longueur"]="Length";
    _english["Longueur (en mètres) :"]="Length (meters):";
    _english["Calcul de la modélisation sans pont thermique"]="Computation without thermal bridge";
    _english["Simulation impossible"]="Computation error";
    _english["Une erreur irrécupérable est survenue durant la simulation."]="Fatal error has been encountered during computation.";
    _english["Résultats"]="Results";
    _english["Importer un fichier Dxf"]="Import a Dxf file";
    _english["Masquer les fichiers Dxf"]="Hide Dxf files";
    _english["Afficher les fichiers Dxf"]="Show Dxf files";
    _english["fichier Dxf (*.dxf)"]="Dxf files (*dxf)";
    _english["Importer un volume"]="Import volume from Dxf";
    _english["Simulation :"]="Computation:";
    _english["Import de volumes à partir d'éléments Dxf"]="Import volumes from Dxf entities";
    _english["Les éléments Dxf sélectionnés ne peuvent pas être convertis en volumes : ceux-ci ne définissent pas un ensemble de volumes fermés et ne se coupant pas eux-mêmes."]="Selected Dxf entities cannot be imported to create volumes: entities must define a set of closed & non self-intersecting volumes.";
    _english["La licence conducteö que vous avez importée est compatible avec la version 2 uniquement. Une licence compatible avec la version 3 est nécessaire."]="Imported license is compatible only with version2. A valid license for version 3 is required.";
    _english["Dessiner un contour fermé"]="Draw a closed polyline";
    _english["Dessin de contour fermé"]="Polyline path";
    _english["Impossible de fermer automatiquement le contour fermé dessiné.<br/>Intersection détectée."]="Unable to automatically close polyline path. Self intersection detected.";
    _english["Ajouter un point"]="Add point";
    _english["Maillages"]="Meshes";
    _english["Dimensions du plancher"]="Floor dimensions";
    _english["Largeur :"]="Width:";
    _english["Aire :"]="Area:";
    _english["Périmètre :"]="Perimeter:";
    _english["Largeur de plancher modélisée :"]="Modeled floor width:";
    _english["Propriétés thermiques du sol"]="Ground thermal properties";
    _english["choix d'un matériau :"]="material selection:";
    _english["conductivité thermique :"]="thermal conductivity:";
    _english["Ambiances et températures"]="Environments & temperatures";
    _english["Température intérieure :"]="Internal temperature:";
    _english["Température extérieure :"]="External temperature:";
    _english["Ambiance thermique intérieure :"]="Internal environment:";
    _english["Ambiance thermique extérieure :"]="External environment:";
    _english["Type de plancher bas :"]="Floor type:";
    _english["Plancher bas sur terre-plein"]="Low floor on grade";
    _english["Vide sanitaire"]="Crawl";
    _english["Sous-sol"]="Basement";
    _english["Flux de chaleur sortant :"]="Outgoing heat flow:";
    _english["Epaisseur totale du mur :"]="Total wall thickness:";
    _english["Plancher au contact du sol"]="Ground floor";
    _english["Résistance superficielle intérieure :"]="Internal superficial resistance:";
    _english["Résistance superficielle extérieure :"]="External superficial resistance:";
    _english["Résistance thermique du plancher :"]="Floor thermal resistance:";
    _english["Isolation périphérique"]="Perimeter insulation";
    _english["Epaisseur de l'isolation :"]="Insulation thickness:";
    _english["Résistance thermique de l'isolation :"]="Insulation thermal resistance:";
    _english["Longueur de l'isolation :"]="Insulation length:";
    _english["Hauteur de l'isolation :"]="Insulation height:";
    _english["inclure une isolation périphérique horizontale"]="include horizontal perimeter insulation";
    _english["inclure une isolation périphérique verticale"]="include vertical perimeter insulation";
    _english["Hauteur du vide sanitaire par rapport au sol :"]="Crawl height from ground:";
    _english["Résistance thermique du mur donnant sur l'extérieur :"]="External wall thermal resistance:";
    _english["Aire des ouvertures de ventilation divisée par le périmètre du vide sanitaire :"]="Openings area divided by crawl perimeter:";
    _english["Vitesse moyenne du vent à 10 m de hauteur :"]="Mean wind speed at 10 m height:";
    _english["Facteur de protection au vent :"]="Wind protection factor:";
    _english["Profondeur moyenne du vide sanitaire :"]="Mean crawl depth:";
    _english["Résistance superficielle intérieure du mur donnant sur le sol :"]="Underground wall internal superficial resistance:";
    _english["Résistance thermique du mur donnant sur le sol :"]="Underground wall thermal resistance:";
    _english["Résistance superficielle extérieure du mur donnant sur le sol :"]="Underground wall external superficial resistance:";
    _english["valeur par défaut :"]="default value:";
    _english["valeur fixe :"]="fixed value:";
    _english["Abritée - Centre ville (0,02)"]="Sheltered - Downtown (0.02)";
    _english["Moyenne - Banlieue (0,05)"]="Average - Suburbs (0.05)";
    _english["Exposée - Milieu rural (0,10)"]="Exposed - Rural (0.10)";
    _english["Profondeur du sous-sol :"]="Basement depth:";
    _english["Résistance superficielle intérieure :"]="Internal superficial resistance:";
    _english["Résistance superficielle extérieure :"]="External superficial resistance:";
    _english["Résistance thermique du mur :"]="Wall thermal resistance:";
    _english["Surface chauffée :"]="Heated area:";
    _english["Surface non chauffée :"]="Unheated area:";
    _english["Résistance thermique du plafond du sous-sol :"]="Basement ceil thermal resistance:";
    _english["Hauteur :"]="Height:";
    _english["Résistance thermique :"]="Thermal resistance:";
    _english["Hauteur moyenne du sous-sol :"]="Mean basement height:";
    _english["Taux de renouvellement d'air :"]="Air flow rate:";
    _english["Mur du sous-sol"]="Basement wall";
    _english["Chauffage du sous-sol :"]="Basement heating:";
    _english["sous-sol non chauffé"]="unheated basement";
    _english["sous-sol chauffé"]="heated basement";
    _english["sous-sol partiellement chauffé"]="partially heated basement";
    _english["Mur du sous-sol donnant sur l'extérieur"]="External basement wall";
    _english["Résistance thermique du plafond du vide sanitaire :"]="Crawl ceil thermal resistance:";
    _english["Intégrer les pertes des planchers bas"]="Include low-floor losses";
    _english["conducteö - calcul des déperditions des planchers bas selon la norme EN 13370"]="conducteö - low-floor losses calculation according to EN 13370 standard";
    _english["norme <b>EN 13370:2007</b> - transfert de chaleur par le sol"]="<b>EN 13370:2007</b> standard - losses through ground";
    _english["Mesurer une longueur"]="Read a distance";
    _english["Mesurer un angle"]="Read an angle";
    _english["Manuel d'utilisation"]="User manual";
    _english["Modéliser les ponts thermiques"]="How to model thermal bridges?";
    _english["Flux traversant le plancher bas (EN 13370) : "]="Heat flow through low-floor (EN 13370):";
    _english["Préférences"]="Preferences";
    _english["Langue de l'interface :"]="Interface language:";
    _english["Licence d'utilisation :"]="License:";
    _english["pas de licence installée"]="no license found";
    _english["Installer"]="Install";
    _english["Vérification des mises à jour :"]="Upgrades checks:";
    _english["vérifier au démarrage du logiciel"]="check at software startup";
    _english["Général"]="General";
    _english["Modèles de rapport DocX :"]="DocX report models:";
    _english["Ajouter"]="Add";
    _english["Contenu des images exportées :"]="Exported images content:";
    _english["Modélisation avec pont thermique :"]="Model with thermal bridge:";
    _english["Modélisation sans pont thermique"]="Model without thermal bridge";
    _english["Champ de températures :"]="Temperatures field:";
    _english["Ambiances thermiques :"]="Environments:";
    _english["Annotations"]="Annotations";
    _english["Images"]="Images";
    _english["Dxf"]="Dxf";
    _english["Changer"]="Change";
    _english["Ajouter une annotation textuelle"]="Add a textual annotation";
    _english["Ajouter une annotation fléchée"]="Add an arrow annotation";
    _english["Ajouter une côte"]="Add a cotation";
    _english["Annotation textuelle"]="Textual annotation";
    _english["Texte :"]="Text:";
    _english["Importer les matériaux dans :"]="Import materials here:";
    _english["Longueur :"]="Length:";
    _english["Type de calcul"]="Computation type";
    _english["Pont thermique linéique"]="Linear thermal bridge";
    _english["Flux de chaleur"]="Thermal flux";
    _english["Transmission thermique"]="Thermal transmission";
    _english["Longueur fixée"]="Fixed length";
    _english["Longueur fixée :"]="Fixed length:";
    _english["Longueur (en mm) :"]="Length (in mm):";
    _english["Modélisation :"]="Model:";
    _english["Coefficient U :"]="U factor:";
    _english["Modélisation non calculée"]="Outdated results";
    _english["Souhaitez-vous recalculer la modélisation ?"]="Model results are out of date. Recalculate?";
    _english["Déplier la condition à la limite"]="Expand boundary condition";
    _english["Extruder la modélisation"]="Extrude model";
    _english["Calcul de la modélisation :"]="Model computation:";
    _english["proposer lors de la fermeture"]="ask when exiting";
    _english["Exemples simples"]="Simple examples";
    _english["Ponts thermiques répartis"]="Splitted thermal bridges";
    _english["Planchers bas"]="Ground floor";
    _english["Menuiseries"]="Windows";
    _english["Exemples du tutoriel"]="Tutorial examples";
    _english["Exemples du dossier de validation"]="Validation report examples";
    _english["Angle entrant"]="Inside corner";
    _english["Angle sortant"]="Outside corner";
    _english["Angle sortant avec parement plâtre"]="Outside corner with plaster cladding";
    _english["Acrotère"]="Acroter";
    _english["Plancher intermédiaire isolé"]="Isolated intermediate floor";
    _english["Plancher intermédiaire"]="Intermediate floor";
    _english["Renfort d'isolation par l'extérieur"]="External insulation reinforcement";
    _english["Plancher bas sur terre plein (EN 13370) - Option A"]="Ground floor (EN 13370) - Option A";
    _english["Plancher bas sur terre plein - Option B"]="Ground floor (EN 13370) - Option B";
    _english["Menuiserie en aluminium (EN 10077-2)"]="Aluminium frame (EN 10077-2)";
    _english["Norme EN 10211"]="EN 10211 standard";
    _english["Norme EN 10077-2"]="EN 10077-2 standard";
    _english["Autres tests"]="Other tests";
    _english["Cas n°1"]="Case no 1";
    _english["Cas n°2"]="Case no 2";
    _english["Cas D.1"]="Case D.1";
    _english["Cas D.2"]="Case D.2";
    _english["Cas D.3"]="Case D.3";
    _english["Cas D.4"]="Case D.4";
    _english["Cas D.5"]="Case D.5";
    _english["Cas D.6"]="Case D.6";
    _english["Cas D.7"]="Case D.7";
    _english["Cas D.8"]="Case D.8";
    _english["Cas D.9"]="Case D.9";
    _english["Cas D.10"]="Case D.10";
    _english["Bâtiment de référence"]="Reference building";
    _english["Plancher intermédiaire"]="Intermediate floor";
    _english["Plancher haut"]="Ceil";
    _english["Angle de mur"]="Wall corner";
    _english["Mur avec tasseaux"]="Wall with cleats";
    _english["Acrotère"]="Acroter";
    _english["Plancher intermédiaire ψ₁ – ψ₂"]="Intermediate floor ψ₁ – ψ₂";
    _english["Plancher bas sur terre-plein"]="Ground floor on earth";
    _english["Plancher bas sur terre-plein sous le niveau du sol"]="Ground floor under earth";
    _english["Plancher bas surélevé sur terre-plein"]="Elevated ground floor on earth";
    _english["Plancher bas sur vide sanitaire"]="Ground floor on crawl";
    _english["Plancher bas en sous-sol"]="Low floor basement";
    _english["Mur de refend"]="Shear wall";
    _english["Cadre de menuiserie"]="Window frame";
    _english["Jonction vitrage - menuiserie"]="Galss - frame junction";
    _english["Mise en œuvre de menuiserie"]="Window implementation";
    _english["Bâtiment entier"]="Whole building";
    _english["Moitié de bâtiment"]="Simplified building";
    _english["Modèle complet"]="Whole model";
    _english["Modèle extrait"]="Extracted model";
    _english["Modèle extrait - dimensions intérieures"]="Extracted model - internal dimensions";
    _english["Modèle extrait - dimensions extérieures"]="Extracted model - external dimensions";
    _english["Dimensions intérieures"]="Internal dimensions";
    _english["Dimensions extérieures"]="External dimensions";
    _english["Option A - EN 13370"]="Option A - EN 13370";
    _english["Option B"]="Option B";
    _english["Option B (modifiée)"]="Option B (modified)";
    _english["Mise à jour disponible"]="Update available";
    _english["La mise à jour de conducteö a été téléchargée. Cliquez ici pour l'installer."]="conducteö update has been downloaded. Please click here to install it.";
    _english["Guide de démarrage"]="Quickstart";
    _english["C'est votre premier démarrage de conducteö.<br/><br/>Souhaitez-vous afficher le guide de démarrage rapide pour modéliser votre premier pont thermique en moins de 10 minutes ?"]="It's the first time you launch conducteö.<br/><br/>Do you want to display conducteö quick start to model your first thermal bridge in 10 minuts ?";
    _english["ne plus me proposer à l'avenir"]="hide in the future";
    _english["Renommer"]="Rename";
    _english["Renommer une ambiance thermique"]="Rename a thermal environment";
    _english["Nom :"]="Name:";
    _english["Supprimer l'annotation"]="Delete annotation";
    _english["Modifier le texte"]="Update text";    
    _english["Sélectionner un matériau"]="Select a material";
    _english["Conductivité thermique équivalente"]="Equivalent thermal conductivity";
    _english["Epaisseur :"] = "Thickness:";
    _english["Conductivité équivalente"] = "Equivalent conductivity";
    _english["Largeur (en mm) :"] = "Width (mm):";
    _english["Epaisseur"] = "Thickness";
    _english["Epaisseur (en mm) :"] = "Thickness (mm):";
    _english["Résistances superficielles :"] = "Superficial resistances:";
    _english["Tourner la sélection"] = "Rotate selection";
    _english["Angle de rotation (en °) :"] = "Rotation angle (°):";
    _english["Effacer les conditions limites"] = "Clean all boundary conditions";
    _english["Effacer les ambiances thermiques"] = "Clean all thermal environments";
    _english["Appliquer le matériau aux volumes"] = "Apply material to volumes";
    _english["Appliquer la condition limite aux surfaces"] = "Apply boundary condition to surfaces";
    _english["Condensation - méthode de Glaser"] = "Condensation - Glaser method";
    _english["Taux d'humidité"] = "Humidity";
    _english["Risques de condensation"] = "Condensation zones";
    _english["Calcul des flux d'humidité"] = "Humidity computation";
    _english["Humidité"] = "Humidity";
    _english["Condensation"] = "Condensation";
    _english["pont thermique linéique"] = "linear thermal bridge";
    _english["flux de chaleur"] = "thermal flux";
    _english["transmission thermique"] = "thermal transmission";
    _english["conductivité thermique équivalente"] = "equivalent thermal conductivity";
    _english["risques de condensation"] = "condensations zones";
    _english["Condensation :"] = "Condensation:";
    _english["Zones de condensation"] = "Condensation zones";
    _english["Humidité min. :"] = "Min. humidity:";
	_english["Humidité max. :"] = "Max. humidity:";
	_english["Matériaux DXF :"] = "DXF materials:";
	_english["Erreur d'import DXF"] = "DXF import error";
	_english["Aucune catégorie de matériaux n'est définie pour l'import DXF !"] = "DXF materials import category not defined!";
	_english["La catégorie des matériaux d'import DXF est vide !"] = "DXF import category is empty!";
	_english["Le fichier DXF ne contient aucun calque !"] = "DXF file doesn't contain any layer!";
	_english["Le fichier DXF ne contient aucune Polyligne fermée ne se recoupant pas elle-même !"] = "DXF file doesn't contain any valid poyline without self intersection!";
}
