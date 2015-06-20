//
//  perlin.h
//  [Mac] Minecraft
//
//  Created by Jakub Powierza on 24.01.2014.
//  Copyright (c) 2014 Jakub Powierza. All rights reserved.
//

#ifndef ___Mac__Minecraft__perlin__
#define ___Mac__Minecraft__perlin__

#include <iostream>
#include <cmath>

#define SAMPLE_SIZE 1024

class Perlin
{
public:
    
    Perlin(int octaves,float freq,float amp,int seed);
    
    float Get2D(float x,float y)
    {
        float vec[2];
        vec[0] = x;
        vec[1] = y;
        return perlin_noise_2D(vec);
    };
    
    float Get3D(float x,float y,float z)
    {
        float vec[3];
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        return perlin_noise_3D(vec);
    };
    
private:
    void init_perlin(int n,float p);
    float perlin_noise_2D(float vec[2]);
    float perlin_noise_3D(float vec[3]);
    
    float noise1(float arg);
    float noise2(float vec[2]);
    float noise3(float vec[3]);
    void normalize2(float v[2]);
    void normalize3(float v[3]);
    void init(void);
    
    int   mOctaves;
    float mFrequency;
    float mAmplitude;
    int   mSeed;
    
    int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
    float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
    float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
    float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
    bool  mStart;
    
};

#define B SAMPLE_SIZE
#define BM (SAMPLE_SIZE-1)

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )
#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
t = vec[i] + N;\
b0 = ((int)t) & BM;\
b1 = (b0+1) & BM;\
r0 = t - (int)t;\
r1 = r0 - 1.0f;

#endif /* defined(___Mac__Minecraft__perlin__) */