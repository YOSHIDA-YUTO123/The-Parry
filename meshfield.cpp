//================================================
//
// メッシュフィールド [meshfield.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "meshfield.h"
#include "manager.h"
#include "math.h"
#include "debugproc.h"
#include "explosion.h"
#include "dust.h"
#include "LoadManager.h"
#include "effect.h"
#include "impact.h"
#include "Collider.h"
#include "sound.h"
#include "player.h"
#include "game.h"
#include "math.h"
#include "renderer.h"
#include "textureManager.h" 
#include "Collision.h"
#include <string>

using namespace Const;		// 名前空間Constを使用する
using namespace std;		// 名前空間stdを使用する
using namespace math;		// 名前空間mathを使用する

// 匿名の名前空間を使用
namespace
{
	constexpr int NUM_POLYGON = 2;			// 判定するポリゴンの数
	constexpr int NUM_SIRCLE = 5;			// メッシュサークルを出す数
	constexpr float MAX_COLOR = 1.0f;		// 色の最大値
	constexpr float COLOR_EASE = 0.004f;	// 色のイージング値
	constexpr float AUDIO_DISTANCE = 2000.0f; // 音の聞こえる距離
}

//================================================
// コンストラクタ
//================================================
CMeshField::CMeshField(int nPriority) : CObject(nPriority)
{
	D3DXMatrixIdentity(&m_mtxWorld);
	m_pIdxBuffer = nullptr;
	m_pVtxBuffer = nullptr;
	m_nNumIdx = NULL;
	m_nNumPolygon = NULL;
	m_nNumVtx = NULL;
	m_nTextureIdx = -1;
	m_nSegV = 1;
	m_nSegH = 1;
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
	m_Size = VEC2_NULL;
	m_Nor = VEC3_NULL;
	m_apImpact = nullptr;
}

//================================================
// デストラクタ
//================================================
CMeshField::~CMeshField()
{
}

//================================================
// 生成処理
//================================================
CMeshField* CMeshField::Create(const D3DXVECTOR3 pos, const int nSegH, const int nSegV, const D3DXVECTOR2 Size, const D3DXVECTOR3 rot)
{
	// メッシュフィールドを生成
	CMeshField* pMeshField = new CMeshField;
	
	// 頂点数の設定
	pMeshField->m_nNumVtx = (nSegH + 1) * (nSegV + 1);

	// ポリゴン数の設定
	pMeshField->m_nNumPolygon = ((nSegH * nSegV) * 2) + (4 * (nSegV - 1));

	// インデックス数の設定
	pMeshField->m_nNumIdx = pMeshField->m_nNumPolygon + 2;

	pMeshField->m_nSegH = nSegH;
	pMeshField->m_nSegV = nSegV;

	pMeshField->m_pos = pos;
	pMeshField->m_Size = Size;
	pMeshField->m_rot = rot;
	
	// 初期化処理
	if (FAILED(pMeshField->Init()))
	{
		pMeshField->Uninit();

		// nullにする
		pMeshField = nullptr;

		return nullptr;
	}

	return pMeshField;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshField::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVtx,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	//インデックスバッファの生成
	if (FAILED(pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	SetTextureID("field.jpg");

	// 頂点のカウント
	int nCntVtx = 0;

	// テクスチャの座標の割合
	float fTexPosX = 1.0f / m_nSegH;
	float fTexPosY = 1.0f / m_nSegV;

	// 計算用の位置
	D3DXVECTOR3 posWk;

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntZ = 0; nCntZ <= m_nSegV; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= m_nSegH; nCntX++)
		{
			// 位置の設定
			posWk.x = ((m_Size.x / m_nSegH) * nCntX) - (m_Size.x * 0.5f);
			posWk.y = m_pos.y;
			posWk.z = m_Size.y - ((m_Size.y / m_nSegV) * nCntZ) - (m_Size.y * 0.5f);

			pVtx[nCntVtx].pos = posWk;

			//法線ベクトルの設定
			pVtx[nCntVtx].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			//頂点カラーの設定
			pVtx[nCntVtx].col = WHITE;

			//テクスチャ座標の設定
			pVtx[nCntVtx].tex = D3DXVECTOR2((fTexPosX * nCntX), (fTexPosY * nCntZ));

			nCntVtx++;
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();

	// インデックスバッファへのポインタ
	WORD* pIdx;

	// インデックスバッファのロック
	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	int IndxNum = m_nSegH + 1;//X

	int IdxCnt = 0;//配列

	int Num = 0;//

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < m_nSegV; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= m_nSegH; IndxCount2++, IndxNum++, Num++)
		{
			// インデックスバッファの設定
			pIdx[IdxCnt] = static_cast<WORD>(IndxNum);
			pIdx[IdxCnt + 1] = static_cast<WORD>(Num);
			IdxCnt += 2;
		}

		// NOTE:最後の行じゃなかったら
		if (IndxCount1 < m_nSegV - 1)
		{
			pIdx[IdxCnt] = static_cast<WORD>(Num - 1);
			pIdx[IdxCnt + 1] = static_cast<WORD>(IndxNum);
			IdxCnt += 2;
		}
	}

	// インデックスバッファのアンロック
	m_pIdxBuffer->Unlock();

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshField::Uninit(void)
{
	// 波の破棄
	for (int nCnt = 0; nCnt < (int)m_apWave.size(); nCnt++)
	{
		if (m_apWave[nCnt] != nullptr)
		{
			delete m_apWave[nCnt];
			m_apWave[nCnt] = nullptr;
		}
	}

	// 要素のクリア
	m_apWave.clear();


	if (m_apImpact != nullptr)
	{
		m_apImpact->Uninit();
		delete m_apImpact;
		m_apImpact = nullptr;
	}

	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuffer != nullptr)
	{
		m_pIdxBuffer->Release();
		m_pIdxBuffer = nullptr;
	}

	// 自分自身の破棄
	CObject::Release();
}

//================================================
// 更新処理
//================================================
void CMeshField::Update(void)
{
	// 法線の再設定
	UpdateNor();

	int nSegH = m_nSegH;
	int nSegV = m_nSegV;

	// 頂点数の設定
	int nNumVtx = (nSegH + 1) * (nSegV + 1);
#if 1

	// ウェーブの総数
	int nWaveSize = static_cast<int>(m_apWave.size());

	// 要素分調べる
	for (int nCnt = 0; nCnt < nWaveSize; nCnt++)
	{
		// nullなら処理を飛ばす
		if (m_apWave[nCnt] == nullptr) continue;

		// 波の更新処理
		bool bResult = m_apWave[nCnt]->Update(this, nNumVtx);

		// ウェーブが消えたら
		if (bResult == false)
		{
			// ウェーブの破棄
			delete m_apWave[nCnt];
			m_apWave[nCnt] = nullptr;
		}
	}

	// nullなら処理を飛ばす
	if (m_apImpact != nullptr)
	{
		// インパクトの更新処理
		bool bResult = m_apImpact->Update(this, nNumVtx);

		// インパクトの破棄
		if (bResult == false)
		{
			m_apImpact->Uninit();
			delete m_apImpact;
			m_apImpact = nullptr;
		}
	}

	// 要素分調べる
	for (int nCnt = 0; nCnt < nWaveSize; nCnt++)
	{
		// 波が使われているなら
		if (m_apWave[nCnt] != nullptr)
		{
			return;
		}
	}

	// インパクトが使われているなら
	if (m_apImpact != nullptr)
	{
		return;
	}
	
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点の高さを0に戻す
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		// 0より高い所にあったら
		if (pVtx[nCnt].pos.y >= 0.0f)
		{
			// 0にだんだん近づける
			pVtx[nCnt].pos.y += (0.0f - pVtx[nCnt].pos.y) * 0.01f;
		}

		// 色
		D3DXCOLOR col = pVtx[nCnt].col;

		// 色を白に近づける
		col += (WHITE - col) * COLOR_EASE;

		// 色の設定
		pVtx[nCnt].col = col;
	}

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();
#endif // 0
}

//================================================
// 描画処理
//================================================
void CMeshField::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファをデバイスのデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuffer);

	//テクスチャフォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャが無かったら
	if (m_nTextureIdx == -1)
	{
		//テクスチャの設定
		pDevice->SetTexture(0, NULL);
	}
	else
	{
		//テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	}

	//ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVtx, 0, m_nNumPolygon);
}

//================================================
// メッシュフィールドの当たり判定
//================================================
bool CMeshField::Collision(const D3DXVECTOR3 pos,float *pOutHeight)
{
	// 着地判定
	bool bLanding = false;

	int nSegH = m_nSegH;
	int nSegV = m_nSegV;

	// 1マスのサイズ
	float GridSizeX = m_Size.x / (float)nSegH;
	float GridSizeZ = m_Size.y / (float)nSegV;

	float X = pos.x + (m_Size.x * 0.5f);
	float Z = (m_Size.y * 0.5f) - pos.z;

	// 何番目のポリゴンか
	int polyX = (int)(X / GridSizeX);
	int polyZ = (int)(Z / GridSizeZ);

	// 現在のポリゴンのインデックス番号
	int polyIndex = ((polyZ * (nSegH - 1) + polyX) * 2) + (polyZ * 6);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegH * nSegV) * 2) + (4 * (nSegV - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// インデックスバッファへのポインタ
	WORD* pIdx;

	// インデックスバッファのロック
	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	for (int nCnt = 0; nCnt < NUM_POLYGON; nCnt++)
	{
		// 頂点のインデックス
		int nCntVertex = (polyIndex + nCnt);

		// マイナスだったら
		if (nCntVertex < 0 || nCntVertex > nNumIndex) continue;

		// インデックスを取得
		int nIdx0 = pIdx[nCntVertex];
		int nIdx1 = pIdx[nCntVertex + 1];
		int nIdx2 = pIdx[nCntVertex + 2];

		//// オーバーしていたら
		//if (nIdx0 >= nCntVertex || nIdx1 >= nCntVertex || nIdx2 >= nCntVertex) continue;

		// 頂点を取得
		D3DXVECTOR3 vtx0 = pVtx[nIdx0].pos;
		D3DXVECTOR3 vtx1 = pVtx[nIdx1].pos;
		D3DXVECTOR3 vtx2 = pVtx[nIdx2].pos;

		D3DXVECTOR3 edge0 = vtx1 - vtx0; // 辺ベクトル0
		D3DXVECTOR3 edge1 = vtx2 - vtx1; // 辺ベクトル1
		D3DXVECTOR3 edge2 = vtx0 - vtx2; // 辺ベクトル2

		D3DXVECTOR3 Normal = {};

		if (nCnt % 2 == 0)
		{
			// 偶数番目の三角形
			D3DXVec3Cross(&Normal, &edge0, &edge1);
		}
		else
		{
			// 奇数番目の三角形（順序が逆になっている）
			D3DXVec3Cross(&Normal, &edge1, &edge0);
		}

		D3DXVec3Normalize(&Normal, &Normal);
		m_Nor = Normal;

		D3DXVECTOR3 PlayerVec0 = pos - vtx0;
		D3DXVECTOR3 PlayerVec1 = pos - vtx1;
		D3DXVECTOR3 PlayerVec2 = pos - vtx2;

		D3DXVECTOR3 Cross0 = {};
		D3DXVECTOR3 Cross1 = {};
		D3DXVECTOR3 Cross2 = {};

		if (nCnt % 2 == 0)
		{
			// 三角形の頂点から外積
			D3DXVec3Cross(&Cross0, &edge0, &PlayerVec0);
			D3DXVec3Normalize(&Cross0, &Cross0);

			D3DXVec3Cross(&Cross1, &edge1, &PlayerVec1);
			D3DXVec3Normalize(&Cross1, &Cross1);

			D3DXVec3Cross(&Cross2, &edge2, &PlayerVec2);
			D3DXVec3Normalize(&Cross2, &Cross2);
		}
		else
		{
			// 三角形の頂点から外積
			D3DXVec3Cross(&Cross0, &PlayerVec0, &edge0);
			D3DXVec3Normalize(&Cross0, &Cross0);

			D3DXVec3Cross(&Cross1, &PlayerVec1, &edge1);
			D3DXVec3Normalize(&Cross1, &Cross1);

			D3DXVec3Cross(&Cross2, &PlayerVec2, &edge2);
			D3DXVec3Normalize(&Cross2, &Cross2);
		}

		if (Cross0.y >= 0.0f && Cross1.y >= 0.0f && Cross2.y >= 0.0f)
		{
			//float xz = ((pPos->x - vtx0.x) * Normal.x + (pPos->z - pPos->z) * Normal.z);

			//float fHeight = vtx0.y - (xz / Normal.y);

			// 平面の方程式のDを計算
			float D = -(Normal.x * vtx0.x + Normal.y * vtx0.y + Normal.z * vtx0.z);

			// プレイヤーの位置に基づいて、プレイヤーのY位置を計算
			float PosY = (Normal.x * pos.x + Normal.z * pos.z + D) / -Normal.y;

			D3DXVECTOR3 vec = vtx0 - pos;
			D3DXVec3Normalize(&vec, &vec);

			// プレイヤーがポリゴンの裏側かどうかの判定
			float dot = D3DXVec3Dot(&Normal, &vec); // 法線とプレイヤー方向との内積

			if (pOutHeight != nullptr)
			{
				*pOutHeight = m_pos.y + PosY;
			}

			if (dot >= 0.0f)
			{
				bLanding = true;
				break;
			}
		}
	}

	//インデックスバッファのアンロック
	m_pIdxBuffer->Unlock();

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();

	return bLanding;//判定を返す
}

//================================================
// インパクトとの当たり判定
//================================================
bool CMeshField::CollisionImpact(const D3DXVECTOR3 pos, const float fRadius, const CMeshFieldImpact::OBJ myObj, D3DXVECTOR3* pFirstPos, D3DXVECTOR3* pImpactPos)
{
	// nullなら処理を飛ばす
	if (m_apImpact != nullptr)
	{
		// 当たっている
		if (m_apImpact->Collision(pos, fRadius, myObj, pFirstPos, pImpactPos))
		{
			return true;
		}
	}

	return false;
}

//================================================
// インパクトの再設定
//================================================
void CMeshField::ResetImpact(D3DXVECTOR3 dir, const CMeshFieldImpact::OBJ obj, const D3DXVECTOR3 FirstPos, const D3DXCOLOR Circlecol)
{
	if (m_apImpact != nullptr)
	{
		// 再設定
		m_apImpact->Reset(dir, obj, FirstPos, Circlecol);
	}
}

//================================================
// 法線の再設定処理
//================================================
void CMeshField::UpdateNor(void)
{
	int nCnt = 0;
	int nSegH = m_nSegH;
	int nSegV = m_nSegV;

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点数分調べる
	for (int nCntZ = 0; nCntZ <= nSegV; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegH; nCntX++)
		{
			// 計算用頂点0,1,2,3,4
			D3DXVECTOR3 vtx0, vtx1, vtx2, vtx3, vtx4;

			// 計算用頂点を初期化
			vtx0 = vtx1 = vtx2 = vtx3 = vtx4 = VEC3_NULL;

			// 計算用ベクトル0,1,2,3
			D3DXVECTOR3 vec0, vec1, vec2, vec3;

			// 計算用ベクトルの初期化
			vec0 = vec1 = vec2 = vec3 = VEC3_NULL;

			// 計算用インデックス0,1,2,3の宣言
			int nIdx0 = 0;
			int nIdx1 = 0;
			int nIdx2 = 0;
			int nIdx3 = 0;

			// 計算用法線0,1,2,3
			D3DXVECTOR3 Nor0, Nor1, Nor2, Nor3;

			// 計算用法線の初期化
			Nor0 = Nor1 = Nor2 = Nor3 = VEC3_NULL;

			// 計算後の法線格納用変数
			D3DXVECTOR3 Normal = VEC3_NULL;

			// 左の上だったら
			if (nCntX == 0)
			{
				// 左上だったら
				if (nCntZ == 0)
				{
					nIdx0 = 0;
					nIdx1 = 1;
					nIdx2 = nSegH + 1;

					vtx0 = pVtx[nIdx0].pos;
					vtx1 = pVtx[nIdx1].pos;
					vtx2 = pVtx[nIdx2].pos;

					vec0 = vtx1 - vtx0;
					vec1 = vtx2 - vtx0;

					D3DXVec3Cross(&Normal, &vec0, &vec1);
				}
				// 左下だったら
				else if (nCntZ == nSegV)
				{
					nIdx0 = (nSegH + 1) * nSegV;
					nIdx1 = (nSegH + 1) * (nSegV - 1);
					nIdx2 = ((nSegH + 1) * nSegV) + 1;

					vtx0 = pVtx[nIdx0].pos;
					vtx1 = pVtx[nIdx1].pos;
					vtx2 = pVtx[nIdx2].pos;

					vec0 = vtx1 - vtx0;
					vec1 = vtx2 - vtx0;

					D3DXVec3Cross(&Normal, &vec0, &vec1);
				}
				// 左の辺(角以外)だったら
				else if(nCnt == (nSegH + 1) * nCntZ)
				{
					nIdx0 = nCnt - (nSegH + 1);
					nIdx1 = nCnt + 1;
					nIdx2 = nCnt + (nSegH + 1);

					vtx0 = pVtx[nIdx0].pos;
					vtx1 = pVtx[nIdx1].pos;
					vtx2 = pVtx[nIdx2].pos;
					vtx3 = pVtx[nCnt].pos;

					vec0 = vtx0 - vtx3;
					vec1 = vtx1 - vtx3;
					vec2 = vtx2 - vtx3;

					D3DXVec3Cross(&Nor0, &vec0, &vec1);
					D3DXVec3Cross(&Nor1, &vec1, &vec2);

					Normal = (Nor0 + Nor1) * 0.5f;
				}
			}
			// 上の辺だったら
			else if (nCntZ == 0 && nCnt < nSegH)
			{
				nIdx0 = (nSegH + 1) + nCntX;
				nIdx1 = nCnt - 1;
				nIdx2 = nCnt + 1;

				vtx0 = pVtx[nCnt].pos;
				vtx1 = pVtx[nIdx1].pos;
				vtx2 = pVtx[nIdx2].pos;
				vtx3 = pVtx[nIdx0].pos;

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;
				vec2 = vtx3 - vtx0;

				D3DXVec3Cross(&Nor0, &vec2, &vec0);
				D3DXVec3Cross(&Nor1, &vec1, &vec2);

				Normal = (Nor0 + Nor1) * 0.5f;
			}
			// 右上だったら
			else if (nCntX == nSegH && nCntZ == 0)
			{
				nIdx0 = nSegH;
				nIdx1 = nSegH - 1;
				nIdx2 = nSegH + 1 + nCntX;

				vtx0 = pVtx[nIdx0].pos;
				vtx1 = pVtx[nIdx1].pos;
				vtx2 = pVtx[nIdx2].pos;

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;

				D3DXVec3Cross(&Normal, &vec1, &vec0);
			}
			// 下の辺だったら
			else if (nCntZ == nSegV && nCnt < ((nSegH + 1) * (nSegV + 1)) - 1)
			{
				nIdx0 = nCnt - 1;
				nIdx1 = nCnt - (nSegH + 1);
				nIdx2 = nCnt + 1;

				vtx0 = pVtx[nCnt].pos;
				vtx1 = pVtx[nIdx0].pos;
				vtx2 = pVtx[nIdx1].pos;
				vtx3 = pVtx[nIdx2].pos;

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;
				vec2 = vtx3 - vtx0;

				D3DXVec3Cross(&Nor0, &vec0, &vec1);
				D3DXVec3Cross(&Nor1, &vec1, &vec2);

				Normal = (Nor0 + Nor1) * 0.5f;
			}
			// 右下だったら
			else if (nCnt == ((nSegH + 1) * (nSegV + 1)) - 1)
			{
				nIdx0 = ((nSegH + 1) * (nSegV + 1)) - 1;
				nIdx1 = nIdx0 - (nSegH + 1);
				nIdx2 = nIdx0 - 1;

				vtx0 = pVtx[nIdx0].pos;
				vtx1 = pVtx[nIdx1].pos;
				vtx2 = pVtx[nIdx2].pos;

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;

				D3DXVec3Cross(&Normal, &vec1, &vec0);
			}
			// 右の辺(角以外)だったら
			else if (nCntX == nSegH && nCnt == (nCntX * (nCntZ + 1)) + nCntZ)
			{
				nIdx0 = nCnt - (nSegH + 1);
				nIdx1 = nCnt - 1;
				nIdx2 = nCnt + (nSegH + 1);

				vtx0 = pVtx[nIdx0].pos;
				vtx1 = pVtx[nIdx1].pos;
				vtx2 = pVtx[nIdx2].pos;
				vtx3 = pVtx[nCnt].pos;

				vec0 = vtx0 - vtx3;
				vec1 = vtx1 - vtx3;
				vec2 = vtx2 - vtx3;

				D3DXVec3Cross(&Nor0, &vec1, &vec0);
				D3DXVec3Cross(&Nor1, &vec2, &vec1);

				Normal = (Nor0 + Nor1) * 0.5f;
			}
			// それ以外(端っこでも角でもない)
			else
			{
				nIdx0 = nCnt - 1;
				nIdx1 = nCnt - (nSegH + 1);
				nIdx2 = nCnt + 1;
				nIdx3 = nCnt + (nSegH + 1);

				vtx0 = pVtx[nCnt].pos;
				vtx1 = pVtx[nIdx0].pos;
				vtx2 = pVtx[nIdx1].pos;
				vtx3 = pVtx[nIdx2].pos;
				vtx4 = pVtx[nIdx3].pos;

				vec0 = vtx1 - vtx0; // 左
				vec1 = vtx2 - vtx0; // 上

				vec2 = vtx3 - vtx0; // 右
				vec3 = vtx4 - vtx0; // 下

				D3DXVec3Cross(&Nor0, &vec0, &vec1);
				D3DXVec3Cross(&Nor1, &vec3, &vec0);
				D3DXVec3Cross(&Nor2, &vec1, &vec2);
				D3DXVec3Cross(&Nor3, &vec2, &vec3);

				Normal = (Nor0 + Nor1 + Nor2 + Nor3) * 0.25f;
			}

			// 正規化
			D3DXVec3Normalize(&Normal, &Normal);

			// 法線の再設定
			pVtx[nCnt].nor = Normal;

			nCnt++;
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// 頂点座標の取得
//================================================
D3DXVECTOR3 CMeshField::GetVtxPos(const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	D3DXVECTOR3 OutPos;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	OutPos = pVtx[nIdx].pos;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	return OutPos;
}

//================================================
// ウェーブの設定処理
//================================================
void CMeshField::SetWave(CMeshFieldWave::Config config)
{
	// ウェーブの生成
	CMeshFieldWave *pWave = CMeshFieldWave::Create(config);

	// 要素の追加
	m_apWave.push_back(pWave);
}

//================================================
// インパクトの設定処理
//================================================
void CMeshField::SetImpact(CMeshFieldImpact::Config config)
{
	if (m_apImpact == nullptr)
	{
		// インパクトの生成
		m_apImpact = CMeshFieldImpact::Create(config);
	}
}

//================================================
// 頂点座標の設定
//================================================
void CMeshField::SetVtxPos(const D3DXVECTOR3 pos, const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].pos = pos;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// 色の設定
//================================================
void CMeshField::SetColor(const D3DXCOLOR col, const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].col = col;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// テクスチャのIDの設定処理
//================================================
void CMeshField::SetTextureID(const char* pTextureName)
{
	// テクスチャマネージャーの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// ファイルパス
	string filePath = "data/TEXTURE/field/";

	// 文字列の連結
	filePath += pTextureName;

	if (pTexture != nullptr)
	{
		// テクスチャのIDの取得
		m_nTextureIdx = pTexture->Register(filePath.c_str());
	}
}