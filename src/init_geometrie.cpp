// LAB_2_2D_ZOMBIE.cpp : 

#include "lib.h"
#define PI 3.14159265358979323846

#include <vector>

// viewport size
extern int width;
extern int height;
typedef struct { float x, y, r, g, b, a; } Point;
static float gameplayGroundLine = float(height) * 0.18f;

/*-------------SCENA---------------------*/
int		vertices_Lawn = 6;
Point* Lawn = new Point[vertices_Lawn];

int		vertices_Sky = 6;
Point* Sky = new Point[vertices_Sky];

/*_____________PARTICLES----------------------*/
int	   vertices_particle = 6;
Point* particle = new Point[vertices_particle];

/*-------------CHARACTER---------------------*/
#define CHARACTER_WIDTH  50.0f
#define CHARACTER_HEIGHT 119.0f
#define NORMAL_SPEED 5.0f
#define SPRINT_SPEED 8.0f

float	posxCharacter = float(width) / 5;
float	posyCharacter = gameplayGroundLine;
int		vertices_character = 66;
Point* character = new Point[vertices_character];

float	angle_Player = 30;
bool	goingUp = false;
double	verticalSpeedDown = 0;
double	verticalSpeed = 0;
float   horizontalSpeed = 0.0f;
bool    isSprinting = false;

/*-------------ZOMBIE-----------------------*/
#define ZOMBI_WIDTH  60.0f
#define ZOMBI_HEIGHT 131.0f 
int		vertices_zombi = 66;
float	posxZombi = 0.0f;
float	posyZombi = gameplayGroundLine;
Point* zombi = new Point[vertices_zombi];

/*-------------TREE-----------------------*/
#define SPEEDTREES 4.0f
#define MAXSIZETREE 40
#define trees_per_row 10

int		vertices_tree = 72;
Point* tree = new Point[vertices_tree];
float   dxTrees1 = 0;
float   dxTrees2 = (GLfloat)width;

int     sizeTrees[trees_per_row];
int     positionXTrees[trees_per_row];

/*-------------GROUND DETAILS---------------------*/
int		vertices_ground_detail = 6;
Point* ground_detail = new Point[vertices_ground_detail];

float   dxGround1 = 0;
float   dxGround2 = (GLfloat)width;

#define details_per_row 25
int     sizeGround[details_per_row];
int     positionXGround[details_per_row];
int     positionYGround[details_per_row];
#define MAXSIZEDETAIL 10

/*-------------OBSTACLES---------------------*/
int		vertices_headstone = 48;
Point*  headstone = new Point[vertices_headstone];
float   obstacle_timer = 0.0f;
#define MAX_OBSTACLES 3
#define MAXSIZEHEADSTONE 20

/*-------------CLOUDS-----------------------*/
#define CLOUDS_PER_ROW 8
#define MAXSIZECLOUD 50
#define SPEEDCLOUDS_FACTOR 0.5f

int		vertices_cloud = 120;
Point*  cloud = new Point[vertices_cloud];
float   dxClouds1 = 0;
float   dxClouds2 = (GLfloat)width;

int     sizeClouds[CLOUDS_PER_ROW];
int     positionXClouds[CLOUDS_PER_ROW];
int     positionYClouds[CLOUDS_PER_ROW];

/*-------------MOUNTAIN-----------------------*/
int		vertices_mountain = 9;
Point*  mountain = new Point[vertices_mountain];

/*--------------DIAMOND-----------------------*/
int		vertices_diamond = 6;
Point*  diamond = new Point[vertices_diamond];

/*-------------COLORI---------------------*/
vec4 col_white = { 1.0,1.0,1.0, 1.0 };
vec4 col_grey = { 0.5,0.5,0.5, 1.0 };
vec4 col_dirtygreen = {0.18, 0.27, 0.22, 1.0};
vec4 col_dirtyblue = { 0.0, 0.19, 0.36, 1.0 };
vec4 col_red = { 1.0, 0.0,0.0,1.0 };
vec4 col_black = { 0.0,0.0,0.0, 1.0 };
vec4 col_white2 = { 1,1,0.9, 1.0 };
vec4 col_pink = { 1, 0, 0.5, 1.0 };
vec4 col_yellow = { 1.0, 1.0, 0, 1.0 };
vec4 col_blue = { 0.4, 0.0, 1.0, 1.0 };
vec4 col_skin = { 0.98,0.85,0.87, 1.0 };
vec4 col_lightblue = { 0.53,0.7,0.98, 1.0 };
vec4 col_darkpurple = { 0.31, 0.06, 0.15, 1.0 };
vec4 peak_color = { 0.5, 0.0, 0.0, 1.0 };
vec4 base_color = { 0.43, 0.16, 0.34, 1.0 };
vec4 col_zombie_skin = { 0.5, 0.55, 0.5, 1.0 };
vec4 col_zombie_nail = { 0.1, 0.1, 0.1, 1.0 };
vec4 col_shadow = { 0.0f, 0.0f, 0.0f, 0.4f };
vec4 tombstone_color = { 0.3f, 0.3f, 0.35f, 1.0f };
vec4 cross_color = { 0.15f, 0.15f, 0.15f, 1.0f };
vec4 cloud_color = { 0.85f, 0.85f, 0.9f, 0.4f };
/*---------------------------------------*/

void draw_triangle(float x, float y, float base, float height, vec4 color, Point* plan, int firstPoint)
{
	// Triangolo con base orizzontale e punta in alto
	plan[0 + firstPoint].x = x;              plan[0 + firstPoint].y = y;
	plan[1 + firstPoint].x = x + base;       plan[1 + firstPoint].y = y;
	plan[2 + firstPoint].x = x + base / 2;     plan[2 + firstPoint].y = y + height;

	for (int i = 0; i < 3; i++) {
		plan[i + firstPoint].r = color.r;
		plan[i + firstPoint].g = color.g;
		plan[i + firstPoint].b = color.b;
		plan[i + firstPoint].a = color.a;
	}
}

void draw_circle(Point* data, int firstPoint, float centerX, float centerY, float radius, int segments, vec4 color, float angle)
{
	float angle_step = angle / segments;

	for (int i = 0; i < segments; i++){
		float angle1 = i * angle_step;
		float angle2 = (i + 1) * angle_step;

		Point p1, p2, p3;

		p1.x = centerX;
		p1.y = centerY;

		p2.x = centerX + radius * cos(radians(angle1));
		p2.y = centerY + radius * sin(radians(angle1));

		p3.x = centerX + radius * cos(radians(angle2));
		p3.y = centerY + radius * sin(radians(angle2));

		p1.r = p2.r = p3.r = color.r;
		p1.g = p2.g = p3.g = color.g;
		p1.b = p2.b = p3.b = color.b;
		p1.a = p2.a = p3.a = color.a;

		data[firstPoint + (i * 3) + 0] = p1;
		data[firstPoint + (i * 3) + 1] = p2;
		data[firstPoint + (i * 3) + 2] = p3;
	}
}

void draw_quad(Point* data, int firstPoint, Point p1, Point p2, Point p3, Point p4, vec4 color) {
	p1.r = p2.r = p3.r = p4.r = color.r;
	p1.g = p2.g = p3.g = p4.g = color.g;
	p1.b = p2.b = p3.b = p4.b = color.b;
	p1.a = p2.a = p3.a = p4.a = color.a;

	data[firstPoint + 0] = p1;
	data[firstPoint + 1] = p2;
	data[firstPoint + 2] = p3;

	data[firstPoint + 3] = p1;
	data[firstPoint + 4] = p3;
	data[firstPoint + 5] = p4;
}

/*********************         SCENE DESIGN          *********************************************/
void draw_plan(float x, float y, float width, float height, vec4 color_top, vec4 color_bot, Point* plan, int firstPoint)
{
	plan[0 + firstPoint].x = x;	plan[0 + firstPoint].y = y;
	plan[0 + firstPoint].r = color_top.r; plan[0 + firstPoint].g = color_top.g; plan[0 + firstPoint].b = color_top.b; plan[0 + firstPoint].a = color_top.a;
	plan[1 + firstPoint].x = x + width;	plan[1 + firstPoint].y = y;
	plan[1 + firstPoint].r = color_bot.r; plan[1 + firstPoint].g = color_bot.g; plan[1 + firstPoint].b = color_bot.b; plan[1 + firstPoint].a = color_bot.a;
	plan[2 + firstPoint].x = x + width;	plan[2 + firstPoint].y = y + height;
	plan[2 + firstPoint].r = color_top.r; plan[2 + firstPoint].g = color_top.g; plan[2 + firstPoint].b = color_top.b; plan[2 + firstPoint].a = color_top.a;

	plan[3 + firstPoint].x = x + width;	plan[3 + firstPoint].y = y + height;
	plan[3 + firstPoint].r = color_top.r; plan[3 + firstPoint].g = color_top.g; plan[3 + firstPoint].b = color_top.b; plan[3 + firstPoint].a = color_top.a;
	plan[4 + firstPoint].x = x;	plan[4 + firstPoint].y = y + height;
	plan[4 + firstPoint].r = color_top.r; plan[4 + firstPoint].g = color_top.g; plan[4 + firstPoint].b = color_top.b; plan[4 + firstPoint].a = color_top.a;
	plan[5 + firstPoint].x = x;	plan[5 + firstPoint].y = y;
	plan[5 + firstPoint].r = color_top.r; plan[5 + firstPoint].g = color_top.g; plan[5 + firstPoint].b = color_top.b; plan[5 + firstPoint].a = color_top.a;
}

void draw_lawn(float x, float y, float width, float height, vec4 color_top, vec4 color_bot, Point* plan, int firstPoint)
{
	plan[0 + firstPoint].x = x;
	plan[0 + firstPoint].y = y;
	plan[0 + firstPoint].r = color_bot.r; plan[0 + firstPoint].g = color_bot.g; plan[0 + firstPoint].b = color_bot.b; plan[0 + firstPoint].a = color_bot.a;

	plan[1 + firstPoint].x = x + width;
	plan[1 + firstPoint].y = y;
	plan[1 + firstPoint].r = color_bot.r; plan[1 + firstPoint].g = color_bot.g; plan[1 + firstPoint].b = color_bot.b; plan[1 + firstPoint].a = color_bot.a;

	plan[2 + firstPoint].x = x + width;
	plan[2 + firstPoint].y = y + height;
	plan[2 + firstPoint].r = color_top.r; plan[2 + firstPoint].g = color_top.g; plan[2 + firstPoint].b = color_top.b; plan[2 + firstPoint].a = color_top.a;

	plan[3 + firstPoint].x = x + width;
	plan[3 + firstPoint].y = y + height;
	plan[3 + firstPoint].r = color_top.r; plan[3 + firstPoint].g = color_top.g; plan[3 + firstPoint].b = color_top.b; plan[3 + firstPoint].a = color_top.a;

	plan[4 + firstPoint].x = x;
	plan[4 + firstPoint].y = y + height;
	plan[4 + firstPoint].r = color_top.r; plan[4 + firstPoint].g = color_top.g; plan[4 + firstPoint].b = color_top.b; plan[4 + firstPoint].a = color_top.a;

	plan[5 + firstPoint].x = x;
	plan[5 + firstPoint].y = y;
	plan[5 + firstPoint].r = color_bot.r; plan[5 + firstPoint].g = color_bot.g; plan[5 + firstPoint].b = color_bot.b; plan[5 + firstPoint].a = color_bot.a;
}


void draw_ground_detail(Point* detail_data) {
	draw_plan(0.0f, 0.0f, 1.0f, 1.0f, col_darkpurple, col_darkpurple, detail_data, 0);
}

void draw_zombi(Point* zombi) {
	float neckSize = 0.3;
	float headSize = 0.8;
	float bodyHeight = 0.65;
	float eyeSize = 0.1;

	float y_offset = 0.75f;

	// Testa
	draw_plan(0.3, 0.18 + y_offset, headSize, headSize * 0.35, col_grey, col_grey, zombi, 0);
	// Hair
	draw_plan(0.3, 0.4 + y_offset, headSize, eyeSize * 1.5, col_dirtygreen, col_dirtygreen, zombi, 6);
	// Collo
	draw_plan(0.45, (0.3 - neckSize * 0.8) + y_offset, neckSize, neckSize * 0.5, col_grey, col_grey, zombi, 12);
	// Maglietta
	draw_plan(0.3, (0.3 - neckSize * 0.8) + y_offset, headSize, -bodyHeight * 0.5, col_dirtygreen, col_dirtygreen, zombi, 18);
	// Pantaloni
	draw_plan(0.3, (0.3 - neckSize * 0.8 - bodyHeight * 0.5) + y_offset, headSize, -bodyHeight * 0.5, col_dirtyblue, col_dirtyblue, zombi, 24);
	// Braccio
	draw_plan(0.4, (0.3 - 0.27) + y_offset, 1.0, -0.1, col_grey, col_grey, zombi, 30);
	// Mano
	draw_plan(1.4, (0.3 - 0.27) + y_offset, -neckSize * 0.7, -0.2, col_grey, col_grey, zombi, 36);
	// Gamba 1
	draw_plan(0.35, (0.3 - neckSize * 0.8 - bodyHeight) + y_offset, neckSize * 0.8, -neckSize * 0.5, col_grey, col_grey, zombi, 42);
	// Gamba 2
	draw_plan(0.65, (0.3 - neckSize * 0.8 - bodyHeight) + y_offset, neckSize * 0.8, -neckSize * 0.5, col_grey, col_grey, zombi, 48);
	// Occhio
	draw_plan(0.78, 0.3 + y_offset, eyeSize * 2, eyeSize, col_white2, col_white2, zombi, 54);
	// Pupilla
	draw_plan(0.87, 0.32 + y_offset, eyeSize, eyeSize * 0.5, col_red, col_red, zombi, 60);
}


void draw_character(Point* character) {

	float x = 0;
	float y = 0;
	float neckSize = 0.3;
	float headSize = 0.8;
	float bodyHeight = 0.65;
	float eyeSize = 0.1;
	float legWidth = 0.18;
	float legHeight = 0.3;

	float y_offset = 0.74f;

	// Testa
	draw_plan(0.0, 0.18 + y_offset, headSize, headSize * 0.35, col_skin, col_skin, character, 0);
	// Hair
	draw_plan(0.0, 0.4 + y_offset, headSize, eyeSize * 1.5, col_black, col_black, character, 6);
	// Collo
	draw_plan(0.15, 0.06 + y_offset, neckSize, neckSize * 0.5, col_skin, col_skin, character, 12);
	// Hair 2
	draw_plan(-0.3, 0.55 + y_offset, eyeSize * 6, -headSize * 1.3, col_black, col_black, character, 18);
	// Maglietta
	draw_plan(0.0, 0.06 + y_offset, headSize, -bodyHeight * 0.5, col_white, col_white, character, 24);
	// Pantaloni
	draw_plan(0.0, (0.06 - bodyHeight * 0.5) + y_offset, headSize, -bodyHeight * 0.5, col_black, col_black, character, 30);
	// Braccio
	draw_plan(0.35, 0.0 + y_offset, -neckSize * 0.8, -(legHeight + bodyHeight) * 0.4, col_skin, col_skin, character, 36);
	// Gamba 1
	draw_plan(0.05, (0.06 - bodyHeight) + y_offset, neckSize * 0.8, -neckSize * 0.5, col_skin, col_skin, character, 42);
	// Gamba 2
	draw_plan(0.35, (0.06 - bodyHeight) + y_offset, neckSize * 0.8, -neckSize * 0.5, col_skin, col_skin, character, 48);
	// Occhio
	draw_plan(0.48, 0.3 + y_offset, eyeSize * 2, eyeSize, col_white2, col_white2, character, 54);
	// Pupilla
	draw_plan(0.6, 0.32 + y_offset, eyeSize, eyeSize * 0.5, col_lightblue, col_lightblue, character, 60);


}

void draw_tree(Point* tree) {

	draw_quad(tree, 0,{ 0.46f, 0.0f }, { 0.54f, 0.0f }, { 0.52f, 0.5f }, { 0.48f, 0.5f },	col_darkpurple);
	draw_quad(tree, 6,{ 0.48f, 0.5f }, { 0.52f, 0.5f }, { 0.51f, 0.75f }, { 0.49f, 0.75f }, col_darkpurple);
	draw_quad(tree, 12,	{ 0.48f, 0.55f }, { 0.49f, 0.53f }, { 0.25f, 0.68f }, { 0.23f, 0.70f },	col_darkpurple);
	draw_quad(tree, 18,	{ 0.52f, 0.58f }, { 0.51f, 0.56f }, { 0.72f, 0.72f }, { 0.74f, 0.74f },	col_darkpurple);
	draw_quad(tree, 24,	{ 0.49f, 0.68f }, { 0.495f, 0.67f }, { 0.20f, 0.85f }, { 0.19f, 0.87f }, col_darkpurple);
	draw_quad(tree, 30,	{ 0.51f, 0.70f }, { 0.505f, 0.69f }, { 0.78f, 0.82f }, { 0.79f, 0.84f }, col_darkpurple);
}


void draw_headstone(Point* headstone) {

	draw_plan(0.0f, 0.0f, 1.0f, 0.5f, tombstone_color, tombstone_color, headstone, 0);
	draw_circle(headstone, 6, 0.5f, 0.5f , 0.5f, 10, tombstone_color, 180.0f);
	draw_plan(0.47f, 0.5f, 0.06f, 0.3f, cross_color, cross_color, headstone, 36);
	draw_plan(0.4f, 0.68f, 0.2f, 0.06f, cross_color, cross_color, headstone, 42);
}

void draw_hand(Point* hand) {

	int vertex_offset = 0;
	int circle_segments = 10;
	float finger_width = 0.12f;
	float finger_spacing = 0.02f;
	float initial_x = 0.23f;
	float middle_finger_x = initial_x + finger_width + finger_spacing;
	float pinky_finger_x = initial_x + (finger_width + finger_spacing) * 3;
	float ring_finger_x = initial_x + (finger_width + finger_spacing) * 2;

	draw_circle(hand, 0, 0.5f, 0.05f, 0.25f, circle_segments, col_shadow, 360.0);

	draw_plan(0.3f, 0.0f, 0.4f, 0.5f, col_zombie_skin, col_zombie_skin, hand, circle_segments * 3);

	draw_plan(0.2f, 0.5f, 0.6f, 0.2f, col_zombie_skin, col_zombie_skin, hand, circle_segments * 3 + 6);

	draw_plan(0.80f, 0.5f, 0.15f, 0.25f, col_zombie_skin, col_zombie_skin, hand, circle_segments * 3 + 12);

	draw_plan(0.82f, 0.75f, 0.15f, 0.06f, col_zombie_nail, col_zombie_nail, hand, circle_segments * 3 + 18);

	draw_plan(initial_x, 0.7f, finger_width, 0.22f, col_zombie_skin, col_zombie_skin, hand, circle_segments * 3 + 24);

	draw_plan(initial_x, 0.92f, finger_width, 0.05f, col_zombie_nail, col_zombie_nail, hand, circle_segments * 3 + 30);

	draw_plan(middle_finger_x, 0.7f, finger_width, 0.28f, col_zombie_skin, col_zombie_skin, hand, circle_segments * 3 + 36);

	draw_plan(middle_finger_x, 0.98f, finger_width, 0.05f, col_zombie_nail, col_zombie_nail, hand, circle_segments * 3 + 42);

	draw_plan(ring_finger_x, 0.7f, finger_width, 0.25f, col_zombie_skin, col_zombie_skin, hand, circle_segments * 3 + 48);

	draw_plan(ring_finger_x, 0.95f, finger_width, 0.05f, col_zombie_nail, col_zombie_nail, hand, circle_segments * 3 + 54);

	draw_plan(pinky_finger_x, 0.7f, finger_width, 0.18f, col_zombie_skin, col_zombie_skin, hand, circle_segments * 3 + 60);

	draw_plan(pinky_finger_x, 0.88f, finger_width, 0.05f, col_zombie_nail, col_zombie_nail, hand, circle_segments * 3 + 66);

}

void draw_cloud(Point* cloud) {
	int segments_per_circle = 20;
	int vertices_per_circle = segments_per_circle * 3;

	draw_circle(cloud, 0, 0.0f, 0.0f, 0.6f, segments_per_circle, cloud_color, 360.0);
	draw_circle(cloud, vertices_per_circle, 0.8f, -0.2f, 0.4f, segments_per_circle, cloud_color, 360.0);
}

void draw_diamond(Point* diamond) {

	Point top = { 0.5f, 1.0f };
	Point left = { 0.0f, 0.5f };
	Point right = { 1.0f, 0.5f };
	Point bottom = { 0.5f, 0.0f };

	top.r = left.r = right.r = bottom.r = col_white.r;
	top.g = left.g = right.g = bottom.g = col_white.g;
	top.b = left.b = right.b = bottom.b = col_white.b;
	top.a = left.a = right.a = bottom.a = col_white.a;

	diamond[0] = top;
	diamond[1] = left;
	diamond[2] = right;

	diamond[3] = bottom;
	diamond[4] = right;
	diamond[5] = left;
}

void draw_mountain(Point* mountain) {
	Point p1 = { 0.0f, 0.0f, base_color.r, base_color.g, base_color.b, base_color.a };
	Point p2 = { 0.3f, 0.6f, base_color.r, base_color.g, base_color.b, base_color.a };
	Point p3 = { 0.6f, 1.0f, peak_color.r, peak_color.g, peak_color.b, peak_color.a };
	Point p4 = { 0.8f, 0.4f, base_color.r, base_color.g, base_color.b, base_color.a };
	Point p5 = { 1.0f, 0.0f, base_color.r, base_color.g, base_color.b, base_color.a };

	mountain[0] = p1;
	mountain[1] = p2;
	mountain[2] = p3;

	mountain[3] = p1;
	mountain[4] = p3;
	mountain[5] = p4;

	mountain[6] = p1;
	mountain[7] = p4;
	mountain[8] = p5;
}

void draw_particles(Point* particle) {
	draw_plan(0.0f, 0.0f, 1.0f, 1.0f, col_white, col_white, particle, 0);
}