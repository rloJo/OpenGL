#define vec3_HEADER
#include <math.h>
#include <stdio.h>
class vec3 {
public:
	// 3차원 벡터의 멤버변수 x,y,z
	float x;
	float y;
	float z;


	// vec3의 생성자 - 멤버변수를 초기화한다.
	vec3();

	vec3(float init_x, float init_y, float init_z);

	// vec3의 복사 생성자 
	vec3(const vec3& rhs);

	// vec3의 소멸자
	~vec3();

	// 벡터의 내적 - 현재 벡터와 rhs 벡터간의 내적을 리턴한다.
	float Dot(const vec3& rhs);

	// 벡터의 외적 - u 벡터와 v 벡터간의 외적 벡터를 현재 벡터로 대입.  
	//     (this = u X v )
	vec3 Cross(const vec3& v);

	// 벡터에 스칼라 k 를 곱한 벡터를 리턴한다.
	vec3 Scale(float k);

	// 현재 벡터의 길이를 리턴한다.
	float Length();

	// 현재 벡터에 대한 단위벡터를 리턴한다.
	vec3 Normalize();

	// 연산자 재정의
	vec3 operator*(float a); // 현재 벡터를 a배 해서 리턴
	vec3 operator+(const vec3& rhs);		// 현재벡터와 rhs벡터의 덧셈한 결과 벡터를 리턴한다.
	vec3 operator-(const vec3& rhs);		// 현재벡터와 rhs벡터의 뺄셈한 결과 벡터를 리턴한다.
	void operator=(const vec3& rhs);		// rhs벡터의 모든 멤버변수를 현재 벡터로 복사한다.
	void Print() { printf("(%f, %f, %f)\n", x, y, z); }
};

vec3::vec3()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

vec3::vec3(float init_x, float init_y, float init_z)
{
	x = init_x;
	y = init_y;
	z = init_z;
}

vec3::vec3(const vec3& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
}


vec3::~vec3()
{

}


// 벡터의 내적 - 현재 벡터와 rhs 벡터간의 내적을 리턴한다.
float vec3::Dot(const vec3& rhs)
{
	return (x * rhs.x + y * rhs.y + z * rhs.z);
}


// 벡터의 외적 - u 벡터와 v 벡터간의 외적 벡터를 현재 벡터로 대입.  
//     (this = u X v )
vec3 vec3::Cross(const vec3& v)
{
	vec3 ret(
		y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x);
	return ret;
}

// 벡터에 스칼라 k 를 곱한 벡터를 리턴한다.
vec3 vec3::Scale(float k)
{
	return vec3(k * x, k * y, k * z);
}


// 현재 벡터의 길이를 리턴한다.
float vec3::Length()
{
	return sqrtf(x * x + y * y + z * z);
}

// 현재 벡터에 대한 단위벡터를 리턴한다.
// 0벡터이면 변경되지 않는다.
vec3 vec3::Normalize()
{
	float l = Length();

	if (l == 0)
		return *this;
	x /= l;
	y /= l;
	z /= l;
	return *this;
}

// 연산자 재정의
// 현재벡터와 rhs벡터의 덧셈한 결과 벡터를 리턴한다.
vec3 vec3::operator+(const vec3& rhs)
{
	return vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}


// 현재벡터와 rhs벡터의 뺄셈한 결과 벡터를 리턴한다.		
vec3 vec3::operator-(const vec3& rhs)
{
	return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

vec3 vec3::operator*(float a) {
	return vec3(a*x, a*y, a*z);
}

// rhs벡터의 모든 멤버변수를 현재 벡터로 복사한다.
void vec3::operator=(const vec3& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
}
