#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <iostream>
#include <stdlib.h> 
#include <time.h>
using namespace std;
typedef float point3[3];
point3* colours;
int n_points;
int model;
bool tryb = true;
bool light0 = false;
bool light1 = false;
static GLfloat R = 10.0; //promień
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };
static GLfloat punkt_obserwacji[] = { 0.0, 0.0, 0.0 };
// inicjalizacja położenia obserwatora
static GLfloat theta_x = 0.0;   // kąt obrotu obiektu
static GLfloat theta_y = 0.0;   // kąt obrotu obiektu
static GLfloat theta_x1 = 0.0;   // kąt obrotu obiektu
static GLfloat theta_y1 = 0.0;   // kąt obrotu obiektu
static GLfloat pix2angle;     // przelicznik pikseli na stopnie
bool sciana1_show = true;
bool sciana2_show = true;
bool sciana3_show = true;
bool sciana4_show = true;
bool sciana5_show = true;

static GLint status = 0;       // stan klawiszy myszy
// 0 - nie naciśnięto żadnego klawisza
// 1 - naciśnięty zostać lewy klawisz

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int delta_x = 0;        // różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy
static int y_pos_old = 0;
static int delta_y = 0;
static int z_pos_old = 0;
static int delta_z = 0;
GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{
	// Struktura dla nagłówka pliku  TGA

#pragma pack(1)           
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;
	
	// Wartości domyślne zwracane w przypadku błędu

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	pFile = fopen(FileName, "rb");
	if (pFile == NULL) {
		cout << "blad odczytu pliku"<<endl;
		return NULL;
	}
		
	// Przeczytanie nagłówka pliku 


	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	// Odczytanie szerokości, wysokości i głębi obrazu

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;
	// Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity)

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	/*************************************************************************************/

	// Obliczenie rozmiaru bufora w pamięci


	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	// Alokacja pamięci dla danych obrazu


	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}
	// Ustawienie formatu OpenGL


	switch (sDepth)
	{

	case 3:
		*ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;

	case 4:
		*ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;

	case 1:
		*ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;

	};

	fclose(pFile);

	return pbitsperpixel;

}
float wektorNormXU(float u, float v) //obliczanie wektora normalnego
{
	return (-450 * u * u * u * u + 900 * u * u * u - 810 * u * u + 360 * u - 45) * cos(M_PI * v);
}

float wektorNormXV(float u, float v) //obliczanie wektora normalnego
{
	return M_PI * (90 * u * u * u * u * u - 225 * u * u * u * u + 270 * u * u * u - 180 * u * u + 45 * u) * sin(M_PI * v);
}

float wektorNormYU(float u, float v) //obliczanie wektora normalnego
{
	return 640 * u * u * u - 960 * u * u + 320 * u;
}

float wektorNormYV(float u, float v) //obliczanie wektora normalnego
{
	return 0;
}

float wektorNormZU(float u, float v) //obliczanie wektora normalnego
{
	return (-450 * u * u * u * u + 900 * u * u * u - 810 * u * u + 360 * u - 45) * sin(M_PI * v);
}

float wektorNormZV(float u, float v) //obliczanie wektora normalnego
{
	return -M_PI * (90 * u * u * u * u * u - 225 * u * u * u * u + 270 * u * u * u - 180 * u * u + 45 * u) * cos(M_PI * v);
}

float wektorNormX(float u, float v)
{
	return wektorNormYU(u, v) * wektorNormZV(u, v) - wektorNormZU(u, v) * wektorNormYV(u, v);
}

float wektorNormY(float u, float v)
{
	return wektorNormZU(u, v) * wektorNormXV(u, v) - wektorNormXU(u, v) * wektorNormZV(u, v);
}

float wektorNormZ(float u, float v)
{
	return wektorNormXU(u, v) * wektorNormYV(u, v) - wektorNormYU(u, v) * wektorNormXV(u, v);
}
float observerXS(float R, float azymut, float elewacja)
{
	return R * cos(azymut) * (float)cos(elewacja);
}

float observerYS(float R, float elewacja)
{
	return R * sin(elewacja);
}

float observerZS(float R, float azymut, float elewacja)
{
	return R * sin(azymut) * (float)cos(elewacja);
}

void Mouse(int btn, int state, int x, int y)
{


	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;        // przypisanie aktualnie odczytanej pozycji kursora
		// jako pozycji poprzedniej
		y_pos_old = y;
		status = 1;          // wcięnięty został lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		y_pos_old = y;
		status = 2;          // wcięnięty został lewy klawisz myszy
	}
	else

		status = 0;          // nie został wcięnięty żaden klawisz
}

/*************************************************************************************/
// Funkcja "monitoruje" położenie kursora myszy i ustawia wartości odpowiednich
// zmiennych globalnych

void Motion(GLsizei x, GLsizei y)
{

	delta_x = x - x_pos_old;     // obliczenie różnicy położenia kursora myszy
	x_pos_old = x;            // podstawienie bieżącego położenia jako poprzednie
	delta_y = y - y_pos_old;
	y_pos_old = y;
	glutPostRedisplay();     // przerysowanie obrazu sceny
}

// Funkcja rysująca osie układu wspó?rz?dnych

void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz?tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz?tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz?tek i koniec obrazu osi y

	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x

	glVertex3fv(x_min);
	glVertex3fv(x_max);

	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y

	glVertex3fv(y_min);
	glVertex3fv(y_max);

	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z

	glVertex3fv(z_min);
	glVertex3fv(z_max);

	glEnd();

}

/*************************************************************************************/

// Funkcja określająca co ma być rysowane (zawsze wywoływana, gdy trzeba
// przerysować scenę)
void rysujOstroslup() //rysowanie ostroslupa wraz z pokazywaniem/ukrywaniem poszczegolnych scian

{
	if (sciana1_show)
	{
		glBegin(GL_QUADS);
		glNormal3f(-5.0, -5.0, 0.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-5.0, -5.0, 0.0);

		glNormal3f(5.0, -5.0, 0.0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(5.0, -5.0, 0.0);

		glNormal3f(5.0, 5.0, 0.0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(5.0, 5.0, 0.0);

		glNormal3f(-5.0, 5.0, 0.0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-5.0, 5.0, 0.0);
		glEnd();
	}

	if (sciana2_show)
	{
		glBegin(GL_TRIANGLES);
		glNormal3f(5.0, -5.0, 0.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(5.0, -5.0, 0.0);

		glNormal3f(-5.0, -5.0, 0.0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-5.0, -5.0, 0.0);

		glNormal3f(0.0, 0.0, -10.0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.0, 0.0, -10.0);
		glEnd();
	}

	if (sciana3_show)
	{
		glBegin(GL_TRIANGLES);
		glNormal3f(5.0, 5.0, 0.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(5.0, 5.0, 0.0);

		glNormal3f(5.0, -5.0, 0.0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(5.0, -5.0, 0.0);

		glNormal3f(0.0, 0.0, -10.0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.0, 0.0, -10.0);
		glEnd();
	}

	if (sciana4_show)
	{
		glBegin(GL_TRIANGLES);
		glNormal3f(-5.0, 5.0, 0.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-5.0, 5.0, 0.0);

		glNormal3f(5.0, 5.0, 0.0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(5.0, 5.0, 0.0);

		glNormal3f(0.0, 0.0, -10.0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.0, 0.0, -10.0);
		glEnd();
	}

	if (sciana5_show)
	{
		glBegin(GL_TRIANGLES);
		glNormal3f(-5.0, -5.0, 0.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-5.0, -5.0, 0.0);

		glNormal3f(-5.0, 5.0, 0.0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-5.0, 5.0, 0.0);

		glNormal3f(0.0, 0.0, -10.0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.0, 0.0, -10.0);
		glEnd();
	}
}
void Egg(int n_points)
{
	glColor3f(0.0f, 1.0f, 0.0f);
	point3* points = new point3[n_points * n_points];
	point3* pompoints = new point3[n_points * n_points];
	point3* vpoints = new point3[n_points * n_points];
	double u, v;
	double dlugoscwek;
	int id = 0;
	for (int i = 0; i < n_points; i++)
		for (int j = 0; j < n_points; j++)
		{
			u = i / ((double)(n_points - 1) * 2.0);
			v = j / ((double)n_points / 2.0);
			points[id][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
			points[id][1] = (160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5);
			points[id][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);
			vpoints[id][0] = wektorNormX(u, v);
			vpoints[id][1] = wektorNormY(u, v);
			vpoints[id][2] = wektorNormZ(u, v);
			dlugoscwek = sqrt(vpoints[id][0]* vpoints[id][0] + vpoints[id][1] * vpoints[id][1] + vpoints[id][2] * vpoints[id][2]);
			vpoints[id][0] /= dlugoscwek;
			vpoints[id][1] /= dlugoscwek;
			vpoints[id][2] /= dlugoscwek;
			id++;
		}

	if (model == 1)
	{
		glBegin(GL_POINTS);
		for (int id = 0; id < n_points * n_points; id++)
			glVertex3d(points[id][0], points[id][1], points[id][2]);
		glEnd();
	}
	if (model == 2)
	{
		for (int i = 0; i < n_points * n_points; i += n_points)
		{
			for (int j = i; j < i + n_points - 1; j++)
			{
				glBegin(GL_LINES);
				glVertex3d(points[j][0], points[j][1], points[j][2]);
				glVertex3d(points[j + 1][0], points[j + 1][1], points[j + 1][2]);
				glEnd();
			}
			glBegin(GL_LINES);
			glVertex3d(points[i + n_points - 1][0], points[i + n_points - 1][1], points[i + n_points - 1][2]);
			glVertex3d(points[i][0], points[i][1], points[i][2]);
			glEnd();
		}

		for (int i = 0; i < n_points; i++)
		{
			for (int j = i; j < n_points * n_points - n_points; j += n_points)
			{
				glBegin(GL_LINES);
				glVertex3d(points[j][0], points[j][1], points[j][2]);
				glVertex3d(points[j + n_points][0], points[j + n_points][1], points[j + n_points][2]);
				glEnd();
			}
		}
	}
	if (model == 3)
	{
		int a, b, c, d;
		for (int i = 0; i < n_points; i++)
		{
			for (int j = i; j < n_points * n_points - n_points; j += n_points)
			{
				a = j;
				b = j + n_points;
				c = (j + 1) % (n_points * n_points);
				d = (j + 1) % (n_points * n_points) + n_points;

				glBegin(GL_TRIANGLES);
				glColor3d(colours[a][0], colours[a][1], colours[a][2]);
				glNormal3d(vpoints[a][0], vpoints[a][1], vpoints[a][2]);
				glTexCoord2d(j/n_points, i /n_points);
				glVertex3d(points[a][0], points[a][1], points[a][2]);
				glColor3d(colours[b][0], colours[b][1], colours[b][2]);
				glNormal3d(vpoints[b][0], vpoints[b][1], vpoints[b][2]);
				glTexCoord2d(j / n_points, i+1 / n_points);
				glVertex3d(points[b][0], points[b][1], points[b][2]);
				glColor3d(colours[c][0], colours[c][1], colours[c][2]);
				glNormal3d(vpoints[c][0], vpoints[c][1], vpoints[c][2]);
				glTexCoord2d(j+1 / n_points, i / n_points);
				glVertex3d(points[c][0], points[c][1], points[c][2]);
				glEnd();

				glBegin(GL_TRIANGLES);
				glColor3d(colours[b][0], colours[b][1], colours[b][2]);
				glNormal3d(vpoints[b][0], vpoints[b][1], vpoints[b][2]);
				//glTexCoord2d(a,a);
				glVertex3d(points[b][0], points[b][1], points[b][2]);
				glColor3d(colours[c][0], colours[c][1], colours[c][2]);
				glNormal3d(vpoints[c][0], vpoints[c][1], vpoints[c][2]);
				//glTexCoord2d(a,b);
				glVertex3d(points[c][0], points[c][1], points[c][2]);
				glColor3d(colours[d][0], colours[d][1], colours[d][2]);
				glNormal3d(vpoints[d][0], vpoints[d][1], vpoints[d][2]);
				//glTexCoord2d(b, 0.5*a);
				glVertex3d(points[d][0], points[d][1], points[d][2]);
				glEnd();
			}
		}
	}
	delete[] points;

	if (model == 4)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glutSolidTeapot(3.0);
	}
	if (model == 5)
		rysujOstroslup();
}


void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();
	// Czyszczenie macierzy bie??cej

	gluLookAt(viewer[0], viewer[1], viewer[2], punkt_obserwacji[0], punkt_obserwacji[1], punkt_obserwacji[2], 0.0, 1.0, 0.0);
	// Zdefiniowanie położenia obserwatora
	
	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej powyżej 

	if (status == 1 && tryb)                     // jeśli lewy klawisz myszy wcięnięty
	{
		theta_x += delta_x * pix2angle; // modyfikacja kąta obrotu o kat proporcjonalny
		theta_y += delta_y * pix2angle;
	}  // do różnicy położeń kursora myszy
	else if (status == 2 && tryb) {
		if (delta_y > 0) {
			viewer[2] += 0.1;
			punkt_obserwacji[2] += 0.25;
		}
		else {
			viewer[2] -= 0.1;
			punkt_obserwacji[2] -= 0.25;
		}
	}
	else if (status == 1 && !tryb) {
		theta_x1 += (delta_x * pix2angle/ 100.0f);    // modyfikacja kąta obrotu o kat proporcjonalny
		theta_y1 += (delta_y * pix2angle / 100.0f);
		viewer[0] = observerXS(R, theta_x1, theta_y1);
		viewer[1] = observerYS(R, theta_y1);
		viewer[2] = observerZS(R, theta_x1, theta_y1);

	}
	else if (status == 2 && !tryb) {
		R += (delta_y * pix2angle) / 25.0f; //zabezpieczenie
		if (R < 1.0f) R = 1.0f;
		viewer[0] = observerXS(R, theta_x1, theta_y1);
		viewer[1] = observerYS(R, theta_y1);
		viewer[2] = observerZS(R, theta_x1, theta_y1);
	}
		

	glRotatef(theta_x, 0.0, 1.0, 0.0);  //obrót obiektu o nowy kąt
	glRotatef(theta_y, 1.0, 0.0, 0.0);  //obrót obiektu o nowy kąt

	glColor3f(1.0f, 1.0f, 1.0f);
	// Ustawienie koloru rysowania na biały

	//glutWireTeapot(3.0);
	// Narysowanie czajnika 
	Egg(n_points);
	glFlush();
	// Przekazanie poleceń rysujących do wykonania

	glutSwapBuffers();
}
/*************************************************************************************/

// Funkcja ustalająca stan renderowania
void keys(unsigned char key, int x, int y)
{
	if (key == 'p')
		model = 1;
	if (key == 'w')
		model = 2;
	if (key == 's')
		model = 3;
	if (key == 't')
		model = 4;
	if (key == 'o')
		model = 5;
	if (key == 32)
		tryb = !tryb;
	if (key == '1')
	{
		light0 = !light0;
		if(light0)glEnable(GL_LIGHT0);
		else glDisable(GL_LIGHT0);
	}
	if (key == '2')
	{
		light1 = !light1;
		if (light1)glEnable(GL_LIGHT1);
		else glDisable(GL_LIGHT1);
	}
	if (key == '3') sciana1_show = !sciana1_show;
	if (key == '4') sciana2_show = !sciana2_show;
	if (key == '5') sciana3_show = !sciana3_show;
	if (key == '6') sciana4_show = !sciana4_show;
	if (key == '7') sciana5_show = !sciana5_show;
	RenderScene(); // przerysowywanie obrazu sceny
}

void MyInit(void)
{
	// Zmienne dla obrazu tekstury
	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;


	// Teksturowanie będzie prowadzone tyko po jednej stronie ściany

	//glEnable(GL_CULL_FACE);
	//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga

	pBytes = LoadTGAImage("tekstury/D1_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

	// Zdefiniowanie tekstury 2-D

	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	// Zwolnienie pamięci

	free(pBytes);

	// Włączenie mechanizmu teksturowania

	glEnable(GL_TEXTURE_2D);

	// Ustalenie trybu teksturowania

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Określenie sposobu nakładania tekstur

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszczący (wypełnienia okna) ustawiono na czarny


//  Definicja materiału z jakiego zrobiony jest czajnik
//  i definicja źródła światła

// Definicja materiału z jakiego zrobiony jest czajnik

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki ka =[kar,kag,kab] dla światła otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki kd =[kdr,kdg,kdb] światła rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki ks =[ksr,ksg,ksb] dla światła odbitego               

	GLfloat mat_shininess = { 20.0 };
	// współczynnik n opisujący połysk powierzchni

// Definicja źródła światła nr 1

	GLfloat light_position[] = { 0.0, -10.0, 0.0, 1.0 };
	// położenie źródła

	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// składowe intensywności świecenia źródła światła otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = { 0.0, 0.0, 0.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular[] = { 0.0, 0.0, 0.0, 0.5 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant = { 1.0 };
	// składowa stała ds dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_linear = { 0.05 };
	// składowa liniowa dl dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_quadratic = { 0.001 };
	// składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

/*************************************************************************************/
// Ustawienie parametrów materiału i źródła światła

/*************************************************************************************/
// Ustawienie patrametrów materiału


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	/*************************************************************************************/
	// Ustawienie parametrów źródła

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


	GLfloat light_diffuse1[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat light_specular1[] = { 0.0, 0.0, 1.0, 0.5 };
	GLfloat light_position1[] = { 0.0, 10.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);
	// Ustawienie opcji systemu oświetlania sceny

	glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
	glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
	//glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
	glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora
}


void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bieżącej

	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielkości okna okna widoku (viewport) w zależności
	// relacji pomiędzy wysokością i szerokością okna

	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli
void menu()
{

	cout << "Podaj wartosc liczby N: ";
	cin >> n_points;
	while (n_points < 0)
	{
		cout << "Podana niepoprawna wartosc. Sprobuj ponownie: ";
		cin >> n_points;
	}
	cout << "Celem zmiany wyświetlanego modelu nacisnij klawisz:" << endl
		<< " - p - dla modelu z punktow " << endl
		<< " - w - dla modelu siatkowego" << endl
		<< " - s - dla modelu kolorowego" << endl
		<< " - t - dla modelu czajniczka do herbaty"<<endl
		<< " - 1 - dla wlaczenia/wylaczenia swiatla nr 1" << endl
		<< " - 2 - dla wlaczenia/wylaczenia swiatla nr 2" << endl
		<< " - spacja - dla zmiany obrotu: obiekt/swiatlo";
	cout << endl;
}

void main(void)
{

	menu();
	colours = new point3[n_points * n_points];
	srand(time(NULL));
	for (int i = 0; i < n_points * n_points; i++)
	{
		colours[i][0] = (rand() % 101) * 0.01;
		colours[i][1] = (rand() % 101) * 0.01;
		colours[i][2] = (rand() % 101) * 0.01;
	}
	for (int i = 0; i < n_points; i++)
	{
		colours[i][0] = colours[0][0];
		colours[i][1] = colours[0][1];
		colours[i][2] = colours[0][2];
		colours[n_points * n_points - 1 - i][0] = colours[n_points * n_points - 1][0];
		colours[n_points * n_points - 1 - i][1] = colours[n_points * n_points - 1][1];
		colours[n_points * n_points - 1 - i][2] = colours[n_points * n_points - 1][2];
	}
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);

	glutCreateWindow("Rzutowanie perspektywiczne");

	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną
	// (callback function).  Będzie ona wywoływana za każdym razem
	// gdy zajdzie potrzeba przerysowania okna



	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
	// za zmiany rozmiaru okna                       

	glutMouseFunc(Mouse);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy

	MyInit();
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przystąpieniem do renderowania 

	glEnable(GL_DEPTH_TEST);
	// Włączenie mechanizmu usuwania niewidocznych elementów sceny
	glutKeyboardFunc(keys);
	//glutIdleFunc(spinEgg);
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
	delete[] colours;

}