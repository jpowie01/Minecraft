//
//  vertex.h
//  [Mac] Minecraft
//
//  Created by Jakub Powierza on 24.01.2014.
//  Copyright (c) 2014 Jakub Powierza. All rights reserved.
//

#ifndef _Mac__Minecraft_vertex_h
#define _Mac__Minecraft_vertex_h

#include <iostream>

// Vertex array
struct vertex {
public:
    float tex[2];
    float vec[3];
    vertex (float x, float y, float z, float a, float b)
    {
        tex[0] = a; tex[1] = b;
        vec[0] = x; vec[1] = y; vec[2] = z;
    }
};

#endif
