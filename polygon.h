#ifndef POLYGON_H
#define POLYGON_H

#include <line.h>
#include <vector.h>

class Polygon
{
private:
    Vectors poly;

public:
    Polygon();

    Polygon(const Vectors &v);

    double countSquare(void) const;
    double countSquare_signed(void) const;

    int split(double square, Polygon &poly1, Polygon &poly2, Line &cutLine) const;

    double findDistance(const Vector &point) const;
    Vector findNearestPoint(const Vector &point) const;

    Vector countCenter(void) const;

    void splitNearestEdge(const Vector &point);

    int isPointInside(const Vector &point) const;
    int isClockwise(void) const;

    const Vectors &getVectors(void) const
    {
        return poly;
    }

    void push_back(const Vector &v)
    {
        poly.push_back(v);
    }

    int empty(void) const
    {
        return poly.empty();
    }

    Vector &operator [](size_t index)
    {
        return poly[index];
    }

    Polygon &operator =(const Polygon &p)
    {
        poly = p.poly;
        return *this;
    }

    Vector operator [](size_t index) const
    {
        return poly[index];
    }

    void clear(void)
    {
        poly.clear();
    }

    size_t size(void) const
    {
        return poly.size();
    }
};

#endif // POLYGON_H
