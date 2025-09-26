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
#include"object.h"
#include "FieldWave.h"
#include "FieldImpact.h"
#include<vector>
#include <memory> 

//************************************************
// 前方宣言
//************************************************
class CMeshField;
class CColliderSphere;
class CMeshFieldWave;
class CMeshFieldImpact;

//************************************************
// メッシュフィールドクラスの定義
//************************************************
class CMeshField : public CObject
{
public:
	CMeshField(int nPriority = 2);
	~CMeshField();

	static CMeshField* Create(const D3DXVECTOR3 pos,const int nSegH,const int nSegV,const D3DXVECTOR2 Size, const D3DXVECTOR3 rot = Const::VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	bool Collision(const D3DXVECTOR3 pos, float* pOutHeight); // 地面との当たり判定

	// インパクトとの当たり判定
	bool CollisionImpact(const D3DXVECTOR3 pos, const float fRadius, const CMeshFieldImpact::OBJ myObj, D3DXVECTOR3* pFirstPos = nullptr, D3DXVECTOR3* pImpactPos = nullptr);

	// インパクトの再設定処理
	void ResetImpact(D3DXVECTOR3 dir, const CMeshFieldImpact::OBJ obj, const D3DXVECTOR3 FirstPos, const D3DXCOLOR Circlecol);

	void UpdateNor(void); // 法線の再設定

	// ゲッター
	D3DXVECTOR3 GetNor(void) const { return m_Nor; }
	D3DXVECTOR3 GetVtxPos(const int nIdx);

	// セッター
	void SetWave(CMeshFieldWave::Config config);
	void SetImpact(CMeshFieldImpact::Config config);
	void SetVtxPos(const D3DXVECTOR3 pos, const int nIdx);
	void SetColor(const D3DXCOLOR col, const int nIdx);
	void SetTextureID(const char* pTextureName);
private:
	std::vector<CMeshFieldWave*> m_apWave;	// フィールドの波クラスへのポインタ
	CMeshFieldImpact* m_apImpact;			// フィールドのインパクトクラスへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;	// インデックスバッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXVECTOR2 m_Size;						// 大きさ
	D3DXVECTOR3 m_Nor;						// 法線
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	int m_nSegH;							// 横の分割数
	int m_nSegV;							// 縦の分割数
	int m_nNumVtx;							// 頂点の総数
	int m_nNumPolygon;						// ポリゴン数
	int m_nNumIdx;							// インデックス数
	int m_nTextureIdx;						// テクスチャのID
};

#endif