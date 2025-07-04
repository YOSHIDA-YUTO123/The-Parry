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
#include<memory>

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

	// 波の情報(震源地、内側半径、外側半径、高さ、速さ、係数、発生時間)
	struct Config
	{
		D3DXVECTOR3 epicenter;			// 波の発生地点
		float fInRadius;				// 波の内半径
		float fOutRadius;				// 波の外半径
		float fHeight;					// 波の高さ
		float fSpeed;					// 速さ
		float fcoef;					// 係数
		int nTime;						// 波の発生時間
	};

	CMeshFieldWave();
	~CMeshFieldWave();

	static CMeshFieldWave* Create(Config config);
	void Init(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
private:

	// 波の情報(アライメント対策)
	struct ParamInfo
	{
		float fStartHeight;		// 最初の高さ
		float fTime;			// 高さ
		int nCounter;			// 波のカウンター
	};

	Config m_Confing;		// 波のパラメータ
	ParamInfo m_Info;			// 波の情報
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

	// 衝撃波の構成(位置、発射位置、方向、サークルの色、放ったオブジェクト、当たり判定の半径、高さ、速さ、発生時間)
	struct Config
	{
		D3DXVECTOR3 pos;	         // 位置
		D3DXVECTOR3 FirstPos;		 // 最初の位置
		D3DXVECTOR3 dir;			 // 方向
		D3DXCOLOR Circlecol;		 // 色
		OBJ ObjType;				 // 誰が放った衝撃波か
		float fRadius;	         // 半径
		float fHeight;	         // 高さ
		float fSpeed;				 // 速さ
		int nTime;		         // 時間
	};
	CMeshFieldImpact();
	~CMeshFieldImpact();

	static CMeshFieldImpact* Create(Config config);
	void Uninit(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
	bool Collision(const D3DXVECTOR3 pos,const float fRadius, const OBJ myObj);
	D3DXVECTOR3 GetPosition(void) const { return m_Config.pos; }
	D3DXVECTOR3 GetFirstPos(void) const { return m_Config.FirstPos; }
	void Reset(D3DXVECTOR3 dir, const OBJ obj, const D3DXVECTOR3 FirstPos,const D3DXCOLOR Circlecol);
private:

	// 衝撃波の情報()
	struct Info
	{
		int nCounter;		         // カウンター
	};
	Config m_Config;			 // 構成
	Info m_Info;				 // 情報

	std::unique_ptr<CCollisionSphere> m_pSphere; // 円の当たり判定
	CVelocity* m_pMove;	         // 移動量クラスへのポインタ
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

	// ゲッター
	D3DXVECTOR3 GetNor(void) const { return m_Nor; }
	CMeshFieldImpact* GetImpact(void) { return m_pImpact; }

	// セッター
	void SetWave(CMeshFieldWave::Config config);
	void SetImpact(CMeshFieldImpact::Config config);
private:
	void Load(void);

	std::vector<CMeshFieldWave*> m_pWave;	// フィールドの波クラスへのポインタ
	CMeshFieldImpact* m_pImpact;			// フィールドのインパクトクラスへのポインタ
	D3DXVECTOR3 m_Nor;						// 法線
	float m_fWidth, m_fHeight;				// 横幅,高さ
};

#endif