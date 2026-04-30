/****************************************************************************
** Copyright (C) 2001-2013 RibbonSoft, GmbH. All rights reserved.
**
** This file is part of the dxflib project.
**
** This file is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Licensees holding valid dxflib Professional Edition licenses may use 
** this file in accordance with the dxflib Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.ribbonsoft.com for further details.
**
** Contact info@ribbonsoft.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef DL_CREATIONADAPTER_H
#define DL_CREATIONADAPTER_H

#include "dl_global.h"

#include "dl_creationinterface.h"

#ifdef _DEBUG
#include <fstream>
#define output_debug() { std::ofstream out("dxf_debug.log", std::ios::app); out << "Not implemented: " << __FUNCTION__ << std::endl; out.close(); }
#else
    #define output_debug() 
#endif

/**
 * An abstract adapter class for receiving DXF events when a DXF file is being read. 
 * The methods in this class are empty. This class exists as convenience for creating 
 * listener objects.
 *
 * @author Andrew Mustun
 */
class DXFLIB_EXPORT DL_CreationAdapter : public DL_CreationInterface {
public:
    DL_CreationAdapter() {}
    virtual ~DL_CreationAdapter() {}
    virtual void processCodeValuePair(unsigned int, const std::string&) {output_debug()}
    virtual void endSection() {output_debug()}
    virtual void addLayer(const DL_LayerData&) {output_debug()}
    virtual void addBlock(const DL_BlockData&) {output_debug()}
    virtual void endBlock() {output_debug()}
    virtual void addTextStyle(const DL_StyleData&) {output_debug()}
    virtual void addPoint(const DL_PointData&) {output_debug()}
    virtual void addLine(const DL_LineData&) {output_debug()}
    virtual void addArc(const DL_ArcData&) {output_debug()}
    virtual void addCircle(const DL_CircleData&) {output_debug()}
    virtual void addEllipse(const DL_EllipseData&) {output_debug()}
    
    virtual void addPolyline(const DL_PolylineData&) {output_debug()}
    virtual void addPolylineWithVertices(const DL_PolylineData&, double* vertices) {output_debug()}
    virtual void addVertex(const DL_VertexData&) {output_debug()}
    
    virtual void addSpline(const DL_SplineData&) {output_debug()}
    virtual void addSplineWithVectors(const DL_SplineData&, double* controlPoints, double* weightCP, double* fitPoints, double* knots) {output_debug()}
    
    virtual void addControlPoint(const DL_ControlPointData&) {output_debug()}
    virtual void addFitPoint(const DL_FitPointData&) {output_debug()}
    virtual void addKnot(const DL_KnotData&) {output_debug()}
    
    virtual void addInsert(const DL_InsertData&) {output_debug()}
    
    virtual void addMText(const DL_MTextData&) {output_debug()}
    virtual void addMTextChunk(const std::string&) {output_debug()}
    virtual void addText(const DL_TextData&) {output_debug()}
    
    virtual void addDimAlign(const DL_DimensionData&,
                             const DL_DimAlignedData&) {output_debug()}
    virtual void addDimLinear(const DL_DimensionData&,
                              const DL_DimLinearData&) {output_debug()}
    virtual void addDimRadial(const DL_DimensionData&,
                              const DL_DimRadialData&) {output_debug()}
    virtual void addDimDiametric(const DL_DimensionData&,
                              const DL_DimDiametricData&) {output_debug()}
    virtual void addDimAngular(const DL_DimensionData&,
                              const DL_DimAngularData&) {output_debug()}
    virtual void addDimAngular3P(const DL_DimensionData&,
                              const DL_DimAngular3PData&) {output_debug()}
    virtual void addDimOrdinate(const DL_DimensionData&,
                             const DL_DimOrdinateData&) {output_debug()}
    virtual void addLeader(const DL_LeaderData&) {output_debug()}
    virtual void addLeaderVertex(const DL_LeaderVertexData&) {output_debug()}
    
    virtual void addHatch(const DL_HatchData&) {output_debug()}

    virtual void addTrace(const DL_TraceData&) {output_debug()}
    virtual void add3dFace(const DL_3dFaceData&) {output_debug()}
    virtual void addSolid(const DL_SolidData&) {output_debug()}
    
    virtual void addImage(const DL_ImageData&) {output_debug()}
    virtual void linkImage(const DL_ImageDefData&) {output_debug()}
    virtual void addHatchLoop(const DL_HatchLoopData&) {output_debug()}
    virtual void addHatchEdge(const DL_HatchEdgeData&) {output_debug()}

    virtual void addXRecord(const std::string&) {output_debug()}
    virtual void addXRecordString(int, const std::string&) {output_debug()}
    virtual void addXRecordReal(int, double) {output_debug()}
    virtual void addXRecordInt(int, int) {output_debug()}
    virtual void addXRecordBool(int, bool) {output_debug()}

    virtual void addXDataApp(const std::string&) {output_debug()}
    virtual void addXDataString(int, const std::string&) {output_debug()}
    virtual void addXDataReal(int, double) {output_debug()}
    virtual void addXDataInt(int, int) {output_debug()}

    virtual void addDictionary(const DL_DictionaryData&) {output_debug()}
    virtual void addDictionaryEntry(const DL_DictionaryEntryData&) {output_debug()}

    virtual void endEntity() {output_debug()}

    virtual void addComment(const std::string&) {output_debug()}

    virtual void setVariableVector(const std::string&,  double, double, double, int) {output_debug()}
    virtual void setVariableString(const std::string&, const std::string&, int) {output_debug()}
    virtual void setVariableInt(const std::string&, int, int) {output_debug()}
    virtual void setVariableDouble(const std::string&, double, int) {output_debug()}
#ifdef DL_COMPAT
    virtual void setVariableVector(const char*,  double, double, double, int) {}
    virtual void setVariableString(const char*, const char*, int) {}
    virtual void setVariableInt(const char*, int, int) {}
    virtual void setVariableDouble(const char*, double, int) {}
    virtual void processCodeValuePair(unsigned int, char*) {}
    virtual void addComment(const char*) {}
    virtual void addMTextChunk(const char*) {}
#endif
    virtual void endSequence() {output_debug()}
};

#endif
