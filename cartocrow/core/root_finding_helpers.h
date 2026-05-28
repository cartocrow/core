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

#pragma once

#include "core.h"

namespace cartocrow {
template <class OutputIterator>
void getCubicRoots(double a, double b, double c, double d, OutputIterator out, bool filterWithin01) {
	auto output = [&](double root) {
		if (!filterWithin01 || (root >= 0 && root <= 1)) {
			*out++ = root;
		}
	};

	// Degenerate cases
	if (std::abs(a) < M_EPSILON) {
		// Quadratic: b t^2 + c t + d = 0
		if (std::abs(b) < M_EPSILON) {
			if (std::abs(c) < M_EPSILON) return; // no solution
			output(-d/c); // linear
			return;
		}
		double disc = c*c - 4*b*d;
		if (disc < 0) return;
		double s = sqrt(disc);
		output((-c+s)/(2*b));
		output((-c-s)/(2*b));
		return;
	}

	// Normalize to monic
	double A = b/a;
	double B = c/a;
	double C = d/a;

	// Depressed cubic
	double p = B - A*A/3.0;
	double q = 2*A*A*A/27.0 - A*B/3.0 + C;
	double disc = q*q/4.0 + p*p*p/27.0;

	if (disc > M_EPSILON) {
		// One real root
		double sqrtDisc = std::sqrt(disc);
		double u = std::cbrt(-q/2.0 + sqrtDisc);
		double v = std::cbrt(-q/2.0 - sqrtDisc);
		output(u+v - A/3.0);
	} else if (std::abs(disc) < M_EPSILON) {
		// Double root
		double u = std::cbrt(-q/2.0);
		output(2*u - A/3.0);
		output(-u - A/3.0);
	} else {
		// Three real roots
		double r = std::sqrt(-p/3.0);
		double phi = std::acos(-q/(2*r*r*r));
		output(2*r*std::cos(phi/3.0) - A/3.0);
		output(2*r*std::cos((phi+two_pi)/3.0) - A/3.0);
		output(2*r*std::cos((phi+2*two_pi)/3.0) - A/3.0);
	}
}

template <class OutputIterator>
void getQuadraticRoots(double a, double b, double c, OutputIterator out, bool filterWithin01) {
	auto output = [&](double root) {
		if (!filterWithin01 || (root >= 0 && root <= 1)) {
			*out++ = root;
		}
	};

	// Degenerate cases
	if (std::abs(a) < M_EPSILON) {
		// Linear: b t + c = 0
		if (std::abs(b) < M_EPSILON) return;
		output(-c / b);
		return;
	}

	double disc = b * b - 4 * a * c;
	if (disc < 0) return; // no real roots

	double s = std::sqrt(disc);
	double denom = 2 * a;

	output((-b + s) / denom);
	output((-b - s) / denom);
}
}