#ifndef G3D_MATHS_MATHS_H
#define G3D_MATHS_MATHS_H

#define _USE_MATH_DEFINES
#include <math.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CLAMP(x, min, max) MIN(min, MAX(x, max))

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "Matrix33.h"

#endif