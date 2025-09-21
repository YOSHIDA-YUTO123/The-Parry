//===================================================
//
// カメラ [camera.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _CAMERA_H_
#define _CAMERA_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"

//***************************************************
// カメラクラスの定義
//***************************************************
class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);

	void SetCamera(void);
	void SetCamera(const D3DXVECTOR3 posV, const D3DXVECTOR3 posR, const D3DXVECTOR3 rot);
	void MouseWheel(void);
	void MouseView(void);
	void PadView(void);

	// ゲッター
	D3DXVECTOR3 GetRotaition(void) const { return m_rot; }
	D3DXVECTOR3 GetPosV(void) const { return m_posV; }
	D3DXVECTOR3 GetPosR(void) const { return m_posR; }
	D3DXVECTOR3 GetPosOld(void) const { return m_posVOld; }
	void LerpPos(const D3DXVECTOR3 posRDest, const D3DXVECTOR3 posVDest, const float fCoef);	// 位置を目的の位置に近づける

	// セッター
	void SetRot(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetPosV(const D3DXVECTOR3 posV) { m_posV = posV; }
protected:

	// ゲッター
	D3DXVECTOR3 GetDestPosV(void) const { return m_posVDest; }
	D3DXVECTOR3 GetDestPosR(void) const { return m_posRDest; }
	float GetDistance(void) const { return m_fDistance; }
	void UpdatePositionV(void);			// 視点の更新処理
	void UpdatePositionR(void);			// 注視点の更新処理

	// セッター
	void SetDestPosV(const D3DXVECTOR3 destpos) { m_posVDest = destpos; }
	void SetPosR(const D3DXVECTOR3 posR) { m_posR = posR; }
	void SetDestPosR(const D3DXVECTOR3 destpos) { m_posRDest = destpos; }
	void SetDistance(const float fDistance) { m_fDistance = fDistance; }
private:
	D3DXMATRIX m_mtxView;		// ビューマトリックス
	D3DXMATRIX m_mtxProjection;	// プロジェクションマトリックス
	D3DXVECTOR3 m_posV;			// 視点
	D3DXVECTOR3 m_posVOld;		// 前回の位置
	D3DXVECTOR3 m_posR;			// 注視点
	D3DXVECTOR3 m_vecU;			// 上方向ベクトル
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_posRDest;		// 目的の注視点
	D3DXVECTOR3 m_posVDest;		// 目的の視点
	float m_fDistance;			// 距離
	float m_fFov;				// カメラの視野角
	bool m_bCollision;			//
};

#endif