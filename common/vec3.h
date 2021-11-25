/*
 * 
 * Based on vec2.h - copyright below
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

* This implementation of vector3 is likely missing some transformations.
*/



#ifndef INC_1964_INPUT_JOYSHOCKCPP_VEC3_H
#define INC_1964_INPUT_JOYSHOCKCPP_VEC3_H

#include <cmath>


template <class T>
class vec3 {
public:
    T x, y, z;

    vec3() :x(0), y(0), z(0) {}
    vec3(T x, T y, T z) : x(x), y(y), z(z) {}
    vec3(const vec3& v) : x(v.x), y(v.y), z(v.z) {}

    vec3& operator=(const vec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    vec3 operator+(vec3& v) {
        return vec3(x + v.x, y + v.y, z + v.z);
    }
    vec3 operator-(vec3& v) {
        return vec3(x - v.x, y - v.y, z - v.z);
    }

    vec3& operator+=(vec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    vec3& operator-=(vec3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    vec3 operator+(double s) {
        return vec3(x + s, y + s, z + s);
    }

    vec3 operator-(double s) {
        return vec3(x - s, y - s, z - s);
    }
    vec3 operator*(double s) {
        return vec3(x * s, y * s, z * s);
    }
    vec3 operator/(double s) {
        return vec3(x / s, y / s, z / s);
    }

    vec3& operator+=(double s) {
        x += s;
        y += s;
        z += s;
        return *this;
    }
    vec3& operator-=(double s) {
        x -= s;
        y -= s;
        z -= s;
        return *this;
    }
    vec3& operator*=(double s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    vec3& operator/=(double s) {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    void set(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }



    vec3& normalize() {
        if (length() == 0) return *this;
        *this *= (1.0 / length());
        return *this;
    }

    float dist(vec3 v) const {
        vec3 d(v.x - x, v.y - y, v.z - z);
        return d.length();
    }
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    static float dot(vec3 v1, vec3 v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    // https://en.wikipedia.org/wiki/Cross_product#Computing
    static vec3 cross(vec3 v1, vec3 v2) {
        return vec3(v1.y * v2.z - v1.z * v2.y,
                    v1.z * v2.x - v1.x * v2.z,
                    v1.x * v2.y - v1.y * v2.x);
    }

    // based off of three dimensional basic rotation tables from Wikipedia: https://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations

    void rotatex(double deg) {
        double theta = dev / 180.0 * PI;
        double c = cos(theta);
        double s = sin(theta);
        double ty = y * c - z * s;
        double tz = y * s + z * c;
        y = ty;
        z = tz;
    }

    void rotatey(double deg) {
        double theta = dev / 180.0 * PI;
        double c = cos(theta);
        double s = sin(theta);
        double tx = x * c + z * s;
        double tz = -x * s + z * c;
        x = tx;
        z = tz;
    }

    void rotatez(double deg) {
        double theta = dev / 180.0 * PI;
        double c = cos(theta);
        double s = sin(theta);
        double tx = x * c - y * s;
        double ty = x * s + y * c;
        x = tx;
        y = tz;
    }

};

typedef vec3<float> vec3f;
typedef vec3<double> vec3d;

#endif //INC_1964_INPUT_JOYSHOCKCPP_vec3_H
