/*
CSCI 480
Assignment 3 Raytracer

Name: <Your name here>
*/

#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <iostream>
#include <string>

#include "vector3.h"
#include "Ray.h"

#define MAX_TRIANGLES 2000
#define MAX_SPHERES 10
#define MAX_LIGHTS 10
#define PI 3.1415926

char *filename=0;

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2
int mode=MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
#define WIDTH 640
#define HEIGHT 480

//#define WIDTH 160
//#define HEIGHT 120

// Aspect Ratio
double aspectRatio = static_cast<double> (WIDTH) / HEIGHT;
//the field of view of the camera
#define fov 60.0
// focal length
double focalLength = 1.0f;
// Camera forward direction
vector3 cameraForward(0.0f, 0.0f, -1.0f);
// Camera up direction
vector3 cameraUp(0.0f, 1.0f, 0.0f);
// Camera right direction
vector3 cameraRight(1.0f, 0.0f, 0.0f);
// Camera position
vector3 cameraPos(0.0f, 0.0f, 0.0f);


unsigned char buffer[HEIGHT][WIDTH][3];

struct Vertex
{
	double position[3];
	double color_diffuse[3];
	double color_specular[3];
	double normal[3];
	double shininess;
};

typedef struct _Triangle
{
	struct Vertex v[3];
} Triangle;

typedef struct _Sphere
{
	double position[3];
	double color_diffuse[3];
	double color_specular[3];
	double shininess;
	double radius;
} Sphere;

typedef struct _Light
{
	double position[3];
	double color[3];
} Light;

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles = 0;
int num_spheres = 0;
int num_lights = 0;

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);



//MODIFY THIS FUNCTION
void draw_scene()
{
	//unsigned int x,y;
	////simple output
	//for(x=0; x<WIDTH; x++)
	//{
	//	glPointSize(2.0);
	//	glBegin(GL_POINTS);
	//	for (y = 0; y < HEIGHT; y++)
	//	{
	//		plot_pixel(x, y, x % 256, y % 256, (x + y) % 256);
	//	}
	//	glEnd();
	//	glFlush();
	//}
	//printf("Done!\n"); fflush(stdout);

	// Send out rays first
	double tanHalfFOV = tan(fov * 0.5f / 180 * PI);
	double imagePlaneHeight = 2 * focalLength * tanHalfFOV;
	double imagePlaneWidth = imagePlaneHeight * aspectRatio;
	vector3 imagePlaneCenter = cameraForward * focalLength - cameraPos;
	// Origin is top left
	vector3 imagePlaneOrigin = vector3(imagePlaneCenter.x - imagePlaneWidth / 2, imagePlaneCenter.y - imagePlaneHeight / 2, imagePlaneCenter.z);
	// Pixel height
	double pixelHeight = imagePlaneHeight / HEIGHT;
	// Pixel width
	double pixelWidth = imagePlaneWidth / WIDTH;

	glPointSize(2.0);
	glBegin(GL_POINTS);
	for(int i = 0; i < HEIGHT; ++i)
	{
		glPointSize(2.0);
		glBegin(GL_POINTS);
		for(int j = 0; j < WIDTH; ++j)
		{
			vector3 pixelCenter = imagePlaneOrigin + cameraUp * i * pixelHeight + cameraRight * j * pixelWidth;
			vector3 rayDirection = pixelCenter.normalize();
			Ray ray(rayDirection, pixelCenter);

			double sphereT = DBL_MAX;
			int sphereIndex = -1;
			vector3 sphereIntersectionPos;
			vector3 sphereSurfaceNormal;

			double triangleT = DBL_MAX;
			double triangleU = 0.0f;
			double triangleV = 0.0f;
			int triangleIndex = -1;
			
			for(int k = 0; k < num_spheres; ++k)
			{
				vector3 center(spheres[k].position[0], spheres[k].position[1], spheres[k].position[2]);
				vector3 intersectionPos;
				vector3 surfaceNormal;
				double t = DBL_MAX;
				if (ray.IntersectSphere(center, spheres[k].radius, intersectionPos, surfaceNormal, t))
				{
					if(t < sphereT)
					{
						sphereIntersectionPos = intersectionPos;
						sphereSurfaceNormal = surfaceNormal;
						sphereIndex = k;
						sphereT = t;
					}
				}
			}

			for(int l = 0; l < num_triangles; ++l)
			{
				vector3 v1((triangles[l].v[0].position[0]),  (triangles[l].v[0].position[1]),  (triangles[l].v[0].position[2]));
				vector3 v2( (triangles[l].v[1].position[0]),  (triangles[l].v[1].position[1]),  (triangles[l].v[1].position[2]));
				vector3 v3( (triangles[l].v[2].position[0]),  (triangles[l].v[2].position[1]),  (triangles[l].v[2].position[2]));
				double u;
				double v;
				double t = DBL_MAX;

				if (ray.IntersectTriangle(v1, v2, v3, t, u, v))
				{
					if(t < triangleT)
					{
						triangleIndex = l;
						triangleT = t;
						triangleU = u;
						triangleV = v;
					}
				}
			}

			if(triangleIndex != -1 || sphereIndex != -1)
			{
				double objectDiffuse[3] = {0};
				double objectSpecular[3] = {0};
				double resultColor[3] = {0};

				// default to sphere
				vector3 origin = sphereIntersectionPos;
				vector3 originNormal = sphereSurfaceNormal;
				double originShiness = spheres[sphereIndex].shininess;
				objectDiffuse[0] = spheres[sphereIndex].color_diffuse[0] * 1;
				objectDiffuse[1] = spheres[sphereIndex].color_diffuse[1] * 1;
				objectDiffuse[2] = spheres[sphereIndex].color_diffuse[2] * 1;

				objectSpecular[0] = spheres[sphereIndex].color_specular[0] * 1;
				objectSpecular[1] = spheres[sphereIndex].color_specular[1] * 1;
				objectSpecular[2] = spheres[sphereIndex].color_specular[2] * 1;

				if (triangleT < sphereT)
				{
					// v0 correspond to (1 - u - v)
					// v1 correspond to u
					// v2 correspond to v
					vector3 v0(triangles[triangleIndex].v[0].position[0], triangles[triangleIndex].v[0].position[1], triangles[triangleIndex].v[0].position[2]);
					vector3 v1(triangles[triangleIndex].v[1].position[0], triangles[triangleIndex].v[1].position[1], triangles[triangleIndex].v[1].position[2]);
					vector3 v2(triangles[triangleIndex].v[2].position[0], triangles[triangleIndex].v[2].position[1], triangles[triangleIndex].v[2].position[2]);
					origin = v0 * (1 - triangleU - triangleV) + v1 * triangleU + v2 * triangleV;
					originNormal.x = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].normal[0] + triangleU * triangles[triangleIndex].v[1].normal[0] + triangleV * triangles[triangleIndex].v[2].normal[0];
					originNormal.y = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].normal[1] + triangleU * triangles[triangleIndex].v[1].normal[1] + triangleV * triangles[triangleIndex].v[2].normal[1];
					originNormal.z = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].normal[2] + triangleU * triangles[triangleIndex].v[1].normal[2] + triangleV * triangles[triangleIndex].v[2].normal[2];

					objectDiffuse[0] = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].color_diffuse[0] * 1 + triangleU * triangles[triangleIndex].v[1].color_diffuse[0] * 1 + triangleV * triangles[triangleIndex].v[2].color_diffuse[0] * 1;
					objectDiffuse[1] = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].color_diffuse[1] * 1 + triangleU * triangles[triangleIndex].v[1].color_diffuse[1] * 1 + triangleV * triangles[triangleIndex].v[2].color_diffuse[1] * 1;
					objectDiffuse[2] = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].color_diffuse[2] * 1 + triangleU * triangles[triangleIndex].v[1].color_diffuse[2] * 1 + triangleV * triangles[triangleIndex].v[2].color_diffuse[2] * 1;

					objectSpecular[0] = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].color_specular[0] * 1 + triangleU * triangles[triangleIndex].v[1].color_specular[0] * 1 + triangleV * triangles[triangleIndex].v[2].color_specular[0] * 1;
					objectSpecular[1] = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].color_specular[1] * 1 + triangleU * triangles[triangleIndex].v[1].color_specular[1] * 1 + triangleV * triangles[triangleIndex].v[2].color_specular[1] * 1;
					objectSpecular[2] = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].color_specular[2] * 1 + triangleU * triangles[triangleIndex].v[1].color_specular[2] * 1 + triangleV * triangles[triangleIndex].v[2].color_specular[2] * 1;
				
					originShiness = (1 - triangleU - triangleV) * triangles[triangleIndex].v[0].shininess + triangleU * triangles[triangleIndex].v[1].shininess + triangleV * triangles[triangleIndex].v[2].shininess;
				}

				bool renderPoint = false;
				for (int q = 0; q < num_lights; ++q)
				{
					// calculate direction -> (lightPos - origin).normalize()
					// create ray
					vector3 lightPos( (lights[q].position[0]),  (lights[q].position[1]),  (lights[q].position[2]));
					Ray shadowRay((lightPos - origin).normalize(), origin);
					bool shouldContinue = false;

					for (int w = 0; w < num_spheres; ++w)
					{
						// check intersection with all spheres, see if myself is sphere(triangleT > sphereT && sphereIndex == w)
						if(triangleT > sphereT && sphereIndex == w)
						{
							continue;
						}

						vector3 center( (spheres[w].position[0]),  (spheres[w].position[1]),  (spheres[w].position[2]));
						vector3 intersectionPos;
						vector3 surfaceNormal;
						double t;
						// if intersect, continue to next light
						if(shadowRay.IntersectSphere(center, spheres[w].radius, intersectionPos, surfaceNormal, t))
						{
							if ((intersectionPos - origin).magnitudeSqr() < (lightPos - origin).magnitudeSqr())
							{
								shouldContinue = true;
								break;
							}
						}
					}

					if (shouldContinue)
					{
						continue;
					}

					for (int w = 0; w < num_triangles; ++w)
					{
						if(sphereT > triangleT && triangleIndex == w)
						{
							continue;
						}

						vector3 v0(triangles[w].v[0].position[0], triangles[w].v[0].position[1], triangles[w].v[0].position[2]);
						vector3 v1(triangles[w].v[1].position[0], triangles[w].v[1].position[1], triangles[w].v[1].position[2]);
						vector3 v2(triangles[w].v[2].position[0], triangles[w].v[2].position[1], triangles[w].v[2].position[2]);
						double t;
						double u;
						double v;

						if(shadowRay.IntersectTriangle(v0, v1, v2, t, u, v))
						{
							if ((shadowRay.direction * t).magnitudeSqr() < (lightPos - origin).magnitudeSqr())
							{
								shouldContinue = true;
								break;
							}
						}
					}

					if (shouldContinue)
					{
						continue;
					}

					renderPoint = true;
					// add light color to point color
					originNormal = originNormal.normalize();
					vector3 dirToLight = shadowRay.direction.normalize();
					vector3 dirToCamera = (cameraPos -shadowRay.origin).normalize();
					vector3 reflectDir = originNormal * dirToLight.dot(originNormal) * 2 - dirToLight;
					reflectDir = reflectDir.normalize();
					double lDotN = max(0, dirToLight.dot(originNormal));
					lDotN = min(1, lDotN);
					double rDotV = max(0, reflectDir.dot(dirToCamera));
					rDotV = min(1, rDotV);

					resultColor[0] += lights[q].color[0] * (objectDiffuse[0] * lDotN + objectSpecular[0] * pow(rDotV, originShiness));
					resultColor[1] += lights[q].color[1] * (objectDiffuse[1] * lDotN + objectSpecular[1] * pow(rDotV, originShiness));
					resultColor[2] += lights[q].color[2] * (objectDiffuse[2] * lDotN + objectSpecular[2] * pow(rDotV, originShiness));
				}

				if (renderPoint)
				{
					resultColor[0] += ambient_light[0];
					resultColor[0] = max(0, resultColor[0]);
					resultColor[0] = min(1, resultColor[0]);
					

					resultColor[1] += ambient_light[1];
					resultColor[1] = max(0, resultColor[1]);
					resultColor[1] = min(1, resultColor[1]);
					

					resultColor[2] += ambient_light[2];
					resultColor[2] = max(0, resultColor[2]);
					resultColor[2] = min(1, resultColor[2]);
					

					plot_pixel(j, i, resultColor[0] * 255, resultColor[1] * 255, resultColor[2] * 255);
				}
			}
			else
			{
				plot_pixel(j, i, 255, 255, 255);
			}
		}
		glEnd();
		glFlush();
	}
	
}

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
	glColor3f(((double)r)/256.f,((double)g)/256.f,((double)b)/256.f);
	glVertex2i(x,y);
}

void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
	buffer[HEIGHT - y - 1][x][0] = r;
	buffer[HEIGHT - y - 1][x][1] = g;
	buffer[HEIGHT - y - 1][x][2] = b;
}

void plot_pixel(int x,int y,unsigned char r,unsigned char g, unsigned char b)
{
	plot_pixel_display(x, y, r, g, b);
	if (mode == MODE_JPEG)
	{
		plot_pixel_jpeg(x, y, r, g, b);
	}
}

void save_jpg()
{
	Pic *in = NULL;

	in = pic_alloc(640, 480, 3, NULL);
	printf("Saving JPEG file: %s\n", filename);

	memcpy(in->pix, buffer, 3 * WIDTH*HEIGHT);
	if (jpeg_write(filename, in))
	{
		printf("File saved Successfully\n");
	}
	else
	{
		printf("Error in Saving\n");
	}

	pic_free(in);      
}

void parse_check(char *expected,char *found)
{
	if (stricmp(expected, found))
	{
		char error[100];
		printf("Expected '%s ' found '%s '\n", expected, found);
		printf("Parse error, abnormal abortion\n");
		exit(0);
	}
}

void parse_doubles(FILE*file, char *check, double p[3])
{
	char str[100];
	fscanf(file, "%s", str);
	parse_check(check, str);
	fscanf(file, "%lf %lf %lf", &p[0], &p[1], &p[2]);
	printf("%s %lf %lf %lf\n", check, p[0], p[1], p[2]);
}

void parse_rad(FILE*file,double *r)
{
	char str[100];
	fscanf(file,"%s",str);
	parse_check("rad:",str);
	fscanf(file,"%lf",r);
	printf("rad: %f\n",*r);
}

void parse_shi(FILE*file,double *shi)
{
	char s[100];
	fscanf(file, "%s", s);
	parse_check("shi:", s);
	fscanf(file, "%lf", shi);
	printf("shi: %f\n", *shi);
}

int loadScene(char *argv)
{
	FILE *file = fopen(argv,"r");
	int number_of_objects;
	char type[50];
	int i;
	Triangle t;
	Sphere s;
	Light l;
	fscanf(file,"%i",&number_of_objects);

	printf("number of objects: %i\n",number_of_objects);
	char str[200];

	parse_doubles(file,"amb:",ambient_light);

	for(i=0;i < number_of_objects;i++)
	{
		fscanf(file,"%s\n",type);
		printf("%s\n",type);
		if(stricmp(type,"triangle")==0)
		{
			printf("found triangle\n");
			int j;

			for (j = 0; j < 3; j++)
			{
				parse_doubles(file, "pos:", t.v[j].position);
				parse_doubles(file, "nor:", t.v[j].normal);
				parse_doubles(file, "dif:", t.v[j].color_diffuse);
				parse_doubles(file, "spe:", t.v[j].color_specular);
				parse_shi(file, &t.v[j].shininess);
			}

			if (num_triangles == MAX_TRIANGLES)
			{
				printf("too many triangles, you should increase MAX_TRIANGLES!\n");
				exit(0);
			}
			triangles[num_triangles++] = t;
		}
		else if(stricmp(type,"sphere")==0)
		{
			printf("found sphere\n");

			parse_doubles(file, "pos:", s.position);
			parse_rad(file, &s.radius);
			parse_doubles(file, "dif:", s.color_diffuse);
			parse_doubles(file, "spe:", s.color_specular);
			parse_shi(file, &s.shininess);

			if (num_spheres == MAX_SPHERES)
			{
				printf("too many spheres, you should increase MAX_SPHERES!\n");
				exit(0);
			}
			spheres[num_spheres++] = s;
		}
		else if(stricmp(type,"light")==0)
		{
			printf("found light\n");
			parse_doubles(file,"pos:",l.position);
			parse_doubles(file,"col:",l.color);

			if(num_lights == MAX_LIGHTS)
			{
				printf("too many lights, you should increase MAX_LIGHTS!\n");
				exit(0);
			}
			lights[num_lights++] = l;
		}
		else
		{
			printf("unknown type in scene description:\n%s\n",type);
			exit(0);
		}
	}
  return 0;
}

void display()
{

}

void init()
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(0,WIDTH,0,HEIGHT,1,-1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  //hack to make it only draw once
	static int once = 0;
	if (!once)
	{
		draw_scene();
		if (mode == MODE_JPEG)
		{
			save_jpg();
		}
	}
	once = 1;
}

int main (int argc, char ** argv)
{
	if (argc < 2 || argc > 3)
	{
		printf("usage: %s <scenefile> [jpegname]\n", argv[0]);
		exit(0);
	}
	if (argc == 3)
	{
		mode = MODE_JPEG;
		filename = argv[2];
	}
	else if (argc == 2)
	{
		mode = MODE_DISPLAY;
	}

	glutInit(&argc, argv);
	loadScene(argv[1]);

	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	int window = glutCreateWindow("Ray Tracer");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
}
