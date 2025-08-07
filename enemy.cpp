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

//***************************************************
// 定数定義
//***************************************************
constexpr float SHADOW_ALEVEL = 0.7f;			// 影のアルファ値
constexpr float SHADOW_MAX_HEIGHT = 500.0f;		// 影が見える最大の高さ
constexpr float SHADOW_SIZE = 150.0f;			// 影の大きさ
constexpr float ROCKON_HEIGHT = 100.0f;			// ロックオン時の見る場所
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
CEnemy::CEnemy()
{
	m_nParrySuccess = NULL;
	m_pMove = nullptr;
	m_pMachine = nullptr;
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
CEnemy* CEnemy::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CEnemy* pEnemy = nullptr;

	// 敵オブジェクトの生成
	pEnemy = new CEnemy;

	if (pEnemy == nullptr) return nullptr;

	pEnemy->SetPosition(pos);
	pEnemy->Init();
	pEnemy->GetRotaition()->Set(rot);

	return pEnemy;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CEnemy::Init(void)
{
	// 初期化処理
	CCharacter3D::Init();

	// モーションロード処理
	LoadMotion("data/MOTION/motionEnemy000.txt", MOTION_MAX);

	m_pMachine = make_unique<CStateMachine>();

	ChangeState(make_shared<CEnemyIdle>(10));

	// 移動クラスの生成
	m_pMove = make_unique<CVelocity>();

	// キャラクターの設定処理
	CCharacter3D::SetCharacter(MAX_LIFE, 12.0f,D3DXVECTOR3(5.0f, 1.0f, 5.0f),D3DXVECTOR3(100.0f, 400.0f, 100.0f));

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

	// 移動制御クラスの生成
	m_pMovement = make_unique<CEnemyMovement>();

	m_pMovement->Init(m_pMove,this);

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

	// 要素のクリア
	m_pCapsule = nullptr;

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

	if (CCharacter3D::HitStop())
	{
		return;
	}

#ifdef _DEBUG

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

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
		Hit(MAX_LIFE - 1);
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
	m_pMove->SetInertia3D(0.25f);

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

		if (pMotion->GetBlendType() == MOTION_JUMP)
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

	// プレイヤーとの当たり判定
	pPlayer->CollisionCapsule(m_pCapsule.get());
	
	// シリンダーの取得
	CMeshCylinder* pCylinder = CGame::GetCylinder();

	// シリンダーの当たり判定
	if (pCylinder != nullptr)
	{
		pCylinder->Collision(&pos);
	}
	
	// 重力の設定
	m_pMove->Gravity(-MAX_GRABITY);

	// インパクトとの判定
	const bool bCollision = pMesh->CollisionImpact(pos, 150.0f,CMeshFieldImpact::OBJ_ENEMY);

	// インパクトの当たり判定
	if (bCollision && IsDamageMotion() == false)
	{			
		// 状態の設定
		ChangeState(make_shared<CEnemyDamageL>(5));

		// モーションの設定
		pMotion->SetMotion(MOTION_DAMAGEL, true, 2);
	}
	
	// プレイヤーとの当たり判定
	CollisionPlayer(pPlayerMotion, pPlayer);

	// 武器の先端の位置
	D3DXVECTOR3 WeponPos = GetPositionFromMatrix(m_weponMatrix);
	D3DXVECTOR3 WeponBottom = GetModelPos(15);

	// 胸の位置の取得
	D3DXVECTOR3 chestpos = GetModelPos(2);

	// 敵の攻撃のカウンターの目安の表示
	if (pMotion->IsEventFrame(50, 50, MOTION_SMASH))
	{
		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestpos, 100.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1,314);
	}
	else if (pMotion->IsEventFrame(50, 50, MOTION_IMPACT))
	{
		// ウェーブの生成
		auto pWave = CMeshWave::Create(pos, 50.0f, 50.0f, WHITE);

		// ウェーブの設定処理
		pWave->SetWave(30,50.0f);
	}
	else if (pMotion->IsEventFrame(20, 20, MOTION::MOTION_SWING))
	{
		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestpos, 100.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1,314);
	}

	// 攻撃モーションのたたきつけになったら
	if (pMotion->IsEventFrame(72,72, MOTION_SMASH))
	{
		// 瓦礫の設定処理
		SetRubble();
	}

	// 衝撃波の生成
	if (pMotion->IsEventFrame(102, 102, MOTION_IMPACT))
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

	if (CCharacter3D::GetAlive() == false && pMotion->GetBlendType() != MOTION_DEATH && pMotion->GetBlendType() != MOTION_DOWN)
	{
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

	// カメラがnullじゃないなら
	if (pCamera != nullptr)
	{
		pCamera->Rockon(PlayerPos, chestpos);
	}
}

//===================================================
// 描画処理
//===================================================
void CEnemy::Draw(void)
{
	// キャラクターの描画
	CCharacter3D::Draw();

	// 親子関係の設定処理
	SetParent(MODEL_WEPON);
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
	if (motionType == MOTION_DAMAGEL ||
		motionType == MOTION_DAMAGES)
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
			CColliderSphere *playersphere = pPlayer->GetSphereCollider();

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

	// 障害物の総数の取得
	int nNumObstacle = pObstacleManager->GetObstacleSize();

	// 障害物の総数分調べる
	for (int nCnt = 0; nCnt < nNumObstacle; nCnt++)
	{
		// 障害物の取得
		CObstacle* pObstacle = pObstacleManager->GetObstacle(nCnt);

		// コライダーの更新
		UpdateCollider(*pPos);

		// 当たっていたら
		if (pObstacle != nullptr && pObstacle->Collision(m_pAABB.get(), pPos))
		{
			// 障害物の位置の取得
			D3DXVECTOR3 obstaclePos = pObstacle->GetPosition();

			// 障害物までの向きの取得
			float fAngle = GetTargetAngle(*pPos, obstaclePos);

			// 向きの設定
			CCharacter3D::GetRotaition()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

			// 死亡状態じゃないなら
			if (pMotion->GetBlendType() != MOTION_DEATH)
			{
				// ダメージ状態にする
				ChangeState(make_shared<CEnemyDamageL>(10, true));
			}

			return true;
		}
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

	// 瓦礫の数分出す
	for (int nCnt = 0; nCnt < NUM_RUBBLE; nCnt++)
	{
		// 分割に応じた方向を求める
		float fAngle = (D3DX_PI * 2.0f) / NUM_RUBBLE * nCnt;

		// 吹っ飛び量を選出
		float dir = rand() % 15 + 5.0f;
		float Jump = rand() % 15 + 25.0f;

		// 方向に応じた吹っ飛び量を計算
		float fMoveX = sinf(fAngle) * dir;
		float fMoveZ = cosf(fAngle) * dir;

		// 寿命を選出
		int nLife = rand() % 120 + 60;

		// 種類を選出
		int nType = rand() % CRubble::TYPE_MAX;

		// 瓦礫を生成
		CRubble::Create(WeponPos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife, nType);
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
// 移動時の煙
//===================================================
void CEnemy::MoveSmoke(void)
{
	// モーションの取得
	auto pMotion = CCharacter3D::GetMotion();

	// モーションがないなら処理しない
	if (pMotion == nullptr) return;

	// 向きの取得
	D3DXVECTOR3 rot = CCharacter3D::GetRotaition()->Get();

	// プレイヤーの後ろ方向を設定
	float fMoveX = sinf(rot.y) * 2.0f;
	float fMoveZ = cosf(rot.y) * 2.0f;

	if (pMotion != nullptr)
	{
		// 17フレーム目になったら
		if (pMotion->IsEventFrame(17, 17, MOTION::MOTION_MOVE))
		{
			// 位置の取得
			D3DXVECTOR3 pos = GetModelPos(11);

			// エフェクトの生成
			auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x, pos.y, pos.z), 100.0f, WHITE);

			// エフェクトの設定処理
			pEffect->SetEffect(60, D3DXVECTOR3(fMoveX, 0.0f, fMoveZ));
		}
		// 40フレーム目になったら
		if (pMotion->IsEventFrame(40, 40, MOTION::MOTION_MOVE))
		{
			// 位置の取得
			D3DXVECTOR3 pos = GetModelPos(14);

			// エフェクトの生成
			auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x, pos.y, pos.z), 100.0f, WHITE);

			// エフェクトの設定処理
			pEffect->SetEffect(60, D3DXVECTOR3(fMoveX, 0.0f, fMoveZ));
		}

	}
}

//===================================================
// 向きの設定
//===================================================
void CEnemy::SetAngle(const float fAngle)
{
	CCharacter3D::GetRotaition()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
}

//===================================================
// 攻撃の結果を返す
//===================================================
CEnemy::RESULT CEnemy::AttackResult(CPlayer* pPlayer)
{
	// 位置の取得
	D3DXVECTOR3 pos = GetPosition();

	// プレイヤーのモーションの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// プレイヤーのモーションの取得
	int playerMotionType = pPlayerMotion->GetBlendType();

	// 武器が当たったら
	if (CollisionWepon() && playerMotionType != pPlayer->TYPE_PARRY)
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
		else if (playerMotionType == pPlayer->TYPE_AVOID)
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

	// 全身の更新
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
	
	SetPosition(pos);
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

	// パリィモーションの蹴りになったら
	if (pPlayerMotion->IsEventFrame(38, 38, pPlayer->TYPE_ROUNDKICK) && IsDamageMotion() == false)
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
			SelectDamageMotion(m_nParrySuccess, playerFootR);
		}
	}

	// パリィモーションの蹴りになったら
	if (pPlayerMotion->IsEventFrame(13, 13, pPlayer->TYPE_PUNCH) && IsDamageMotion() == false)
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
			SelectDamageMotion(m_nParrySuccess, playerHandR);
		}
	}
}

//===================================================
// 親子関係の設定処理
//===================================================
void CEnemy::SetParent(const int nCnt)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_weponMatrix);

	// 親の位置、向きの設定
	D3DXVECTOR3 ParentPos = CCharacter3D::GetModelPos(nCnt);
	D3DXVECTOR3 ParentRot = CCharacter3D::GetModelRot(nCnt);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, ParentRot.y, ParentRot.x, ParentRot.z);
	D3DXMatrixMultiply(&m_weponMatrix, &m_weponMatrix, &mtxRot);

	// 大きさの取得
	D3DXVECTOR3 Size = CCharacter3D::GetModelSize(nCnt);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, 0.0f, Size.y, 0.0f);
	D3DXMatrixMultiply(&m_weponMatrix, &m_weponMatrix, &mtxTrans);

	// 親のワールドマトリックスの取得
	pDevice->GetTransform(D3DTS_WORLD, &mtxParent);

	// 親のワールドマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_weponMatrix, &m_weponMatrix, &mtxParent);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_weponMatrix);
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
