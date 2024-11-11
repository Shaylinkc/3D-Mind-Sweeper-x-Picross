//#define _WCHAR_T_DEFINED

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

struct Vertex {
    float x, y, z;
};

struct TextureCoord {
    float u, v;
};

struct Normal {
    float nx, ny, nz;
};

struct Face {
    std::vector<int> vIndex;
    std::vector<int> tIndex;
    std::vector<int> nIndex;
    int numVertices; // Number of vertices in the face
};

extern bool loadOBJ( const char* filePath, std::vector<Vertex>& vertices, std::vector<TextureCoord>& texCoords, std::vector<Normal>& normals, std::vector<Face>& faces );
extern void showObjInfo( std::vector<Vertex>& vertices, std::vector<TextureCoord>& texCoords, std::vector<Normal>& normals, std::vector<Face>& faces );
extern void drawModel( std::vector<Vertex>& vertices, std::vector<TextureCoord>& texCoords, std::vector<Normal>& normals, std::vector<Face>& faces );
