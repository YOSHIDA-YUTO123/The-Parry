//===================================================
//
// 敵 [enemy.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "enemy.h"
#include "player.h"
#include"manager.h"
#include"renderer.h"
#include "math.h"
#include"shadow.h"
#include "dust.h"
#include"explosion.h"
#include "impact.h"
#include "LoadManager.h"
#include"particle.h"
#include"motion.h"
#include"Wave.h"
#include"EnemyState.h"
#include "Orbit.h"
#include"statebase.h"
#include "obstaclemanager.h"
#include"Obstacle.h"
#include"Collider.h"
#include "playerstate.h"
#include"input.h"
#include"meshfield.h"
#include"cylinder.h"
#include"slow.h"
#include"GameCamera.h"
#include "game.h"
#include"Observer.h"
#include "MoveSmoke.h"
#include "ParryEffect.h"
#include "BirdManager.h"
#include"ExplosionArea.h"
#include"sound.h"
//***************************************************
// 定数定義
//***************************************************
constexpr float SHADOW_ALEVEL = 0.7f;			// 影のアルファ値
constexpr float SHADOW_MAX_HEIGHT = 500.0f;		// 影が見える最大の高さ
constexpr float SHADOW_SIZE = 150.0f;			// 影の大きさ
constexpr float ROCKON_HEIGHT = 100.0f;			// ロックオン時の見る場所
constexpr float INERTIA = 0.25f;				// 慣性
constexpr float RUSH_EFFECT_POS = -350.0f;		// 突進エフェクトの位置

constexpr int NUM_RUBBLE = 16;					// 瓦礫を出す数
constexpr int NUM_MATRIX = 8;					// 武器につけるマトリックスの数
constexpr int NEXT_ACTION_TIME = 300;			// 次の行動の抽選までの時間
constexpr int MODEL_WEPON = 15;					// 武器のインデックス

using namespace Const;							// 名前空間Constを使用する
using namespace math;							// 名前空間mathを使用する
using namespace std;							// 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CEnemy::CEnemy() : CCharacter3D(TYPE_ENEMY)
{
	m_bSetMatrix = false;
	m_pMove = nullptr;
	m_pMachine = nullptr;
	D3DXMatrixIdentity(&m_RushEffectMtx);
	D3DXMatrixIdentity(&m_weponMatrix);
	m_pOrbit = nullptr;
	m_posOld = VEC3_NULL;
	m_fInertia = INERTIA;
}

//===================================================
// デストラクタ
//===================================================
CEnemy::~CEnemy()
{
}

//===================================================
// 生成処理
//===================================================
CEnemy* CEnemy::Create(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size,const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CEnemy* pEnemy = nullptr;

	// 敵オブジェクトの生成
	pEnemy = new CEnemy;

	if (pEnemy == nullptr) return nullptr;

	// 構成情報
	pEnemy->Config(nLife, fSpeed, ShadowScal, Size);

	pEnemy->SetPosition(pos);

	if (FAILED(pEnemy->Init()))
	{
		pEnemy->Uninit();
		pEnemy = nullptr;
		return nullptr;
	}
	pEnemy->GetRotaition()->Set(rot);

	return pEnemy;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CEnemy::Init(void)
{
	m_pMachine = make_unique<CStateMachine>();

	ChangeState(make_shared<CEnemyIdle>(120));

	// 移動クラスの生成
	m_pMove = make_unique<CVelocity>();

	// 大きさの取得
	D3DXVECTOR3 Size = CCharacter3D::GetSize();

	// 位置の取得処理
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	// 当たり判定の生成
	m_pSphere = CColliderSphere::Create(pos,150.0f);
	
	// 中心を求める
	D3DXVECTOR3 CenterPos = VEC3_NULL;

	// 中心座標を設定
	CenterPos.x = pos.x;
	CenterPos.y = pos.y + Size.y * 0.5f;
	CenterPos.z = pos.z;

	// 矩形判定AABBの生成
	m_pAABB = CColliderAABB::Create(CenterPos, m_posOld, Size);

	// カプセルコライダーの生成
	m_pCapsule = CColliderCapsule::Create(pos, D3DXVECTOR3(pos.x, pos.y + Size.y, pos.z), 60.0f,0);

	// 視界の生成
	m_pFOV = CColliderFOV::Create(pos, 0.0f, D3DX_PI * 0.5f, -D3DX_PI * 0.5f, 1000.0f);

	// 移動制御クラスの生成
	m_pMovement = make_unique<CEnemyMovement>();

	m_pMovement->Init(m_pMove,this);

	// 状態マネージャーの生成
	m_pStateManager.reset(CEnemyStateManager::Create());
	m_pStateManager->SetOnwer(this);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CEnemy::Uninit(void)
{
	// nullにする
	m_pOrbit = nullptr;
	m_pAABB = nullptr;
	m_pCapsule = nullptr;
	m_pFOV = nullptr;
	m_pStateManager = nullptr;

	//for (const auto& pCapsule : m_apCapsule)
	//{
	//	pCapsule.
	//}
	// オブザーバーの破棄
	if (m_pObserver != nullptr)
	{
		delete m_pObserver;
		m_pObserver = nullptr;
	}

	// 終了処理
	CCharacter3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CEnemy::Update(void)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// カメラの取得処理
	CGameCamera* pCamera = CGame::GetCamera();

	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// モーションの取得
	auto pMotion = CCharacter3D::GetMotion();

	// モーションがなかったら処理しない
	if (pMotion == nullptr) return;

	// モーションのロードに失敗していたら処理しない
	if (pMotion->IsLoad() == false)
	{
		return;
	}

	MOTIONTYPE motiontype = static_cast<MOTIONTYPE>(pMotion->GetBlendType());

	float fDistance = motiontype == MOTIONTYPE_JUMPATTACK ? 600.0f : 600.0f;
	float fHeightV = motiontype == MOTIONTYPE_JUMPATTACK ? 100.0f : 160.0f;

	// カメラがnullじゃないなら
	if (pCamera != nullptr)
	{
		pCamera->Rockon(PlayerPos, D3DXVECTOR3(pos.x, pos.y + fHeightV, pos.z), fDistance);
	}

	if (CCharacter3D::HitStop())
	{
		return;
	}

#ifdef _DEBUG

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// プレイヤーと敵の距離を求める
	float distance = GetDistance(pos - PlayerPos);

	CDebugProc::Print("敵との距離 = [ %.2f ]\n", distance);
	CDebugProc::Print("************************************\n");
	CDebugProc::Print("*             敵の操作             *\n");
	CDebugProc::Print("************************************\n");
	CDebugProc::Print("ボスの攻撃(スマッシュ) [ 1 ]\n");
	CDebugProc::Print("ボスの攻撃(衝撃波) [ 2 ]\n");
	CDebugProc::Print("ボスの攻撃(方向→ダッシュ→回転) [ 3 ]\n");
	CDebugProc::Print("バックステップする [ 4 ]\n");
	CDebugProc::Print("ボスの攻撃(スイング) [ 5 ]\n");
	CDebugProc::Print("ボスの攻撃(ジャンプ攻撃) [ 6 ]\n");
	CDebugProc::Print("死亡モーション [ F4 ]\n");
	CDebugProc::Print("ボスの消去 [ F1 ]\n");

	if (pKeyboard->GetPress(DIK_1))
	{
		ChangeState(make_shared<CEnemyAttackSmash>());
	}
	if (pKeyboard->GetPress(DIK_2))
	{
		ChangeState(make_shared<CEnemyAttackImpact>());
	}
	if (pKeyboard->GetPress(DIK_3))
	{
		ChangeState(make_shared<CEnemyRoar>());
	}
	if (pKeyboard->GetTrigger(DIK_4))
	{
		ChangeState(make_shared<CEnemyBackStep>());
	}
	if (pKeyboard->GetTrigger(DIK_5))
	{
		ChangeState(make_shared<CEnemySwing>());
	}
	if (pKeyboard->GetTrigger(DIK_6))
	{
		ChangeState(make_shared<CEnemyJumpAttack>());
	}
	if (pKeyboard->GetTrigger(DIK_F4))
	{
		ChangeState(make_shared<CEnemyDeath>());
	}
	//if (pKeyboard->GetTrigger(DIK_F5))
	//{
	//	Hit(MAX_LIFE - 1);
	//}
	if (pKeyboard->GetTrigger(DIK_7))
	{
		ChangeState(make_shared<CEnemyRush>());
	}
	if (pKeyboard->GetTrigger(DIK_8))
	{
		ChangeState(make_shared<CEnemyLookBackL>());
	}

	if (pKeyboard->GetTrigger(DIK_F1))
	{
		// 影の消去
		CCharacter3D::DeleteShadow();
		Uninit();
		return;
	}

	static CMeshCylinder* pC = nullptr;

	if (pKeyboard->GetTrigger(DIK_T))
	{
		pCamera->SetShake(120, 20);
		//pCamera->SetState(pCamera->STATE_SHAKE);

		float fRadius = m_pCapsule->GetData().fRadius;
		float fHeight = GetDistance(m_pCapsule->GetData().EndPos - m_pCapsule->GetData().StartPos);
		pC = CMeshCylinder::Create(pos, 16, 1, fRadius, fHeight);
		pC->Set(pC->TYPE_VIEW);
	}

	if (pC != nullptr)
	{
		pC->SetPosition(pos);
	}

#endif // _DEBUG

	// モーションの制御クラスの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// 移動量の減衰
	m_pMove->SetInertia3D(m_fInertia);

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 前回の位置の更新
	m_posOld = pos;

	// 位置の更新処理
	pos += move;

	// メッシュフィールドの取得
	CMeshField* pMesh = CGame::GetField();

	float fHeight = 0.0f;

	// フィールドの当たり判定
	if (pMesh->Collision(pos, &fHeight))
	{
		pos.y = fHeight;

		if (pMotion->GetBlendType() == MOTIONTYPE_JUMP)
		{
			// 着地状態にする
			ChangeState(make_shared<CEnemyLanding>());
		}
		CCharacter3D::SetPosition(pos);
	}

	// 障害物との当たり判定
	if (CollisionObstacle(&pos))
	{

	}

	// コライダーの更新
	UpdateCollider(pos);

	if (pPlayer != nullptr)
	{
		// プレイヤーとの当たり判定
		pPlayer->CollisionCapsule(m_pCapsule.get());
	}

	// シリンダーの取得
	CMeshCylinder* pCylinder = CGame::GetCylinder();

	// シリンダーの当たり判定
	if (pCylinder != nullptr)
	{
		pCylinder->Collision(&pos);
	}
	
	// 鳥のマネージャの取得
	auto pBirdManager = CBirdManager::GetInstance();

	if (pBirdManager != nullptr)
	{
		// 鳥との距離の判定
		pBirdManager->CheckDistance(pos, 500.0f);
	}

	if (m_pMove != nullptr)
	{
		// 重力の設定
		m_pMove->Gravity(-MAX_GRABITY);
	}

	// インパクトとの判定
	const bool bCollision = pMesh->CollisionImpact(pos, 150.0f,CMeshFieldImpact::OBJ_ENEMY);

	// インパクトの当たり判定
	if (bCollision && IsDamageMotion() == false)
	{			
		// 状態の設定
		ChangeState(make_shared<CEnemyDamageL>(5));

		// モーションの設定
		pMotion->SetMotion(MOTIONTYPE_DAMAGEL, true, 2);
	}
	
	// プレイヤーとの当たり判定
	CollisionPlayer(pPlayerMotion, pPlayer);

	// 武器の先端の位置
	D3DXVECTOR3 WeponPos = GetPositionFromMatrix(m_weponMatrix);
	D3DXVECTOR3 WeponBottom = GetModelPos(15);

	// 胸の位置の取得
	D3DXVECTOR3 chestpos = GetModelPos(2);

	// 敵の攻撃のカウンターの目安の表示
	if (pMotion->IsEventFrame(50, 50, MOTIONTYPE_SMASH))
	{
		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestpos, 100.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1,314);
	}
	else if (pMotion->IsEventFrame(50, 50, MOTIONTYPE_IMPACT))
	{
		// ウェーブの生成
		auto pWave = CMeshWave::Create(pos, 50.0f, 50.0f, WHITE);

		// ウェーブの設定処理
		pWave->SetWave(30,50.0f);
	}
	else if (pMotion->IsEventFrame(40, 40, MOTIONTYPE_SWING))
	{
		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestpos, 100.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1,314);
	}

	// 攻撃モーションのたたきつけになったら
	if (pMotion->IsEventFrame(72,72, MOTIONTYPE_SMASH))
	{
		// 瓦礫の設定処理
		SetRubble();
	}

	// 衝撃波の生成
	if (pMotion->IsEventFrame(102, 102, MOTIONTYPE_IMPACT))
	{
		// プレイヤーまでの方向
		D3DXVECTOR3 dir = PlayerPos - WeponPos;

		// インパクトの設定
		CMeshFieldImpact::Config config = { WeponPos,chestpos,dir,D3DXCOLOR(1.0f,0.5f,0.5f,1.0f),CMeshFieldImpact::OBJ_ENEMY,150.0f,750.0f,26.0f,60 };

		// インパクトの生成
		pMesh->SetImpact(config);
	}

	// カメラが使われていたら
	if (pCamera != nullptr)
	{
		// カメラ追従
		pCamera->SetTracking(D3DXVECTOR3(chestpos.x, chestpos.y + 500.0f, chestpos.z), chestpos, 0.1f, CGameCamera::TRACKOBJ_ENEMY);
	}

	// モーションの更新処理
	CCharacter3D::UpdateMotion();

	if (m_pMachine != nullptr)
	{
		// 状態の更新処理
		m_pMachine->Update();
	}

	// オブザーバーへの通知処理
	Notify();

	if (CCharacter3D::GetAlive() == false && pMotion->GetBlendType() != MOTIONTYPE_DEATH && pMotion->GetBlendType() != MOTIONTYPE_DOWN)
	{
		// 状態を追従にする
		pCamera->SetState(CGameCamera::STATE_TRACKING);

		// 敵を追従する
		pCamera->SetTracking(CGameCamera::TRACKOBJ_ENEMY);

		// HPが無かったら
		ChangeState(make_shared<CEnemyDeath>());
	}
	
	// コライダーの更新
	UpdateCollider(pos);

	// 位置の設定処理
	CCharacter3D::SetPosition(pos);

	// キャラクターの更新
	CCharacter3D::Update();

	// 向きの補間
	CCharacter3D::GetRotaition()->SetSmoothAngle(0.1f);

	//// モデルの位置の取得
	//D3DXVECTOR3 EffectPos = GetPositionFromMatrix(m_RushEffectMtx);

	//auto p = CEffect3D::Create(EffectPos, 50.0f, WHITE, CEffect3D::TYPE_NORAML);
	//p->Set(60, VEC3_NULL);

	//// 腰
	//D3DXVECTOR3 waist = CCharacter3D::GetModelPos(MODEL_WAIST);
}

//===================================================
// 描画処理
//===================================================
void CEnemy::Draw(void)
{
	// キャラクターの描画
	CCharacter3D::Draw();

	// 親子関係の設定処理
	SetParent(MODEL_ARMUR,D3DXVECTOR3(0.0f,0.0f,0.0f),&m_RushEffectMtx);

	// 大きさの取得
	D3DXVECTOR3 Size = CCharacter3D::GetModelSize(MODEL_WEPON);

	// 親子関係の設定処理
	SetParent(MODEL_WEPON,D3DXVECTOR3(0.0f, Size.y,0.0f),&m_weponMatrix);

	m_bSetMatrix = true;
}

//===================================================
// 位置の取得処理
//===================================================
D3DXVECTOR3 CEnemy::GetPosition(void)
{
	// 位置
	D3DXVECTOR3 Outpos = VEC3_NULL;

	// 位置の取得
	Outpos = CCharacter3D::GetPosition();
	
	return Outpos;
}

//===================================================
// 移動制御クラスの取得
//===================================================
CEnemyMovement* CEnemy::GetMovement(void)
{
	return m_pMovement.get();
}

//===================================================
// どのダメージモーションが出るか判定
//===================================================
void CEnemy::SelectDamageMotion(int success,const D3DXVECTOR3 ImpactPos)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// ランダムな数値の選出
	int random = rand() % 100;

	// 30%の確率でガードする
	if (random <= 30 && success != CPlayer::PARRY_PARFECT)
	{
		// ガードする
		ChangeState(make_shared<CEnemyGuard>(ImpactPos,2));

		return;
	}

	// 成功度の遷移
	switch (success)
	{
	case CPlayer::PARRY_MISS:
		break;
	case CPlayer::PARRY_WEAK:
	{
		// 状態の設定
		ChangeState(make_shared<CEnemyDamageS>(1));

		// 位置の取得
		D3DXVECTOR3 pos = GetPosition();

		// プレイヤーの位置の取得
		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(ImpactPos, 10.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// パーティクルの設定処理
		pParticle->SetParticle(15.0f, 240, 50, 5,314);

		// ボスまでの角度を取得
		float fAngle = GetTargetAngle(pos, PlayerPos);

		// 向きの設定
		pPlayer->SetAngle(fAngle + D3DX_PI);

		// インパクトを生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.6f, 0.8f), ImpactPos, 0.0f, 50.0f);

		// サークルの設定処理
		pCircle->SetCircle(35.0f, 15.0f, 60, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));
	}
		break;
	case CPlayer::PARRY_NORMAL:
	{
		// 位置の取得
		D3DXVECTOR3 pos = GetPosition();

		// プレイヤーの位置の取得
		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(ImpactPos, 10.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// パーティクルの設定処理
		pParticle->SetParticle(15.0f, 240, 50, 5,314);

		// ボスまでの角度を取得
		float fAngle = GetTargetAngle(pos, PlayerPos);

		// 向きの設定
		pPlayer->SetAngle(fAngle + D3DX_PI);

		//// パリィエフェクトの生成
		//CParryEffect::Create(ImpactPos, D3DXVECTOR3(150.0f, 150.0f, 0.0f), D3DXVECTOR3(0.0f, fAngle, 0.0f), 5, 3, 4, false, CParryEffect::TYPE_ROUND_KICK);

		//// パリィエフェクトの生成
		//CParryEffect::Create(ImpactPos, D3DXVECTOR3(150.0f, 150.0f, 0.0f), D3DXVECTOR3(0.0f, fAngle, 0.0f), 5, 2, 6, false, CParryEffect::TYPE_SPARK);

		// インパクトを生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.6f, 0.8f), ImpactPos, 0.0f, 50.0f);

		// サークルの設定処理
		pCircle->SetCircle(35.0f, 15.0f, 60, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		// 状態の設定
		ChangeState(make_shared<CEnemyDamageS>(5));
	}
		break;
	case CPlayer::PARRY_PARFECT:
	{
		// スローモーションの取得
		CSlow* pSlow = CManager::GetSlow();

		// 位置の取得
		D3DXVECTOR3 pos = GetPosition();

		// プレイヤーの位置の取得
		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(ImpactPos, 10.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// パーティクルの設定処理
		pParticle->SetParticle(15.0f, 60, 50, 5,314);

		// ボスまでの角度を取得
		float fAngle = GetTargetAngle(pos, PlayerPos);

		// 向きの設定
		pPlayer->SetAngle(fAngle + D3DX_PI);

		//// パリィエフェクトの生成
		//CParryEffect::Create(ImpactPos, D3DXVECTOR3(150.0f, 150.0f, 0.0f), D3DXVECTOR3(0.0f, fAngle, 0.0f), 5, 3, 7, false, CParryEffect::TYPE_ROUND_KICK);

		//// パリィエフェクトの生成
		//CParryEffect::Create(ImpactPos, D3DXVECTOR3(150.0f, 150.0f, 0.0f), D3DXVECTOR3(0.0f, fAngle, 0.0f), 5, 2, 8, false, CParryEffect::TYPE_SPARK);

		// インパクトを生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.8f), ImpactPos, 0.0f, 100.0f);

		// サークルの設定処理
		pCircle->SetCircle(35.0f, 10.0f, 120, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		if (pSlow != nullptr)
		{
			// スローモーション
			pSlow->Start(60, 12);
		}

		// 状態の設定
		ChangeState(make_shared<CEnemyDamageL>(10));
	}
		break;
	default:
		break;
	}
}

//===================================================
// ダメージモーションかどうか
//===================================================
bool CEnemy::IsDamageMotion(void)
{
	// モーションの取得
	auto pMotion = CCharacter3D::GetMotion();

	// モーションがなかったら処理しない
	if (pMotion == nullptr) return false;

	// モーションの種類の取得
	int motionType = pMotion->GetBlendType();

	// ダメージモーションだったら
	if (motionType == MOTIONTYPE_DAMAGEL ||
		motionType == MOTIONTYPE_DAMAGES)
	{
		return true;
	}

	return false;
}

//===================================================
// 武器との当たり判定
//===================================================
bool CEnemy::CollisionWepon(void)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 武器の先の座標
	D3DXVECTOR3 WeponTop = GetPositionFromMatrix(m_weponMatrix);

	// 武器の根元の座標
	D3DXVECTOR3 WeponBottom = CCharacter3D::GetModelPos(15);

	// 武器の長さを求める
	D3DXVECTOR3 diff = WeponTop - WeponBottom;

	// 武器のマトリックス分回す
	for (int nCnt = 0; nCnt < NUM_MATRIX; nCnt++)
	{
		// 相対値
		float fRate = nCnt / (float)NUM_MATRIX;

		// 武器の根元(基準)から先まで点を打つ
		D3DXVECTOR3 pos = WeponBottom + diff * fRate;

		// 円の判定
		if (m_pSphere != nullptr)
		{
			// 位置の更新
			m_pSphere->SetPosition(pos);
		}

#ifdef _DEBUG

		//// 武器のマトリックス確認用
		//CEffect3D::Create(pos, 50.0f, VEC3_NULL, WHITE, 10);
#endif // _DEBUG

		// 円の判定の取得
		CCollisionSphere* pCollision = CCollisionSphere::GetInstance();

		// 敵の武器に当たったら
		if (pCollision != nullptr)
		{
			CColliderSphere* playersphere = pPlayer->GetSphereCollider();

			if (pCollision->Collision(m_pSphere.get(), playersphere))
			{
				return true;
			}
		}
	}

	return false;
}

//===================================================
// プレイヤーを追いかける処理
//===================================================
void CEnemy::ChasePlayer(float chaseScal, const float speedScal)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();
	
	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 現在の移動方向を求める
	float fRotMove = atan2f(move.x, move.z);

	// 目的までの差分を求める
	D3DXVECTOR3 Diff = playerPos - pos;

	// 目的の方向を求める
	float fDest = atan2f(Diff.x, Diff.z);

	// 目的までの差分を求める
	float fDiffRot = fDest - fRotMove;

	// 角度の差分を求める
	if (fDiffRot < -D3DX_PI)
	{
		fDiffRot += D3DX_PI * 2.0f;
	}
	else if (fDiffRot > D3DX_PI)
	{
		fDiffRot += -D3DX_PI * 2.0f;
	}

	// 範囲制限
	chaseScal = Clamp(chaseScal, 0.0f, 1.0f);

	// 目的までだんだん近づける
	fRotMove += fDiffRot * chaseScal;

	// 速さの取得
	float fSpeed = CCharacter3D::GetSpeed();

	move.x = sinf(fRotMove) * (fSpeed * speedScal);
	move.z = cosf(fRotMove) * (fSpeed * speedScal);

	D3DXVECTOR3 TargetDir = GetVector(playerPos, pos);	  // ターゲットまでの方向を取得
	float fAngle = GetTargetAngle(playerPos, pos);		  // 目標までの角度の取得

	// 移動量の設定
	m_pMove->Set(move);

	// 目的の角度の設定
	CCharacter3D::GetRotaition()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));
}

//===================================================
// プレイヤーの方向を見る処理
//===================================================
void CEnemy::AngleToPlayer(void)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// プレイヤーの位置
	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// 自分の位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	// プレイヤーまでの角度を求める
	float fAngle = GetTargetAngle(pos, PlayerPos);

	// 向きの設定
	CCharacter3D::GetRotaition()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
}

//===================================================
// 距離の判定
//===================================================
bool CEnemy::CheckDistane(const float fRadius)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerpos = pPlayer->GetPosition();

	// 敵の位置の取得
	D3DXVECTOR3 enemypos = CCharacter3D::GetPosition();
	
	// 距離の取得
	float distance = GetDistance(playerpos - enemypos);

	// 一定の範囲に入ったら
	if (distance <= fRadius)
	{
		return true;
	}
	return false;
}

//===================================================
// 軌跡の処理
//===================================================
void CEnemy::Orbit(const int nSegH, const D3DXCOLOR col)
{
	// 武器の先端の位置
	D3DXVECTOR3 WeponTop = GetPositionFromMatrix(m_weponMatrix);

	D3DXVECTOR3 WeponBottom = CCharacter3D::GetModelPos(15);

	// 軌跡の生成
	if (m_pOrbit == nullptr)
	{
		m_pOrbit = CMeshOrbit::Create(WeponTop, WeponBottom, nSegH, col);
	}
	
	// 軌跡が使われていて寿命が無かったら
	if (m_pOrbit != nullptr)
	{
		m_pOrbit->SetPosition(WeponTop, WeponBottom);
	}
}

//===================================================
// 軌跡の消去
//===================================================
void CEnemy::DeleteOrbit(void)
{
	if (m_pOrbit != nullptr)
	{
		m_pOrbit->Uninit();
		m_pOrbit = nullptr;
	}
}

//===================================================
// 状態の変更
//===================================================
void CEnemy::ChangeState(std::shared_ptr<CEnemyState> pNewState)
{
	// オーナの設定
	pNewState->SetOwner(this);

	// 状態の変更
	m_pMachine->Change(pNewState);
}

//===================================================
// 障害物との当たり判定
//===================================================
bool CEnemy::CollisionObstacle(D3DXVECTOR3 *pPos)
{
	// モーションの取得
	auto pMotion = CCharacter3D::GetMotion();

	// モーションがないなら処理しない
	if (pMotion == nullptr) return false;

	//	障害物マネージャーのインスタンスの取得
	CObstacleManager* pObstacleManager = CObstacleManager::GetInstance();

	// マネージャーが無かったら
	if (pObstacleManager == nullptr) return false;

	// 障害物の総数分調べる
	for (auto itr = pObstacleManager->Begin(); itr != pObstacleManager->End();)
	{
		// コライダーの更新
		UpdateCollider(*pPos);

		// 障害物と武器の判定
		if (CollisionObstacleToWepon((*itr)))
		{
			// 爆発のエフェクト
			if (SetTNTEffect((*itr)))
			{
				// 障害物の位置の取得
				D3DXVECTOR3 obstaclePos = (*itr)->GetPosition();

				// 爆発のエリアの生成
				CExplosionArea::Create(obstaclePos);

				(*itr)->Uninit();
				(*itr) = nullptr;
				itr = pObstacleManager->Erase(itr);

				// 処理を飛ばす
				continue;
			}
		}

		// 当たり判定
		const bool bCollision = (*itr) != nullptr && (*itr)->Collision(m_pAABB.get(), pPos);

		// 当たっていたら
		if (!bCollision)
		{
			++itr;
			continue;
		}

		// 種類の取得
		CObstacle::TYPE type = (*itr)->GetType();

		// 障害物の位置の取得
		D3DXVECTOR3 obstaclePos = (*itr)->GetPosition();

		// 障害物までの向きの取得
		float fAngle = GetTargetAngle(*pPos, obstaclePos);

		// 向きの設定
		CCharacter3D::GetRotaition()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
		
		if (type != CObstacle::TYPE_TNT_BARREL)
		{
			// 死亡状態じゃないなら
			if (pMotion->GetBlendType() != MOTIONTYPE_DEATH)
			{
				// ダメージ状態にする
				ChangeState(make_shared<CEnemyDamageL>(10, true));
			}
		}

		++itr;

		return true;
	}

	return false;
}

//===================================================
// ヒットストップの設定処理
//===================================================
void CEnemy::SetHitStop(const int nTime)
{
	// ヒットストップの設定
	CCharacter3D::SetHitStop(nTime);
}

//===================================================
// 瓦礫の設定処理
//===================================================
void CEnemy::SetRubble(void)
{
	// 武器の先端の位置
	D3DXVECTOR3 WeponPos = GetPositionFromMatrix(m_weponMatrix);

	// フィールドの波の設定
	CMeshFieldWave::Config config = { WeponPos,250.0f,380.0f,280.0f,12.0f,0.01f,120 };

	// メッシュフィールドの取得処理
	CMeshField* pMeshField = CGame::GetField();

	if (pMeshField != nullptr)
	{
		// 地面に波を発生させる
		pMeshField->SetWave(config);
	}

	// メッシュサークルの生成
	auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 0.5f, 0.5f, 1.0f), WeponPos, 0.0f, 35.0f);

	// サークルの設定処理
	pCircle->SetCircle(0.0f, 50.0f, 60, true);

	// 瓦礫を出す
	CRubbleManager::SetImpact(WeponPos, 120, NUM_RUBBLE, D3DXVECTOR2(15.0f, 35.0f));
}

//===================================================
// ヒット時の処理
//===================================================
void CEnemy::Hit(const int nDamage)
{
	// ダメージ状態じゃないなら
	if (IsDamageMotion() == false)
	{
		CCharacter3D::Hit(nDamage);
	}
}

//===================================================
// 移動時の煙
//===================================================
void CEnemy::MoveSmoke(void)
{
	//// モーションの取得
	//auto pMotion = CCharacter3D::GetMotion();

	//// モーションがないなら処理しない
	//if (pMotion == nullptr) return;

	//// 向きの取得
	//D3DXVECTOR3 rot = CCharacter3D::GetRotaition()->Get();

	//// プレイヤーの後ろ方向を設定
	//float fMoveX = sinf(rot.y) * 2.0f;
	//float fMoveZ = cosf(rot.y) * 2.0f;

	//if (pMotion != nullptr)
	//{
	//	// 17フレーム目になったら
	//	if (pMotion->IsEventFrame(17, 17, MOTIONTYPE_MOVE))
	//	{
	//		// 位置の取得
	//		D3DXVECTOR3 pos = GetModelPos(11);

	//		// エフェクトの生成
	//		auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x, pos.y, pos.z), 100.0f, WHITE);

	//		// エフェクトの設定処理
	//		pEffect->SetEffect(60, D3DXVECTOR3(fMoveX, 0.0f, fMoveZ));
	//	}
	//	// 40フレーム目になったら
	//	if (pMotion->IsEventFrame(40, 40, MOTIONTYPE_MOVE))
	//	{
	//		// 位置の取得
	//		D3DXVECTOR3 pos = GetModelPos(14);

	//		// エフェクトの生成
	//		auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x, pos.y, pos.z), 100.0f, WHITE);

	//		// エフェクトの設定処理
	//		pEffect->SetEffect(60, D3DXVECTOR3(fMoveX, 0.0f, fMoveZ));
	//	}
	//}
}

//===================================================
// 向きの設定
//===================================================
void CEnemy::SetAngle(const float fAngle)
{
	CCharacter3D::GetRotaition()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
}

//===================================================
// 視界の判定
//===================================================
bool CEnemy::CollisionFOV(const D3DXVECTOR3 pos)
{
	// 視界判定の取得
	auto pCollision = CCollisionFOV::GetInstance();

	// 視界内だったら
	if (pCollision->Collision(pos, m_pFOV.get()))
	{
		return true;
	}

	return false;
}

//===================================================
// 視界の判定
//===================================================
bool CEnemy::CollisionFOV(const D3DXVECTOR3 Targetpos, const float fLeftAngle, const float fRightAngle)
{
	// 視界判定の取得
	auto pCollision = CCollisionFOV::GetInstance();

	// 位置の取得
	D3DXVECTOR3 myPos = CCharacter3D::GetPosition();

	// 向きの取得
	D3DXVECTOR3 Angle = CCharacter3D::GetRotaition()->Get();

	// 視界の作成
	auto ColliderFOV = CColliderFOV::CreateCollider(myPos, Angle.y, fLeftAngle, fRightAngle,1000.0f);
	
	// 視界内だったら
	if (pCollision->Collision(Targetpos, &ColliderFOV))
	{
		return true;
	}

	return false;
}

//===================================================
// 突進のエフェクト
//===================================================
void CEnemy::RushEffect(void)
{
	// 向きの取得
	D3DXVECTOR3 rot = CCharacter3D::GetRotaition()->Get();
	
	//// モデルの位置を取得
	//D3DXVECTOR3 modelpos = GetPositionFromMatrix(m_RushEffectMtx);

	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();
	D3DXVECTOR3 HeadPos = CCharacter3D::GetModelPos(MODEL_HEAD);

	D3DXVECTOR3 CirclePos;

	CirclePos.x = pos.x + sinf(rot.y) * RUSH_EFFECT_POS;
	CirclePos.y = HeadPos.y;
	CirclePos.z = pos.z + cosf(rot.y) * RUSH_EFFECT_POS;

	// サークルの生成
	auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f,1.0f,1.0f,0.5f), CirclePos, 0.0f, 50.0f);
	pCircle->SetCircle(-100.0f, 10.0f, 60, false, D3DXVECTOR3(D3DX_PI * 0.5f, rot.y, 0.0f));
}

//===================================================
// 武器攻撃の結果を返す
//===================================================
CEnemy::RESULT CEnemy::WeponAttackResult(CPlayer* pPlayer)
{
	// 位置の取得
	D3DXVECTOR3 pos = GetPosition();

	// プレイヤーのモーションの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// プレイヤーのモーションの取得
	int playerMotionType = pPlayerMotion->GetBlendType();

	// 武器が当たったら
	if (CollisionWepon())
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

//===================================================
// 攻撃の結果を返す
//===================================================
CEnemy::RESULT CEnemy::AttackResult(CPlayer* pPlayer, const MODEL model,const float fRadius)
{
	// カプセルの当たり判定の取得
	auto pCollisionCapsule = CCollisionCapsule::GetInstance();

	// 取得できなかったら処理しない
	if (pCollisionCapsule == nullptr) return RESULT_NONE;
	
	D3DXVECTOR3 pos = VEC3_NULL;

	if (model != MODEL_NONE)
	{
		// モデルの位置の取得
		pos = CCharacter3D::GetModelPos(model);
	}
	else
	{
		// 位置の取得
		pos = CCharacter3D::GetPosition();
	}

	// 円のコライダーの生成
	auto SphereCollider = CColliderSphere::CreateCollider(pos, fRadius);

	// プレイヤーのコライダーの取得
	auto pPlayerCapsule = pPlayer->GetCapsuleCollider();

	// 円とカプセルの判定
	if (pCollisionCapsule->CollisionSphere(pPlayerCapsule, &SphereCollider))
	{
		// プレイヤーのモーションの取得
		CMotion* pPlayerMotion = pPlayer->GetMotion();

		// プレイヤーのモーションの取得
		int playerMotionType = pPlayerMotion->GetBlendType();

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

//===================================================
// オブザーバーへの通知処理
//===================================================
void CEnemy::Notify(void)
{
	if (m_pObserver != nullptr)
	{
		// HPの取得
		int nLife = CCharacter3D::GetLife();

		// HPの変化を通知する
		m_pObserver->OnNotify(nLife);
	}
}

//===================================================
// コライダーの更新処理
//===================================================
void CEnemy::UpdateCollider(const D3DXVECTOR3 pos)
{
	// 大きさの取得
	D3DXVECTOR3 Size = CCharacter3D::GetSize();

	if (m_pAABB != nullptr)
	{
		// 中心を求める
		D3DXVECTOR3 CenterPos = VEC3_NULL;

		// 中心座標を設定
		CenterPos.x = pos.x;
		CenterPos.y = pos.y + Size.y * 0.5f;
		CenterPos.z = pos.z;

		// データの更新処理
		m_pAABB->UpdateData(CenterPos, D3DXVECTOR3(m_posOld.x, m_posOld.y + Size.y * 0.5f, m_posOld.z));
	}

	// カプセルの更新
	if (m_pCapsule != nullptr)
	{
		// データの取得
		auto dataCapsule = m_pCapsule->GetData();

		dataCapsule.EndPos = D3DXVECTOR3(pos.x,pos.y + Size.y,pos.z);

		// データの更新
		dataCapsule.StartPos = pos;

		// データの更新処理
		m_pCapsule->UpdateData(dataCapsule);
	}
	
	// 視界の更新
	if (m_pFOV != nullptr)
	{
		// 位置の設定
		m_pFOV->SetPosition(pos);

		// 向きの取得
		D3DXVECTOR3 Angle = CCharacter3D::GetRotaition()->Get();

		// データの更新処理
		m_pFOV->UpdateData(Angle.y);
	}

	SetPosition(pos);
}

//===================================================
// 敵の設定
//===================================================
void CEnemy::Config(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size)
{
	// 初期化処理
	CCharacter3D::Init();

	// モーションロード処理
	CCharacter3D::LoadMotion("motionEnemy000.txt", MOTIONTYPE_MAX);

	// キャラクターの設定処理
	CCharacter3D::SetCharacter(nLife, fSpeed, ShadowScal, Size);
}

//===================================================
// 武器と障害物の当たり判定
//===================================================
bool CEnemy::CollisionObstacleToWepon(CObstacle *pObstacle)
{
	// 当たり判定の取得
	auto pCollision = CCollisionCapsule::GetInstance();

	// 使われていないなら処理しない
	if (pObstacle == nullptr) return false;

	// ワールドマトリックスの設定がされていなかったら処理しない
	if (!m_bSetMatrix) return false;

	// 爆発樽じゃないなら処理しない
	if (pObstacle->GetType() != CObstacle::TYPE_TNT_BARREL) return false;

	// 障害物の位置
	D3DXVECTOR3 obstaclePos = pObstacle->GetPosition();
	D3DXVECTOR3 obstacleSize = pObstacle->GetSize();
	D3DXVECTOR3 obstacleTopPos = obstaclePos;
	obstacleTopPos.y = obstaclePos.y + obstacleSize.y;

	// 位置の取得
	D3DXVECTOR3 sword_buttom = CCharacter3D::GetModelPos(MODEL_WEPON);
	D3DXVECTOR3 sword_Top = GetPositionFromMatrix(m_weponMatrix);

	// コライダーの作成
	auto Weponcapsule = CColliderCapsule::CreateCollider(sword_buttom, sword_Top, 140.0f);

	// 半径
	float fRadius = 100.0f;

	// カプセルの生成
	auto capsule = CColliderCapsule::CreateCollider(obstaclePos, obstacleTopPos, fRadius);

	if (pCollision->Collision(m_pCapsule.get(),&capsule) || pCollision->Collision(&Weponcapsule, &capsule))
	{
		return true;
	}
	return false;
}

//===================================================
// 爆発の演出の処理
//===================================================
bool CEnemy::SetTNTEffect(CObstacle* pObstacle)
{
	// nullだったら処理しない
	if (pObstacle == nullptr) return false;

	// 現在の状態の取得
	auto STATE = CCharacter3D::GetState();

	// 状態が攻撃じゃないなら処理しない
	if (STATE != STATE_ACTION)
	{
		//// 状態の変更
		//ChangeState(make_shared<CEnemySwing>());

		return false;
	}

	// 障害物の位置の取得
	D3DXVECTOR3 obstaclePos = pObstacle->GetPosition();
	D3DXVECTOR3 Size = pObstacle->GetSize();
	D3DXVECTOR3 CenterPos = { obstaclePos.x,obstaclePos.y + Size.y * 0.5f,obstaclePos.z };
	D3DXVECTOR3 TopPos = { obstaclePos.x,obstaclePos.y + Size.y,obstaclePos.z };

	// 爆発の生成
	auto pExplotion = CExplosionManager::SetParam(CenterPos, D3DXVECTOR2(100.0f, 100.0f), D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.8f), 4, 3, 10);

	if (pExplotion != nullptr)
	{
		// 爆発の生成
		pExplotion->Create(CExplosion::TYPE_SMOKE, D3DXVECTOR2(25.0f, 1.0f), 16);
	}
	else
	{
		return false;
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 爆発
		pSound->PlaySoundA(CSound::SOUND_LABEL_EXPLOSION);
	}

	// 瓦礫の生成
	CRubbleManager::SetExplosionTNT(CenterPos, 120, 16, D3DXVECTOR2(15.0f, 15.0f));

	// 爆発の生成
	CExplosion::Create(TopPos, D3DXVECTOR2(500.0f, 500.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 8, 8, 3, CExplosion::TYPE_EXPLOSION);
	CExplosion::Create(TopPos, D3DXVECTOR2(100.0f, 100.0f), D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f), 5, 2, 5, CExplosion::TYPE_FIRE);

	return true;
}

//===================================================
// 障害物との距離を調べる
//===================================================
bool CEnemy::CheckObstacleDistance(const float fRange)
{
	//	障害物マネージャーのインスタンスの取得
	CObstacleManager* pObstacleManager = CObstacleManager::GetInstance();

	// マネージャーが無かったら
	if (pObstacleManager == nullptr) return false;

	// 障害物の総数分調べる
	for (auto itr = pObstacleManager->Begin(); itr != pObstacleManager->End(); ++itr)
	{
		// 取得できなかったら処理しない
		if ((*itr) == nullptr) continue;

		// 位置の取得
		D3DXVECTOR3 pos = CCharacter3D::GetPosition();
		D3DXVECTOR3 obstaclePos = (*itr)->GetPosition();

		// 障害物との距離を取得
		float fDistance = GetDistance(obstaclePos - pos);

		// 距離が範囲以下だったら
		if (fDistance <= fRange)
		{
			return true;
		}
	}

	return false;
}

//===================================================
// プレイヤーとの当たり判定
//===================================================
void CEnemy::CollisionPlayer(CMotion *pPlayerMotion,CPlayer *pPlayer)
{
	// 胸の位置の取得
	D3DXVECTOR3 chestpos = GetModelPos(2);

	// プレイヤーの位置の取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// 反撃の成功度の取得
	int nParrySuccess = pPlayer->SuccessParry();

	// パリィモーションの蹴りになったら
	if (pPlayerMotion->IsEventFrame(38, 38, pPlayer->MOTIONTYPE_ROUNDKICK) && IsDamageMotion() == false)
	{
		// プレイヤーの右足の位置
		D3DXVECTOR3 playerFootR = pPlayer->GetModelPos(11);

		// 円の当たり判定の取得
		CCollisionSphere* pSphere = CCollisionSphere::GetInstance();

		// 右手の円
		CColliderSphere FootRSphere = CColliderSphere::CreateCollider(playerFootR, 50.0f);
		CColliderSphere ChestSphere = CColliderSphere::CreateCollider(chestpos, 250.0f);

		// 手が当たったら
		if (pSphere != nullptr && pSphere->Collision(&ChestSphere, &FootRSphere))
		{
			// どの攻撃モーションがでるか判定
			SelectDamageMotion(nParrySuccess, playerFootR);
		}
	}

	// パリィモーションの蹴りになったら
	if (pPlayerMotion->IsEventFrame(13, 13, pPlayer->MOTIONTYPE_PUNCH) && IsDamageMotion() == false)
	{
		// プレイヤーの右手の位置
		D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(5);

		// 円の当たり判定の取得
		CCollisionSphere* pSphere = CCollisionSphere::GetInstance();

		// 右手の円
		CColliderSphere HandRSphere = CColliderSphere::CreateCollider(playerHandR, 80.0f);
		CColliderSphere ChestSphere = CColliderSphere::CreateCollider(chestpos, 250.0f);

		// 手が当たったら
		if (pSphere != nullptr && pSphere->Collision(&ChestSphere, &HandRSphere))
		{
			// どの攻撃モーションがでるか判定
			SelectDamageMotion(nParrySuccess, playerHandR);
		}
	}
}

//===================================================
// 親子関係の設定処理
//===================================================
void CEnemy::SetParent(const int nCnt, const D3DXVECTOR3 offPos, D3DXMATRIX* pMatrixOut)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(pMatrixOut);

	// 親の位置、向きの設定
	D3DXVECTOR3 ParentPos = CCharacter3D::GetModelPos(nCnt);
	D3DXVECTOR3 ParentRot = CCharacter3D::GetModelRot(nCnt);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, ParentRot.y, ParentRot.x, ParentRot.z);
	D3DXMatrixMultiply(pMatrixOut, pMatrixOut, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, offPos.x, offPos.y, offPos.z);
	D3DXMatrixMultiply(pMatrixOut, pMatrixOut, &mtxTrans);

	// 親のマトリックスの取得
	mtxParent = CCharacter3D::GetParent(nCnt);

	// 親のワールドマトリックスと掛け合わせる
	D3DXMatrixMultiply(pMatrixOut, pMatrixOut, &mtxParent);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, pMatrixOut);
}

//===================================================
// コンストラクタ
//===================================================
CEnemyMovement::CEnemyMovement()
{
	m_pEnemy = nullptr;
	m_pMove = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CEnemyMovement::~CEnemyMovement()
{
}

//===================================================
// 初期化処理
//===================================================
void CEnemyMovement::Init(std::shared_ptr<CVelocity> enemyMove, CEnemy* pEnemy)
{
	// nullなら
	if (m_pMove == nullptr)
	{
		// 敵の移量を参照
		m_pMove = enemyMove;
	}

	// nullなら
	if (m_pEnemy == nullptr)
	{
		m_pEnemy = pEnemy;
	}
}

//===================================================
// 吹き飛び処理
//===================================================
void CEnemyMovement::BlowOff(const D3DXVECTOR3 attacker,const float blowOff, const float jump)
{
	// 位置の取得
	D3DXVECTOR3 pos = m_pEnemy->GetPosition();

	// アタッカーからプレイヤーまでの差分を求める
	D3DXVECTOR3 diff = pos - attacker;

	// 角度を求める
	float fAngle = atan2f(diff.x, diff.z);

	// 移動量
	D3DXVECTOR3 move;

	// 向きの設定
	m_pEnemy->GetRotaition()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

	// 移動量の設定
	move.x = sinf(fAngle) * blowOff;
	move.y = jump;
	move.z = cosf(fAngle) * blowOff;

	m_pMove->Set(move);
}

//===================================================
// 向いている方向に進む処理
//===================================================
void CEnemyMovement::MoveForWard(const float fSpeed)
{
	// 向きの取得
	D3DXVECTOR3 rot = m_pEnemy->GetRotaition()->Get();

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 移動量の設定
	move.x = sinf(rot.y + D3DX_PI) * fSpeed;
	move.z = cosf(rot.y + D3DX_PI) * fSpeed;

	// 移動量の設定
	m_pMove->Set(move);
}

//===================================================
// 移動方向の設定処理
//===================================================
void CEnemyMovement::SetMoveDir(const float dir,const float fSpeed)
{
	// 向きの取得
	D3DXVECTOR3 rot = m_pEnemy->GetRotaition()->Get();

	// 移動量の取得
	D3DXVECTOR3 moveWk = m_pMove->Get();

	// 移動量の設定
	moveWk.x = sinf(rot.y + dir) * fSpeed;
	moveWk.z = cosf(rot.y + dir) * fSpeed;

	// 移動量の設定
	m_pMove->Set(moveWk);
}

//===================================================
// ジャンプ
//===================================================
void CEnemyMovement::Jump(const float fHeight)
{
	if (m_pMove != nullptr)
	{
		m_pMove->Jump(fHeight);
	}
}
