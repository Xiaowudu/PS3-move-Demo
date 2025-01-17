
#include <stdio.h>
#include <stdlib.h>	

#include <string>
#include <iostream>

#include <sys/process.h>

#include "Renderer.h"
#include "Input.h"
#include "timer.h"
#include "HeightMap.h"
#include "OBJMesh.h"
#include "MD5Mesh.h"
#include "MotionController.h"
#include "CameraNode.h"
#include "cell/gem.h"
#include "CollisionResponse.h"
#include "CollisionDetection.h"
#include "StateMachine.h"
#include "GameData.h"

SYS_PROCESS_PARAM(1001, 0x10000)

bool done = false;
Renderer renderer;
SceneNode *root;
GameData gameData;
CameraNode* camera;

void start_button()		{
	done = true;
	std::cout << "Pressed start button!" << std::endl;
}

void select_button()		{
	camera->SetPosition(Vector3(2056,0,2056));
	camera->SetPitch(0.0f);
	camera->SetYaw(0.0f);
}

void square_button() {
	//camera->toggleFreeMode();
	renderer.toggleDebug();
}

void Done_game() {
	done = true;
}

void Restart_game() {
	gameData.Reset();
	camera->SetPosition(Vector3(1200,200,2000));
	camera->SetPitch(0);
	camera->SetYaw(0);
	MotionController::SetGemFunction(GEM_CROSS,		NULL);
	MotionController::SetGemFunction(GEM_CIRCLE,	NULL);
}

//void CollisionTest(SceneNode& weapon, SceneNode& enemy, Vector3& vel) {
//	bool hit;
//	CollisionData_c data_c;
//	hit = CubeSphere(*weapon.bounding_box, *enemy.bounding_sphere, &data_c);
//	if(hit) {
//		if(length(vel) * 0.1 < 100) {
//			Collision_responseII(*weapon.body, data_c.m_normal, data_c.m_penetration);
//		}
//		else {
//			std::cout << "hit!" << std::endl;
//		}
//		//std::cout << "hit!" << std::endl;
//	}
//}
/*
Here's a quick example program. It'll load up a single SceneNode
scene, containing a textured quad, with a camera controllable by
the joypad. Pretty basic, but as with Graphics for Games, its
everything you need to get started and doing something more 
interesting with the Playstation!

*/
int main(void)	{
	std::cout << "All-New intro to GCM V1.0!\n" << std::endl;

	//Start off by initialising the Input system
	Input::Initialise();
	//If the start button is pressed, call this function!
	//Input::SetPadFunction(INPUT_START,	start_button);
	//Input::SetPadFunction(INPUT_SELECT,	select_button);
	
	MotionController::motionControllerInit();
 	//cellSysutilRegisterCallback(0, exitCallbackHandler, NULL);
	/////////////////////////////////////////////////////////////////////////////////////
	MotionController::SetGemFunction(GEM_START,	start_button);
	MotionController::SetGemFunction(GEM_SELECT, select_button);
	MotionController::SetGemFunction(GEM_SQUARE, square_button);

	//Create a new scenenode
	root = new SceneNode();
	renderer.SetRootNode(root); //Set our new SceneNode as the root for our Renderer	
	renderer.SetGameData(&gameData);
	
	////Make a new quad mesh, and set its texture to a newcastle logo
	//Mesh* m = Mesh::GenerateQuad();
	//m->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/ncl.gtf"));
	//m->SetBumpTexture(*GCMRenderer::LoadGTF("/Textures/2563-normal.gtf"));

	//SceneNode*logo = new SceneNode();
	//logo->SetMesh(m); //tell it to draw our new quad

	//logo->SetTransform(/*Matrix4::translation(Vector3(2056,0,2056)) **/ Matrix4::rotationX(DegToRad(-90)) * Matrix4::scale(Vector3(400,400,400)));

	//root->AddChild(*logo);

	//HeightMap
	HeightMap* heightMap = new HeightMap("/Textures/new terrain.raw");
	heightMap->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/Slumbering Volcano.gtf"));
	heightMap->SetBumpTexture(*GCMRenderer::LoadGTF("/Textures/rockNormal.gtf"));

	SceneNode* terrain = new SceneNode();
	terrain->SetMesh(heightMap);

	root->AddChild(*terrain);
	
	//OBJMesh
	OBJMesh* sword = new OBJMesh("/Meshes/short_sword_tex1.obj");
	sword->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/sword.gtf"));
	sword->SetBumpTexture(*GCMRenderer::LoadGTF("/Textures/grassNormal.gtf"));

	SceneNode* Weapon_Sword = new SceneNode();
	Weapon_Sword->SetMesh(sword);
	
	Matrix4  origin_trans = Matrix4::translation(Vector3(0,-30,-100));
	Weapon_Sword->SetTransform(origin_trans);
	Weapon_Sword->SetBoundingRadius(50);

	Weapon_Sword->have_physics = true;
	Weapon_Sword->gameObject.SetType(GAMEOBJECT_TYPE_WEAPON);
	Weapon_Sword->bounding_box = new Box_c(Vector3(0,0,-40), Vector3(5,10,60));

	root->AddChild(*Weapon_Sword);
	
	//object2
	OBJMesh* sphere = new OBJMesh("/Meshes/Heart.obj");
	sphere->SetDefaultTexture(*GCMRenderer::LoadGTF("/Textures/desert-texture.gtf"));
	
	SceneNode* object2 = new SceneNode();
	object2->SetMesh(sphere);
	
	Vector3 p = heightMap->getHeight(2056,2000);
	p.setY(p.getY());
	object2->SetTransform(Matrix4::translation(p));
	object2->invMass = 1;
	object2->have_physics = true;
	object2->body = new Body();
	object2->bounding_sphere = new Sphere_c();
	object2->bounding_sphere->m_radius = 25;
	object2->gameObject.SetType(GAMEOBJECT_TYPE_SPHERE);
	//root->AddChild(*object2);
	
	//MD5Mesh
	MD5Mesh* hellKnight;
	hellKnight = new MD5Mesh();
	
	if(!hellKnight->LoadMD5Mesh("/Meshes/hellknight.md5mesh"))
	{
		return 0;
	}
	
	hellKnight->AddAnim("/Meshes/idle2.md5anim");
	hellKnight->AddAnim("/Meshes/walk7.md5anim");
	hellKnight->AddAnim("/Meshes/attack3.md5anim");

	hellKnight->PlayAnim("/Meshes/idle2.md5anim");
	
	SceneNode* Enemy_Hellknight = new SceneNode();
	Enemy_Hellknight->SetMesh(hellKnight);	
	Enemy_Hellknight->SetOriginTransform(Matrix4::translation(p) * Matrix4::rotationY(DegToRad(-90)));
	Enemy_Hellknight->SetBoundingRadius(50);

	Enemy_Hellknight->bounding_sphere = new Sphere_c(p + Vector3(0,60,0),50);
	Enemy_Hellknight->have_physics = true;
	Enemy_Hellknight->have_direction = true;

	Enemy_Hellknight->gameObject.SetType(GAMEOBJECT_TYPE_ENEMY);
	Enemy_Hellknight->gameObject.SetState(GAMEOBJECT_STATE_WANDER);
	//Enemy_Hellknight->setHitPoint(0);

	root->AddChild(*Enemy_Hellknight);

	//SceneNode* Enemy_Hellknight[10];
	//for(int i = 0; i < 10; i++) {
	//Enemy_Hellknight[i] = new SceneNode();
	//Enemy_Hellknight[i]->SetMesh(hellKnight);	
	//Enemy_Hellknight[i]->SetTransform(Matrix4::translation(heightMap->getHeight(i*400,i*400) ) );
	//Enemy_Hellknight[i]->SetBoundingRadius(50);

	//Enemy_Hellknight[i]->bounding_sphere = new Sphere_c(heightMap->getHeight(i*400,i*400) + Vector3(0,60,0),50);
	//Enemy_Hellknight[i]->have_physics = true;

	//Enemy_Hellknight[i]->gameObject.SetType(GAMEOBJECT_TYPE_ENEMY);
	//Enemy_Hellknight[i]->gameObject.SetState(GAMEOBJECT_STATE_WANDER);

	//root->AddChild(*Enemy_Hellknight[i]);
	//}

	//We need a new camera!
	camera = new CameraNode();	
	camera->SetControllingPad(JOYPAD_NAV);	//Controlled via joypad Nav
	camera->SetPosition(Vector3(1200,200,2000)); //And set back slightly so we can see the node at the origin
	
	camera->AddChild(*Weapon_Sword);
	renderer.SetCamera(camera);	//Set the current renderer camera
	
	root->AddChild(*camera);
	camera->bounding_sphere = new Sphere_c();
	camera->bounding_sphere->m_radius = 150;
	camera->gameObject.SetType(GAMEOBJECT_TYPE_CAMERA);
	Matrix4 gem_trans;

	Timer gameTime;
	root->Update(0.01);	
	Vector3 vel;
	int fps=0;
	int t = 0;
	int counter = gameData.GetTimeLeft();
	StateMachine* FSM;
	FSM = new StateMachine(Enemy_Hellknight, hellKnight, object2,camera);
	
	while(!done) {
		Input::UpdateJoypad();	//Receive latest joypad input for all joypads
		MotionController::motionControllerUpdate();	
		if(gameData.GetLife() != 0 && gameData.GetTimeLeft() !=0) {
			/////////////////////////////////////////////////////////////////////////////		
			
				MotionController::GetControllerTransform(gem_trans);
				Weapon_Sword->SetTransform(origin_trans * gem_trans );	
		
			/////////////////////////////////////////////////////////////////////////////	
			float msec = (float)gameTime.GetTimedMS();
			
			camera->SetPosY(100 + heightMap->getHeight(camera->GetPosition().getX(), camera->GetPosition().getZ()).getY());
			Vector3 dir = camera->GetPosition() - Enemy_Hellknight->GetWorldTransform().getTranslation();
			if(dir.getY() != 0) {
				dir.setY(0);
			}
			Enemy_Hellknight->SetDirection(dir);
			Enemy_Hellknight->bounding_sphere->m_pos = Enemy_Hellknight->body->m_position + Vector3(0,60,0);

			FSM->EnemyStates(msec);		
			
			Enemy_Hellknight->movement = hellKnight->getMovement();	
			//Enemy_Hellknight->SetPosition(p + mov);
			float h_y = (heightMap->getHeight(Enemy_Hellknight->body->m_position.getX(),Enemy_Hellknight->body->m_position.getZ()) ).getY();
			Enemy_Hellknight->body->m_position.setY(h_y);

			root->Update(msec);	//Update our scene hierarchy. This bit is new (previously the renderer handled it)

			//
			MotionController::GetCotnrollerVelocity(vel);
			gameData.SetVelocity(vel);
			//
			Weapon_Sword->bounding_box->transform =  Weapon_Sword->GetWorldTransform() * Matrix4::translation(Weapon_Sword->bounding_box->m_pos);



			camera->bounding_sphere->m_pos = camera->GetPosition();
			renderer.NodesPhysics();
			//Vector3 distance = Weapon_Sword->GetWorldTransform().getTranslation()/*camera->GetPosition()*/- (Enemy_Hellknight->bounding_sphere->m_pos);
			//if(dot(distance,distance) < 150 * 150) {	
			//	CollisionTest(*Weapon_Sword, *Enemy_Hellknight,vel);
			//}
			Weapon_Sword->SetWorldTransformTranslation(Weapon_Sword->body->m_position);
			Weapon_Sword->bounding_box->transform =  Weapon_Sword->GetWorldTransform() * Matrix4::translation(Weapon_Sword->bounding_box->m_pos);
			//				
			//gameData.SetTimeLeft(gameTime.TimeCounter(600));

			fps++;
			t += msec;
			if(t >= 1000) {
				//std::cout << fps << std::endl;
				fps = 0;
				t = 0;
				gameData.SetTimeLeft(gameData.GetTimeLeft() -1 );
			}

			//		
			//renderer.RenderScene();	//Render the scene
		}

		else {
			if(gameData.set_button) {
				MotionController::SetGemFunction(GEM_CROSS, Restart_game);
				MotionController::SetGemFunction(GEM_CIRCLE, Done_game);
				gameData.set_button = false;
				gameData.gameOver = true;

				Enemy_Hellknight->gameObject.SetState(GAMEOBJECT_STATE_WANDER);
				Enemy_Hellknight->setHitPoint(50);
				Enemy_Hellknight->body->Reset();
				Enemy_Hellknight->body->m_position = p;
				FSM->toggle = true;
			}
		}
		renderer.RenderScene();	//Render the scene	
	}
	//If we get here, joypad A has had its start button pressed

	std::cout << "Quitting..." << std::endl;

	//delete m->GetDefaultTexture();
	//delete m;
	delete heightMap->GetDefaultTexture();
	delete heightMap->GetBumpTexture();
	delete heightMap;
	delete sword->GetDefaultTexture();
	delete sword;
	delete sphere->GetDefaultTexture();
	delete sphere;
	delete hellKnight;
	delete FSM;
	delete root;
	Input::Destroy();

	MotionController::motionControllerEnd();
	
	std::cout << "Finished!" <<std::endl;

	return 0;
}