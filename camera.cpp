//===================================================
//
// カメラ [camera.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "camera.h"
#include "object.h"
#include"manager.h"
#include"renderer.h"
#include "player.h"
#include"math.h"
#include"meshfield.h"
#include"cylinder.h"
#include"input.h"
#include "game.h"

using namespace Const; // 名前空間Constを使用

constexpr float MAX_VIEW_TOP = 2.90f;	// カメラの制限(上)
constexpr float MAX_VIEW_BOTTOM = 0.1f; // カメラの制限(下)
constexpr float HEIGHT_OFFSET = 20.0f;	// カメラの高さのオフセット
constexpr float ROCKON_HEIGHT = 200.0f;	// ロックオンの時のカメラの高さ

//===================================================
// コンストラクタ
//===================================================
CCamera::CCamera()
{
	memset(m_mtxView, NULL, sizeof(D3DXMATRIX));
	memset(m_mtxProjection, NULL, sizeof(D3DXMATRIX));
	m_posR = VEC3_NULL;
	m_posV = VEC3_NULL;
	m_rot = VEC3_NULL;
	m_posRDest = VEC3_NULL;
	m_posVDest = VEC3_NULL;
	m_vecU = VEC3_NULL;
	m_fDistance = NULL;
	m_state = STATE_NONE;
	m_Oldstate = STATE_NONE;
	m_nZoomTime = NULL;
	m_fDistanceBase = NULL;
	m_fZoomAngleBase = NULL;
}

//===================================================
// デストラクタ
//===================================================
CCamera::~CCamera()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CCamera::Init(void)
{
	m_posR = VEC3_NULL;

	m_posV = D3DXVECTOR3(0.0f, 250.0f, -350.0f);		// 視点

	m_rot = D3DXVECTOR3(D3DX_PI * 0.65f, 0.0f, 0.0f);	// 角度

	float fRotX = m_posV.x - m_posR.x;
	float fRotY = m_posV.y - m_posR.y;
	float fRotZ = m_posV.z - m_posR.z;

	// 距離を求める
	m_fDistance = sqrtf((fRotX * fRotX) + (fRotY * fRotY) + (fRotZ * fRotZ));
	m_fDistanceBase = m_fDistance; // 距離を保存する
	m_fDistanceZoom = m_fDistance - 150.0f;

	//カメラの注視点初期座標を設定
	m_posR.x = m_posV.x + sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posR.y = m_posV.y + cosf(m_rot.x) * m_fDistance;
	m_posR.z = m_posV.z + sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;

	// カメラの視点の情報
	m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
	m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;

	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル

	m_state = STATE_TRACKING;

	m_nZoomTime = -1;
	m_fZoomAngleBase = NULL;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CCamera::Uninit(void)
{
}

//===================================================
// 更新処理
//===================================================
void CCamera::Update(void)
{
	// マウスの視点移動
	MouseView();

	// ズームの処理
	ZoomIn();

	// カメラの状態がズームじゃないなら
	if (m_state != STATE_ZOOMIN)
	{
		// 距離をもとに戻す
		m_fDistance += (m_fDistanceBase - m_fDistance) * 0.1f;
	}

#if 0

	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

	// プレイヤーの取得
	D3DXVECTOR3 playerPos(pPlayer->GetPosition().x, pPlayer->GetPosition().y + 200.0f, pPlayer->GetPosition().z);
	D3DXVECTOR3 playerRot = pPlayer->GetRotation();

	m_posRDest.x = playerPos.x + sinf(playerRot.y) * 1.0f;
	m_posRDest.y = playerPos.y + sinf(playerRot.y) * 1.0f;
	m_posRDest.z = playerPos.z + cosf(playerRot.y) * 1.0f;

	m_posVDest.x = playerPos.x - sinf(m_rot.y) * m_fDistance;
	m_posVDest.y = playerPos.y - cosf(m_rot.y) * m_fDistance;
	m_posVDest.z = playerPos.z - cosf(m_rot.y) * m_fDistance;

	m_posR.x += ((m_posRDest.x - m_posR.x) * 0.07f);
	m_posR.y += ((m_posRDest.y - m_posR.y) * 0.07f);
	m_posR.z += ((m_posRDest.z - m_posR.z) * 0.07f);

	m_posV.x += ((m_posVDest.x - m_posV.x) * 0.07f);
	m_posV.z += ((m_posVDest.z - m_posV.z) * 0.07f);

#endif

	// メッシュシリンダーの取得
	CMeshCylinder* pCylinder = CGame::GetCylinder();

	// メッシュフィールドの取得
	CMeshField* pField = CGame::GetField();

	float fHeight = 0.0f;

	if (pField != nullptr)
	{
		// フィールドの当たり判定
		if (pField->Collision(m_posV, &fHeight))
		{
			// 高さを設定
			m_posV.y = fHeight + HEIGHT_OFFSET;
		}
	}

	// シリンダーの当たり判定
	if (pCylinder != nullptr)
	{
		pCylinder->Collision(&m_posV);
	}

	// 角度の正規化
	NormalizeRot(&m_rot.x);
	NormalizeRot(&m_rot.y);
	NormalizeRot(&m_rot.z);
}

//===================================================
// 設定,描画処理
//===================================================
void CCamera::SetCamera(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();

	// レンダラーがnullだったら処理しない
	if (pRenderer == nullptr) return;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(45.0f),
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		10.0f,
		100000.0f);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}

//===================================================
// マウスホイールの処理
//===================================================
void CCamera::MouseWheel(void)
{
	CInputMouse* pMouse = CManager::GetInputMouse();

	// マウスホイールの回転量を取得
	int nWheel = pMouse->GetMouseWeel();

	// ホイールを回してなかったら
	if (nWheel == 0)
	{
		return;
	}

	if (nWheel < 0)
	{
		m_fDistance += 100.0f;
	}
	else if (nWheel > 0)
	{
		m_fDistance -= 100.0f;
	}

	// 視点の更新処理
	UpdatePositionV();
}

//===================================================
// マウスの視点移動処理
//===================================================
void CCamera::MouseView(void)
{
	if (m_state != STATE_TRACKING) return;

	// マウスの取得
	CInputMouse* pMouse = CManager::GetInputMouse();

	//D3DXVECTOR2 Move = pMouse->GetVelocity();
	//D3DXVECTOR2 MoveOld = pMouse->GetOldVelocity();

	//D3DXVECTOR2 fAngle = Move - MoveOld;

#ifdef _DEBUG
	// マウスホイール
	CCamera::MouseWheel();
#endif

	// パッドの視点操作
	CCamera::PadView();

	// ポーズ中かどうか
	bool bPause = CManager::GetPause();

	static POINT prevCursorPos = { (long)SCREEN_WIDTH / (long)2.0f,(long)SCREEN_HEIGHT / (long)2.0f };

	POINT cursorPos;
	GetCursorPos(&cursorPos);

	float X = (float)cursorPos.x - prevCursorPos.x;
	float Y = (float)cursorPos.y - prevCursorPos.y;

	const float mouseSensitivity = 0.00095f;

	X *= mouseSensitivity;
	Y *= mouseSensitivity;

	if (pMouse->OnMousePress(0) || bPause == false)
	{
		//回転量を更新
		m_rot.y += X;
		m_rot.x += Y;

		// 回転量を制限
		if (m_rot.x > MAX_VIEW_TOP)
		{
			m_rot.x -= Y;
		}
		else if (m_rot.x < MAX_VIEW_BOTTOM)
		{
			m_rot.x -= Y;
		}

		// カーソルの位置の設定
		SetCursorPos((long)SCREEN_WIDTH / (long)2.0f, (long)SCREEN_HEIGHT / (long)2.0f);

		// 視点の更新処理
		UpdatePositionV();
	}
#ifdef _DEBUG

	if (pMouse->OnMousePress(1) && bPause == true)
	{
		// 回転量を更新
		m_rot.y += X;
		m_rot.x += Y;

		// 回転量を制限
		if (m_rot.x > MAX_VIEW_TOP)
		{
			m_rot.x -= Y;
		}
		else if (m_rot.x < MAX_VIEW_BOTTOM)
		{
			m_rot.x -= Y;
		}

		SetCursorPos((long)SCREEN_WIDTH / (long)2.0f, (long)SCREEN_HEIGHT / (long)2.0f);

		// 注視点の更新処理
		UpdatePositionR();
	}
#endif // _DEBUG
}

//===================================================
// パッドの視点移動処理
//===================================================
void CCamera::PadView(void)
{
	// コントローラーの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// スティック
	XINPUT_STATE* pStick = pJoypad->GetJoyStickAngle();

	if (pJoypad->GetJoyStickR() == true)
	{
		float RStickAngleY = pStick->Gamepad.sThumbRY;
		float RStickAngleX = pStick->Gamepad.sThumbRX;

		float DeadZone = 10920.0f;
		float fMag = sqrtf((RStickAngleX * RStickAngleX) + (RStickAngleY * RStickAngleY));

		if (fMag >= DeadZone)
		{
			float NormalizeX = RStickAngleX / fMag;
			float NormalizeY = RStickAngleY / fMag;

			float fAngle = fMag * 0.000003f;

			m_rot.y += NormalizeX * 0.5f * fAngle;
			m_rot.x -= NormalizeY * 0.5f * fAngle;
		}
	}

	// 視点の更新処理
	UpdatePositionV();
}

//===================================================
// カメラの追従処理
//===================================================
void CCamera::SetTracking(const D3DXVECTOR3 posVDest,const D3DXVECTOR3 posRDest, const float fSpeed, const float fcoef)
{
	if (m_state != STATE_TRACKING) return;

	m_posRDest.x = posRDest.x * fSpeed;
	m_posRDest.y = posRDest.y * fSpeed;
	m_posRDest.z = posRDest.z * fSpeed;

	m_posVDest.x = posVDest.x - sinf(m_rot.y) * m_fDistance;
	m_posVDest.y = posVDest.y - cosf(m_rot.y) * m_fDistance;
	m_posVDest.z = posVDest.z - cosf(m_rot.y) * m_fDistance;

	m_posR.x += ((m_posRDest.x - m_posR.x) * fcoef);
	m_posR.y += ((m_posRDest.y - m_posR.y) * fcoef);
	m_posR.z += ((m_posRDest.z - m_posR.z) * fcoef);

	m_posV.x += ((m_posVDest.x - m_posV.x) * fcoef);
	m_posV.z += ((m_posVDest.z - m_posV.z) * fcoef);
}

//===================================================
// ロックオン処理
//===================================================
void CCamera::Rockon(D3DXVECTOR3 playerPos, D3DXVECTOR3 enemyPos)
{
	// カメラの状態がロックオンじゃないなら
	if (m_state != STATE_ROCKON) return;

	// プレイヤーまでの方向を求める
	D3DXVECTOR3 dir = playerPos - enemyPos;

	// 角度を求める
	float fAngle = atan2f(dir.x, dir.z) + D3DX_PI;

	// 注視点
	D3DXVECTOR3 posR;

	// 注視点を敵の位置にする
	posR.x = enemyPos.x;
	posR.y = (playerPos.y + enemyPos.y) * 0.5f;
	posR.z = enemyPos.z;

	// 目的の注視点の設定
	m_posRDest = posR;

	// カメラのrotを設定
	m_rot.y = fAngle;
	
	// y座標は考慮しない
	dir.y = 0.0f; 

	// 方向ベクトルにする
	D3DXVec3Normalize(&dir, &dir);

	// 距離を掛ける
	dir *= m_fDistance;

	// y座標を設定
	dir.y = (playerPos.y + enemyPos.y) * 0.5f;

	// 目的の視点の設定
	m_posVDest = playerPos + dir;

	// 目的の注視点に近づける
	m_posR.x += ((m_posRDest.x - m_posR.x) * 0.1f);
	m_posR.y += ((m_posRDest.y - m_posR.y) * 0.1f);
	m_posR.z += ((m_posRDest.z - m_posR.z) * 0.1f);

	// 目的の視点に近づける
	m_posV.x += ((m_posVDest.x - m_posV.x) * 0.1f);
	m_posV.y += ((m_posVDest.y - m_posV.y) * 0.1f);
	m_posV.z += ((m_posVDest.z - m_posV.z) * 0.1f);
}

//===================================================
// ズームの設定処理
//===================================================
void CCamera::SetZoomIn(const int nTime, const float fAngle)
{
	m_nZoomTime = nTime;
	m_fZoomAngleBase = fAngle;
	m_Oldstate = m_state;
	m_state = STATE_ZOOMIN;
}

//===================================================
// ズーム処理
//===================================================
void CCamera::ZoomIn(void)
{
	m_nZoomTime--;
	
	// ズーム状態だったら
	if (m_nZoomTime >= 0 && m_state == STATE_ZOOMIN)
	{
		// 目的の距離に近づける
		m_fDistance += (m_fDistanceZoom - m_fDistance) * 0.07f;

		// 目標の角度を設定
		float fDestAngleX = 1.45f;
		float fDestAngleY = m_fZoomAngleBase - 0.65f;

		// 目的の角度までの距離を求める
		float fDiff = fDestAngleY - m_rot.y;

		// 目的の角度までPI以上あったら逆回りする
		NormalizeDiffRot(fDiff,&m_rot.y);

		// 目的の角度に近づける
		m_rot.x += (fDestAngleX - m_rot.x) * 0.07f;
		m_rot.y += (fDestAngleY - m_rot.y) * 0.07f;

		// 視点の更新処理
		UpdatePositionV();

		if (m_nZoomTime <= 0)
		{
			ResetState();
		}
	}
}

//===================================================
// 視点の更新処理
//===================================================
void CCamera::UpdatePositionV(void)
{
	// カメラの視点の更新
	m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
	m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;
}

//===================================================
// 注視点の更新処理
//===================================================
void CCamera::UpdatePositionR(void)
{
	// カメラの注視点の更新
	m_posR.x = m_posV.x + sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posR.y = m_posV.y + cosf(m_rot.x) * m_fDistance;
	m_posR.z = m_posV.z + sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;
}
