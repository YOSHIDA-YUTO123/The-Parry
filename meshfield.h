//================================================
//
// メッシュフィールド [meshfield.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

//************************************************
// インクルードファイル
//************************************************
#include"mesh.h"
#include<vector>
#include"transform.h"
#include"Collision.h"

//************************************************
// 前方宣言
//************************************************
class CMeshField;

//************************************************
// メッシュのフィールドのウェーブクラスの定義
//************************************************
class CMeshFieldWave
{
public:
	CMeshFieldWave();
	~CMeshFieldWave();

	static CMeshFieldWave* Create(const D3DXVECTOR3 epicenter, const float InR, const float OutR, const float fHeight, const float fSpeed, const float fcoef, const int nTime);
	void Init(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
private:
	D3DXVECTOR3 m_epicenter;			// 波の発生地点
	float m_fInRadius, m_fOutRadius;	// 波の半径
	float m_fHeight;					// 波の高さ
	float m_fStartHeight;				// 最初の高さ
	float m_fTime;						// 高さ
	float m_fSpeed;						// 速さ
	float m_fcoef;						// 係数
	int m_nCounter;						// 波のカウンター
	int m_nTime;						// 波の発生時間
};

//************************************************
// メッシュフィールドのインパクトクラスの定義
//************************************************
class CMeshFieldImpact
{
public:

	// アタッカーの種類
	typedef enum
	{
		OBJ_NONE = 0,
		OBJ_PLAYER,
		OBJ_ENEMY,
		OBJ_MAX
	}OBJ;

	CMeshFieldImpact();
	~CMeshFieldImpact();

	static CMeshFieldImpact* Create(const D3DXVECTOR3 pos,D3DXVECTOR3 dir, const float fHeight, const float fRadius, const int nTime,const float fSpeed,const OBJ attacker);
	void Uninit(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
	bool Collision(const D3DXVECTOR3 pos,const float fRadius, const OBJ myObj);
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetFirstPos(void) const { return m_FirstPos; }
	void Reset(D3DXVECTOR3 dir, const OBJ obj, const D3DXVECTOR3 FirstPos);
private:
	CCollisionSphere* m_pSphere; // 円の当たり判定
	D3DXVECTOR3 m_FirstPos;		 // 波の最初の発生位置
	D3DXVECTOR3 m_pos;	         // 位置
	CVelocity* m_pMove;	         // 移動量クラスへのポインタ
	OBJ m_ObjType;				 // 誰が放った衝撃波か
	float m_fRadius;	         // 半径
	float m_fHeight;	         // 高さ
	float m_fSpeed;				 // 速さ
	int m_nTime;		         // 時間
	int m_nCounter;		         // カウンター
};
//************************************************
// メッシュフィールドクラスの定義
//************************************************
class CMeshField : public CMesh
{
public:
	CMeshField(int nPriority = 2);
	~CMeshField();

	static CMeshField* Create(const D3DXVECTOR3 pos,const int nSegX,const int nSegZ,const D3DXVECTOR2 Size, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetMeshField(const int nSegX, const int nSegZ, const  D3DXVECTOR3 pos, const D3DXVECTOR2 Size);
	bool Collision(const D3DXVECTOR3 pos, float* pOutHeight);
	void UpdateNor(void); // 法線の再設定

	D3DXVECTOR3 GetNor(void) const { return m_Nor; }

	void SetWave(const D3DXVECTOR3 epicenter, const float InR, const float OutR, const float fHeight, const float fSpeed, const float fcoef, const int nTime);
	void SetImpact(const D3DXVECTOR3 pos, D3DXVECTOR3 dir, const float fHeight, const float fRadius, const int nTime, const float fSpeed,CMeshFieldImpact::OBJ attacker);
	CMeshFieldImpact* GetImpact(void) { return m_pImpact; }
private:
	void Load(void);

	CMeshFieldImpact* m_pImpact;			// フィールドのインパクトクラスへのポインタ
	std::vector<CMeshFieldWave*> m_pWave;	// フィールドの波クラスへのポインタ
	D3DXVECTOR3 m_Nor;					// 法線
	float m_fWidth, m_fHeight;			// 横幅,高さ
};

#endif