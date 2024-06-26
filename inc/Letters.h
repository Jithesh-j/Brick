// Letters.h
// Copyright (c) 2024 Jules Bloomenthal, all rights reserved. Commercial use requires license.

#ifndef LETTERS_HDR
#define LETTERS_HDR

#include "VecMat.h"

void Letters(int x, int y, const char *s, vec3 color, float ptSize);
void Letters(vec3 p, mat4 m, const char *s, vec3 color, float ptSize);

// s is any string but only letters, numerals, space, period, or dash, plus-sign, or slash are printed

#endif
