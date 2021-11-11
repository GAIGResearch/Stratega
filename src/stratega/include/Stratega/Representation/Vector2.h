#pragma once
#include <iostream> // TODO: Without this include struct hash causes compile problems, why?
#include <cmath>
#include <algorithm>

namespace SGA
{
	struct Vector2i
	{
	public:
		int x;
		int y;

		Vector2i();
		Vector2i(int newX, int newY);
		Vector2i(int value);

		Vector2i operator- (const Vector2i& v) const { return Vector2i(x - v.x, y - v.y); }
		Vector2i operator+ (const Vector2i& v) const { return Vector2i(x + v.x, y + v.y); }
		bool operator==(const Vector2i& other) const;
		bool operator!=(const Vector2i& other) const;
		double distance(const Vector2i& other) const { int dx = other.x - x; int dy = other.y - y; return std::sqrt(dx * dx + dy * dy); }
		int manhattanDistance(const Vector2i& other) const { return std::abs(other.x - x) + std::abs(other.y - y); }
		int chebyshevDistance(const Vector2i& other) const { int dx = other.x - x; int dy = other.y - y; return std::max(std::abs(dx), std::abs(dy)); }
	};
	
	inline Vector2i::Vector2i()
	{
		x = 0;
		y = 0;
	}
	
	inline Vector2i::Vector2i(int newX, int newY)
	{
		this->x = newX;
		this->y = newY;
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
		Vector2f(double newX, double newY);
		explicit Vector2f(double value);
		explicit Vector2f(Vector2i v);

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
			const auto magnitudeValue = magnitude();
			
			//Check if result is different to 0
			if (magnitudeValue <= 0) return *this;
			return (*this) / magnitudeValue;
		}
		double distance(const Vector2f& other) const { auto dx = other.x - x; auto dy = other.y - y; return std::sqrt(dx * dx + dy * dy); }
		double manhattanDistance(const Vector2f& other) const { return std::abs(other.x - x) + std::abs(other.y - y); }
		double chebyshevDistance(const Vector2f& other) const { auto dx = other.x - x; auto dy = other.y - y; return std::max(std::abs(dx), std::abs(dy)); }


		double magnitude() const { return std::sqrt(x * x + y * y); }
		
		double x;
		double y;
	};

	inline Vector2f::Vector2f()
	{
		x = 0;
		y = 0;
	}

	inline Vector2f::Vector2f(double newX, double newY)
	{
		this->x = newX;
		this->y = newY;
	}

	inline Vector2f::Vector2f(double value)
	{
		x = value;
		y = value;
	}

	inline Vector2f::Vector2f(Vector2i v)
		: x(v.x), y(v.y)
	{
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
		std::size_t operator()(const SGA::Vector2i& v) const noexcept
		{
			using std::hash;
			// Not the best hashcode, but we do not expect large values
			return static_cast<size_t>(v.x << 15 | v.y);
		}
	};

}