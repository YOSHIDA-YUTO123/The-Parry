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
	m_pImpact = nullptr;
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
CMeshField* CMeshField::Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ, const D3DXVECTOR2 Size, const D3DXVECTOR3 rot)
{
	// メッシュフィールドを生成
	CMeshField* pMeshField = new CMeshField;

	// 優先順位の取得
	int nPriority = pMeshField->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pMeshField != nullptr)
	{
		// 自分のポインタの解放
		pMeshField->Release();

		// nullにする
		pMeshField = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pMeshField == nullptr) return nullptr;
	
	// 頂点数の設定
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	// 頂点の設定
	pMeshField->SetVtxElement(nNumVtx, nNumPolygon, nNumIndex);
	pMeshField->SetSegment(nSegX, nSegZ);

	// 初期化処理
	pMeshField->Init();

	// 設定処理
	pMeshField->SetPosition(pos);
	pMeshField->SetRotation(rot);
	pMeshField->SetMeshField(nSegX, nSegZ, pos, Size);
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

	// メッシュフィールドのロード
	Load();

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshField::Uninit(void)
{
	// 波の破棄
	for (int nCnt = 0; nCnt < (int)m_pWave.size(); nCnt++)
	{
		if (m_pWave[nCnt] != nullptr)
		{
			delete m_pWave[nCnt];
			m_pWave[nCnt] = nullptr;
		}
	}
	// 要素のクリア
	m_pWave.clear();

	// インパクトの破棄
	if (m_pImpact != nullptr)
	{
		m_pImpact->Uninit();
		delete m_pImpact;
		m_pImpact = nullptr;
	}

	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshField::Update(void)
{
	//CPlayer* pPlayer = CManager::GetPlayer();
	//CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// 法線の再設定
	UpdateNor();

	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// 頂点数の設定
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);
#if 1

	// 要素分調べる
	for (int nCnt = 0; nCnt < (int)m_pWave.size(); nCnt++)
	{
		// 波の更新処理
		if (m_pWave[nCnt] != nullptr)
		{
			// 波の更新処理
			bool bResult = m_pWave[nCnt]->Update(this, nNumVtx);

			// ウェーブが消えたら
			if (m_pWave[nCnt] != nullptr && bResult == false)
			{
				// ウェーブの破棄
				delete m_pWave[nCnt];
				m_pWave[nCnt] = nullptr;
			}
		}
	}

	// nullじゃないなら
	if (m_pImpact != nullptr)
	{
		// インパクトの更新処理
		bool bResult = m_pImpact->Update(this, nNumVtx);

		// インパクトの破棄
		if (bResult == false && m_pImpact != nullptr)
		{
			m_pImpact->Uninit();
			delete m_pImpact;
			m_pImpact = nullptr;
		}
	}

	// 要素分調べる
	for (int nCnt = 0; nCnt < (int)m_pWave.size(); nCnt++)
	{
		// 波が使われているなら
		if (m_pWave[nCnt] != nullptr)
		{
			return;
		}
	}

	// インパクトが使われているなら
	if (m_pImpact != nullptr)
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
void CMeshField::SetMeshField(const int nSegX, const int nSegZ, const D3DXVECTOR3 pos,const D3DXVECTOR2 Size)
{
	int nCntVtx = 0;

	float fTexPosX = 1.0f + 1.0f / nSegX;
	float fTexPosY = 1.0f + 1.0f / nSegZ;

	D3DXVECTOR3 posWk;

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			// 位置の設定
			posWk.x = ((Size.x / nSegX) * nCntX) - (Size.x * 0.5f);
			posWk.y = pos.y;
			posWk.z = Size.y - ((Size.y / nSegZ) * nCntZ) - (Size.y * 0.5f);

			// 頂点バッファの設定
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2((fTexPosX * nCntX), (fTexPosY * nCntZ)));

			nCntVtx++;
		}
	}

	int IndxNum = nSegX + 1;//X

	int IdxCnt = 0;//配列

	int Num = 0;//

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
// メッシュフィールドの当たり判定
//================================================
bool CMeshField::Collision(const D3DXVECTOR3 pos,float *pOutHeight)
{
	// 着地判定
	bool bLanding = false;

	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// 1マスのサイズ
	float GridSizeX = m_fWidth / (float)nSegX;
	float GridSizeZ = m_fHeight / (float)nSegZ;

	float X = pos.x + (m_fWidth * 0.5f);
	float Z = (m_fHeight * 0.5f) - pos.z;

	// 何番目のポリゴンか
	int polyX = (int)(X / GridSizeX);
	int polyZ = (int)(Z / GridSizeZ);

	// 現在のポリゴンのインデックス番号
	int polyIndex = ((polyZ * (nSegX - 1) + polyX) * 2) + (polyZ * 6);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

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

			*pOutHeight = field.y + PosY;

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
// 法線の再設定処理
//================================================
void CMeshField::UpdateNor(void)
{
	int nCnt = 0;
	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// 頂点数分調べる
	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
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
					nIdx2 = nSegX + 1;

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);

					vec0 = vtx1 - vtx0;
					vec1 = vtx2 - vtx0;

					D3DXVec3Cross(&Normal, &vec0, &vec1);
				}
				// 左下だったら
				else if (nCntZ == nSegZ)
				{
					nIdx0 = (nSegX + 1) * nSegZ;
					nIdx1 = (nSegX + 1) * (nSegZ - 1);
					nIdx2 = ((nSegX + 1) * nSegZ) + 1;

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);

					vec0 = vtx1 - vtx0;
					vec1 = vtx2 - vtx0;

					D3DXVec3Cross(&Normal, &vec0, &vec1);
				}
				// 左の辺(角以外)だったら
				else if(nCnt == (nSegX + 1) * nCntZ)
				{
					nIdx0 = nCnt - (nSegX + 1);
					nIdx1 = nCnt + 1;
					nIdx2 = nCnt + (nSegX + 1);

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
			else if (nCntZ == 0 && nCnt < nSegX)
			{
				nIdx0 = (nSegX + 1) + nCntX;
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
			else if (nCntX == nSegX && nCntZ == 0)
			{
				nIdx0 = nSegX;
				nIdx1 = nSegX - 1;
				nIdx2 = nSegX + 1 + nCntX;

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;

				D3DXVec3Cross(&Normal, &vec1, &vec0);
			}
			// 下の辺だったら
			else if (nCntZ == nSegZ && nCnt < ((nSegX + 1) * (nSegZ + 1)) - 1)
			{
				nIdx0 = nCnt - 1;
				nIdx1 = nCnt - (nSegX + 1);
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
			else if (nCnt == ((nSegX + 1) * (nSegZ + 1)) - 1)
			{
				nIdx0 = ((nSegX + 1) * (nSegZ + 1)) - 1;
				nIdx1 = nIdx0 - (nSegX + 1);
				nIdx2 = nIdx0 - 1;

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;

				D3DXVec3Cross(&Normal, &vec1, &vec0);
			}
			// 右の辺(角以外)だったら
			else if (nCntX == nSegX && nCnt == (nCntX * (nCntZ + 1)) + nCntZ)
			{
				nIdx0 = nCnt - (nSegX + 1);
				nIdx1 = nCnt - 1;
				nIdx2 = nCnt + (nSegX + 1);

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
				nIdx1 = nCnt - (nSegX + 1);
				nIdx2 = nCnt + 1;
				nIdx3 = nCnt + (nSegX + 1);

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
	m_pWave.push_back(pWave);
}

//================================================
// インパクトの設定処理
//================================================
void CMeshField::SetImpact(CMeshFieldImpact::Config config)
{
	// インパクトの生成
	if (m_pImpact == nullptr)
	{
		// インパクトの生成
		m_pImpact = CMeshFieldImpact::Create(config);
	}
}

//================================================
// メッシュフィールドのロード
//================================================
void CMeshField::Load(void)
{
	fstream file("data/system.ini"); // ファイルを開く
	string line; // ファイルの文字列読み取り用
	string input; // 値を代入する

	// ファイルを開けたら
	if (file.is_open())
	{
		// ロードのマネージャの生成
		CLoadManager* pLoadManager = new CLoadManager;

		// 最後じゃないなら
		while (getline(file, line))
		{
			// プレイヤーのモーションファイルを読み取ったら
			if (line.find("FIELD_TEXTURE") != string::npos)
			{
				size_t equal_pos = line.find("="); // =の位置

				// [=] から先を求める
				input = line.substr(equal_pos + 1);

				// ファイルの名前を取得
				string file_name = pLoadManager->GetString(input);

				// ファイルの名前を代入
				const char* FILE_NAME = file_name.c_str();

				// テクスチャのIDの設定
				CMesh::SetTextureID(FILE_NAME);
			}
		}

		// ロードのマネージャーの破棄
		if (pLoadManager != nullptr)
		{
			delete pLoadManager;
			pLoadManager = nullptr;
		}
		// ファイルを閉じる
		file.close();
	}
	else
	{
		MessageBox(NULL, "system.iniが開けません", "ファイルが存在しません。", MB_OK | MB_ICONWARNING);
		return;
	}
}

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
	pImpact->m_pSphere = CCollisionSphere::Create(config.pos, config.fRadius);

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
		m_pSphere->SetPos(m_Config.pos);
	}

#ifdef _DEBUG

	CEffect3D::Create(m_Config.pos, m_Config.fRadius, VEC3_NULL, m_Config.Circlecol, 10);
#endif // _DEBUG

	// インパクトを出すタイミングを求める
	int SetImpact = m_Config.nTime / NUM_SIRCLE;

	// いちばん最初に出す、
	if (SetImpact != 0 && (m_Info.nCounter % SetImpact == 0 || m_Info.nCounter == 0))
	{
		// 角度を求める
		float rotY = atan2f(m_pMove->Get().x, m_pMove->Get().z);

		// インパクトの設定
		CMeshCircle::Confing Circleconfig = { 50.0f,10.0f,0.0f,50.0f,60,false };

		// サークルの生成
		CMeshCircle::Create(Circleconfig,m_Config.Circlecol,m_Config.pos,32, D3DXVECTOR3(D3DX_PI * 0.5f, rotY, 0.0f));
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
			CCollisionSphere spere = m_pSphere->CreateCollider(vtxPos, 50.0f);

			// 円と円の判定
			if (m_pSphere->Collision(&spere))
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
bool CMeshFieldImpact::Collision(const D3DXVECTOR3 pos, const float fRadius,const OBJ myObj)
{
	// コライダーの作成
	CCollisionSphere sphere = m_pSphere->CreateCollider(pos, fRadius);

	// 当たり判定
	if (m_pSphere->Collision(&sphere) && myObj != m_Config.ObjType)
	{
		return true;
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
