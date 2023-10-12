#pragma once
#include "../menu.h"
#include "hack.hpp"

inline bool ScreenTransform(const Vector3& in, Vector3& out, view_matrix_t viewmatrix)
{
	const view_matrix_t& w2sMatrix = viewmatrix;

	out.x = w2sMatrix.matrix[0][0] * in.x + w2sMatrix.matrix[0][1] * in.y + w2sMatrix.matrix[0][2] * in.z + w2sMatrix.matrix[0][3];
	out.y = w2sMatrix.matrix[1][0] * in.x + w2sMatrix.matrix[1][1] * in.y + w2sMatrix.matrix[1][2] * in.z + w2sMatrix.matrix[1][3];
	out.z = 0.0f;

	float w = w2sMatrix.matrix[3][0] * in.x + w2sMatrix.matrix[3][1] * in.y + w2sMatrix.matrix[3][2] * in.z + w2sMatrix.matrix[3][3];
	if (w < 0.001f)
	{
		out.x *= 100000;
		out.y *= 100000;
		return false;
	}

	out.x /= w;
	out.y /= w;

	return true;
}

inline bool WorldToScreen(const Vector3& in, Vector3& out, view_matrix_t viewmatrix)
{
	if (ScreenTransform(in, out, viewmatrix))
	{
		int w = ScreenWidth, h = ScreenHeight;

		out.x = (w / 2.0f) + (out.x * w) / 2.0f;
		out.y = (h / 2.0f) - (out.y * h) / 2.0f;

		return true;
	}

	return false;
}
