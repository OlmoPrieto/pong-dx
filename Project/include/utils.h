#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstdint>
#include <cstring>

struct Vec2 {
  float x;
  float y;

  Vec2() : x(0.0f), y(0.0f) {

  }

  Vec2(float x_, float y_) : x(x_), y(y_) {

  }

  Vec2(const Vec2& other) {
    x = other.x; y = other.y;
  }

  Vec2& operator = (const Vec2& other) {
    x = other.x; y = other.y;
    return *this;
  }

  Vec2 operator += (const Vec2& other) {
    x += other.x; y += other.y;
    return *this;
  }

  Vec2 operator *= (const Vec2& other) {
    x *= other.x; y *= other.y;
    return *this;
  }

  Vec2 operator * (const Vec2& other) {
    Vec2 r;
    r.x *= other.x; r.y *= other.y;
    return r;
  }

  Vec2 operator *= (float a) {
    x *= a; y *= a;
    return *this;
  }

  Vec2 operator * (float a) {
    Vec2 r;
    r.x *= a; r.y *= a;
    return r;
  }

};

struct Vec3 {
  float x;
  float y;
  float z;

  Vec3() : x(0.0f), y(0.0f), z(0.0f) {

  }

  Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {

  }

  Vec3(const Vec3& other) {
    x = other.x; y = other.y; z = other.z;
  }

  Vec3& operator = (const Vec3& other) {
    x = other.x; y = other.y; z = other.z;
    return *this;
  }

  Vec3 operator += (const Vec3& other) {
    x += other.x; y += other.y; z += other.z;
    return *this;
  }

  Vec3 operator *= (const Vec3& other) {
    x *= other.x; y *= other.y; z *= other.z;
    return *this;
  }

  Vec3 operator * (const Vec3& other) {
    Vec3 r;
    r.x *= other.x; r.y *= other.y; r.z *= other.z;
    return r;
  }

  Vec3 operator *= (float a) {
    x *= a; y *= a; z *= a;
    return *this;
  }

  Vec3 operator * (float a) {
    Vec3 r;
    r.x *= a; r.y *= a; r.z *= a;
    return r;
  }

};


struct Mat4 {
  float matrix[16];

  Mat4() {
    setIdentity();
  }

  ~Mat4() {

  }

  void setIdentity() {
    memset(matrix, 0, sizeof(float) * 16);
    matrix[0]  = 1.0f;
    matrix[5]  = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
  }

  void operator =(const Mat4& other) {
    matrix[0]  = other.matrix[0];  matrix[1]  = other.matrix[1];  matrix[2]  = other.matrix[2];  matrix[3]  = other.matrix[3];
    matrix[4]  = other.matrix[4];  matrix[5]  = other.matrix[5];  matrix[6]  = other.matrix[6];  matrix[7]  = other.matrix[7];
    matrix[8]  = other.matrix[8];  matrix[9]  = other.matrix[9];  matrix[10] = other.matrix[10]; matrix[11] = other.matrix[11];
    matrix[12] = other.matrix[12]; matrix[13] = other.matrix[13]; matrix[14] = other.matrix[14]; matrix[15] = other.matrix[15];
  }

  Mat4 operator *(const Mat4& other) {
    Mat4 result;
    result.matrix[0]  = matrix[0] * other.matrix[0] + matrix[1] * other.matrix[4] + matrix[2] * other.matrix[8]  + matrix[3] * other.matrix[12];
    result.matrix[1]  = matrix[0] * other.matrix[1] + matrix[1] * other.matrix[5] + matrix[2] * other.matrix[9]  + matrix[3] * other.matrix[13];
    result.matrix[2]  = matrix[0] * other.matrix[2] + matrix[1] * other.matrix[6] + matrix[2] * other.matrix[10] + matrix[3] * other.matrix[14];
    result.matrix[3]  = matrix[0] * other.matrix[3] + matrix[1] * other.matrix[7] + matrix[2] * other.matrix[11] + matrix[3] * other.matrix[15];
  
    result.matrix[4]  = matrix[4] * other.matrix[0] + matrix[5] * other.matrix[4] + matrix[6] * other.matrix[8]  + matrix[7] * other.matrix[12];
    result.matrix[5]  = matrix[4] * other.matrix[1] + matrix[5] * other.matrix[5] + matrix[6] * other.matrix[9]  + matrix[7] * other.matrix[13];
    result.matrix[6]  = matrix[4] * other.matrix[2] + matrix[5] * other.matrix[6] + matrix[6] * other.matrix[10] + matrix[7] * other.matrix[14];
    result.matrix[7]  = matrix[4] * other.matrix[3] + matrix[5] * other.matrix[7] + matrix[6] * other.matrix[11] + matrix[7] * other.matrix[15];
  
    result.matrix[8]  = matrix[8] * other.matrix[0] + matrix[9] * other.matrix[4] + matrix[10] * other.matrix[8]  + matrix[11] * other.matrix[12];
    result.matrix[9]  = matrix[8] * other.matrix[1] + matrix[9] * other.matrix[5] + matrix[10] * other.matrix[9]  + matrix[11] * other.matrix[13];
    result.matrix[10] = matrix[8] * other.matrix[2] + matrix[9] * other.matrix[6] + matrix[10] * other.matrix[10] + matrix[11] * other.matrix[14];
    result.matrix[11] = matrix[8] * other.matrix[3] + matrix[9] * other.matrix[7] + matrix[10] * other.matrix[11] + matrix[11] * other.matrix[15];
  
    result.matrix[12] = matrix[12] * other.matrix[0] + matrix[13] * other.matrix[4] + matrix[14] * other.matrix[8]  + matrix[15] * other.matrix[12];
    result.matrix[13] = matrix[12] * other.matrix[1] + matrix[13] * other.matrix[5] + matrix[14] * other.matrix[9]  + matrix[15] * other.matrix[13];
    result.matrix[14] = matrix[12] * other.matrix[2] + matrix[13] * other.matrix[6] + matrix[14] * other.matrix[10] + matrix[15] * other.matrix[14];
    result.matrix[15] = matrix[12] * other.matrix[3] + matrix[13] * other.matrix[7] + matrix[14] * other.matrix[11] + matrix[15] * other.matrix[15];
  
    return result;
  }

  void operator *=(const Mat4& other) {
    *this = *this * other;
  }

};


#endif // __UTILS_H__