//
//  chunk.cpp
//  [Mac] Minecraft
//
//  Created by Jakub Powierza on 24.01.2014.
//  Copyright (c) 2014 Jakub Powierza. All rights reserved.
//

#include "chunk.h"

int maxChunkX;
int maxChunkY;
vector < vector<int> > chunksId;
vector <chunk> chunks;

int chunkAtXY (int x, int y) {
    for (int i = 0; i < chunks.size(); i++)
    {
        if (chunks[i].x == x && chunks[i].y == y)
            return i;
    }
    return 0;
}