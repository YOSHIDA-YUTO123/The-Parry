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
#include"Observer.h"
#include"Gage.h"
#include "EffectAnim.h"
#include "MoveSmoke.h"
#include"Orbit.h"

using namespace math; // 名前空間mathを使用
using namespace std;  // 名前空間をstdを使用する
using namespace Const;							// 名前空間Constを使用する

//using MOTION = CPlayerMotionController::TYPE; // 列挙型を使用する
using STATE = CCharacter3D::STATE;			  // キャラクターの状態

constexpr float JUMP_HEIGHT = 25.0f;		// ジャンプ量
constexpr float SHADOW_SIZE = 50.0f;		// 影の大きさ
constexpr float SHADOW_MAX_HEIGHT = 500.0f; // 影が見える最大の高さ
constexpr float SHADOW_A_LEVEL = 0.9f;		// 影のアルファ値のオフセット
constexpr int PARRY_TIME = 15;				// パリィの有効時間
constexpr int ATTACK_TIME = 120;			// 攻撃の有効時間
constexpr float AVOID_STAMINA = 30.0f;		// 回避に使用するスタミナ

//===================================================
// コンストラクタ
//===================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	m_pMotion = nullptr;				// モーションのクラスへのポインタ
	m_pCharacter3D = nullptr;			// キャラクタークラス
	m_pMachine = nullptr;				// ステートマシーン
	m_nNumModel = NULL;					// モデルの最大数
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

	// キャラクターの初期化処理
	m_pCharacter3D->Init();

	// キャラクターの設定処理
	m_pCharacter3D->SetCharacter(MAX_LIFE, 6.0f,D3DXVECTOR3(3.0f,1.0f,3.5f));

	// 状態制御の生成
	m_pMachine = make_unique<CStateMachine>();

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

	if (m_pMotion != nullptr)
	{
		// モーションの終了処理
		m_pMotion->Uninit();
	}

	if (m_pCharacter3D != nullptr)
	{
		// キャラクターの破棄
		m_pCharacter3D->Uninit();
	}

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

	// カメラの取得処理
	CCamera* pCamera = CManager::GetCamera();

	// 読み込めていなかったら
	if (m_pMotion->IsLoad() == false)
	{
		return;
	}
	
	// ヒットストップ状態だったら
	if (m_pCharacter3D->HitStop())
	{
		// 更新を止める
		return;
	}

	// 位置の取得
	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

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

	if (m_pCharacter3D != nullptr)
	{
		// キャラクターの更新処理
		m_pCharacter3D->Update();

		// 目的の視点に近づける
		m_pCharacter3D->GetRotation()->SetSmoothAngle(0.1f);
	}

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

	D3DXVECTOR3 posRDest;

	D3DXVECTOR3 rot = m_pCharacter3D->GetRotation()->Get();

	D3DXVECTOR3 modelpos = GetPositionFromMatrix(m_apModel[1]->GetMatrixWorld());

	posRDest.x = modelpos.x + sinf(rot.y) * 1.0f;
	posRDest.y = (modelpos.y + 0.0f) + sinf(rot.y) * 1.0f;
	posRDest.z = modelpos.z + cosf(rot.y) * 1.0f;

	// 視点の設定
	D3DXVECTOR3 posVDest(modelpos);

	// カメラの追従処理
	pCamera->SetTracking(posVDest,posRDest,0.1f,CCamera::TRACKOBJ_PLAYER);
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
// 状態の変更
//===================================================
void CPlayer::ChangeState(std::shared_ptr<CPlayerState> pNewState)
{
	if (pNewState != nullptr)
	{
		// オーナの設定
		pNewState->SetOwner(this,m_pCharacter3D.get());
	}

	if (m_pMachine != nullptr)
	{
		// 状態の変更
		m_pMachine->Change(pNewState);
	}
}

//===================================================
// モーションの更新処理
//===================================================
void CPlayer::UpdateMotion(void)
{
	if (m_pMotion != nullptr)
	{
		// モーションの更新処理
		m_pMotion->Update(&m_apModel[0], m_nNumModel);
	}
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
void CPlayerMovement::Init(CVelocity* pMove,CRotation* pRot)
{
	m_pMove = pMove;
	m_pRot = pRot;
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

	CInputJoypad* pJoyPad = CManager::GetInputJoypad();

	if (pJoyPad->GetJoyStickL())
	{
		return false;
	}

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
		D3DXVECTOR3 moveWk = m_pMove->Get();

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

//===================================================
// 向いている方向に移動する処理
//===================================================
void CPlayerMovement::MoveForward(const float fSpeed)
{
	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 今の向きの取得
	float forward = m_pRot->Get().y;

	// 移動方向の計算
	move.x = sinf(forward + D3DX_PI) * fSpeed;
	move.z = cosf(forward + D3DX_PI) * fSpeed;

	// 移動量の設定
	m_pMove->Set(move);
}

//===================================================
// コンストラクタ(ゲーム中の処理)
//===================================================
CPlayerGame::CPlayerGame()
{
	m_pMovement = nullptr;	
	m_pFOV = nullptr;			
	m_pSphere = nullptr;		
	m_pMove = nullptr;		
	m_posOld = VEC3_NULL;		
	m_nParryTime = NULL;	
	m_nParryCounter = NULL;
	m_bJump = false;		
	m_bDash = false;
	m_pHpObserver = nullptr;
	m_pStaminaObserver = nullptr;
	m_pOrbit = nullptr;
	m_nAttackCounter = NULL;
	m_fStamina = NULL;
}

//===================================================
// デストラクタ(ゲーム中の処理)
//===================================================
CPlayerGame::~CPlayerGame()
{
}

//===================================================
// 初期化処理(ゲーム中の処理)
//===================================================
HRESULT CPlayerGame::Init(void)
{
	// プレイヤーの初期化
	CPlayer::Init();

	// キャラクターの取得
	auto pCharacter = CPlayer::GetCharacter();

	// 位置の取得
	D3DXVECTOR3 pos = pCharacter->GetPosition();

	// 視界判定の生成
	m_pFOV = CColliderFOV::Create(pos, 0.0f, D3DX_PI * 0.5f, -D3DX_PI * 0.5f,1000.0f);

	// 円の当たり判定の生成
	m_pSphere = CColliderSphere::Create(pos, 50.0f);

	// 移動クラスの生成
	m_pMove = make_unique<CVelocity>();

	m_pMovement = make_unique<CPlayerMovement>();

	m_pMovement->Init(m_pMove.get(),pCharacter->GetRotation());

	D3DXVECTOR3 Size = { 50.0f,200.0f,50.0f };

	// コライダーの生成
	m_pAABB = CColliderAABB::Create(pos,m_posOld, Size);

	// スタミナを設定
	m_fStamina = MAX_STAMINA;

	return S_OK;
}

//===================================================
// 終了処理(ゲーム中の処理)
//===================================================
void CPlayerGame::Uninit(void)
{
	m_pFOV = nullptr;
	m_pSphere = nullptr;
	m_pMove = nullptr;
	m_pMovement = nullptr;
	m_pOrbit = nullptr;
	m_pAABB = nullptr;

	// HPオブザーバーの破棄
	if (m_pHpObserver != nullptr)
	{
		delete m_pHpObserver;
		m_pHpObserver = nullptr;
	}

	// Staminaオブザーバーの破棄
	if (m_pStaminaObserver != nullptr)
	{
		delete m_pStaminaObserver;
		m_pStaminaObserver = nullptr;
	}

	// プレイヤーの破棄
	CPlayer::Uninit();
}

//===================================================
// 更新処理(ゲーム中の処理)
//===================================================
void CPlayerGame::Update(void)
{
	// キャラクターの取得
	auto pCharacter = CPlayer::GetCharacter();

	// モーションの取得
	auto pMotion = CPlayer::GetMotion();

	// キャラクターがなかったら処理しない
	if (pCharacter == nullptr) return;

	// モーションがなかったら処理しない
	if (pMotion == nullptr) return;

	// プレイヤーの更新処理
	CPlayer::Update();

	// 生きてるかどうか
	bool bAlive = pCharacter->GetAlive();

	// プレイヤーが死んだら
	if (bAlive == false)
	{
		// ゲームを終了
		CGame::SetState(CGame::STATE_END);
	}

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// コントローラーの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// マウスの取得
	CInputMouse* pMouse = CManager::GetInputMouse();

	// メッシュフィールドの取得
	CMeshField* pMesh = CGame::GetField();

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	if (pCharacter->HitStop())
	{
		return;
	}

#ifdef _DEBUG

	// デバッグ表示
	CDebugProc::Print("カメラの回転 X = %.2f Y = %.2f\n", pCamera->GetRotaition().x, pCamera->GetRotaition().y);

#endif // _DEBUG

	// 速さ
	float fSpeed = m_bDash ? pCharacter->GetSpeed() : 1.5f;

	float fAngleDest = 0.0f;


	// 移動できるなら
	if (IsMove(pMotion) && bAlive)
	{
		// 移動を入力していたら
		const bool bKeyboardMove = m_pMovement->MoveKeyboard(pKeyboard, fSpeed, &fAngleDest);
		const bool bJoypadMove = m_pMovement->MoveJoypad(pJoypad, fSpeed, &fAngleDest);

		// 移動ごとの処理ができるか判定
		const bool bPlayerMove = bKeyboardMove || bJoypadMove;

		// 移動ごとの処理ができるなら
		if (bPlayerMove)
		{
			pCharacter->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngleDest, 0.0f));

			// ダッシュモーションか歩きモーションかを判定
			int isDashMotion = (m_bDash ? TYPE_DASH : TYPE_MOVE);

			// ジャンプかjumpじゃないかを判定
			int motiontype = m_bJump ? isDashMotion : TYPE_JUMP;

			// フレームを設定
			const int nFrame = m_bDash ? 5 : 10;

			// モーションの設定
			pMotion->SetMotion(motiontype, true, nFrame);

			// 移動状態にする
			pCharacter->SetState(STATE::STATE_MOVE, 1);

			// ダッシュ状態だったら
			if (motiontype == TYPE::TYPE_DASH)
			{
				// 状態の変更
				ChangeState(make_shared<CPlayerDash>());
			}
			else
			{
				// 状態の変更
				ChangeState(make_shared<CPlayerMove>());
			}
		}
		else
		{
			// モーションの種類の取得
			int motiontype = pMotion->GetBlendType();

			// 移動状態だったら
			if (motiontype == TYPE_MOVE || motiontype == TYPE_DASH && pMotion != nullptr)
			{
				pMotion->SetMotion(TYPE_NEUTRAL, true, 15);

				// 状態の変更
				ChangeState(make_shared<CPlayerNormal>());
			}
		}
	}

	// ダッシュボタンを押したら
	if ((pKeyboard->GetPress(DIK_LSHIFT) || pJoypad->GetPress(pJoypad->JOYKEY_LEFT_SHOULDER)))
	{
		m_bDash = true;
	}
	else
	{
		m_bDash = false;
	}

	D3DXVECTOR3 pos = pCharacter->GetPosition();

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
		if (pMotion->GetBlendType() == TYPE_JUMP)
		{
			// 着地モーションの再生
			pMotion->SetMotion(TYPE_LANDING, true, 5);

			// サークルを生成
			auto pCircle = CMeshCircle::Create(D3DCOLOR_RGBA(220, 220, 220, 200), D3DXVECTOR3(pos.x,pos.y + 3.0f,pos.z), 0.0f, 50.0f, 32);

			// サークルの設定
			pCircle->SetCircle(0.0f, 10.0f, 30, true);
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

	if (pMotion->IsEventFrame(24, 24, TYPE_ROUNDKICK))
	{
		// 軌跡のリセット
		DeleteOrbit();
	}
	else if (pMotion->IsEventFrame(25,35, TYPE_ROUNDKICK))
	{
		// 軌跡の設定
		Orbit(32, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));
	}

	// 障害物との当たり判定
	CollisionObstacle(&pos);

	// インパクトの当たり判定
	CollisionImpact(pMesh, &pos, pCharacter, pMotion);

	// 重力を加算
	m_pMove->Gravity(-MAX_GRABITY);

	// ジャンプできるなら
	if ((pKeyboard->GetPress(DIK_SPACE) == true || pJoypad->GetPress(pJoypad->JOYKEY_A) == true) && m_bJump == true)
	{
		// 生きてるなら
		if (bAlive)
		{
			pMotion->SetMotion(TYPE_JUMP, true, 2);

			// 移動量を上方向に設定
			m_pMove->Jump(JUMP_HEIGHT);
			m_bJump = false;
		}
	}

	// 回避ボタンを押したかつ生きているなら
	if ((pMouse->OnMouseTriggerDown(1) || pJoypad->GetTrigger(pJoypad->JOYKEY_B)) && bAlive)
	{
		// 回避できるなら
		if (IsAvoid(pMotion))
		{
			// スタミナの設定
			SetStamina(-AVOID_STAMINA);

			// 向きを設定
			SetMoveAngle(pCamera, pKeyboard, pJoypad, pCharacter);

			// 回避
			ChangeState(make_shared<CPlayerAvoid>(20.0f));
		}
	}
	
#ifdef _DEBUG

#endif // _DEBUG

	// カウンター状態
	if ((pMouse->OnMouseTriggerDown(0) || pJoypad->GetTrigger(pJoypad->JOYKEY_X)))
	{
		// 構えが出せるなら
		if (IsStance(pMotion) && bAlive)
		{
			pMotion->SetMotion(TYPE_STANCE, true, 5);

			// パリィの時間
			m_nParryTime = PARRY_TIME;
			m_nParryCounter = 0;

			pCharacter->SetState(pCharacter->STATE_ACTION, PARRY_TIME);
		}
	}

	// ズームインだったら解除
	if (pMotion->GetBlendType() != TYPE_PARRY && pCamera->GetState() == CCamera::STATE_ZOOMIN)
	{
		// カメラのズーム解除
		pCamera->ResetState();
	}

	// 反撃
	if (pKeyboard->GetTrigger(DIK_F) && m_nAttackCounter >= 0)
	{
		m_nAttackCounter = 0;

		// 回し蹴り状態を設定
		ChangeState(make_shared<CPlayerRoundKick>());
	}

	// 視界判定
	if (m_pFOV != nullptr)
	{
		// 位置の設定
		m_pFOV->SetPosition(pos);
	}

	// スタミナの更新処理
	UpdateStamina();

	// オブザーバーへの通知処理
	Notify();

	// パリィの更新処理
	UpdateParry();

	// 位置の設定
	pCharacter->SetPosition(pos);

	// コライダーの更新
	UpdateCollider(pos);
}

//===================================================
// 描画処理(ゲーム中の処理)
//===================================================
void CPlayerGame::Draw(void)
{
	// プレイヤーの描画処理
	CPlayer::Draw();
}

//===================================================
// プレイヤーの取得
//===================================================
CPlayerGame* CPlayerGame::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	// プレイヤーの生成
	auto pPlayer = new CPlayerGame;

	// プレイヤーの初期化
	pPlayer->Init();
	
	// キャラクターの取得
	auto pCharacter = pPlayer->GetCharacter();

	pCharacter->SetPosition(pos);
	pCharacter->GetRotation()->Set(rot);

	return pPlayer;
}

//===================================================
// パリィの更新処理
//===================================================
void CPlayerGame::UpdateParry(void)
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

	// 0以上だったら
	if (m_nAttackCounter >= 0)
	{
		// 攻撃カウンターを減らす
		m_nAttackCounter--;
	}
}

//===================================================
// 移動方向の設定処理
//===================================================
void CPlayerGame::SetMoveAngle(CCamera* pCamera, CInputKeyboard* pKeyboard, CInputJoypad* pJoypad,CCharacter3D *pCaracter)
{
	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	if (pJoypad->GetJoyStickL())
	{
		return;
	}

	// 角度の取得
	D3DXVECTOR3 Angle = CPlayer::GetRotaition();

	if (pKeyboard->GetPress(DIK_A))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W) == true)
		{
			Angle.y = cameraRot.y + D3DX_PI * 0.75f;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			Angle.y = cameraRot.y + D3DX_PI * 0.25f;
		}
		// プレイヤーの移動(左)
		else
		{
			Angle.y = cameraRot.y + D3DX_PI * 0.5f;
		}
	}
	//プレイヤーの移動(右)
	else if (pKeyboard->GetPress(DIK_D))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W))
		{
			Angle.y = cameraRot.y - D3DX_PI * 0.75f;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			Angle.y = cameraRot.y - D3DX_PI * 0.25f;
		}
		// プレイヤーの移動(右)
		else
		{
			Angle.y = cameraRot.y - D3DX_PI * 0.5f;
		}
	}
	//プレイヤーの移動(上)
	else if (pKeyboard->GetPress(DIK_W) == true)
	{
		Angle.y = cameraRot.y + D3DX_PI;
	}
	//プレイヤーの移動(下)
	else if (pKeyboard->GetPress(DIK_S) == true)
	{
		Angle.y = cameraRot.y;
	}

	// 向きの設定
	pCaracter->GetRotation()->Set(Angle);
	pCaracter->GetRotation()->SetDest(Angle);
}

//===================================================
// コライダーの更新処理
//===================================================
void CPlayerGame::UpdateCollider(D3DXVECTOR3 pos)
{
	// 当たり判定の設定処理
	if (m_pSphere != nullptr)
	{
		// 位置の設定処理
		m_pSphere->SetPosition(pos);
	}

	if (m_pAABB != nullptr)
	{
		// データの取得
		auto dataAABB = m_pAABB->GetData();

		// 大きさの取得
		float fSizeY = dataAABB.Size.y * 0.5f;

		// コライダーの更新処理
		m_pAABB->UpdateData(D3DXVECTOR3(pos.x, pos.y + fSizeY, pos.z), D3DXVECTOR3(m_posOld.x, m_posOld.y + fSizeY, m_posOld.z));
	}
}

//===================================================
// スタミナの更新処理
//===================================================
void CPlayerGame::UpdateStamina(void)
{
	m_fStamina += 0.1f;

	// 範囲制限する
	m_fStamina = Clamp(m_fStamina, 0.0f, MAX_STAMINA);
}

////===================================================
//// オブザーバーの設定処理
////===================================================
//template <class T> void CPlayerGame::SetObserver(CObserver<T>* pObserver, const OBSERVER type)
//{
//}

//===================================================
// パリィの成功度の取得
//===================================================
int CPlayerGame::SuccessParry(void)
{
	// キャラクターの取得
	auto pCharacter = CPlayer::GetCharacter();

	// キャラクターが無かったら処理しない
	if (pCharacter == nullptr) return PARRY_MISS;

	// 状態がアクションじゃなかったら抜ける
	if (pCharacter->GetState() != STATE::STATE_ACTION) return PARRY_MISS;

	//// パーフェクトタイムまでの差分を求める
	//int nDiff = abs(nParfectTime - m_nParryCounter);

	// 攻撃の有効時間を設定
	m_nAttackCounter = ATTACK_TIME;

	// パーフェクトだったら
	if (m_nParryCounter >= 0 && m_nParryCounter <= 3)
	{
		// 完璧
		return PARRY_PARFECT;
	}
	else if (m_nParryCounter > 3 && m_nParryCounter <= 10)
	{
		// 普通
		return PARRY_NORMAL;
	}
	else if (m_nParryCounter > 10 && m_nParryCounter <= m_nParryTime)
	{
		// 弱い
		return PARRY_WEAK;
	}

	return PARRY_MISS;
}

//===================================================
// 吹き飛び処理
//===================================================
void CPlayerGame::BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump)
{
	// キャラクターの取得
	auto pCharacter = CPlayer::GetCharacter();

	// キャラクターが無かったら処理しない
	if (pCharacter == nullptr) return;

	// 位置
	D3DXVECTOR3 pos = pCharacter->GetPosition();

	// アタッカーからプレイヤーまでの差分を求める
	D3DXVECTOR3 diff = pos - attacker;

	// 角度を求める
	float fAngle = atan2f(diff.x, diff.z);

	// 目的の角度の設定
	pCharacter->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

	// 移動量
	D3DXVECTOR3 move;

	// 移動量の設定
	move.x = sinf(fAngle) * blowOff;
	move.y = jump;
	move.z = cosf(fAngle) * blowOff;

	m_pMove->Set(move);
}

//===================================================
// パリィできるか判定
//===================================================
bool CPlayerGame::IsParry(const D3DXVECTOR3 pos)
{
	// キャラクターの取得
	auto pCharacter = CPlayer::GetCharacter();

	// モーションの取得
	auto pMotion = CPlayer::GetMotion();

	// キャラクターが無かったら処理しない
	if (pCharacter == nullptr) return false;

	// モーションが無かったら処理しない
	if (pMotion == nullptr) return false;

	// 向きの取得
	D3DXVECTOR3 rot = pCharacter->GetRotation()->Get();

	// 視界の更新処理
	m_pFOV->UpdateData(rot.y);

	// 視界判定の取得
	CCollisionFOV* pCollision = CCollisionFOV::GetInstance();

	// 視界内かつ状態が攻撃の時
	if (pCharacter->GetState() == CCharacter3D::STATE_ACTION &&
		pCollision->Collision(pos, m_pFOV.get()) &&
		pMotion->GetBlendType() != TYPE_PARRY)
	{
		return true;
	}

	return false;
}

//===================================================
// 角度の設定処理
//===================================================
void CPlayerGame::SetAngle(const float angleY)
{
	// キャラクターの取得
	auto pCharacter = CPlayer::GetCharacter();

	// キャラクターが無かったら処理しない
	if (pCharacter == nullptr) return;

	// 角度の設定
	pCharacter->GetRotation()->SetDest(D3DXVECTOR3(0.0f, angleY, 0.0f));
}

//===================================================
// 障害物との当たり判定
//===================================================
bool CPlayerGame::CollisionObstacle(D3DXVECTOR3* pPos)
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

		// コライダーを更新する
		UpdateCollider(*pPos);

		// 当たっていたら
		if (pObstacle != nullptr && pObstacle->Collision(m_pAABB.get(), pPos))
		{
			// ダメージ状態にする
			ChangeState(make_shared<CPlayerDamage>(3));

			return true;
		}
	}
	return false;
}

//===================================================
// 軌跡の処理
//===================================================
void CPlayerGame::Orbit(const int nSegH, const D3DXCOLOR col)
{
	// 武器の先端の位置
	D3DXVECTOR3 Top = GetModelPos(9);
	D3DXVECTOR3 Bottom = GetModelPos(11);

	// 軌跡の生成
	if (m_pOrbit == nullptr)
	{
		m_pOrbit = CMeshOrbit::Create(Top, Bottom, nSegH, col);
	}

	// 軌跡が使われていて寿命が無かったら
	if (m_pOrbit != nullptr)
	{
		m_pOrbit->SetPosition(Top, Bottom);
	}
}

//===================================================
// 軌跡のリセット
//===================================================
void CPlayerGame::DeleteOrbit(void)
{
	if (m_pOrbit != nullptr)
	{
		m_pOrbit->Uninit();
		m_pOrbit = nullptr;
	}
}

//===================================================
// 構えモーションの設定
//===================================================
void CPlayerGame::SetStance(void)
{
	D3DXVECTOR3 rot = CPlayer::GetRotaition();

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// ズームインの処理
	pCamera->SetZoomIn(60, rot.y + D3DX_PI);

	// モーションの取得
	auto pMotion = GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(TYPE_PARRY, false, 0);

		// モーションを更新してポーズを設定
		CPlayer::UpdateMotion();
	}
}

//===================================================
// スタミナの設定
//===================================================
void CPlayerGame::SetStamina(const float fStamina)
{
	m_fStamina += fStamina;
}

//===================================================
// インパクトの当たり判定
//===================================================
void CPlayerGame::CollisionImpact(CMeshField* pMeshField, D3DXVECTOR3* pPos,CCharacter3D * pCharacter,CMotion*pMotion)
{
	// nullだったら処理しない
	if (pMeshField == nullptr) return;

	// 最初の位置
	D3DXVECTOR3 firstPos = VEC3_NULL;

	// 衝撃波の位置
	D3DXVECTOR3 ImpactPos = VEC3_NULL;

	// インパクトとの判定
	const bool bCollision = pMeshField->CollisionImpact(*pPos, 150.0f, CMeshFieldImpact::OBJ_PLAYER, &firstPos, &ImpactPos);

	if (bCollision && pCharacter->GetState() == STATE::STATE_ACTION)
	{
		// 方向の設定
		D3DXVECTOR3 dir = firstPos - *pPos;

		float fAngle = GetTargetAngle(firstPos, *pPos);

		// 向きの設定
		pCharacter->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));

		// 右手の位置
		D3DXVECTOR3 playerHandR = GetModelPos(5);

		// モーションをダメージにする
		pMotion->SetMotion(TYPE_PUNCH, true, 2);

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(playerHandR, 20.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// パーティクルの設定処理
		pParticle->SetParticle(15.0f, 240, 25, 2);

		// インパクトを生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f), playerHandR, 0.0f, 50.0f, 32);

		// サークルの設定処理
		pCircle->SetCircle(50.0f, 10.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		// 再設定
		pMeshField->ResetImpact(dir, CMeshFieldImpact::OBJ_PLAYER, playerHandR, D3DXCOLOR(1.0f, 1.0f, 0.5f, 1.0f));
	}
	// インパクトの当たり判定
	else if (bCollision && pMotion->GetBlendType() != TYPE_DAMAGE)
	{
		// 吹き飛び処理
		BlowOff(ImpactPos, 50.0f, 10.0f);

		// モーションの設定
		ChangeState(make_shared<CPlayerDamage>(5));
	}
}

//===================================================
// 移動できるか判定
//===================================================
bool CPlayerGame::IsMove(CMotion *pMotion)
{
	// モーションの種類
	int motiontype = pMotion->GetBlendType();

	// ダメージ状態だったら移動できない
	if (motiontype == TYPE_DAMAGE) return false;

	// 回避状態だったら移動できない
	if (motiontype == TYPE_AVOID) return false;

	// カウンター状態だったら移動できない
	if (pMotion->IsEventFrame(1, 40, TYPE_PARRY)) return false;

	// パリィだったら移動できない
	if (motiontype == TYPE_PUNCH) return false;

	// 反撃状態だったら移動できない
	if (motiontype == TYPE_ROUNDKICK) return false;

	// 構え状態だったら移動できない
	if (pMotion->IsEventFrame(1,35, TYPE_STANCE)) return false;

	// 移動できる
	return true;
}

//===================================================
// 構えを出せるか判定
//===================================================
bool CPlayerGame::IsStance(CMotion *pMotion)
{
	// モーションの種類
	int motiontype = pMotion->GetBlendType();

	// ダメージモーションだったら
	if (motiontype == TYPE_DAMAGE) return false;
	
	// 構え状態だったら
	if (motiontype == TYPE_STANCE) return false;

	// 回避状態だったら
	if (motiontype == TYPE_AVOID) return false;

	return true;
}

//===================================================
// 回避を出せるか判定
//===================================================
bool CPlayerGame::IsAvoid(CMotion* pMotion)
{
	// 反撃受付時間は回避できない
	if (pMotion->IsEventFrame(1, m_nParryTime, TYPE_STANCE)) return false;

	// 反撃モーションの時一定時間回避できない
	if (pMotion->IsEventFrame(1, 15, TYPE_PARRY)) return false;

	// 回避モーションの時回避できない
	if (pMotion->GetBlendType() == TYPE_AVOID) return false;

	// スタミナが消費分無かったら
	if (m_fStamina < AVOID_STAMINA) return false;

	return true;
}

//===================================================
// 通知処理
//===================================================
void CPlayerGame::Notify(void)
{
	// キャラクターの取得
	auto pCharacter = CPlayer::GetCharacter();

	// キャラクターが無かったら処理しない
	if (pCharacter == nullptr) return;

	if (m_pHpObserver != nullptr)
	{
		// HPの取得
		int nLife = pCharacter->GetLife();

		// HPの変化を通知する
		m_pHpObserver->OnNotify(nLife);
	}

	if (m_pStaminaObserver != nullptr)
	{
		// スタミナの変化を通知する
		m_pStaminaObserver->OnNotify(m_fStamina);
	}
}