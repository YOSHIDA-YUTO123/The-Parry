//===================================================
//
// プレイヤー [player.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "player.h"
#include "input.h"
#include "manager.h"
#include "effect.h"
#include "particle.h"
#include"renderer.h"
#include "impact.h"
#include"motion.h"
#include"score.h"
#include"shadow.h"
#include "explosion.h"
#include "dust.h"
#include "LoadManager.h"
#include "Wave.h"
#include "cylinder.h"
#include"Collider.h"
#include "obstaclemanager.h"
#include"Obstacle.h"
#include"statebase.h"
#include"playerstate.h"
#include"shadowS.h"
#include"meshfield.h"
#include"camera.h"
#include"slow.h"
#include "game.h"

using namespace math; // 名前空間mathを使用
using namespace std;  // 名前空間をstdを使用する
using namespace Const;							// 名前空間Constを使用する

//using MOTION = CPlayerMotionController::TYPE; // 列挙型を使用する
using STATE = CCharacter3D::STATE;			  // キャラクターの状態

constexpr float JUMP_HEIGHT = 25.0f;		// ジャンプ量
constexpr float SHADOW_SIZE = 50.0f;		// 影の大きさ
constexpr float SHADOW_MAX_HEIGHT = 500.0f; // 影が見える最大の高さ
constexpr float SHADOW_A_LEVEL = 0.9f;		// 影のアルファ値のオフセット
constexpr int PARRY_TIME = 10;				// パリィの有効時間

//===================================================
// コンストラクタ
//===================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	m_pMove = nullptr;
	m_bJump = true;
	m_pScore = nullptr;
	m_nNumModel = NULL;
	m_bDash = false;
	m_posOld = VEC3_NULL;
	m_pMachine = nullptr;
	m_pMovement = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CPlayer::~CPlayer()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CPlayer::Init(void)
{
	// プレイヤーのロード処理
	Load();

	// キャラクタークラスの生成
	m_pCharacter3D = make_unique<CCharacter3D>();

	// キャラクターの設定処理
	m_pCharacter3D->SetCharacter(10, 6.0f,D3DXVECTOR3(2.0f,1.0f,2.0f));

	// キャラクターの位置の取得
	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	// 視界判定の生成
	m_pFOV = CColliderFOV::Create(pos, 0.0f, D3DX_PI * 0.5f, -D3DX_PI * 0.5f,1000.0f);

	// 円の当たり判定の生成
	m_pSphere = CColliderSphere::Create(pos, 50.0f);

	// スコアの生成
	m_pScore = (CScoreLerper*)CScore::Create(CScore::TYPE_LERPER,D3DXVECTOR3(1150.0f, 50.0f, 0.0f), 180.0f, 30.0f);

	// 移動クラスの生成
	m_pMove = make_unique<CVelocity>();

	// 状態制御の生成
	m_pMachine = make_unique<CStateMachine>();

	m_pMovement = make_unique<CPlayerMovement>();
	m_pMovement->Init(m_pMove.get());

	// 初期状態を設定
	ChangeState(make_shared<CPlayerNormal>());

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CPlayer::Uninit(void)
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

	// スコアの解放
	if (m_pScore != nullptr)
	{
		m_pScore = nullptr;
	}

	// 移動クラスの破棄
	if (m_pMove != nullptr)
	{
		m_pMove = nullptr;
	}

	// モーションの終了処理
	m_pMotion->Uninit();

	// キャラクターの破棄
	m_pCharacter3D->Uninit();

	CObject::Release();
}

//===================================================
// 更新処理
//===================================================
void CPlayer::Update(void)
{
	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// コントローラーの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// メッシュフィールドの取得
	CMeshField* pMesh = CGame::GetField();

	// カメラの取得処理
	CCamera* pCamera = CManager::GetCamera();

	// 読み込めていなかったら
	if (m_pMotion->IsLoad() == false)
	{
		return;
	}

	// 速さ
	float fSpeed = m_bDash ? m_pCharacter3D->GetSpeed() : 1.5f;

	float fAngleDest = 0.0f;

	// 移動できるか判定
	const bool bNotMove = m_pMotion->GetBlendType() == TYPE_DAMAGE || m_pMotion->GetBlendType() == TYPE_AVOID;

	// 移動できるなら
	if (bNotMove == false)
	{
		// 移動を入力していたら
		const bool bKeyboardMove = m_pMovement->MoveKeyboard(pKeyboard, fSpeed, &fAngleDest);
		const bool bJoypadMove = m_pMovement->MoveJoypad(pJoypad, fSpeed, &fAngleDest);

		// 移動ごとの処理ができるか判定
		const bool bPlayerMove = bKeyboardMove || bJoypadMove;

		// 移動ごとの処理ができるなら
		if (bPlayerMove)
		{
			m_pCharacter3D->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngleDest, 0.0f));

			// ダッシュモーションか歩きモーションかを判定
			int isDashMotion = (m_bDash ? TYPE_DASH : TYPE_MOVE);

			// ジャンプかjumpじゃないかを判定
			int motiontype = m_bJump ? isDashMotion : TYPE_JUMP;

			// モーションの設定
			m_pMotion->SetMotion(motiontype, true, 5);

			m_pCharacter3D->SetState(STATE::STATE_MOVE, 1);

			// 状態の変更
			ChangeState(make_shared<CPlayerMove>());

		}
		else
		{
			// モーションの種類の取得
			int motiontype = m_pMotion->GetBlendType();

			// 移動状態だったら
			if (motiontype == TYPE_MOVE || motiontype == TYPE_DASH && m_pMotion != nullptr)
			{
				m_pMotion->SetMotion(TYPE_NEUTRAL, true, 15);

				// 状態の変更
				ChangeState(make_shared<CPlayerNormal>());
			}
		}
	}

	// ダッシュボタンを押したら
	if ((pKeyboard->GetPress(DIK_LSHIFT) || pJoypad->GetPress(pJoypad->JOYKEY_RIGHT_SHOULDER)))
	{
		m_bDash = true;
	}
	else
	{
		m_bDash = false;
	}

	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	// 移動量の減衰
	m_pMove->SetInertia3D(0.25f);

	// 前回の位置の取得
	m_posOld = pos;

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 位置の更新
	pos += move;

	float fHeight = 0.0f;

	// メッシュフィールドの当たり判定
	if (pMesh != nullptr && pMesh->Collision(pos, &fHeight))
	{
		// 高さの設定
		pos.y = fHeight;

		// ジャンプできる
		m_bJump = true;
		
		// モーションがジャンプだったら
		if (m_pMotion->GetBlendType() == TYPE_JUMP)
		{
			// 着地モーションの再生
			m_pMotion->SetMotion(TYPE_LANDING, true, 5);

			// インパクトの設定
			CMeshCircle::Confing Circleconfig = { 0.0f,10.0f,10.0f,50.0f,30,true };

			// サークルを生成
			CMeshCircle::Create(Circleconfig, D3DCOLOR_RGBA(220, 220, 220, 200),pos,32);
		}
	}
	else
	{
		m_bJump = false;
	}

	// メッシュシリンダーの取得
	CMeshCylinder* pCylinder = CGame::GetCylinder();

	// シリンダーの判定
	if (pCylinder != nullptr && pCylinder->Collision(&pos))
	{
		// ここに処理があれば書く
	}

	// 障害物との当たり判定
	CollisionObstacle(&pos);

	// 最初の位置
	D3DXVECTOR3 firstPos = VEC3_NULL;

	// 衝撃波の位置
	D3DXVECTOR3 ImpactPos = VEC3_NULL;

	// インパクトのインデックス
	int ImpactIdx = -1;

	// インパクトとの判定
	const bool bCollision = pMesh->CollisionImpact(pos, 150.0f, CMeshFieldImpact::OBJ_PLAYER,&ImpactIdx, &firstPos,&ImpactPos);

	if (bCollision && m_pCharacter3D->GetState() == STATE::STATE_ACTION)
	{
		// 方向の設定
		D3DXVECTOR3 dir = firstPos - pos;

		float fAngle = GetTargetAngle(firstPos, pos);

		// 向きの設定
		m_pCharacter3D->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));

		// 右手の位置
		D3DXVECTOR3 playerHandR = GetPositionFromMatrix(m_apModel[5]->GetMatrixWorld());

		// モーションをダメージにする
		m_pMotion->SetMotion(TYPE_PARRY, true, 2);

		// パーティクルの生成
		CParticle3D::Create(playerHandR, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f), 240, 20.0f, 25, 2, 15.0f);

		// インパクトの設定
		CMeshCircle::Confing Circleconfig = { 50.0f,10.0f,0.0f,50.0f,30,false };

		// インパクトを生成
		CMeshCircle::Create(Circleconfig,D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f), playerHandR,32, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		// 再設定
		pMesh->ResetImpact(dir, CMeshFieldImpact::OBJ_PLAYER, playerHandR,D3DXCOLOR(1.0f,1.0f,0.5f,1.0f), ImpactIdx);
	}
	// インパクトの当たり判定
	else if (bCollision && m_pMotion->GetBlendType() != TYPE_DAMAGE)
	{
		// 吹き飛び処理
		BlowOff(ImpactPos, 50.0f, 10.0f);

		// モーションの設定
		ChangeState(make_shared<CPlayerDamage>(5));
	}
	

	// 重力を加算
	m_pMove->Gravity(-MAX_GRABITY);

	// ジャンプできるなら
	if ((pKeyboard->GetPress(DIK_SPACE) == true || pJoypad->GetPress(pJoypad->JOYKEY_A) == true) && m_bJump == true)
	{
		m_pMotion->SetMotion(TYPE_JUMP, true, 2);

		// 移動量を上方向に設定
		m_pMove->Jump(JUMP_HEIGHT);
		m_bJump = false;
	}

#ifdef _DEBUG

	if (pKeyboard->GetTrigger(DIK_B))
	{
		m_pScore->SetDestScore(100000,120);

		// 瓦礫を生成
		CRubble::Create(pos, D3DXVECTOR3(15.0f, 15.0f, 15.0f), 120,0);

		// スローモーションの取得
		CSlow* pSlow = CManager::GetSlow();
		pSlow->Start(540, 4);
	}

	if (pKeyboard->GetTrigger(DIK_V))
	{
		// 回避
		ChangeState(make_shared<CPlayerAvoid>(20.0f));
	}
#endif // _DEBUG

	// カウンター状態
	if (pKeyboard->GetTrigger(DIK_RETURN) && m_pMotion->GetBlendType() != TYPE_DAMAGE)
	{
		m_pMotion->SetMotion(TYPE_ACTION, true,6);
	
		// パリィの時間
		m_nParryTime = PARRY_TIME;
		m_nParryCounter = 0;

		m_pCharacter3D->SetState(m_pCharacter3D->STATE_ACTION, PARRY_TIME);
	}

	// ロックオン
	if (pKeyboard->GetTrigger(DIK_R) || pJoypad->GetTrigger(pJoypad->JOYKEY_RIGHT_THUMB))
	{
		CCamera::STATE state; // カメラの状態

		// カメラの状態を判定
		const bool rockon = pCamera->GetState() == CCamera::STATE_ROCKON;

		// ロックオンじゃなかったらロックオン
		state = rockon ? CCamera::STATE_TRACKING : CCamera::STATE_ROCKON;

		pCamera->SetState(state);
	}

	// 当たり判定の設定処理
	if (m_pSphere != nullptr)
	{
		// 位置の設定処理
		m_pSphere->SetPosition(pos);
	}

	// 視界判定
	if (m_pFOV != nullptr)
	{
		// 位置の設定
		m_pFOV->SetPosition(pos);
	}

	UpdateParry();

	// キャラクターの更新処理
	m_pCharacter3D->Update();

	// 位置の設定
	m_pCharacter3D->SetPosition(pos);

	if (m_pMotion != nullptr)
	{
		// モーションの更新処理
		m_pMotion->Update(&m_apModel[0], m_nNumModel);
	}

	// 状態制御がnullじゃなかったら
	if (m_pMachine != nullptr)
	{
		// 状態の更新処理
		m_pMachine->Update();
	}
	// 目的の視点に近づける
	m_pCharacter3D->GetRotation()->SetSmoothAngle(0.1f);

	D3DXVECTOR3 posRDest;

	D3DXVECTOR3 rot = m_pCharacter3D->GetRotation()->Get();

	D3DXVECTOR3 modelpos = math::GetPositionFromMatrix(m_apModel[2]->GetMatrixWorld());

	posRDest.x = modelpos.x + sinf(rot.y) * 1.0f;
	posRDest.y = (modelpos.y + 0.0f) + sinf(rot.y) * 1.0f;
	posRDest.z = modelpos.z + cosf(rot.y) * 1.0f;

	// 視点の設定
	D3DXVECTOR3 posVDest(pos.x, pos.y + 200.0f, pos.z);

	// カメラの追従処理
	pCamera->SetTracking(posVDest,posRDest,1.0f,0.1f);
}

//===================================================
// 描画処理
//===================================================
void CPlayer::Draw(void)
{
	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	//m_pMotion->Debug();
	// デバック
	CDebugProc::Print("プレイヤーの位置 X = %.2f Y = %.2f Z = %.2f\n", pos.x, pos.y,pos.z);

#if 0

	LPDIRECT3DSURFACE9 pRenderDef, pZBuffer;
	D3DVIEWPORT9 viepowtDef;
	D3DXMATRIX mtxViewDef, mtxProjectionDef;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 現在のレンダーターゲットの取得
	pDevice->GetRenderTarget(0, &pRenderDef);

	// 現在のZバッファの取得
	pDevice->GetDepthStencilSurface(&pZBuffer);

	// 現在のビューポートの取得
	pDevice->GetViewport(&viepowtDef);

	// 現在のビューマトリックスの取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxViewDef);

	// 現在のプロジェクションマトリックスの取得
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjectionDef);

	CCamera* pCamera = CManager::GetCamera();

	D3DXVECTOR3 posV, posR,vecU;

	posV = pCamera->GetPosV();

	posR = pCamera->GetPosR();

	vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// レンダリングターゲットの変更
	CManager::GetRenderer()->ChangeTarget(posV, posR, vecU);

	// レンダリングターゲット用テクスチャのクリア
	pDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 255, 255, 255), 1.0f, 0);
#endif

	// キャラクターの描画
	m_pCharacter3D->Draw();

	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		if (m_apModel[nCnt] != nullptr)
		{
			// 描画処理
			m_apModel[nCnt]->Draw();
		}
	}

#if 0

	// レンダーターゲットをもとに戻す
	pDevice->SetRenderTarget(0, pRenderDef);

	// Zバッファをもとに戻す
	pDevice->SetDepthStencilSurface(pZBuffer);

	// ビューポートをもとに戻す
	pDevice->SetViewport(&viepowtDef);

	pDevice->SetTransform(D3DTS_VIEW, &mtxViewDef);

	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjectionDef);

#endif // 0
}

//===================================================
// パリィの更新
//===================================================
void CPlayer::UpdateParry(void)
{
	// パリィのカウンター
	if (m_nParryCounter >= m_nParryTime)
	{
		m_nParryCounter = 0;
	}
	else
	{
		m_nParryCounter++;
	}
}

//===================================================
// パリィが成功したかどうか
//===================================================
int CPlayer::SuccessParry(const int nParfectTime)
{
	// 状態がアクションじゃなかったら抜ける
	if (m_pCharacter3D->GetState() != STATE::STATE_ACTION) return PARRY_MISS;

	// パーフェクトタイムまでの差分を求める
	int nDiff = abs(nParfectTime - m_nParryCounter);

	// パーフェクトだったら
	if (nDiff >= 0 && nDiff <= 5)
	{
		// 完璧
		return PARRY_PARFECT;
	}
	else if (nDiff > 5 && nDiff <= 10)
	{
		// 普通
		return PARRY_NORMAL;
	}
	else if (nDiff > 10 && nDiff <= m_nParryTime)
	{
		// 弱い
		return PARRY_WEAK;
	}
	
	return PARRY_MISS;
}

//===================================================
// 吹き飛び処理
//===================================================
void CPlayer::BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump)
{
	// 位置
	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	// アタッカーからプレイヤーまでの差分を求める
	D3DXVECTOR3 diff = pos - attacker;

	// 角度を求める
	float fAngle = atan2f(diff.x, diff.z);

	// 目的の角度の設定
	m_pCharacter3D->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

	// 移動量
	D3DXVECTOR3 move;

	// 移動量の設定
	move.x = sinf(fAngle) * blowOff;
	move.y = jump;
	move.z = cosf(fAngle) * blowOff;

	m_pMove->Set(move);
}

//===================================================
// 状態の変更
//===================================================
void CPlayer::ChangeState(std::shared_ptr<CPlayerState> pNewState)
{
	if (pNewState != nullptr)
	{
		// オーナの設定
		pNewState->SetOwner(this);
	}

	if (m_pMachine != nullptr)
	{
		// 状態の変更
		m_pMachine->Change(pNewState);
	}
}

//===================================================
// 向いている方向に進む処理
//===================================================
void CPlayer::MoveForward(const float fSpeed)
{
	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 今の向きの取得
	float forward = m_pCharacter3D->GetRotation()->Get().y;

	// 移動方向の計算
	move.x = sinf(forward + D3DX_PI) * fSpeed;
	move.z = cosf(forward + D3DX_PI) * fSpeed;

	// 移動量の設定
	m_pMove->Set(move);
}

//===================================================
// ヒット時の処理
//===================================================
void CPlayer::Hit(int nDamage)
{
	// 死んでいたら
	if (m_pCharacter3D->Hit(nDamage) == false)
	{
		CGame::SetState(CGame::STATE_END);
	}
}

//===================================================
// プレイヤーの視界の(パリィ時)判定
//===================================================
bool CPlayer::IsParry(const D3DXVECTOR3 pos)
{
	// 向きの取得
	D3DXVECTOR3 rot = m_pCharacter3D->GetRotation()->Get();

	// 視界の更新処理
	m_pFOV->UpdateData(rot.y);

	// 視界判定の取得
	CCollisionFOV* pCollision = CCollisionFOV::GetInstance();

	// 視界内かつ状態が攻撃の時
	if (m_pCharacter3D->GetState() == CCharacter3D::STATE_ACTION &&
		pCollision->Collision(pos, m_pFOV.get()) &&
		m_pMotion->GetBlendType() != TYPE_PARRY)
	{
		return true;
	}
	return false;
}

//===================================================
// 障害物との当たり判定
//===================================================
bool CPlayer::CollisionObstacle(D3DXVECTOR3 *pPos)
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

		D3DXVECTOR3 Size = { 50.0f,200.0f,50.0f };

		D3DXVECTOR3 center;

		center.x = pPos->x;
		center.y = pPos->y + Size.y * 0.5f;
		center.z = pPos->z;

		CColliderAABB aabb = CColliderAABB::CreateCollider(center,D3DXVECTOR3(m_posOld.x, m_posOld.y + Size.y * 0.5f, m_posOld.z), Size);

		// 当たっていたら
		if (pObstacle != nullptr && pObstacle->Collision(&aabb, pPos))
		{
			// ダメージ状態にする
			ChangeState(make_shared<CPlayerDamage>(3));

			return true;
		}
	}

	return false;
}

//===================================================
// 向きの設定
//===================================================
void CPlayer::SetAngle(const float angleY)
{
	m_pCharacter3D->GetRotation()->SetDest(D3DXVECTOR3(0.0f, angleY, 0.0f));
}

//===================================================
// 生成処理
//===================================================
CPlayer* CPlayer::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = nullptr;

	// 3Dオブジェクトの生成
	pPlayer = new CPlayer;

	if (pPlayer == nullptr) return nullptr;

	pPlayer->Init();
	pPlayer->m_pCharacter3D->Init();
	// 初期化処理
	pPlayer->m_pCharacter3D->SetPosition(pos);
	pPlayer->m_pCharacter3D->GetRotation()->Set(rot);

	return pPlayer;
}

//===================================================
// プレイヤーのロード
//===================================================
void CPlayer::Load(void)
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
			if (line.find("PLAYER_MOTION_FILE") != string::npos)
			{
				size_t equal_pos = line.find("="); // =の位置

				// [=] から先を求める
				input = line.substr(equal_pos + 1);

				// ファイルの名前を取得
				string file_name = pLoadManager->GetString(input);

				// ファイルの名前を代入
				const char* FILE_NAME = file_name.c_str();

				// モーションのロード処理
				m_pMotion = CMotion::Load(FILE_NAME, m_apModel, &m_nNumModel, TYPE_MAX, CMotion::LOAD_TEXT);
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
// コンストラクタ
//===================================================
CPlayerMovement::CPlayerMovement()
{
}

//===================================================
// デストラクタ
//===================================================
CPlayerMovement::~CPlayerMovement()
{
}

//===================================================
// 初期化処理
//===================================================
void CPlayerMovement::Init(CVelocity* pMove)
{
	m_pMove = pMove;
}

//===================================================
// キーボードの処理
//===================================================
bool CPlayerMovement::MoveKeyboard(CInputKeyboard* pKeyboard,const float fSpeed, float* pRotDest)
{
	bool bMove = false;

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// 移動量
	D3DXVECTOR3 move = m_pMove->Get();

	if (pKeyboard->GetPress(DIK_A))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W) == true)
		{
			move.x += sinf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;
			move.z += cosf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;

			*pRotDest = cameraRot.y + D3DX_PI * 0.75f;

			bMove = true;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;

			*pRotDest = cameraRot.y + D3DX_PI * 0.25f;

			bMove = true;
		}
		// プレイヤーの移動(左)
		else
		{
			move.z += sinf(cameraRot.y) * fSpeed;
			move.x -= cosf(cameraRot.y) * fSpeed;

			*pRotDest = cameraRot.y + D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//プレイヤーの移動(右)
	else if (pKeyboard->GetPress(DIK_D))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W))
		{
			move.x += sinf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;
			move.z += cosf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;

			*pRotDest = cameraRot.y - D3DX_PI * 0.75f;

			bMove = true;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;

			*pRotDest = cameraRot.y - D3DX_PI * 0.25f;

			bMove = true;
		}
		// プレイヤーの移動(右)
		else
		{
			move.z -= sinf(cameraRot.y) * fSpeed;
			move.x += cosf(cameraRot.y) * fSpeed;

			*pRotDest = cameraRot.y - D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//プレイヤーの移動(上)
	else if (pKeyboard->GetPress(DIK_W) == true)
	{
		move.x += sinf(cameraRot.y) * fSpeed;
		move.z += cosf(cameraRot.y) * fSpeed;

		*pRotDest = cameraRot.y + D3DX_PI;

		bMove = true;
	}
	//プレイヤーの移動(下)
	else if (pKeyboard->GetPress(DIK_S) == true)
	{
		move.x -= sinf(cameraRot.y) * fSpeed;
		move.z -= cosf(cameraRot.y) * fSpeed;

		*pRotDest = cameraRot.y;

		bMove = true;
	}

	// 移動量の設定
	m_pMove->Set(move);

	return bMove;
}

//===================================================
// パッドの処理
//===================================================
bool CPlayerMovement::MoveJoypad(CInputJoypad* pJoypad, const float fSpeed, float* pRotDest)
{
	bool bMove = false;

	XINPUT_STATE* pStick;

	pStick = pJoypad->GetJoyStickAngle();

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// Lスティックの角度
	float LStickAngleY = pStick->Gamepad.sThumbLY;
	float LStickAngleX = pStick->Gamepad.sThumbLX;

	// デッドゾーン
	float deadzone = 32767.0f * 0.25f;

	// スティックの傾けた角度を求める
	float magnitude = sqrtf((LStickAngleX * LStickAngleX) + (LStickAngleY * LStickAngleY));

	// 動かせる
	if (magnitude > deadzone)
	{
		bMove = true;

		// アングルを正規化
		float normalizeX = (LStickAngleX / magnitude);
		float normalizeY = (LStickAngleY / magnitude);

		// プレイヤーの移動量
		float moveX = normalizeX * cosf(-cameraRot.y) - normalizeY * sinf(-cameraRot.y);
		float moveZ = normalizeX * sinf(-cameraRot.y) + normalizeY * cosf(-cameraRot.y);

		// 移動量をスティックの角度によって変更
		float speedWk = fSpeed * ((magnitude - deadzone) / (32767.0f - deadzone));

		// 移動量
		D3DXVECTOR3 moveWk = VEC3_NULL;

		// プレイヤーの移動
		moveWk.x += moveX * speedWk;
		moveWk.z += moveZ * speedWk;

		// 移動量の設定
		m_pMove->Set(moveWk);

		// プレイヤーの角度を移動方向にする
		float fDestAngle = atan2f(-moveX, -moveZ);

		*pRotDest = fDestAngle;
	}

	return bMove;
}
