#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include "common.h"
#include "Utils.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include<iostream>
#include<vector>

// variabila in care tinem fereastra
GLFWwindow* window = NULL;


// WIDTH-ul si HEIGHT-ul initial
#define WWIDTH 1024
#define WHEIGHT 768

// width-ul si height-ul ferestrei  (chiar si dupa ce si-a primit resize)
int g_gl_width	= WWIDTH;
int g_gl_height = WHEIGHT;

//vector pentru proiectil
vector<CSprite*> projectiles;
bool semafor = true;



void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
	g_gl_width = width;
	g_gl_height = height;

}

void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void* userParam)
{
	printf("%s", message);
	__debugbreak();
}

// Clasa cu care testam functionalitatea introdusa (draft de Player)
struct PlayerTest
{
	// viteza aleasa empiric
	#define PLAYER_MOVE_SPEED 2.0f
	
	// Enum cu toate tipurile de animatii ale player-ului
	enum EAnimationTypePlayer
	{
		NONE,
		IDLE,
		RIGHT,
		LEFT
	};

	// Sprite-ul cu care se deseneaza player-ul
	CSprite*				m_Sprite;
	// tipul curent de animatie (animatia 
	EAnimationTypePlayer	m_CurrentAnimType;

	// Constructor
	PlayerTest() :m_Sprite(NULL), m_CurrentAnimType(NONE){}
	
	// Initializare
	void Init(CSprite *spr)
	{
		m_Sprite = spr;
	}
	
	// In functia de update:
	// 1.Detectam directia de deplasare a player-ului in functie de tastele apasate
	// 2.Determinam animatia ce trebuie afisata
	// 3.Limitam din topor spatiul in care se poate deplasa player-ul
	void Update(float dt)
	{

		glm::vec3 pos;
		glm::vec3 dir(0.0f, 0.0f, 0.0f);
		bool recalculate_position = false;

		// Determinarea animatiilor si a directiei de deplasare
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT))
		{
			if (m_CurrentAnimType != LEFT)
			{
				m_Sprite->PlayAnimation("Left");
				m_CurrentAnimType = LEFT;
			}

			dir.x = -1.0f;
			recalculate_position = true;

		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT))
		{
			if (m_CurrentAnimType != RIGHT)
			{
				m_Sprite->PlayAnimation("Right");
				m_CurrentAnimType = RIGHT;
			}

			dir.x = 1.0f;
			recalculate_position = true;
		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP))
		{
			if (m_CurrentAnimType != IDLE)
			{
				m_Sprite->PlayAnimation("Idle");
				m_CurrentAnimType = IDLE;
			}

			dir.y = 1.0f;
			recalculate_position = true;

		}
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN))
		{
			if (m_CurrentAnimType != IDLE)
			{
				m_Sprite->PlayAnimation("Idle");
				m_CurrentAnimType = IDLE;
			}

			dir.y = -1.0f;
			recalculate_position = true;
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE) && semafor) {
			projectiles.push_back(CSpriteManager::Get()->AddSprite("projectile.png", -1));
			projectiles[projectiles.size() - 1]->SetPosition(glm::vec3(m_Sprite->GetPosition().x, m_Sprite->GetPosition().y, -11));
			semafor = false;
		}

		if (GLFW_RELEASE == glfwGetKey(window, GLFW_KEY_SPACE) && !semafor) {
			semafor = true;
		}



		pos = m_Sprite->GetPosition();

		if (recalculate_position)
		{
		//	cout <<"initial "<< dir.x << " " << dir.y << " " << dir.z << endl;
			dir = glm::normalize(dir);
			//cout << dir.x << " " << dir.y << " " << dir.z<<endl;
			pos = pos + dir * PLAYER_MOVE_SPEED * dt;
		}
		else
		{
			if (m_CurrentAnimType != IDLE)
			{
				m_Sprite->PlayAnimation("Idle");
				m_CurrentAnimType = IDLE;
			}

		}
		// END



		// Limitarea playerului 
		if (pos.x < SCREEN_LEFT)
		{
			pos.x = SCREEN_LEFT + 0.01f;
		}
		if (pos.x  > SCREEN_RIGHT - 0.4f)
		{
			pos.x = SCREEN_RIGHT - 0.4f;
		}
		if (pos.y < SCREEN_BOTTOM)
		{
			pos.y = SCREEN_BOTTOM + 0.01f;
		}
		if (pos.y > SCREEN_TOP - 1.0f)
		{
			pos.y = SCREEN_TOP - 1.0f;
		}
		// END

		// update pozitia spriteului
		m_Sprite->SetPosition(pos);

	}


};



int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	glfwInit();
	
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(WWIDTH, WHEIGHT, "GL Framework", NULL, NULL);
	glfwSetWindowSizeCallback(window, glfw_window_size_callback);
	
	glfwMakeContextCurrent(window);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, g_gl_width, g_gl_height);	

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);


	// Initializam spriteManagerul
	CSpriteManager::Get()->Init();

	// Initializam PlayerEntity si creem sprite-ul asociat
	PlayerTest PlayerEntity;


	//CSprite *projectile = CSpriteManager::Get()->AddSprite("projectile.png", -1);
	CSprite *back = CSpriteManager::Get()->AddSprite("Background.png",2);
	CSprite *playerSprite = CSpriteManager::Get()->AddSprite("player0000.png", 0);
	CSprite *RandomRocket = CSpriteManager::Get()->AddSprite("PlayerRocket.png", 1);
	float x = -1;
	RandomRocket->SetPosition(glm::vec3(-4.0f, 1.0f, -12));
	
	back->SetPosition(glm::vec3(-4, -4, -15));

	assert(playerSprite);
	PlayerEntity.Init(playerSprite);


	double prevTime = glfwGetTime();
	glm::vec3 posProj;

	while (!glfwWindowShouldClose(window))
	{
		//projectile->SetPosition(glm::vec3(playerSprite->GetPosition().x, playerSprite->GetPosition().y, -12));
		float deltaTime = (float)(glfwGetTime() - prevTime);
		prevTime = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, g_gl_width, g_gl_height);

		// update logic pentru player
		PlayerEntity.Update(deltaTime);

		// update animatie
		CSpriteManager::Get()->Update(deltaTime);
		// desenare
		CSpriteManager::Get()->Draw();

		glm::vec2 a, b, c, d, p, q, r, s;
		//player
		a.x = d.x = playerSprite->model_mat[3][0];
		a.y = b.y = playerSprite->model_mat[3][1];
		b.x = c.x = a.x + 0.4f;
		c.y = d.y = a.y + 1.0f;
		//inamic
		p.x = s.x = RandomRocket->model_mat[3][0];
		p.y = q.y = RandomRocket->model_mat[3][1];
		q.x = r.x = p.x + 0.6f;
		r.y = s.y = p.y + 1.0f;

		//verificare coliziune
		CSpriteManager::Get()->CheckCollision(a, b, c, d, p, q, r, s);

		if (CSpriteManager::Get()->check == true){
			playerSprite->SetPosition(glm::vec3(playerSprite->GetPosition().x,
				playerSprite->GetPosition().y - 4.0f, playerSprite->GetPosition().z));
			CSpriteManager::Get()->check = false;
		}


		// Limitarea proiectil
		for (int i = 0; i < projectiles.size(); i++){
			if (projectiles[i]->GetPosition().y > SCREEN_TOP - 0.4f)
			{
				CSpriteManager::Get()->RemoveSprite(projectiles[i]->index);
				projectiles.erase(projectiles.begin() + i);
				
				
			}
			else{
				posProj = projectiles[i]->GetPosition();
				projectiles[i]->SetPosition(glm::vec3(posProj.x, posProj.y + 13* deltaTime, posProj.z));
			}
		}

		glfwPollEvents();
		glfwSwapBuffers(window);

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}

	}
	glfwTerminate();
	return 0;
}