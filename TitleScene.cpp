#include "TitleScene.h"
#include "DirectXCommon.h"
#include "Input.h"

TitleScene::~TitleScene() { 
	
	//天球の開放
	delete SkydomeModel_;

	delete modelTitle_; }

void TitleScene::Initialize() {

	// 天球の生成
	SkydomeModel_ = Model::CreateFromOBJ("skydome", true);

	// 天球の初期化
	skydome_ = new SkyDome();

	skydome_->Initialize(SkydomeModel_, &viewProjection_);

	modelTitle_ = Model::CreateFromOBJ("title", true);
	viewProjection_.Initialize();
	worldTransform.Initialize();
	worldTransform.translation_ = {0, 10, 0};
	worldTransform.rotation_ = {-4.4f,3.2f,0};
	worldTransform.scale_ = {10, 10, 10};
}

void TitleScene::Update() {

	// 天球
	skydome_->Update();

	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		IsFinished_ = true;
	}
	//viewProjection_.UpdateMatrix();
	worldTransform.UpdateMatrix();
}

void TitleScene::Draw() {
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	// 天球の描画
	skydome_->Draw();

	modelTitle_->Draw(worldTransform, viewProjection_);

	Model::PostDraw();
}
