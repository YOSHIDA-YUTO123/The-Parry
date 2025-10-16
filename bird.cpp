//===================================================
//
// 鳥 [bird.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "bird.h"
#include "BirdState.h"
#include "motion.h"
#include "CharacterManager.h"
#include "BirdManager.h"
#include "Collision.h"
#include "Collider.h"
#include "transform.h"
#include "game.h"
#include"meshfield.h"
#include "manager.h"
#include "wing.h"
#include "velocity.h"

using namespace std;	// 名前空間stdの使用
using namespace Const;	// 名前空間Constの使用

//***************************************************
// 定数宣言
//***************************************************
constexpr float BIRD_RADIUS = 50.0f; // 鳥の半径
constexpr int MAX_LIFE = 180;		 // 鳥の寿命
constexpr int POP_TIME = 600;		 // 鳥の最大の寿命

//***************************************************
// 静的メンバ変数宣言
//***************************************************
bool CBird::m_bLoad = false;  // ロードしたかどうか

//===================================================
// コンストラクタ
//===================================================
CBird::CBird() : CCharacter3D(TYPE_BIRD)
{
	m_fTimeOffset = NULL;
	m_fTime = NULL;
	m_type = TYPE_NORMAL;
	m_bGravity = true;
	m_nMaxLife = POP_TIME;
	m_nLife = MAX_LIFE;
	m_pMove = nullptr;
	m_pMachine = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CBird::~CBird()
{
}

//===================================================
// 生成処理
//===================================================
CBird* CBird::Create(const D3DXVECTOR3 pos, const TYPE type, const bool bGravity)
{
	CBird* pBird = new CBird;

	// 位置の設定
	pBird->SetPosition(pos);
	pBird->m_bGravity = bGravity;
	pBird->m_type = type;

	// 初期化処理
	if (FAILED(pBird->Init()))
	{
		pBird->Uninit();
		pBird = nullptr;
		return nullptr;
	}

	// ランダムな向きの選出
	float fAngleY = static_cast<float>(rand() % 629 - 314) * 0.01f;

	// 向きの設定
	pBird->SetRotation(D3DXVECTOR3(0.0f, fAngleY, 0.0f));

	return pBird;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CBird::Init(void)
{
	// 初期化処理
	if (FAILED(CCharacter3D::Init()))
	{
		return E_FAIL;
	}
	
	// 鳥のマネージャーの取得
	auto pBirdManager = CBirdManager::GetInstance();

	// 取得出来たら
	if (pBirdManager != nullptr)
	{
		// リストに追加
		pBirdManager->AddBirdList(this);
	}

	if (!m_bLoad)
	{
		// 鳥のモーションのロード
		CCharacter3D::LoadMotion("motionBird.txt", MOTIONTYPE_MAX);

		// モーションのロード
		m_bLoad = true;
	}
	else
	{
		CCharacter3D::SetCharacter();
	}

	// 状態マシーンの生成
	m_pMachine = make_unique<CStateMachine>();

	// 種類
	switch (m_type)
	{
	case TYPE_NORMAL:
		// 状態の設定
		ChangeState(make_shared<CBirdIdle>());
		break;
	case TYPE_FLY_MOVE:
		// 状態の設定
		ChangeState(make_shared<CBirdFlyMove>());
		break;
	default:
		break;
	}

	// キャラクターの設定処理
	CCharacter3D::SetCharacter(10, 6.0f, D3DXVECTOR3(2.0f, 1.0f, 2.0f), D3DXVECTOR3(50.0f, 100.0f, 50.0f));

	// 移動クラスの生成
	m_pMove = make_unique<CVelocity>();

	// 2πをintに変換
	int nTwoPI = static_cast<int>(D3DX_PI * 2);

	// オフセットを選出
	m_fTimeOffset = static_cast<float>(rand() % nTwoPI);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CBird::Uninit(void)
{
	m_pMove.reset();
	m_pMachine.reset();

	// 終了処理
	CCharacter3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CBird::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	if (m_pMove != nullptr)
	{
		// 移動量の減衰
		m_pMove->SetInertia3D(0.25f);

		// 位置の更新
		pos += m_pMove->Get();
	}

	// メッシュフィールドの取得
	CMeshField* pMeshField = CGame::GetField();

	float fHeight = 0.0f;

	// メッシュフィールドの当たり判定
	if (pMeshField != nullptr && pMeshField->Collision(pos, &fHeight))
	{
		pos.y = fHeight;
	}

	if (m_pMove != nullptr && m_bGravity)
	{
		// 重力の処理
		m_pMove->Gravity(-MAX_GRABITY);
	}
	if (m_pMachine != nullptr)
	{
		// 状態の更新処理
		m_pMachine->Update();
	}

	// モーション更新処理
	CCharacter3D::UpdateMotion();

	// 更新処理
	CCharacter3D::Update();

	// 位置の設定
	CCharacter3D::SetPosition(pos);

	// 現在のモードの取得
	CScene::MODE mode = CManager::GetMode();

	// モードがリザルトだったら
	if (mode == CScene::MODE_RESULT) return;

	// モーションの取得
	CMotion* pMotion = CCharacter3D::GetMotion();

	if (pMotion != nullptr)
	{
		int motionFly = pMotion->GetBlendType();

		if (motionFly == MOTIONTYPE_FLY)
		{
			// 寿命を減らす
			m_nLife--;

			// 寿命が尽きたら
			if (m_nLife <= 0)
			{
				// 羽の生成
				CWing::Create(pos);

				// 破棄
				DeleteList();

				return;
			}
		}
	}

	m_nMaxLife--;

	// 寿命が尽きたら
	if (m_nMaxLife <= 0)
	{
		// 状態の変更
		ChangeState(make_shared<CBirdFly>());

		return;
	}
}

//===================================================
// 描画処理
//===================================================
void CBird::Draw(void)
{
	// 割合を求める
	float fRate = m_nLife / static_cast<float>(MAX_LIFE);

	// 描画処理
	CCharacter3D::Draw(fRate);
}

//===================================================
// 状態の変更
//===================================================
void CBird::ChangeState(std::shared_ptr<CBirdState> pNewState)
{
	if (pNewState != nullptr)
	{
		// オーナーの設定
		pNewState->SetOwner(this);
	}

	if (m_pMachine != nullptr)
	{
		// 状態の設定
		m_pMachine->Change(pNewState);
	}
}

//===================================================
// 対象の位置との距離の判定
//===================================================
bool CBird::CheckDistance(const D3DXVECTOR3 otherPos, const float fRadius)
{
	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	// 自分の円
	auto mySphere = CColliderSphere::CreateCollider(pos, BIRD_RADIUS);

	// 相手の円
	auto OtherSphere = CColliderSphere::CreateCollider(otherPos, fRadius);

	// 円の当たり判定
	if (CCollisionSphere::Collision(&mySphere, &OtherSphere))
	{
		// 状態の変更
		ChangeState(make_shared<CBirdFly>());

		return true;
	}

	return false;
}

//===================================================
// 飛行の設定処理
//===================================================
void CBird::SetFly(const D3DXVECTOR3 move)
{
	// 計算用の移動量
	D3DXVECTOR3 moveWk = VEC3_NULL;

	// 現在の向きの取得
	float fAngleY = CCharacter3D::GetRotation().y;

	moveWk.x = sinf(fAngleY + D3DX_PI) * move.x;
	moveWk.y = move.y;
	moveWk.z = cosf(fAngleY + D3DX_PI) * move.z;

	if (m_pMove != nullptr)
	{
		// 移動量の設定
		m_pMove->Set(moveWk);
	}
}

//===================================================
// 飛びながら移動
//===================================================
void CBird::FlyMove(const float fMove)
{
	// nullなら処理しない
	if (m_pMove == nullptr) return;

	// 計算用の移動量
	D3DXVECTOR3 moveWk = m_pMove->Get();

	m_fTime += 0.02f;

	float fTime = m_fTime + m_fTimeOffset;

	// 円運動の速度ベクトルを計算
	moveWk.x = -sinf(fTime) * fMove;
	moveWk.z = cosf(fTime) * fMove;
	
	// 移動量の設定
	m_pMove->Set(moveWk);

	// 移動量の正規化
	D3DXVec3Normalize(&moveWk, &moveWk);

	// 移動方向を求める
	float fAngle = atan2f(moveWk.x, moveWk.z);

	CCharacter3D::SetRotation(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));
}

//===================================================
// リストからの消去
//===================================================
void CBird::DeleteList(void)
{
	// 鳥のマネージャーの取得
	auto pBirdManager = CBirdManager::GetInstance();

	// 取得出来たら
	if (pBirdManager != nullptr)
	{
		// 影の消去
		CCharacter3D::DeleteShadow();

		// リストから切り離す
		pBirdManager->Erase(this);
	}
}
