//
//  chunk.h
//  [Mac] Minecraft
//
//  Created by Jakub Powierza on 24.01.2014.
//  Copyright (c) 2014 Jakub Powierza. All rights reserved.
//

#ifndef ___Mac__Minecraft__chunk__
#define ___Mac__Minecraft__chunk__

#include <iostream>
#include <vector>
#include "vertex.h"
#include "cube.h"

using namespace std;

struct chunk {
public:
    int x;
    int y;
    float multiple;
    cube cubes[16][16][256];
    vector <vertex> facesToDraw;
};

extern int maxChunkX;
extern int maxChunkY;
extern vector < vector<int> > chunksId;
extern vector <chunk> chunks;

int chunkAtXY (int x, int y);

#endif /* defined(___Mac__Minecraft__chunk__) */
