/*
Copyright (C) 2026  TU Eindhoven

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "ellipse.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numbers>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <CGAL/number_utils.h>

namespace cartocrow {

bool isZero(double x) {
	return std::abs(x) < 1e-10;
}

std::array<std::array<double, 3>, 3> Ellipse::Parameters::matrix() const {
	// multiply matrices left to right: translation, rotation, scaling
	const double cos = std::cos(angle);
	const double sin = std::sin(angle);

	return std::array<std::array<double, 3>, 3> {{
		{cos*a, -sin*b, x0},
		{ sin*a, cos*b, y0},
		{	0, 0, 1}
	}};
}

// TODO: only enable checks in debug builds?
Ellipse::Ellipse(double a, double b, double c, double d, double e, double f)
    : A(a), B(b), C(c), D(d), E(e), F(f) {
	if (!std::isfinite(a) || !std::isfinite(b) || !std::isfinite(c) ||
	    !std::isfinite(d) || !std::isfinite(e) || !std::isfinite(f)) {
		throw std::invalid_argument("The coefficients cannot be infinite or NaN");
	}

	// see Lawrence (1972), page 63
	const double min = 4*A*C - B*B;
	const double det = min * F - A*E*E + (B*E - C*D) * D;
	if (det == 0) {
		throw std::invalid_argument("The conic cannot be degenerate");
	}
	if (min <= 0) {
		throw std::invalid_argument("The conic cannot be a hyperbola or parabola");
	}
	if (A*det >= 0) {
		throw std::invalid_argument("The ellipse cannot be imaginary");
	}
}

double Ellipse::angle() const {
	// substitute  x -> x cos(t) - y sin(t)  and  y -> x sin(t) + y cos(t)
	// and solve for t such that coefficient of xy is 0
	if (isZero(A - C)) return std::numbers::pi / 2;
	return .5 * std::atan(B / (A-C));
}

Point<Inexact> Ellipse::center() const {
	// solve system  { dQ/dx = 0, dQ/dy = 0 }  for  x,y
	const double x = (2*C*D - B*E) / (B*B - 4*A*C);
	return { x, -(B*x + E) / (2*C) };
}

Ellipse Ellipse::normalizeSign() const {
	if (A > 0) return *this;  // note:  sgn(A) = sgn(C)
	return { -A, -B, -C, -D, -E, -F };
}

Ellipse::Parameters Ellipse::parameters() const {
	Parameters p = translateToOrigin().parameters();
	const Point<Inexact> c = center();
	p.x0 = c.x(), p.y0 = c.y();
	return p;
}

Ellipse Ellipse::stretch(double cx, double cy) const {
	// substitute  x -> cx x  and  y -> cy y  and compute new coefficients
	return { A*cx*cx, B*cx*cy, C*cy*cy, D*cx, E*cy, F };
}

Ellipse Ellipse::translate(double dx, double dy) const {
	// substitute  x -> x-dx  and  y -> y-dy  and compute new coefficients
	return {
		A, B, C,
		D - 2*A*dx - B*dy,
		E - 2*C*dy - B*dx,
		(A*dx - D) * dx + (B*dx + C*dy - E) * dy + F
	};
}

Ellipse Ellipse::translateTo(double x, double y) const {
	const Point<Inexact> c = center();
	return translate(x - c.x(), y - c.y());
}

EllipseAtOrigin Ellipse::translateToOrigin() const {
	const Point<Inexact> c = center();
	const double x0 = c.x(), y0 = c.y();
	return {
		A, B, C,
		(A*x0 + D) * x0 + (B*x0 + C*y0 + E) * y0 + F
	};
}

std::ostream& operator<<(std::ostream &os, const Ellipse &e) {
	const std::array<double, 6> coefficients = e.coefficients();
	static const std::vector<std::string> variables = { "x²", "xy", "y²", "x", "y", "" };

	bool first = true;
	for (int i = 0; i < coefficients.size(); i++) {
		double c = coefficients[i];
		const std::string &v = variables[i];

		if (!c) continue;
		if (!first) {
			os << (c > 0 ? " + " : " - ");
			c = std::abs(c);
		}
		if (c != 1 || v.empty()) os << c;
		os << v;

		first = false;
	}
	return os << " = 0";
}

std::string Ellipse::toString(int precision) const {
	std::stringstream ss;
	ss << std::setprecision(precision) << *this;
	return ss.str();
}

double EllipseAtOrigin::area() const {
	// https://math.stackexchange.com/q/2499695
	return 2 * std::numbers::pi * std::abs(F) / std::sqrt(4*A*C - B*B);
}

EllipseAtOrigin EllipseAtOrigin::normalizeContours(double deltaArea) const {
	// let  g(x) = area(Q + x)  and solve  g'(1) = deltaArea  for (scaling factor of) new coefficients
	const double c = 2 * std::numbers::pi / (std::sqrt(4*A*C - B*B) * deltaArea);
	return { c*A, c*B, c*C, c*F };
}

Ellipse::Parameters EllipseAtOrigin::parameters() const {
	Parameters p;
	p.angle = angle();
	p.x0 = p.y0 = 0;

	const double cos = std::cos(p.angle);
	const double sin = std::sin(p.angle);

	// substitute  x -> x cos(t) - y sin(t)  and  y -> x sin(t) + y cos(t)  where  t := angle
	// this eliminates rotation and transforms ellipse into the form  Ax² + Cy² + F = 0
	const double A2 = A*cos*cos + ( B*cos + C*sin) * sin;
	const double C2 = C*cos*cos + (-B*cos + A*sin) * sin;

	p.a = std::sqrt(-F / A2);
	p.b = std::sqrt(-F / C2);

	return p;
}

double EllipseAtOrigin::radius(double slope) const {
	// special case: vertical line intersection
	if (std::isinf(slope)) return std::sqrt(-F / C);

	// substitute  y -> mx  i.e. the linear function with slope m through the origin
	// and rewrite as quadratic equation in x, i.e., ax² + 0x + F, where:
	const double a = A + (B + C*slope) * slope;
	// then  x = \pm\sqrt{-F/a}  so:
	const double xSq = -F / a;
	// return distance from origin to  (x, y) = (x, mx)
	return std::sqrt(xSq * (1 + slope*slope));
}

EllipseAtOrigin EllipseAtOrigin::scaleTo(double area) const {
	// note that area is linear in |F|, so  areaNew / areaOld = |Fnew| / |Fold| = Fnew / Fold
	// since the sign must remain the same
	return { A, B, C, F * area / this->area() };
}

} // namespace cartocrow
