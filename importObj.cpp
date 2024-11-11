#include "importObj.h"
#include <GL/glut.h>

bool loadOBJ(const char* filePath, std::vector<Vertex>& vertices, std::vector<TextureCoord>& texCoords, std::vector<Normal>& normals, std::vector<Face>& faces) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vertex vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (prefix == "vt") {
            TextureCoord texCoord;
            iss >> texCoord.u >> texCoord.v;
            texCoords.push_back(texCoord);
        } else if (prefix == "vn") {
            Normal normal;
            iss >> normal.nx >> normal.ny >> normal.nz;
            normals.push_back(normal);
        }else if (prefix == "f") {
			Face face;
			std::string indices;
			while (iss >> indices) {
				std::istringstream indicesStream(indices);
				int vIndex, tIndex, nIndex;
				indicesStream >> vIndex;
				face.vIndex.push_back(vIndex - 1); // OBJ indices start from 1, but vector indices start from 0
				if (indicesStream.peek() == '/') {
					indicesStream.ignore();
					if (indicesStream.peek() != '/') {
						indicesStream >> tIndex;
						face.tIndex.push_back(tIndex - 1); // OBJ indices start from 1, but vector indices start from 0
					}
					if (indicesStream.peek() == '/') {
						indicesStream.ignore();
						indicesStream >> nIndex;
						face.nIndex.push_back(nIndex - 1); // OBJ indices start from 1, but vector indices start from 0
					}
				}
			}
			face.numVertices = face.vIndex.size(); // Store the number of vertices in the face
			faces.push_back(face);
		}
    }
    file.close();
    return true;
}
void drawModel( std::vector<Vertex>& vertices, std::vector<TextureCoord>& texCoords, std::vector<Normal>& normals, std::vector<Face>& faces )
{
	for( const auto& face : faces )
	{
		glBegin( GL_POLYGON );
		for( int i=0; i<face.numVertices; i++ )
		{
			int v=face.vIndex[i], n=face.nIndex[i];
			glNormal3f( normals[n].nx, normals[n].ny, normals[n].nz );
			glVertex3f( vertices[v].x, vertices[v].y, vertices[v].z );
			//std::cout << v << "-" << n << std::endl;
			//std::cout << vertices[v].x << ", " << vertices[v].y << ", " << vertices[v].z << std::endl;
			//std::cout << normals[n].nx << ", " << normals[n].ny << ", " << normals[n].nz << std::endl;
		}
		glEnd();
	}
}
void showObjInfo(std::vector<Vertex>& vertices, std::vector<TextureCoord>& texCoords, std::vector<Normal>& normals, std::vector<Face>& faces )
{

    // Process loaded data
    // Print vertices:
    for( const auto& vertex : vertices ) {
        std::cout << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
    }
	// Print texture coordinates
	for( const auto& tex : texCoords ) {
        std::cout << "vt " << tex.u << " " << tex.v << std::endl;
    }
    // Print normals
    for( const auto& nor : normals ) {
        std::cout << "vn " << nor.nx << " " << nor.ny << " " << nor.nz << std::endl;
    }
	// Print faces
	std::cout << "Faces indexes (vertex/texture/normal)" << std::endl;
	for( const auto& face : faces ) {
        for( int i=0; i<face.numVertices; i++ )
			std::cout << face.vIndex[i] << "/" << face.tIndex[i] << "/" << face.nIndex[i] << "  ";
		std::cout << std::endl;
	}

	std::cout << "#Faces: " << faces.size() << std::endl;
}
