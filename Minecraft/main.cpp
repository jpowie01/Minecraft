#include <iostream>
#include <cmath>
#include <cstdlib>
#include <stdio.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "ResourcePath.hpp"

// Classes
#include "perlin.h"
#include "chunk.h"

using namespace std;

#define chunk(x,y) chunkAtXY(x,y);

// Texture
GLuint stoneTexture;
GLuint dirtTexture;
GLuint waterTexture;

// Perlin
int perlinX = 200;
int perlinY = 200;

// World
int worldX = 1;
int worldY = 1;

// Add Cube
void addCube(int x, int y, int z);

// Calculate faces
void calculateFacesToDraw(chunk* t);

// Calculate water level
void calculateWaterLevel(Perlin perlin2D);
vector <vertex> water;

// Delete faces
void deleteFaces(int a);

// Delete cube
void deleteCube(cube* a);

int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Minecraft", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);
    
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    
    // Normalize
	glEnable(GL_NORMALIZE);
    
    // Blend
	glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Smooth
	glShadeModel(GL_SMOOTH);
    
    // Materials
    glEnable(GL_COLOR_MATERIAL);
    
    // Textures
    sf::Image stoneTextureImage;
    sf::Image dirtTextureImage;
    sf::Image waterTextureImage;
    
    // Stone
    if (!stoneTextureImage.loadFromFile(resourcePath()+"stone.png"))
        cout << "ERROR LOADING IMAGE TOP\n";
    glGenTextures(1, &stoneTexture);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stoneTextureImage.getSize().x, stoneTextureImage.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, stoneTextureImage.getPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Water
    if (!waterTextureImage.loadFromFile(resourcePath()+"water_still.png"))
        cout << "ERROR LOADING IMAGE TOP\n";
    glGenTextures(1, &waterTexture);
    glBindTexture(GL_TEXTURE_2D, waterTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, waterTextureImage.getSize().x, waterTextureImage.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, waterTextureImage.getPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Dirt
    if (!dirtTextureImage.loadFromFile(resourcePath()+"dirt.png"))
        cout << "ERROR LOADING IMAGE TOP\n";
    glGenTextures(1, &dirtTexture);
    glBindTexture(GL_TEXTURE_2D, dirtTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dirtTextureImage.getSize().x, dirtTextureImage.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, dirtTextureImage.getPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Mouse position
    int mouseX = window.getSize().x/2;
    int mouseY = window.getSize().y/2;
    
    // Mouse
    bool mouseLeftClick = false;
    bool mouseRightClick = false;
    
    // Camera
    float cameraX = 135.0f;
    float cameraY = 120.0f;
    
    // Mouse sensitivity
    float mouseSensitivity = 0.1;
    
    // Character position
    float characterX = 5.0f;
    float characterY = 5.0f;
    float characterZ = 65.0f;
    
    // Character speed
    float characterSpeed = 10.0f;
    
    // FPS
    sf::Time lastFrame;
    sf::Clock lastFrameClock;
    lastFrameClock.restart();
    
    // Perlin X Y
    maxChunkX = (int)ceil(worldX*perlinX/16.0f);
    maxChunkY = (int)ceil(worldY*perlinY/16.0f);
    chunksId.resize(maxChunkX);
    for (int x = 0; x < maxChunkX; x++)
        chunksId[x].resize(maxChunkY);
    chunks.resize((int)ceil(worldX*perlinX/16.0f)*(int)ceil(2*perlinY/16.0f));
    int id = 0;
    for (int x = 0; x < maxChunkX; x++)
    {
        for (int y = 0; y < maxChunkY; y++)
        {
            chunksId[x][y] = id;
            chunks[id].x = x;
            chunks[id].y = y;
            id++;
        }
    }
        
    // Table
    Perlin *perlin2D = new Perlin(16, 2, 1, 94);
    Perlin *perlin3D = new Perlin(16, 8, 1, 94);
    for (int x = 0; x < worldX*perlinX; x++)
        for (int y = 0; y < worldY*perlinY; y++)
        {
            float maxZ = 40.0f * perlin2D->Get2D(1.0*x/perlinX, 1.0*y/perlinY) * perlin2D->Get2D(1.0*x/perlinX, 1.0*y/perlinY) + 60.0f; // 60.0f
            if (maxZ < 61.0f)
            {
                maxZ = floor(100.0f * perlin2D->Get2D(1.0*x/perlinX, 1.0*y/perlinY) * perlin2D->Get2D(1.0*x/perlinX, 1.0*y/perlinY)) + 59.0f; // 60.0f
            }
            for (int z = 0; z < maxZ; z++)
            {
                float t = 100.0f * perlin3D->Get3D(1.0*x/perlinX/1.5f, 1.0*y/perlinY/1.5f, 1.0f*z/256.0f) * perlin3D->Get3D(1.0*x/perlinX/1.5f, 1.0*y/perlinY/1.5f, 1.0f*z/256.0f);
                //printf("%lf\n", t);
                if (t < 17.0f)
                    addCube(x, y, z);
            }
            if (maxZ >= 0 && maxZ < 256)
                addCube(x, y, maxZ);
        }
    for (int t = 0; t < chunks.size(); t++)
        calculateFacesToDraw(&chunks[t]);
    calculateWaterLevel(*perlin2D);
    cout << water.size() << "\n";
    
    // Start the game loop
    while (window.isOpen())
    {
        // Last frame
        lastFrame = lastFrameClock.getElapsedTime();
        lastFrameClock.restart();
        
        // Mouse position
        mouseX = sf::Mouse::getPosition(window).x;
        mouseY = sf::Mouse::getPosition(window).y;
        sf::Mouse::setPosition(sf::Vector2i(window.getSize().x/2, window.getSize().y/2), window);
        
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left)
                        mouseLeftClick = true;
                    if (event.mouseButton.button == sf::Mouse::Right)
                        mouseRightClick = true;
                    break;
                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Left)
                        mouseLeftClick = false;
                    if (event.mouseButton.button == sf::Mouse::Right)
                        mouseRightClick = false;
                    break;
                default:
                    break;
            }
        }
        
        // Close window : Escape
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();
        
        // Character movement
        float s = characterSpeed * lastFrame.asSeconds();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            characterX -= sin(-cameraX*M_PI/180.0f) * s;
            characterY += cos(-cameraX*M_PI/180.0f) * s;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            characterX += sin(-cameraX*M_PI/180.0f) * s;
            characterY -= cos(-cameraX*M_PI/180.0f) * s;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            characterX -= sin((-cameraX+90)*M_PI/180.0f) * s;
            characterY += cos((-cameraX+90)*M_PI/180.0f) * s;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            characterX -= sin((-cameraX-90)*M_PI/180.0f) * s;
            characterY += cos((-cameraX-90)*M_PI/180.0f) * s;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            characterZ += s;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            characterZ -= s;
        
        // Move camera
        int halfWidth = window.getSize().x/2;
        int halfHeigth = window.getSize().y/2;
        cameraX += 1.0f*mouseSensitivity*(mouseX - halfWidth);
        if (cameraX < 0)
            cameraX = 360;
        if (cameraX > 360)
            cameraX = 0;
        cameraY += 1.0f*mouseSensitivity*(mouseY - halfHeigth);
        if (cameraY < 0)
            cameraY = 0;
        if (cameraY > 180)
            cameraY = 180;
        
        // Add cube
        if (mouseLeftClick)
        {
            addCube((int)characterX, (int)characterY, (int)characterZ);
            calculateFacesToDraw(&chunks[chunksId[(int)floor(characterX/16.0f)][(int)floor(characterY/16.0f)]]);
        }
        
        /*
        // Delete cube
        if (mouseRightClick)
        {
            if ((int)characterY < worldSizeY)
            {
                deleteCube(&world((int)characterX, (int)characterY, (int)characterZ));
                calculateFacesToDraw();
            }
        }
        */

        // Clear screen
        window.clear();
        
        // Clear screen with color and clear Z-buffer
        glClearColor(129.0f/256.0f, 174.0f/256.0f, 254.0f/256.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set viewport
        glViewport(0, 0, window.getSize().x, window.getSize().y);
        
        // Matrix mode
        glMatrixMode(GL_MODELVIEW);
        
        // Reset view and transformations
        glLoadIdentity();
        
        // Textures parametrs
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        // Perspective
        gluPerspective(60.0, 1.0*window.getSize().x/window.getSize().y, 0.1, 500.0);
        
        // Rotate when user changes rotate_x and rotate_y
        glRotatef(cameraY-180, 1.0, 0.0, 0.0);
        glRotatef(cameraX, 0.0, 0.0, 1.0);
        
        // Push Matrix
        glPushMatrix();
        
        // Translate
        glTranslatef(-characterX, -characterY, -characterZ-1.5f);
        
        // Enable Vertex Array
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        
        // Color
        glColor3f(0.85f, 0.85f, 0.85f);
        
        // Enable textures
        glEnable(GL_TEXTURE_2D);
        
        // Texture
        glBindTexture(GL_TEXTURE_2D, dirtTexture);
        
        for (int t = 0; t < chunks.size(); t++)
        {
            // Define data
            glInterleavedArrays(GL_T2F_V3F, 0, &chunks[t].facesToDraw[0]);
                
            // Draw VBO
            glDrawArrays(GL_QUADS, 0, chunks[t].facesToDraw.size());
        }
        
        // Disable textures
        glDisable(GL_TEXTURE_2D);
        
        // Enable textures
        glEnable(GL_TEXTURE_2D);
        
        // Texture
        glBindTexture(GL_TEXTURE_2D, waterTexture);
        
        // Define data
        glInterleavedArrays(GL_T2F_V3F, 0, &water[0]);
        
        // Draw VBO
        glDrawArrays(GL_QUADS, 0, water.size());
        
        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Disable textures
        glDisable(GL_TEXTURE_2D);
        
        // Disable Vertex Array
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        
        // Pop matrix
        glPopMatrix();
        
        // Double buffering
        glFlush();
        
        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}

void addCube (int x, int y, int z)
{
    int chunkX = (int)floor(x/16.0f);
    int chunkY = (int)floor(y/16.0f);
    if (chunks[chunksId[chunkX][chunkY]].cubes[(int)x%16][(int)y%16][(int)z].type == 0)
        chunks[chunksId[chunkX][chunkY]].cubes[(int)x%16][(int)y%16][(int)z].type = 1;
}

void calculateFacesToDraw (chunk *t)
{
    t->facesToDraw.clear();
    int chunkX = t->x;
    int chunkY = t->y;
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int z = 0; z < 256; z++)
            {
                if (t->cubes[x][y][z].type != 0) {
                    // Is something above this cube
                    bool above = false;
                    
                    // TOP
                    if (z == 256 || (z < 256 && t->cubes[x][y][z+1].type == 0)) {
                        t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16,  0.5+y+chunkY*16,  0.5+z, 48.0f/48.0f, 1.0f));
                        t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 48.0f/48.0f, 0.0f));
                        t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 32.0f/48.0f, 0.0f));
                        t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16,  0.5+y+chunkY*16,  0.5+z, 32.0f/48.0f, 1.0f));
                        t->cubes[x][y][z].topFace = t->facesToDraw.size()-4;
                    } else {
                        above = true;
                    }

                    // BOTTOM
                    if (z == 0 || (z > 0 && t->cubes[x][y][z-1].type == 0)) {
                        t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 1.0f));
                        t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16,  0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 0.0f));
                        t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16,  0.5+y+chunkY*16, -0.5+z, 0.0f, 0.0f));
                        t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 0.0f, 1.0f));
                        t->cubes[x][y][z].bottomFace = t->facesToDraw.size()-4;
                    }
                    
                    // FRONT
                    if ((y == 0 && chunkY == 0) ||
                        (y == 0 && chunkY > 0 && chunks[chunksId[chunkX][chunkY-1]].cubes[x][15][z].type == 0) ||
                        (y > 0 && t->cubes[x][y-1][z].type == 0)) {
                        if (above) {
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 1.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 16.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 0.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 0.0f, 1.0f));
                        } else {
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 32.0f/48.0f, 1.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 32.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 16.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 1.0f));
                        }
                        t->cubes[x][y][z].frontFace = t->facesToDraw.size()-4;
                    }
                    
                    // BACK
                    if ((y == 15 && chunkY == maxChunkY-1) ||
                        (y == 15 && chunkY < maxChunkY-1 && chunks[chunksId[chunkX][chunkY+1]].cubes[x][0][z].type == 0) ||
                        (y < 15 && t->cubes[x][y+1][z].type == 0)) {
                        if (above) {
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, 0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 1.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, 0.5+y+chunkY*16,  0.5+z, 16.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, 0.5+y+chunkY*16,  0.5+z, 0.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, 0.5+y+chunkY*16, -0.5+z, 0.0f, 1.0f));
                        } else {
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, 0.5+y+chunkY*16, -0.5+z, 32.0f/48.0f, 1.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, 0.5+y+chunkY*16,  0.5+z, 32.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, 0.5+y+chunkY*16,  0.5+z, 16.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, 0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 1.0f));
                        }
                        t->cubes[x][y][z].backFace = t->facesToDraw.size()-4;
                    }
                    
                    // RIGHT
                    if ((x == 15 && chunkX == maxChunkX-1) ||
                        (x == 15 && chunkX < maxChunkX-1 && chunks[chunksId[chunkX+1][chunkY]].cubes[0][y][z].type == 0) ||
                        (x < 15 && t->cubes[x+1][y][z].type == 0)) {
                        if (above) {
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 1.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 16.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16,  0.5+y+chunkY*16,  0.5+z, 0.0f, 0.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16,  0.5+y+chunkY*16, -0.5+z, 0.0f, 1.0f));
                        } else {
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 32.0f/48.0f, 1.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 32.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16,  0.5+y+chunkY*16,  0.5+z, 16.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex( 0.5+x+chunkX*16,  0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 1.0f));
                        }
                        t->cubes[x][y][z].rightFace = t->facesToDraw.size()-4;
                    }
                    
                    // LEFT
                    if ((x == 0 && chunkX == 0) ||
                        (x == 0 && chunkX > 0 && chunks[chunksId[chunkX-1][chunkY]].cubes[15][y][z].type == 0) ||
                        (x > 0 && t->cubes[x-1][y][z].type == 0)) {
                        if (above) {
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16,  0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 1.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16,  0.5+y+chunkY*16,  0.5+z, 16.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 0.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 0.0f/48.0f, 1.0f));
                        } else {
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16,  0.5+y+chunkY*16, -0.5+z, 32.0f/48.0f, 1.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16,  0.5+y+chunkY*16,  0.5+z, 32.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16,  0.5+z, 16.0f/48.0f, 0.0f));
                            t->facesToDraw.push_back(vertex(-0.5+x+chunkX*16, -0.5+y+chunkY*16, -0.5+z, 16.0f/48.0f, 1.0f));
                        }
                        t->cubes[x][y][z].leftFace = t->facesToDraw.size()-4;
                    }
                }
            }
        }
    }
}

void calculateWaterLevel(Perlin perlin2D)
{
    for (int x = 0; x < worldX*perlinX; x++)
    {
        for (int y = 0; y < worldY*perlinY; y++)
        {
            float maxZ = 40.0f * perlin2D.Get2D(1.0*x/perlinX, 1.0*y/perlinY) * perlin2D.Get2D(1.0*x/perlinX, 1.0*y/perlinY) + 60.0f; // 60.0f
            if (maxZ < 61.0f)
            {
                maxZ = floor(100.0f * perlin2D.Get2D(1.0*x/perlinX, 1.0*y/perlinY) * perlin2D.Get2D(1.0*x/perlinX, 1.0*y/perlinY)) + 59.0f; // 60.0f
            }
            if (maxZ < 61.0f)
            {
                water.push_back(vertex( 0.5+x,  0.5+y,  0.4+61.0f, 1.0f, 1.0f));
                water.push_back(vertex( 0.5+x, -0.5+y,  0.4+61.0f, 1.0f, 0.0f));
                water.push_back(vertex(-0.5+x, -0.5+y,  0.4+61.0f, 0.0f, 0.0f));
                water.push_back(vertex(-0.5+x,  0.5+y,  0.4+61.0f, 0.0f, 1.0f));
            }
        }
    }
}

/*
void deleteCube(cube* cubeToDelete)
{
    if (cubeToDelete->type != 0) {
        // TOP
        if (cubeToDelete->topFace > -1)
        {
            allCubes.erase(allCubes.begin()+cubeToDelete->topFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->topFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->topFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->topFace);
            for (int x = 0; x < worldSizeX; x++)
                for (int y = 0; y < worldSizeY; y++)
                    for (int z = 0; z < worldSizeZ; z++) {
                        if (world(x, y, z).topFace > cubeToDelete->topFace) world(x, y, z).topFace -= 4;
                        if (world(x, y, z).bottomFace > cubeToDelete->topFace) world(x, y, z).bottomFace -= 4;
                        if (world(x, y, z).leftFace > cubeToDelete->topFace) world(x, y, z).leftFace -= 4;
                        if (world(x, y, z).rightFace > cubeToDelete->topFace) world(x, y, z).rightFace -= 4;
                        if (world(x, y, z).frontFace > cubeToDelete->topFace) world(x, y, z).frontFace -= 4;
                        if (world(x, y, z).backFace > cubeToDelete->topFace) world(x, y, z).backFace -= 4;
                    }
            cubeToDelete->topFace = -1;
        }
        // BOTTOM
        if (cubeToDelete->bottomFace > -1)
        {
            allCubes.erase(allCubes.begin()+cubeToDelete->bottomFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->bottomFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->bottomFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->bottomFace);
            for (int x = 0; x < worldSizeX; x++)
                for (int y = 0; y < worldSizeY; y++)
                    for (int z = 0; z < worldSizeZ; z++) {
                        if (world(x, y, z).topFace > cubeToDelete->bottomFace) world(x, y, z).topFace -= 4;
                        if (world(x, y, z).bottomFace > cubeToDelete->bottomFace) world(x, y, z).bottomFace -= 4;
                        if (world(x, y, z).leftFace > cubeToDelete->bottomFace) world(x, y, z).leftFace -= 4;
                        if (world(x, y, z).rightFace > cubeToDelete->bottomFace) world(x, y, z).rightFace -= 4;
                        if (world(x, y, z).frontFace > cubeToDelete->bottomFace) world(x, y, z).frontFace -= 4;
                        if (world(x, y, z).backFace > cubeToDelete->bottomFace) world(x, y, z).backFace -= 4;
                    }
            cubeToDelete->rightFace = -1;
        }
        // FRONT
        if (cubeToDelete->frontFace > -1)
        {
            allCubes.erase(allCubes.begin()+cubeToDelete->frontFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->frontFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->frontFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->frontFace);
            for (int x = 0; x < worldSizeX; x++)
                for (int y = 0; y < worldSizeY; y++)
                    for (int z = 0; z < worldSizeZ; z++) {
                        if (world(x, y, z).topFace > cubeToDelete->frontFace) world(x, y, z).topFace -= 4;
                        if (world(x, y, z).bottomFace > cubeToDelete->frontFace) world(x, y, z).bottomFace -= 4;
                        if (world(x, y, z).leftFace > cubeToDelete->frontFace) world(x, y, z).leftFace -= 4;
                        if (world(x, y, z).rightFace > cubeToDelete->frontFace) world(x, y, z).rightFace -= 4;
                        if (world(x, y, z).frontFace > cubeToDelete->frontFace) world(x, y, z).frontFace -= 4;
                        if (world(x, y, z).backFace > cubeToDelete->frontFace) world(x, y, z).backFace -= 4;
                    }
            cubeToDelete->frontFace = -1;
        }
        // BACK
        if (cubeToDelete->backFace > -1)
        {
            allCubes.erase(allCubes.begin()+cubeToDelete->backFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->backFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->backFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->backFace);
            for (int x = 0; x < worldSizeX; x++)
                for (int y = 0; y < worldSizeY; y++)
                    for (int z = 0; z < worldSizeZ; z++) {
                        if (world(x, y, z).topFace > cubeToDelete->backFace) world(x, y, z).topFace -= 4;
                        if (world(x, y, z).bottomFace > cubeToDelete->backFace) world(x, y, z).bottomFace -= 4;
                        if (world(x, y, z).leftFace > cubeToDelete->backFace) world(x, y, z).leftFace -= 4;
                        if (world(x, y, z).rightFace > cubeToDelete->backFace) world(x, y, z).rightFace -= 4;
                        if (world(x, y, z).frontFace > cubeToDelete->backFace) world(x, y, z).frontFace -= 4;
                        if (world(x, y, z).backFace > cubeToDelete->backFace) world(x, y, z).backFace -= 4;
                    }
            cubeToDelete->backFace = -1;
        }
        // LEFT
        if (cubeToDelete->leftFace > -1)
        {
            allCubes.erase(allCubes.begin()+cubeToDelete->leftFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->leftFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->leftFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->leftFace);
            for (int x = 0; x < worldSizeX; x++)
                for (int y = 0; y < worldSizeY; y++)
                    for (int z = 0; z < worldSizeZ; z++) {
                        if (world(x, y, z).topFace > cubeToDelete->leftFace) world(x, y, z).topFace -= 4;
                        if (world(x, y, z).bottomFace > cubeToDelete->leftFace) world(x, y, z).bottomFace -= 4;
                        if (world(x, y, z).leftFace > cubeToDelete->leftFace) world(x, y, z).leftFace -= 4;
                        if (world(x, y, z).rightFace > cubeToDelete->leftFace) world(x, y, z).rightFace -= 4;
                        if (world(x, y, z).frontFace > cubeToDelete->leftFace) world(x, y, z).frontFace -= 4;
                        if (world(x, y, z).backFace > cubeToDelete->leftFace) world(x, y, z).backFace -= 4;
                    }
            cubeToDelete->leftFace = -1;
        }
        // RIGHT
        if (cubeToDelete->rightFace > -1)
        {
            allCubes.erase(allCubes.begin()+cubeToDelete->rightFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->rightFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->rightFace);
            allCubes.erase(allCubes.begin()+cubeToDelete->rightFace);
            for (int x = 0; x < worldSizeX; x++)
                for (int y = 0; y < worldSizeY; y++)
                    for (int z = 0; z < worldSizeZ; z++) {
                        if (world(x, y, z).topFace > cubeToDelete->rightFace) world(x, y, z).topFace -= 4;
                        if (world(x, y, z).bottomFace > cubeToDelete->rightFace) world(x, y, z).bottomFace -= 4;
                        if (world(x, y, z).leftFace > cubeToDelete->rightFace) world(x, y, z).leftFace -= 4;
                        if (world(x, y, z).rightFace > cubeToDelete->rightFace) world(x, y, z).rightFace -= 4;
                        if (world(x, y, z).frontFace > cubeToDelete->rightFace) world(x, y, z).frontFace -= 4;
                        if (world(x, y, z).backFace > cubeToDelete->rightFace) world(x, y, z).backFace -= 4;
                    }
            cubeToDelete->rightFace = -1;
        }
        cubeToDelete->type = 0;
    }
}
*/