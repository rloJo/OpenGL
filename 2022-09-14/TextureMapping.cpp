#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#define WIDTH   256
#define HEIGHT  256

unsigned char vol[225][256][256]; //volume data 225*256*256
void LoadVolume() {
	FILE* fp;
	fp = fopen("bighead.den", "rb");
	fread(vol, 256 * 256 * 225, 1, fp);
	fclose(fp);
}

/*
class pixel {
	unsigned char d[3];
};
pixel MyTexture[HEIGHT][WIDTH];
아래코드와 똑같은 의미 
*/

unsigned char MyTexture[HEIGHT][WIDTH][3];
// 3-> R,G,B


/*
int img[2][3] = {
	{1,2,3},
	{4,5,6}
};
void func() {
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 3; x++) {
			img[y][x] = 10;//process();
		}
	}
}
이미지의 좌표는 img[y][x] 
*/

// MPR: multi-plane reformation
void FillMyTexture() {
	int z = 100;
	int x = 100;
	int s, t;
	for (t = 0; t < HEIGHT; t++) {
		for (s = 0; s < WIDTH; s++) {
			if (t >= 225) {  // 좌표가 볼륨 데이터를 벗어나는 경우
				continue;
			}
			unsigned char Intensity = vol[t][s][x]; // zy 평면으로 vol data를 자름 
			unsigned char maxvalue = 0;
			for (int z = 0; z < 225; z++) {
				unsigned char Intensity = vol[z][t][s]; // xy 평면으로 vol data를 자름
				if (Intensity > maxvalue) { //그 중 최댓값(255와 가까운 값)을 구해서 MyTexture에 할당  
					maxvalue = Intensity;
				}
			} 
			//((s + t) % 2) * 255;    //0는 흑색, 255는 백색
			//Intensity = rand()%255;
			MyTexture[t][s][0] = maxvalue;             //Red 값에 할당
			MyTexture[t][s][1] = maxvalue;             //Green 값에 할당
			MyTexture[t][s][2] = maxvalue;             //Blue 값에 할당
		}
	}
}

void MyInit() {
	LoadVolume();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	FillMyTexture();
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
	float size = 1.0;
	glTexCoord2f(0.0, 1.0); glVertex3f(-size, -size, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-size, size, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(size, size, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(size, -size, 0.0);
	glEnd();
	glutSwapBuffers();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("openGL Sample Program");
	MyInit();
	glutDisplayFunc(MyDisplay);
	glutMainLoop();
	return 0;
}
