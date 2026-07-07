#pragma once

#include <string>
#include <utility>

#include "core.h"

namespace cartocrow {

// TODO: convert to/from CGAL ellipse

class EllipseAtOrigin;

/// \author Gijs Pennings
class Ellipse {
  protected:
	double A, B, C, D, E, F;

  public:
	struct Parameters {
		double a, b, angle, x0, y0;
		std::array<std::array<double, 3>, 3> matrix() const;
	};

	Ellipse() : A(1), B(), C(1), D(), E(), F(-1) {}
	Ellipse(double a, double b, double c, double d, double e, double f);
	Ellipse(Circle<Inexact> c)
	    : A(1), B(0), C(1), D(-2 * c.center().x()), E(-2 * c.center().y()),
	      F(c.center().x() * c.center().x() + c.center().y() * c.center().y() - c.squared_radius()) {}

	double angle() const;
	Point<Inexact> center() const;
	std::array<double, 6> coefficients() const { return { A, B, C, D, E, F }; }
	Ellipse contour(double c) const { return { A, B, C, D, E, F - c }; }
	double evaluate(double x, double y) const { return (A*x + B*y + D) * x + (C*y + E) * y + F; }
	double evaluate(const Point<Inexact> &p) const { return evaluate(p.x(), p.y()); }
	/// Returns a new ellipse where coefficients \c A and \c C are positive.
	Ellipse normalizeSign() const;
	virtual Parameters parameters() const;
	Ellipse stretch(double cx, double cy) const;
	Ellipse stretch(double c) const { return stretch(c, c); }
	Ellipse translate(double dx, double dy) const;
	Ellipse translate(const Vector<Inexact> &v) const { return translate(v.x(), v.y()); }
	Ellipse translateTo(double x, double y) const;
	Ellipse translateTo(const Point<Inexact> &p) const { return translateTo(p.x(), p.y()); }
	EllipseAtOrigin translateToOrigin() const;

	friend std::ostream& operator<<(std::ostream &os, const Ellipse &e);
	std::string toString(int precision = 3) const;

};

/// \author Gijs Pennings
class EllipseAtOrigin : public Ellipse {
  public:
	EllipseAtOrigin() : Ellipse() {}
	EllipseAtOrigin(double a, double b, double c, double f) : Ellipse(a, b, c, 0, 0, f) {}

	double area() const;
	/// Scales the coefficients such that adding 1 to \c F increases the area by \c deltaArea.
	EllipseAtOrigin normalizeContours(double deltaArea = 1) const;
	Parameters parameters() const override;
	/// Let ℓ be the line with slope \c slope through the origin. This function computes the length
	/// of the part of ℓ that is contained in the ellipse.
	double radius(double slope) const;
	EllipseAtOrigin scaleTo(double area) const;
};

} // namespace cartocrow
