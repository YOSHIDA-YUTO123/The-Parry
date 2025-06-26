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

//***************************************************
// マクロ定義
//***************************************************
#define SHADOW_A_LEVEL (0.9f)		// 影のアルファ値
#define SHADOW_SIZE (300.0f)		// 影の大きさ
#define SHADOW_MAX_HEIGHT (500.0f)	// 影が見える最大の高さ
#define NUM_DUST (16)				// 瓦礫を出す数

using namespace math;

//===================================================
// コンストラクタ
//===================================================
CEnemy::CEnemy(int nPriority) : CCharacter3D(nPriority)
{
	memset(m_apModel, NULL, sizeof(m_apModel));
	m_fSpeed = NULL;
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
	Load();
	//m_pMotion = CMotion::Load("data/MOTION/motionEnemy000.txt", &m_apModel[0], ENEMY_MAX_PARTS, &m_nNumModel, MOTIONTYPE_MAX,CMotion::LOAD_TEXT);

	// 敵のAIの生成
	m_pAI = new CEnemyAI;

	// 敵のAI初期化処理
	m_pAI->Init(m_pMotion);

	// 影の取得生成
	m_pShadow = CShadow::Create(GetPosition()->Get(), 100.0f, 100.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, SHADOW_A_LEVEL));

	// 当たり判定の生成
	m_pCollision = new CCollisionSphere;
	
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

	if (pMesh->Collision(pos, &fHeight))
	{
		pos.y = fHeight;
		GetPosition()->Set(pos);
	}
	
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
		m_pShadow->Setting(D3DXVECTOR3(pos.x, pos.y - fHeight, pos.z), D3DXVECTOR3(pos.x, fHeight + 5.0f, pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT, SHADOW_A_LEVEL);

		m_pShadow->GetRotaition()->Set(rot);
	}

	if (m_pMotion != nullptr)
	{
		float fAngle = GetTargetAngle(PlayerPos, pos);		  // ターゲットまでの角度の取得

		// 待機じゃなかったら
		if (m_pAI != nullptr && m_pAI->Wait() == false)
		{
			// 一定の距離にいたら
			if (m_pAI->CheckDistance(PlayerPos, pos, 1000.0f) == CEnemyAI::ACTION_MOVE)
			{
				m_pMotion->SetMotion(MOTIONTYPE_MOVE, true, 10);
			}
			// いなかったら
			else if (m_pAI->CheckDistance(PlayerPos, pos, 1000.0f) == CEnemyAI::ACTION_IDLE)
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

	D3DXVECTOR3 modelpos = GetPositionFromMatrix(m_weponMatrix);

	// 攻撃モーションのたたきつけになったら
	if (m_pMotion->IsIventFrame(72,72,MOTIONTYPE_ACTION))
	{
		// 地面に波を発生させる
		pMesh->SetWave(modelpos,20.0f,300.0f,380.0f,25.0f,0.01f,120);

		// メッシュサークルの生成
		CMeshCircle::Create(modelpos, 0.0f, 50.0f, 35.0f, 60,32,D3DXCOLOR(1.0f,0.5f,0.5f,1.0f));

		// 瓦礫の数分出す
		for (int nCnt = 0; nCnt < NUM_DUST; nCnt++)
		{
			// 分割に応じた方向を求める
			float fAngle = (D3DX_PI * 2.0f) / NUM_DUST * nCnt;

			// 吹っ飛び量を選出
			float dir = rand() % 15 + 5.0f;
			float Jump = rand() % 15 + 25.0f;

			// 方向に応じた吹っ飛び量を計算
			float fMoveX = sinf(fAngle) * dir;
			float fMoveZ = cosf(fAngle) * dir;

			// 寿命を選出
			int nLife = rand() % 120 + 60;

			// 瓦礫を生成
			CRubble::Create(modelpos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife);
		}
	}

	// イベントフレームの判定
	if (m_pMotion->IsIventFrame(64, 78, MOTIONTYPE_ACTION))
	{
		// 当たり判定がnullじゃなかったら
		if (m_pCollision != nullptr)
		{
			m_pCollision->SetRadius(250.0f);
			m_pCollision->SetElement(modelpos);

			// 状態の取得
			STATE PlayerState = pPlayer->GetState();

			// 当たったら
			if (m_pCollision->Collision(pPlayer->GetSphere()) && PlayerState == STATE_ACTION)
			{
				m_pMove->Jump(55.0f);
			}
		}
	}

	// モーションの遷移
	TransitionMotion();

	// 位置の設定処理
	GetPosition()->Set(pos);

	// 向きの補間
	GetRotation()->SetSmoothAngle(0.1f);

	D3DXVECTOR3 modelPos = math::GetPositionFromMatrix(m_apModel[2]->GetMatrixWorld());

	modelPos.y *= 0.5f;

	pCamera->Rockon(PlayerPos, modelPos);
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
// 敵の移動処理
//===================================================
void CEnemy::UpdateMoveMotion(void)
{
	CPlayer* pPlayer = CManager::GetPlayer();

	D3DXVECTOR3 pos = GetPosition()->Get();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition()->Get();

	D3DXVECTOR3 TargetDir = GetVector(PlayerPos, pos); // ターゲットまでの方向を取得
	float fAngle = GetTargetAngle(PlayerPos, pos);		  // ターゲットまでの角度の取得

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 移動量の設定
	move.x = TargetDir.x * 10.0f;
	move.z = TargetDir.z * 10.0f;

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
	MOTIONTYPE type = (MOTIONTYPE)m_pMotion->GetBlendMotionType();

	switch (type)
	{
	case MOTIONTYPE_NEUTRAL:
		break;
	case MOTIONTYPE_MOVE:

		// 敵の移動処理
		UpdateMoveMotion();

		break;
	case MOTIONTYPE_ACTION:
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

	// 距離を求める
	float fDistance = GetDistance(Diff);

	if (fDistance >= fRadius)
	{
		m_Action = ACTION_MOVE;

		return ACTION_MOVE;
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

		m_pMotion->SetMotion(CEnemy::MOTIONTYPE_NEUTRAL, true, 10);

		m_nCounterAction = 60;

		return true;
	}

	return false;
}
