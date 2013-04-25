// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once
#include <math.h>

namespace SpaceTriangle
{
	struct vec3
	{
		float x,y,z;

		vec3() : x(0), y(0), z(0) { }

		vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }

		inline vec3& operator+=(float s) {
			x += s;
			y += s;
			z += s;
			return *this;
		}

		inline vec3& operator+=(const vec3 &rhs) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		inline vec3& operator/=(float s) {
			x /= s;
			y /= s;
			z /= s;
			return *this;
		}

		inline void rotate2D(float d)
		{
			float c = ::cos(d);
			float s = ::sin(d);
			x = x * c - y * s;
			y = x * s + y * c;
		}

		inline float length() const {
			sqrt(x*x + y*y + z*z);
		}

		inline void normalize()
		{
			*this /= length();
		}
	};

	inline vec3 operator*(float s, const vec3 &rhs) {
		vec3 r;
		r.x = rhs.x * s;
		r.y = rhs.y * s;
		r.z = rhs.z * s;
		return r;
	}

	inline vec3 operator-(const vec3 &lhs, const vec3 &rhs) {
		vec3 r;
		r.x = lhs.x - rhs.x;
		r.y = lhs.y - rhs.y;
		r.z = lhs.z - rhs.z;
		return r;
	}
}