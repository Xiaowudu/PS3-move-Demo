#include "StateMachine.h"

StateMachine::StateMachine(SceneNode* n, MD5Mesh* m, SceneNode* s, CameraNode* c) : node(n), md5mesh(m), sphere(s), camera(c)
{
	hellknight_idle2 = "/Meshes/idle2.md5anim";
	hellknight_walk7 = "/Meshes/walk7.md5anim";
	hellknight_attack3 = "/Meshes/attack3.md5anim";

	currentAnim = NULL;
	toggle = false;
}

StateMachine::~StateMachine(void)
{
}

void StateMachine::EnemyStates(float msec) {
	if(node->getHitPoint() <= 0&& node->gameObject.GetState()!= GAMEOBJECT_STATE_ALIVE) {
		node->gameObject.SetState(GAMEOBJECT_STATE_DEAD);
	}

	if(node->gameObject.GetState() == GAMEOBJECT_STATE_WANDER) {
		Vector3 dir = camera->GetPosition() - node->body->m_position;
		dir.setY(0);
		float dis = length(dir);

		//Behaviour_wandering
		if(toggle) {
			toggle = false;
			node->body->m_linearVelocity =	Vector3(0,0,0);
			md5mesh->PlayAnim(hellknight_idle2);
		}
		md5mesh->UpdateAnim(msec);

		if(length(dir) <= 800) {
			node->gameObject.SetState(GAMEOBJECT_STATE_MOVE);
			toggle = true;
		}		
	}

	else if(node->gameObject.GetState() == GAMEOBJECT_STATE_MOVE) {
		Vector3 dir = camera->GetPosition() - node->body->m_position;
		dir.setY(0);
		float dis = length(dir);
		node->body->m_linearVelocity  = normalize(dir) * 50;
		
		//Behaviour_moving	
		if(toggle) {
			toggle = false;
			md5mesh->PlayAnim(hellknight_walk7);
		}		
		md5mesh->UpdateAnim(msec);

		if(dis >= 1000) {
			node->gameObject.SetState(GAMEOBJECT_STATE_WANDER);
			toggle = true;
		}
		else if(dis <= 220) {
			node->gameObject.SetState(GAMEOBJECT_STATE_ATTACK);
			toggle = true;
		}
	}

	else if(node->gameObject.GetState() == GAMEOBJECT_STATE_ATTACK) {
		Vector3 dir = camera->GetPosition() - node->body->m_position;
		dir.setY(0);
		float dis = length(dir);		
		
		//Behaviour_attacking
		if(toggle) {
			toggle = false;
			node->body->m_linearVelocity =	Vector3(0,0,0);
			md5mesh->PlayAnim(hellknight_attack3);
		}		
		md5mesh->UpdateAnim(msec);

		if(dis >= 300) {
			node->gameObject.SetState(GAMEOBJECT_STATE_MOVE);
			toggle = true;
		}
	}

	else if(node->gameObject.GetState() == GAMEOBJECT_STATE_DEAD) {
		node->gameObject.SetState(GAMEOBJECT_STATE_ALIVE);

		sphere->body->m_position = node->body->m_position + Vector3(0,100,0);
		sphere->bounding_sphere->m_pos = sphere->body->m_position ;
		node->GetParent()->RemoveChild(sphere);
		node->GetParent()->AddChild(*sphere);

		node->GetParent()->RemoveChild(node);
	}

	else if(node->gameObject.GetState() == GAMEOBJECT_STATE_ALIVE) {
		Matrix4 viewm = camera->BuildViewMatrix();
		float x = -viewm.getElem(3,0);
		//float y = -viewm.getElem(3,1);
		float z = -viewm.getElem(3,2);
		Vector3 pos = camera->GetPosition() + normalize(Vector3(x,0,z)) * 700;
		node->body->m_position = pos;
		node->setHitPoint(50);
		node->gameObject.SetState(GAMEOBJECT_STATE_MOVE);
		node->GetParent()->AddChild(*node);
		toggle = true;
	}
}