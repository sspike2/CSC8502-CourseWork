#pragma once
#include"Vector4.h"

static class Color
{
public:
	const static Vector4 red   () { return Vector4(1, 0, 0, 1); }
	const static Vector4 green () { return Vector4(0, 1, 0, 1); }
	const static Vector4 blue  () { return Vector4(0, 0, 1, 1); }
	 
	const static Vector4 cyan  () { return Vector4(0, 1, 1, 1); }
	const static Vector4 pink  () { return Vector4(1, 0, 1, 1); }
	const static Vector4 yellow() { return Vector4(1, 1, 0, 1); }
	 
	const static Vector4 white () { return Vector4(1, 1, 1, 1); }
	const static Vector4 black () { return Vector4(0, 0, 0, 1); }
	const static Vector4 grey  () { return Vector4(0.5f, 0.5f, 0.5f, 1); }
	 
	const static Vector4 orange() { return Vector4(1, 0.2f, 0, 1); }
	const static Vector4 purple() { return Vector4(.5f, 0, 1, 1); }
	
};