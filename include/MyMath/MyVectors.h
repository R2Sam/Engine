#pragma once

#include "Raylib/raylib.h"

#include <cmath>

// Vector2 made out of doubles
struct Vector2d
{
    double x, y;

	constexpr Vector2d(const double& x = 0, const double& y = 0) : x(x), y(y) {}

	constexpr Vector2d operator+(const Vector2d& rhs) const
	{
	    return Vector2d(x + rhs.x, y + rhs.y);
	}

	constexpr Vector2d operator-(const Vector2d& rhs) const
	{
	    return Vector2d(x - rhs.x, y - rhs.y);
	}

	constexpr Vector2d operator+(const double& scalar) const
	{
	    return Vector2d(x + scalar, y + scalar);
	}

	constexpr Vector2d operator-(const double& scalar) const 
	{
	    return Vector2d(x - scalar, y - scalar);
	}

	constexpr Vector2d operator*(const Vector2d& rhs) const
	{
	    return Vector2d(x * rhs.x, y * rhs.y);
	}

	constexpr Vector2d operator*(const double& scalar) const
	{
	    return Vector2d(x * scalar, y * scalar);
	}

	friend constexpr Vector2d operator*(const double scalar, const Vector2d& lhs)
	{
        return Vector2d(lhs.x * scalar, lhs.y * scalar);
    }

	constexpr Vector2d operator/(const double& scalar) const
	{
	    return Vector2d(x / scalar, y / scalar);
	}

	constexpr Vector2d operator-() const
	{
	    return Vector2d(-x, -y);
	}

	constexpr Vector2d& operator+=(const Vector2d& rhs)
	{
	    x += rhs.x;
	    y += rhs.y;
	    return *this;
	}

	constexpr Vector2d& operator-=(const Vector2d& rhs)
	{
	    x -= rhs.x;
	    y -= rhs.y;
	    return *this;
	}

	constexpr Vector2d& operator+=(const double& scalar)
	{
	    x += scalar;
	    y += scalar;
	    return *this;
	}

	constexpr Vector2d& operator-=(const double& scalar)
	{
	    x -= scalar;
	    y -= scalar;
	    return *this;
	}

	constexpr Vector2d& operator*=(const Vector2d& rhs)
	{
	    x *= rhs.x;
	    y *= rhs.y;
	    return *this;
	}

	constexpr Vector2d& operator*=(const double& scalar)
	{
	    x *= scalar;
	    y *= scalar;
	    return *this;
	}

	constexpr Vector2d& operator/=(const double& scalar)
	{
	    x /= scalar;
	    y /= scalar;
	    return *this;
	}

	#ifndef NORAYLIB
	constexpr Vector2 vec2() const
	{
		return Vector2{(float)x, (float)y};
	}
	#endif

	constexpr double dot(const Vector2d& rhs) const
	{
	    return x * rhs.x + y * rhs.y;
	}

	constexpr Vector2d cross(const Vector2d& rhs) const
	{
	    return Vector2d(x * rhs.y - y * rhs.x, y * rhs.x - x * rhs.y);
	}

	constexpr double length() const
	{
	    return std::sqrt(x * x + y * y);
	}

	constexpr double lengthSqr() const
	{
	    return x * x + y * y;
	}

	constexpr double distance(const Vector2d& other) const
	{
	    double dx = other.x - x;
	    double dy = other.y - y;
	    return std::sqrt(dx * dx + dy * dy);
	}

	constexpr Vector2d& normalize()
	{
	    double len = length();
	    if (len > 0)
	    {
	        x /= len;
	        y /= len;
	    }
	    return *this;
	}
};

constexpr Vector2d Vector2dZero()
{
    return Vector2d {0, 0};
}


// Vector3 made out of doubles
struct Vector3d
{
    double x, y, z;

	constexpr Vector3d(const double& x = 0, const double& y = 0, const double& z = 0) : x(x), y(y), z(z) {}

	constexpr Vector3d operator+(const Vector3d& rhs) const
	{
	    return Vector3d(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	constexpr Vector3d operator-(const Vector3d& rhs) const
	{
	    return Vector3d(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	constexpr Vector3d operator+(const double& scalar) const
	{
	    return Vector3d(x + scalar, y + scalar, z + scalar);
	}

	constexpr Vector3d operator-(const double& scalar) const 
	{
	    return Vector3d(x - scalar, y - scalar, z - scalar);
	}

	constexpr Vector3d operator*(const Vector3d& rhs) const
	{
	    return Vector3d(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	constexpr Vector3d operator*(const double& scalar) const
	{
	    return Vector3d(x * scalar, y * scalar, z * scalar);
	}

	friend constexpr Vector3d operator*(const double scalar, const Vector3d& lhs)
	{
        return Vector3d(lhs.x * scalar, lhs.y * scalar, lhs.z * scalar);
    }

	constexpr Vector3d operator/(const double& scalar) const
	{
	    return Vector3d(x / scalar, y / scalar, z / scalar);
	}

	constexpr Vector3d operator-() const
	{
	    return Vector3d(-x, -y, -z);
	}

	constexpr bool operator== (const Vector3d& rhs) const
	{
	    if (x == rhs.x && y == rhs.y && z == rhs.z)
	    {
	    	return true;
	    }

	    return false;
	}

	constexpr bool operator!= (const Vector3d& rhs) const
	{
	    if (x != rhs.x || y != rhs.y || z != rhs.z)
	    {
	    	return true;
	    }

	    return false;
	}

	constexpr Vector3d& operator+=(const Vector3d& rhs)
	{
	    x += rhs.x;
	    y += rhs.y;
	    z += rhs.z;
	    return *this;
	}

	constexpr Vector3d& operator-=(const Vector3d& rhs)
	{
	    x -= rhs.x;
	    y -= rhs.y;
	    z -= rhs.z;
	    return *this;
	}

	constexpr Vector3d& operator+=(const double& scalar)
	{
	    x += scalar;
	    y += scalar;
	    z += scalar;
	    return *this;
	}

	constexpr Vector3d& operator-=(const double& scalar)
	{
	    x -= scalar;
	    y -= scalar;
	    z -= scalar;
	    return *this;
	}

	constexpr Vector3d& operator*=(const Vector3d& rhs)
	{
	    x *= rhs.x;
	    y *= rhs.y;
	    z *= rhs.z;
	    return *this;
	}

	constexpr Vector3d& operator*=(const double& scalar)
	{
	    x *= scalar;
	    y *= scalar;
	    z *= scalar;
	    return *this;
	}

	constexpr Vector3d& operator/=(const double& scalar)
	{
	    x /= scalar;
	    y /= scalar;
	    z /= scalar;
	    return *this;
	}

	#ifndef NORAYLIB
	constexpr Vector3 vec3() const
	{
		return Vector3{(float)x, (float)y, (float)z};
	}
	#endif

	constexpr double dot(const Vector3d& rhs) const
	{
	    return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	constexpr Vector3d cross(const Vector3d& rhs) const
	{
	    return Vector3d(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	constexpr double length() const
	{
	    return std::sqrt(x * x + y * y + z * z);
	}

	constexpr double lengthSqr() const
	{
	    return x * x + y * y + z * z;
	}

	constexpr double distance(const Vector3d& other) const
	{
	    double dx = other.x - x;
	    double dy = other.y - y;
	    double dz = other.z - z;
	    return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	constexpr Vector3d& normalize()
	{
	    double len = length();
	    if (len > 0)
	    {
	        x /= len;
	        y /= len;
	        z /= len;
	    }
	    return *this;
	}
};

constexpr Vector3d Vector3dZero()
{
    return Vector3d {0, 0, 0};
}

// Vector2 made out of float
struct Vector2f
{
    float x, y;

	constexpr Vector2f(const float& x = 0, const float& y = 0) : x(x), y(y) {}

	constexpr Vector2f operator+(const Vector2f& rhs) const
	{
	    return Vector2f(x + rhs.x, y + rhs.y);
	}

	constexpr Vector2f operator-(const Vector2f& rhs) const
	{
	    return Vector2f(x - rhs.x, y - rhs.y);
	}

	constexpr Vector2f operator+(const float& scalar) const
	{
	    return Vector2f(x + scalar, y + scalar);
	}

	constexpr Vector2f operator-(const float& scalar) const 
	{
	    return Vector2f(x - scalar, y - scalar);
	}

	constexpr Vector2f operator*(const Vector2f& rhs) const
	{
	    return Vector2f(x * rhs.x, y * rhs.y);
	}

	constexpr Vector2f operator*(const float& scalar) const
	{
	    return Vector2f(x * scalar, y * scalar);
	}

	friend constexpr Vector2f operator*(const double scalar, const Vector2f& lhs)
	{
        return Vector2f(lhs.x * scalar, lhs.y * scalar);
    }

	constexpr Vector2f operator/(const float& scalar) const
	{
	    return Vector2f(x / scalar, y / scalar);
	}

	constexpr Vector2f operator-() const
	{
	    return Vector2f(-x, -y);
	}

	constexpr Vector2f& operator+=(const Vector2f& rhs)
	{
	    x += rhs.x;
	    y += rhs.y;
	    return *this;
	}

	constexpr Vector2f& operator-=(const Vector2f& rhs)
	{
	    x -= rhs.x;
	    y -= rhs.y;
	    return *this;
	}

	constexpr Vector2f& operator+=(const float& scalar)
	{
	    x += scalar;
	    y += scalar;
	    return *this;
	}

	constexpr Vector2f& operator-=(const float& scalar)
	{
	    x -= scalar;
	    y -= scalar;
	    return *this;
	}

	constexpr Vector2f& operator*=(const Vector2f& rhs)
	{
	    x *= rhs.x;
	    y *= rhs.y;
	    return *this;
	}

	constexpr Vector2f& operator*=(const float& scalar)
	{
	    x *= scalar;
	    y *= scalar;
	    return *this;
	}

	constexpr Vector2f& operator/=(const float& scalar)
	{
	    x /= scalar;
	    y /= scalar;
	    return *this;
	}

	#ifndef NORAYLIB
	constexpr Vector2 vec2() const
	{
		return Vector2{x, y};
	}
	#endif

	constexpr float dot(const Vector2f& rhs) const
	{
	    return x * rhs.x + y * rhs.y;
	}

	constexpr Vector2f cross(const Vector2f& rhs) const
	{
	    return Vector2f(x * rhs.y - y * rhs.x, y * rhs.x - x * rhs.y);
	}

	constexpr float length() const
	{
	    return std::sqrt(x * x + y * y);
	}

	constexpr float lengthSqr() const
	{
	    return x * x + y * y;
	}

	constexpr float distance(const Vector2f& other) const
	{
	    float dx = other.x - x;
	    float dy = other.y - y;
	    return std::sqrt(dx * dx + dy * dy);
	}

	constexpr Vector2f& normalize()
	{
	    float len = length();
	    if (len > 0)
	    {
	        x /= len;
	        y /= len;
	    }
	    return *this;
	}
};

constexpr Vector2f Vector2fZero()
{
    return Vector2f {0, 0};
}

// Vector3 made out of floats
struct Vector3f
{
    float x, y, z;

	constexpr Vector3f(const float& x = 0, const float& y = 0, const float& z = 0) : x(x), y(y), z(z) {}

	constexpr Vector3f operator+(const Vector3f& rhs) const
	{
	    return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	constexpr Vector3f operator-(const Vector3f& rhs) const
	{
	    return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	constexpr Vector3f operator+(const float& scalar) const
	{
	    return Vector3f(x + scalar, y + scalar, z + scalar);
	}

	constexpr Vector3f operator-(const float& scalar) const 
	{
	    return Vector3f(x - scalar, y - scalar, z - scalar);
	}

	constexpr Vector3f operator*(const Vector3f& rhs) const
	{
	    return Vector3f(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	constexpr Vector3f operator*(const float& scalar) const
	{
	    return Vector3f(x * scalar, y * scalar, z * scalar);
	}

	friend constexpr Vector3f operator*(const double scalar, const Vector3f& lhs)
	{
        return Vector3f(lhs.x * scalar, lhs.y * scalar, lhs.z * scalar);
    }

	constexpr Vector3f operator/(const float& scalar) const
	{
	    return Vector3f(x / scalar, y / scalar, z / scalar);
	}

	constexpr Vector3f operator-() const
	{
	    return Vector3f(-x, -y, -z);
	}

	constexpr Vector3f& operator+=(const Vector3f& rhs)
	{
	    x += rhs.x;
	    y += rhs.y;
	    z += rhs.z;
	    return *this;
	}

	constexpr Vector3f& operator-=(const Vector3f& rhs)
	{
	    x -= rhs.x;
	    y -= rhs.y;
	    z -= rhs.z;
	    return *this;
	}

	constexpr Vector3f& operator+=(const float& scalar)
	{
	    x += scalar;
	    y += scalar;
	    z += scalar;
	    return *this;
	}

	constexpr Vector3f& operator-=(const float& scalar)
	{
	    x -= scalar;
	    y -= scalar;
	    z -= scalar;
	    return *this;
	}

	constexpr Vector3f& operator*=(const Vector3f& rhs)
	{
	    x *= rhs.x;
	    y *= rhs.y;
	    z *= rhs.z;
	    return *this;
	}

	constexpr Vector3f& operator*=(const float& scalar)
	{
	    x *= scalar;
	    y *= scalar;
	    z *= scalar;
	    return *this;
	}

	constexpr Vector3f& operator/=(const float& scalar)
	{
	    x /= scalar;
	    y /= scalar;
	    z /= scalar;
	    return *this;
	}

	#ifndef NORAYLIB
	constexpr Vector3 vec3() const
	{
		return Vector3{x, y, z};
	}
	#endif

	constexpr float dot(const Vector3f& rhs) const
	{
	    return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	constexpr Vector3f cross(const Vector3f& rhs) const
	{
	    return Vector3f(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	constexpr float length() const
	{
	    return std::sqrt(x * x + y * y + z * z);
	}

	constexpr float lengthSqr() const
	{
	    return x * x + y * y + z * z;
	}

	constexpr float distance(const Vector3f& other) const
	{
	    float dx = other.x - x;
	    float dy = other.y - y;
	    float dz = other.z - z;
	    return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	constexpr Vector3f& normalize()
	{
	    float len = length();
	    if (len > 0)
	    {
	        x /= len;
	        y /= len;
	        z /= len;
	    }
	    return *this;
	}
};

constexpr Vector3f Vector3fZero()
{
    return Vector3f {0, 0, 0};
}

// Vector2 made out of ints
struct Vector2i
{
    int x, y;

	constexpr Vector2i(const int& x = 0, const int& y = 0) : x(x), y(y) {}

	constexpr Vector2i operator+(const Vector2i& rhs) const
	{
	    return Vector2i(x + rhs.x, y + rhs.y);
	}

	constexpr Vector2i operator-(const Vector2i& rhs) const
	{
	    return Vector2i(x - rhs.x, y - rhs.y);
	}

	constexpr Vector2i operator+(const int& scalar) const
	{
	    return Vector2i(x + scalar, y + scalar);
	}

	constexpr Vector2i operator-(const int& scalar) const 
	{
	    return Vector2i(x - scalar, y - scalar);
	}

	constexpr Vector2i operator*(const Vector2i& rhs) const
	{
	    return Vector2i(x * rhs.x, y * rhs.y);
	}

	constexpr Vector2i operator*(const int& scalar) const
	{
	    return Vector2i(x * scalar, y * scalar);
	}

	constexpr Vector2i operator/(const int& scalar) const
	{
	    return Vector2i(x / scalar, y / scalar);
	}

	constexpr Vector2i operator-() const
	{
	    return Vector2i(-x, -y);
	}

	constexpr Vector2i& operator+=(const Vector2i& rhs)
	{
	    x += rhs.x;
	    y += rhs.y;
	    return *this;
	}

	constexpr Vector2i& operator-=(const Vector2i& rhs)
	{
	    x -= rhs.x;
	    y -= rhs.y;
	    return *this;
	}

	constexpr Vector2i& operator+=(const int& scalar)
	{
	    x += scalar;
	    y += scalar;
	    return *this;
	}

	constexpr Vector2i& operator-=(const int& scalar)
	{
	    x -= scalar;
	    y -= scalar;
	    return *this;
	}

	constexpr Vector2i& operator*=(const Vector2i& rhs)
	{
	    x *= rhs.x;
	    y *= rhs.y;
	    return *this;
	}

	constexpr Vector2i& operator*=(const int& scalar)
	{
	    x *= scalar;
	    y *= scalar;
	    return *this;
	}

	constexpr Vector2i& operator/=(const int& scalar)
	{
	    x /= scalar;
	    y /= scalar;
	    return *this;
	}

	constexpr bool operator>(const Vector2i& other) const
    {
        return (x > other.x) || (y > other.y);
    }

	constexpr bool operator<(const Vector2i& other) const
    {
        return (x < other.x) || (y < other.y);
    }

	#ifndef NORAYLIB
	constexpr Vector2 vec2() const
	{
		return Vector2{x, y};
	}
	#endif

	constexpr int dot(const Vector2i& rhs) const
	{
	    return x * rhs.x + y * rhs.y;
	}

	constexpr Vector2i cross(const Vector2i& rhs) const
	{
	    return Vector2i(x * rhs.y - y * rhs.x, y * rhs.x - x * rhs.y);
	}

	constexpr int length() const
	{
	    return std::sqrt(x * x + y * y);
	}

	constexpr int lengthSqr() const
	{
	    return x * x + y * y;
	}

	constexpr int distance(const Vector2i& other) const
	{
	    int dx = other.x - x;
	    int dy = other.y - y;
	    return std::sqrt(dx * dx + dy * dy);
	}

	constexpr Vector2i& normalize()
	{
	    int len = length();
	    if (len > 0)
	    {
	        x /= len;
	        y /= len;
	    }
	    return *this;
	}
};

constexpr Vector2i Vector2iZero()
{
    return Vector2i {0, 0};
}

// Vector3 made out of ints
struct Vector3i
{
    int x, y, z;

	constexpr Vector3i(const int& x = 0, const int& y = 0, const int& z = 0) : x(x), y(y), z(z) {}

	constexpr Vector3i operator+(const Vector3i& rhs) const
	{
	    return Vector3i(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	constexpr Vector3i operator-(const Vector3i& rhs) const
	{
	    return Vector3i(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	constexpr Vector3i operator+(const int& scalar) const
	{
	    return Vector3i(x + scalar, y + scalar, z + scalar);
	}

	constexpr Vector3i operator-(const int& scalar) const 
	{
	    return Vector3i(x - scalar, y - scalar, z - scalar);
	}

	constexpr Vector3i operator*(const Vector3i& rhs) const
	{
	    return Vector3i(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	constexpr Vector3i operator*(const int& scalar) const
	{
	    return Vector3i(x * scalar, y * scalar, z * scalar);
	}

	constexpr Vector3i operator/(const int& scalar) const
	{
	    return Vector3i(x / scalar, y / scalar, z / scalar);
	}

	constexpr Vector3i operator-() const
	{
	    return Vector3i(-x, -y, -z);
	}

	constexpr Vector3i& operator+=(const Vector3i& rhs)
	{
	    x += rhs.x;
	    y += rhs.y;
	    z += rhs.z;
	    return *this;
	}

	constexpr Vector3i& operator-=(const Vector3i& rhs)
	{
	    x -= rhs.x;
	    y -= rhs.y;
	    z -= rhs.z;
	    return *this;
	}

	constexpr Vector3i& operator+=(const int& scalar)
	{
	    x += scalar;
	    y += scalar;
	    z += scalar;
	    return *this;
	}

	constexpr Vector3i& operator-=(const int& scalar)
	{
	    x -= scalar;
	    y -= scalar;
	    z -= scalar;
	    return *this;
	}

	constexpr Vector3i& operator*=(const Vector3i& rhs)
	{
	    x *= rhs.x;
	    y *= rhs.y;
	    z *= rhs.z;
	    return *this;
	}

	constexpr Vector3i& operator*=(const int& scalar)
	{
	    x *= scalar;
	    y *= scalar;
	    z *= scalar;
	    return *this;
	}

	constexpr Vector3i& operator/=(const int& scalar)
	{
	    x /= scalar;
	    y /= scalar;
	    z /= scalar;
	    return *this;
	}

	#ifndef NORAYLIB
	constexpr Vector3 vec3() const
	{
		return Vector3{x, y, z};
	}
	#endif

	constexpr int dot(const Vector3i& rhs) const
	{
	    return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	constexpr Vector3i cross(const Vector3i& rhs) const
	{
	    return Vector3i(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	constexpr int length() const
	{
	    return std::sqrt(x * x + y * y + z * z);
	}

	constexpr int lengthSqr() const
	{
	    return x * x + y * y + z * z;
	}

	constexpr int distance(const Vector3i& other) const
	{
	    int dx = other.x - x;
	    int dy = other.y - y;
	    int dz = other.z - z;
	    return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	constexpr Vector3i& normalize()
	{
	    int len = length();
	    if (len > 0)
	    {
	        x /= len;
	        y /= len;
	        z /= len;
	    }
	    return *this;
	}
};

constexpr Vector3i Vector3iZero()
{
    return Vector3i {0, 0, 0};
}