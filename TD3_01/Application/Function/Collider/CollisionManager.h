#pragma once

#include <forward_list>

class BaseCollider;

class CollisionManager {
public: //静的メンバ関数
	static CollisionManager* GetInstance();

public: //メンバ関数
	//コライダー追加
	inline void AddCollider(BaseCollider* collider) {
		colliders_.push_front(collider);
	}

	//コライダーの削除
	inline void RemoveCollider(BaseCollider* collider) {
		colliders_.remove(collider);
	}

	//全ての衝突チェック
	void CheckAllCollisions();

private: //メンバ変数
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator =(const CollisionManager&) = delete;
	//コライダーのリスト
	std::forward_list<BaseCollider*> colliders_;
};