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
#include "manager.h"
#include "renderer.h"
#include "player.h"
#include "math.h"
#include "meshfield.h"
#include "cylinder.h"
#include "input.h"
#include "game.h"
#include "pause.h"
#include "BlockManager.h"
#include "Collider.h"

using namespace Const; // 名前空間Constを使用

namespace
{
	constexpr float MAX_VIEW_TOP = 2.90f;	// カメラの制限(上)
	constexpr float MAX_VIEW_BOTTOM = D3DX_PI * 0.45f; // カメラの制限(下)
	constexpr float HEIGHT_OFFSET = 20.0f;	// カメラの高さのオフセット
	constexpr float ROCKON_HEIGHT = 200.0f;	// ロックオンの時のカメラの高さ
	constexpr float FOV_BASE = 45.0f;		// 視野角ベース
	constexpr float FOV_COLLISION = 65.0f;	// 視野角壁際の時
}

//===================================================
// コンストラクタ
//===================================================
CCamera::CCamera()
{
	m_posVOld = VEC3_NULL;
	m_fFov = FOV_BASE;
	m_bCollision = false;
	D3DXMatrixIdentity(&m_mtxView);
	D3DXMatrixIdentity(&m_mtxProjection);
	m_posR = VEC3_NULL;
	m_posV = VEC3_NULL;
	m_rot = VEC3_NULL;
	m_posRDest = VEC3_NULL;
	m_posVDest = VEC3_NULL;
	m_vecU = VEC3_NULL;
	m_fDistance = NULL;
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

	m_posV = VEC3_NULL;		// 視点

	m_rot = VEC3_NULL;	// 角度

	//m_posV = D3DXVECTOR3(0.0f, 250.0f, -350.0f);		// 視点

	//m_rot = D3DXVECTOR3(D3DX_PI * 0.65f, 0.0f, 0.0f);	// 角度

	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル

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
	//// 前回の位置の設定
	//m_posVOld = m_posV;

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
#ifdef _DEBUG
		// ポーズマネージャーの取得
		auto pPause = CPauseManager::GetInstance();

		// ポーズ状態じゃないなら
		if (pPause != nullptr && !pPause->GetPause())
		{
			if (pCylinder->Collision(&m_posV))
			{
				m_fFov += (FOV_COLLISION - m_fFov) * 0.01f;
				m_bCollision = true;
			}
			else
			{
				m_fFov += (FOV_BASE - m_fFov) * 0.01f;
				m_bCollision = false;
			}
		}
#else
		if (pCylinder->Collision(&m_posV))
		{
			m_fFov += (FOV_COLLISION - m_fFov) * 0.01f;
			m_bCollision = true;
		}
		else
		{
			m_fFov += (FOV_BASE - m_fFov) * 0.01f;
			m_bCollision = false;
		}
#endif // _DEBUG
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
		D3DXToRadian(m_fFov),
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		10.0f,
		100000.0f);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}

//===================================================
// カメラの設定処理
//===================================================
void CCamera::SetCamera(const D3DXVECTOR3 posV, const D3DXVECTOR3 posR, const D3DXVECTOR3 rot)
{
	// 設定処理
	m_posV = posV;
	m_posR = posR;
	m_rot = rot;

	float fRotX = m_posV.x - m_posR.x;
	float fRotY = m_posV.y - m_posR.y;
	float fRotZ = m_posV.z - m_posR.z;

	// 距離を求める
	m_fDistance = sqrtf((fRotX * fRotX) + (fRotY * fRotY) + (fRotZ * fRotZ));

	//カメラの注視点初期座標を設定
	m_posR.x = m_posV.x + sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posR.y = m_posV.y + cosf(m_rot.x) * m_fDistance;
	m_posR.z = m_posV.z + sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;

	// カメラの視点の情報
	m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
	m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;
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
	// マウスの取得
	CInputMouse* pMouse = CManager::GetInputMouse();

#ifdef _DEBUG
	// マウスホイール
	CCamera::MouseWheel();
#endif

	// ポーズ中かどうか
	bool bPause = CPauseManager::GetPause();

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

	}

	// 視点の更新処理
	UpdatePositionV();

	// 注視点の更新処理
	UpdatePositionR();

#ifdef _DEBUG

	if (pMouse->OnMousePress(1))
	{
		if (!m_bCollision)
		{
			// 回転量を更新
			m_rot.y += X;
			m_rot.x += Y;
		}
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

			// 回転量を計算
			float fAngleX = NormalizeX * 0.5f * fAngle;
			float fAngleY = NormalizeY * 0.5f * fAngle;

			m_rot.y += fAngleX;
			m_rot.x -= fAngleY;

			// 回転量を制限
			if (m_rot.x > MAX_VIEW_TOP)
			{
				m_rot.x += fAngleY;
			}
			else if (m_rot.x < MAX_VIEW_BOTTOM)
			{
				m_rot.x += fAngleY;
			}

			// 視点の更新処理
			UpdatePositionV();
		}
	}
}

//===================================================
// 視点の更新処理
//===================================================
void CCamera::UpdatePositionV(void)
{
	// 前回の位置の設定
	m_posVOld = m_posV;

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

//===================================================
// 位置を目的の位置に近づける
//===================================================
void CCamera::LerpPos(const D3DXVECTOR3 posRDest, const D3DXVECTOR3 posVDest, const float fCoef)
{
	// 前回の位置の設定
	m_posVOld = m_posV;

	// 目的の注視点の設定
	m_posRDest.x = posRDest.x;
	m_posRDest.y = posRDest.y;
	m_posRDest.z = posRDest.z;

	// 目的の視点の設定
	m_posVDest.x = posVDest.x;
	m_posVDest.y = posVDest.y;
	m_posVDest.z = posVDest.z;

	// 目的の注視点に近づける
	m_posR.x += (m_posRDest.x - m_posR.x) * fCoef;
	m_posR.y += (m_posRDest.y - m_posR.y) * fCoef;
	m_posR.z += (m_posRDest.z - m_posR.z) * fCoef;

	// 目的の視点に近づける
	m_posV.x += (m_posVDest.x - m_posV.x) * fCoef;
	m_posV.y += (m_posVDest.y - m_posV.y) * fCoef;
	m_posV.z += (m_posVDest.z - m_posV.z) * fCoef;
}