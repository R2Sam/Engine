#pragma once

#include "raylib.h"

#include <cmath>

template <typename T>
	requires(std::is_arithmetic_v<T>)
struct Vec2
{
	T x = 0;
	T y = 0;

	constexpr Vec2(const T& x = 0, const T& y = 0) :
	x(x),
	y(y)
	{
	}

	constexpr Vec2 operator+(const Vec2& rhs) const
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}

	constexpr Vec2 operator-(const Vec2& rhs) const
	{
		return Vec2(x - rhs.x, y - rhs.y);
	}

	constexpr Vec2 operator+(const T& scalar) const
	{
		return Vec2(x + scalar, y + scalar);
	}

	constexpr Vec2 operator-(const T& scalar) const
	{
		return Vec2(x - scalar, y - scalar);
	}

	constexpr Vec2 operator*(const T& scalar) const
	{
		return Vec2(x * scalar, y * scalar);
	}

	friend constexpr Vec2 operator*(const T scalar, const Vec2& lhs)
	{
		return Vec2(lhs.x * scalar, lhs.y * scalar);
	}

	constexpr Vec2 operator/(const T& scalar) const
	{
		return Vec2(x / scalar, y / scalar);
	}

	constexpr Vec2 operator-() const
	{
		return Vec2(-x, -y);
	}

	constexpr Vec2& operator+=(const Vec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	constexpr Vec2& operator-=(const Vec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	constexpr Vec2& operator+=(const T& scalar)
	{
		x += scalar;
		y += scalar;
		return *this;
	}

	constexpr Vec2& operator-=(const T& scalar)
	{
		x -= scalar;
		y -= scalar;
		return *this;
	}

	constexpr Vec2& operator*=(const T& scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	constexpr Vec2& operator/=(const T& scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	constexpr Vector2 Raylib() const
	{
		return Vector2{static_cast<float>(x), static_cast<float>(y)};
	}

	constexpr T Dot(const Vec2& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	constexpr Vec2 Cross(const Vec2& rhs) const
	{
		return Vec2(x * rhs.y - y * rhs.x, y * rhs.x - x * rhs.y);
	}

	constexpr T Length() const
	{
		return std::sqrt(x * x + y * y);
	}

	constexpr T LengthSqr() const
	{
		return x * x + y * y;
	}

	constexpr T Distance(const Vec2& other) const
	{
		T dx = other.x - x;
		T dy = other.y - y;
		return std::sqrt(dx * dx + dy * dy);
	}

	constexpr Vec2& Normalize()
	{
		T len = Length();
		if (len > 0)
		{
			x /= len;
			y /= len;
		}
		return *this;
	}
};

template <typename T>
	requires(std::is_arithmetic_v<T>)
struct Vec3
{
	T x = 0;
	T y = 0;
	T z = 0;

	constexpr Vec3(const T& x = 0, const T& y = 0, const T& z = 0) :
	x(x),
	y(y),
	z(z)
	{
	}

	constexpr Vec3 operator+(const Vec3& rhs) const
	{
		return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	constexpr Vec3 operator-(const Vec3& rhs) const
	{
		return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	constexpr Vec3 operator+(const T& scalar) const
	{
		return Vec3(x + scalar, y + scalar, z + scalar);
	}

	constexpr Vec3 operator-(const T& scalar) const
	{
		return Vec3(x - scalar, y - scalar, z - scalar);
	}

	constexpr Vec3 operator*(const T& scalar) const
	{
		return Vec3(x * scalar, y * scalar, z * scalar);
	}

	friend constexpr Vec3 operator*(const T scalar, const Vec3& lhs)
	{
		return Vec3(lhs.x * scalar, lhs.y * scalar, lhs.z * scalar);
	}

	constexpr Vec3 operator/(const T& scalar) const
	{
		return Vec3(x / scalar, y / scalar, z / scalar);
	}

	constexpr Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	constexpr bool operator==(const Vec3& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	constexpr bool operator!=(const Vec3& rhs) const
	{
		return x != rhs.x || y != rhs.y || z != rhs.z;
	}

	constexpr Vec3& operator+=(const Vec3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	constexpr Vec3& operator-=(const Vec3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	constexpr Vec3& operator+=(const T& scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;
		return *this;
	}

	constexpr Vec3& operator-=(const T& scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;
		return *this;
	}

	constexpr Vec3& operator*=(const T& scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	constexpr Vec3& operator/=(const T& scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	constexpr Vector3 Raylib() const
	{
		return Vector3{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
	}

	constexpr T Dot(const Vec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	constexpr Vec3 Cross(const Vec3& rhs) const
	{
		return Vec3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	constexpr T Length() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	constexpr T LengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	constexpr T Distance(const Vec3& other) const
	{
		T dx = other.x - x;
		T dy = other.y - y;
		T dz = other.z - z;
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	constexpr Vec3& Normalize()
	{
		T len = Length();
		if (len > 0)
		{
			x /= len;
			y /= len;
			z /= len;
		}
		return *this;
	}
};