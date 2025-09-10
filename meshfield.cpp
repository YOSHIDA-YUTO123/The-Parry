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
#include"Collider.h"

using namespace Const;		// 名前空間Constを使用する
using namespace std;		// 名前空間stdを使用する

// 匿名の名前空間を使用
namespace
{
	constexpr int NUM_POLYGON = 2;			// 判定するポリゴンの数
	constexpr int NUM_SIRCLE = 5;			// メッシュサークルを出す数
	constexpr float MAX_COLOR = 1.0f;		// 色の最大値
	constexpr float COLOR_EASE = 0.004f;	// 色のイージング値
}

//================================================
// コンストラクタ
//================================================
CMeshField::CMeshField(int nPriority) : CMesh(nPriority)
{
	m_fWidth = NULL;
	m_fHeight = NULL;
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

	if (pMeshField == nullptr) return nullptr;
	
	// 頂点数の設定
	int nNumVtx = (nSegH + 1) * (nSegV + 1);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegH * nSegV) * 2) + (4 * (nSegV - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	// 頂点の設定
	pMeshField->SetVtxElement(nNumVtx, nNumPolygon, nNumIndex);
	pMeshField->SetSegment(nSegH, nSegV);

	// 初期化処理
	if (FAILED(pMeshField->Init()))
	{
		pMeshField->Uninit();

		// nullにする
		pMeshField = nullptr;

		return nullptr;
	}

	// 設定処理
	pMeshField->SetPosition(pos);
	pMeshField->SetRotation(rot);
	pMeshField->SetMeshField(nSegH, nSegV, pos, Size);
	pMeshField->m_fWidth = Size.x;
	pMeshField->m_fHeight = Size.y;

	return pMeshField;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshField::Init(void)
{
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CMesh::SetTextureID("data/TEXTURE/field/field.jpg");

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
	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshField::Update(void)
{
	// 法線の再設定
	UpdateNor();

	int nSegH = GetSegH();
	int nSegV = GetSegV();

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
	

	// 頂点の高さを0に戻す
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		D3DXVECTOR3 vtxPos = GetVtxPos(nCnt);

		if (vtxPos.y >= 0.0f)
		{
			vtxPos.y += (0.0f - vtxPos.y) * 0.01f;
			SetVtxPos(vtxPos, nCnt);
		}

		// 色の取得
		D3DXCOLOR vtxcol = GetColor(nCnt);

		// 色を白に近づける
		vtxcol += (WHITE - vtxcol) * COLOR_EASE;

		// 色の設定
		SetVtxColor(vtxcol, nCnt);
	}

#endif // 0

	
#if 0

	m_Wave.fTime += 1.0f / 25.0f;
	
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		D3DXVECTOR3 pos = GetVtxPos(nCnt);

		D3DXVECTOR3 diff = pPlayer->GetPosition() - pos;

		float dis = sqrtf((diff.x * diff.x) + (diff.z * diff.z));

		float offset = sinf((dis * 0.005f) - m_Wave.fTime);

		pos.y = offset * 150.0f;
		
		SetVtxPos(pos, nCnt);
	}
#endif // 0
}

//================================================
// 描画処理
//================================================
void CMeshField::Draw(void)
{
	// 描画処理
	CMesh::Draw();
}

//================================================
// メッシュフィールドの設定処理
//================================================
void CMeshField::SetMeshField(const int nSegH, const int nSegV, const D3DXVECTOR3 pos,const D3DXVECTOR2 Size)
{
	int nCntVtx = 0;

	float fTexPosX = 1.0f / nSegH;
	float fTexPosY = 1.0f / nSegV;

	D3DXVECTOR3 posWk;

	for (int nCntZ = 0; nCntZ <= nSegV; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegH; nCntX++)
		{
			// 位置の設定
			posWk.x = ((Size.x / nSegH) * nCntX) - (Size.x * 0.5f);
			posWk.y = pos.y;
			posWk.z = Size.y - ((Size.y / nSegV) * nCntZ) - (Size.y * 0.5f);

			// 頂点バッファの設定
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2((fTexPosX * nCntX), (fTexPosY * nCntZ)));

			nCntVtx++;
		}
	}

	int IndxNum = nSegH + 1;//X

	int IdxCnt = 0;//配列

	int Num = 0;//

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < nSegV; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegH; IndxCount2++, IndxNum++, Num++)
		{
			// インデックスバッファの設定
			SetIndexBuffer((WORD)IndxNum, IdxCnt);
			SetIndexBuffer((WORD)Num, IdxCnt + 1);
			IdxCnt += 2;
		}

		// NOTE:最後の行じゃなかったら
		if (IndxCount1 < nSegV - 1)
		{
			SetIndexBuffer((WORD)Num - 1, IdxCnt);
			SetIndexBuffer((WORD)IndxNum, IdxCnt + 1);
			IdxCnt += 2;
		}
	}
}

//================================================
// メッシュフィールドの当たり判定
//================================================
bool CMeshField::Collision(const D3DXVECTOR3 pos,float *pOutHeight)
{
	// 着地判定
	bool bLanding = false;

	int nSegH = GetSegH();
	int nSegV = GetSegV();

	// 1マスのサイズ
	float GridSizeX = m_fWidth / (float)nSegH;
	float GridSizeZ = m_fHeight / (float)nSegV;

	float X = pos.x + (m_fWidth * 0.5f);
	float Z = (m_fHeight * 0.5f) - pos.z;

	// 何番目のポリゴンか
	int polyX = (int)(X / GridSizeX);
	int polyZ = (int)(Z / GridSizeZ);

	// 現在のポリゴンのインデックス番号
	int polyIndex = ((polyZ * (nSegH - 1) + polyX) * 2) + (polyZ * 6);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegH * nSegV) * 2) + (4 * (nSegV - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	for (int nCnt = 0; nCnt < NUM_POLYGON; nCnt++)
	{
		// 頂点のインデックス
		int nCntVertex = (polyIndex + nCnt);

		// マイナスだったら
		if (nCntVertex < 0 || nCntVertex > nNumIndex) break;

		// インデックスを取得
		int nIdx0 = GetIndex(nCntVertex);
		int nIdx1 = GetIndex(nCntVertex + 1);
		int nIdx2 = GetIndex(nCntVertex + 2);

		if (nIdx0 >= nCntVertex || nIdx1 >= nCntVertex || nIdx2 >= nCntVertex) continue;

		// 頂点を取得
		D3DXVECTOR3 vtx0 = GetVtxPos(nIdx0);
		D3DXVECTOR3 vtx1 = GetVtxPos(nIdx1);
		D3DXVECTOR3 vtx2 = GetVtxPos(nIdx2);

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

			D3DXVECTOR3 field = GetPosition();

			D3DXVECTOR3 vec = vtx0 - pos;
			D3DXVec3Normalize(&vec, &vec);

			// プレイヤーがポリゴンの裏側かどうかの判定
			float dot = D3DXVec3Dot(&Normal, &vec); // 法線とプレイヤー方向との内積

			if (pOutHeight != nullptr)
			{
				*pOutHeight = field.y + PosY;
			}

			if (dot >= 0.0f)
			{
				bLanding = true;
				break;
			}
		}
	}

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
	int nSegH = GetSegH();
	int nSegV = GetSegV();

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

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);

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

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);

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

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);
					vtx3 = GetVtxPos(nCnt);

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

				vtx0 = GetVtxPos(nCnt);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);
				vtx3 = GetVtxPos(nIdx0);

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

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);

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

				vtx0 = GetVtxPos(nCnt);
				vtx1 = GetVtxPos(nIdx0);
				vtx2 = GetVtxPos(nIdx1);
				vtx3 = GetVtxPos(nIdx2);

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

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);

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

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);
				vtx3 = GetVtxPos(nCnt);

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

				vtx0 = GetVtxPos(nCnt);
				vtx1 = GetVtxPos(nIdx0);
				vtx2 = GetVtxPos(nIdx1);
				vtx3 = GetVtxPos(nIdx2);
				vtx4 = GetVtxPos(nIdx3);

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

			D3DXVec3Normalize(&Normal, &Normal);

			SetNormal(Normal, nCnt);
			nCnt++;
		}
	}
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

////================================================
//// メッシュフィールドのロード
////================================================
//void CMeshField::Load(void)
//{
//	fstream file("data/system.ini"); // ファイルを開く
//	string line; // ファイルの文字列読み取り用
//	string input; // 値を代入する
//
//	// ファイルを開けたら
//	if (file.is_open())
//	{
//		// ロードのマネージャの生成
//		CLoadManager* pLoadManager = new CLoadManager;
//
//		// 最後じゃないなら
//		while (getline(file, line))
//		{
//			// プレイヤーのモーションファイルを読み取ったら
//			if (line.find("FIELD_TEXTURE") != string::npos)
//			{
//				size_t equal_pos = line.find("="); // =の位置
//
//				// [=] から先を求める
//				input = line.substr(equal_pos + 1);
//
//				// ファイルの名前を取得
//				string file_name = pLoadManager->GetString(input);
//
//				// ファイルの名前を代入
//				const char* FILE_NAME = file_name.c_str();
//
//				// テクスチャのIDの設定
//				CMesh::SetTextureID(FILE_NAME);
//			}
//		}
//
//		// ロードのマネージャーの破棄
//		if (pLoadManager != nullptr)
//		{
//			delete pLoadManager;
//			pLoadManager = nullptr;
//		}
//		// ファイルを閉じる
//		file.close();
//	}
//	else
//	{
//		MessageBox(NULL, "system.iniが開けません", "ファイルが存在しません。", MB_OK | MB_ICONWARNING);
//		return;
//	}
//}

//================================================
// コンストラクタ
//================================================
CMeshFieldWave::CMeshFieldWave()
{
	// 値のクリア
	ZeroMemory(&m_Confing, sizeof(m_Confing));
	ZeroMemory(&m_Info, sizeof(m_Info));

}

//================================================
// デストラクタ
//================================================
CMeshFieldWave::~CMeshFieldWave()
{
}

//================================================
// 生成処理
//================================================
CMeshFieldWave* CMeshFieldWave::Create(Config config)
{
	// 波の生成
	CMeshFieldWave* pWave = new CMeshFieldWave;

	// nullだったら
	if (pWave == nullptr) return nullptr;

	pWave->Init(); // 初期化処理

	// パラメーターの設定
	pWave->m_Confing = config;

	// 最初の高さを設定
	pWave->m_Info.fStartHeight = config.fHeight;

	return pWave;
}

//================================================
// 初期化処理
//================================================
void CMeshFieldWave::Init(void)
{
	m_Info.nCounter = 0;
}

//================================================
// 更新処理
//================================================
bool CMeshFieldWave::Update(CMeshField* pMeshField,const int nNumVtx)
{
	// 波のカウンターを進める
	m_Info.nCounter++;

	// 相対値を求める
	float fRate = (float)m_Info.nCounter / (float)m_Confing.nTime;

	// 速さに応じた波の幅を設定
	m_Info.fTime += m_Confing.fSpeed;

	// 波の高さをだんだん0に近づける
	m_Confing.fHeight = m_Info.fStartHeight + (0.0f - m_Info.fStartHeight) * fRate;

	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		// 頂点座標の取得
		D3DXVECTOR3 pos = pMeshField->GetVtxPos(nCnt);

		// 震源地から頂点までの差分
		D3DXVECTOR3 diff = m_Confing.epicenter - pos;

		// 距離をもとめる
		float dis = sqrtf((diff.x * diff.x) + (diff.z * diff.z));

		// 時間に応じた距離を設定
		float fTimeInRadius = m_Confing.fInRadius + m_Info.fTime;
		float fTimeOutRadius = m_Confing.fOutRadius + m_Info.fTime;

		// 範囲内だったら
		if (dis >= fTimeInRadius && dis <= fTimeOutRadius)
		{
			////頂点カラーの設定
			//pMeshField->SetVtxColor(D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f), nCnt);

			// 高さの設定
			float dest = m_Confing.fHeight + sinf(dis * m_Confing.fcoef);

			// 目的の高さに近づける
			pos.y += (dest - pos.y) * 0.1f;
		}
		else
		{
			//// 色の設定
			//pMeshField->SetVtxColor(WHITE, nCnt);

			// 目的の高さに近づける
			pos.y += (0.0f - pos.y) * 0.05f;
		}

		// 頂点座標の設定
		pMeshField->SetVtxPos(pos, nCnt);
	}

	// 最大値だったら
	if (m_Info.nCounter >= m_Confing.nTime)
	{
		// カウンターをリセット
		m_Info.nCounter = 0;

		return false;
	}

	return true;
}

//================================================
// コンストラクタ
//================================================
CMeshFieldImpact::CMeshFieldImpact()
{
	ZeroMemory(&m_Config, sizeof(m_Config));
	ZeroMemory(&m_Info, sizeof(m_Info));
	m_pMove = nullptr;
}

//================================================
// デストラクタ
//================================================
CMeshFieldImpact::~CMeshFieldImpact()
{
}

//================================================
// 生成処理
//================================================
CMeshFieldImpact* CMeshFieldImpact::Create(Config config)
{
	// 衝撃波の生成
	CMeshFieldImpact* pImpact = new CMeshFieldImpact;

	// 移動量の生成
	pImpact->m_pMove = new CVelocity;

	// 当たり判定の生成
	pImpact->m_pSphere = CColliderSphere::Create(config.pos, config.fRadius);

	// 方向ベクトルにする
	D3DXVec3Normalize(&config.dir, &config.dir);

	// yは考慮しない
	config.dir.y = 0.0f;

	// 移動量の設定
	pImpact->m_pMove->Set(config.dir * config.fSpeed);

	// 設定処理
	pImpact->m_Config = config;

	return pImpact;
}

//================================================
// 終了処理
//================================================
void CMeshFieldImpact::Uninit(void)
{
	// 移動量の破棄
	if (m_pMove != nullptr)
	{
		delete m_pMove;
		m_pMove = nullptr;
	}
}

//================================================
// 更新処理
//================================================
bool CMeshFieldImpact::Update(CMeshField* pMeshField, const int nNumVtx)
{
	// nullじゃないなら
	if (m_pMove != nullptr)
	{
		// 移動量の更新処理
		m_Config.pos += m_pMove->Get();
	}

	// 吹っ飛び量を選出
	float dir = rand() % 15 + 5.0f;
	float Jump = rand() % 15 + 25.0f;

	float fAngle = (float)(rand() % 629 - 314);

	// 方向に応じた吹っ飛び量を計算
	float fMoveX = sinf(fAngle) * dir;
	float fMoveZ = cosf(fAngle) * dir;

	// 寿命を選出
	int nLife = rand() % 120 + 60;

	// 瓦礫を生成
	CRubble::Create(m_Config.pos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife, CRubble::TYPE_THREE);

	// nullじゃないなら
	if (m_pSphere != nullptr)
	{
		// 位置の更新処理
		m_pSphere->SetPosition(m_Config.pos);
	}

	// インパクトを出すタイミングを求める
	int SetImpact = m_Config.nTime / NUM_SIRCLE;

	// いちばん最初に出す、
	if (SetImpact != 0 && (m_Info.nCounter % SetImpact == 0 || m_Info.nCounter == 0))
	{
		// 角度を求める
		float rotY = atan2f(m_pMove->Get().x, m_pMove->Get().z);		

		// サークルの生成
		auto pCircle = CMeshCircle::Create(m_Config.Circlecol,m_Config.pos, 10.0f, 50.0f,32);

		// サークルの設定
		pCircle->SetCircle(50.0f, 10.0f, 60, false, D3DXVECTOR3(D3DX_PI * 0.5f, rotY, 0.0f));
	}

	// 波のカウンターを進める
	m_Info.nCounter++;


	// 頂点数分調べる
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		// 頂点の位置の取得
		D3DXVECTOR3 vtxPos = pMeshField->GetVtxPos(nCnt);

		// nullじゃないなら
		if (m_pSphere != nullptr)
		{
			// コライダーの作成
			CColliderSphere spere = m_pSphere->CreateCollider(vtxPos,50.0f);

			// 当たり判定の取得
			CCollisionSphere* pCollision = CCollisionSphere::GetInstance();

			// 円と円の判定
			if (pCollision != nullptr && pCollision->Collision(&spere, m_pSphere.get()))
			{
				//頂点カラーの設定
				pMeshField->SetVtxColor(D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f), nCnt);

				vtxPos.y += (m_Config.fHeight - vtxPos.y) * 0.3f;
			}
			else
			{
				// 目的の高さに近づける
				vtxPos.y += (0.0f - vtxPos.y) * 0.05f;
			}

			pMeshField->SetVtxPos(vtxPos, nCnt);
		}
	}

	// 最大値だったら
	if (m_Info.nCounter >= m_Config.nTime)
	{
		// カウンターをリセット
		m_Info.nCounter = 0;

		return false;
	}

	return true;
}

//================================================
// 当たり判定
//================================================
bool CMeshFieldImpact::Collision(const D3DXVECTOR3 pos, const float fRadius,const OBJ myObj, D3DXVECTOR3* pFirstPos, D3DXVECTOR3* pImpactPos)
{
	// 位置を保存
	D3DXVECTOR3 NewPos = pos;

	// y座標を考慮しない
	NewPos.y = 0.0f;

	// コライダーの作成
	CColliderSphere sphere = m_pSphere->CreateCollider(NewPos, fRadius);

	// 当たり判定の取得
	CCollisionSphere* pCollision = CCollisionSphere::GetInstance();

	if (pCollision != nullptr)
	{
		// 当たり判定
		if (pCollision->Collision(&sphere, m_pSphere.get()) && myObj != m_Config.ObjType)
		{
			if (pFirstPos != nullptr)
			{
				// 発射地点を設定
				*pFirstPos = m_Config.FirstPos;
			}

			if (pImpactPos != nullptr)
			{
				// 衝撃波の位置を設定
				*pImpactPos = m_Config.pos;
			}
			return true;
		}
	}

	return false;
}

//================================================
// 設定処理
//================================================
void CMeshFieldImpact::Reset(D3DXVECTOR3 dir,const OBJ obj,const D3DXVECTOR3 FirstPos, const D3DXCOLOR Circlecol)
{
	m_Info.nCounter = 0; 				 // カウンターをリセット
	m_Config.FirstPos = FirstPos; 		 // 発射地点を設定
	m_Config.ObjType = obj;				 // 発射したオブジェクトを設定
	D3DXVec3Normalize(&dir, &dir);		 // 方向の正規化
	m_pMove->Set(dir * m_Config.fSpeed); // 移動量を設定
	m_Config.Circlecol = Circlecol;		 // サークルの色
}
