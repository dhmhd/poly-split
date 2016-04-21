#ifndef LINE_H
#define LINE_H

#include <vector.h>
#include <ostream>

class Line {
private:
    double A, B, C;
    Vector start, end;
public:
    Line();
    Line(const Vector &start, const Vector &end);
    Line(double A, double B, double C);

    Vector getStart() const;
    Vector getEnd() const;

    double length() const;
    double squareLength() const;

    Line reverse() const;

    Vector getPointAlong(double t) const;
    double getDistance(const Vector &point) const;
    Vector getLineNearestPoint(const Vector &point) const;
    Vector getSegmentNearestPoint(const Vector &point) const;

    int pointSide(const Vector &point) const;
    int crossLineSegment(const Line &line, Vector &result) const;
    int crossSegmentSegment(const Line &line, Vector &result) const;
    int crossLineLine(const Line &line, Vector &result) const;

    static Line getBisector(const Line &l1, const Line &l2);
    static double getTanAngle(const Line &l1, const Line &l2);

    static Line directedLine(const Vector &p, const Vector &d);

    friend std::ostream& operator<< (std::ostream &out, const Line &l)
    {
        out << "[" << l.A << ", " << l.B << ", " << l.C << "]-{" << l.start << ", " << l.end << "}";
        return out;
    }
};

#endif // LINE_H
