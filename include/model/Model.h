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

#ifndef MODEL_H
#define MODEL_H

#include <numeric/EnvironmentResult.h>
#include <importer/ThermImporter.h>
#include <geometry/Point.h>
#include <Eigen/Sparse>
#include <vector>

// Eigen.
typedef Eigen::Triplet<double> double_triplet;
typedef std::map<unsigned int, double> DbRowMatrix;
typedef DbRowMatrix::iterator DbRowMatrixIt;
typedef std::map<unsigned int, DbRowMatrix> DbMatrix;
typedef DbMatrix::iterator DbMatrixIt;

class AirCavityFromMesh;
class En13370Plugin;
class ErrorHandler;
class TiXmlElement;
class DxfContent;
class Annotation;
class Surface;
class Volume;
class Image;
class Line;
class Mesh;
class Cell;

class CellLine
{

public:

    unsigned int    _cellIdx;
    unsigned int    _lineIdx;
    double          _area;
    double          _internalResistance;
    double          _scalarProduct;
    Surface        *_surface;
    Point           _superficialPoint;

};

class Model: public std::vector<Surface*>,
             public std::vector<Volume*>,
             public std::vector<Image*>,
             public std::vector<DxfContent*>,
             public std::vector<Annotation*>,
             public ThermImporter
{

public:

    enum CallbackType
    {
        PhysicalFlux,
        CouplingCoefficients,
        WithoutTBFlux,
        GlaserComputation
    };

    enum ExportViewUserZoomType
    {
        UserView1dModel,
        UserView2dModel,
        UserViewResults,
        UserViewEnvs,
        UserViewCondensation
    };

    enum ModelType
    {
        ConducteoFile,
        Therm_7_3_File
    };

    enum ComputationType
    {
        ThermalBridge,
        Flux,
        ThermalTransmission,
        EquivalentThermalConductivity,
        GlaserCondensation
    };

    enum TransmissionCoefficientLengthType
    {
        FullLength,
        ProjectedXLength,
        ProjectedYLength,
        CustomLength
    };

    typedef void (*ModelComputationCallback) (Model::CallbackType, int, int, void*);

    Model();
    Model(const Model &model);
    virtual ~Model();

    bool check(ErrorHandler *error);
    ModelType modelType() const;

    void setModel1d(Model *model);
    int volumeStackPosition(Volume *volume) const;
    int volumeStackPosition(const std::string &id) const;
    int imageStackPosition(Image *image) const;
    void addVolume(Volume *volume);
    void insertVolume(Volume *volume, int position);
    void insertImage(Image *image, int position);
    void removeVolume(Volume *volume);
    void removeVolume(const std::string &id);
    void removeImage(Image *image);
    void removeImage(const std::string &id);
    void removeDxf(DxfContent *dxf);
    void removeDxf(const std::string &id);
    void recreateSurfaces();
    void removeAnnotation(const std::string &id);

    void bringVolumeUp(Volume *volume);
    void bringVolumeDown(Volume *volume);
    void bringImageUp(Image *image);
    void bringImageDown(Image *image);

    void setVolumeStackPosition(Volume *volume, unsigned int position);
    void setImageStackPosition(Image *image, unsigned int position);

    void selectAllVolumes();
    void unselectAllVolumes();
    void unselectAllImages();
    void unselectAllDxf();
    void unselectAllAnnotations();

    bool isEmpty() const;

    double xMin(bool showImages=true, bool showDxf=true, bool showAnnotations=true) const;
    double xMax(bool showImages=true, bool showDxf=true, bool showAnnotations=true) const;
    double yMin(bool showImages=true, bool showDxf=true, bool showAnnotations=true) const;
    double yMax(bool showImages=true, bool showDxf=true, bool showAnnotations=true) const;

    double xMin(ExportViewUserZoomType type, bool showImages=true, bool showDxf=true, bool showAnnotations=true) const;
    double xMax(ExportViewUserZoomType type, bool showImages=true, bool showDxf=true, bool showAnnotations=true) const;
    double yMin(ExportViewUserZoomType type, bool showImages=true, bool showDxf=true, bool showAnnotations=true) const;
    double yMax(ExportViewUserZoomType type, bool showImages=true, bool showDxf=true, bool showAnnotations=true) const;

    void use1dModelUserView(double xmin, double ymin, double xmax, double ymax);
    void use1dModelUserView(bool use);
    void use2dModelUserView(double xmin, double ymin, double xmax, double ymax);
    void use2dModelUserView(bool use);
    void useResultsUserView(double xmin, double ymin, double xmax, double ymax);
    void useResultsUserView(bool use);
    void useEnvsUserView(double xmin, double ymin, double xmax, double ymax);
    void useEnvsUserView(bool use);
    void useCondensationUserView(double xmin, double ymin, double xmax, double ymax);
    void useCondensationUserView(bool use);

    Volume *getVolumeAtPosition(double x, double y) const;
    Volume *getVolumeById(const std::string &id) const;
    Surface *getSurfaceAtPosition(double x, double y, double precision) const;
    Annotation *getAnnotationAtPosition(double x, double y, double precision) const;
    Annotation *getAnnotationById(const std::string& id) const;
    Image *getImageAtPosition(double x, double y, double precision=0.0) const;
    Image *getImageById(const std::string &id) const;
    DxfContent *getDxfById(const std::string &id) const;
    Model *model1d() const;

    Mesh *mesh() const;
    Mesh *glaserMesh() const;

    void compute(ErrorHandler *error, bool compute_environments);
    void computeHumidity(ErrorHandler *error);
    void abort();
    bool isAborted() const;
    void set1dLevel(int level);
    
    void setRunning(bool running);
    bool running() const;
    void deleteObject(bool d);
    bool deleteObject() const;

    unsigned int numberOfNodes() const;
    double fluxRelativeVariation() const;
    double fluxSum() const;
    double flux2d() const;
    double flux1d() const;
    bool psiCoefficientComputed() const;
    double psiCoefficient() const;
    std::vector<EnvironmentResult> environmentResults() const;

    void setMaterialImportCategory(const std::string &category);

    double temperatureAtPosition(double x, double y, bool *success) const;
    double humidityAtPosition(double x, double y, bool *success) const;
    double pressureAtPosition(double x, double y, bool *success) const;

    Cell *cellAtPosition(double x, double y) const;
    Cell *cellAtPosition(Mesh *mesh, double x, double y) const;
    double getSaturationPression(double temperature) const;
    std::vector<Cell*> createCellsFromLines(Cell *ref_cell, const std::vector<Line*> &lines, Volume *ref_volume=0) const;

    void fromXml(const std::string &xml, ErrorHandler *error, bool updateSurfaces=true);

    std::string toXml_V3(ErrorHandler *error, bool export_databases=true) const;
    std::string toXml_V2(ErrorHandler *error, bool export_databases=true) const;

    void setCallback(ModelComputationCallback cb, void *p);
    void setCurrentErrorHandler(ErrorHandler *error);

    std::vector<Point> points() const;

#ifdef DISPLAY_MATRIX_CONTENT
    std::string matrixData(unsigned int row) const;
    std::vector<unsigned int> matrixNeighbors(unsigned int row) const;
#endif

    En13370Plugin *en13370Plugin() const;
    void createEn13370Plugin();
    void removeEn13370Plugin();

    void setComputationType(ComputationType type);
    ComputationType computationType() const;
    double transmissionCoefficientLength() const;
    void setTransmissionCoefficientLength(double length);
    double transmissionCoefficient() const;
    double equivalentThermalConductivity() const;
    TransmissionCoefficientLengthType transmissionCoefficientLengthType() const;
    void setTransmissionCoefficientLengthType(TransmissionCoefficientLengthType type);

    // Interop.
    bool validResults() const;
    void set2dPngView(const std::string &image);
    void set1dPngView(const std::string &image);
    void set2dResultsPngView(const std::string &image);
    void set1dResultsPngView(const std::string &image);
    void setResultsValidity(bool valid);

    double getEquivalentConductivityWidth() const;
    void setEquivalentConductivityWidth(double equivalentConductivityWidth);

    double getEquivalentConductivityThickness() const;
    void setEquivalentConductivityThickness(double equivalentConductivityThickness);

    double getEquivalentConductivityResistanceSum() const;
    void setEquivalentConductivityResistanceSum(double equivalentConducitityResistanceSum);

    bool getCondensation() const;
    void setCondensation(bool condensation);

protected:

    void addPolygon(const std::vector<double> x_values, const std::vector<double> y_values, const std::string &materialId);
    void finalizePolygons();
    void addSurface(double x0, double y0, double x1, double y1, const std::string &boundaryId);

private:

    void fromV2Xml(const std::string &xml, ErrorHandler *error);
    void fromV3Xml(const std::string &xml, ErrorHandler *error);
    void clean();
    void importEnvironments(const std::string &xml);

    std::string exportEnvironments() const;

    void recreateSurfaces(Volume *volume);
    std::vector<Line*> extractBoundarySurfaces(Volume *volume) const;
    std::vector<Line*> extractNonIntersectedSurfaces(const std::vector<Line*> &surfaces) const;
    std::vector<Line*> getAllSurfaces() const;
    std::vector<Line*> getAllVisibleNonIntersectedSurfaces() const;
    std::vector<Line*> removeInternalSurfaces(const std::vector<Line*> &surfaces) const;
    std::vector<Line*> copySurfacesAndAddVolume(Volume *volume) const;
    std::vector<Line*> cellAndVolumesSurfaces(Cell *cell, Volume *ref_volume) const;
    bool intersection(Cell *cell, const std::vector<Line*> &lines) const;
    std::vector<Line*> splitCell(Cell *cell, const std::vector<Line*> &surfaces, Volume *ref_volume) const;
    Line *nextLine(Line *line, const std::vector<Line*> &lines, int &idx) const;
    Line *firstLine(const std::vector<Line*> &lines, int &idx) const;
    Surface *surfaceByPoint(const std::vector<Surface*> &surfaces, const Point &point) const;
    bool mergeSurfaces();

    void generateMatrix(ErrorHandler *error);
    void generateHumidityMatrix(ErrorHandler *error);

    Mesh *createMesh(bool &use_air_cavities, bool humidityMode, ErrorHandler *error) const;
    Mesh *createHumidityMesh(ErrorHandler *error) const;

    std::vector<AirCavityFromMesh*> createCavities(std::vector<Cell*> &unventilated, std::vector<Cell*> &partiallyVentilated) const;

    void createMatrix(DbMatrix &matrix_coefficients, Eigen::VectorXd &b, std::vector<CellLine> &lineWithExternalBoundaryCondition, const std::vector<Cell*> &internal_cells, const std::vector<Cell*> &external_cells, const std::vector<Cell*> &external_lookup_table) const;
    void createHumidityMatrix(DbMatrix &matrix_coefficients, Eigen::VectorXd &b, std::vector<CellLine> &lineWithExternalBoundaryCondition, const std::vector<Cell*> &internal_cells, const std::vector<Cell*> &external_cells, const std::vector<Cell*> &external_lookup_table) const;

    unsigned int addCellsIntoCavity(AirCavityFromMesh *cavity, std::vector<Cell*> &cells) const;
    double globalPositiveFlux() const;
    double globalFlux() const;
    double compute1dFlux() const;
    double compute2dFlux() const;
    double computePsiCoefficient(bool *success) const;
    inline double round(double value) const;

    void recreatePointsList();

    double coefficientTransmissionFullLength() const;
    double coefficientTransmissionProjectedXLength() const;
    double coefficientTransmissionProjectedYLength() const;

    void importComputationType(TiXmlElement *model);
    std::string exportComputationType() const;

    // Interop.
    void importInterop(const std::string &xml, ErrorHandler *error);
    std::string exportInterop() const;

    Model  *_1dModel;
    Mesh   *_glaserMesh;
    Mesh   *_mesh;

    bool    _deleteObject;
    bool    _running;
    bool    _is1dModel;

    double  _positiveFlux;
    double  _positiveFluxOld;
    double  _residualFlux;
    double  _flux2d;
    double  _flux1d;
    double  _psiCoefficient;
    bool    _psiCoefficientComputed;
    bool    _condensation;

    std::vector<EnvironmentResult>  _environmentResults;

    ModelComputationCallback    _callback;
    void                       *_caller;

    ErrorHandler               *_currentError;

    std::vector<Point>          _points;

#ifdef DISPLAY_MATRIX_CONTENT
    std::vector<std::string>                _matrixData;
    std::vector<std::vector<unsigned int>>  _matrixNeighborCells;
#endif

    En13370Plugin      *_en13370;

    bool                _2dModelUserView;
    bool                _1dModelUserView;
    bool                _resultsUserView;
    bool                _envsUserView;
    bool                _condensationUserView;

    double              _2dModelXmin;
    double              _2dModelXmax;
    double              _2dModelYmin;
    double              _2dModelYmax;

    double              _1dModelXmin;
    double              _1dModelXmax;
    double              _1dModelYmin;
    double              _1dModelYmax;

    double              _resultsXmin;
    double              _resultsXmax;
    double              _resultsYmin;
    double              _resultsYmax;

    double              _envsXmin;
    double              _envsXmax;
    double              _envsYmin;
    double              _envsYmax;

    double              _condensationXmin;
    double              _condensationXmax;
    double              _condensationYmin;
    double              _condensationYmax;

    ModelType           _filetype;

    std::string         _materialImportCategory;

    ComputationType                     _computationType;
    double                              _transmissionCoefficientLength;
    TransmissionCoefficientLengthType   _transmissionCoefficientLengthType;

    // lambda equivalent.
    double                              _equivalentConductivityWidth;
    double                              _equivalentConductivityThickness;
    double                              _equivalentConductivityResistanceSum;

    // Interop.
    bool                _validResults;
    std::string         _2dPngView;
    std::string         _1dPngView;
    std::string         _2dResultsPngView;
    std::string         _1dResultsPngView;

    // Fix material & bc swapping.
    std::map<std::string, std::string>  _swap_materials;
    std::map<std::string, std::string>  _swap_boundaries;
};

#endif
