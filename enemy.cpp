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
#include"camera.h"
#include "game.h"

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
	m_nNumModel = NULL;
	D3DXMatrixIdentity(&m_weponMatrix);
	m_pOrbit = nullptr;
	m_posOld = VEC3_NULL;
	m_Size = VEC3_NULL;
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

	pEnemy->Init();
	pEnemy->m_pCharactor->Init();
	pEnemy->m_pCharactor->SetPosition(pos);
	pEnemy->m_pCharactor->GetRotation()->Set(rot);

	return pEnemy;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CEnemy::Init(void)
{
	// モーションロード処理
	Load();

	m_pMachine = make_unique<CStateMachine>();

	ChangeState(make_shared<CEnemyIdle>(10));

	// 移動クラスの生成
	m_pMove = make_unique<CVelocity>();

	// キャラクターの作成
	m_pCharactor = make_unique<CCharacter3D>();

	// キャラクターの設定処理
	m_pCharactor->SetCharacter(10, 12.0f,D3DXVECTOR3(5.0f, 1.0f, 5.0f));

	// 位置の取得処理
	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

	// 当たり判定の生成
	m_pSphere = CColliderSphere::Create(pos,150.0f);
	
	// 中心を求める
	D3DXVECTOR3 CenterPos = VEC3_NULL;

	// TODO : 大きさ(ファイル読み込みしたい)
	m_Size = { 100.0f,400.0f,100.0f };

	// 中心座標を設定
	CenterPos.x = pos.x;
	CenterPos.y = pos.y + m_Size.y * 0.5f;
	CenterPos.z = pos.z;

	// 矩形判定AABBの生成
	m_pAABB = CColliderAABB::Create(CenterPos, m_posOld, m_Size);

	// 移動制御クラスの生成
	m_pMovement = make_unique<CEnemyMovement>();

	m_pMovement->Init(m_pMove, m_pCharactor);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CEnemy::Uninit(void)
{
	for (int nCnt = 0; nCnt < (int)m_apModel.size(); nCnt++)
	{
		// モデルの破棄
		if (m_apModel[nCnt] != nullptr)
		{
			// 終了処理
			m_apModel[nCnt]->Uninit();

			delete m_apModel[nCnt];

			m_apModel[nCnt] = nullptr;
		}
	}

	if (m_pMotion != nullptr)
	{
		// モーションの終了処理
		m_pMotion->Uninit();
	}

	// nullにする
	m_pOrbit = nullptr;
	m_pAABB = nullptr;

	if (m_pCharactor != nullptr)
	{
		// キャラクターの終了処理
		m_pCharactor->Uninit();
	}

	CObject::Release();
}

//===================================================
// 更新処理
//===================================================
void CEnemy::Update(void)
{
	// プレイヤーの取得
	CPlayerGame* pPlayer = CGame::GetPlayer();

	// カメラの取得処理
	CCamera* pCamera = CManager::GetCamera();

	// 位置の取得
	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

#ifdef _DEBUG

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	CDebugProc::Print("ボスの攻撃(スマッシュ) [ F3 ]\n");
	CDebugProc::Print("ボスの攻撃(衝撃波) [ F4 ]\n");
	CDebugProc::Print("ボスの攻撃(方向→ダッシュ→回転) [ F5 ]\n");
	CDebugProc::Print("ボスの消去 [ 1 ]\n");

	if (pKeyboard->GetPress(DIK_F3))
	{
		ChangeState(make_shared<CEnemyAttackSmash>());
	}
	if (pKeyboard->GetPress(DIK_F4))
	{
		ChangeState(make_shared<CEnemyAttackImpact>());
	}
	if (pKeyboard->GetPress(DIK_F5))
	{
		ChangeState(make_shared<CEnemyRoar>());
	}
	if (pKeyboard->GetTrigger(DIK_T))
	{
		ChangeState(make_shared<CEnemyBackStep>());
	}
	if (pKeyboard->GetTrigger(DIK_1))
	{
		// 影の消去
		m_pCharactor->DeleteShadow();
		Uninit();
		return;
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

		if (m_pMotion->GetBlendType() == MOTION_JUMP)
		{
			// 着地状態にする
			ChangeState(make_shared<CEnemyLanding>());
		}
		m_pCharactor->SetPosition(pos);
	}

	// 障害物との当たり判定
	if (CollisionObstacle(&pos))
	{
	}

	// シリンダーの取得
	CMeshCylinder* pCylinder = CGame::GetCylinder();

	// シリンダーの当たり判定
	if (pCylinder != nullptr)
	{
		pCylinder->Collision(&pos);
	}
	
	// 重力の設定
	m_pMove->Gravity(-MAX_GRABITY);

	// モーションがnullじゃないなら
	if (m_pMotion != nullptr)
	{
		// モーションの更新処理
		m_pMotion->Update(&m_apModel[0], m_nNumModel);
	}

	// インパクトとの判定
	const bool bCollision = pMesh->CollisionImpact(pos, 150.0f,CMeshFieldImpact::OBJ_ENEMY);

	// インパクトの当たり判定
	if (bCollision && IsDamageMotion() == false)
	{			
		// 状態の設定
		ChangeState(make_shared<CEnemyDamageL>());

		// モーションの設定
		m_pMotion->SetMotion(MOTION_DAMAGEL, true, 2);
	}
	

	// 武器の先端の位置
	D3DXVECTOR3 WeponPos = GetPositionFromMatrix(m_weponMatrix);
	D3DXVECTOR3 WeponBottom = GetPositionFromMatrix(m_apModel[15]->GetMatrixWorld());

	// 胸の位置の取得
	D3DXVECTOR3 chestpos = GetPositionFromMatrix(m_apModel[2]->GetMatrixWorld());

	// 敵の攻撃のカウンターの目安の表示
	if (m_pMotion->IsEventFrame(50, 50, MOTION_SMASH))
	{
		// パーティクルの生成
		CParticle3D::Create(chestpos, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 240, 100.0f, 50, 1, 15.0f);
	}
	else if (m_pMotion->IsEventFrame(50, 50, MOTION_IMPACT))
	{
		CMeshWave::Config WaveConfig = { WHITE,50.0f,50.0f,0.0f,30 };
		CMeshWave::Create(WaveConfig, pos);
	}

	// パリィモーションのパンチになったら
	if (pPlayerMotion->IsEventFrame(11, 11,pPlayer->TYPE_PARRY) && IsDamageMotion() == false)
	{
		// プレイヤーの右手の位置
		D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(5);

		// 円の当たり判定の取得
		CCollisionSphere* pSphere = CCollisionSphere::GetInstance();

		// 右手の円
		CColliderSphere HandRSphere = CColliderSphere::CreateCollider(playerHandR,50.0f);
		CColliderSphere ChestSphere = CColliderSphere::CreateCollider(chestpos, 250.0f);

		// 手が当たったら
		if (pSphere != nullptr && pSphere->Collision(&ChestSphere, &HandRSphere))
		{
			// ボスまでの角度を取得
			float fAngle = GetTargetAngle(pos, PlayerPos);

			// 向きの設定
			pPlayer->SetAngle(fAngle + D3DX_PI);

			// パーティクルの生成
			CParticle3D::Create(playerHandR, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f), 240, 10.0f, 50, 5, 15.0f);

			// インパクトの設定
			CMeshCircle::Confing Circleconfig = { 35.0f,10.0f,0.0f,120.0f,120,false };

			// インパクトを生成
			CMeshCircle::Create(Circleconfig, D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.8f), playerHandR, 32, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

			// どの攻撃モーションがでるか判定
			SelectDamageMotion(m_nParrySuccess);
		}
	}

	// 攻撃モーションのたたきつけになったら
	if (m_pMotion->IsEventFrame(72,72, MOTION_SMASH))
	{
		// フィールドの波の設定
		CMeshFieldWave::Config config = { WeponPos,250.0f,380.0f,280.0f,12.0f,0.01f,120 };

		// 地面に波を発生させる
		pMesh->SetWave(config);

		// インパクトの設定
		CMeshCircle::Confing Circleconfig = { 0.0f,35.0f,0.0f,50.0f,60,true };

		// メッシュサークルの生成
		CMeshCircle::Create(Circleconfig, D3DXCOLOR(1.0f, 0.5f, 0.5f, 1.0f), WeponPos);

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

	// 衝撃波の生成
	if (m_pMotion->IsEventFrame(102, 102, MOTION_IMPACT))
	{
		// プレイヤーまでの方向
		D3DXVECTOR3 dir = PlayerPos - WeponPos;

		// インパクトの設定
		CMeshFieldImpact::Config config = { WeponPos,chestpos,dir,D3DXCOLOR(1.0f,0.5f,0.5f,1.0f),CMeshFieldImpact::OBJ_ENEMY,150.0f,750.0f,26.0f,60 };

		// インパクトの生成
		pMesh->SetImpact(config);
	}

	if (m_pMachine != nullptr)
	{
		// 状態の更新処理
		m_pMachine->Update();
	}

	// キャラクターがnullじゃないなら
	if (m_pCharactor != nullptr)
	{
		// 位置の設定処理
		m_pCharactor->SetPosition(pos);

		m_pCharactor->Update();

		// 向きの補間
		m_pCharactor->GetRotation()->SetSmoothAngle(0.1f);
	}

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
	//// 影の描画処理
	//if (m_pShadow != nullptr)
	//{
	//	m_pShadow->Draw();
	//}

	// キャラクターの描画
	if (m_pCharactor != nullptr)
	{
		m_pCharactor->Draw();
	}

	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		if (m_apModel[nCnt] != nullptr)
		{
			// 描画処理
			m_apModel[nCnt]->Draw();

			// モデルが武器だったら
			if (nCnt == 15)
			{
				// 親子関係の設定処理
				SetParent(nCnt);
			}
		}
	}
}

//===================================================
// 位置の取得処理
//===================================================
D3DXVECTOR3 CEnemy::GetPosition(void)
{
	// 位置
	D3DXVECTOR3 Outpos = VEC3_NULL;

	if (m_pCharactor != nullptr)
	{
		// 位置の取得
		Outpos = m_pCharactor->GetPosition();
	}

	return Outpos;
}

//===================================================
// モーションの取得
//===================================================
CMotion* CEnemy::GetMotion(void)
{
	return m_pMotion.get();
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
void CEnemy::SelectDamageMotion(int success)
{
	int a = 0;
	// 成功度の遷移
	switch (success)
	{
	case CPlayerGame::PARRY_MISS:
		break;
	case CPlayerGame::PARRY_WEAK:
		// 状態の設定
		ChangeState(make_shared<CEnemyDamageS>());
		break;
	case CPlayerGame::PARRY_NORMAL:
		// 状態の設定
		ChangeState(make_shared<CEnemyDamageS>());
		break;
	case CPlayerGame::PARRY_PARFECT:
		// 状態の設定
		ChangeState(make_shared<CEnemyDamageL>());
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
	// モーションの種類の取得
	int motionType = m_pMotion->GetBlendType();

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
	CPlayerGame* pPlayer = CGame::GetPlayer();

	// 武器の先の座標
	D3DXVECTOR3 WeponTop = GetPositionFromMatrix(m_weponMatrix);

	// 武器の根元の座標
	D3DXVECTOR3 WeponBottom = GetPositionFromMatrix(m_apModel[15]->GetMatrixWorld());

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
	D3DXVECTOR3 playerPos = pPlayer->GetPos();
	
	// 位置の取得
	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

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
	float fSpeed = m_pCharactor->GetSpeed();

	move.x = sinf(fRotMove) * (fSpeed * speedScal);
	move.z = cosf(fRotMove) * (fSpeed * speedScal);

	D3DXVECTOR3 TargetDir = GetVector(playerPos, pos);	  // ターゲットまでの方向を取得
	float fAngle = GetTargetAngle(playerPos, pos);		  // 目標までの角度の取得

	// 移動量の設定
	m_pMove->Set(move);

	// 目的の角度の設定
	m_pCharactor->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));
}

//===================================================
// プレイヤーの方向を見る処理
//===================================================
void CEnemy::AngleToPlayer(void)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// プレイヤーの位置
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	// 自分の位置の取得
	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

	// プレイヤーまでの角度を求める
	float fAngle = GetTargetAngle(pos, PlayerPos);

	// 向きの設定
	m_pCharactor->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
}

//===================================================
// 距離の判定
//===================================================
bool CEnemy::CheckDistane(const float fRadius)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerpos = pPlayer->GetPos();

	// 敵の位置の取得
	D3DXVECTOR3 enemypos = m_pCharactor->GetPosition();
	
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
	D3DXVECTOR3 WeponBottom = GetPositionFromMatrix(m_apModel[15]->GetMatrixWorld());

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

		// 中心を求める
		D3DXVECTOR3 CenterPos = VEC3_NULL;

		// 中心座標を設定
		CenterPos.x = pPos->x;
		CenterPos.y = pPos->y + m_Size.y * 0.5f;
		CenterPos.z = pPos->z;

		// データの更新処理
		m_pAABB->UpdateData(CenterPos, D3DXVECTOR3(m_posOld.x, m_posOld.y + m_Size.y * 0.5f, m_posOld.z));

		// 当たっていたら
		if (pObstacle != nullptr && pObstacle->Collision(m_pAABB.get(), pPos))
		{
			// 障害物の位置の取得
			D3DXVECTOR3 obstaclePos = pObstacle->GetPosition();

			// 障害物までの向きの取得
			float fAngle = GetTargetAngle(*pPos, obstaclePos);

			// 向きの設定
			m_pCharactor->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

			// ダメージ状態にする
			ChangeState(make_shared<CEnemyDamageL>(true));

			// モーションの設定
			m_pMotion->SetMotion(MOTION_DAMAGEL, true, 2);

			return true;
		}
	}

	return false;
}

//===================================================
// 読み込み処理
//===================================================
void CEnemy::Load(void)
{
	fstream file("data/system.ini"); // ファイルを開く
	string line; // ファイルの文字列読み取り用
	string input; // 値を代入する

	// ファイルを開けたら
	if (file.is_open())
	{
		// ロードのマネージャの生成
		CLoadManager* pLoadManager = new CLoadManager;

		// 最後じゃないなら
		while (getline(file, line))
		{
			// プレイヤーのモーションファイルを読み取ったら
			if (line.find("ENEMY000_MOTION_FILE") != string::npos)
			{
				size_t equal_pos = line.find("="); // =の位置

				// [=] から先を求める
				input = line.substr(equal_pos + 1);

				// ファイルの名前を取得
				string file_name = pLoadManager->GetString(input);

				// ファイルの名前を代入
				const char* FILE_NAME = file_name.c_str();

				// モーションのロード処理
				m_pMotion = CMotion::Load(FILE_NAME, m_apModel, &m_nNumModel, MOTION_MAX, m_pMotion->LOAD_TEXT);
			}
		}

		// ロードのマネージャーの破棄
		if (pLoadManager != nullptr)
		{
			delete pLoadManager;
			pLoadManager = nullptr;
		}
		// ファイルを閉じる
		file.close();
	}
	else
	{
		MessageBox(NULL, "system.iniが開けません", "ファイルが存在しません。", MB_OK | MB_ICONWARNING);
		return;
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
	D3DXVECTOR3 ParentPos = m_apModel[nCnt]->GetPosition();
	D3DXVECTOR3 ParentRot = m_apModel[nCnt]->GetRotaition();

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, ParentRot.y, ParentRot.x, ParentRot.z);
	D3DXMatrixMultiply(&m_weponMatrix, &m_weponMatrix, &mtxRot);

	// 大きさの取得
	D3DXVECTOR3 Size = m_apModel[nCnt]->GetSize();

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, ParentPos.x, ParentPos.y + Size.y, ParentPos.z);
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
void CEnemyMovement::Init(std::shared_ptr<CVelocity> enemyMove, std::shared_ptr<CCharacter3D> enemy)
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
		// 敵の移量を参照
		m_pEnemy = enemy;
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
	m_pEnemy->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

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
	D3DXVECTOR3 rot = m_pEnemy->GetRotation()->Get();

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
	D3DXVECTOR3 rot = m_pEnemy->GetRotation()->Get();

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
