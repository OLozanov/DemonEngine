#include "math\math3d.h"

mat4 mat4::Translate(const vec3 & pos)
{
    return mat4( vec4(1, 0, 0, 0),
                 vec4(0, 1, 0, 0),
                 vec4(0, 0, 1, 0),
                 vec4(pos));
}

//Left-Handed system (DirecX).

mat4 mat4::RotateX(float rad)
{
    return mat4( vec4(1, 0, 0, 0),
                 vec4(0, cos(rad), sin(rad), 0),
                 vec4(0, -sin(rad), cos(rad), 0),
                 vec4(0, 0, 0, 1));
}

mat4 mat4::RotateY(float rad)
{
    return mat4( vec4(cos(rad), 0, -sin(rad), 0),
                 vec4(0, 1, 0, 0),
                 vec4(sin(rad), 0, cos(rad), 0),
                 vec4(0, 0, 0, 1));
}

mat4 mat4::RotateZ(float rad)
{
    return mat4( vec4(cos(rad), sin(rad), 0, 0),
                 vec4(-sin(rad), cos(rad), 0, 0),
                 vec4(0, 0, 1, 0),
                 vec4(0, 0, 0, 1));
}

mat3 mat3::RotateX(float rad)
{
    return mat3(vec3(1, 0, 0),
                vec3(0, cos(rad), sin(rad)),
                vec3(0, -sin(rad), cos(rad)));
}

mat3 mat3::RotateY(float rad)
{
    return mat3(vec3(cos(rad), 0, -sin(rad)),
                vec3(0, 1, 0),
                vec3(sin(rad), 0, cos(rad)));
}

mat3 mat3::RotateZ(float rad)
{
    return mat3(vec3(cos(rad), sin(rad), 0),
                vec3(-sin(rad), cos(rad), 0),
                vec3(0, 0, 1));
}

// Z*Y*X
mat3 mat3::Rotate(float x, float y, float z)
{
    float CosX = cos(x);
    float SinX = sin(x);

    float CosY = cos(y);
    float SinY = sin(y);

    float CosZ = cos(z);
    float SinZ = sin(z);

    return mat3(vec3(CosZ * CosY, SinZ * CosY, -SinY),
                vec3(CosZ * SinY * SinX - SinZ * CosX, SinZ * SinY * SinX + CosZ * CosX, CosY * SinX),
                vec3(CosZ * SinY * CosX + SinZ * SinX, SinZ * SinY * CosX - CosZ * SinX, CosY * CosX));
}

mat4 mat4::Rotate(float x, float y, float z)
{
    float CosX = cos(x);
    float SinX = sin(x);

    float CosY = cos(y);
    float SinY = sin(y);

    float CosZ = cos(z);
    float SinZ = sin(z);

    return mat4(vec4(CosZ*CosY, SinZ*CosY, -SinY, 0),
                vec4(CosZ*SinY*SinX - SinZ*CosX, SinZ*SinY*SinX + CosZ*CosX, CosY*SinX, 0),
                vec4(CosZ*SinY*CosX + SinZ*SinX, SinZ*SinY*CosX - CosZ*SinX, CosY*CosX, 0),
                vec4(0, 0, 0, 1));
}

mat3 mat3::Rotate(const vec3& axis, float ang)
{
    float Cos = cos(ang);
    float Sin = sin(ang);

    return {{axis.x*axis.x*(1 - Cos) + Cos,
            axis.y*axis.x*(1 - Cos) + axis.z*Sin,
            axis.z*axis.x*(1 - Cos) - axis.y*Sin},

            {axis.x*axis.y*(1 - Cos) - axis.z*Sin,
            axis.y*axis.y*(1 - Cos) + Cos,
            axis.z*axis.y*(1 - Cos) + axis.x*Sin},

            {axis.x*axis.z*(1 - Cos) + axis.y*Sin,
            axis.y*axis.z*(1 - Cos) - axis.x*Sin,
            axis.z*axis.z*(1 - Cos) + Cos}};
}

mat3 mat3::SkewSymmetric(const vec3& v)
{
    return { {   0,  v.z, -v.y },
             { -v.z,  0,   v.x },
             {  v.y, -v.x,  0  } };
}

float mat3::minor(int k, int i) const
{
    int i0, i1;
    int k0, k1;

    switch(i)
    {
        case 0:
            i0 = 1; i1 = 2;
        break;

        case 1:
            i0 = 0; i1 = 2;
        break;

        case 2:
            i0 = 0; i1 = 1;
        break;
    }

    switch(k)
    {
        case 0:
            k0 = 1; k1 = 2;
        break;

        case 1:
            k0 = 0; k1 = 2;
        break;

        case 2:
            k0 = 0; k1 = 1;
        break;
    }

    return axis[i0][k0]*axis[i1][k1] - axis[i0][k1]*axis[i1][k0];
}

float mat4::minor(int i, int k) const
{
    static const int index[] = { 1, 2, 3,
                                 0, 2, 3,
                                 0, 1, 3,
                                 0, 1, 2 };
    
    const int* y = index + k * 3;
    const int* x = index + i * 3;
    
    return axis[x[0]][y[0]] * axis[x[1]][y[1]] * axis[x[2]][y[2]]
        + axis[x[0]][y[1]] * axis[x[1]][y[2]] * axis[x[2]][y[0]]
        + axis[x[0]][y[2]] * axis[x[1]][y[0]] * axis[x[2]][y[1]]
        - axis[x[0]][y[2]] * axis[x[1]][y[1]] * axis[x[2]][y[0]]
        - axis[x[0]][y[1]] * axis[x[1]][y[0]] * axis[x[2]][y[2]]
        - axis[x[0]][y[0]] * axis[x[1]][y[2]] * axis[x[2]][y[1]];    
}

mat4 mat4::Rotate(const vec3& axis, float ang)
{
    float Cos = cos(ang);
    float Sin = sin(ang);

    vec4 X = {axis.x*axis.x*(1 - Cos) + Cos,
            axis.y*axis.x*(1 - Cos) + axis.z*Sin,
            axis.z*axis.x*(1 - Cos) - axis.y*Sin,
            0};

    vec4 Y = {axis.x*axis.y*(1 - Cos) - axis.z*Sin,
            axis.y*axis.y*(1 - Cos) + Cos,
            axis.z*axis.y*(1 - Cos) + axis.x*Sin,
            0};

    vec4 Z = {axis.x*axis.z*(1 - Cos) + axis.y*Sin,
            axis.y*axis.z*(1 - Cos) - axis.x*Sin,
            axis.z*axis.z*(1 - Cos) + Cos,
            0};

    vec4 W = {0, 0, 0, 1};

    return mat4(X, Y, Z, W);
}

mat4 mat4::Scale(const vec3 & pos)
{
    return mat4( vec4(pos.x, 0, 0, 0),
                 vec4(0, pos.y, 0, 0),
                 vec4(0, 0, pos.z, 0),
                 vec4(0, 0, 0, 1));
}

mat4 mat4::Scale(float sz)
{
    return mat4( vec4(sz, 0, 0, 0),
                 vec4(0, sz, 0, 0),
                 vec4(0, 0, sz, 0),
                 vec4(0, 0, 0, 1));
}

mat4 mat4::Reflection(const vec4& pl)
{
    return {	{-2*pl.x*pl.x + 1, 	-2*pl.x*pl.y, 		-2*pl.x*pl.z, 		0},
                {-2*pl.y*pl.x, 		-2*pl.y*pl.y + 1, 	-2*pl.y*pl.z, 		0},
                {-2*pl.z*pl.x, 		-2*pl.z*pl.y, 		-2*pl.z*pl.z + 1, 	0},
                {-2*pl.w*pl.x, 		-2*pl.w*pl.y, 		-2*pl.w*pl.z, 		1} };
}

// Left-handed projection matrix
mat4 mat4::Projection(float fov, float aspect, float znear, float zfar)
{
    float fovr = 1.0f/tanf(fov*0.5f*math::pi/180.0f);

    float A = zfar/(zfar - znear);
    float B = -znear*zfar/(zfar - znear);

    return mat4({fovr/aspect, 0, 0, 0},
                {0, fovr, 0, 0},
                {0, 0, A, 1},
                {0, 0, B, 0});
}

mat4 mat4::OrthoProjection(float l, float r, float b, float t, float n, float f)
{
    return {{2.0f/(r-l), 0, 0, 0},
            {0, 2.0f/(t-b), 0, 0},
            {0, 0, 1.0f/(f-n), 0},
            {(r+l)/(l-r), (t+b)/(b-t), n/(n-f), 1}};
}
