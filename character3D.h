//===================================================
//
// キャラクター [character.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _CHARACTER3D_H_
#define _CHARACTER3D_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"object.h"
#include<memory>
#include<vector>

//***************************************************
// 前方宣言
//***************************************************
class CShadowS;
class CRotation;
class CMotion;
class CModel;

//***************************************************
// キャラクター3Dクラスの定義
//***************************************************
class CCharacter3D : public CObject
{
public:

	// 状態の種類
	enum STATE
	{
		STATE_NORMAL = 0,
		STATE_MOVE,
		STATE_ACTION,
		STATE_DAMAGE,
		STATE_DEATH,
		STATE_MAX
	};

	CCharacter3D();
	~CCharacter3D();

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;
	void LoadMotion(const char *pFileName, const int nNumMotion);

	// ゲッター
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	CRotation* GetRotaition(void) const { return m_pRot; }
	STATE GetState(void) { return m_state; }
	float GetSpeed(void) { return m_fSpeed; }
	CMotion* GetMotion(void) { return m_pMotion.get(); } // モーションの取得
	D3DXVECTOR3 GetModelPos(const int nIdx);
	D3DXVECTOR3 GetModelRot(const int nIdx);
	D3DXVECTOR3 GetModelSize(const int nIdx);

	int GetLife(void) const { return m_nLife; }
	bool GetAlive(void);		// 生きているか

	// セッター
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCharacter(const int nLife, const float fSpeed,const D3DXVECTOR3 ShadowScal);
	void SetState(const STATE state,const int nTime);
	void DeleteShadow(void);	// 影の消去
	bool Hit(int nDamage);		// ヒット時の処理
	bool HitStop(void);			// ヒットストップしてるかどうか
	void SetHitStop(const int nTime) { m_nHitStopTime = nTime; } // ヒットストップの設定
	void UpdateMotion(void);
private:
	std::unique_ptr<CMotion> m_pMotion;				// モーションのクラスへのポインタ
	std::vector<CModel*> m_apModel;					// モデルクラスのポインタ
	CShadowS* m_pShadowS;		// 影(ステンシル)
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_ShadowScal;	// 影の大きさ
	CRotation *m_pRot;			// 向きクラスへのポインタ
	STATE m_state;				// 状態
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	float m_fSpeed;				// 足の速さ
	int m_nNumModel;			// モデルの最大数
	int m_nCounterState;		// 状態のカウンター
	int m_nLife;				// 寿命
	int m_nHitStopTime;			// ヒットストップの時間
};

#endif