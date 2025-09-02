//================================================
//
// 練習用の敵 [TrainingEnemy.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "TrainingEnemy.h"
#include"motion.h"
#include"transform.h"
#include"player.h"
#include"tutorial.h"
#include"Collider.h"
#include"statebase.h"
#include"math.h"
#include"TrainingEnemyState.h"
#include"manager.h"
#include"renderer.h"
#include "Collision.h"

using namespace std;  // 名前空間stdを使用
using namespace math; // 名前空間mathを使用

//================================================
// コンストラクタ
//================================================
CTrainingEnemy::CTrainingEnemy()
{
	m_pCapsule = nullptr;
	m_pMachine = nullptr;
	D3DXMatrixIdentity(&m_SwordMtx);
}

//================================================
// デストラクタ
//================================================
CTrainingEnemy::~CTrainingEnemy()
{
}

//================================================
// 生成処理
//================================================
CTrainingEnemy* CTrainingEnemy::Create(const D3DXVECTOR3 pos)
{
	CTrainingEnemy* pEnemy = new CTrainingEnemy;
	
	// 位置の取得
	pEnemy->SetPosition(pos);

	// 初期化処理
	if (FAILED(pEnemy->Init()))
	{
		pEnemy->Uninit();
		pEnemy = nullptr;
		return nullptr;
	}
	return pEnemy;
}

//================================================
// 初期化処理
//================================================
HRESULT CTrainingEnemy::Init(void)
{
	// 初期化処理
	if (FAILED(CCharacter3D::Init()))
	{
		return E_FAIL;
	}

	// モーションのロード
	CCharacter3D::LoadMotion("motionTrainingEnemy.txt", MOTIONTYPE_MAX);
	CCharacter3D::SetCharacter(9999, 0.0f, D3DXVECTOR3(5.0f, 1.0f, 5.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// 向きの設定
	CCharacter3D::GetRotaition()->Set(D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));
	CCharacter3D::GetRotaition()->SetDest(D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));

	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();
	D3DXVECTOR3 headPos = CCharacter3D::GetModelPos(MODEL_HEAD);

	// カプセルの生成
	m_pCapsule = CColliderCapsule::Create(pos, headPos, 55.0f, 0);

	// 状態マシーンの生成
	m_pMachine = make_unique<CStateMachine>();

	// 初期状態の設定
	ChangeState(make_shared<CTrainingEnemyIdle>());

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CTrainingEnemy::Uninit(void)
{
	m_pCapsule = nullptr;
	m_pMachine = nullptr;

	// 終了処理
	CCharacter3D::Uninit();
}

//================================================
// 更新処理
//================================================
void CTrainingEnemy::Update(void)
{
	// モーションの取得
	CMotion* pMotion = CCharacter3D::GetMotion();

	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();
	D3DXVECTOR3 headPos = CCharacter3D::GetModelPos(MODEL_HEAD);

	if (m_pCapsule != nullptr && pMotion->GetBlendType() != MOTIONTYPE_DAMAGE)
	{
		// コライダーのデータの取得
		CColliderCapsule::Data data = m_pCapsule->GetData();

		// データの更新
		data.StartPos = pos;
		data.EndPos = headPos;

		// コライダーの更新
		m_pCapsule->UpdateData(data);

		// 当たり判定
		pPlayer->CollisionCapsule(m_pCapsule.get());

		// 攻撃の判定処理
		CollisionPlayerAttack();
	}

	if (m_pMachine != nullptr)
	{
		// 状態の更新処理
		m_pMachine->Update();
	}

	// 更新処理
	CCharacter3D::Update();

	// モーションの更新処理
	CCharacter3D::UpdateMotion();
}

//================================================
// 描画処理
//================================================
void CTrainingEnemy::Draw(void)
{
	// 描画処理
	CCharacter3D::Draw();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_SwordMtx);

	// 親の位置、向きの設定
	D3DXVECTOR3 ParentPos = CCharacter3D::GetModelPos(MODEL_WEPON);
	D3DXVECTOR3 ParentRot = CCharacter3D::GetModelRot(MODEL_WEPON);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, ParentRot.y, ParentRot.x, ParentRot.z);
	D3DXMatrixMultiply(&m_SwordMtx, &m_SwordMtx, &mtxRot);

	// 大きさの取得
	D3DXVECTOR3 Size = CCharacter3D::GetModelSize(MODEL_WEPON);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, 0.0f, Size.y, 0.0f);
	D3DXMatrixMultiply(&m_SwordMtx, &m_SwordMtx, &mtxTrans);

	// 親のワールドマトリックスの取得
	pDevice->GetTransform(D3DTS_WORLD, &mtxParent);

	// 親のワールドマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_SwordMtx, &m_SwordMtx, &mtxParent);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_SwordMtx);
}

//================================================
// 範囲内にいるかどうか
//================================================
bool CTrainingEnemy::CheckDistance(float fRange)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerpos = pPlayer->GetPosition();

	// 敵の位置の取得
	D3DXVECTOR3 enemypos = CCharacter3D::GetPosition();

	// 距離の取得
	float distance = GetDistance(playerpos - enemypos);

	// 一定の範囲に入ったら
	if (distance <= fRange)
	{
		return true;
	}
	return false;
}

//================================================
// 状態の変更
//================================================
void CTrainingEnemy::ChangeState(std::shared_ptr<CTrainingEnemyState> pNewState)
{
	if (pNewState != nullptr)
	{
		// 自分のポインタを渡す
		pNewState->SetOwner(this);
	}

	if (m_pMachine != nullptr)
	{
		// 状態の変更
		m_pMachine->Change(pNewState);
	}
}

//================================================
// 攻撃の結果の取得
//================================================
CTrainingEnemy::RESULT CTrainingEnemy::GetAttackResult(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = GetPosition();

	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return RESULT_NONE;

	// プレイヤーのモーションの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// プレイヤーのモーションの取得
	int playerMotionType = pPlayerMotion->GetBlendType();

	// 武器が当たったら
	if (CollisionPlayer())
	{
		if (playerMotionType == pPlayer->MOTIONTYPE_REVENGE)
		{
			// パリィした
			return RESULT_SPREVENGE;
		}
		else if (playerMotionType != pPlayer->MOTIONTYPE_PARRY)
		{
			// パリィできるか判定
			const bool bParry = pPlayer->IsParry(pos);

			// パリィできた
			if (bParry)
			{
				// パリィした
				return RESULT_PARRY;
			}
			// 回避だったら
			else if (playerMotionType == pPlayer->MOTIONTYPE_AVOID || pPlayerMotion->GetType() == pPlayer->MOTIONTYPE_AVOID)
			{
				// 回避した
				return RESULT_AVOID;
			}
			// カウンター失敗した
			else if (bParry == false)
			{
				// 当たった
				return RESULT_HIT;
			}
		}
	}

	return RESULT_NONE;
}

//================================================
// プレイヤーの攻撃の判定
//================================================
void CTrainingEnemy::CollisionPlayerAttack(void)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// プレイヤーのモーションの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// パリィモーションの蹴りになったら
	if (pPlayerMotion->IsEventFrame(38, 38, pPlayer->MOTIONTYPE_ROUNDKICK))
	{
		// プレイヤーの右足の位置
		D3DXVECTOR3 playerFootR = pPlayer->GetModelPos(CPlayer::MODEL_FOOTR);

		// 円の当たり判定の取得
		CCollisionCapsule* pCapsule = CCollisionCapsule::GetInstance();

		// 右手の円
		CColliderSphere FootRSphere = CColliderSphere::CreateCollider(playerFootR, 250.0f);

		// 手が当たったら
		if (pCapsule != nullptr && pCapsule->CollisionSphere(m_pCapsule.get(), &FootRSphere))
		{
			// 状態の変更
			ChangeState(make_shared<CTrainingEnemyDamage>());
		}
	}

	//// パリィモーションの蹴りになったら
	//if (pPlayerMotion->IsEventFrame(13, 13, pPlayer->MOTIONTYPE_PUNCH) && IsDamageMotion() == false)
	//{
	//	// プレイヤーの右手の位置
	//	D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(5);

	//	// 円の当たり判定の取得
	//	CCollisionSphere* pSphere = CCollisionSphere::GetInstance();

	//	// 右手の円
	//	CColliderSphere HandRSphere = CColliderSphere::CreateCollider(playerHandR, 80.0f);
	//	CColliderSphere ChestSphere = CColliderSphere::CreateCollider(chestpos, 250.0f);

	//	// 手が当たったら
	//	if (pSphere != nullptr && pSphere->Collision(&ChestSphere, &HandRSphere))
	//	{
	//		// どの攻撃モーションがでるか判定
	//		SelectDamageMotion(m_nParrySuccess, playerHandR);
	//	}
	//}
}

//================================================
// プレイヤーとの当たり判定
//================================================
bool CTrainingEnemy::CollisionPlayer(void)
{
	// 位置の取得
	D3DXVECTOR3 sword_buttom = CCharacter3D::GetModelPos(MODEL_WEPON);
	D3DXVECTOR3 sword_Top = GetPositionFromMatrix(m_SwordMtx);

	// コライダーの作成
	CColliderCapsule capsule = CColliderCapsule::CreateCollider(sword_buttom, sword_Top,50.0f);

	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// プレイヤーの取得
	if (pPlayer == nullptr) return false;

	// プレイヤーとの当たり判定
	if (pPlayer->CollisionCapsule(&capsule))
	{
		return true;
	}

	return false;
}
