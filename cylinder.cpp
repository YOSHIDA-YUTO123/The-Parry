//================================================
//
// メッシュシリンダー [cylinder.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "cylinder.h"
#include "player.h"
#include "manager.h"
#include"math.h"

using namespace math; // 名前空間mathを使用

//================================================
// コンストラクタ
//================================================
CMeshCylinder::CMeshCylinder(int nPriority) : CMesh(nPriority)
{
	m_CenterPos = VEC3_NULL;
	m_fRadius = NULL;
}

//================================================
// デストラクタ
//================================================
CMeshCylinder::~CMeshCylinder()
{
}

//================================================
// 生成処理
//================================================
CMeshCylinder* CMeshCylinder::Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ,const float fRadius, const float fHeight, const D3DXVECTOR3 rot)
{
	// メッシュフィールドを生成
	CMeshCylinder* pMesh = new CMeshCylinder;

	// 優先順位の取得
	int nPriority = pMesh->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pMesh != nullptr)
	{
		// 自分のポインタの解放
		pMesh->Release();

		// nullにする
		pMesh = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pMesh == nullptr) return nullptr;

	// 頂点数の設定
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	// 頂点の設定
	pMesh->SetVtxElement(nNumVtx, nNumPolygon, nNumIndex);
	pMesh->SetSegment(nSegX, nSegZ);

	// 初期化処理
	pMesh->Init();

	// 設定処理
	pMesh->SetPosition(pos);
	pMesh->SetRotation(rot);
	pMesh->SetCylinder(nSegX, nSegZ, fRadius, fHeight);
	pMesh->m_CenterPos = pos;
	pMesh->m_fRadius = fRadius;

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshCylinder::Init(void)
{
	// 初期化処理
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CMesh::SetTextureID("data/TEXTURE/field.jpg");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshCylinder::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshCylinder::Update(void)
{
}

//================================================
// 描画処理
//================================================
void CMeshCylinder::Draw(void)
{
	// 描画処理
	CMesh::Draw();
}

//================================================
// シリンダーの設定処理
//================================================
void CMeshCylinder::SetCylinder(const int nSegX, const int nSegZ, const float fRadius,const float fHeight)
{
	int nCntVtx = 0;

	float fTexPosX = 1.0f / nSegX;
	float fTexPosY = 1.0f / nSegZ;

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			D3DXVECTOR3 posWk = VEC3_NULL;

			// 横の分割数
			float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

			posWk.x = sinf(fAngle) * fRadius;
			posWk.y = fHeight - (fHeight / nSegZ * nCntZ);
			posWk.z = cosf(fAngle) * fRadius;

			// 頂点バッファの設定
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2((fTexPosX * nCntX), (fTexPosY * nCntZ)),VEC3_NULL,D3DXCOLOR(1.0f,1.0f,1.0f,0.0f));

			// 法線の正規化
			D3DXVECTOR3 nor = NormalizeNormal(nCntVtx);

			// 法線の正規化
			SetNormal(nor, nCntVtx);

			nCntVtx++;
		}
	}

	int IndxNum = nSegX + 1; // インデックスの数値1

	int IdxCnt = 0; // 配列

	int Num = 0; // インデックスの数値2

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < nSegZ; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegX; IndxCount2++, IndxNum++, Num++)
		{
			// インデックスバッファの設定
			SetIndexBuffer((WORD)IndxNum, IdxCnt);
			SetIndexBuffer((WORD)Num, IdxCnt + 1);
			IdxCnt += 2;
		}

		// NOTE:最後の行じゃなかったら
		if (IndxCount1 < nSegZ - 1)
		{
			SetIndexBuffer((WORD)Num - 1, IdxCnt);
			SetIndexBuffer((WORD)IndxNum, IdxCnt + 1);
			IdxCnt += 2;
		}
	}
}

//================================================
// 当たり判定
//================================================
bool CMeshCylinder::Collision(D3DXVECTOR3 *pPos)
{
	// 横の分割数の取得
	int nSegX = GetSegX();

	// 横の分割数分調べる
	for (int nCnt = 0; nCnt <= nSegX; nCnt++)
	{
		int nNextIndx = (nCnt + 1) % nSegX; // 次の頂点のインデックス
		int nIndx = nCnt;					// 今の頂点のインデックス

		// 頂点座標の取得
		D3DXVECTOR3 vtx0 = GetVtxPos(nIndx), vtx1 = GetVtxPos(nNextIndx);

		// 辺ベクトルの作成
		D3DXVECTOR3 edge = GetVector(vtx1, vtx0);

		// プレイヤーまでのベクトルを作成
		D3DXVECTOR3 playerVec = GetVector(*pPos, vtx0);

		D3DXVECTOR3 cross; // 外積

		// 外積する
		D3DXVec3Cross(&cross, &edge, &playerVec);

		// 正規化する
		D3DXVec3Normalize(&cross, &cross);

		// プレイヤーがシリンダーの外に出たら
		if (cross.y < 0.0f)
		{
			D3DXVECTOR3 objectPos = *pPos;
			objectPos.y = 0.0f;

			// シリンダーの中心までのベクトルを作成
			D3DXVECTOR3 CenterDir = GetVector(objectPos, m_CenterPos);

			// 中心からプレイヤーまでの距離を求める
			float fDistance = GetDistance(objectPos - m_CenterPos);

			// めり込んだ深さを求める
			float fDepth = (m_fRadius - 10.0f) - fDistance;

			// プレイヤーの位置から中心までの方向×めり込んだ深さを足す
			D3DXVECTOR3 pos = objectPos + CenterDir * fDepth;
			
			// 位置を設定
			pPos->x = pos.x;
			pPos->z = pos.z;

			return true;
		}
	}
	return false;
}
