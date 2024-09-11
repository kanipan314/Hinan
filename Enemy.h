#pragma once
#include "AABB.h"
#include "MatrixFunction.h"
#include "Model.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "cassert"

class Player;

class Enemy {

	std::list<Enemy*> enemies_;

public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();
	void Draw();
	AABB GetAABB();
	Vector3 GetWorldPosition();
	// 衝突応答
	void OnCollision(const Player* player);
	void SetPlayer(Player* player) { player_ = player; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t txHandle_ = 0u;
	ViewProjection* viewProjection_ = nullptr;
	static inline const float kWalkSpeed = 0.1f;
	Vector3 velocity_ = {};
	static inline const float kWalkMotionAngleStart = 1.0f;
	static inline const float kWalkMotionAngleEnd = 20.0f;
	static inline const float kWalkMotionTime = 1.0f;
	float walkTimer_ = 0.0f;
	static inline const float kWidth = 1.90f;
	static inline const float kHeight = 1.90f;
	Player* player_ = nullptr;
	//static inline const Vector3 kHomingSpeed = {0.1f, 0.1f, 0.1f};
	float t = 0.01f;
};