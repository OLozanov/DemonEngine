#pragma once

class mat3
{
    vec3 axis[3];

public:

    constexpr mat3() : axis{{1, 0, 0},
                            {0, 1, 0},
                            {0, 0, 1}} {}

    constexpr mat3(const vec3 & x, const vec3 & y, const vec3 & z) : axis{x, y, z} {}

    vec3 & operator[](int i)
    {
        return axis[i];
    }

    const vec3 & operator[](int i) const
    {
        return axis[i];
    }

    mat3 operator +(const mat3& mat)
    {
        return { axis[0] + mat.axis[0],
                 axis[1] + mat.axis[1],
                 axis[2] + mat.axis[2] };
    }

    const mat3& operator+=(const mat3& mat)
    {
        axis[0] += mat.axis[0];
        axis[1] += mat.axis[1];
        axis[2] += mat.axis[2];

        return *this;
    }

    vec3 operator *(const vec3 & vect) const
    {
        vec3 X = axis[0]*vect.x;
        vec3 Y = axis[1]*vect.y;
        vec3 Z = axis[2]*vect.z;

        return X + Y + Z;
    }

    mat3 operator *(const mat3& mat) const
    {
        vec3 xAxis = (*this)*mat[0];
        vec3 yAxis = (*this)*mat[1];
        vec3 zAxis = (*this)*mat[2];

        return mat3(xAxis, yAxis, zAxis);
    }

    operator const float*() const
    {
        return &axis[0][0];
    }

    float minor(int i, int k) const;

    float det() const
    {
        return axis[0][0]*minor(0, 0) - axis[1][0]*minor(1, 0) + axis[2][0]*minor(2, 0);
    }

    mat3 transpose() const
    {
        return mat3({axis[0].x, axis[1].x, axis[2].x},
                    {axis[0].y, axis[1].y, axis[2].y},
                    {axis[0].z, axis[1].z, axis[2].z});
    }

    mat3 inverse() const
    {
        float rdet = 1.0f/det();

        return mat3({minor(0,0)*rdet, -minor(0,1)*rdet, minor(0,2)*rdet},
                    {-minor(1,0)*rdet, minor(1,1)*rdet, -minor(1,2)*rdet},
                    {minor(2,0)*rdet, -minor(2,1)*rdet, minor(2,2)*rdet});
    }

    void orthogonalize()
    {
        axis[1] -= axis[0] * (axis[0] * axis[1]);
        axis[2] -= axis[0] * (axis[0] * axis[2]);
        axis[2] -= axis[1] * (axis[1] * axis[2]);

        axis[0].normalize();
        axis[1].normalize();
        axis[2].normalize();
    }

    static mat3 RotateX(float rad);
    static mat3 RotateY(float rad);
    static mat3 RotateZ(float rad);
    static mat3 Rotate(float x, float y, float z);
    static mat3 Rotate(const vec3& axis, float ang);
    
    static mat3 SkewSymmetric(const vec3& v);
};

inline vec3 operator *(const vec3 & vect, const mat3 & mat)
{
    float x = vect*mat[0];
    float y = vect*mat[1];
    float z = vect*mat[2];

    return vec3(x, y, z);
}

// mat * D * mat.transpose
inline mat3 TransformDiagonal(mat3 mat, vec3 d)
{
    vec3 x = { mat[0][0], mat[1][0], mat[2][0] };
    vec3 y = { mat[0][1], mat[1][1], mat[2][1] };
    vec3 z = { mat[0][2], mat[1][2], mat[2][2] };

    vec3 xd = { mat[0][0] * d.x, mat[1][0] * d.y, mat[2][0] * d.z };
    vec3 yd = { mat[0][1] * d.x, mat[1][1] * d.y, mat[2][1] * d.z };
    vec3 zd = { mat[0][2] * d.x, mat[1][2] * d.y, mat[2][2] * d.z };

    float xxd = x * xd;
    float yyd = y * yd;
    float zzd = z * zd;

    float yxd = y * xd;
    float zxd = z * xd;
    float zyd = z * yd;

    return { {xxd, yxd, zxd},
             {yxd, yyd, zyd},
             {zxd, zyd, zzd} };
}
