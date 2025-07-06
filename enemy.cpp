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

constexpr float SHADOW_ALEVEL = 0.7f;			// 影のアルファ値
constexpr float SHADOW_MAX_HEIGHT = 500.0f;		// 影が見える最大の高さ
constexpr float SHADOW_SIZE = 150.0f;			// 影の大きさ
constexpr int NUM_RUBBLE = 16;					// 瓦礫を出す数
constexpr int NUM_MATRIX = 8;					// 武器につけるマトリックスの数
constexpr int NEXT_ACTION_TIME = 300;			// 次の行動の抽選までの時間
constexpr float ROCKON_HEIGHT = 100.0f;			// ロックオン時の見る場所

using MOTION = CEnemyMotionController::TYPE;    // モーションの種類の列挙型を使用
using namespace math;							// 名前空間mathを使用する
using namespace std;							// 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CEnemy::CEnemy()
{
	m_pMove = nullptr;
	m_nNumModel = NULL;
	D3DXMatrixIdentity(&m_weponMatrix);
	m_pOrbit = nullptr;
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

	// 優先順位の取得
	int nPriority = pEnemy->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pEnemy != nullptr)
	{
		// 自分のポインタの解放
		pEnemy->Uninit();

		// nullにする
		pEnemy = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

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
	// 状態パターンの生成
	m_pState = make_unique<CStateIdle>(0);

	// 移動クラスの生成
	m_pMove = make_shared<CVelocity>();

	// モーション制御クラスの生成
	m_pMotion = make_unique<CEnemyMotionController>();

	// ロード処理
	m_pMotion->Load(m_apModel,&m_nNumModel);
	m_pMotion->Init(m_pMove);

	// キャラクターの作成
	m_pCharactor = make_unique<CCharacter3D>();

	// キャラクターの設定処理
	m_pCharactor->SetCharacter(10, 5.0f);

	//// 敵のAIの生成
	//m_pAI = make_unique<CEnemyAI>();

	//// 敵のAI初期化処理
	//m_pAI->Init(m_pMotion);

	// 位置の取得処理
	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

	// 影の取得生成
	m_pShadow = CShadow::Create(pos, 100.0f, 100.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, SHADOW_ALEVEL));

	// 当たり判定の生成
	m_pSphere = CCollisionSphere::Create(pos,150.0f);
	
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

	if (m_pShadow != nullptr)
	{
		// 影クラスの破棄
		m_pShadow->Uninit();
	}

	// nullにする
	m_pOrbit = nullptr;

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
	CPlayer* pPlayer = CManager::GetPlayer();

	// カメラの取得処理
	CCamera* pCamera = CManager::GetCamera();

	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	// モーションの制御クラスの取得
	CPlayerMotionController* pPlayerMotion = pPlayer->GetMotionController();

	// 移動量の減衰
	m_pMove->SetInertia3D(0.25f);

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	pos += move;

	// メッシュフィールドの取得
	CMeshField* pMesh = CManager::GetMeshField();

	float fHeight = 0.0f;

	// フィールドの当たり判定
	if (pMesh->Collision(pos, &fHeight))
	{
		pos.y = fHeight;
		m_pCharactor->SetPosition(pos);
	}

	// シリンダーの取得
	CMeshCylinder* pCylinder = CManager::GetCylinder();

	// シリンダーの当たり判定
	pCylinder->Collision(&pos);

	// 重力の設定
	m_pMove->Gravity(-MAX_GRABITY);

	if (m_pShadow != nullptr)
	{
		pMesh = CManager::GetMeshField();

		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 		   // 地面の法線ベクトルの取得
		D3DXVECTOR3 VecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // 上方向ベクトルの作成

		// 地面の角度に合わせた角度を設定
		m_pShadow->SetFieldAngle(FieldNor, VecU);

		// 影の設定処理
		m_pShadow->Update(D3DXVECTOR3(pos.x, pos.y - fHeight, pos.z), D3DXVECTOR3(pos.x, fHeight + 5.0f, pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT, SHADOW_ALEVEL);
	}

	// モーションがnullじゃないなら
	if (m_pMotion != nullptr)
	{
		// モーションの更新処理
		m_pMotion->Update(&m_apModel[0], m_nNumModel);
	}
	// インパクトの取得
	CMeshFieldImpact* pImpact = pMesh->GetImpact();

	if (pImpact != nullptr)
	{
		// インパクトとの判定
		const bool bCollision = pImpact->Collision(pos, 150.0f, pImpact->OBJ_ENEMY);

		// インパクトの当たり判定
		if (bCollision && m_pMotion->GetBlendType() != MOTION::TYPE_DAMAGE)
		{
			// インパクトの位置の取得
			D3DXVECTOR3 impactPos = pImpact->GetPosition();

			// 吹き飛び処理
			BlowOff(impactPos, 150.0f, 10.0f);

			// 状態の設定
			SetState(make_unique<CStateDamage>());

			// モーションの設定
			m_pMotion->SetMotion(MOTION::TYPE_DAMAGE, true, 5);
		}
	}

	// 武器の先端の位置
	D3DXVECTOR3 WeponPos = GetPositionFromMatrix(m_weponMatrix);
	D3DXVECTOR3 WeponBottom = GetPositionFromMatrix(m_apModel[15]->GetMatrixWorld());

	// 胸の位置の取得
	D3DXVECTOR3 chestpos = GetPositionFromMatrix(m_apModel[2]->GetMatrixWorld());

	// 敵の攻撃のカウンターの目安の表示
	if (m_pMotion->IsEventFrame(50, 50, MOTION::TYPE_SMASH))
	{
		// パーティクルの生成
		CParticle3D::Create(chestpos, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 240, 100.0f, 50, 120, 15.0f);
	}
	else if (m_pMotion->IsEventFrame(50, 50, MOTION::TYPE_IMPACT))
	{
		CMeshWave::Config WaveConfig = { WHITE,50.0f,50.0f,0.0f,30 };
		CMeshWave::Create(WaveConfig, pos);
	}


	// イベントフレームの判定
	if (m_pMotion->IsEventFrame(64, 71, MOTION::TYPE_SMASH) && m_pMotion->GetBlendType() != MOTION::TYPE_DAMAGE)
	{
		// 当たり判定がnullじゃなかったら
		if (m_pSphere != nullptr)
		{
			// プレイヤーの視界の中にいる
			const bool bParry = pPlayer->IsParry(pos);

			// 当たったら
			if (CollisionWepon() && bParry)
			{
				// パリィモーションの再生
				pPlayerMotion->SetMotion(pPlayerMotion->TYPE_PARRY, true, 2);
			}
			// 範囲内で視界に入っていない、カウンターしていない
			else if(CollisionWepon() && bParry == false)
			{
				// 吹き飛び処理
				pPlayer->BlowOff(pos, 10.0f, 10.0f);

				// ダメージモーションの再生
				pPlayerMotion->SetMotion(pPlayerMotion->TYPE_DAMAGE, true, 5);
			}
		}
	}

	// パリィモーションのパンチになったら
	if (pPlayerMotion->IsParryEvent(11, 11) && m_pMotion->GetBlendType() == MOTION::TYPE_SMASH)
	{
		// スローモーションの取得
		CSlow* pSlow = CManager::GetSlow();

		// スローモーション
		pSlow->Start(540, 12);

		// プレイヤーの右手の位置
		D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(5);

		// パーティクルの生成
		CParticle3D::Create(playerHandR, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f), 240, 20.0f, 50, 120, 15.0f);

		// ボスまでの角度を取得
		float fAngle = GetTargetAngle(pos, playerHandR);

		// 吹き飛び処理
		BlowOff(PlayerPos, 150.0f, 5.0f);

		// インパクトの設定
		CMeshCircle::Confing Circleconfig = { 35.0f,10.0f,0.0f,120.0f,120,false };

		// インパクトを生成
		CMeshCircle::Create(Circleconfig, D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.8f), playerHandR, 32, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		// モーションをダメージにする
		m_pMotion->SetMotion(MOTION::TYPE_DAMAGE, true, 2);

		// 状態の設定
		SetState(make_unique<CStateDamage>());
	}

	// 攻撃モーションのたたきつけになったら
	if (m_pMotion->IsEventFrame(72,72, MOTION::TYPE_SMASH))
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
	if (m_pMotion->IsEventFrame(102, 102, MOTION::TYPE_IMPACT))
	{
		// プレイヤーまでの方向
		D3DXVECTOR3 dir = PlayerPos - WeponPos;

		// インパクトの設定
		CMeshFieldImpact::Config config = { WeponPos,chestpos,dir,D3DXCOLOR(1.0f,0.5f,0.5f,1.0f),CMeshFieldImpact::OBJ_ENEMY,100.0f,750.0f,26.0f,60 };

		// インパクトの生成
		pMesh->SetImpact(config);
	}

	// 状態の更新処理
	m_pState->Update(this);

	// モーションの遷移
	m_pMotion->TransitionMotion(m_pCharactor.get());

	// 位置の設定処理
    m_pCharactor->SetPosition(pos);

	// 向きの補間
	m_pCharactor->GetRotation()->SetSmoothAngle(0.1f);

	pos.y += ROCKON_HEIGHT;

	pCamera->Rockon(PlayerPos, pos);
}

//===================================================
// 描画処理
//===================================================
void CEnemy::Draw(void)
{
	// 影の描画処理
	if (m_pShadow != nullptr)
	{
		m_pShadow->Draw();
	}

	// モーションのデバッグ表示
	if (m_pMotion != nullptr)
	{
		//m_pMotion->Debug();
	}

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
// 敵の吹き飛び処理
//===================================================
void CEnemy::BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump)
{
	// 位置
	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

	// アタッカーからプレイヤーまでの差分を求める
	D3DXVECTOR3 diff = pos - attacker;

	// 角度を求める
	float fAngle = atan2f(diff.x, diff.z);

	// 目的の角度の設定
	m_pCharactor->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

	// 移動量
	D3DXVECTOR3 move;

	// 移動量の設定
	move.x = sinf(fAngle) * blowOff;
	move.y = jump;
	move.z = cosf(fAngle) * blowOff;

	m_pMove->Set(move);
}

//===================================================
// 武器との当たり判定
//===================================================
bool CEnemy::CollisionWepon(void)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

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

		// 位置の更新
		m_pSphere->SetPos(pos);

#ifdef _DEBUG

		//// 武器のマトリックス確認用
		//CEffect3D::Create(pos, 50.0f, VEC3_NULL, WHITE, 10);
#endif // _DEBUG

		// 敵の武器に当たったら
		if (m_pSphere->Collision(pPlayer->GetSphere()))
		{
			return true;
		}
	}

	return false;
}

//===================================================
// 新しい状態の設定処理
//===================================================
void CEnemy::SetState(std::unique_ptr<CEnemyState> NewState)
{
	// 違うポインタなら
	if (m_pState != nullptr && m_pState->GetID() != NewState->GetID())
	{
		// 新しい状態の設定(所有権を移動させる)
		m_pState = std::move(NewState);
	}
}

//===================================================
// プレイヤーを追いかける処理
//===================================================
void CEnemy::ChasePlayer(float chaseScal, const float speedScal)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

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
// 距離の判定
//===================================================
bool CEnemy::CheckDistane(const float fRadius)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

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
// 向いている方向に進む処理
//===================================================
void CEnemy::MoveForWard(const float fSpeed)
{
	// 向きの取得
	D3DXVECTOR3 rot = m_pCharactor->GetRotation()->Get();

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 移動量の設定
	move.x = sinf(rot.y + D3DX_PI) * fSpeed;
	move.z = cosf(rot.y + D3DX_PI) * fSpeed;

	// 移動量の設定
	m_pMove->Set(move);
}

//===================================================
// 軌跡の処理
//===================================================
void CEnemy::Orbit(const int nSegH, const D3DXCOLOR col, const int nLife)
{
	// 武器の先端の位置
	D3DXVECTOR3 WeponTop = GetPositionFromMatrix(m_weponMatrix);
	D3DXVECTOR3 WeponBottom = GetPositionFromMatrix(m_apModel[15]->GetMatrixWorld());

	// 軌跡の生成
	if (m_pOrbit == nullptr)
	{
		m_pOrbit = CMeshOrbit::Create(WeponTop, WeponBottom, nSegH, col, nLife);
	}
	
	// 軌跡が使われていて寿命が無かったら
	if (m_pOrbit != nullptr && m_pOrbit->SetPosition(WeponTop, WeponBottom) == false)
	{
		// nullにする
		m_pOrbit = nullptr;
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
CEnemyMotionController::CEnemyMotionController()
{

}

//===================================================
// デストラクタ
//===================================================
CEnemyMotionController::~CEnemyMotionController()
{

}

//===================================================
// ロード処理
//===================================================
void CEnemyMotionController::Load(std::vector<CModel*>& pModel, int* pOutNumModel)
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
				m_pMotion = CMotion::Load(FILE_NAME, pModel, pOutNumModel, TYPE_MAX, m_pMotion->LOAD_TEXT);
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
// 初期化処理
//===================================================
void CEnemyMotionController::Init(shared_ptr<CVelocity> pMove)
{
	// 共有ポインタの取得
	m_pMove = pMove;
}

//===================================================
// 終了処理
//===================================================
void CEnemyMotionController::Uninit(void)
{
	// モーションの破棄
	if (m_pMotion != nullptr)
	{
		m_pMotion->Uninit();
	}
}

//===================================================
// 更新処理
//===================================================
void CEnemyMotionController::Update(CModel** ppModel, const int nNumModel)
{
	if (m_pMotion != nullptr)
	{
		// モーションの更新処理
		m_pMotion->Update(ppModel, nNumModel);
	}
}

//===================================================
// ロードできたかどうか
//===================================================
bool CEnemyMotionController::IsLoad(void) const
{
	// ロードできたかどうか
	if (m_pMotion->IsLoad())
	{
		return true;
	}
	return false;
}

//===================================================
// モーションの設定処理
//===================================================
void CEnemyMotionController::SetMotion(const int type, bool bBlend, const int nFrameBlend)
{
	m_pMotion->SetMotion(type, bBlend, nFrameBlend);
}

//===================================================
// ブレンドモーションの種類の取得処理
//===================================================
int CEnemyMotionController::GetBlendType(void) const
{
	return m_pMotion->GetBlendType();
}

//===================================================
// モーションの種類の取得処理
//===================================================
int CEnemyMotionController::GetType(void) const
{
	return m_pMotion->GetType();
}

//===================================================
// イベントフレームの判定
//===================================================
bool CEnemyMotionController::IsEventFrame(const int start, const int end, const TYPE type)
{
	// イベントフレーム内だったら
	if (m_pMotion->IsEventFrame(start, end, type))
	{
		return true;
	}

	return false;
}

//===================================================
// モーションのブレンドが完了したら
//===================================================
bool CEnemyMotionController::IsFinishEndBlend(void)
{
	// 最後のブレンドが終わったら
	if (m_pMotion->IsFinishEndBlend())
	{
		return true;
	}
	return false;
}

//===================================================
// モーションが終わったら
//===================================================
bool CEnemyMotionController::IsFinishMotion(void)
{
	return m_pMotion->FinishMotion();
}

//===================================================
// モーションの遷移処理
//===================================================
void CEnemyMotionController::TransitionMotion(CCharacter3D *pCharacter)
{
	// モーションの種類の取得
	TYPE type = (TYPE)m_pMotion->GetBlendType();

	CPlayer* pPlayer = CManager::GetPlayer();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	D3DXVECTOR3 pos = pCharacter->GetPosition();

	switch (type)
	{
	case TYPE_NEUTRAL:
		break;
	case TYPE_MOVE:

		break;
	case TYPE_SMASH:
	{
		if (m_pMotion->IsEventFrame(1, 64, TYPE_SMASH))
		{
			// プレイヤーまでの角度を求める
			float fAngle = GetTargetAngle(pos, PlayerPos);

			pCharacter->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
		}
	}
		break;
	case TYPE_DAMAGE:
		break;
	case TYPE_IMPACT:
	{
		if (m_pMotion->IsEventFrame(1, 91, TYPE_IMPACT))
		{
			// プレイヤーまでの角度を求める
			float fAngle = GetTargetAngle(pos, PlayerPos);

			pCharacter->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
		}
	}
		break;
	default:
		break;
	}
}
