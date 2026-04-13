#include "lib.h"
#include "ShaderMaker.h"
#include "gestione_callback.h"

static unsigned int programId;
static double limitFPS = 1.0 / 60.0;

unsigned int VAO_SKY, VAO_LAWN, VAO_CHARACTER, VAO_ZOMBI, VAO_TREE, VAO_GROUND_DETAIL, VAO_HEADSTONE, VAO_HAND, VAO_CLOUD, VAO_DIAMOND, VAO_MOUNTAIN, VAO_PARTICLE;
unsigned int VBO_SKY, VBO_LAWN, VBO_CHARACTER, VBO_ZOMBI, VBO_TREE, VBO_GROUND_DETAIL, VBO_HEADSTONE, VBO_HAND, VBO_CLOUD, VBO_DIAMOND, VBO_MOUNTAIN, VBO_PARTICLE;
unsigned int MatProj, MatModel;

typedef struct { float x, y, r, g, b, a; } Point;

void draw_plan(float x, float y, float width, float height, vec4 color_top, vec4 color_bot, Point* plan, int firstPoint);
void draw_lawn(float x, float y, float width, float height, vec4 color_top, vec4 color_bot, Point* plan, int firstPoint);
void draw_character(Point* character);
void draw_zombi(Point* zombi);
void draw_ground_detail(Point* detail);
void draw_headstone(Point* headstone);
void draw_hand(Point* hand);
void draw_diamond(Point* diamond);
void draw_mountain(Point* mountain);
void draw_cloud(Point* cloud);
void draw_tree(Point* tree);
void draw_particles(Point* particle);

// Global variables -------------------------------------------------------------------

// viewport size
int width = 1200;
int height = 720;

mat4 Projection;
mat4 Model;

static float gameplayGroundLine = float(height) * 0.18f;
static float backgroundLawnHeight = float(height) * 0.2f;

/*-------------SCENE---------------------*/
extern int		vertices_Lawn;
extern Point*	Lawn;
extern int		vertices_Sky;
extern Point*	Sky;

//PARTICLE SYSTEM
/*_____________PARTICLES-------------------*/
#define			MAX_PARTICLES 500

struct Particle {
	vec2 position;
	vec2 velocity;
	vec4 color;
	float life;
	bool isActive;
	vec2 explosionVelocity;
};

Particle		particles[MAX_PARTICLES];
int				lastUsedParticle = 0;

extern int		vertices_particle;
extern Point*	particle;

// OGGETTI DI SCENA
/*--------------MOUNTAIN------------------*/
#define MAX_MOUNTAINS 4
#define SPEED_MOUNTAINS 0.8f // Parallax

extern int		vertices_mountain;
extern Point*	mountain;

struct Mountain {
	float x, y, width, height;
	bool isActive;
};

Mountain mountains[MAX_MOUNTAINS];
float mountain_timer = 5.0f;

/*-------------TREE-----------------------*/
#define         MAXSIZETREE 40
#define         trees_per_row 10

extern int		vertices_tree;
extern Point*   tree;
extern float    dxTrees1;
extern float    dxTrees2;

extern int      sizeTrees[trees_per_row];
extern int      positionXTrees[trees_per_row];

/*---------------GROUND DETAIL------------------*/
#define			details_per_row 25
#define			MAXSIZEDETAIL 10

extern int		vertices_ground_detail;
extern Point*	ground_detail;

extern float	dxGround1;
extern float	dxGround2;

extern int		sizeGround[details_per_row];
extern int		positionXGround[details_per_row];
extern int		positionYGround[details_per_row];

/*-----------------------CLOUD------------------------ */
#define			MAXSIZECLOUD 50
#define			cloud_per_row 4

extern int		vertices_cloud;
extern Point*   cloud;
extern float    dxClouds1;
extern float    dxClouds2;

extern int      sizeClouds[cloud_per_row];
extern int      positionXClouds[cloud_per_row];
extern int      positionYClouds[cloud_per_row];

//INTERACTIONS
/*---------------- - OBSTACLES-------------------- - */
#define			MAX_OBSTACLES 6

extern int		vertices_headstone;
extern Point*	headstone;
extern float	obstacle_timer;

enum ObstacleType {
	HEADSTONE,
	HAND,
	DIAMOND
};

struct Obstacle {
	float x, y;
	float width, height;
	bool isActive;
	ObstacleType type;
};

Obstacle obstacles[MAX_OBSTACLES];

/*-------------DIAMOND--------------------*/
extern int		vertices_diamond;
extern Point*	diamond;


//CHARACTERS
/*-------------ZOMBIE---------------------*/
#define			ZOMBI_WIDTH  60.0f
#define			ZOMBI_HEIGHT 131.0f 

extern int		vertices_zombi;
extern float	posxZombi;
extern float	posyZombi;
extern Point*   zombi;

/*-------------CHARACTER---------------------*/
#define			CHARACTER_WIDTH  50.0f
#define			CHARACTER_HEIGHT 119.0f
#define			NORMAL_SPEED 5.0f
#define			SPRINT_SPEED 8.0f

extern Point*   character;
extern float	posyCharacter;
extern int		vertices_character;
extern float	posxCharacter;

extern float	angle_Player;
extern bool		goingUp;
extern double	verticalSpeed;
extern float	horizontalSpeed;
extern bool		isSprinting;

bool			hit = false;
bool			pressing_top = false;
bool			pressing_left = false;
bool			pressing_right = false;
bool			isGrounded = true;

/*-----------------WORLD--------------------------*/
float			speed = 4.0f;
float			road = 0.0f;
int				nextSpeedMilestone = 50;
int				diamondsCollected = 0;
float			diamond_timer = 8.0f;
/*---------------------------------------------------*/


int findParticles() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (!particles[i].isActive) {
			lastUsedParticle = i;
			return i;
		}
	}
	return 0;
}

void emitParticles(vec2 position, int count) {
	for (int i = 0; i < count; i++) {
		float angle = (rand() % 360) * 3.14159f / 180.0f;
		float explosionSpeed = 150.0f + (rand() % 100);

		int particleIndex = findParticles();
		auto& p = particles[particleIndex];

		p.isActive = true;
		p.position = position;
		p.life = 0.3f + (rand() % 5) / 10.0f;
		p.explosionVelocity = vec2(cos(angle), sin(angle)) * explosionSpeed;
		p.velocity = vec2(-speed, 0.0f);
		p.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}


void updateParticles(float deltaTime) {

	for (int i = 0; i < MAX_PARTICLES; i++) {
		Particle& p = particles[i];
		if (p.isActive) {
			
			float damping = 0.95f;
			vec2 totalVelocity = p.explosionVelocity * deltaTime + p.velocity * deltaTime;
			p.life -= deltaTime;
			if (p.life <= 0.0f) {
				p.isActive = false;
				continue;
			}
			p.explosionVelocity *= damping;
			p.position += totalVelocity;
			p.velocity.x = -speed;

		}
	}
}

void updateSpeed() {
	if (road >= nextSpeedMilestone) {
		speed += 0.5f;
		nextSpeedMilestone += 50;

		printf("Current speed: %f | Next milestone: %d metres\n", speed, nextSpeedMilestone);
	}
}

void updateWorld(int value) {

	if (!hit) {
		dxTrees1 -= speed;
		dxTrees2 -= speed;

		if (dxTrees1 + width <= 0) {
			dxTrees1 = width;
		}
		if (dxTrees2 + width <= 0) {
			dxTrees2 = width;
		}

		dxGround1 -= speed;
		dxGround2 -= speed;

		if (dxGround1 + width <= 0) {
			dxGround1 = width;
		}
		if (dxGround2 + width <= 0) {
			dxGround2 = width;
		}

		dxClouds1 -= speed;
		dxClouds2 -= speed;

		if (dxClouds1 + width <= 0) {
			dxClouds1 = width;
		}
		if (dxClouds2 + width <= 0) {
			dxClouds2 = width;
		}

		mountain_timer -= (1.0f / 60.0f);

		for (int i = 0; i < MAX_MOUNTAINS; i++) {
			if (mountains[i].isActive) {
				mountains[i].x -= speed * SPEED_MOUNTAINS; // Effetto parallasse
				if (mountains[i].x + mountains[i].width < 0) {
					mountains[i].isActive = false;
				}
			}
		}
		if (mountain_timer <= 0) {
			for (int i = 0; i < MAX_MOUNTAINS; i++) {
				if (!mountains[i].isActive) {
					mountains[i].isActive = true;
					mountains[i].x = width + 50.0f;
					mountains[i].y = backgroundLawnHeight;
					mountains[i].width = 400 + (rand() % 300); 
					mountains[i].height = 350 + (rand() % 200);

					mountain_timer = 7.0f + (rand() % 10);
					break;
				}
			}
		}

		// Obstacles
		for (int i = 0; i < MAX_OBSTACLES; i++) {
			if (obstacles[i].isActive) {
				obstacles[i].x -= speed;
				if (obstacles[i].x + obstacles[i].width < 0) {
					obstacles[i].isActive = false;
				}
			}
			float zombiLeft = posxZombi;
			float zombiRight = posxZombi + ZOMBI_WIDTH;
			float zombiBottom = posyZombi;
			float zombiTop = posyZombi + ZOMBI_HEIGHT;

			const auto& obs = obstacles[i];
			float obsLeft = obs.x;
			float obsRight = obs.x + obs.width;
			float obsBottom = obs.y;
			float obsTop = obs.y + obs.height;
			// ----------------------------------------------------

			if (zombiRight > obsLeft && zombiLeft < obsRight && zombiTop > obsBottom && zombiBottom < obsTop){
				obstacles[i].isActive = false;
			}
		}

		obstacle_timer -= limitFPS;


		if (obstacle_timer <= 0) {
			for (int i = 0; i < MAX_OBSTACLES; i++) {
				if (!obstacles[i].isActive) {
					obstacles[i].isActive = true;
					if (rand() % 3 == 0) {
						obstacles[i].type = HAND;
						obstacles[i].width = 50.0f;
						obstacles[i].height = 75.0f;
					}
					else {
						obstacles[i].type = HEADSTONE;
						obstacles[i].width = 100 + rand() % 30;
						obstacles[i].height = 120 + rand() % 70;
					}

					obstacles[i].x = width + 100.0f;
					obstacles[i].y = gameplayGroundLine;

					obstacle_timer = 1.2f + (rand() % 30) / 10.0f;
					break;
				}
			}
		}
		diamond_timer -= limitFPS;

		if (diamond_timer <= 0) {
			for (int i = 0; i < MAX_OBSTACLES; i++) {
				if (!obstacles[i].isActive) {
					obstacles[i].isActive = true;
					obstacles[i].type = DIAMOND;
					obstacles[i].width = 25.0f;
					obstacles[i].height = 25.0f;

					float min_y = gameplayGroundLine + 20;
					float max_y = height * 0.6f;

					obstacles[i].x = width + 200.0f;
					obstacles[i].y = min_y + (rand() % int(max_y - min_y));

					diamond_timer = 8.0f + (rand() % 5);
					break;
				}
			}
		}
	}
}

// character
void update(int a)
{
	if (!hit)
	{
		if (isSprinting) horizontalSpeed = SPRINT_SPEED;
		else horizontalSpeed = NORMAL_SPEED;
		

		if (pressing_top && isGrounded) {
			verticalSpeed = 20.0f;
			isGrounded = false;
			goingUp = true;
		}
		if (!isGrounded) {
			verticalSpeed -= 0.8f;
			if (verticalSpeed <= 0) {
				goingUp = false;
			}
		}
		posyCharacter += verticalSpeed;

		if (pressing_left) posxCharacter -= horizontalSpeed;
		else if (pressing_right) posxCharacter += horizontalSpeed;
		else posxCharacter -= speed;

		if (posxCharacter < 0) {
			posxCharacter = 0;
		}
		if (posxCharacter > width - CHARACTER_WIDTH) {
			posxCharacter = width - CHARACTER_WIDTH;
		}

		bool isStandingOnObstacle = false;
		float prevCharBottom = posyCharacter - verticalSpeed;

		for (int i = 0; i < MAX_OBSTACLES; i++) {
			if (obstacles[i].isActive) {

				float charLeft = posxCharacter;
				float charRight = posxCharacter + CHARACTER_WIDTH;
				float charBottom = posyCharacter;
				float charTop = posyCharacter + CHARACTER_HEIGHT;

				const auto& obs = obstacles[i];
				float obsLeft = obs.x;
				float obsRight = obs.x + obs.width;
				float obsBottom = obs.y;
				float obsTop = obs.y + obs.height;

				// Collision
				if (charRight > obsLeft && charLeft < obsRight && charTop > obsBottom && charBottom < obsTop) {
					if (obs.type == DIAMOND){
						// Particles effect
						vec2 diamondCenter = vec2(obs.x + obs.width / 2.0f, obs.y + obs.height / 2.0f);
						emitParticles(diamondCenter, 40);

						obstacles[i].isActive = false;
						diamondsCollected++;
						printf("\rCollected gems: %d\n", diamondsCollected);
						continue;
					}
					else if (obs.type == HAND) { // Mano zombie, morte
						hit = true;
						break;
					}
					else {
						float headstoneMargin = 15.0f;
						float  headstoneLeft = obsLeft + headstoneMargin;
						float  headstoneRight = obsRight - headstoneMargin;

						if (verticalSpeed <= 0 
							&& prevCharBottom >= obsTop 
							&& charRight > headstoneLeft 
							&& charLeft < headstoneRight) {

							posyCharacter = obsTop;
							verticalSpeed = 0;
							isGrounded = true;
							isStandingOnObstacle = true;
						}

						else if (!goingUp) {
							float fromLeft = charRight - obsLeft;
							float fromRight = obsRight - charLeft;

							if (fromLeft < fromRight) posxCharacter -= fromLeft;
							else posxCharacter += fromRight;
						}
					}
				}
			}
		}

		if (!isStandingOnObstacle) {
			if (posyCharacter <= gameplayGroundLine) {
				posyCharacter = gameplayGroundLine;
				verticalSpeed = 0;
				isGrounded = true;
				goingUp = false;
			}
			else isGrounded = false;
		}
		road += 0.02f; 
	}

	// COLLISION CHARACTER-ZOMBIE
	float finalCharLeft = posxCharacter;
	float finalCharRight = posxCharacter + CHARACTER_WIDTH;
	float finalCharBottom = posyCharacter;
	float finalCharTop = posyCharacter + CHARACTER_HEIGHT;

	float finalZombiLeft = posxZombi;
	float finalZombiRight = posxZombi + ZOMBI_WIDTH;
	float finalZombiBottom = posyZombi;
	float finalZombiTop = posyZombi + ZOMBI_HEIGHT;

	if (finalCharRight > finalZombiLeft &&
		finalCharLeft < finalZombiRight &&
		finalCharTop > finalZombiBottom &&
		finalCharBottom < finalZombiTop) {
		hit = true;
	}
}

void restart(void)
{
	printf("RESTART! \n");
	
	hit = false;
	speed =4.0f;
	road = 0.0f;
	posxCharacter = width / 5.0f;
	posyCharacter = gameplayGroundLine;
	isGrounded = true;
	verticalSpeed = 0;
	diamondsCollected = 0;
	diamond_timer = 8.0f;

	//reset montagna
	for (int i = 0; i < MAX_MOUNTAINS; i++) {
		mountains[i].isActive = false;
	}
	mountain_timer = 5.0f;

	//reset ostacoli
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		obstacles[i].isActive = false;
	}
	obstacle_timer = 3.0f;
}

void initShader(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_C_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_C_M.glsl";
	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void init(void)
{
	Projection = ortho(0.0f, float(width), 0.0f, float(height));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");

	/***************************************
	*Geometry construction and colors of the SKY
	****************************************/
	vec4 col_purple = { 0.5,0.0,0.5, 1.0 };
	vec4 col_darkred = { 0.5,0.0,0.0, 1.0 };
	vec4 col_lightpurple = { 0.38,0.14,0.30, 1.0 };

	draw_lawn(0, backgroundLawnHeight, width, height, col_purple, col_darkred, Sky, 0);
	//Sky VAO generation
	glGenVertexArrays(1, &VAO_SKY);
	glBindVertexArray(VAO_SKY);
	glGenBuffers(1, &VBO_SKY);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_SKY);
	glBufferData(GL_ARRAY_BUFFER, vertices_Sky * sizeof(Point), &Sky[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	/***************************************
	*Geometry and color construction of the LAWN
	****************************************/
	draw_lawn(0, 0, width, backgroundLawnHeight, col_lightpurple, col_lightpurple, Lawn, 0);
	//VAO 
	glGenVertexArrays(1, &VAO_LAWN);
	glBindVertexArray(VAO_LAWN);
	glGenBuffers(1, &VBO_LAWN);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_LAWN);
	glBufferData(GL_ARRAY_BUFFER, vertices_Lawn * sizeof(Point), &Lawn[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);


	/***************************************
	* Geometry and color construction of the CLOUDS
	****************************************/
	draw_cloud(cloud);
	glGenVertexArrays(1, &VAO_CLOUD);
	glBindVertexArray(VAO_CLOUD);
	glGenBuffers(1, &VBO_CLOUD);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_CLOUD);
	glBufferData(GL_ARRAY_BUFFER, vertices_cloud * sizeof(Point), &cloud[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	/***************************************
	* Geometry for GROUND DETAILS
	****************************************/
	draw_ground_detail(ground_detail);
	glGenVertexArrays(1, &VAO_GROUND_DETAIL);
	glBindVertexArray(VAO_GROUND_DETAIL);
	glGenBuffers(1, &VBO_GROUND_DETAIL);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_GROUND_DETAIL);
	glBufferData(GL_ARRAY_BUFFER, vertices_ground_detail * sizeof(Point), &ground_detail[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);


	/***************************************
   * Geometry and color construction of the ZOMBIE HAND
   ****************************************/
	int temp_vertices_hand = 102;
	Point* temp_hand_geom = new Point[temp_vertices_hand];
	draw_hand(temp_hand_geom);

	glGenVertexArrays(1, &VAO_HAND);
	glBindVertexArray(VAO_HAND);
	glGenBuffers(1, &VBO_HAND);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_HAND);
	glBufferData(GL_ARRAY_BUFFER, temp_vertices_hand * sizeof(Point), &temp_hand_geom[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	delete[] temp_hand_geom;

	/***************************************
	* Geometry and color construction of the DIAMOND
	****************************************/
	draw_diamond(diamond);
	glGenVertexArrays(1, &VAO_DIAMOND);
	glBindVertexArray(VAO_DIAMOND);
	glGenBuffers(1, &VBO_DIAMOND);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_DIAMOND);
	glBufferData(GL_ARRAY_BUFFER, vertices_diamond * sizeof(Point), &diamond[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	/***************************************
	* Geometry and color construction of the HEADSTONE
	****************************************/
	draw_headstone(headstone);
	glGenVertexArrays(1, &VAO_HEADSTONE);
	glBindVertexArray(VAO_HEADSTONE);
	glGenBuffers(1, &VBO_HEADSTONE);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_HEADSTONE);
	glBufferData(GL_ARRAY_BUFFER, vertices_headstone * sizeof(Point), &headstone[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);


	/***************************************
	* Geometry and color construction of the ZOMBIE
	****************************************/

	draw_zombi(zombi);
	glGenVertexArrays(1, &VAO_ZOMBI);
	glBindVertexArray(VAO_ZOMBI);
	glGenBuffers(1, &VBO_ZOMBI);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ZOMBI);
	glBufferData(GL_ARRAY_BUFFER, vertices_zombi * sizeof(Point), &zombi[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//Defines the color that will be assigned to the screen
	glClearColor(0.3, 0.3, 0.3, 1.0);

	/***************************************
	* Geometry for PARTICLES
	****************************************/
	draw_particles(particle);
	glGenVertexArrays(1, &VAO_PARTICLE);
	glBindVertexArray(VAO_PARTICLE);
	glGenBuffers(1, &VBO_PARTICLE);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_PARTICLE);
	glBufferData(GL_ARRAY_BUFFER, vertices_particle * sizeof(Point), &particle[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	/***************************************
	* Geometry and color construction of the character
	****************************************/
	draw_character(character);
	glGenVertexArrays(1, &VAO_CHARACTER);
	glBindVertexArray(VAO_CHARACTER);
	glGenBuffers(1, &VBO_CHARACTER);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_CHARACTER);
	glBufferData(GL_ARRAY_BUFFER, vertices_character * sizeof(Point), &character[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//Defines the color that will be assigned to the screen
	glClearColor(0.3, 0.3, 0.3, 1.0);

	/***************************************
	* Geometry and color construction of the TREE
	****************************************/
	draw_tree(tree);
	glGenVertexArrays(1, &VAO_TREE);
	glBindVertexArray(VAO_TREE);
	glGenBuffers(1, &VBO_TREE);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_TREE);
	glBufferData(GL_ARRAY_BUFFER, vertices_tree * sizeof(Point), &tree[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	/***************************************
	* Geometry and color construction of the MOUNTAIN
	****************************************/
	draw_mountain(mountain);
	glGenVertexArrays(1, &VAO_MOUNTAIN);
	glBindVertexArray(VAO_MOUNTAIN);
	glGenBuffers(1, &VBO_MOUNTAIN);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_MOUNTAIN);
	glBufferData(GL_ARRAY_BUFFER, vertices_mountain * sizeof(Point), &mountain[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void drawScene(void)
{
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(programId);

	/**************
	* draw SKY
	***************/
	Model = mat4(1.0);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(VAO_SKY);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //GL_LINE TO SEE LINEE
	glDrawArrays(GL_TRIANGLES, 0, vertices_Sky);
	glBindVertexArray(0);

	/************
	*draw LAWN
	*************/
	Model = mat4(1.0);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(VAO_LAWN);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, vertices_Lawn);
	glBindVertexArray(0);

	/****************************
	* draw MOUNTAINS
	*****************************/
	glBindVertexArray(VAO_MOUNTAIN);
	for (int i = 0; i < MAX_MOUNTAINS; i++) {
		if (mountains[i].isActive) {
			const auto& m = mountains[i];
			Model = mat4(1.0);
			Model = translate(Model, vec3(m.x, m.y, 0.0f));
			Model = scale(Model, vec3(m.width, m.height, 1.0f));
			glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
			glDrawArrays(GL_TRIANGLES, 0, vertices_mountain);
		}
	}
	glBindVertexArray(0);

	/****************************
	* draw GROUND DETAILS
	*****************************/
	float distanceDetailsColumn = ((float)width + 100) / details_per_row;
	glBindVertexArray(VAO_GROUND_DETAIL);

	for (int i = 0; i < details_per_row; i++) {
		float posxDetail = i * distanceDetailsColumn;
		float detailFinalSize = (float)sizeGround[i];


		Model = mat4(1.0);
		Model = translate(Model, vec3(dxGround1 + posxDetail + positionXGround[i], (float)positionYGround[i], 0.0f));
		Model = scale(Model, vec3(detailFinalSize, detailFinalSize / 2.0f, 1.0f));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, vertices_ground_detail);
	}

	for (int i = 0; i < details_per_row; i++) {
		float posxDetail = i * distanceDetailsColumn;
		float detailFinalSize = (float)sizeGround[i];
		Model = mat4(1.0);
		Model = translate(Model, vec3(dxGround2 + posxDetail + positionXGround[i], (float)positionYGround[i], 0.0f));
		Model = scale(Model, vec3(detailFinalSize, detailFinalSize / 2.0f, 1.0f));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, vertices_ground_detail);
	}
	glBindVertexArray(0);

	/****************************
	* draw TREE*************
	*****************************/
	float distanceTreesColumn = ((float)width + 100) / trees_per_row;
	glBindVertexArray(VAO_TREE);

	for (int i = 0; i < trees_per_row; i++) {
		float posxTree = i * distanceTreesColumn;
		float treeBaseSize = 40.0f;
		float treeFinalSize = treeBaseSize + sizeTrees[i];

		Model = mat4(1.0);
		Model = translate(Model, vec3(dxTrees1 + posxTree + positionXTrees[i], backgroundLawnHeight, 0.0));
		Model = scale(Model, vec3(treeFinalSize * 5.0f, treeFinalSize * 10.0f, 1.0));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, vertices_tree);
	}

	for (int i = 0; i < trees_per_row; i++) {
		float posxTree = i * distanceTreesColumn;
		float treeBaseSize = 40.0f;
		float treeFinalSize = treeBaseSize + sizeTrees[i];

		Model = mat4(1.0);
		Model = translate(Model, vec3(dxTrees2 + posxTree + positionXTrees[i], backgroundLawnHeight, 0.0));
		Model = scale(Model, vec3(treeFinalSize * 5.0f, treeFinalSize * 10.0f, 1.0));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, vertices_tree);
	}
	glBindVertexArray(0);

	/****************************
	* draw PARTICLES
	*****************************/
	glBindVertexArray(VAO_PARTICLE);
	for (int i = 0; i < MAX_PARTICLES; i++) {
		const auto& p = particles[i];
		if (p.isActive) {
			Model = mat4(1.0);
			Model = translate(Model, vec3(p.position.x, p.position.y, 0.0f));
			Model = scale(Model, vec3(3.0f, 3.0f, 1.0f));
			glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
			glDrawArrays(GL_TRIANGLES, 0, vertices_particle);
		}
	}
	glBindVertexArray(0);

	/****************************
	* draw CHARACTER*************
	*****************************/
	Model = mat4(1.0);
	Model = translate(Model, vec3(posxCharacter, posyCharacter, 0.0f));
	if (hit) {
		Model = translate(Model, vec3(CHARACTER_WIDTH / 2.0f, CHARACTER_HEIGHT / 2.0f, 0.0f));
		Model = rotate(Model, radians(180.0f), vec3(0.0, 0.0, 1.0));
		Model = translate(Model, vec3(-CHARACTER_WIDTH / 2.0f, -CHARACTER_HEIGHT / 2.0f, 0.0f));
	}
	else if (goingUp) {
		Model = translate(Model, vec3(CHARACTER_WIDTH / 2.0f, CHARACTER_HEIGHT / 2.0f, 0.0f));
		Model = rotate(Model, radians(angle_Player), vec3(0.0, 0.0, 1.0));
		Model = translate(Model, vec3(-CHARACTER_WIDTH / 2.0f, -CHARACTER_HEIGHT / 2.0f, 0.0f));
	}
	Model = scale(Model, vec3(CHARACTER_WIDTH, CHARACTER_HEIGHT, 1.0f));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(VAO_CHARACTER);
	glDrawArrays(GL_TRIANGLES, 0, vertices_character);
	glBindVertexArray(0);

	/****************************
	* draw ZOMBIE*************
	*****************************/
	Model = mat4(1.0);
	Model = translate(Model, vec3(posxZombi, posyZombi, 0.0f));
	Model = scale(Model, vec3((float)ZOMBI_WIDTH, (float)ZOMBI_HEIGHT, 1.0f));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(VAO_ZOMBI);
	glDrawArrays(GL_TRIANGLES, 0, vertices_zombi);
	glBindVertexArray(0);

	/****************************
	* draw OBSTACLES (Headstone & Hand)
	*****************************/
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		if (obstacles[i].isActive) {
			const auto& obs = obstacles[i];

			if (obs.type == HAND) {
				glBindVertexArray(VAO_HAND);
			}
			else if (obs.type == DIAMOND) {
				glBindVertexArray(VAO_DIAMOND);
			}
			else { // HEADSTONE
				glBindVertexArray(VAO_HEADSTONE);
			}

			Model = mat4(1.0);
			Model = translate(Model, vec3(obs.x, obs.y, 0.0f));
			Model = scale(Model, vec3(obs.width, obs.height, 1.0f));
			glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));

			if (obs.type == HAND) {
				glDrawArrays(GL_TRIANGLES, 0, 102);
			}
			else if (obs.type == DIAMOND) {
				glDrawArrays(GL_TRIANGLES, 0, vertices_diamond);
			}
			else { // HEADSTONE
				glDrawArrays(GL_TRIANGLES, 0, vertices_headstone);
			}
		}
	}
	glBindVertexArray(0);

	/****************************
	* draw CLOUDS
	*****************************/
	float distanceCloudsColumn = ((float)width + 100) / cloud_per_row;
	glBindVertexArray(VAO_CLOUD);

	for (int i = 0; i < cloud_per_row; i++) {
		float posxCloud = i * distanceCloudsColumn;
		float cloudFinalSize = (float)sizeClouds[i];

		Model = mat4(1.0);
		Model = translate(Model, vec3(dxClouds1 + posxCloud + positionXClouds[i], (float)positionYClouds[i], 0.0f));
		Model = scale(Model, vec3(cloudFinalSize * 1.8f, cloudFinalSize, 1.0f));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, vertices_cloud);
	}

	for (int i = 0; i < cloud_per_row; i++) {
		float posxCloud = i * distanceCloudsColumn;
		float cloudFinalSize = (float)sizeClouds[i];

		Model = mat4(1.0);
		Model = translate(Model, vec3(dxClouds2 + posxCloud + positionXClouds[i], (float)positionYClouds[i], 0.0f));
		Model = scale(Model, vec3(cloudFinalSize * 1.8f, cloudFinalSize, 1.0f));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, vertices_cloud);
	}
	glBindVertexArray(0);


}

// Function to initialize OpenGL and create a window.
GLFWwindow* initOpenGL() {

	// Initialize GLFW.
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW." << std::endl;
		glfwTerminate();
	}


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Double buffering
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	/******* Create a window and its OpenGL context *****/
	GLFWwindow* window = glfwCreateWindow(width, height, "Grave Run!", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window !" << std::endl;
		glfwTerminate();
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window); 	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load opengl function pointers !" << std::endl;
		glfwTerminate();
	}
	return window;
}


int main(void)
{
	srand((unsigned)time(NULL));

	for (int i = 0; i < trees_per_row; i++) {
		sizeTrees[i] = rand() % MAXSIZETREE;
		positionXTrees[i] = rand() % 80;
	}

	for (int i = 0; i < details_per_row; i++) {
		sizeGround[i] = rand() % MAXSIZEDETAIL + 5;
		positionXGround[i] = rand() % 100;

		positionYGround[i] = rand() % (int)(gameplayGroundLine - 2);

	}
	for (int i = 0; i < cloud_per_row; i++) {
		sizeClouds[i] = rand() % MAXSIZECLOUD + 30;
		positionXClouds[i] = rand() % 150;
		positionYClouds[i] = (gameplayGroundLine + 50) + (rand() % int(height - (gameplayGroundLine + 50)));
	}

	for (int i = 0; i < MAX_MOUNTAINS; i++) {
		mountains[i].isActive = false;
	}

	// Initialize OpenGL and create a window.
	GLFWwindow* window = initOpenGL();

	initShader();
	init();

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	/******* Function callbacks : chiamate quando si verificano determinati eventi ******/
	// process keyboard input
	glfwSetKeyCallback(window, key_callback);
	// process mouse motion
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// process mouse button pressed or released
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// process window resize
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	for (int i = 0; i < MAX_OBSTACLES; i++) {
		obstacles[i].isActive = false;
	}
	// Management time
	int frames = 0, updates = 0;
	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// Measure time
		nowTime = glfwGetTime();
		deltaTime += (nowTime - lastTime) / limitFPS;
		lastTime = nowTime;

		// Only update at 60 frames / s
		while (deltaTime >= 1.0) {
			update(0);			// Update function
			updateSpeed();
			updateParticles(limitFPS);
			updateWorld(0);
			updates++;
			deltaTime--;
		}

		// Render 
		drawScene();
		frames++;

		// Reset after one second
		if (glfwGetTime() - timer > 1.0) {
			timer++;
			updates = 0;
			frames = 0;
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(programId);
	glDeleteVertexArrays(1, &VAO_LAWN);
	glDeleteVertexArrays(1, &VAO_CHARACTER);
	glDeleteVertexArrays(1, &VAO_ZOMBI);
	glDeleteVertexArrays(1, &VAO_TREE);
	glDeleteVertexArrays(1, &VAO_GROUND_DETAIL);
	glDeleteVertexArrays(1, &VAO_HEADSTONE);
	glDeleteVertexArrays(1, &VAO_CLOUD);
	glDeleteVertexArrays(1, &VAO_DIAMOND);
	glDeleteVertexArrays(1, &VAO_MOUNTAIN);
	glDeleteVertexArrays(1, &VAO_PARTICLE);

	glfwTerminate();
	return 0;
}