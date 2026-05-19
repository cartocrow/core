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

#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

namespace cartocrow {
/// A Qt widget for controlling a double value within a range [min, max].
/// The widget provides a similar interface as QSlider.
class DoubleSlider : public QWidget {
	Q_OBJECT

  private:
	void initialize();

  public:
	DoubleSlider(QWidget* parent = nullptr);
	DoubleSlider(Qt::Orientation orientation, QWidget* parent = nullptr);
	QSize sizeHint() const override;
	void setOrientation(Qt::Orientation orientation);
	void setMinimum(double min);
	void setMaximum(double max);
	void setRange(double min, double max);

	/// Set the number of discrete values (+1) that the slider uses.
	/// Example: a precision of 1 has 2 discrete steps: the minimum and maximum value.
	/// The default precision is 1000.
	void setPrecision(int precision);
	void setValue(double val);
	[[nodiscard]] double value() const;
	[[nodiscard]] double minimum() const;
	[[nodiscard]] double maximum() const;

  signals:
	void valueChanged(double value);

  private slots:
	void handleIntValueChanged(int);

  private:
	double m_min = 0.0;
	double m_max = 1.0;
	QSlider* m_intSlider;
	int m_precision = 1000;
};
}
