#pragma once

struct ColorNorm;
struct ColorRGB {
	float r, g, b, a;

	ColorNorm normalized() const;
};

struct ColorNorm {
	float r, g, b, a;
};




inline ColorNorm ColorRGB::normalized() const {
	return ColorNorm{
		r / 255.0f,
		g / 255.0f,
		b / 255.0f,
		a / 255.0f
	};
}