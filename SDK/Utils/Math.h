#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Vector2D {
  float x, y;
  Vector2D(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Vector3D {
  float x, y, z;
  Vector3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

extern const unsigned char COLOR_WHITE[3];
extern const unsigned char COLOR_GRAY[3];
extern const unsigned char COLOR_GREEN[3];
extern const unsigned char COLOR_RED[3];
extern const unsigned char COLOR_YELLOW[3];
extern const unsigned char COLOR_MERO[3];

extern const int FONT_HEIGHT;
