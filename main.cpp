#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <glm/glm.hpp>
#include <ctime>

#include <vertex.h>
#include <texture.h>
#include <model.h>
#include <shader.h>
#include <mesh.h>
#include <car.h>
#include <scenario.h>
#include <kdtree.hpp>
#include <light.h>
#include <camera.h>

const std::string baseDir="../";
using namespace glm;

// FUNCTIONS
void clear(float,float,float,float);
void displayWin();
void reshapeWin(int,int);
void keyBorardFunc(unsigned char,int,int);
void idle();
void init();
void loadCar(int carNr);
// GLOBAL VARIABLES
Light light(vec3(0.0f,2.0f,10.0f));
Camera *camera;

std::map<std::string,Shader*> shaders;
std::map<std::string,Car*> cars;
Scenario *scenario;
float ang=0;
vector<int> carsList={9};
mat4 moveCar(1.0f);
int gkey=0;
int speedKey=0;
int directionKey=0;
mat4 cM(1.0f);

void renderCar(int carNr)
{
	shaders["car"]->use(true);
	GLuint program=shaders["car"]->getProgram();
	
	char key[50];
	sprintf(key,"car%d",carNr);

	mat4 Model = cars[key]->modelMatrix;
	Model = glm::rotate(mat4(1.0f),ang,glm::vec3(0.0,1.0,0.0))*moveCar;

	vec4 v=Model*vec4(0.0f,0.0f,0.0f,1.0f);
	camera->lookAtPos=vec3(v.x,v.y,v.z);

	camera->update();

	light.loadToShader(program);
	camera->loadToShader(program,Model);

    cars[key]->draw(shaders["car"]);
}

void renderScene()
{
	
	shaders["scenario"]->use(true);
	GLuint program=shaders["scenario"]->getProgram();

	mat4 Model = scenario->modelMatrix;
	Model = glm::rotate(Model,ang,glm::vec3(0.0,1.0,0.0));

	light.loadToShader(program);
	camera->loadToShader(program,Model);

    scenario->draw(shaders["scenario"]);
}

void displayWin()
{
	clear(0.22,0.22,0.22,1.0);
	shaders["car"]->reloadShader();
	shaders["scenario"]->reloadShader();

		switch(speedKey) 
	{
	case GLUT_KEY_UP :
			moveCar = glm::translate(moveCar,vec3(0.0,0.0,0.2));
			break;
	case GLUT_KEY_DOWN :
			moveCar = glm::translate(moveCar,vec3(0.0,0.0,-0.2));
			break;
	}
	vec3 v2;
	vec4 v;
	switch(directionKey) 
	{
	case GLUT_KEY_LEFT :
			cM = glm::rotate(cM,-3.1415f/200.0f,glm::vec3(0.0,1.0,0.0));
			v2=vec3(0.0f,5.0f,-10.0f);
			v=vec4(v2.x,v2.y,v2.z,1.0);
			v=v*cM;
			v2=vec3(v.x,v.y,v.z);
			camera->eyePos=v2;

			moveCar = glm::rotate(moveCar,3.1415f/200.0f,glm::vec3(0.0,1.0,0.0));
			break;
	case GLUT_KEY_RIGHT :
			cM = glm::rotate(cM,3.1415f/200.0f,glm::vec3(0.0,1.0,0.0));
			v2=vec3(0.0f,5.0f,-10.0f);
			v=vec4(v2.x,v2.y,v2.z,1.0);
			v=v*cM;
			v2=vec3(v.x,v.y,v.z);
			camera->eyePos=v2;

			moveCar = glm::rotate(moveCar,-3.1415f/200.0f,glm::vec3(0.0,1.0,0.0));
			break;
	}

	for (int carNr: carsList)
	{
		renderCar(carNr);
	}

	renderScene();

    static float lastTime=std::clock();
    static int frameN=0;
    frameN++;
    if (frameN%50==0)
    {

    	float temp=(std::clock()-lastTime)/50.0/(double)CLOCKS_PER_SEC*1000;
    	std::cout << "Time per Frame = " << temp << std::endl;
    	lastTime=std::clock();
    }

    // ang+=0.003;
	glutSwapBuffers();
}

void init()
{
	// Shader for all the cars
	shaders["car"]		= new Shader(baseDir+"shaders/car.vs", baseDir+"shaders/car.frag");
	// Shader for the scenario
	shaders["scenario"]	= new Shader(baseDir+"shaders/scenario.vs", baseDir+"shaders/scenario.frag");

	// Load the scenario mesh
	scenario = new Scenario(baseDir+"3dModels/city3/city.obj");

	// Load one car mesh
	for (int carNr: carsList)
	{
		loadCar(carNr);
	}
    // cars["car1"] = new Car(baseDir+"3dModels/car9/car.obj");
    // cars["car1"]->setColor(vec4(0.67,0.48,0.0,1.0));
    // cars["car1"]->modelMatrix = glm::rotate((cars["car1"]->modelMatrix),80.0f,glm::vec3(0.0f,1.0f,0.0f));
}

void processSpecialKeys(int key, int , int ) 
{
	gkey=key;
	switch( key ) 
	{
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
			speedKey=key;
		case GLUT_KEY_RIGHT:
		case GLUT_KEY_LEFT:
			directionKey=key;
			break;
	}
	std::cout << gkey << std::endl;
}

void keyBoardUp( int key, int x, int y )
{
	switch( key ) 
	{
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
			speedKey=0;
			break;
		case GLUT_KEY_RIGHT:
		case GLUT_KEY_LEFT:
			directionKey=0;
			break;
	}
	gkey=0;
}

void keyBorardFunc(unsigned char key, int, int)
{
	switch(key)
	{
		case 27:
		case 'q':
			glutLeaveMainLoop();
			break;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	int wWidth=720;
	int wHeight=480;

	const char windowName[]="window";
	glutInit(&argc, argv);
    glutInitWindowSize(wWidth,wHeight);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
 	glutCreateWindow(windowName);
 	
 	glEnable(GL_DEPTH_TEST); 
 	// glEnable(GL_BLEND);
 	glAlphaFunc ( GL_GREATER, 0.5 ) ;
 	glEnable ( GL_ALPHA_TEST ) ;
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

 	camera = new Camera(vec3(0.0f,5.0f,-10.0f),vec3(0.0,0.0,0.0),wWidth,wHeight);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
    	std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    	return 1;
    }
    init();
    glutDisplayFunc(displayWin);
	glutReshapeFunc(reshapeWin);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyBorardFunc);
	glutSpecialFunc(processSpecialKeys);
	glutSpecialUpFunc(keyBoardUp);
    glutMainLoop();

	return 0;
}

void loadCar(int carNr)
{
	char path[50];
	char key[50];
	sprintf(path,"%s/3dModels/car%d/car.obj",baseDir.c_str(),carNr);
	sprintf(key,"car%d",carNr);
	cars[key] = new Car(path);
}

void clear(float r,float g, float b, float a)
{
	glClearColor(r,g,b,a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void reshapeWin(int wWidth, int wHeight) 
{
	camera->setAspect(wWidth,wHeight);
	glViewport( 0, 0, wWidth, wHeight );
	glutPostRedisplay();
}

void idle()
{
	glutPostRedisplay();
}