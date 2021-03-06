#ifndef COGRGEOMETRYFILEREADER_H
#define COGRGEOMETRYFILEREADER_H

#include <ogrsf_frmts.h>
#include <memory>
#include <string>
#include "GomoGeometry2D.h"

using Gomo::Geometry2D::Point2DArray;
using Gomo::Geometry2D::Point2D;

class COGRGeometryFileReader
{
public:
    COGRGeometryFileReader();

public:
    static std::auto_ptr<OGRGeometry> GetFirstOGRGeometryFromFile(std::string ogrfile);
    static std::auto_ptr<OGRGeometry> GetFirstOGRPointFromFile(std::string ogrfile);
    static std::auto_ptr<OGRGeometry> GetFirstOGRLineStringFromFile(std::string ogrfile);
    static std::auto_ptr<OGRGeometry> GetFirstOGRPolygonFromFile(std::string ogrfile);
    static void GetPointsFromFile(Point2DArray &pointArray, std::string filePath);
};

#endif // COGRGEOMETRYFILEREADER_H
