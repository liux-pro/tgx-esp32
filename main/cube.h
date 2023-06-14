// 3D model [cube]
//
// - vertices   : 8
// - textures   : 0
// - normals    : 0
// - triangles  : 12
//
// - memory size: 0kb
//
// - model bounding box: [-1.0,1.0]x[-1.0,1.0]x[-1.0,1.0]
//
// object [cube] (tagged []) with 12 triangles (1 chains)

#pragma once

#include <tgx.h>


// vertex array: 0kb.
const tgx::fVec3 cube_vert_array[8] PROGMEM = {
{1.0,1.0,1.0},
{1.0,-1.0,-1.0},
{1.0,1.0,-1.0},
{-1.0,1.0,1.0},
{-1.0,1.0,-1.0},
{-1.0,-1.0,-1.0},
{1.0,-1.0,1.0},
{-1.0,-1.0,1.0}
};


// face array: 0kb.
const uint16_t cube_face[16] PROGMEM = {
12, // chain 0
0, 1, 2, 
3, 32772, 32773, 32769, 6, 32768, 3, 7, 32772, 5, 6, 

 0};


// mesh info for object cube (with tag [])
const tgx::Mesh3D<tgx::RGB565> cube PROGMEM = 
    {
    1, // version/id
    
    8, // number of vertices
    0, // number of texture coords
    0, // number of normal vectors
    12, // number of triangles
    16, // size of the face array. 

    cube_vert_array, // array of vertices
    nullptr, // array of texture coords
    nullptr, // array of normal vectors        
    cube_face, // array of face vertex indexes   
    
    nullptr, // pointer to texture image 
    
    { 0.75f , 0.75f, 0.75f }, // default color
    
    0.1f, // ambiant light strength 
    0.7f, // diffuse light strength
    0.6f, // specular light strength
    32, // specular exponent
    
    nullptr, // next mesh to draw after this one    
    
    { // mesh bounding box
    -1.0f, 1.0f, 
    -1.0f, 1.0f, 
    -1.0f, 1.0f
    },
    
    "cube" // model name    
    };
    
                
/** end of cube.h */
    
    
    