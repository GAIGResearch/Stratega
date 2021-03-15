#pragma once
#include <iostream> // TODO: Without this include struct hash causes compile problems, why?
#include <cmath>
#include <algorithm>

namespace SGA
{
	struct Vector2i
	{
	public:
		Vector2i();
		Vector2i(int x, int y);
		Vector2i(int value);

		Vector2i operator- (const Vector2i& v) const { return Vector2i(x - v.x, y - v.y); }
		Vector2i operator+ (const Vector2i& v) const { return Vector2i(x + v.x, y + v.y); }
		bool operator==(const Vector2i& other) const;
		bool operator!=(const Vector2i& other) const;
		double distance(const Vector2i& other) const { int dx = other.x - x; int dy = other.y - y; return std::sqrt(dx * dx + dy * dy); }
		int manhattanDistance(const Vector2i& other) const { return std::abs(other.x - x) + std::abs(other.y - y); }
		int chebyshevDistance(const Vector2i& other) const { int dx = other.x - x; int dy = other.y - y; return std::max(std::abs(dx), std::abs(dy)); }

		int x;
		int y;
	};
	
	inline Vector2i::Vector2i()
	{
		x = 0;
		y = 0;
	}
	
	inline Vector2i::Vector2i(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	inline Vector2i::Vector2i(int value)
	{
		x = value;
		y = value;
	}
	
	inline bool Vector2i::operator==(const Vector2i& other) const {
		return x == other.x && y == other.y;
	}
	
	inline bool Vector2i::operator!=(const Vector2i& other) const {
		return !(*this == other);
	}

	struct Vector2f
	{
	public:
		Vector2f();
		Vector2f(float x, float y);
		Vector2f(const Vector2i& pos) : Vector2f(pos.x, pos.y) {}
		Vector2f(float value);

		template<typename Value>
		Vector2f operator* (Value v) const { return Vector2f(x * v, y * v); }
		template<typename Value>
		Vector2f operator/ (Value v) const { return Vector2f(x / v, y / v); }
		Vector2f operator- (const Vector2f& v) const { return Vector2f(x - v.x, y - v.y); }
		Vector2f operator+ (const Vector2f& v) const { return Vector2f(x + v.x, y + v.y); }
		bool operator==(const Vector2f& other) const;
		bool operator!=(const Vector2f& other) const;

		Vector2f normalized() const
		{
			auto magnitudeValue = magnitude();
			
			//Check if result is different to 0
			if (magnitudeValue == 0) return *this;
			return (*this) / magnitudeValue;
		}
		double distance(const Vector2f& other) const { float dx = other.x - x; float dy = other.y - y; return std::sqrt(dx * dx + dy * dy); }
		float manhattanDistance(const Vector2f& other) const { return std::abs(other.x - x) + std::abs(other.y - y); }
		float chebyshevDistance(const Vector2f& other) const { float dx = other.x - x; float dy = other.y - y; return std::max(std::abs(dx), std::abs(dy)); }


		double magnitude() const { return std::sqrt(x * x + y * y); }
		
		float x;
		float y;
	};

	inline Vector2f::Vector2f()
	{
		x = 0;
		y = 0;
	}

	inline Vector2f::Vector2f(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	inline Vector2f::Vector2f(float value)
	{
		x = value;
		y = value;
	}

	inline bool Vector2f::operator==(const Vector2f& other) const {
		return x == other.x && y == other.y;
	}

	inline bool Vector2f::operator!=(const Vector2f& other) const {
		return !(*this == other);
	}
}

namespace std
{

	template <>
	struct hash<SGA::Vector2i>
	{
		std::size_t operator()(const SGA::Vector2i& v) const
		{
			using std::hash;
			// Not the best hashcode, but we do not expect large values
			return v.x << 15 | v.y;
		}
	};

}