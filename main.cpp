#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <unistd.h>
#include <math.h>
#include <windows.h>


#define LIGHT_POSX 25.0f
#define LIGHT_POSY 40.0f
#define LIGHT_POSZ 100.0f

using namespace std;

//Vision parameters
int advance=0;
float vx=0, vy=0, vz=10, rx, ry, rz, elev, azim;
float d_lat, d_fro, d_azim, d_ver, m_d_fro; //desplazamientos lateral, frontal y vertical
//Mouse navigation
int mouse_left=0, mouse_right=0;
int mouse_ctrl=0, mn_x0, mn_y0;

const int rows = 5;
const int cols = 5;
const int num_mines = 6;
const int num_points = 6;
const int cellSize = 100;
int rowTotal[5] = {0,0,0,0,0};
int colTotal[5] = {0,0,0,0,0};
int rowBomb[5] = {0,0,0,0,0};
int colBomb[5] = {0,0,0,0,0};
bool tryAgain = false;
int i, j;
int number=0;

static GLfloat pos[4] = { LIGHT_POSX, LIGHT_POSY, LIGHT_POSZ, 1.0f };
static GLfloat	blue[] =    { 0.3f, 0.3f, 1.0f }; //position. 0.0 for infinite
static GLfloat	fullWhite[]={ 1.0f, 1.0f, 1.0f };
static GLfloat	gold[] =    { 0.7f, 0.6f, 0.0f };
static GLfloat	white[] =   { 0.2f, 0.2f, 0.2f };
GLuint textureTopCube, textureBomb, textureNumbers[4];

vector<vector<char>> mines_grid(rows, vector<char>(cols, ' '));
vector<vector<int>> picross_grid(rows, vector<int>(cols, 1));
vector<vector<bool>> revealed(rows, vector<bool>(cols, false));

void init3D( void );
void loop( void );
void vision( void );
void draw( void );
void reshape(int ancho, int alto );
void keyboard(unsigned char key, int x, int y);
void placeMines(void);
void generatePicrossNumbers(void);
void printPicrossGrid(void);
void bombCheck(int x, int y);
void initTexture(void);
void textureCube(void);
void textureUnderCube(void);
void textureNumberCube();
bool topCubeVisible = true;
bool topCubesVis[5][5] =
{{true,true,true,true,true},
{true,true,true,true,true},
{true,true,true,true,true},
{true,true,true,true,true},
{true,true,true,true,true}};


//Main function, initialize graphics

int main( int argc, char **argv ){
    placeMines();
    generatePicrossNumbers();
    printPicrossGrid();

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH  );
	glutInitWindowSize( 1024, 700 );
	glutInitWindowPosition( 0, 0 );
	glutCreateWindow( "3D Template" );
	init3D();

    initTexture();
	glutDisplayFunc( draw );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutIdleFunc( loop );
	glutMainLoop( );
}


void init3D(){
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glClearColor( 0.5,0.5,0.5, 0.0 );
}
//Loop function for the animation

void loop( void ){
    ::advance++;
    vision();
    draw();
    Sleep( 40 ); // milliseconds
}

void vision( void ){

	d_fro+=m_d_fro;

	azim += d_azim;
	vx += d_lat * cos( azim ) + d_fro * sin( azim );
	vy += -d_lat * sin( azim ) + d_fro * cos( azim );
	vz += d_ver;

	rx = vx + 30.0 * sin( azim );
	ry = vy + 30.0 * cos( azim );
	rz = vz * 0.40;
}

void draw(){
	int	x, y;
	glLightfv( GL_LIGHT0, GL_POSITION, pos ); //needed here

	glLoadIdentity();
	gluLookAt( vx, vy, vz,   rx, ry, rz,  0.0, 0.0, 1.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//Topgrid - add basic texture
    glTranslated( -5, -5, 5 );
    if (topCubeVisible) {
        glPushMatrix();
            for(int i = 0; i < 5; i++){
                glTranslated( 10, 0, 0 );
                    for(int j = 0; j < 5; j++){
                        glTranslated( 0, 10, 0 );
                        glPushMatrix();
                        glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white );
                        glScaled(5.0, 5.0, 1.0);
                            if(topCubesVis[i][j]){
                                glutSolidCube(1.0);
                                textureCube();
                            }
                        glPopMatrix();
                        glEnd();
            }
            glTranslated( 0, -50, 0 );
        }
        glPopMatrix();
}
// BOMB & Number
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            glPushMatrix();
            glTranslated(10 * (i + 1), 10 * (j + 1), -2);
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
            glScaled(4.0, 4.0, 1.0);
            glutSolidCube(1.0);

            if (mines_grid[i][j] == '*') {
                textureUnderCube();
            } else {
                textureNumberCube();
            }
        glPopMatrix();
        glEnd();
    }
}

//Light source
	glPushMatrix();
	glTranslatef( LIGHT_POSX, LIGHT_POSY, LIGHT_POSZ );
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white );
	glDisable( GL_LIGHTING );
	glutSolidSphere( 2.0, 10, 10 );
	glEnable( GL_LIGHTING );
	glPopMatrix();
    glEnd();
	glutSwapBuffers();
}

void reshape(int ancho, int alto ){
	glViewport(0, 0, ancho, alto );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 45.0, (float)ancho/(float)alto, 10.0, 800.0 );
	glMatrixMode( GL_MODELVIEW );
}

//Keyboard actions
void keyboard(unsigned char key, int x, int y) {
    const float cameraSpeed = -2.0f; // Adjust the camera speed as needed
    if(!tryAgain)
    {
        switch (key) {
        case 'q': // Move the camera forward
            vz -= cameraSpeed;
            break;
        case 'e': // Move the camera backward
            vz += cameraSpeed;
            break;
        case 'd': // Move the camera left
            vx -= cameraSpeed;
            break;
        case 'a': // Move the camera right
            vx += cameraSpeed;
            break;
        case 's': // Move the camera up
            vy += cameraSpeed;
            break;
        case 'w': // Move the camera down
            vy -= cameraSpeed;
            break;

        case 'A':
            topCubesVis[0][4] = !topCubesVis[0][4];
            bombCheck(0,4);
            break;
        case 'B':
            topCubesVis[1][4] = !topCubesVis[1][4];
            bombCheck(1,4);
            break;
        case 'C':
            topCubesVis[2][4] = !topCubesVis[2][4];
            bombCheck(2,4);
            break;
        case 'D':
            topCubesVis[3][4] = !topCubesVis[3][4];
            bombCheck(3,4);
            break;
        case 'E':
            topCubesVis[4][4] = !topCubesVis[4][4];
            bombCheck(4,4);
            break;

        case 'F':
            topCubesVis[0][3] = !topCubesVis[0][3];
            bombCheck(0,3);
            break;
        case 'G':
            topCubesVis[1][3] = !topCubesVis[1][3];
            bombCheck(1,3);
            break;
        case 'H':
            topCubesVis[2][3] = !topCubesVis[2][3];
            bombCheck(2,3);
            break;
        case 'I':
            topCubesVis[3][3] = !topCubesVis[3][3];
            bombCheck(3,3);
            break;
        case 'J':
            topCubesVis[4][3] = !topCubesVis[4][3];
            bombCheck(4,3);
            break;

        case 'K':
            topCubesVis[0][2] = !topCubesVis[0][2];
            bombCheck(0,2);
            break;
        case 'L':
            topCubesVis[1][2] = !topCubesVis[1][2];
            bombCheck(1,2);
            break;
        case 'M':
            topCubesVis[2][2] = !topCubesVis[2][2];
            bombCheck(2,2);
            break;
        case 'N':
            topCubesVis[3][2] = !topCubesVis[3][2];
            bombCheck(3,2);
            break;
        case 'O':
            topCubesVis[4][2] = !topCubesVis[4][2];
            bombCheck(4,2);
            break;

        case 'P':
            topCubesVis[0][1] = !topCubesVis[0][1];
            bombCheck(0,1);
            break;
        case 'Q':
            topCubesVis[1][1] = !topCubesVis[1][1];
            bombCheck(1,1);
            break;
        case 'R':
            topCubesVis[2][1] = !topCubesVis[2][1];
            bombCheck(2,1);
            break;
        case 'S':
            topCubesVis[3][1] = !topCubesVis[3][1];
            bombCheck(3,1);
            break;
        case 'T':
            topCubesVis[4][1] = !topCubesVis[4][1];
            bombCheck(4,1);
            break;

        case 'U':
            topCubesVis[0][0] = !topCubesVis[0][0];
            bombCheck(0,0);
            break;
        case 'V':
            topCubesVis[1][0] = !topCubesVis[1][0];
            bombCheck(1,0);
            break;
        case 'W':
            topCubesVis[2][0] = !topCubesVis[2][0];
            bombCheck(2,0);
            break;
        case 'X':
            topCubesVis[3][0] = !topCubesVis[3][0];
            bombCheck(3,0);
            break;
        case 'Y':
            topCubesVis[4][0] = !topCubesVis[4][0];
            bombCheck(4,0);
            break;

        case 'Z':
            topCubeVisible = !topCubeVisible; //debug to make sure all blocks can disappear
            break;
        case 27: // ESC key to exit
            exit(0);
            break;
    }
    }
    glutPostRedisplay();
}

void placeMines() {
    srand(time(NULL));
    int count = 0;
    while (count < num_mines) {
        int x = rand() % rows;
        int y = rand() % cols;
        if (mines_grid[y][x] != '*') {
            mines_grid[y][x] = '*';
            count++;
            rowBomb[x]++;
            colBomb[y]++;
        }
    }
}

void generatePicrossNumbers(){
    int count = 0;
    while(count < num_points){
        int x = rand() % rows;
        int y = rand() % cols;
        if (mines_grid[y][x] != '*' ) {
            picross_grid[y][x] = rand()%2 + 2;
            count++;
        }
    }
    for (int i = 0; i < cols; ++i){
        for (int j = 0; j < rows; ++j){
            if (mines_grid[i][j] == '*'){
                    picross_grid[i][j] = 0;
                }
                // add to row and col total
                rowTotal[j] += picross_grid[i][j];
                colTotal[i] += picross_grid[i][j];
            }
        }
}

//For Debug Purposes displays answer to game
void printPicrossGrid() {

    for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) {
            if (mines_grid[i][j] == '*') {
                cout << "* ";
            }
            else {
                cout << picross_grid[i][j] << " ";
            }
        }
        cout <<"| " << colTotal[i] << " ";
        cout << colBomb[i] << " ";
        cout << endl;
    }
    cout <<"----------"<< endl;
    for(int j = 0; j < cols; ++j){
        cout  << rowTotal[j] << " ";
    }
    cout << endl;
    for(int j = 0; j < cols; ++j){
        cout  << rowBomb[j] << " ";
    }
}

void initTexture(){
	glBlendFunc( GL_ONE_MINUS_DST_ALPHA, GL_ONE );

	textureTopCube = SOIL_load_OGL_texture("pngTextures/ill.png",SOIL_LOAD_RGBA,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
    textureBomb = SOIL_load_OGL_texture("pngTextures/bomb.png", SOIL_LOAD_RGBA,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);

    for (int i = 0; i < 4; ++i) {
        string filename = "pngTextures/Number" + to_string(i) + ".png";
        textureNumbers[i] = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    }
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void textureCube(){ // top interactable one
    glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, textureTopCube );
    glBegin(GL_QUADS);
    // Front face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);
    // Back face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, -1.0, -1.0);
    // Top face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, -1.0);
    // Bottom face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 1.0);
    // Right face
    glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, -1.0, 1.0);
    // Left face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, -1.0);
    glEnd();
}

void textureUnderCube() {
    glEnable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, textureBomb );
    glBegin(GL_QUADS);
    // Front face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);
    // Back face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, -1.0, -1.0);
    // Top face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, -1.0);
    // Bottom face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 1.0);
    // Right face
    glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, -1.0, 1.0);
    // Left face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, -1.0);
    glEnd();
}

void textureNumberCube() {
    glEnable( GL_TEXTURE_2D );
    int number = picross_grid[i][j];

    if (number >= 0 && number < 4) {
        glBindTexture(GL_TEXTURE_2D, textureNumbers[number]);
    } else {
    }

    glBegin(GL_QUADS);
    // Front face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);
    // Back face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, -1.0, -1.0);
    // Top face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, -1.0);
    // Bottom face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 1.0);
    // Right face
    glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, -1.0, 1.0);
    // Left face
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, -1.0);
    glEnd();
}


void bombCheck(int x, int y)
{
    if(!topCubesVis[x][y] && mines_grid[x][y]== '*')
    {
        topCubeVisible = false;
        tryAgain = true;
    }
}
