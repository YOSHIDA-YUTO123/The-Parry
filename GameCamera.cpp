//===================================================
//
// ゲームカメラ [GameCamera.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "GameCamera.h"
#include "object.h"
#include"manager.h"
#include"renderer.h"
#include"math.h"
#include"input.h"
#include "game.h"
#include"pause.h"

using namespace Const; // 名前空間Constを使用

//constexpr float MAX_VIEW_TOP = 2.90f;	// カメラの制限(上)
//constexpr float MAX_VIEW_BOTTOM = 0.1f; // カメラの制限(下)
//constexpr float HEIGHT_OFFSET = 20.0f;	// カメラの高さのオフセット
//constexpr float ROCKON_HEIGHT = 200.0f;	// ロックオンの時のカメラの高さ
 
//===================================================
// コンストラクタ
//===================================================
CGameCamera::CGameCamera()
{
	m_state = STATE_NONE;
	m_Oldstate = STATE_NONE;
	m_nZoomTime = NULL;
	m_fDistanceBase = NULL;
	m_fZoomAngleBase = NULL;
	m_TrackObj = TRACKOBJ_PLAYER;
}

//===================================================
// デストラクタ
//===================================================
CGameCamera::~CGameCamera()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CGameCamera::Init(void)
{
	// 初期化処理
	CCamera::Init();

	CCamera::SetCamera(D3DXVECTOR3(0.0f, 250.0f, -350.0f), VEC3_NULL, D3DXVECTOR3(D3DX_PI * 0.65f, 0.0f, 0.0f));

	// 距離の取得
	float fDistance = CCamera::GetDistance();

	m_fDistanceBase = fDistance; // 距離を保存する
	m_fDistanceZoom = fDistance - 150.0f;

	m_state = STATE_TRACKING;
	m_TrackObj = TRACKOBJ_PLAYER;

	m_nZoomTime = -1;
	m_fZoomAngleBase = NULL;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CGameCamera::Uninit(void)
{
	// 終了処理
	CCamera::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CGameCamera::Update(void)
{
	// 追従状態なら
	if (m_state == STATE_TRACKING)
	{
		// マウスの視点移動
		CCamera::MouseView();

		// パッドの視点操作
		CCamera::PadView();
	}

	// ズームの処理
	ZoomIn();

	// カメラの状態がズームじゃないなら
	if (m_state != STATE_ZOOMIN)
	{
		// 距離の取得
		float fDistance = CCamera::GetDistance();

		// 距離をもとに戻す
		fDistance += (m_fDistanceBase - fDistance) * 0.1f;

		// 距離の設定
		CCamera::SetDistance(fDistance);
	}

	// 更新処理
	CCamera::Update();
}

//===================================================
// カメラ追従の設定処理
//===================================================
void CGameCamera::SetTracking(const D3DXVECTOR3 posVDest, const D3DXVECTOR3 posRDest, const float fcoef, const TRACKOBJ obj)
{
	// 状態が追従じゃないなら
	if (m_state != STATE_TRACKING) return;

	// 追従するオブジェクトが違うなら
	if (m_TrackObj != obj) return;

	D3DXVECTOR3 posR = posRDest * 1.0f;
	D3DXVECTOR3 posV = VEC3_NULL;

	// Y軸の向きの取得
	float rotY = CCamera::GetRotaition().y;

	// 距離の取得
	float fDistance = CCamera::GetDistance();

	posV.x = posVDest.x - sinf(rotY) * fDistance;
	posV.y = posVDest.y - cosf(rotY) * fDistance;
	posV.z = posVDest.z - cosf(rotY) * fDistance;

	// カメラの位置の補間
	CCamera::LerpPos(posR, posV, fcoef);
}

//===================================================
// ロックオンの設定処理
//===================================================
void CGameCamera::Rockon(D3DXVECTOR3 playerPos, D3DXVECTOR3 enemyPos)
{
	//	// カメラの状態がロックオンじゃないなら
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

	// 向きの取得
	D3DXVECTOR3 rot = CCamera::GetRotaition();

	// カメラのrotを設定
	rot.y = fAngle;

	// 向きの設定
	CCamera::SetRot(rot);

	// y座標は考慮しない
	dir.y = 0.0f;

	// 方向ベクトルにする
	D3DXVec3Normalize(&dir, &dir);

	// 距離の取得
	float fDistance = CCamera::GetDistance();

	// 距離を掛ける
	dir *= fDistance;

	// y座標を設定
	dir.y = (playerPos.y + enemyPos.y) * 0.5f;

	// 位置の設定
	D3DXVECTOR3 posV = playerPos + dir;

	// 目的の位置に近づける
	CCamera::LerpPos(posR, posV, 0.1f);
}

//===================================================
// ズームインの設定処理
//===================================================
void CGameCamera::SetZoomIn(const int nTime, const float fAngle)
{
	m_nZoomTime = nTime;
	m_fZoomAngleBase = fAngle;
	m_Oldstate = m_state;
	m_state = STATE_ZOOMIN;
}

//===================================================
// ズームインの更新処理
//===================================================
void CGameCamera::ZoomIn(void)
{
	m_nZoomTime--;

	// ズーム状態だったら
	if (m_nZoomTime >= 0 && m_state == STATE_ZOOMIN)
	{
		// 距離の取得
		float fDistance = CCamera::GetDistance();

		// 目的の距離に近づける
		fDistance += (m_fDistanceZoom - fDistance) * 0.07f;

		// 距離の設定
		CCamera::SetDistance(fDistance);

		// 向きの取得
		D3DXVECTOR3 rot = CCamera::GetRotaition();

		// 目標の角度を設定
		float fDestAngleX = 1.45f;
		float fDestAngleY = m_fZoomAngleBase - 0.65f;

		// 目的の角度までの距離を求める
		float fDiff = fDestAngleY - rot.y;

		// 目的の角度までPI以上あったら逆回りする
		NormalizeDiffRot(fDiff, &rot.y);

		// 目的の角度に近づける
		rot.x += (fDestAngleX - rot.x) * 0.07f;
		rot.y += (fDestAngleY - rot.y) * 0.07f;

		// 向きの設定
		CCamera::SetRot(rot);

		// 視点の更新処理
		UpdatePositionV();

		if (m_nZoomTime <= 0)
		{
			// 状態のリセット
			ResetState();
		}
	}
}
