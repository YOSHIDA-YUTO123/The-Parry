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
#include "Collision.h"
#include "EnemyStateAttack.h"
#include "EnemyStateMovement.h"
#include "EnemyMovement.h"

//***************************************************
// 定数定義
//***************************************************
constexpr float SHADOW_ALEVEL = 0.7f;			// 影のアルファ値
constexpr float SHADOW_MAX_HEIGHT = 500.0f;		// 影が見える最大の高さ
constexpr float SHADOW_SIZE = 150.0f;			// 影の大きさ
constexpr float ROCKON_HEIGHT = 100.0f;			// ロックオン時の見る場所
constexpr float RUSH_EFFECT_POS = -350.0f;		// 突進エフェクトの位置
constexpr int NUM_RUBBLE = 16;					// 瓦礫を出す数
constexpr int NUM_MATRIX = 8;					// 武器につけるマトリックスの数
constexpr int NEXT_ACTION_TIME = 300;			// 次の行動の抽選までの時間
constexpr int MODEL_WEPON = 15;					// 武器のインデックス
constexpr int WEAK_DAMAGE = 1;					// セーフ時のダメージ
constexpr int NORMAL_DAMAGE = 5;				// 普通時のダメージ
constexpr int PARFECT_DAMAGE = 10;				// パーフェクト時のダメージ
constexpr int GUARD_WEAK_DAMAGE = 1;			// セーフ時のダメージ(ガード時)
constexpr int GUARD_NORMAL_DAMAGE = 3;			// 普通時のダメージ(ガード時)
constexpr int GUARD_PARFECT_DAMAGE = 5;		// パーフェクト時のダメージ(ガード時)

using namespace Const;							// 名前空間Constを使用する
using namespace math;							// 名前空間mathを使用する
using namespace std;							// 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CEnemy::CEnemy() : CCharacter3D(TYPE_ENEMY)
{
	m_bSetMatrix = false;
	m_pMachine = nullptr;
	D3DXMatrixIdentity(&m_RushEffectMtx);
	D3DXMatrixIdentity(&m_weponMatrix);
	m_pOrbit = nullptr;
	m_posOld = VEC3_NULL;
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

	pEnemy->SetRotation(rot);

	// 状態マネージャーの生成
	pEnemy->m_pStateManager.reset(CEnemyStateManager::Create());
	pEnemy->m_pStateManager->SetOnwer(pEnemy);
	pEnemy->m_pStateManager->SetMaxLife(nLife);

	return pEnemy;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CEnemy::Init(void)
{
	// 状態マシーンの処理
	m_pMachine = make_unique<CStateMachine>();

	// 状態の変更処理
	ChangeState(make_shared<CEnemyIdle>(120));

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

	m_pMovement->Init();

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
	//float fHeightV = motiontype == MOTIONTYPE_JUMPATTACK ? 150.0f : 160.0f;
	 
	// 胸の位置の取得
	D3DXVECTOR3 chestpos = GetModelPos(CEnemy::MODEL_CHEST);
	
	// 頭の位置の取得
	D3DXVECTOR3 Headpos = GetModelPos(CEnemy::MODEL_HEAD);

	// カメラがnullじゃないなら
	if (pCamera != nullptr)
	{
		// ロックオン処理
		pCamera->Rockon(PlayerPos, Headpos, fDistance);
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
	if (pKeyboard->GetTrigger(DIK_F5))
	{
		Hit(50);
	}
	if (pKeyboard->GetTrigger(DIK_7))
	{
		ChangeState(make_shared<CEnemyRush>());
	}
	if (pKeyboard->GetTrigger(DIK_8))
	{
		ChangeState(make_shared<CEnemyLookBackL>());
	}
	if (pKeyboard->GetTrigger(DIK_9))
	{
		ChangeState(make_shared<CEnemyRushSwing>());
	}

	if (pKeyboard->GetTrigger(DIK_F1))
	{
		// 影の消去
		CCharacter3D::DeleteShadow();
		Uninit();
		return;
	}

	if (pKeyboard->GetTrigger(DIK_T))
	{
		pCamera->SetShake(120, 20);
		//pCamera->SetState(pCamera->STATE_SHAKE);
	}

#endif // _DEBUG

	// モーションの制御クラスの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// 位置の更新処理
	m_pMovement->UpdatePosition(&pos, &m_posOld);

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

	// コライダーの更新
	UpdateCollider(pos);

	// シリンダーの取得
	CMeshCylinder* pCylinder = CGame::GetCylinder();

	// シリンダーの当たり判定
	if (pCylinder != nullptr)
	{
		pCylinder->Collision(&pos);
	}
	
	// 障害物との当たり判定
	if (CollisionObstacle(&pos))
	{

	}

	// 鳥のマネージャの取得
	auto pBirdManager = CBirdManager::GetInstance();

	if (pBirdManager != nullptr)
	{
		// 鳥との距離の判定
		pBirdManager->CheckDistance(pos, 500.0f);
	}

	// 重力の処理
	m_pMovement->Gravity(-MAX_GRABITY);

	// インパクトの当たり判定
	CollisionImpact(pMotion, pMesh, &pos);

	// プレイヤーとの当たり判定
	CollisionPlayer(pPlayerMotion, pPlayer);

	// 武器の先端の位置
	D3DXVECTOR3 WeponPos = GetPositionFromMatrix(m_weponMatrix);
	D3DXVECTOR3 WeponBottom = GetModelPos(MODEL_WEPON);

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// 衝撃波の生成
	if (pMotion->IsEventFrame(102, 102, MOTIONTYPE_IMPACT))
	{
		// プレイヤーまでの方向
		D3DXVECTOR3 dir = PlayerPos - WeponPos;

		// インパクトの設定
		CMeshFieldImpact::Config config = { WeponPos,chestpos,dir,D3DXCOLOR(1.0f,0.5f,0.5f,1.0f),CMeshFieldImpact::OBJ_ENEMY,150.0f,750.0f,26.0f,60 };

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_IMPACT001);
		}
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

	if (m_pStateManager != nullptr)
	{
		m_pStateManager->SetLowEvent();
	}

	// HPが半分以下だったら
	if (m_pStateManager != nullptr && m_pStateManager->CheckLowHp(2))
	{
		// パーティクルの生成
		auto pNormal = CParticle3DNormal::Create(CCharacter3D::GetModelPos(MODEL_CHEST), 20.0f, D3DXCOLOR(1.0f, 0.4f, 1.0f, 1.0f));

		// パーティクルの設定処理
		pNormal->SetParticle(5.0f, 120, 5, 1, 60);
		pNormal->SetParam(CEffect3D::TYPE_NORAML, 200);
	}

	if (CCharacter3D::GetAlive() == false)
	{
		// モーションの種類の取得
		int nMotionType = pMotion->GetBlendType();

		if ((nMotionType != MOTIONTYPE_DEATH && nMotionType != MOTIONTYPE_DOWN) &&
			(nMotionType != MOTIONTYPE_STANP_DEATH && nMotionType != MOTIONTYPE_STANP_DOWN)&&
			nMotionType != MOTIONTYPE_COMBODAMAGE)
		{

			// 状態を追従にする
			pCamera->SetState(CGameCamera::STATE_TRACKING);

			// 敵を追従する
			pCamera->SetTracking(CGameCamera::TRACKOBJ_ENEMY);

			// 敵死亡
			CGame::SetState(CGame::STATE_ENEMY_DEATH);

			if (nMotionType == MOTIONTYPE_STANP_DAMAGE)
			{
				// 死亡状態の設定
				auto pDeath = make_shared<CEnemyDeath>();

				// 種類の設定
				pDeath->SetType(CEnemyDeath::TYPE_STANP);

				// HPが無かったら
				ChangeState(pDeath);
			}
			else
			{
				// 死亡状態の設定
				auto pDeath = make_shared<CEnemyDeath>();

				// 種類の設定
				pDeath->SetType(CEnemyDeath::TYPE_NORMAL);

				// HPが無かったら
				ChangeState(pDeath);
			}
		}
	}
	
	// コライダーの更新
	UpdateCollider(pos);

	// 位置の設定処理
	CCharacter3D::SetPosition(pos);

	// キャラクターの更新
	CCharacter3D::Update();
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

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// 30%の確率でガードする
	if (random <= 38 && m_pStateManager->CheckLowHp(2))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_WEAK);
		}

		// ダメージ
		int nDamage = 0;

		// 成功度の遷移
		switch (success)
		{
		case CPlayer::PARRY_MISS:
			break;
		case CPlayer::PARRY_WEAK:
			nDamage = GUARD_WEAK_DAMAGE;
			break;
		case CPlayer::PARRY_NORMAL:
			nDamage = GUARD_NORMAL_DAMAGE;
			break;
		case CPlayer::PARRY_PARFECT:
			nDamage = GUARD_PARFECT_DAMAGE;
			break;
		default:
			break;
		}

		// ガードする
		ChangeState(make_shared<CEnemyGuard>(ImpactPos, nDamage));

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
		ChangeState(make_shared<CEnemyDamageS>(WEAK_DAMAGE));

		// 位置の取得
		D3DXVECTOR3 pos = GetPosition();

		// プレイヤーの位置の取得
		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(ImpactPos, 10.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// パーティクルの設定処理
		pParticle->SetParticle(15.0f, 240, 50, 5,314);

		// ボスまでの角度を取得
		float fAngle = GetTargetAngle(pos, PlayerPos);

		// 向きの設定
		pPlayer->SetAngle(fAngle + D3DX_PI);

		// インパクトを生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.6f, 0.4f), ImpactPos, 0.0f, 50.0f);

		// サークルの設定処理
		pCircle->SetCircle(35.0f, 8.0f, 60, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_WEAK);
		}
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
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.6f, 0.4f), ImpactPos, 0.0f, 50.0f);

		// サークルの設定処理
		pCircle->SetCircle(35.0f, 8.0f, 60, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_NORMAL);
		}

		// 状態の設定
		ChangeState(make_shared<CEnemyDamageS>(NORMAL_DAMAGE));
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

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_PERFECT);
		}

		// 状態の設定
		ChangeState(make_shared<CEnemyDamageL>(PARFECT_DAMAGE));
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
		motionType == MOTIONTYPE_DAMAGES ||
		motionType == MOTIONTYPE_STANP_DAMAGE ||
		motionType == MOTIONTYPE_DEATH ||
		motionType == MOTIONTYPE_STANP_DEATH)
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

		CColliderSphere* playersphere = pPlayer->GetSphereCollider();

		if (CCollisionSphere::Collision(m_pSphere.get(), playersphere))
		{
			return true;
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
	D3DXVECTOR3 move = m_pMovement->Get();

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
	m_pMovement->Set(move);

	// 目的の角度の設定
	CCharacter3D::SetRotDest(fAngle + D3DX_PI);
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
	CCharacter3D::SetRotDest(fAngle);
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
		if (CollisionObstacleToWepon((*itr)) || pObstacleManager->CollisionImpact((*itr)))
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
		CCharacter3D::SetRotDest(fAngle);
		
		if (type != CObstacle::TYPE_TNT_BARREL)
		{
			// 死亡状態じゃないなら
			if (pMotion->GetBlendType() != MOTIONTYPE_DEATH && pMotion->GetBlendType() != MOTIONTYPE_DAMAGEL &&
				pMotion->GetBlendType() != MOTIONTYPE_DOWN && pMotion->GetType() != MOTIONTYPE_STANP_DOWN && pMotion->GetBlendType() != MOTIONTYPE_DOWN)
			{
				// 音の取得
				CSound* pSound = CManager::GetSound();

				// 音の再生
				if (pSound != nullptr) pSound->Play(CSound::SOUND_LABEL_SPIKE);

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

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_IMPACT000);
	}
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
// 向きの設定
//===================================================
void CEnemy::SetAngle(const float fAngle)
{
	CCharacter3D::SetRotDest(fAngle);
}

//===================================================
// 視界の判定
//===================================================
bool CEnemy::CollisionFOV(const D3DXVECTOR3 pos)
{
	// 視界内だったら
	if (CCollisionFOV::Collision(pos, m_pFOV.get()))
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
	// 位置の取得
	D3DXVECTOR3 myPos = CCharacter3D::GetPosition();

	// 向きの取得
	D3DXVECTOR3 Angle = CCharacter3D::GetRotation();

	// 視界の作成
	auto ColliderFOV = CColliderFOV::CreateCollider(myPos, Angle.y, fLeftAngle, fRightAngle,1000.0f);
	
	// 視界内だったら
	if (CCollisionFOV::Collision(Targetpos, &ColliderFOV))
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
	D3DXVECTOR3 rot = CCharacter3D::GetRotation();
	
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

	// 武器が当たって無かったら
	if (!CollisionWepon()) return RESULT_NONE;
	
	if (playerMotionType == pPlayer->MOTIONTYPE_REVENGE)
	{
		// パリィした
		return RESULT_SPREVENGE;
	}
	
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
	
	
	return RESULT_NONE;
}

//===================================================
// 攻撃の結果を返す
//===================================================
CEnemy::RESULT CEnemy::AttackResult(CPlayer* pPlayer, const MODEL model,const float fRadius)
{	
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
	if (CCollisionCapsule::CollisionSphere(pPlayerCapsule, &SphereCollider))
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
	// HPの取得
	int nLife = CCharacter3D::GetLife();

	if (m_pObserver != nullptr)
	{
		// HPの変化を通知する
		m_pObserver->OnNotify(nLife);
	}

	if (m_pStateManager != nullptr)
	{
		// 体力の設定
		m_pStateManager->SetLife(nLife);
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
		D3DXVECTOR3 Angle = CCharacter3D::GetRotation();

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

	if (CCollisionCapsule::Collision(m_pCapsule.get(),&capsule) || CCollisionCapsule::Collision(&Weponcapsule, &capsule))
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
	if (STATE != STATE_ACTION && STATE != STATE_DEATH && !IsDamageMotion())
	{
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
		pSound->Play(CSound::SOUND_LABEL_EXPLOSION);
	}

	// 瓦礫の生成
	CRubbleManager::SetExplosionTNT(CenterPos, 120, 16, D3DXVECTOR2(15.0f, 15.0f));

	// 爆発の生成
	CExplosion::Create(TopPos, D3DXVECTOR2(500.0f, 500.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 8, 8, 3, CExplosion::TYPE_EXPLOSION);
	CExplosion::Create(TopPos, D3DXVECTOR2(100.0f, 100.0f), D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f), 5, 2, 5, CExplosion::TYPE_FIRE);

	return true;
}

//===================================================
// 衝撃波との当たり判定
//===================================================
void CEnemy::CollisionImpact(CMotion* pMotion, CMeshField* pMeshField, D3DXVECTOR3* pPos)
{
	// 最初の位置
	D3DXVECTOR3 firstPos = VEC3_NULL;

	// インパクトとの判定
	const bool bCollision = pMeshField->CollisionImpact(*pPos, 150.0f, CMeshFieldImpact::OBJ_ENEMY,&firstPos);

	// モーションの種類の取得
	int nMotionBlendType = pMotion->GetBlendType();
	int nMotionType = pMotion->GetType();

	// インパクトの当たり判定
	if (bCollision && IsDamageMotion() == false)
	{
		int nRevenge = -1;

		if (nMotionBlendType != MOTIONTYPE_REVENGE_IMPACT && nMotionType != MOTIONTYPE_REVENGE_IMPACT)
		{
			nRevenge = rand() % 100;
		}

		if (nRevenge <= 40 && nRevenge != -1)
		{
			// 発生位置に返す
			D3DXVECTOR3 dir = firstPos - *pPos;

			// 跳ね返す
			pMeshField->ResetImpact(dir, CMeshFieldImpact::OBJ_ENEMY, *pPos, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

			// 状態の遷移
			ChangeState(make_shared<CEnemyRevengeImpact>());
			return;
		}

		// 状態の設定
		ChangeState(make_shared<CEnemyDamageL>(5));
	}
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
	D3DXVECTOR3 chestpos = GetModelPos(CEnemy::MODEL_CHEST);

	// プレイヤーの位置の取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// 反撃の成功度の取得
	int nParrySuccess = pPlayer->SuccessParry();

	// モーションの取得
	CMotion *pMotion = CCharacter3D::GetMotion();

	// モーションの種類の取得
	int nMotionType = pMotion->GetBlendType();

	if (pPlayer != nullptr && nMotionType != CEnemy::MOTIONTYPE_BACK_HIT && nMotionType != CEnemy::MOTIONTYPE_STANP_DAMAGE)
	{
		// プレイヤーとの当たり判定
		pPlayer->CollisionCapsule(m_pCapsule.get());
	}

	// パリィモーションの蹴りになったら
	if (pPlayerMotion->IsEventFrame(38, 38, pPlayer->MOTIONTYPE_ROUNDKICK) && IsDamageMotion() == false)
	{
		// プレイヤーの右足の位置
		D3DXVECTOR3 playerFootR = pPlayer->GetModelPos(11);

		// 右手の円
		CColliderSphere FootRSphere = CColliderSphere::CreateCollider(playerFootR, 50.0f);
		CColliderSphere ChestSphere = CColliderSphere::CreateCollider(chestpos, 250.0f);

		// 手が当たったら
		if (CCollisionSphere::Collision(&ChestSphere, &FootRSphere))
		{
			// どの攻撃モーションがでるか判定
			SelectDamageMotion(nParrySuccess, playerFootR);
		}
	}

	// パリィモーションのパンチになったら
	if (pPlayerMotion->IsEventFrame(13, 13, pPlayer->MOTIONTYPE_PUNCH) && IsDamageMotion() == false)
	{
		// プレイヤーの右手の位置
		D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(5);

		// 右手の円
		CColliderSphere HandRSphere = CColliderSphere::CreateCollider(playerHandR, 80.0f);
		CColliderSphere ChestSphere = CColliderSphere::CreateCollider(chestpos, 250.0f);

		// 手が当たったら
		if (CCollisionSphere::Collision(&ChestSphere, &HandRSphere))
		{
			// どの攻撃モーションがでるか判定
			SelectDamageMotion(nParrySuccess, playerHandR);
		}
	}

	// ジャンプ攻撃のフレームの判定
	if (pPlayerMotion->IsEventFrame(CPlayer::MOTIONTYPE_JUMP_ATTACK,2) && IsDamageMotion() == false)
	{
		// プレイヤーの位置
		D3DXVECTOR3 playerPos = pPlayer->GetPosition();

		// 右手の円
		CColliderSphere Sphere = CColliderSphere::CreateCollider(playerPos, 80.0f);
		CColliderSphere ChestSphere = CColliderSphere::CreateCollider(chestpos, 250.0f);

		// 手が当たったら
		if (CCollisionSphere::Collision(&ChestSphere, &Sphere))
		{
			// 成功度の設定
			pPlayer->SetParryResult(nParrySuccess);

			// エフェクトの生成
			auto pEffect = CParticle3DNormal::Create(PlayerPos, 20.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));
			pEffect->SetParticle(35.0f, 360, 150, 1, 314);
			pEffect->SetParam(CEffect3D::TYPE_NORAML, 0,0.1f);

			// ダメージ状態の生成
			auto pDamageS = make_shared<CEnemyDamageS>(5);

			// 種類の設定
			pDamageS->SetType(CEnemyDamageS::TYPE_STANP);

			// 状態の設定
			ChangeState(pDamageS);

			// インパクトを生成
			auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.6f, 0.4f), PlayerPos, 0.0f, 50.0f);

			// サークルの設定処理
			pCircle->SetCircle(-35.0f, 15.0f, 60, false);
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
