﻿#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include "Camera.h"
#include "Particle.h"

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector2.h"

/// <summary>
/// 3Dオブジェクト
/// </summary>
class ParticleManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	

public: // サブクラス
	// 頂点データ構造体
	//struct VertexPosNormalUv
	//{
	//	Vector3 pos; // xyz座標
	//	Vector3 normal; // 法線ベクトル
	//	Vector2 uv;  // uv座標
	//};


	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		//Vector4 color;	// 色 (RGBA)
		Matrix4 mat;// ３Ｄ変換行列
		Matrix4 matBillboard;	//ビルボード行列
	};
	
private: // 定数
	static const int division = 50;					// 分割数
	static const float radius;				// 底面の半径
	static const float prizmHeight;			// 柱の高さ
	static const int planeCount = division * 2 + division * 2;		// 面の数
	static const int vertexCount = 1024;		// 頂点数
	//static const int indexCount = 3 * 2;	//インデックス数
public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static ParticleManager* Create();

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device_;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;

private:// 静的メンバ関数

	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();

public: // メンバ関数
	bool Initialize();
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// パーティクル発射
	/// </summary>
	/// <param name="p">パーティクル</param>
	/// <param name="setpos">位置</param>
	/// <param name="setvel">移動量</param>
	/// <param name="setacc">重力分布</param>
	/// <param name="setnum">一気に何個か</param>
	///  <param name="setscale">x = 開始スケール , y = 終了スケール</param>
	void Active(Particle* p, const float& setpos, const float& setvel, const float& setacc, const int& setnum, const Vector2& setscale);


private: // メンバ変数
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	// ローカルスケール
	Vector3 scale = { 1.0f,1.0f,1.0f };
	
	Particle* particle_ = nullptr;

	Camera* camera_ = nullptr;

public://アクセッサ置き場
	//パーティクルモデル
	void SetParticleModel(Particle* pmodel) { this->particle_ = pmodel; }
	//カメラ
	void SetCamera(Camera* camera) { this->camera_ = camera; }
};

