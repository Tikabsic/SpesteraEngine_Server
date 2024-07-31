#pragma once
#include <cmath>

class TDM
{
public:

   static const int gravity_force_;

    struct Vector3 {
        float x, y, z;

        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        Vector3 operator*(const float& scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        void operator+=(const Vector3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
        }

        void operator*=(const Vector3& other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
        }

        void operator/=(const Vector3& other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
        }

        void operator-=(const Vector3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
        }


        void operator*=(const float& scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
        }

        void floor() {
            x = std::floor(x * 100) / 100;
            y = std::floor(y * 100) / 100;
            z = std::floor(z * 100) / 100;
        }
    };

    struct Vector2 {
        float x, z;

        Vector2() : x(0), z(0) {}
        Vector2(float x, float z) : x(x), z(z) {}

        void operator+=(const Vector2& other) {
            x += other.x;
            z += other.z;
        }

        void operator-=(const Vector2& other) {
            x -= other.x;
            z -= other.z;
        }

        void operator*=(const Vector2& other) {
            x *= other.x;
            z *= other.z;
        }

        void operator/=(const Vector2& other) {
            x /= other.x;
            z /= other.z;
        }

        void AddTransform(const Vector3& transform) {
            x += transform.x;
            z += transform.z;
        }
    };

    struct FieldOfView {
        Vector2 tl, tr, br, bl;
        float fov_distance_ = 50;

        FieldOfView() : tl(0, 0), tr(0, 0), br(0, 0), bl(0, 0) {}
        FieldOfView(Vector2 tl, Vector2 tr, Vector2 br, Vector2 bl) : tl(tl), tr(tr), br(br), bl(bl) {}

        FieldOfView(const Vector3& player_transform) {
            tl = Vector2(player_transform.x - fov_distance_, player_transform.z + fov_distance_);
            tr = Vector2(player_transform.x + fov_distance_, player_transform.z + fov_distance_);
            br = Vector2(player_transform.x + fov_distance_, player_transform.z - fov_distance_);
            bl = Vector2(player_transform.x - fov_distance_, player_transform.z - fov_distance_);
        }

        void RepositionFov(const Vector3& transform) {
            tl.AddTransform(transform);
            tr.AddTransform(transform);
            br.AddTransform(transform);
            bl.AddTransform(transform);
        }
    };

    static float customRound(float value) {
        if (value >= 0.0f) {
            return std::floor(value + 0.005f);
        }
        else {
            return std::ceil(value - 0.005f);
        }
    }
};

