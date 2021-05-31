# include <iostream>
# include <SDL2/SDL.h>

# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>

# include <stdlib.h>
# include <stdio.h>
# include <math.h>

# include "quadTree.h" 
# include "heightMap.h"
# include "config.h"
# include "visu.h"
# include "gldrawing.h"
# include "geometry.h"
# include "textures.h"
#include <chrono>

#include "draw.h"

// # include "../include/camera.h"

#define STEP_ANGLE M_PI/90.
#define STEP_PROF  0.05

float latitude = 0.;
float longitude = M_PI/2.;
float angle = M_PI/2.;

using namespace std;

/* ====================== VARIABLES ===================== */

#define VITESSE_DEPLACEMENT 10.0
#define VITESSE_ROTATION 0.1

// dclaration de l'image et du quadTree
Image* pgm = new Image();
Node* quadTree = new Node();
Triangle * champCam = new Triangle();

// vue des triangles
bool isFilled = true;

// Gestion de la caméra
double pos_x = 1.0, pos_y = 1.0, pos_z = 5.0;
double x_vise = 1.0, z_vise = 0.0; 

//skybox
GLuint tabTextureId[6];

// bateaux 
const int maxBoat = 15;
GLuint tabBoat[maxBoat]; 
Point * posBoat = (Point *)malloc(sizeof(Point)*maxBoat);

float timerr = SDL_GetPerformanceCounter();

/* ================== DESSIN A L'ECRAN ================== */

static void drawFunc(void) { 
	
	Vector pVise = createPoint(	pos_x + cos(latitude) * sin(longitude), 
								pos_y + sin(latitude) * sin(longitude), 
								pos_z+cos(longitude));

	Vector vise = createPoint(	cos(latitude) * sin(longitude),
								sin(latitude) * sin(longitude),
								cos(longitude));
	
	Vector vectL = createPoint(	cos(latitude + angle),
								sin(latitude + angle),
								0.);

	Vector vectUp = produitVect(vise,vectL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHT0);
		
		glDrawRepere(10.0);
		
		glDepthMask(GL_FALSE);
		drawCenteredBox(5., tabTextureId);
		glDepthMask(GL_TRUE);

		gluLookAt(	pos_x, pos_z, pos_y, 
					pVise.x, pVise.z, pVise.y, 
					vectUp.x, vectUp.z, vectUp.y);

		glTranslatef(- pgm->xMax /2, 0, - pgm->yMax / 2);

		drawTerrain(quadTree, *pgm, isFilled, true, timerr, {pos_x, pos_y}); 

		for (int i = 0; i < maxBoat; i++) {
			drawBoat(angle, tabBoat[i], posBoat[i]);
		}

	glPopMatrix();
	
	glFinish(); 			// Fin de la définition de la scène 
	glutSwapBuffers();		// Changement buffer d'affichage
	
}


/* ================ REDIMENSION FENETRE ================ */

static void reshapeFunc(int width, int height) {

	GLfloat h = (GLfloat) width / (GLfloat) height ;
	
	glViewport(0, 0, (GLint)width, (GLint)height);	// dimension écran GL
	glMatrixMode(GL_PROJECTION);					// matrice de projection
	glLoadIdentity();
	
	gluPerspective( 60.0, h, 0.01, 100.0); 			// Définition caméra 

	glMatrixMode(GL_MODELVIEW);						// Retour pile de matrice
	glLoadIdentity();
}


/* ================ EVENEMENTS CLAVIER ================= */

static void kbdFunc(unsigned char c, int x, int y) {
	switch(c) {
		case 27 : case 'Q' : case 'q' :
			exit(0);
			break;
		case 'F' : case 'f' : 
			if (isFilled) {
				isFilled = false;
			} 
			else {
				isFilled = true;
			}
			break;
		default:
			printf("Appui sur la touche %c\n",c);
	}
	glutPostRedisplay();
}

/* ====== EVENEMENTS CLAVIER (TOUCHES SPECIALES) ======= */

static void kbdSpFunc(int c, int x, int y) {

	switch(c) {
		case GLUT_KEY_UP :
			pos_x += STEP_PROF * VITESSE_DEPLACEMENT;
			break;
		case GLUT_KEY_DOWN :
			pos_x -= STEP_PROF * VITESSE_DEPLACEMENT;
			break;
		case GLUT_KEY_LEFT :
			pos_y -= STEP_PROF * VITESSE_DEPLACEMENT;

			break;

		case GLUT_KEY_RIGHT :
			pos_y += STEP_PROF * VITESSE_DEPLACEMENT;
			break;
		case GLUT_KEY_PAGE_UP :
			pos_z += STEP_PROF * VITESSE_DEPLACEMENT;
			break;
		case GLUT_KEY_PAGE_DOWN :
			pos_z -= STEP_PROF * VITESSE_DEPLACEMENT;
			break;
		default:
			printf("Appui sur une touche spéciale\n");
	}
	glutPostRedisplay();
}

/* ================== INITIALISATION =================== */

static void init() {

	/* ===== Skybox ===== */ 

    char* name[6]={"sky_img/back.jpg",
		   "sky_img/front.jpg",
		   "sky_img/left.jpg", 
		   "sky_img/right.jpg", 
		   "sky_img/bottom.jpg", 
		   "sky_img/top.jpg"};
    for(int i = 0; i<6; i++){
        tabTextureId[i] = creaTexture(name[i]);
    }	

	/* ===== Bateaux ===== */
	
	char* boat = "boat.png"; 

	for (int i = 0; i < maxBoat; i++) {
		int xPos = rand() % pgm->xMax ; 
		int yPos = rand() % pgm->yMax; 
		int zPos = findZ(pgm->allZ, xPos, yPos, timerr);
		posBoat[i] = createPoint(xPos, yPos, zPos);
		tabBoat[i] = creaTexture(boat);
	}
	
	glClearColor(0.4, 0.7, 1. ,0.0);	// Background 
	glEnable(GL_DEPTH_TEST);			// Z-Buffer
	glShadeModel(GL_SMOOTH);			// Lissage couleurs
}


void idle(void) {
	glutPostRedisplay();
}

int main(int argc, char** argv) {

	/* ================ CONFIGURATION TIMAC ================= */
	
	Config* config = new Config();
	
    config = createConfig(argv[1]);
	if (config) {
		cout << "Fichier de configuration " << config->config << " chargé." << endl;
	}
	else {
		cout << "Echec du fichier de configuration." << config->config << endl;
		return -1;
	}

	// récupération des valeurs du fichier de configuration 
    int xSize = config->xsize;			// largeur du terrain 
    int ySize = config->ysize;			// profondeur du terrain
    int zMin = config->zmin;			// hauteur min
    int zMax = config->zmax;			// hauteur max

	// c
    int zNear = config->znear;			// vue la plus proche 
    int zFar = config->zfar;			// vue la plus loin 
    int fov = config->fov;				// champ de vision  

	/* ====================== LOAD MAP ====================== */

	pgm = loadImage(*config);

	/* ================ CREATION DU QUAD TREE =============== */
    
	int x1 = 0; 
    int x2 = xSize - x1 -1;
    int y1 = 0;
    int y2 = ySize - y1 -1;

	Square rect = createSquare(x1, x2, y1, y2);
	cout << "Square initialisé. " << endl;

	// création du quad tree 
	quadTree = initNode(quadTree, rect);
	cout << "QuadTree initialisé." << endl;


	/* ======================= CAMERA ======================= */

	*champCam = createTriangle({pos_x, pos_y}, zFar, fov);
	cout << "Triangle initialisé. " << endl;


    /* ======================= OPENGL ======================= */

	glutInit(&argc, argv);
	
	// initialisation du mode d'affichage
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(500, 500);

	if (glutCreateWindow("VisuTerrIMAC | Sirine & Audrey") == GL_FALSE) {
		return 1;
	}

	init();

	// Callbacks
	glutReshapeFunc(reshapeFunc);	// Redimensionnement
	glutDisplayFunc(drawFunc);		// Affichage 
	glutKeyboardFunc(kbdFunc);		// Event clavier 
	glutSpecialFunc(kbdSpFunc);		// Event clavier (touches spé.)
	glutIdleFunc(idle);				// Animation 

	glutMainLoop();					// Appel des callbacks
	
	return 0;
}
