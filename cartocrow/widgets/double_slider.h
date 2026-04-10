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
