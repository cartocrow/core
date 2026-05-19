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

#include "double_slider.h"

namespace cartocrow {
void DoubleSlider::initialize() {
	m_intSlider->setRange(0, m_precision);
	auto* layout = new QVBoxLayout(this);
	layout->addWidget(m_intSlider);
	layout->setContentsMargins(0, 0, 0, 0);
	setSizePolicy(m_intSlider->sizePolicy());
	connect(m_intSlider, &QSlider::valueChanged, this, &DoubleSlider::handleIntValueChanged);
}

DoubleSlider::DoubleSlider(QWidget* parent) : QWidget(parent) {
	m_intSlider = new QSlider(this);
	initialize();
}

DoubleSlider::DoubleSlider(Qt::Orientation orientation, QWidget* parent) {
	m_intSlider = new QSlider(orientation, this);
	initialize();
}

QSize DoubleSlider::sizeHint() const {
	return m_intSlider->sizeHint();
}

void DoubleSlider::setOrientation(Qt::Orientation orientation) {
	m_intSlider->setOrientation(orientation);
	setSizePolicy(m_intSlider->sizePolicy());
}

void DoubleSlider::setMinimum(double min) {
	m_min = min;
}

void DoubleSlider::setMaximum(double max) {
	m_max = max;
}

void DoubleSlider::setRange(double min, double max) {
	m_min = min;
	m_max = max;
}

/// Set the number of discrete values (+1) that the slider uses.
/// Example: a precision of 1 has 2 discrete steps: the minimum and maximum value.
/// The default precision is 1000.
void DoubleSlider::setPrecision(int precision) {
	double oldValue = value();
	m_precision = precision;
	m_intSlider->setMaximum(precision);
	setValue(oldValue);
}

void DoubleSlider::setValue(double val) {
	int sliderVal = static_cast<int>(((val - m_min) / (m_max - m_min)) * m_precision);
	m_intSlider->setValue(sliderVal);
}

[[nodiscard]] double DoubleSlider::value() const {
	return m_min + (m_intSlider->value() / static_cast<double>(m_precision)) * (m_max - m_min);
}

[[nodiscard]] double DoubleSlider::minimum() const {
	return m_min;
}

[[nodiscard]] double DoubleSlider::maximum() const {
	return m_max;
}

void DoubleSlider::handleIntValueChanged(int) {
	emit valueChanged(value());
}
}