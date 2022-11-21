#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "vec3.h"
#define WIDTH   512
#define HEIGHT  512
vec3 eye(100, 90, 80);
vec3 u, v; // 카메라의 x,y축 방향
vec3 w; // 카메라의 z축 방향
unsigned char vol[225][256][256];
unsigned char bmax[57][64][64];
unsigned char bmin[57][64][64];

void makeBlock(unsigned char in[225][256][256], unsigned char outMax[57][64][64], unsigned char outMin[57][64][64]) {
	// 출력 각 원소에 대해서
	for (int i = 0; i < 57; i++) {
		for (int j = 0; j < 64; j++) {
			for (int k = 0; k < 64; k++) {
				// 원본에서 반복을 돌자
				unsigned char Max = 0; // in[i * 4];
				unsigned char Min = 255; // in[i * 4];
				for (int ii = i * 4; ii < __min(i * 4 + 5, 225); ii++) { // 224
					for (int jj = j * 4; jj < j * 4 + 5; jj++) {
						for (int kk = k * 4; kk < k * 4 + 5; kk++) {
							Max = __max(Max, in[ii][jj][kk]);
							Min = __min(Min, in[ii][jj][kk]);
						}
					}
				}
				// 출력 각 원소에 값을 대입하자
				outMax[i][j][k] = Max;
				outMin[i][j][k] = Min;
			}
		}
	}




}

void LoadVolume() {
	FILE* fp;
	fp = fopen("bighead.den", "rb");
	fread(vol, 256 * 256 * 225, 1, fp);
	fclose(fp);
}

//class pixel {
//	unsigned char d[3];
//};
//pixel MyTexture[HEIGHT][WIDTH];
unsigned char MyTexture[HEIGHT][WIDTH][3]; // 3: rgb
float alphaTable[256] = { 0 };
float colorTable[256] = { 0 };
void MakeAlphaTable(int a, int b) {
	for (int i = 0; i < a; i++) {
		alphaTable[i] = 0;
	}
	for (int i = a; i < b; i++) {
		alphaTable[i] = (float)(i - a) / (b - a);
	}
	for (int i = b; i < 256; i++) {
		alphaTable[i] = 1;
	}
}
void MakeColorTable(int a, int b) {
	for (int i = 0; i < a; i++) {
		colorTable[i] = 0;
	}
	for (int i = a; i < b; i++) {
		colorTable[i] = (float)255.0 * (i - a) / (b - a);
	}
	for (int i = b; i < 256; i++) {
		colorTable[i] = 255;
	}
}
// 보간:INTERPOLATION
//float alphaFunc(int x) {
//	return alphaTable[x];
//}
// MPR: multi-plane reformation
unsigned char interpolation(float x, float y, float z) {
	//return vol[(int)z][(int)y][(int)x];
	//int ix = (int)p.x;
	//float wx = p.x - ix; // [0,1)
	//float den = (1 - wx) * vol[ix] + wx * vol[ix + 1];
	int ix = (int)x, iy = (int)y, iz = int(z);
	float wx = x - ix, wy = y - iy, wz = z - iz;
	float den = vol[iz][iy][ix] * (1 - wx) * (1 - wy) * (1 - wz) +
		vol[iz][iy][ix + 1] * wx * (1 - wy) * (1 - wz) +
		vol[iz][iy + 1][ix] * (1 - wx) * wy * (1 - wz) +
		vol[iz][iy + 1][ix + 1] * wx * wy * (1 - wz) +
		vol[iz + 1][iy][ix] * (1 - wx) * (1 - wy) * wz +
		vol[iz + 1][iy][ix + 1] * wx * (1 - wy) * wz +
		vol[iz + 1][iy + 1][ix] * (1 - wx) * wy * wz +
		vol[iz + 1][iy + 1][ix + 1] * wx * wy * wz;
	return (unsigned char)den;
}
inline int getBlockId(const vec3& p, int bid[3]) {
	bid[0] = ((int)p.x) >> 2; // 각 값이 0~64 < 100 확실
	bid[1] = ((int)p.y) >> 2;
	bid[2] = ((int)p.z) >> 2;
	return bid[0] * 10000 + bid[1] * 100 + bid[2];
}
void FillMyTexture() {
	int z = 100;
	int x = 150;
	for (int iy = 0; iy < HEIGHT; iy++) {
		for (int ix = 0; ix < WIDTH; ix++) {

			//if (t >= 225) {  // 좌표가 볼륨 데이터를 벗어나는 경우
			//	continue;
			//}
			//unsigned char Intensity = vol[t][s][x]; //[z][y][x]
			unsigned char maxvalue = 0;
			//for (int z = 0; z < 225; z++) {
			//	unsigned char Intensity = vol[z][t][s]; //[z][y][x]
			//	if (Intensity > maxvalue) {
			//		maxvalue = Intensity;
			//	}
			//}
			const float ss = 0.5; // super sampling
			vec3 rayStart = eye + u * (ix - WIDTH * 0.5) * ss + v * (iy - HEIGHT * 0.5) * ss;
			float tx0 = -rayStart.x / w.x;
			float tx1 = (255 - rayStart.x) / w.x;
			float xmin = __min(tx0, tx1);
			float xmax = __max(tx0, tx1);
			float ty0 = -rayStart.y / w.y;
			float ty1 = (255 - rayStart.y) / w.y;
			float ymin = __min(ty0, ty1);
			float ymax = __max(ty0, ty1);
			float tz0 = -rayStart.z / w.z;
			float tz1 = (255 - rayStart.z) / w.z;
			float zmin = __min(tz0, tz1);
			float zmax = __max(tz0, tz1);

			float tmin = __max(__max(xmin, ymin), zmin);
			float tmax = __min(__min(xmax, ymax), zmax);

			// 픽셀마다 밝기 초기화
			float asum = 0;
			float I = 0;
			for (float t = tmin; t < tmax; t += 1.0) {
				// 영상좌표 (ix,iy,t)가 전역좌표 어떤 점으로 가지?
				//vol(vx,vy,vz,1) = 특별한 변환행렬4x4 * (ix,iy,t,1)
				vec3 p = rayStart + w * t; // 현재점 좌표

				if (p.x < 1 || p.y < 1 || p.z < 1 ||
					p.x >= 254 || p.y >= 254 || p.z >= 223)
					continue;

				// 나는 어떤 블록 소속인가?
				int bid[3] = { 0 };
				int id = getBlockId(p, bid);
				unsigned char maxv = bmax[bid[2]][bid[1]][bid[0]];
				if (maxv < 50) { // 투명!
					for (float tt = 1; true; tt += 1.0) {
						vec3 q = p + w * tt;
						int tempid[3];
						int tid = getBlockId(q, tempid);
						if (id != tid) {
							t += (tt - 1);
							break;
						}
					}
					continue;
				}
				unsigned char Intensity = interpolation(p.x, p.y, p.z); // vol[(int)p.z][(int)p.y][(int)p.x];
				float alpha = alphaTable[Intensity];
				if (alpha == 0)
					continue;
				float col = colorTable[Intensity]; // 0~255. 색상

				float Ia = 0.2f, Id = 0.65, Is = 0.1; // 합이 1. overflow 방지. 약간 작게
				float ka = col, kd = col, ks = 255; // 거울 성질 주의
				float m = 20;

				// l : 조명, 방법1) (0,1,0) 방법2) l = w; 로 세팅. 광부 모델
				// h : L, v=관찰방향  평균 L + v(=w)   .Normalize()
				// n : 매번 구한다.
				float dx, dy, dz;
				dx = (interpolation(p.x + 1, p.y, p.z) - interpolation(p.x - 1, p.y, p.z)) / 2;
				dy = (interpolation(p.x, p.y + 1, p.z) - interpolation(p.x, p.y - 1, p.z)) / 2;
				dz = (interpolation(p.x, p.y, p.z + 1) - interpolation(p.x, p.y, p.z - 1)) / 2;
				vec3 n(dx, dy, dz), l = w, h = w;
				n.Normalize();

				float NL = fabs(n.Dot(l)); // 절대값, 음수값 방지
				float NH = fabs(n.Dot(h));
				float light = Ia * ka + Id * kd * NL + Is * ks * pow(NH, m); // 조명 계산

				float E = alpha * light; // 
				//float E = alpha * col; // 
				I += E * (1 - asum); // 
				asum += (1 - asum) * alpha;
				if (asum > 0.99)
					break;
				//
				//if (Intensity > maxvalue) {
				//	maxvalue = Intensity;
				//}
			}
			MyTexture[iy][ix][0] = I;
			MyTexture[iy][ix][1] = I;
			MyTexture[iy][ix][2] = I;


			//	((s + t) % 2) * 255;    //0는 흑색, 255는 백색
			// Intensity = rand()%255;
			//MyTexture[t][s][0] = maxvalue;             //Red 값에 할당
			//MyTexture[t][s][1] = maxvalue;             //Green 값에 할당
			//MyTexture[t][s][2] = maxvalue;             //Blue 값에 할당
		}
	}
}

void MyInit() {
	LoadVolume();
	makeBlock(vol, bmax, bmin);
	// 51
	MakeAlphaTable(50, 100);
	MakeColorTable(30, 80);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	LARGE_INTEGER t;
	QueryPerformanceFrequency(&t);

	LARGE_INTEGER a, b;
	QueryPerformanceCounter(&a);
	FillMyTexture();
	QueryPerformanceCounter(&b);
	double time = (b.QuadPart - a.QuadPart) / (double)t.QuadPart;
	printf("Time : %f\n", time);



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
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("openGL Sample Program");
	MyInit();
	glutDisplayFunc(MyDisplay);
	glutMainLoop();
	return 0;
}
