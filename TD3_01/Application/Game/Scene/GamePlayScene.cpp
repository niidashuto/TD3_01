#include "GamePlayScene.h"
#include "SafeDelete.h"
#include "Quaternion.h"
#include "SphereCollider.h"
#include "Collision.h"
#include "CollisionManager.h"
#include "Player.h"
#include "Fan.h"
#include "imgui.h"
#include "CollisionAttribute.h"

#include "SceneManager.h"

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
DrawBasis* GamePlayScene::drawBas_ = DrawBasis::GetInstance();
ImGuiManager* GamePlayScene::imGuiManager_ = ImGuiManager::GetInstance();

void GamePlayScene::Initialize() {
	Initialize2d();
	Initialize3d();
}

void GamePlayScene::Update() {
	input_->Update();
	imGuiManager_->Begin();

	Update3d();
	Update2d();

#ifdef _DEBUG
	{
		float playerPos[Vector3Count_] = {
			player_->GetPosition().x,
			player_->GetPosition().y,
			player_->GetPosition().z
		};

		float playerDir[Vector3Count_] = {
			player_->GetRotation().x,
			player_->GetRotation().y,
			player_->GetRotation().z
		};

		float blockPos[Vector3Count_] = {
			rayObj_->GetPosition().x,
			rayObj_->GetPosition().y,
			rayObj_->GetPosition().z
		};
		float block2Pos[Vector3Count_] = {
			rayObj_2->GetPosition().x,
			rayObj_2->GetPosition().y,
			rayObj_2->GetPosition().z
		};

		float rayDir0[Vector3Count_] = {
			fan_[0]->GetRay()->dir_.x,
			fan_[0]->GetRay()->dir_.y,
			fan_[0]->GetRay()->dir_.z
		};
		float rayPos0[Vector3Count_] = {
			fan_[0]->GetRay()->start_.x,
			fan_[0]->GetRay()->start_.y,
			fan_[0]->GetRay()->start_.z
		};

		float rayDir1[Vector3Count_] = {
			fan_[1]->GetRay()->dir_.x,
			fan_[1]->GetRay()->dir_.y,
			fan_[1]->GetRay()->dir_.z
		};
		float rayPos1[Vector3Count_] = {
			fan_[1]->GetRay()->start_.x,
			fan_[1]->GetRay()->start_.y,
			fan_[1]->GetRay()->start_.z
		};

		float rayDir2[Vector3Count_] = {
			fan_[2]->GetRay()->dir_.x,
			fan_[2]->GetRay()->dir_.y,
			fan_[2]->GetRay()->dir_.z
		};
		float rayPos2[Vector3Count_] = {
			fan_[2]->GetRay()->start_.x,
			fan_[2]->GetRay()->start_.y,
			fan_[2]->GetRay()->start_.z
		};
		float rayInter[Vector3Count_] = {
			interRay_.x,
			interRay_.y,
			interRay_.z
		};

		float fanDir0[Vector3Count_] = {
			fan_[0]->GetRotation().x,
			fan_[0]->GetRotation().y,
			fan_[0]->GetRotation().z
		};
		float fanPos0[Vector3Count_] = {
			fan_[0]->GetPosition().x,
			fan_[0]->GetPosition().y,
			fan_[0]->GetPosition().z
		};

		float fanDir1[Vector3Count_] = {
			fan_[1]->GetRotation().x,
			fan_[1]->GetRotation().y,
			fan_[1]->GetRotation().z
		};
		float fanPos1[Vector3Count_] = {
			fan_[1]->GetPosition().x,
			fan_[1]->GetPosition().y,
			fan_[1]->GetPosition().z
		};

		float fanDir2[Vector3Count_] = {
			fan_[2]->GetRotation().x,
			fan_[2]->GetRotation().y,
			fan_[2]->GetRotation().z
		};
		float fanPos2[Vector3Count_] = {
			fan_[2]->GetPosition().x,
			fan_[2]->GetPosition().y,
			fan_[2]->GetPosition().z
		};

		float rayCol[1] = {
			colRay_
		};

		ImGui::Begin("Player");
		ImGui::SetWindowPos(ImVec2(700, 0));
		ImGui::SetWindowSize(ImVec2(500, 100));
		ImGui::InputFloat3("PlayerPos", playerPos);
		ImGui::InputFloat3("PlayerDir", playerDir);
		ImGui::End();

		ImGui::Begin("Block");
		ImGui::SetWindowPos(ImVec2(0, 400));
		ImGui::SetWindowSize(ImVec2(500, 100));
		ImGui::InputFloat3("BlockPos", blockPos);
		ImGui::InputFloat3("Block2Pos", block2Pos);
		ImGui::InputFloat3("RayInter", rayInter);
		ImGui::End();

		ImGui::Begin("Fan");
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(500, 200));
		ImGui::InputFloat3("FanPos", fanPos0);
		ImGui::InputFloat3("FanDir", fanDir0);
		ImGui::InputFloat3("FanPos", fanPos1);
		ImGui::InputFloat3("FanDir", fanDir1);
		ImGui::InputFloat3("FanPos", fanPos2);
		ImGui::InputFloat3("FanDir", fanDir2);
		ImGui::End();

		ImGui::Begin("Ray");
		ImGui::SetWindowPos(ImVec2(0, 500));
		ImGui::SetWindowSize(ImVec2(500, 200));
		ImGui::InputFloat3("RayPos", rayPos0);
		ImGui::InputFloat3("RayDir", rayDir0);
		ImGui::InputFloat3("RayPos", rayPos1);
		ImGui::InputFloat3("RayDir", rayDir1);
		ImGui::InputFloat3("RayPos", rayPos2);
		ImGui::InputFloat3("RayDir", rayDir2);
		ImGui::InputFloat("RayCol", rayCol);
		ImGui::End();

	}
#endif

	imGuiManager_->End();
}

void GamePlayScene::Draw() {
	Object3d::PreDraw(dxBas_->GetCommandList().Get());
	Draw3d();
	Object3d::PostDraw();

	ParticleManager::PreDraw(dxBas_->GetCommandList().Get());
	DrawParticle();
	ParticleManager::PostDraw();

	drawBas_->PreDraw();
	Draw2d();
	drawBas_->PostDraw();
}

void GamePlayScene::Initialize3d() {
	collisionManager_ = CollisionManager::GetInstance();
	//カメラの初期化
	camera_ = new Camera();

	//
	camera_->SetEye({ 15.0f, 70.0f, -40.0f });
	camera_->SetTarget({ 15.0f,20.0f,10.0f });
	/*
	camera_->SetEye({ 0.0f, 90.0f, -100.0f });
	camera_->SetTarget({ 0.0f,25.0f,0.0f });
	*/
	camera_->SetUp({ 0.0f, 1.0f, 0.0f });


	//各種モデル
	playerModel_ = new Model();
	playerModel_ = Model::LoadFromOBJ("PaperPlane", true);
	rayModel_ = new Model();
	rayModel_ = Model::LoadFromOBJ("cube", true);
	fanModel_ = new Model();
	fanModel_ = Model::LoadFromOBJ("Fan", true);

	//プレイヤーの初期化
	player_ = Player::Create(playerModel_);

	player_->SetCamera(camera_);
	player_->SetScale({ 1.0f, 1.0f, 1.0f });

	player_->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f)));

	player_->SetCamera(camera_);
	player_->Update();

	for (int i = 0; i < FanCount_; i++) {
		//ファンの初期化
		fan_[i] = Fan::Create(fanModel_);
		fan_[i]->SetScale({ 1.0f,1.0f,1.0f });
		fan_[i]->SetCamera(camera_);
	}

	//ファン下向き時の数値設定
	fan_[0]->SetFanDirection(Fan::Down);
	fan_[0]->SetIsControl(true);

	//ファン左向き時の数値設定
	fan_[1]->SetFanDirection(Fan::Down);
	fan_[1]->SetIsControl(true);

	//ファン右向き時の数値設定
	fan_[2]->SetFanDirection(Fan::Down);
	fan_[2]->SetIsControl(true);

	fan_[3]->SetFanDirection(Fan::Down);
	fan_[3]->SetIsControl(true);

	fan_[4]->SetFanDirection(Fan::Down);
	fan_[4]->SetIsControl(true);

	//レイ接触確認オブジェクトの初期化
	rayObj_ = Object3d::Create();
	rayObj_->SetModel(rayModel_);
	rayObj_->SetPosition(fan_[0]->GetRay()->start_);
	rayObj_->SetScale({ 2.0f, 2.0f, 2.0f });
	rayObj_->SetCamera(camera_);

	rayObj_2 = Object3d::Create();
	rayObj_2->SetModel(rayModel_);
	rayObj_2->SetPosition(fan_[0]->GetRay()->start_ + (50 * fan_[0]->GetRay()->dir_));
	rayObj_2->SetScale({ 2.0f, 2.0f, 2.0f });
	rayObj_2->SetCamera(camera_);

	skydome_ = new Skydome();
	skydome_->Initialize(camera_);

	//ライト生成
	lightGroup_ = new LightGroup();
	lightGroup_ = LightGroup::Create();
	lightGroup_->SetAmbientColor({ 1.0f,1.0f,1.0f });
	Object3d::SetLight(lightGroup_);

	//パーティクル
	particle1_ = Particle::LoadFromParticleTexture("particle6.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	particle2_ = Particle::LoadFromParticleTexture("particle5.png");
	pm2_ = ParticleManager::Create();
	pm2_->SetParticleModel(particle2_);
	pm2_->SetCamera(camera_);

	//ステージ生成
	stage_ = new Stage();
	stage_->Initialize(camera_);

	ParameterPlayer(positionPlayer, 0);
	ParamaterFun(positionFan[0], positionFan[1], positionFan[2], positionFan[3], positionFan[4]);
}

void GamePlayScene::Initialize2d() {
	drawBas_->LoadTexture(1, "texture.png");
	sprite_->Initialize(drawBas_, 1);

	sprite_->SetAnchorPoint({ 0.5f, 0.5f });
	sprite_->SetSize({ 64.0f,64.0f });
}

void GamePlayScene::Update3d() {
	colRay_ = false;

	{
		//imGuiからのライトパラメータを反映
		lightGroup_->SetDirLightDir(0, lightDir_);
	}

	//// カメラ移動
	//if (input_->PressKey(DIK_W) ||
	//	input_->PressKey(DIK_S) ||
	//	input_->PressKey(DIK_D) ||
	//	input_->PressKey(DIK_A)) {
	//	if (input_->PressKey(DIK_W)) { camera_->MoveVector({ 0.0f,+1.0f,0.0f }); }
	//	else if (input_->PressKey(DIK_S)) { camera_->MoveVector({ 0.0f,-1.0f,0.0f }); }
	//	if (input_->PressKey(DIK_D)) { camera_->MoveVector({ +1.0f,0.0f,0.0f }); }
	//	else if (input_->PressKey(DIK_A)) { camera_->MoveVector({ -1.0f,0.0f,0.0f }); }
	//}

	RaycastHit raycastHit_;

	lightGroup_->Update();
	camera_->Update();

	rayObj_2->SetPosition(fan_[0]->GetRay()->start_ + (50.0f * fan_[0]->GetRay()->dir_));

	stageCollision = CollisionStageFlag(player_, stage_);

	if (stage_->GetIsGoal())
	{
		isClear_ = true;
	}

	if (isClear_)
	{
		pm1_->Active(particle1_, 100.0f, 0.2f, 0.001f, 10, { 13.0f, 0.0f });
		pm2_->Active(particle2_, 30.0f, 0.2f, 0.001f, 5, { 6.0f,0.0f });

		ImGui::Begin("Stage Clear!");
		ImGui::SetWindowPos(ImVec2(10, 10));
		ImGui::SetWindowSize(ImVec2(500, 200));
		ImGui::SetWindowFontScale(2.0f);
		ImGui::Text("Pless Enter Next Stage");
		ImGui::End();
		if (input_->TriggerKey(DIK_RETURN))
		{
			switch (scene_)
			{
			case Stage0:

				//ここで次のステージ(ここだとステージ1の値)の値をセット(サンプル)
				positionPlayer = { 28.0f,0.0f,24.0f };
				ParameterPlayer(positionPlayer, 1);

				positionFan[0] = { 60.0f,0.0f,50.0f };
				positionFan[1] = { -10.0f,0.0f,18.0f };
				positionFan[2] = { -36.0f,0.0f,42.0f };
				positionFan[3] = { 36.0f,0.0f,66.0f };
				positionFan[4] = { 12.0f,0.0f,90.0f };

				fan_[0]->SetIsControl(true);
				fan_[0]->SetFanDirection(Fan::Up);
				fan_[1]->SetIsControl(false);
				fan_[1]->SetFanDirection(Fan::Up);
				fan_[2]->SetIsControl(false);
				fan_[2]->SetFanDirection(Fan::Up);
				fan_[3]->SetIsControl(false);
				fan_[3]->SetFanDirection(Fan::Left);
				fan_[4]->SetIsControl(false);
				fan_[4]->SetFanDirection(Fan::Left);

				ParamaterFun(positionFan[0], positionFan[1], positionFan[2], positionFan[3], positionFan[4]);

				scene_ = Stage1;
				break;

			case Stage1:
				positionPlayer = { -28.0f,0.0f,40.0f };
				ParameterPlayer(positionPlayer, 2);

				positionFan[0] = { 60.0f,0.0f,60.0f };
				positionFan[1] = { 60.0f,0.0f,30.0f };
				positionFan[2] = { -36.0f,0.0f,26.0f };
				positionFan[3] = { 28.0f,0.0f,18.0f };
				positionFan[4] = { 20.0f,0.0f,90.0f };

				fan_[0]->SetIsControl(true);
				fan_[0]->SetFanDirection(Fan::Up);
				fan_[1]->SetIsControl(true);
				fan_[1]->SetFanDirection(Fan::Up);
				fan_[2]->SetIsControl(false);
				fan_[2]->SetFanDirection(Fan::Right);
				fan_[3]->SetIsControl(false);
				fan_[3]->SetFanDirection(Fan::Up);
				fan_[4]->SetIsControl(false);
				fan_[4]->SetFanDirection(Fan::Left);

				ParamaterFun(positionFan[0], positionFan[1], positionFan[2], positionFan[3], positionFan[4]);
				scene_ = Stage2;
				break;

			case Stage2:
				positionPlayer = { 36.0f,0.0f,82.0f };
				ParameterPlayer(positionPlayer, 3);

				positionFan[0] = { 60.0f,0.0f,80.0f };
				positionFan[1] = { 60.0f,0.0f,50.0f };
				positionFan[2] = { 60.0f,0.0f,20.0f };
				positionFan[3] = { 12.0f,0.0f,90.0f };
				positionFan[4] = { -12.0f,0.0f,66.0f };

				fan_[0]->SetIsControl(true);
				fan_[0]->SetFanDirection(Fan::Up);
				fan_[1]->SetIsControl(true);
				fan_[1]->SetFanDirection(Fan::Up);
				fan_[2]->SetIsControl(true);
				fan_[2]->SetFanDirection(Fan::Up);
				fan_[3]->SetIsControl(false);
				fan_[3]->SetFanDirection(Fan::Down);
				fan_[4]->SetIsControl(false);
				fan_[4]->SetFanDirection(Fan::Down);

				ParamaterFun(positionFan[0], positionFan[1], positionFan[2], positionFan[3], positionFan[4]);
				scene_ = Stage3;
				break;
			case Stage3:
				//positionPlayer = { 8.0f,0.0f,20.0f };
				//ParameterPlayer(positionPlayer, 0);

				//positionFan[0] = { 60.0f, 0.0f, 50.0f };
				//positionFan[1] = { 60.0f,0.0f,50.0f };
				//positionFan[2] = { 60.0f,0.0f,50.0f };
				//positionFan[3] = { 60.0f,0.0f,50.0f };
				//positionFan[4] = { 60.0f,0.0f,50.0f };
				////ファン下向き時の数値設定
				//fan_[0]->SetFanDirection(Fan::Down);
				//fan_[0]->SetIsControl(true);

				////ファン左向き時の数値設定
				//fan_[1]->SetFanDirection(Fan::Down);
				//fan_[1]->SetIsControl(true);

				////ファン右向き時の数値設定
				//fan_[2]->SetFanDirection(Fan::Down);
				//fan_[2]->SetIsControl(true);

				//fan_[3]->SetFanDirection(Fan::Down);
				//fan_[3]->SetIsControl(true);

				//fan_[4]->SetFanDirection(Fan::Down);
				//fan_[4]->SetIsControl(true);
				//ParamaterFun(positionFan[0], positionFan[1], positionFan[2], positionFan[3], positionFan[4]);

				//scene_ = Stage0;
				if (input_->TriggerKey(DIK_RETURN)) {
					//シーンの切り替えを依頼
					SceneManager::GetInstance()->ChangeScene("TITLE");
				}
				break;
			case Stage4:
				ParameterPlayer(positionPlayer, 0);
				ParamaterFun(positionFan[0], positionFan[1], positionFan[2], positionFan[3], positionFan[4]);
				scene_ = Stage0;
				break;
			}
		}
	}
	else
	{
		player_->Update();

		rayObj_->Update();

		rayObj_2->Update();

		for (int i = 0; i < FanCount_; i++) {
			fan_[i]->Update();
		}
		if (input_->TriggerKey(DIK_R))
		{
			ReSetPositionPlayer(positionPlayer);
			ReSetPositionFan(positionFan[0], positionFan[1], positionFan[2], positionFan[3], positionFan[4]);
		}
	}


	skydome_->Update();


	//レイキャストをチェック
	for (int i = 0; i < FanCount_; i++) {
		if (collisionManager_->Raycast(*fan_[i]->GetRay(), COLLISION_ATTR_PLAYER, &raycastHit_)) {

			if (raycastHit_.distance_ <= 50.0f) {

				rayObj_->SetPosition(raycastHit_.inter_);
				rayObj_->Update();

				raycastHit_.object_->SetRotation(fan_[i]->GetRotation());
				raycastHit_.object_->Update();

				colRay_ = true;
				interRay_ = raycastHit_.inter_;
			}
		}
	}

	stage_->Update();
	//全ての衝突をチェック
	collisionManager_->CheckAllCollisions();

	player_->OnCollisionStage(stageCollision, positionPlayer);

	pm1_->Update();
	pm2_->Update();


}

void GamePlayScene::Update2d() {
	// 現在の座標を取得
	Vector2 position = input_->GetMousePosition();

	//移動後の座標を計算

	// 座標の変更を反映
	sprite_->SetPosition(position);

	sprite_->Update();
}

void GamePlayScene::Draw3d() {
	skydome_->Draw();

	rayObj_->Draw();
	rayObj_2->Draw();
	for (int i = 0; i < FanCount_; i++) {
		fan_[i]->Draw();
	}
	player_->Draw();
	stage_->Draw();
}

void GamePlayScene::DrawParticle()
{
	pm1_->Draw();
	pm2_->Draw();
}

void GamePlayScene::Draw2d() {
	//sprite_->Draw();
}

void GamePlayScene::Finalize() {
	player_->Finalize();
	SafeDelete(player_);
	SafeDelete(playerModel_);

	for (int i = 0; i < FanCount_; i++) {
		fan_[i]->Finalize();
		SafeDelete(fan_[i]);
	}
	SafeDelete(fanModel_);

	SafeDelete(stage_);

	skydome_->Finalize();
	SafeDelete(skydome_);

	SafeDelete(rayObj_);
	SafeDelete(rayObj_2);
	SafeDelete(rayModel_);
	//パーティクル
	SafeDelete(particle1_);
	SafeDelete(pm1_);
	SafeDelete(particle2_);
	SafeDelete(pm2_);

	SafeDelete(sprite_);

	SafeDelete(lightGroup_);
	SafeDelete(camera_);
}

bool GamePlayScene::CollisionStageFlag(Player* p, Stage* s)
{
	// 各座標変数の宣言
	Vector3 pPos = p->GetPosition();
	float pRadius = p->GetRadius();
	float bscale = s->GetBlockSize();

	float pX1, pX2, pZ1, pZ2;
	// プレイヤーの矩形座標
	pX1 = pPos.x - pRadius;
	pX2 = pPos.x + pRadius;
	pZ1 = pPos.z - pRadius;
	pZ2 = pPos.z + pRadius;

	// プレイヤーLeftTop座標
	int pLT[2] = { static_cast<int>((pX1 / (bscale * 2) + 5))/* * -1)*/,
		static_cast<int>(((pZ1 / (bscale * 2)) - 11) * -1) };
	int isFloor = 0;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			// 足元のブロックを判別
			if (s->CheckFloorBlock(pLT[0] + i, pLT[1] + j)) {
				isFloor++;
			}
			if (isFloor == 2) {
				p->Stop(positionPlayer);
			}
			s->CheckBlock(pLT[0] + i, pLT[1] + j);
			// 各座標変数の宣言
			Vector3 bPos = s->GetBlockPosition(pLT[0] + i, pLT[1] + j);
			float bRadius = s->GetRadius();
			float bX1, bX2, bZ1, bZ2;
			// ブロックの矩形座標
			bX1 = bPos.x - bRadius;
			bX2 = bPos.x + bRadius;
			bZ1 = bPos.z - bRadius;
			bZ2 = bPos.z + bRadius;

			// 当たり判定
			if (pX1 < bX2 && pX2 > bX1 && pZ1 < bZ2 && pZ2 > bZ1) {

				return true;
			}
		}
	}

	return false;
}

void GamePlayScene::ParameterPlayer(const Vector3& playerPos, const int& stageNum)
{
	Vector3 pos = playerPos;
	player_->SetPosition(pos);
	player_->Update();
	player_->Stop(pos);

	stage_->StageInitialize(filename_[stageNum]);
	isClear_ = false;

}

void GamePlayScene::ParamaterFun(const Vector3& fanPos1, const Vector3& fanPos2, const Vector3& fanPos3, const Vector3& fanPos4, const Vector3& fanPos5)
{
	Vector3 pos[FanCount_] = { {fanPos1},{fanPos2},{fanPos3},{fanPos4},{fanPos5} };
	for (size_t i = 0; i < FanCount_; i++)
	{
		fan_[i]->SetPosition(pos[i]);
		fan_[i]->Update();
	}

}

void GamePlayScene::ReSetPositionPlayer(const Vector3& playerPos)
{
	Vector3 pos = playerPos;
	player_->SetPosition(pos);

}

void GamePlayScene::ReSetPositionFan(const Vector3& fanPos1, const Vector3& fanPos2, const Vector3& fanPos3, const Vector3& fanPos4, const Vector3& fanPos5)
{
	Vector3 pos[FanCount_] = { {fanPos1},{fanPos2},{fanPos3},{fanPos4},{fanPos5} };
	for (size_t i = 0; i < FanCount_; i++)
	{
		fan_[i]->SetPosition(pos[i]);

	}

}
