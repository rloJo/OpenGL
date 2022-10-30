#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#define WIDTH   256
#define HEIGHT  256

unsigned char vol[225][256][256];
char direct = 'x';

void LoadVolume() {
	FILE* fp;
	fp = fopen("bighead.den", "rb");
	fread(vol, 256 * 256 * 225, 1, fp);
	fclose(fp);
}

unsigned char MyTexture[HEIGHT][WIDTH][3]; // 3: rgb

// MPR: multi-plane reformation
void FillMyTexture(char direction) {
	int s, t;
	switch (direction)
	{
	case 'z':
		for (t = 0; t < HEIGHT; t++) {
			for (s = 0; s < WIDTH; s++) {
				unsigned char maxvalue = 0;
				for (int z = 0; z < 225; z++) {
					unsigned char Intensity = vol[z][t][s]; //[z][y][x]
					if (Intensity > maxvalue) {
						maxvalue = Intensity;
					}
				}
				//	((s + t) % 2) * 255;    //0는 흑색, 255는 백색
				// Intensity = rand()%255;
				MyTexture[t][s][0] = maxvalue;             //Red 값에 할당
				MyTexture[t][s][1] = maxvalue;             //Green 값에 할당
				MyTexture[t][s][2] = maxvalue;             //Blue 값에 할당
			}
		}
		break;
	case 'x':
		for (t = 0; t < HEIGHT; t++) {
			for (s = 0; s < WIDTH; s++) {
				unsigned char maxvalue = 0;
				if (t >= 225)
					continue;
				for (int x = 0; x < 226; x++) {
					unsigned char Intensity = vol[t][s][x]; //[z][y][x]
					if (Intensity > maxvalue) {
						maxvalue = Intensity;
					}
				}
				MyTexture[t][s][0] = maxvalue;             //Red 값에 할당
				MyTexture[t][s][1] = maxvalue;             //Green 값에 할당
				MyTexture[t][s][2] = maxvalue;             //Blue 값에 할당
			}
		}
		break;
	case'y':
		for (t = 0; t < HEIGHT; t++) {
			for (s = 0; s < WIDTH; s++) {
				if (t >= 225) continue;
				unsigned char maxvalue = 0;
				for (int y = 0; y < 226; y++) {
					unsigned char Intensity = vol[t][y][s]; //[z][y][x]
					if (Intensity > maxvalue) {
						maxvalue = Intensity;
					}
				}
				MyTexture[t][s][0] = maxvalue;             //Red 값에 할당
				MyTexture[t][s][1] = maxvalue;             //Green 값에 할s당
				MyTexture[t][s][2] = maxvalue;  
			}
		}
		break; 
	default: break;
	}		
}

void MyInit() {
	LoadVolume();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	FillMyTexture(direct);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, WIDTH, HEIGHT, 0, GL_RGB,
		GL_UNSIGNED_BYTE, &MyTexture[0][0][0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // GL_REPEAT
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // GL_REPEAT
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
	// (0, 0) ~ (3, 3)
	float size = 1;
	glTexCoord2f(0.0, 1.0); glVertex3f(-size, -size, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-size, size, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(size, size, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(size, -size, 0.0);
	glEnd();
	glutSwapBuffers();
}

void MyKeyboard(unsigned char KeyPressed, int X, int Y) {
	if (KeyPressed == 'x')
		direct = 'x';
	else if (KeyPressed == 'y')
		direct = 'y';
	else if (KeyPressed == 'z')
		direct = 'z';

	MyInit();
	glutPostRedisplay();
	}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("volume");
	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutMainLoop();
	return 0;
}
