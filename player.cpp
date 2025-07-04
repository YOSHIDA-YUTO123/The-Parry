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

using namespace math; // 名前空間mathを使用
using namespace std;  // 名前空間をstdを使用する

using MOTION = CPlayerMotionController::TYPE; // 列挙型を使用する

//***************************************************
// マクロ定義
//***************************************************
#define PLAYER_JUMP_HEIGHT (25.0f)  // ジャンプ量
#define SHADOW_SIZE (50.0f)			// 影の大きさ
#define SHADOW_MAX_HEIGHT (500.0f)  // 影が見える最大の高さ
#define SHADOW_A_LEVEL (0.9f)       // 影のアルファ値のオフセット

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
	// プレイヤーのモーション制御クラスの生成
	m_pMotion = make_unique<CPlayerMotionController>();

	// プレイヤーのロード処理
	m_pMotion->Load(m_apModel,&m_nNumModel);

	// キャラクタークラスの生成
	m_pCharacter3D = make_unique<CCharacter3D>();

	// キャラクターの設定処理
	m_pCharacter3D->SetCharacter(10, 6.0f);

	// キャラクターの位置の取得
	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	// 視界判定の生成
	m_pFOV = CCollisionFOV::Create(pos, 1000.0f);

	// スコアの生成
	m_pScore = (CScoreLerper*)CScore::Create(CScore::TYPE_LERPER,D3DXVECTOR3(1150.0f, 50.0f, 0.0f), 180.0f, 30.0f);

	// 影の生成
	m_pShadow = CShadow::Create(VEC3_NULL, 50.0f, 50.0f, WHITE);

	// 移動クラスの生成
	m_pMove = new CVelocity;

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
		delete m_pMove;
		m_pMove = nullptr;
	}

	// モーションの終了処理
	m_pMotion->Uninit();

	// 影クラスの破棄
	m_pShadow->Uninit();

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
	CMeshField* pMesh = CManager::GetMeshField();

	// カメラの取得処理
	CCamera* pCamera = CManager::GetCamera();

	// 読み込めていなかったら
	if (m_pMotion->IsLoad() == false)
	{
		return;
	}

	// 移動処理
	if (pJoypad->GetJoyStickL() == true)
	{
		// パッドの移動処理
		CPlayer::MoveJoypad(pJoypad);
	}
	// キーボードの移動処理
	else if(CPlayer::MoveKeyboard(pKeyboard))
	{
		// ダッシュモーションか歩きモーションかを判定
		int isDashMotion = (m_bDash ? MOTION::TYPE_DASH : MOTION::TYPE_MOVE);

		// ジャンプかjumpじゃないかを判定
		int motiontype = m_bJump ? isDashMotion : MOTION::TYPE_JUMP;

		// モーションの設定
		m_pMotion->SetMotion(motiontype, true, 5);
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

	// 移動量の減衰
	m_pMove->SetInertia3D(0.25f);

	// 前回の位置の取得
	m_posOld = m_pCharacter3D->GetPosition();

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	// 位置の更新
	pos += move;

	float fHeight = 0.0f;

	// メッシュフィールドの当たり判定
	if (pMesh->Collision(pos,&fHeight))
	{
		// 高さの設定
		pos.y = fHeight;

		// ジャンプできる
		m_bJump = true;
		
		// モーションがジャンプだったら
		if (m_pMotion->GetBlendType() == MOTION::TYPE_JUMP)
		{
			// 着地モーションの再生
			m_pMotion->SetMotion(MOTION::TYPE_LANDING, true, 5);

			// インパクトの設定
			CMeshCircle::Confing Circleconfig = { 0.0f,10.0f,10.0f,50.0f,30,false };

			// サークルを生成
			CMeshCircle::Create(Circleconfig, D3DCOLOR_RGBA(220, 220, 220, 200),pos,32);
		}
	}
	else
	{
		m_bJump = false;
	}

	// メッシュシリンダーの取得
	CMeshCylinder* pCylinder = CManager::GetCylinder();

	// シリンダーの判定
	pCylinder->Collision(&pos);

	// インパクトの取得
	CMeshFieldImpact* pImpact = pMesh->GetImpact();

	if (pImpact != nullptr)
	{
		// インパクトとの判定
		const bool bCollision = pImpact->Collision(pos, 150.0f, pImpact->OBJ_PLAYER);

		if (bCollision && m_pCharacter3D->GetState() == m_pCharacter3D->STATE_ACTION)
		{
			// 最初の位置
			D3DXVECTOR3 firstPos = pImpact->GetFirstPos();

			// 方向の設定
			D3DXVECTOR3 dir = firstPos - pos;

			float fAngle = GetTargetAngle(firstPos, pos);

			D3DXVECTOR3 playerHandR = GetPositionFromMatrix(m_apModel[5]->GetMatrixWorld());

			// モーションをダメージにする
			m_pMotion->SetMotion(MOTION::TYPE_PARRY, true, 2);

			// パーティクルの生成
			CParticle3D::Create(playerHandR, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f), 240, 20.0f, 25, 120, 15.0f);

			// インパクトの設定
			CMeshCircle::Confing Circleconfig = { 50.0f,10.0f,0.0f,50.0f,30,false };

			// インパクトを生成
			CMeshCircle::Create(Circleconfig,D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f),pos,32, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

			// 再設定
			pImpact->Reset(dir,pImpact->OBJ_PLAYER, pos,D3DXCOLOR(1.0f,1.0f,0.5f,1.0f));
		}
		// インパクトの当たり判定
		else if (bCollision && m_pMotion->GetBlendType() != MOTION::TYPE_DAMAGE)
		{
			// インパクトの位置の取得
			D3DXVECTOR3 impactPos = pImpact->GetPosition();

			// 吹き飛び処理
			BlowOff(impactPos, 50.0f, 10.0f);

			// モーションの設定
			m_pMotion->SetMotion(MOTION::TYPE_DAMAGE, true, 5);
		}
	}

	// 重力を加算
	m_pMove->Gravity(-MAX_GRABITY);

	// 影の更新処理
	if (m_pShadow != nullptr)
	{
		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 	// 地面の法線ベクトルの取得

		D3DXVECTOR3 PlayerRay = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // 上方向ベクトルの作成

		// 影の設定処理
		m_pShadow->Update(D3DXVECTOR3(pos.x,pos.y - fHeight,pos.z),D3DXVECTOR3(pos.x, fHeight + 2.0f, pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT,SHADOW_A_LEVEL);

		// 地面の角度に合わせた角度を設定
		m_pShadow->SetFieldAngle(FieldNor, PlayerRay);
	}

	// ジャンプできるなら
	if ((pKeyboard->GetPress(DIK_SPACE) == true || pJoypad->GetPress(pJoypad->JOYKEY_A) == true) && m_bJump == true)
	{
		m_pMotion->SetMotion(MOTION::TYPE_JUMP, true, 2);

		// 移動量を上方向に設定
		m_pMove->Jump(PLAYER_JUMP_HEIGHT);
		m_bJump = false;
	}

#ifdef _DEBUG

	//if (pKeyboard->GetTrigger(DIK_B))
	//{
	//	m_pScore->SetDestScore(100000,120);

	//	// 瓦礫を生成
	//	CRubble::Create(pos, D3DXVECTOR3(15.0f, 15.0f, 15.0f), 120);

	//	// スローモーションの取得
	//	CSlow* pSlow = CManager::GetSlow();
	//	pSlow->Start(540, 4);
	//}

	//if (pKeyboard->GetTrigger(DIK_V))
	//{
	//	// 地面に波を発生させる
	//	pMesh->SetWave(pos, 20.0f, 300.0f, 380.0f, 15.0f, 0.01f, 120);
	//}

#endif // _DEBUG

	// カウンター状態
	if (pKeyboard->GetTrigger(DIK_RETURN) && m_pMotion->GetBlendType() != MOTION::TYPE_DAMAGE)
	{
		m_pMotion->SetMotion(MOTION::TYPE_ACTION, true,6);
	
		m_pCharacter3D->SetState(m_pCharacter3D->STATE_ACTION, 25);
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
		m_pSphere->SetPos(pos);
	}

	// 視界判定
	if (m_pFOV != nullptr)
	{
		// 位置の設定
		m_pFOV->SetPos(pos);
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

	// 目的の視点に近づける
	m_pCharacter3D->GetRotation()->SetSmoothAngle(0.1f);

	D3DXVECTOR3 posRDest;

	D3DXVECTOR3 rot = m_pCharacter3D->GetRotation()->Get();

	D3DXVECTOR3 modelpos = math::GetPositionFromMatrix(m_apModel[2]->GetMatrixWorld());

	posRDest.x = modelpos.x + sinf(rot.y) * 1.0f;
	posRDest.y = (modelpos.y + 0.0f) + sinf(rot.y) * 1.0f;
	posRDest.z = modelpos.z + cosf(rot.y) * 1.0f;

	// カメラの追従処理
	pCamera->SetTracking(posRDest,1.0f,0.1f);
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

	if (m_pShadow != nullptr)
	{
		// 影の描画
		m_pShadow->Draw();
	}

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
// キーボードの移動処理
//===================================================
bool CPlayer::MoveKeyboard(CInputKeyboard* pKeyboard)
{
	bool bMove = false;

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// 速さ
	float fSpeed = m_bDash ? m_pCharacter3D->GetSpeed() : 1.5f;

	// 移動量
	D3DXVECTOR3 move = m_pMove->Get();

	// 現在の目的の向きの取得
	D3DXVECTOR3 rotDest = m_pCharacter3D->GetRotation()->GetDest();

	if (m_pMotion->GetBlendType() == MOTION::TYPE_DAMAGE)
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

			rotDest.y = cameraRot.y + D3DX_PI * 0.75f;

			bMove = true;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;

			rotDest.y = cameraRot.y + D3DX_PI * 0.25f;

			bMove = true;
		}
		// プレイヤーの移動(左)
		else
		{
			move.z += sinf(cameraRot.y) * fSpeed;
			move.x -= cosf(cameraRot.y) * fSpeed;

			rotDest.y = cameraRot.y + D3DX_PI * 0.5f;

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

			rotDest.y = cameraRot.y - D3DX_PI * 0.75f;

			bMove = true;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;

			rotDest.y = cameraRot.y - D3DX_PI * 0.25f;

			bMove = true;
		}
		// プレイヤーの移動(右)
		else
		{
			move.z -= sinf(cameraRot.y) * fSpeed;
			move.x += cosf(cameraRot.y) * fSpeed;

			rotDest.y = cameraRot.y - D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//プレイヤーの移動(上)
	else if (pKeyboard->GetPress(DIK_W) == true)
	{
		move.x += sinf(cameraRot.y) * fSpeed;
		move.z += cosf(cameraRot.y) * fSpeed;

		rotDest.y = cameraRot.y + D3DX_PI;

		bMove = true;
	}
	//プレイヤーの移動(下)
	else if (pKeyboard->GetPress(DIK_S) == true)
	{
		move.x -= sinf(cameraRot.y) * fSpeed;
		move.z -= cosf(cameraRot.y) * fSpeed;

		rotDest.y = cameraRot.y;

		bMove = true;
	}
	else
	{
		int motiontype = m_pMotion->GetBlendType();

		if (motiontype == MOTION::TYPE_MOVE || motiontype == MOTION::TYPE_DASH && m_pMotion != nullptr)
		{
			m_pMotion->SetMotion(MOTION::TYPE_NEUTRAL, true, 15);
		}
	}

	// 移動量の設定
	m_pMove->Set(move);

	// 目的の向きの設定
	m_pCharacter3D->GetRotation()->SetDest(rotDest);

	return bMove;
}

//===================================================
// パッドの移動処理
//===================================================
void CPlayer::MoveJoypad(CInputJoypad* pJoypad)
{
	XINPUT_STATE* pStick;

	pStick = pJoypad->GetJoyStickAngle();

	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// 速さ
	float fSpeed = m_bDash ? m_pCharacter3D->GetSpeed() : 2.0f;

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

		// 現在の目的の向きの取得
		D3DXVECTOR3 rotDest = m_pCharacter3D->GetRotation()->GetDest();

		rotDest.y = fDestAngle;

		// 目的の向きの設定
		m_pCharacter3D->GetRotation()->SetDest(rotDest);

		// ダッシュモーションか歩きモーションかを判定
		int isDashMotion = (m_bDash ? MOTION::TYPE_DASH : MOTION::TYPE_MOVE);

		// ジャンプかjumpじゃないかを判定
		int motiontype = m_bJump ? isDashMotion : MOTION::TYPE_JUMP;

		m_pMotion->SetMotion(motiontype, true, 5);
	}
	else
	{
		int motiontype = m_pMotion->GetBlendType();

		if ((motiontype == MOTION::TYPE_MOVE || motiontype == MOTION::TYPE_DASH))
		{
			m_pMotion->SetMotion(MOTION::TYPE_NEUTRAL, true, 15);
		}
	}
	
}


//===================================================
// カウンター
//===================================================
void CPlayer::UpdateParry(void)
{
	if (m_nParryCounter >= 0)
	{
		m_nParryCounter--;
	}
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
// プレイヤーの視界の(パリィ時)判定
//===================================================
bool CPlayer::IsParry(const D3DXVECTOR3 pos)
{
	// 向きの取得
	D3DXVECTOR3 rot = m_pCharacter3D->GetRotation()->Get();

	// 視界内かつ状態が攻撃の時
	if (m_pCharacter3D->GetState() == CCharacter3D::STATE_ACTION && m_pFOV->Collision(pos, rot.y, D3DX_PI * 0.5f, -D3DX_PI * 0.5f))
	{
		return true;
	}
	return false;
}



//===================================================
// 生成処理
//===================================================
CPlayer* CPlayer::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = nullptr;

	// 3Dオブジェクトの生成
	pPlayer = new CPlayer;

	// 優先順位の取得
	int nPriority = pPlayer->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pPlayer != nullptr)
	{
		// 自分のポインタの解放
		pPlayer->Release();

		// nullにする
		pPlayer = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pPlayer == nullptr) return nullptr;


	// 当たり判定の生成
	pPlayer->m_pSphere = CCollisionSphere::Create(pos, 50.0f);

	pPlayer->Init();
	pPlayer->m_pCharacter3D->Init();
	// 初期化処理
	pPlayer->m_pCharacter3D->SetPosition(pos);
	pPlayer->m_pCharacter3D->GetRotation()->Set(rot);

	return pPlayer;
}

//===================================================
// コンストラクタ
//===================================================
CPlayerMotionController::CPlayerMotionController()
{
}

//===================================================
// デストラクタ
//===================================================
CPlayerMotionController::~CPlayerMotionController()
{

}

//===================================================
// プレイヤーのパラメーターのロード処理
//===================================================
void CPlayerMotionController::Load(std::vector<CModel*>& pModel, int* pOutNumModel)
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
				m_pMotion = CMotion::Load(FILE_NAME, pModel, pOutNumModel, TYPE_MAX, CMotion::LOAD_TEXT);
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
// 終了処理
//===================================================
void CPlayerMotionController::Uninit(void)
{
	// モーションの終了処理
	m_pMotion->Uninit();
}

//===================================================
// 更新処理
//===================================================
void CPlayerMotionController::Update(CModel** ppModel, const int nNumModel)
{
	if (m_pMotion != nullptr)
	{
		m_pMotion->Update(ppModel, nNumModel);
	}

	// モーションの遷移
	TransitionMotion();
}

//===================================================
// モーションをロードできたかどうか
//===================================================
bool CPlayerMotionController::IsLoad(void) const
{
	// ロードできたかどうか
	if (m_pMotion->IsLoad())
	{
		return true;
	}
	return false;
}
//===================================================
// プレイヤーのモーションのセット
//===================================================
void CPlayerMotionController::SetMotion(const int type, bool bBlend, const int nFrameBlend)
{
	// モーションの設定
	m_pMotion->SetMotion(type, bBlend, nFrameBlend);
}

//===================================================
// プレイヤーのモーションの遷移
//===================================================
void CPlayerMotionController::TransitionMotion(void)
{
	// モーションの種類
	TYPE motiontype = (TYPE)m_pMotion->GetBlendType();

	// モーションの遷移
	switch (motiontype)
	{
	case TYPE_NEUTRAL:
		break;
	case TYPE_MOVE:
		break;
	case TYPE_ACTION:
		break;
	case TYPE_DAMAGE:
		break;
	default:
		break;
	}
}

//===================================================
// モーションタイプの取得
//===================================================
int CPlayerMotionController::GetBlendType(void) const
{
	return m_pMotion->GetBlendType();
}
//===================================================
// パリィのイベント判定の取得
//===================================================
bool CPlayerMotionController::IsParryEvent(const int start, const int end)
{
	// パリィのフレームの判定
	if (m_pMotion->IsEventFrame(start, end, TYPE_PARRY))
	{
		return true;
	}

	return false;
}
