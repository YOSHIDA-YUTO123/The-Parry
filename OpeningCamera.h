//===================================================
//
// オープニングカメラ [openingcamera.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OPENINGCAMERA_H_
#define _OPENINGCAMERA_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "camera.h"
#include <vector>

//***************************************************
// オープニングカメラクラスの定義
//***************************************************
class COpeningCamera : public CCamera
{
public:

	// カメラの状態設定
	typedef enum
	{
		STATE_NORMAL = 0, // 通常状態
		STATE_ANIM,		  // アニメーション
		STATE_TRACKING,	  // 追従
		STATE_MOVE,		  // 移動
		STATE_MAX
	}STATE;

	COpeningCamera();
	~COpeningCamera();

	STATE GetState(void) const { return m_state; }
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void SetTracking(const D3DXVECTOR3 posRDest);		   // 追従処理
	void SetShake(const int nShakeTime, const int nRange); // カメラの揺れ
	void SetMove(const D3DXVECTOR3 move);
	void EnableTutorial(const bool bTutorial) { m_bTutorial = bTutorial; }
private:
	void Load(void);
	void UpdateKey(D3DXVECTOR3* pPosR);
	void UpdateShake(void);				// カメラの揺れの更新
	void UpdateMoveShake(D3DXVECTOR3* pPosV);			// 移動状態の時の揺れの更新

	static constexpr float FADEOUT_POS = 5500.0f;	 // フェードアウトする位置
	static constexpr float DEST_MVOE_POS = -1500.0f; // 目的の位置する位置

	// キー情報
	struct Key_Info
	{
		D3DXVECTOR3 DestPosR; // 目的の向き
		int nFrame;		   	  // フレーム
	};

	// カメラの情報
	struct Info
	{
		std::vector<Key_Info> akey_info; // キー情報
		int nNumKey;
	};

	Info m_Info;				// 情報
	STATE m_state;				// 状態
	D3DXVECTOR3 m_move;			// 移動量
	float m_fMoveShakeSpeed;	// 速さ
	float m_fCounter;			// カウンター
	int m_nKey;					// 現在のキー
	int m_nextKey;				// 次のキー
	int m_nCounter;				// カウンター
	int m_nShakeRange;			// 揺れの幅
	int m_nShakeTime;			// 振れの時間
	bool m_bTutorial;			// チュートリアルに移行するかどうか
	bool m_bFlash;				// 画面が光ったか判定
	bool m_bReset;				// 再設定したかどうか
	bool m_bDestPos;			// 目的の位置に到達したかどうか
};

#endif