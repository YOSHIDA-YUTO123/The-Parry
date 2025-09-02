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

	if (m_pCapsule != nullptr)
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

	return RESULT();
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
