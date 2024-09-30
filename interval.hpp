#ifndef INTERVAL_H
#define INTERVAL_H

class Interval {
public:
	double m_Min, m_Max;

	Interval() : m_Min{ +infinity }, m_Max{ -infinity } {} // Default interval is empty

	Interval(double min, double max) : m_Min(min), m_Max{ max } {}

	double size() const {
		return m_Max - m_Min;
	}

	bool contains(double x) const {
		return m_Min <= x && x <= m_Max;
	}

	bool surrounds( double x ) const {
		// Not on edge
		return m_Min < x && x < m_Max;
	}

	double clamp(double x) const {
		if (x < m_Min) return m_Min;
		if (x > m_Max) return m_Max;
		return x;
	}

	static const Interval empty, universe;
};

const Interval Interval::empty = Interval{ +infinity, -infinity };
const Interval Interval::universe = Interval{ -infinity, +infinity };

#endif // !INTERVAL_H
