//
//  cube.h
//  [Mac] Minecraft
//
//  Created by Jakub Powierza on 24.01.2014.
//  Copyright (c) 2014 Jakub Powierza. All rights reserved.
//

#ifndef _Mac__Minecraft_cube_h
#define _Mac__Minecraft_cube_h

#include <iostream>

// Cube
struct cube {
public:
    short int type;
    int frontFace;
    int backFace;
    int leftFace;
    int rightFace;
    int topFace;
    int bottomFace;
    cube ()
    {
        frontFace = backFace = leftFace = rightFace = topFace = bottomFace = -1;
        type = 0;
    }
};

#endif
