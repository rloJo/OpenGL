#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "vec3.h"
#define WIDTH   256
#define HEIGHT  256
vec3 eye(255, 255, 0);
vec3 u, v; // 카메라의 x,y축 방향
vec3 w; // 카메라의 z축 방향

unsigned char vol[225][256][256];

void LoadVolume() {
	FILE* fp;
	fp = fopen("bighead.den", "rb");
	fread(vol, 256 * 256 * 225, 1, fp);
	fclose(fp);
}

unsigned char MyTexture[HEIGHT][WIDTH][3]; // 3: rgb

// MPR: multi-plane reformation
void FillMyTexture() {
	for (int iy = 0; iy < HEIGHT; iy++) {
		for (int ix = 0; ix < WIDTH; ix++) {
			unsigned char maxvalue = 0;
			vec3 rayStart = eye + u * (ix - 128) + v * (iy - 128);
			for (float t = 0; t < 256; t += 1.0) {

				// 영상좌표 (ix,iy,t)가 전역좌표 어떤 점으로 가지?
				//vol(vx,vy,vz,1) = 특별한 변환행렬4x4 * (ix,iy,t,1)

				vec3 p = rayStart + w * t; // 현재점 좌표
				if (p.x < 0 || p.y < 0 || p.z < 0 ||
					p.x >= 256 || p.y >= 256 || p.z >= 225)
					continue;
				unsigned char Intensity = vol[(int)p.z][(int)p.y][(int)p.x];
				if (Intensity > maxvalue) {
					maxvalue = Intensity;
				}
			}
			MyTexture[iy][ix][0] = maxvalue;
			MyTexture[iy][ix][1] = maxvalue;
			MyTexture[iy][ix][2] = maxvalue;
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
	vec3 at(128, 128, 112);
	vec3 dir = at - eye; // eye에서 at으로 가려면 이만큼 전진하면 됨
	w = dir.Normalize(); // 크기를 1로 만든다. 방향은 유지 // 사실은 w가 카메라의 z방향
	vec3 up(0, 1, 0); // 하드코딩, 이유는 나중에. 대략 카메라 y와 관계있음
	u = up.Cross(w); // u = up 외적 w // w 와 u는 수직
	u.Normalize();
	v = w.Cross(u); // v = w 외적 u // v와 w,  v와 u는 수직
	v.Normalize();
	// 이미 u,v,w는 서로 수직
	// up을 uv 평면에 투영시키면 v와 일치한다.
	printf("uvw:\n");
	u.Print();
	v.Print();
	w.Print();


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("openGL Sample Program");
	MyInit();
	glutDisplayFunc(MyDisplay);
	glutMainLoop();
	return 0;
}
