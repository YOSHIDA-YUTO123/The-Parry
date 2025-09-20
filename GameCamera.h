//===================================================
//
// ゲームカメラ [GameCamera.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _GAMECAMERA_H_
#define _GAMECAMERA_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "camera.h"

//***************************************************
// ゲームのカメラクラスの定義
//***************************************************
class CGameCamera : public CCamera
{
public:
	// カメラの状態の種類
	typedef enum
	{
		STATE_NONE = 0, // 何もなし
		STATE_TRACKING,	// 追従
		STATE_ROCKON,	// ロックオン
		STATE_ZOOMIN,	// ズーム
		STATE_SHAKE,	// カメラの揺れ
		STATE_MAX
	}STATE;

	// カメラ追従するオブジェクトの種類
	typedef enum
	{
		TRACKOBJ_PLAYER = 0,
		TRACKOBJ_ENEMY,
		TRACKOBJ_MAX
	}TRACKOBJ;

	CGameCamera();
	~CGameCamera();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;

	void SetTracking(const D3DXVECTOR3 posVDest, const D3DXVECTOR3 posRDest, const float fcoef, const TRACKOBJ obj);
	void SetZoomIn(const int nTime, const float fAngle);

	STATE GetState(void) const { return m_state; }
	void SetTracking(const TRACKOBJ obj) { m_TrackObj = obj; }
	void SetState(const STATE state);
	void ResetState(void);
	void Rockon(D3DXVECTOR3 playerPos, D3DXVECTOR3 enemyPos,float fDistance);
	void ZoomIn(void);
	void SetShake(const int nShakeTime, const int nRange);
private:
	void ResetDistance(void);	// 距離のリセット
	void UpdateShake(void);		// カメラの揺れの更新
	STATE m_state;				// カメラの状態
	STATE m_Oldstate;			// 前の状態
	TRACKOBJ m_TrackObj;		// 追従するオブジェクトの種類
	D3DXVECTOR3 m_posOld;		// 前回の位置
	float m_fDistanceBase;		// 保存用の距離
	float m_fDistanceZoom;		// ズームする距離
	float m_fZoomAngleBase;		// ズームの角度の基準
	int m_nShakeRange;			// 揺れの幅
	int m_nShakeTime;			// 振れの時間
	int m_nZoomTime;			// ズームの時間
};

#endif