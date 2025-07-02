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

//***************************************************
// マクロ定義
//***************************************************

// 匿名の名前空間(shadow.cpp限定)
namespace
{
	constexpr float SHADOW_ALEVEL = 0.7f;		// 影のアルファ値
	constexpr float SHADOW_MAX_HEIGHT = 500.0f; // 影が見える最大の高さ
	constexpr float SHADOW_SIZE = 200.0f;		// 影の大きさ
	constexpr int NUM_RUBBLE = 16;				// 瓦礫を出す数
	constexpr int NUM_MATRIX = 8;				// 武器につけるマトリックスの数
	constexpr int NEXT_ACTION_TIME = 300;		// 次の行動の抽選までの時間
	constexpr float ROCKON_HEIGHT = 100.0f;		// ロックオン時の見る場所
}

using namespace math; // 名前空間mathを使用する

//===================================================
// コンストラクタ
//===================================================
CEnemy::CEnemy(int nPriority) : CCharacter3D(nPriority)
{
	memset(m_apModel, NULL, sizeof(m_apModel));
	m_pMove = nullptr;
	m_nNumModel = NULL;
	m_pMotion = nullptr;
	m_pAI = nullptr;
	m_pShadow = nullptr;
	D3DXMatrixIdentity(&m_weponMatrix);
	m_pCollision = nullptr;
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
		pEnemy->Release();

		// nullにする
		pEnemy = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pEnemy == nullptr) return nullptr;

	pEnemy->CCharacter3D::Init();
	pEnemy->GetPosition()->Set(pos);
	pEnemy->GetRotation()->Set(rot);
	pEnemy->Init();

	return pEnemy;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CEnemy::Init(void)
{
	// ロード処理
	Load();

	// キャラクターの設定処理
	SetCharacter(10, 5.0f);

	// 敵のAIの生成
	m_pAI = new CEnemyAI;

	// 敵のAI初期化処理
	m_pAI->Init(m_pMotion);

	// 位置の取得処理
	D3DXVECTOR3 pos = GetPosition()->Get();

	// 影の取得生成
	m_pShadow = CShadow::Create(pos, 100.0f, 100.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, SHADOW_ALEVEL));

	// 当たり判定の生成
	m_pCollision = CCollisionSphere::Create(pos,150.0f);
	
	// 移動クラスの生成
	m_pMove = new CVelocity;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CEnemy::Uninit(void)
{
	for (int nCnt = 0; nCnt < ENEMY_MAX_PARTS; nCnt++)
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

	// モーションの終了処理
	m_pMotion->Uninit();

	// モーションの破棄
	if (m_pMotion != nullptr)
	{
		delete m_pMotion;

		m_pMotion = nullptr;
	}

	// AIの破棄
	if (m_pAI != nullptr)
	{
		delete m_pAI;
		m_pAI = nullptr;
	}

	// 当たり判定の破棄
	if (m_pCollision != nullptr)
	{
		delete m_pCollision;
		m_pCollision = nullptr;
	}

	// 移動クラスの破棄
	if (m_pMove != nullptr)
	{
		delete m_pMove;
		m_pMove = nullptr;
	}

	CCharacter3D::Uninit();
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

	//D3DXVECTOR3 pos = GetPosition();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition()->Get();

	// 移動量の減衰
	m_pMove->SetInertia3D(0.25f);

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	GetPosition()->UpdatePosition(move);

	// メッシュフィールドの取得
	CMeshField* pMesh = CManager::GetMeshField();

	float fHeight = 0.0f;

	D3DXVECTOR3 pos = GetPosition()->Get();

	// フィールドの当たり判定
	if (pMesh->Collision(pos, &fHeight))
	{
		pos.y = fHeight;
		GetPosition()->Set(pos);
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

		// 地面の角度に合わせた角度を取得
		D3DXVECTOR3 rot = m_pShadow->GetFieldAngle(FieldNor, VecU);

		// 影の設定処理
		m_pShadow->Setting(D3DXVECTOR3(pos.x, pos.y - fHeight, pos.z), D3DXVECTOR3(pos.x, fHeight + 5.0f, pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT, SHADOW_ALEVEL);

		m_pShadow->GetRotaition()->Set(rot);
	}

	// モーションがnullじゃないなら
	if (m_pMotion != nullptr)
	{
		float fAngle = GetTargetAngle(PlayerPos, pos);		  // ターゲットまでの角度の取得

		// 待機じゃなかったら
		if (m_pAI != nullptr && m_pAI->Wait() == false)
		{
			// 一定の距離にいたら
			if (m_pAI->CheckDistance(PlayerPos, pos, 250.0f) == CEnemyAI::ACTION_MOVE)
			{
				m_pMotion->SetMotion(MOTIONTYPE_MOVE, true, 10);
			}
			// いなかったら
			else if (m_pAI->CheckDistance(PlayerPos, pos, 250.0f) == CEnemyAI::ACTION_IDLE)
			{
				m_pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 10);
			}

			// 攻撃可能だったら
			if (m_pAI->IsAttack() == CEnemyAI::ACTION_ATTACK)
			{
				m_pMotion->SetMotion(MOTIONTYPE_ACTION, true, 10);

				// 目的の角度の設定
				GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));
			}
		}

		// AIの更新処理
		if (m_pAI != nullptr)
		{
			m_pAI->Update();
		}

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
		if (bCollision && m_pMotion->GetBlendType() != MOTIONTYPE_DAMAGE)
		{
			// インパクトの位置の取得
			D3DXVECTOR3 impactPos = pImpact->GetPosition();

			// 吹き飛び処理
			BlowOff(impactPos, 150.0f, 10.0f);

			// モーションの設定
			m_pMotion->SetMotion(MOTIONTYPE_DAMAGE, true, 5);
		}
	}

	D3DXVECTOR3 modelpos = GetPositionFromMatrix(m_weponMatrix);

	// イベントフレームの判定
	if (m_pMotion->IsIventFrame(64, 71, MOTIONTYPE_ACTION) && m_pMotion->GetBlendType() != MOTIONTYPE_DAMAGE)
	{
		// 当たり判定がnullじゃなかったら
		if (m_pCollision != nullptr)
		{
			// 状態の取得
			STATE PlayerState = pPlayer->GetState();

			// プレイヤーの視界判定の取得
			CCollisionFOV* pPlayerFOV = pPlayer->GetFOV();

			// 向きの取得
			D3DXVECTOR3 playerRot = pPlayer->GetRotation()->Get();

			// プレイヤーの視界の中にいる
			const bool bCollisionFOV = pPlayerFOV->Collision(pos, playerRot.y, D3DX_PI * 0.5f, -D3DX_PI * 0.5f);

			// 当たったら
			if (CollisionWepon() && bCollisionFOV && PlayerState == STATE_ACTION)
			{
				//// スローモーションの取得
				//CSlow* pSlow = CManager::GetSlow();

				//// スローモーション
				//pSlow->Start(540, 12);

				// プレイヤーの右手の位置
				D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(8);

				// パーティクルの生成
				CParticle3D::Create(playerHandR, D3DXCOLOR(1.0f,1.0f,0.4f,1.0f), 240, 20.0f, 50, 120, 15.0f);

				// ボスまでの角度を取得
				float fAngle = GetTargetAngle(pos, playerHandR);

				// 吹き飛び処理
				BlowOff(PlayerPos, 150.0f, 12.0f);

				// インパクトを生成
				CMeshCircle::Create(playerHandR, 0.0f, 120.0f, 10.0f, 30,35.0f, 32,D3DXCOLOR(1.0f,1.0f,0.4f,0.8f),D3DXVECTOR3(D3DX_PI * 0.5f, fAngle,0.0f),false);

				// ダメージモーションの再生
				pPlayer->SetMotion(pPlayer->MOTIONTYPE_PARRY, true, 2);

				// モーションをダメージにする
				m_pMotion->SetMotion(MOTIONTYPE_DAMAGE, true, 2);
			}
			// 範囲内で視界に入っていない、カウンターしていない
			else if(CollisionWepon() && (bCollisionFOV == false || PlayerState != STATE_ACTION))
			{
				// 吹き飛び処理
				pPlayer->BlowOff(pos, 10.0f, 10.0f);

				// ダメージモーションの再生
				pPlayer->SetMotion(pPlayer->MOTIONTYPE_DAMAGE, true, 5);
			}
		}
	}

	// 攻撃モーションのたたきつけになったら
	if (m_pMotion->IsIventFrame(72,72,MOTIONTYPE_ACTION))
	{
		// 地面に波を発生させる
		pMesh->SetWave(modelpos,250.0f,380.0f,280.0f,12.0f,0.01f,120);

		// メッシュサークルの生成
		CMeshCircle::Create(modelpos, 0.0f, 50.0f, 35.0f, 60,0.0f,32,D3DXCOLOR(1.0f,0.5f,0.5f,1.0f));

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
			CRubble::Create(modelpos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife, nType);
		}
	}

	if (m_pMotion->IsIventFrame(105, 105, MOTIONTYPE_ACTION2))
	{
		D3DXVECTOR3 dir = PlayerPos - modelpos;

		pMesh->SetImpact(modelpos, dir, 750.0f, 100.0f, 60,20.0f,CMeshFieldImpact::OBJ_ENEMY);
	}

	// モーションの遷移
	TransitionMotion();

	// 位置の設定処理
	GetPosition()->Set(pos);

	// 向きの補間
	GetRotation()->SetSmoothAngle(0.1f);

	pos.y += ROCKON_HEIGHT;

	pCamera->Rockon(PlayerPos, pos);
}

//===================================================
// 描画処理
//===================================================
void CEnemy::Draw(void)
{
	// キャラクターの描画
	CCharacter3D::Draw();

	if (m_pMotion != nullptr)
	{
		m_pMotion->Debug();
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
	D3DXVECTOR3 pos = GetPosition()->Get();

	// アタッカーからプレイヤーまでの差分を求める
	D3DXVECTOR3 diff = pos - attacker;

	// 角度を求める
	float fAngle = atan2f(diff.x, diff.z);

	// 目的の角度の設定
	GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

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
		m_pCollision->SetPos(pos);

#ifdef _DEBUG

		// 武器のマトリックス確認用
		CEffect3D::Create(pos, 50.0f, VEC3_NULL, WHITE, 10);
#endif // _DEBUG

		// 敵の武器に当たったら
		if (m_pCollision->Collision(pPlayer->GetSphere()))
		{
			return true;
		}
	}

	return false;
}

//===================================================
// 敵の移動処理
//===================================================
void CEnemy::UpdateMoveMotion(void)
{
	CPlayer* pPlayer = CManager::GetPlayer();

	D3DXVECTOR3 pos = GetPosition()->Get();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition()->Get();

	D3DXVECTOR3 TargetDir = GetVector(PlayerPos, pos);	  // ターゲットまでの方向を取得
	float fAngle = GetTargetAngle(PlayerPos, pos);		  // ターゲットまでの角度の取得

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 移動速度の取得
	float speed = GetSpeed();

	// 移動量の設定
	move.x = TargetDir.x * speed;
	move.z = TargetDir.z * speed;

	// 移動量の設定
	m_pMove->Set(move);

	// 目的の角度の設定
	GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));
}


//===================================================
// モーションの遷移
//===================================================
void CEnemy::TransitionMotion(void)
{
	// モーションの種類の取得
	MOTIONTYPE type = (MOTIONTYPE)m_pMotion->GetBlendType();

	CPlayer* pPlayer = CManager::GetPlayer();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition()->Get();

	D3DXVECTOR3 pos = GetPosition()->Get();

	switch (type)
	{
	case MOTIONTYPE_NEUTRAL:
		break;
	case MOTIONTYPE_MOVE:

		// 敵の移動処理
		UpdateMoveMotion();

		break;
	case MOTIONTYPE_ACTION:
	{
		if (m_pMotion->IsIventFrame(1, 64, MOTIONTYPE_ACTION))
		{
			// プレイヤーまでの角度を求める
			float fAngle = GetTargetAngle(pos, PlayerPos);

			GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
		}
	}
		break;
	case MOTIONTYPE_DAMAGE:
		break;
	case MOTIONTYPE_ACTION2:
	{
		if (m_pMotion->IsIventFrame(1, 91, MOTIONTYPE_ACTION2))
		{
			// プレイヤーまでの角度を求める
			float fAngle = GetTargetAngle(pos, PlayerPos);

			GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
		}
	}
		break;
	default:
		break;
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
// 敵のロード処理
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
				m_pMotion = CMotion::Load(FILE_NAME, &m_apModel[0], ENEMY_MAX_PARTS, &m_nNumModel, MOTIONTYPE_MAX, CMotion::LOAD_TEXT);
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
// 敵のAIのコンストラクタ
//===================================================
CEnemyAI::CEnemyAI()
{
	m_pMotion = nullptr;
	m_Action = ACTION_IDLE;
	m_nCounterAction = NULL;
	m_nNextCounter = NULL;
}

//===================================================
// 敵のAIのデストラクタ
//===================================================
CEnemyAI::~CEnemyAI()
{

}

//===================================================
// 敵のAIの初期化処理
//===================================================
void CEnemyAI::Init(CMotion* pMotion, const int nBlendFrame, const int nFirstMotion)
{
	// モーションの情報の取得
	m_pMotion = pMotion;

	m_pMotion->SetMotion(nFirstMotion, true, nBlendFrame);
}

//===================================================
// 更新処理
//===================================================
void CEnemyAI::Update(void)
{
	switch (m_Action)
	{
	case ACTION_WAIT:
		m_nCounterAction--;

		if (m_nCounterAction <= 0)
		{
			m_Action = ACTION_IDLE;
		}
		break;
	default:
		break;
	}
}

//===================================================
// 敵の移動のAI
//===================================================
int CEnemyAI::CheckDistance(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos,const float fRadius)
{
	if (m_Action != ACTION_IDLE && m_Action != ACTION_MOVE) return -1;

	// 差分を求める
	D3DXVECTOR3 Diff = dest - pos;

	// 次の行動の抽選までのカウンター
	m_nNextCounter++;

	int random = -1;

	// 次の行動の抽選タイムに達したら
	if (m_nNextCounter >= NEXT_ACTION_TIME)
	{
		// 行動を抽選
		random = rand() % 100 + 1;

		// カウンターをリセット
		m_nNextCounter = 0;
	}

	// 距離を求める
	float fDistance = GetDistance(Diff);

	// 範囲内に入ったら
	if (fDistance >= fRadius)
	{
		// 55%の確率で攻撃
		if (random <= 55 && random != -1)
		{
			m_Action = ACTION_ATTACK;
			m_pMotion->SetMotion(CEnemy::MOTIONTYPE_ACTION2, true, 5);
		}
		else
		{
			m_Action = ACTION_MOVE;
		}

		return m_Action;
	}

	m_Action = ACTION_IDLE;

	return ACTION_IDLE;
}

//===================================================
// 攻撃開始判定
//===================================================
int CEnemyAI::IsAttack(void)
{
	if (m_Action != ACTION_IDLE) return -1;

	int random = rand() % 100;

	m_Action = ACTION_ATTACK;

	return ACTION_ATTACK;
}

//===================================================
// 待機状態
//===================================================
bool CEnemyAI::Wait(void)
{
	if (m_pMotion->IsFinishEndBlend() == true)
	{
		m_Action = ACTION_WAIT;

		// モーションの種類を取得
		int moyiontype = m_pMotion->GetType();

		switch (moyiontype)
		{
		case CEnemy::MOTIONTYPE_ACTION:
			m_nCounterAction = 60;
			break;
		case CEnemy::MOTIONTYPE_DAMAGE:
			m_nCounterAction = 30;
			break;
		default:
			break;
		}

		m_pMotion->SetMotion(CEnemy::MOTIONTYPE_NEUTRAL, true, 10);


		return true;
	}

	return false;
}
