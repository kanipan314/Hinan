#include "GameScene.h"
#include "DebugCamera.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include <random>

GameScene::GameScene() {}

GameScene::~GameScene() {

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	// 敵キャラの開放
	for (Enemy* enemyList : enemies_) {
		delete enemyList;
	}
	enemies_.clear();
	delete modelPlayer_;
	delete modelBlock_;
	delete modelEnemy;

	delete player_;
	delete skyDome_;
	delete modelSkyDome_;
	delete mapChipField_;
	delete cameraController_;
	delete modelParticles_;
	delete deathParticles_;
	delete matrixFunction;
	delete box_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	viewProjection_.Initialize();
	cameraViewProjection_.Initialize();
	PrimitiveDrawer::GetInstance()->SetViewProjection(&cameraViewProjection_);
	matrixFunction = new MatrixFunction;

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/stage1.csv"); // CSVファイル読み込み

	modelBlock_ = Model::CreateFromOBJ("cube", true);
	GenerateBlocks();

	
	modelPlayer_ = Model::CreateFromOBJ("player", true); // 3Dモデルの生成
	player_ = new Player();                              // 自キャラの生成
	Vector3 playerposition = mapChipField_->GetMapChipPositionByIndex(2, 2);
	player_->initialize(modelPlayer_, &cameraViewProjection_, playerposition, audio_); // 自キャラの初期化
	player_->SetMapChipField(mapChipField_);

	phase_ = Phase::kPlay;

	skyDome_ = new SkyDome();                              // 天球の生成
	modelSkyDome_ = Model::CreateFromOBJ("SkyDome", true); // 3Dモデルの生成
	skyDome_->Initialize(modelSkyDome_, &viewProjection_); // 天球の初期化

	deathParticles_ = new DeathParticles();
	modelParticles_ = Model::CreateFromOBJ("Particle", true);
	deathParticles_->Initialize(modelParticles_, &cameraViewProjection_, playerposition);

	// 敵キャラの生成
	//enemyTxhandle = TextureManager::Load("sample.png"); // テクスチャの読み込み
	modelEnemy = Model::CreateFromOBJ("Enemy",true);

	for (int i = 0; i < 5; i++) {

		enemy_ = new Enemy();

		Vector3 enemyPosition = {(float)(i * 5 + 45 * 20, i * 15 * 20, 0.0f)};

		enemy_->Initialize(modelEnemy,&cameraViewProjection_, enemyPosition);

		enemy_->SetPlayer(player_);
		enemies_.push_back(enemy_);
		
	}

	

	// カメラコントローラー初期化
	Rect area_ = {30.0f, 300.0f, 0.0f, 100.0f};
	cameraController_ = new CameraController;
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
	cameraController_->SetMovableArea(area_);

	// スプライト初期化
	box_ = new Box();
	box_->Initialize();

	// サウンド
	BGM = audio_->LoadWave("sound/BGM.mp3");
	if (audio_->IsPlaying(BGM) == false) {
		BGM = audio_->PlayWave(BGM, true, 0.07f);
	}
}

void GameScene::Update() { ChangePhase(); }

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	// Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる

	/// </summary>

	// スプライト描画後処理
	// Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる

	for (const auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto worldTransformBlock : worldTransformBlockLine) {
			modelBlock_->Draw(*worldTransformBlock, cameraViewProjection_);
		}
	}

	skyDome_->Draw();

	if (player_->IsDeadGetter() == false) {
		player_->Draw();
	}

	// 敵キャラの描画
	for (Enemy* enmeyList : enemies_) {

		enmeyList->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}
	//	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, -40, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
	/// </summary>
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	
#pragma endregion
}

void GameScene::GenerateBlocks() {
	uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t y = 0; y < kNumBlockVirtical; y++) { // キューブの生成
		worldTransformBlocks_[y].resize(kNumBlockHorizontal);

		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			worldTransformBlocks_[y][x] = new WorldTransform();
			WorldTransform* worldTransform = new WorldTransform();

			if (mapChipField_->GetMapChipTypeByIndex(x, y) == MapChipType::kBlock) {
				worldTransformBlocks_[y][x] = worldTransform;
				worldTransformBlocks_[y][x]->translation_ = mapChipField_->GetMapChipPositionByIndex(x, y);
			}
			worldTransformBlocks_[y][x]->Initialize();
			// ImGui::Text("BlocksPos=%5.2f",worldTransformBlocks_[0][19]->translation_.y);
		}
	}
}

void GameScene::CheckAllCollisions() {

#pragma region 自キャラと敵の当たり判定
	AABB aabb1, aabb2;
	// 自キャラの座標
	aabb1 = player_->GetAABB();
	// 敵の座標
	aabb2 = enemy_->GetAABB();

	// AABB同士の交差判定
	if (aabb1.isHit(aabb2)) {
		IsFinished_ = true;
		player_->OnCollision(enemy_); // 自キャラの衝突時コールバックを呼び出す
		enemy_->OnCollision(player_); // 敵の衝突時コールバックを呼び出す
	}

#pragma endregion
}

void GameScene::ChangePhase() {

	switch (phase_) {

	case Phase::kPlay:

		BlocksUpdate();

		if (player_->IsDeadGetter() == false) {
			player_->Update();
		}

	#pragma region ブロック移動色々

		worldTransformBlocks_[18][1]->translation_.y += 0.05f;
		worldTransformBlocks_[18][1]->UpdateMatrix();
		worldTransformBlocks_[3][41]->translation_.y -= 0.25f;
		worldTransformBlocks_[3][41]->UpdateMatrix();
		
		if (worldTransformBlocks_[3][41]->translation_.y <= 30) {
		
			worldTransformBlocks_[2][41]->translation_.y -= 0.25f;
			worldTransformBlocks_[2][41]->UpdateMatrix();

		}

		if (worldTransformBlocks_[2][148]->translation_.x <= -10) {
		
			worldTransformBlocks_[2][148]->translation_.x -= 1.0f;
			worldTransformBlocks_[2][148]->UpdateMatrix();
		}

	#pragma endregion

		skyDome_->Update();

		// 敵キャラの描画
		for (Enemy* enmeyList : enemies_) {

			enmeyList->Update();
		}

		enemy_->Update();
		CameraUpdate();

		CheckAllCollisions();

		// 自キャラがやられたら
		if (player_->IsDeadGetter()) {
			phase_ = Phase::kDeath;
			audio_->StopWave(BGM);
			const Vector3& deathParticlePosition = player_->GetWorldPosition();
			deathParticles_->Initialize(modelParticles_, &cameraViewProjection_, deathParticlePosition);
		}

		box_->Update();

		break;

	case Phase::kDeath:
		BlocksUpdate();
		skyDome_->Update();
		enemy_->Update();

		CameraUpdate();

		if (deathParticles_) {
			deathParticles_->Update();
		}

		if (deathParticles_ && deathParticles_->IsFinished()) {
			IsFinished_ = true;
		}
		break;
	}
}

void GameScene::BlocksUpdate() {
	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			worldTransformBlock->matWorld_ = matrixFunction->MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			// 定数バッファに転送する
			worldTransformBlock->UpdateMatrix();
			// ImGui::Text("BlocksPos=%5.2f",worldTransformBlock->translation_.y );
		}
	}
}

void GameScene::CameraUpdate() {

	isDebugCameraactive_ = true;

	if (isDebugCameraactive_) {
		cameraController_->Update();
		cameraViewProjection_.matView = cameraController_->GetMatView();

		cameraViewProjection_.matProjection = cameraController_->GetMatProjection();

		cameraViewProjection_.TransferMatrix();
	} else {
		cameraViewProjection_.UpdateMatrix();
	}
}
