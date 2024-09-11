#pragma once
#include "Model.h"
#include "SkyDome.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include"cassert"
/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {

public:
	~TitleScene();
	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return IsFinished_; }
	

private:
	// 終了フラグ
	bool IsFinished_ = false;
	ViewProjection viewProjection_;
	WorldTransform worldTransform;
	Model* modelTitle_ = nullptr;
	Model* SkydomeModel_ = nullptr;

	SkyDome* skydome_ = nullptr;
};