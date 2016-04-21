#include "polygon.h"

#include <cfloat>
#include <algorithm>

struct Polygons {
    Line bisector;

    Polygon leftTriangle;
    Polygon trapezoid;
    Polygon rightTriangle;

    int p1_exist;
    int p2_exist;
    int p3_exist;
    int p4_exist;

    double leftTriangleSquare;
    double trapezoidSquare;
    double rightTriangleSquare;
    double totalSquare;
};

void createPolygons(const Line &l1, const Line &l2, Polygons &res)
{
    res.bisector = Line::getBisector(l1, l2);

    Vector v1 = l1.getStart();
    Vector v2 = l1.getEnd();
    Vector v3 = l2.getStart();
    Vector v4 = l2.getEnd();

    res.p1_exist = 0;
    res.p4_exist = 0;
    if(v1 != v4)
    {
        Line l1s = Line(v1, res.bisector.getLineNearestPoint(v1));
        Vector p1;
        res.p1_exist = l1s.crossLineSegment(l2, p1) && p1 != v4;
        if(res.p1_exist)
        {
            res.leftTriangle.push_back(v1);
            res.leftTriangle.push_back(v4);
            res.leftTriangle.push_back(p1);

            res.trapezoid.push_back(p1);
        }
        else
        {
            res.trapezoid.push_back(v4);
        }

        Line l2e = Line(v4, res.bisector.getLineNearestPoint(v4));
        Vector p4;
        res.p4_exist = l2e.crossLineSegment(l1, p4) && p4 != v1;
        if(res.p4_exist)
        {
            res.leftTriangle.push_back(v4);
            res.leftTriangle.push_back(v1);
            res.leftTriangle.push_back(p4);

            res.trapezoid.push_back(p4);
        }
        else
        {
            res.trapezoid.push_back(v1);
        }
    }
    else
    {
        res.trapezoid.push_back(v4);
        res.trapezoid.push_back(v1);
    }

    res.p2_exist = 0;
    res.p3_exist = 0;
    if(v2 != v3)
    {
        Line l2s = Line(v3, res.bisector.getLineNearestPoint(v3));
        Vector p3;
        res.p3_exist = l2s.crossLineSegment(l1, p3) && p3 != v2;
        if(res.p3_exist)
        {
            res.rightTriangle.push_back(v3);
            res.rightTriangle.push_back(v2);
            res.rightTriangle.push_back(p3);

            res.trapezoid.push_back(p3);
        }
        else
        {
            res.trapezoid.push_back(v2);
        }

        Line l1e = Line(v2, res.bisector.getLineNearestPoint(v2));
        Vector p2;
        res.p2_exist = l1e.crossLineSegment(l2, p2) && p2 != v3;
        if(res.p2_exist)
        {
            res.rightTriangle.push_back(v2);
            res.rightTriangle.push_back(v3);
            res.rightTriangle.push_back(p2);

            res.trapezoid.push_back(p2);
        }
        else
        {
            res.trapezoid.push_back(v3);
        }
    }
    else
    {
        res.trapezoid.push_back(v2);
        res.trapezoid.push_back(v3);
    }

    res.leftTriangleSquare = res.leftTriangle.countSquare();
    res.trapezoidSquare = res.trapezoid.countSquare();
    res.rightTriangleSquare = res.rightTriangle.countSquare();

    res.totalSquare = res.leftTriangleSquare + res.trapezoidSquare + res.rightTriangleSquare;
}

int findCutLine(double square, Polygons &res, Line &cutLine)
{
    if(square > res.totalSquare)
    {
        return 0;
    }

    if(!res.leftTriangle.empty() && square < res.leftTriangleSquare)
    {
        double m = square / res.leftTriangleSquare;
        Vector p = res.leftTriangle[1] + (res.leftTriangle[2] - res.leftTriangle[1]) * m;
        if(res.p1_exist)
        {
            cutLine = Line(p, res.leftTriangle[0]);
            return 1;
        }
        else if(res.p4_exist)
        {
            cutLine = Line(res.leftTriangle[0], p);
            return 1;
        }
    }
    else if(res.leftTriangleSquare < square && square < (res.leftTriangleSquare + res.trapezoidSquare))
    {
        Line t(res.trapezoid[0], res.trapezoid[3]);
        double tgA = Line::getTanAngle(t, res.bisector);
        double S = square - res.leftTriangleSquare;
        double m;
        if(fabs(tgA) > POLY_SPLIT_EPS)
        {
            double a = Line(res.trapezoid[0], res.trapezoid[1]).length();
            double b = Line(res.trapezoid[2], res.trapezoid[3]).length();
            double hh = 2.0 * res.trapezoidSquare / (a + b);
            double d = a * a - 4.0 * tgA * S;
            double h = -(-a + sqrt(d)) / (2.0 * tgA);
            m = h / hh;
        }
        else
        {
            m = S / res.trapezoidSquare;
        }
        Vector p = res.trapezoid[0] + (res.trapezoid[3] - res.trapezoid[0]) * m;
        Vector pp = res.trapezoid[1] + (res.trapezoid[2] - res.trapezoid[1]) * m;

        cutLine = Line(p, pp);
        return 1;
    }
    else if(!res.rightTriangle.empty() && square > res.leftTriangleSquare + res.trapezoidSquare)
    {
        double S = square - res.leftTriangleSquare - res.trapezoidSquare;
        double m = S / res.rightTriangleSquare;
        Vector p = res.rightTriangle[2] + (res.rightTriangle[1] - res.rightTriangle[2]) * m;
        if(res.p3_exist)
        {
            cutLine = Line(res.rightTriangle[0], p);
            return 1;
        }
        else if(res.p2_exist)
        {
            cutLine = Line(p, res.rightTriangle[0]);
            return 1;
        }
    }

    return 0;
}

int getCut(const Line &l1, const Line &l2, double s, const Polygon &poly1, const Polygon &poly2, Line &cut)
{
    double sn1 = s + poly2.countSquare_signed();
    double sn2 = s + poly1.countSquare_signed();

    if(sn1 > 0)
    {
        Polygons res;
        createPolygons(l1, l2, res);

        if(findCutLine(sn1, res, cut))
        {
            return 1;
        }
    }
    else if(sn2 > 0)
    {
        Polygons res;
        createPolygons(l2, l1, res);

        if(findCutLine(sn2, res, cut))
        {
            cut = cut.reverse();
            return 1;
        }
    }

    return 0;
}

void createSubPoly(const Vectors &poly, int line1, int line2, Polygon &poly1, Polygon &poly2)
{
    poly1.clear();
    poly2.clear();

    int pc1 = line2 - line1;
    for(int i = 1; i <= pc1; i++)
    {
        poly1.push_back(poly[i + line1]);
    }

    int polySize = poly.size();
    int pc2 = polySize - pc1;
    for(int i = 1; i <= pc2; i++)
    {
        poly2.push_back(poly[(i + line2) % polySize]);
    }
}


int isPointInsidePoly(const Vectors &poly, const Vector &point)
{
    int pointsCount = (int)poly.size() - 1;
    Line l = Line::directedLine(point, Vector(0.0, 1e100));
    int result = 0;
    Vector v;
    for(int i = 0; i < pointsCount; i++)
    {
        Line line(poly[i], poly[i + 1]);
        result += l.crossSegmentSegment(line, v);
    }
    Line line(poly[pointsCount], poly[0]);
    result += l.crossSegmentSegment(line, v);
    return result % 2 != 0;
}

int isSegmentInsidePoly(const Vectors &poly, const Line &l, size_t excludeLine1, size_t excludeLine2)
{
    size_t pointsCount = poly.size();
    for(size_t i = 0; i < pointsCount; i++)
    {
        if(i != excludeLine1 && i != excludeLine2)
        {
            Vector p1 = poly[i];
            Vector p2 = poly[i + 1 < pointsCount ? i + 1 : 0];
            Vector p;
            if(Line(p1, p2).crossSegmentSegment(l, p))
            {
                if((p1 - p).squareLength() > POLY_SPLIT_EPS)
                {
                    if((p2 - p).squareLength() > POLY_SPLIT_EPS)
                    {
                        return 0;
                    }
                }
            }
        }
    }
    return isPointInsidePoly(poly, l.getPointAlong(0.5));
}

Vector polygonCentroid(const Vectors& points)
{
    int n = points.size();
    Vector result;
    for(int i = 0; i < n; i++)
        result += points[i];
    result /= n;
    return result;
}





Polygon::Polygon()
{

}

Polygon::Polygon(const Vectors &v)
{
    poly = v;
}

double Polygon::countSquare_signed(void) const
{
    size_t pointsCount = poly.size();
    if(pointsCount < 3)
    {
        return 0;
    }

    double result = 0;
    for(size_t i = 0; i < pointsCount; i++)
    {
        if(i == 0)
            result += poly[i].x * (poly[pointsCount - 1].y - poly[i + 1].y);
        else if (i == pointsCount - 1)
            result += poly[i].x * (poly[i - 1].y - poly[0].y);
        else
            result += poly[i].x * (poly[i - 1].y - poly[i + 1].y);
    }

    return result / 2.0;
}

double Polygon::countSquare() const
{
    return fabs(countSquare_signed());
}

int Polygon::split(double square, Polygon &poly1, Polygon &poly2, Line &cutLine) const
{
    int polygonSize = (int)poly.size();

    Vectors polygon = poly;
    if(!isClockwise())
    {
        std::reverse(polygon.begin(), polygon.end());
    }

    poly1.clear();
    poly2.clear();

    if(countSquare() - square <= POLY_SPLIT_EPS)
    {
        poly1 = *this;
        return 0;
    }

    int minCutLine_exists = 0;
    double minSqLength = DBL_MAX;

    for(int i = 0; i < polygonSize - 1; i++)
    {
        for(int j = i + 1; j < polygonSize; j++)
        {
            Polygon p1;
            Polygon p2;

            createSubPoly(polygon, i, j, p1, p2);

            Line l1 = Line(polygon[i], polygon[i + 1]);
            Line l2 = Line(polygon[j], polygon[(j + 1) < polygonSize ? (j + 1) : 0]);
            Line cut;

            if(getCut(l1, l2, square, p1, p2, cut))
            {
                double sqLength = cut.squareLength();
                if(sqLength < minSqLength && isSegmentInsidePoly(polygon, cut, i, j))
                {
                    minSqLength = sqLength;
                    poly1 = p1;
                    poly2 = p2;
                    cutLine = cut;
                    minCutLine_exists = 1;
                }
            }
        }
    }

    if(minCutLine_exists)
    {
        poly1.push_back(cutLine.getStart());
        poly1.push_back(cutLine.getEnd());

        poly2.push_back(cutLine.getEnd());
        poly2.push_back(cutLine.getStart());

        return 1;
    }
    else
    {
        poly1 = Polygon(polygon);
        return 0;
    }
}

double Polygon::findDistance(const Vector &point) const
{
    double distance = DBL_MAX;
    for(int i = 0; i < (int)poly.size() - 1; i++)
    {
        Line line(poly[i], poly[i + 1]);
        Vector p = line.getSegmentNearestPoint(point);
        double l = (p - point).length();
        if(l < distance)
            distance = l;
    }
    Line line(poly[poly.size() - 1], poly[0]);
    Vector p = line.getSegmentNearestPoint(point);
    double l = (p - point).length();
    if(l < distance)
        distance = l;
    return distance;
}

Vector Polygon::findNearestPoint(const Vector &point) const
{
    Vector result;
    double distance = DBL_MAX;
    for(int i = 0; i < (int)poly.size() - 1; i++)
    {
        Line line(poly[i], poly[i + 1]);
        Vector p = line.getSegmentNearestPoint(point);
        double l = (p - point).length();
        if(l < distance)
        {
            distance = l;
            result = p;
        }
    }
    Line line(poly[poly.size() - 1], poly[0]);
    Vector p = line.getSegmentNearestPoint(point);
    double l = (p - point).length();
    if(l < distance)
    {
        distance = l;
        result = p;
    }
    return result;
}

Vector Polygon::countCenter() const
{
    return polygonCentroid(poly);
}

void Polygon::splitNearestEdge(const Vector &point)
{
    Vector result;
    int ri = -1;
    double distance = DBL_MAX;
    for(int i = 0; i < (int)poly.size() - 1; i++)
    {
        Line line(poly[i], poly[i + 1]);
        Vector p = line.getSegmentNearestPoint(point);
        double l = (p - point).length();
        if(l < distance)
        {
            distance = l;
            ri = i;
            result = p;
        }
    }
    Line line(poly[poly.size() - 1], poly[0]);
    Vector p = line.getSegmentNearestPoint(point);
    double l = (p - point).length();
    if(l < distance)
    {
        distance = l;
        ri = poly.size() - 1;
        result = p;
    }

    if(ri != -1)
    {
        poly.insert(poly.begin() + ri + 1, result);
    }
}

int Polygon::isPointInside(const Vector &point) const
{
    return isPointInsidePoly(poly, point);
}

int Polygon::isClockwise() const
{
    double sum = 0;
    int t = (int)poly.size() - 1;
    for(int i = 0; i < t; i++)
    {
        sum += (poly[i + 1].x - poly[i].x) * (poly[i + 1].y + poly[i].y);
    }
    sum += (poly[0].x - poly[t].x) * (poly[0].y + poly[t].y);
    return sum <= 0;
}
