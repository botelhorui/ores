#pragma once
/*
* Represents the coordinates of a pixel in the screen
* the x values grow from left to right of the screen,
* but the y values grow from the top to bottom of the screen
*/
class PixelPoint
{
public:
	PixelPoint(int x, int y): x(x), y(y) {}
	int x = 0;
	int y = 0;
};

class MatrixPoint
{
public:
	MatrixPoint(int i, int j) : i(i), j(j) {}
	int i = 0; // line
	int j = 0; // column
};