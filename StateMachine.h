#pragma once

#include "GameObject.h"
#include "SceneNode.h"
#include "CameraNode.h"
#include "MD5Mesh.h"
#include "MD5Anim.h"
#include <vectormath/cpp/vectormath_aos.h>

using namespace Vectormath::Aos;

class StateMachine
{
public:
	StateMachine(SceneNode* n, MD5Mesh* m, SceneNode* sphere,CameraNode* c);
	~StateMachine(void);

	void EnemyStates(float msec);
	bool toggle;
protected:
	SceneNode* node;
	SceneNode* sphere;
	CameraNode*	camera;
	MD5Mesh* md5mesh;
	MD5Anim* currentAnim;

	std::string hellknight_idle2;
	std::string hellknight_walk7;
	std::string hellknight_attack3;

};
