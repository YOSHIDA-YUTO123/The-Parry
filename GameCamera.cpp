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
#include "effect.h"
#include "RockOn.h"
#include "BlockManager.h"
#include "Collider.h"

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
	m_posOld = VEC3_NULL;
	m_nShakeRange = NULL;
	m_nShakeTime = NULL;
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
	// 過去の位置の設定
	m_posOld = CCamera::GetPosV();

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

	// 距離のリセット
	ResetDistance();
	
	// カメラの揺れの更新
	UpdateShake();

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
// 状態の設定
//===================================================
void CGameCamera::SetState(const STATE state)
{
	m_Oldstate = m_state; // 前回の状態を保存
	m_state = state;

	if (state == CGameCamera::STATE_ROCKON)
	{
		// ロックオンの生成
		CRockOn::Create();
	}
}

//===================================================
// 状態のリセット
//===================================================
void CGameCamera::ResetState(void)
{
	// 前の状態を設定
	m_state = m_Oldstate;

	if (m_state == CGameCamera::STATE_ROCKON)
	{
		// ロックオンの生成
		CRockOn::Create();
	}

	// レンダラーの取得
	auto pRenderer = CManager::GetRenderer();

	if (pRenderer != nullptr)
	{
		// ブラーの解除
		pRenderer->offEffect();
	}

}

//===================================================
// ロックオンの設定処理
//===================================================
void CGameCamera::Rockon(D3DXVECTOR3 playerPos, D3DXVECTOR3 enemyPos, float fDistance)
{
	//	// カメラの状態がロックオンじゃないなら
	if (m_state != STATE_ROCKON) return;

	// 敵までの方向を求める
	D3DXVECTOR3 dir = enemyPos - playerPos;

	// 角度を求める
	float fAngleY = atan2f(dir.x, dir.z);

	// 向きの取得
	D3DXVECTOR3 rot = CCamera::GetRotaition();

	rot.y = fAngleY;

	CCamera::SetRot(rot);

	// 注視点
	D3DXVECTOR3 posR = CCamera::GetPosR();

	// 敵とプレイヤーの間を見る
	D3DXVECTOR3 CenterPos = (enemyPos + playerPos) * 0.5f;

	// 注視点を敵の位置にする
	posR.x = CenterPos.x;
	posR.y = CenterPos.y;
	posR.z = CenterPos.z;

	D3DXVECTOR3 posV;
	float fHeightV = playerPos.y + 200.0f;

	posV.x = playerPos.x - sinf(rot.y) * fDistance;
	posV.y = fHeightV;
	posV.z = playerPos.z - cosf(rot.y) * fDistance;

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
		rot.x += (fDestAngleX - rot.x) * 0.1f;
		rot.y += (fDestAngleY - rot.y) * 0.1f;

		// 目的の角度までPI以上あったら逆回りする
		NormalizeRot(&rot.y);

		// 向きの設定
		CCamera::SetRot(rot);

		// 視点の更新処理
		UpdatePositionV();

		D3DXVECTOR3 posV = CCamera::GetPosV();

		// 視点の高さを固定
		posV.y = 100.0f;

		CCamera::SetPosV(posV);

		// 視点の更新処理
		UpdatePositionR();

		if (m_nZoomTime <= 0)
		{
			// 状態のリセット
			ResetState();
		}
	}
}

//===================================================
// 揺れの設定処理
//===================================================
void CGameCamera::SetShake(const int nShakeTime, const int nRange)
{
	m_nShakeTime = nShakeTime;
	m_nShakeRange = nRange;
}

//===================================================
// ズームインの更新処理
//===================================================
void CGameCamera::ResetDistance(void)
{
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
}

//===================================================
// カメラの揺れの更新
//===================================================
void CGameCamera::UpdateShake(void)
{
	if (m_nShakeTime <= 0) return;

	m_nShakeTime--;

	// 注視点
	D3DXVECTOR3 posR = CCamera::GetPosR();

	D3DXVECTOR3 posRWk = posR;

	// 揺れの最大
	int nRangeMax = m_nShakeRange * 2;
	float fRangeMin = static_cast<float>(m_nShakeRange);

	posRWk.x = posR.x + static_cast<float>(rand() % nRangeMax) - fRangeMin;
	//posRWk.y = posR.y + static_cast<float>(rand() % nRangeMax) - fRangeMin;
	posRWk.z = posR.z + static_cast<float>(rand() % nRangeMax) - fRangeMin;

	// 注視点の設定
	CCamera::SetPosR(posRWk);

	CCamera::UpdatePositionV();
	CCamera::UpdatePositionR();
}
