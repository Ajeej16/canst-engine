/* date = March 22nd 2021 9:26 pm */

#ifndef MATH_H
#define MATH_H


// TODO(ajeej): Organize functions

//~ Math
inline f32
inverse_sqrt(f32 num)
{
#if 1
    i32 i;
    f32 x2, y;
    
    x2 = num * 0.5f;
    y = num;
    i = *(i32 *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(f32 *)&i;
    y = y * (1.5f - (x2*y*y));
    
    return y;
#else
    
    f32 result = 1/Sqrt(num);
    return result;
    
#endif
}

inline f32
lerp(f32 a, f32 b, f32 t)
{
#if 1
    return a + (b-a)*t;
#else
    return (1-t)*a + t*b;
#endif
}

inline f32
clamp(f32 x, f32 min, f32 max)
{
    f32 result = x;
    
    if(x < min)
        result = min;
    else if (x > max)
        result = max;
    
    return result;
}

inline f32
absolute(f32 x)
{
    f32 result = x;
    
    if(x < 0)
        result = -x;
    
    return result;
}

inline f32
to_radians(f32 degrees)
{
    f32 result = 0.0f;
    result = degrees * PI/180.0f;
    return result;
}

inline f32
to_degrees(f32 radians)
{
    f32 result = 0.0f;
    result = radians * 180.0f/PI;
    return result;
}

inline f32
get_min(f32 a, f32 b)
{
    if(a < b)
        return a;
    else
        return b;
}

inline f32
get_max(f32 a, f32 b)
{
    if(a > b)
        return a;
    else
        return b;
}

inline i32
get_min(i32 a, i32 b)
{
    if(a < b)
        return a;
    else
        return b;
}

inline i32
get_max(i32 a, i32 b)
{
    if(a > b)
        return a;
    else
        return b;
}

inline f32 
get_min(f32 a, f32 b, f32 c)
{
    return get_min(a, get_min(b,c));
}

inline f32 
get_max(f32 a, f32 b, f32 c)
{
    return get_max(a, get_max(b,c));
}

inline f32
floorf32(f32 x)
{
    return floorf(x);
}

inline f32
ceilf32(f32 x)
{
    return ceilf(x);
}

//~ Vectors
union v2_t
{
    
    struct
    {
        f32 x;
        f32 y;
    };
    
    struct
    {
        f32 width;
        f32 height;
    };
    
    f32 e[2];
    
    inline f32& operator[](i32 index)
    {
        return e[index];
    }
    
};


union iv2_t
{
    
    struct
    {
        i32 x;
        i32 y;
    };
    
    struct
    {
        i32 width;
        i32 height;
    };
    
    i32 e[2];
    
    inline i32& operator[](i32 index)
    {
        return e[index];
    }
    
};

union v3_t
{
    
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };
    
    struct
    {
        v2_t xy;
        f32 _z;
    };
    
    struct
    {
        f32 r;
        f32 g;
        f32 b;
    };
    
    struct
    {
        f32 width;
        f32 height;
        f32 depth;
    };
    
    f32 e[3];
    
    inline f32& operator[](i32 index)
    {
        return e[index];
    }
    
};

union v4_t
{
    
    struct
    {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    
    struct
    {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    
    struct
    {
        v3_t xyz;
        f32 _w;
    };
    
    f32 e[4];
    
    inline f32& operator[](i32 index)
    {
        return e[index];
    }
    
};

#define v2(x, y) v2_t{x,y}
#define v3(x, y, z) v3_t{x,y,z}
#define v4(x, y, z, w) v4_t{x,y,z,w}
#define v3_to_v4(xyz, w) v4_t{xyz[0], xyz[1], xyz[2], w}

inline v2_t
operator-(v2_t v)
{
    v2_t result = {};
    u32 i;
    
    for(i = 0;
        i < 2;
        i++)
    {
        result[i] = -v[i];
    }
    
    return result;
}

inline v3_t
operator-(v3_t v)
{
    v3_t result = {};
    u32 i;
    
    for(i = 0;
        i < 3;
        i++)
    {
        result[i] = -v[i];
    }
    
    return result;
}

inline v4_t
operator-(v4_t v)
{
    v4_t result = {};
    u32 i;
    
    for(i = 0;
        i < 4;
        i++)
    {
        result[i] = -v[i];
    }
    
    return result;
}

inline v2_t
operator*(v2_t v, f32 a)
{
    v2_t result = {};
    u32 i;
    
    for(i = 0;
        i < 2;
        i++)
    {
        result[i] = v[i]*a;
    }
    
    return result;
}

inline v2_t
operator/(v2_t v, f32 a)
{
    f32 x = 1/a;
    return v*x;
}

inline v3_t
operator*(v3_t v, f32 a)
{
    v3_t result = {};
    u32 i;
    
    for(i = 0;
        i < 3;
        i++)
    {
        result[i] = v[i]*a;
    }
    
    return result;
}

inline v3_t
operator/(v3_t v, f32 a)
{
    f32 x = 1/a;
    return v*x;
}

inline v4_t
operator*(v4_t v, f32 a)
{
    v4_t result = {};
    u32 i;
    
    for(i = 0;
        i < 4;
        i++)
    {
        result[i] = v[i]*a;
    }
    
    return result;
}

inline v4_t
operator/(v4_t v, f32 a)
{
    f32 x = 1/a;
    return v*x;
}

inline v2_t
operator-(v2_t v, v2_t u)
{
    v2_t result = {};
    u32 i;
    
    for(i = 0;
        i < 2;
        i++)
    {
        result[i] = v[i] - u[i];
    }
    
    return result;
}

inline v2_t
operator+(v2_t v, v2_t u)
{
    v2_t result = {};
    u32 i;
    
    for(i = 0;
        i < 2;
        i++)
    {
        result[i] = v[i] + u[i];
    }
    
    return result;
}

inline v3_t
operator-(v3_t v, v3_t u)
{
    v3_t result = {};
    u32 i;
    
    for(i = 0;
        i < 3;
        i++)
    {
        result[i] = v[i] - u[i];
    }
    
    return result;
}

inline v3_t
operator+(v3_t v, v3_t u)
{
    v3_t result = {};
    u32 i;
    
    for(i = 0;
        i < 3;
        i++)
    {
        result[i] = v[i] + u[i];
    }
    
    return result;
}

inline v4_t
operator-(v4_t v, v4_t u)
{
    v4_t result = {};
    u32 i;
    
    for(i = 0;
        i < 4;
        i++)
    {
        result[i] = v[i] - u[i];
    }
    
    return result;
}

inline v4_t
operator+(v4_t v, v4_t u)
{
    v4_t result = {};
    u32 i;
    
    for(i = 0;
        i < 4;
        i++)
    {
        result[i] = v[i] + u[i];
    }
    
    return result;
}

inline v3_t &
operator-=(v3_t &v, v3_t u)
{
    v = v - u;
    return v;
}

inline v3_t &
operator+=(v3_t &v, v3_t u)
{
    v = v + u;
    return v;
}

inline f32
dot(v4_t a, v4_t b)
{
    f32 result = 0.0f;
    u32 i;
    
    for(i = 0;
        i < 4;
        i++)
    {
        result += a.e[i]*b.e[i];
    }
    
    return result;
}

inline f32
dot(v3_t a, v3_t b)
{
    f32 result = 0.0f;
    u32 i;
    
    for(i = 0;
        i < 3;
        i++)
    {
        result += a.e[i]*b.e[i];
    }
    
    return result;
}

inline v2_t
normalize(v2_t u)
{
    v2_t result = {0};
    f32 length2 = 0;
    u32 i;
    
    for(i = 0;
        i < 2;
        i++)
    {
        f32 e =  u.e[i];
        length2 += e*e;
    }
    
    result = u * inverse_sqrt(length2);
    
    return result;
}

inline v3_t
normalize(v3_t u)
{
    v3_t result = {0};
    f32 length2 = 0;
    u32 i;
    
    for(i = 0;
        i < 3;
        i++)
    {
        f32 e =  u.e[i];
        length2 += e*e;
    }
    
    result = u * inverse_sqrt(length2);
    
    return result;
}

inline v4_t
normalize(v4_t u)
{
    v4_t result = {0};
    f32 length2 = 0;
    u32 i;
    
    for(i = 0;
        i < 3;
        i++)
    {
        f32 e =  u.e[i];
        length2 += e*e;
    }
    
    result = u * inverse_sqrt(length2);
    
    return result;
}

inline f32
cross(v2_t u, v2_t v)
{
    f32 result = {};
    
    result = u[0]*v[1] - u[1]*v[0];
    
    return result;
}

inline v3_t
cross(v3_t u, v3_t v)
{
    v3_t result = {};
    
    result.x = u[1]*v[2] - u[2]*v[1];
    result.y = u[2]*v[0] - u[0]*v[2];
    result.z = u[0]*v[1] - u[1]*v[0];
    
    return result;
}

inline void
swap(v4_t *a, v4_t *b)
{
    v4_t temp = *a;
    *a = *b;
    *b = temp;
}

inline v2_t
get_min(v2_t a, v2_t b)
{
    v2_t result = {};
    u32 i;
    
    for(i = 0;
        i < 2;
        i++)
    {
        result[i] = min(a[i], b[i]);
    }
    
    return result;
}

inline v2_t
get_max(v2_t a, v2_t b)
{
    v2_t result = {};
    u32 i;
    
    for(i = 0;
        i < 2;
        i++)
    {
        result[i] = max(a[i], b[i]);
    }
    
    return result;
}

inline v2_t
get_min(v2_t a, v2_t b, v2_t c)
{
    return get_min(get_min(a, b), c);
}

inline v2_t
get_max(v2_t a, v2_t b, v2_t c)
{
    return get_max(get_max(a, b), c);
}

inline v3_t
lerp(v3_t a, v3_t b, f32 t)
{
    v3_t result = {};
    u32 i;
    
    for(i = 0;
        i < 3;
        i++)
    {
        result[i] = lerp(a[i], b[i], t);
    }
    
    return result;
}

inline v4_t
lerp(v4_t a, v4_t b, f32 t)
{
    v4_t result = {};
    u32 i;
    
    for(i = 0;
        i < 4;
        i++)
    {
        result[i] = lerp(a[i], b[i], t);
    }
    
    return result;
}

//~ Rects
union rect2d_t
{
    struct
    {
        f32 left;
        f32 top;
        f32 right;
        f32 bottom;
    };
    
    struct
    {
        f32 minx;
        f32 miny;
        f32 maxx;
        f32 maxy;
    };
    
    struct
    {
        v2_t min;
        v2_t max;
    };
    
    v2_t ev[2];
    f32 e[4];
    
};

union irect2d_t
{
    struct
    {
        i32 left;
        i32 top;
        i32 right;
        i32 bottom;
    };
    
    struct
    {
        i32 minx;
        i32 miny;
        i32 maxx;
        i32 maxy;
    };
    
    struct
    {
        iv2_t min;
        iv2_t max;
    };
    
    iv2_t ev[2];
    i32 e[4];
    
};


//~ Matrix
union mat3_t
{
    
    struct
    {
        f32 e00;
        f32 e01;
        f32 e02;
        f32 e10;
        f32 e11;
        f32 e12;
        f32 e20;
        f32 e21;
        f32 e22;
    };
    
    struct
    {
        v3_t r1;
        v3_t r2;
        v3_t r3;
    };
    
    v3_t er[3];
    
    f32 e[3][3];
    
    inline f32& operator()(i32 r, i32 c)
    {
        return e[r][c];
    }
    
};

union mat4_t
{
    
    struct
    {
        f32 e00;
        f32 e01;
        f32 e02;
        f32 e03;
        f32 e10;
        f32 e11;
        f32 e12;
        f32 e13;
        f32 e20;
        f32 e21;
        f32 e22;
        f32 e23;
        f32 e30;
        f32 e31;
        f32 e32;
        f32 e33;
    };
    
    struct
    {
        v4_t r1;
        v4_t r2;
        v4_t r3;
        v4_t r4;
    };
    
    v4_t er[4];
    
    f32 e[4][4];
    
    inline f32& operator()(i32 r, i32 c)
    {
        return e[r][c];
    }
    
};

inline mat4_t
identity()
{
    mat4_t result = {};
    u32 i;
    
    for(i = 0;
        i < 4;
        i++)
    {
        result(i, i) = 1;
    }
    
    return result;
}

inline mat4_t
transpose(mat4_t m)
{
    mat4_t result = {};
    u32 y;
    u32 x;
    
    for(y = 0;
        y < 4;
        y++)
    {
        for(x = 0;
            x < 4;
            x++)
        {
            result(x, y) = m(y, x);
        }
    }
    
    return result;
}

inline f32
determinant(mat3_t m)
{
    float a = +m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1));
    float b = -m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0));
    float c = +m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
    return a + b + c;
}

inline f32
minor(mat4_t m, u32 r, u32 c)
{
    mat3_t section = {};
    u32 i;
    u32 j;
    
    for(i = 0;
        i < 3;
        i++)
    {
        for(j = 0;
            j < 3;
            j++)
        {
            u32 row = i < r ? i : i + 1;
            u32 col = j < c ? j : j + 1;
            section(i, j) = m(row, col);
        }
    }
    
    return determinant(section);
}

inline f32
cofactor(mat4_t m, u32 r, u32 c)
{
    f32 sign = !((r+c) & 1) ? 1.0f : -1.0f;
    f32 min = minor(m, r, c);
    return sign * min;
}

inline mat4_t
adjoint(mat4_t m)
{
    mat4_t result = {};
    u32 i;
    u32 j;
    
    for(i = 0;
        i < 4;
        i++)
    {
        for(j = 0;
            j < 4;
            j++)
        {
            result(i, j) = cofactor(m, i, j);
        }
    }
    
    return result;
}

inline mat4_t
inverse_transpose(mat4_t m)
{
    mat4_t result = {}, adj = {};
    f32 determinant = 0, inv_determinant = 0;
    
    adj = adjoint(m);
    
    u32 i;
    u32 j;
    
    for(i = 0;
        i < 4;
        i++)
    {
        determinant += m(0, i) * adj(0, i);
    }
    
    inv_determinant = 1/determinant;
    for(i = 0;
        i < 4;
        i++)
    {
        for(j = 0;
            j < 4;
            j++)
        {
            result(i, j) = adj(i, j) * inv_determinant;
        }
    }
    
    return result;
}

inline mat4_t
inverse(mat4_t m)
{
    return transpose(inverse_transpose(m));
}

inline v4_t
operator*(mat4_t m, v4_t v)
{
    v4_t result = {};
    u32 i;
    
    for(i = 0;
        i < 4;
        i++)
    {
        f32 a = m(i, 0) * v[0];
        f32 b = m(i, 1) * v[1];
        f32 c = m(i, 2) * v[2];
        f32 d = m(i, 3) * v[3];
        result[i] = a + b + c + d;
    }
    
    return result;
}

inline v3_t
operator*(mat4_t m, v3_t v)
{
    v3_t result = {};
    
    v4_t temp = v3_to_v4(v, 1);
    
    temp = m * temp;
    result = temp.xyz;
    
    return result;
}

inline mat4_t
operator*(mat4_t n, mat4_t m)
{
    mat4_t result = {};
    u32 r;
    u32 c;
    u32 e;
    
    for(r = 0;
        r < 4;
        r++)
    {
        for(c = 0;
            c < 4;
            c++)
        {
            for(e = 0;
                e < 4;
                e++)
            {
                result(r, c) += n(r, e)*m(e, c);
            }
        }
    }
    
    return result;
}

inline mat4_t rotate(mat4_t m, f32 angle, v3_t axis)
{
    v3_t n = normalize(axis);
    f32 c = (f32)cos(to_radians(angle));
    f32 s = (f32)sin(to_radians(angle));
    
    mat4_t r = identity();
    
    r(0, 0) = n.x * n.x * (1 - c) + c;
    r(0, 1) = n.y * n.x * (1 - c) - s * n.z;
    r(0, 2) = n.z * n.x * (1 - c) + s * n.y;
    
    r(1, 0) = n.x * n.y * (1 - c) + s * n.z;
    r(1, 1) = n.y * n.y * (1 - c) + c;
    r(1, 2) = n.z * n.y * (1 - c) - s * n.x;
    
    r(2, 0) = n.x * n.z * (1 - c) - s * n.y;
    r(2, 1) = n.y * n.z * (1 - c) + s * n.x;
    r(2, 2) = n.z * n.z * (1 - c) + c;
    
    return r*m;
}

inline mat4_t
translate(mat4_t m, v3_t translation)
{
    mat4_t i = identity();
    u32 r;
    
    for(r = 0;
        r < 3;
        r++)
    {
        i(r, 3) = translation[r];
    }
    
    return i*m;
}
#endif //MATH_H
