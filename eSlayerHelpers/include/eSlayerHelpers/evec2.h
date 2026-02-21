/*
Copyright (c) 2020 Chan Jer Shyan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef VEC2_H
#define VEC2_H

#include <cmath>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif


template <class T>
class eVec2 {
public:
	T x, y;
	
    eVec2() :x(0), y(0) {}
    eVec2(T x, T y) : x(x), y(y) {}
    eVec2(const eVec2& v) : x(v.x), y(v.y) {}
	
    eVec2& operator=(const eVec2& v) {
		x = v.x;
		y = v.y;
		return *this;
	}
	
    eVec2 operator+(const eVec2& v) const {
        return eVec2(x + v.x, y + v.y);
	}

    eVec2 operator-(const eVec2& v) const {
        return eVec2(x - v.x, y - v.y);
	}
	
    eVec2& operator+=(const eVec2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

    eVec2& operator-=(const eVec2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	
    eVec2 operator+(const double s) const {
        return eVec2(x + s, y + s);
	}

    eVec2 operator-(const double s) const {
        return eVec2(x - s, y - s);
	}

    eVec2 operator*(const double s) const {
        return eVec2(x * s, y * s);
	}

    eVec2 operator/(const double s) const {
        return eVec2(x / s, y / s);
	}
	
    eVec2& operator+=(const double s) {
		x += s;
		y += s;
		return *this;
	}

    eVec2& operator-=(const double s) {
		x -= s;
		y -= s;
		return *this;
	}

    eVec2& operator*=(const double s) {
		x *= s;
		y *= s;
		return *this;
	}

    eVec2& operator/=(const double s) {
		x /= s;
		y /= s;
		return *this;
	}
	
    void set(const T x, const T y) {
		this->x = x;
		this->y = y;
	}
	
    void rotate(const double deg) {
		double theta = deg / 180.0 * M_PI;
		double c = cos(theta);
		double s = sin(theta);
		double tx = x * c - y * s;
		double ty = x * s + y * c;
		x = tx;
		y = ty;
	}
	
    eVec2& normalize(const double newLength = 1.0) {
        if(length() == 0) return *this;
        *this *= (newLength / length());
		return *this;
	}
	
    T dist(const eVec2 v) const {
        eVec2 d(v.x - x, v.y - y);
		return d.length();
	}

    T length() const {
		return std::sqrt(x * x + y * y);
	}

    void truncate(const double length) {
		double angle = atan2f(y, x);
		x = length * cos(angle);
		y = length * sin(angle);
	}

    T angle() const { // 0 - 360
        const double radAngle = std::atan2(y, x);
        const double radAngle2 = radAngle < 0 ? 2*M_PI + radAngle : radAngle;
        const double degAngle = radAngle2 * 180 / M_PI;
        return degAngle;
    }
	
    eVec2 ortho() const {
        return eVec2(y, -x);
	}
	
    static T dot(const eVec2& v1, const eVec2& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}

    static T cross(const eVec2& v1, const eVec2& v2) {
		return (v1.x * v2.y) - (v1.y * v2.x);
	}
};

typedef eVec2<float> eVec2f;
typedef eVec2<double> eVec2d;

#endif // VEC2_H
