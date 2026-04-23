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

#include <errors/ErrorHandler.h>

ErrorHandler::ErrorHandler():
    _code(NoError)
{
}

void ErrorHandler::setCode(ErrorCode code)
{
    _code=code;
}

void ErrorHandler::setContext(const std::string &context)
{
    _context=context;
}

void ErrorHandler::setDetails(const std::string &details)
{
    _details=details;
}

ErrorHandler::ErrorCode ErrorHandler::code() const
{
    return _code;
}

std::string ErrorHandler::context() const
{
    return _context;
}

std::string ErrorHandler::details() const
{
    return _details;
}

std::string ErrorHandler::codeToString(ErrorCode code, const std::string &language)
{
    if (language=="en")
    {
        if (code==NoError)
            return "No error";
        if (code==BadConductivity)
            return "Bad thermal conductivity for material (must be positive)";
        if (code==BadResistance)
            return "Bad thermal resistance for boundary condition (must be positive)";
        if (code==BadRadius)
            return "Bad radius for ellipse (must be positive)";
        if (code==BadArcBeginPoint)
            return "Bad arc begin point (must be on the ellipse)";
        if (code==BadArcEndPoint)
            return "Bad arc end point (must be on the ellipse)";
        if (code==BadCellWidth)
            return "Bad cell width for volume (must be positive)";
        if (code==BadCellHeight)
            return "Bad cell height for volume (must be positive)";
        if (code==BadModelFileRootElement)
            return "File doesn't contain any model";
        if (code==BadModelFileVersion)
            return "File contains model with unsupported version";
        if (code==BadVolumeNode)
            return "Model contains invalid volume node";
        if (code==BadArcNode)
            return "Model contains invalid arc node";
        if (code==BadLineNode)
            return "Model contains invalid line node";
        if (code==BadMaterialNode)
            return "Model contains invalid material";
        if (code==BadSurfaceNode)
            return "Model contains invalid boundary condition";
        if (code==BadArcCenter)
            return "Bad arc center";
        if (code==BadArcHorizontalRadius)
            return "Bad arc horizontal radius (must be positive)";
        if (code==BadArcVerticalRadius)
            return "Bad arc vertical radius (must be positive)";
        if (code==BadLineBeginPoint)
            return "Bad line begin point";
        if (code==BadLineEndPoint)
            return "Bad line end point";
        if (code==BadMaterialId)
            return "Bad material id (must be defined)";
        if (code==BadMaterialName)
            return "Bad material name (must be defined)";
        if (code==BadMaterialColor)
            return "Bad material color";
        if (code==BadPathNode)
            return "Bad path detected";
        if (code==UnknownPathType)
            return "Unknown path type detected";
        if (code==BadPointNode)
            return "Bad point node detected";
        if (code==BadPointXValue)
            return "Bad point X value detected";
        if (code==BadPointYValue)
            return "Bad point Y value detected";
        if (code==BadRotationNode)
            return "Bad rotation detected";
        if (code==BadRotationCenter)
            return "Bad rotation center detected";
        if (code==BadRotationAngle)
            return "Bad rotation angle detected";
        if (code==BadBcNode)
            return "Bad boundary condition node detected";
        if (code==BadBcId)
            return "Bad boundary condition id detected (must be defined)";
        if (code==BadBcName)
            return "Bad boundary condition name detected (must be defined)";
        if (code==BadBcResistance)
            return "Bad boundary condition resistance detected (must be positive)";
        if (code==BadBcTemperature)
            return "Bad boundary condition temperature detected";
        if (code==BadBcColor)
            return "Bad boundary condition color detected";
        if (code==SurfaceWithoutPath)
            return "Surface without path detected";
        if (code==BadEnvironmentName)
            return "Bad environment name detected (must be defined)";
        if (code==BadEnvironmentColor)
            return "Bad environment color detected";
        if (code==VolumeNotClosed)
            return "Not closed volume detected";
        if (code==VolumeSelfIntersection)
            return "Self intersection volume detected";
        if (code==BadImageNode)
            return "Bad image node";
        if (code==BadMaterialEmissivity)
            return "Bad material emissivity";
        if (code==BadDxfImportNode)
            return "Bad Dxf import";
        return "Unknown error";
    }
    else if (language=="de")
    {
        if (code==NoError)
            return "???";
        return "???";
    }
    
    if (code==NoError)
        return "Pas d'erreur";
    if (code==BadConductivity)
        return "Un matériau possède une conductivité thermique incorrecte (négative ou nulle)";
    if (code==BadResistance)
        return "Une condition limite possède une résistance thermique incorrecte (négative ou nulle)";
    if (code==BadRadius)
        return "Un arc ou une ellipse possède un rayon incorrect (négatif ou nul)";
    if (code==BadArcBeginPoint)
        return "Un arc possède un point de début incorrect (non positionné sur l'ellipse sous-jacente)";
    if (code==BadArcEndPoint)
        return "Un arc possède un point de fin incorrect (non positionné sur l'ellipse sous-jacente)";
    if (code==BadCellWidth)
        return "Un volume possède une largeur de maille horizontale incorrecte (négative ou nulle)";
    if (code==BadCellHeight)
        return "Un volume possède une hauteur de maille horizontale incorrecte (négative ou nulle)";
    if (code==BadModelFileRootElement)
        return "Le fichier ne contient aucune modélisation";
    if (code==BadModelFileVersion)
        return "Le fichier contient une modélisation dont la version n'est pas supportée";
    if (code==BadVolumeNode)
        return "La modélisation contient un volume incorrect";
    if (code==BadArcNode)
        return "La modélisation contient un arc ou une ellipse incorrects";
    if (code==BadLineNode)
        return "La modélisation contient un segment de droite incorrect";
    if (code==BadMaterialNode)
        return "La modélisation contient un matériau incorrect";
    if (code==BadSurfaceNode)
        return "La modélisation contient une surface incorrecte";
    if (code==BadArcCenter)
        return "Un arc possède un centre d'ellipse incorrect";
    if (code==BadArcHorizontalRadius)
        return "Un arc possède un rayon horizontal incorrect (négatif ou nul)";
    if (code==BadArcVerticalRadius)
        return "Un arc possède un rayon vertical incorrect (négatif ou nul)";
    if (code==BadLineBeginPoint)
        return "Une ligne possède un point de début incorrect";
    if (code==BadLineEndPoint)
        return "Une ligne possède un point de fin incorrect";
    if (code==BadMaterialId)
        return "Un matériau possède un identifiant incorrect (non renseigné)";
    if (code==BadMaterialName)
        return "Un matériau possède un nom incorrect (non renseigné)";
    if (code==BadMaterialColor)
        return "Un matériau possède une couleur incorrecte";
    if (code==BadPathNode)
        return "Un chemin incorrect a été détecté";
    if (code==UnknownPathType)
        return "Un chemin de type inconnu a été détecté";
    if (code==BadPointNode)
        return "Un point incorrect a été détecté";
    if (code==BadPointXValue)
        return "Un point possède une coordonnée X incorrecte";
    if (code==BadPointYValue)
        return "Un point possède une coordonnée Y incorrecte";
    if (code==BadRotationNode)
        return "Une rotation incorrecte a été détectée";
    if (code==BadRotationCenter)
        return "Un centre de rotation incorrect a été détecté";
    if (code==BadRotationAngle)
        return "Un angle de rotation incorrect a été détecté";
    if (code==BadBcNode)
        return "Une condition limite incorrecte a été détectée";
    if (code==BadBcId)
        return "Une condition limite possède un identifiant incorrect (non renseigné)";
    if (code==BadBcName)
        return "Une condition limite possède un nom incorrect (non renseigné)";
    if (code==BadBcResistance)
        return "Une condition limite possède une résistance thermique incorrecte (négative ou nulle)";
    if (code==BadBcTemperature)
        return "Une condition limite possède une température incorrecte";
    if (code==BadBcColor)
        return "Une condition limite possède une couleur incorrecte";
    if (code==SurfaceWithoutPath)
        return "Une surface sans chemin géométrique a été détectée";
    if (code==BadEnvironmentName)
        return "Une ambiance thermique possède un nom incorrect (non renseigné)";
    if (code==BadEnvironmentColor)
        return "Une ambiance thermique possède une couleur incorrecte";
    if (code==VolumeNotClosed)
        return "Un volume n'est pas fermé";
    if (code==VolumeSelfIntersection)
        return "Un volume se coupe lui-même";
    if (code==BadImageNode)
        return "Une image possède un noeud incorrect";
    if (code==BadMaterialEmissivity)
        return "Un matériau possède une émissivité incorrecte";
    if (code==BadDxfImportNode)
        return "Import de fichier Dxf incorrect";
  
    return "Erreur inconnue";
}
