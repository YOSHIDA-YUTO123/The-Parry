//===================================================
//
// モデル [model.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "model.h"
#include "object.h"
#include "modelManager.h"
#include "manager.h"
#include"renderer.h"
#include<stdio.h>
#include"textureManager.h"

using namespace Const;		   // 名前空間Constを使用する

//===================================================
// 定数宣言
//===================================================
constexpr int MAX_TEXTURE = 2; // テクスチャの最大数

//===================================================
// コンストラクタ
//===================================================
CModel::CModel()
{
	memset(m_mtxWorld, NULL, sizeof(m_mtxWorld));
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
	m_offpos = VEC3_NULL;
	m_offrot = VEC3_NULL;
	m_pParent = nullptr;
	m_nModelIdx = NULL;
	m_pTextureIdx = nullptr;
	m_Size = VEC3_NULL;
	m_nTextureMTIdx = -1;
	m_nParentIdx = -1;
}

//===================================================
// デストラクタ
//===================================================
CModel::~CModel()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CModel::Init(const char* pModelName)
{
	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// モデルマネージャークラスの取得
	CModelManager* pModelManager = CManager::GetModel();

	// モデルの番号の登録
	m_nModelIdx = pModelManager->Register(pModelName);

	if (m_nModelIdx == -1)
	{
		return E_FAIL;
	}

	// モデルの名前の設定
	m_aModelName = pModelName;

	// マテリアルの取得
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);
	LPD3DXBUFFER pBuffMat = pModel->GetBuffMat(m_nModelIdx);

	D3DXMATERIAL* pMat;//マテリアルへのポインタ

	//マテリアルのデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	// マテリアルの数分メモリの確保
	m_pTextureIdx = new int[dwNumMat];

	for (int nCnt = 0; nCnt < (int)dwNumMat; nCnt++)
	{
		// ファイル名を使用してテクスチャを読み込む
		m_pTextureIdx[nCnt] = pTexture->Register(pMat[nCnt].pTextureFilename);
	}

	// 大きさの取得
	m_Size = pModel->GetSize(m_nModelIdx);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CModel::Uninit(void)
{
	if (m_pTextureIdx != nullptr)
	{
		delete[]m_pTextureIdx;
		m_pTextureIdx = nullptr;
	}
}

//===================================================
// 更新処理
//===================================================
void CModel::Update(void)
{
}

//===================================================
// 描画処理
//===================================================
void CModel::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxScal,mtxParent;

	D3DMATERIAL9 matDef;//現在のマテリアル保存用

	D3DXMATERIAL* pMat;//マテリアルデータへのポインタ

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y,m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x + m_offpos.x, m_pos.y + m_offpos.y, m_pos.z + m_offpos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	if (m_pParent != nullptr)
	{ // 親が存在している
		// 親モデルのマトリックスの取得
		mtxParent = m_pParent->GetMatrixWorld();
	}
	else
	{
		// ワールドマトリックスの取得
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 親のワールドマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	if (m_nModelIdx == -1)
	{
		//保存していたマテリアルを元に戻す
		pDevice->SetMaterial(&matDef);

		return;
	}

	//マテリアルのデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pModel->GetBuffMat(m_nModelIdx)->GetBufferPointer();

	// マテリアルの総数の取得
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// メッシュの取得
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		//マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		if (m_pTextureIdx[nCntMat] != -1)
		{
			//テクスチャの設定
			pDevice->SetTexture(0, pTexture->GetAdress(m_pTextureIdx[nCntMat]));
		}
		else
		{
			//テクスチャの設定
			pDevice->SetTexture(0, NULL);
		}
		//モデル(パーツ)の描画
		pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを元に戻す
	pDevice->SetMaterial(&matDef);
}

//===================================================
// 描画処理(透明度設定)
//===================================================
void CModel::Draw(const float fAlv)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxScal, mtxParent;

	D3DMATERIAL9 matDef;//現在のマテリアル保存用

	D3DXMATERIAL* pMat;//マテリアルデータへのポインタ

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x + m_offpos.x, m_pos.y + m_offpos.y, m_pos.z + m_offpos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	if (m_pParent != nullptr)
	{ // 親が存在している
		// 親モデルのマトリックスの取得
		mtxParent = m_pParent->GetMatrixWorld();
	}
	else
	{
		// ワールドマトリックスの取得
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 親のワールドマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	if (m_nModelIdx == -1)
	{
		//保存していたマテリアルを元に戻す
		pDevice->SetMaterial(&matDef);

		return;
	}

	//マテリアルのデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pModel->GetBuffMat(m_nModelIdx)->GetBufferPointer();

	// マテリアルの総数の取得
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// メッシュの取得
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		D3DXMATERIAL mat = pMat[nCntMat];
		mat.MatD3D.Diffuse.a = fAlv;

		//マテリアルの設定
		pDevice->SetMaterial(&mat.MatD3D);

		if (m_pTextureIdx[nCntMat] != -1)
		{
			//テクスチャの設定
			pDevice->SetTexture(0, pTexture->GetAdress(m_pTextureIdx[nCntMat]));
		}
		else
		{
			//テクスチャの設定
			pDevice->SetTexture(0, NULL);
		}
		//モデル(パーツ)の描画
		pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを元に戻す
	pDevice->SetMaterial(&matDef);
}

//===================================================
// マルチテクスチャの描画
//===================================================
void CModel::DrawMultTexture(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャステージステートの設定
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxScal, mtxParent;

	D3DMATERIAL9 matDef;//現在のマテリアル保存用

	D3DXMATERIAL* pMat;//マテリアルデータへのポインタ

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x + m_offpos.x, m_pos.y + m_offpos.y, m_pos.z + m_offpos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	if (m_pParent != nullptr)
	{ // 親が存在している
		// 親モデルのマトリックスの取得
		mtxParent = m_pParent->GetMatrixWorld();
	}
	else
	{
		// ワールドマトリックスの取得
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 親のワールドマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	if (m_nModelIdx == -1)
	{
		//保存していたマテリアルを元に戻す
		pDevice->SetMaterial(&matDef);

		return;
	}

	//マテリアルのデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pModel->GetBuffMat(m_nModelIdx)->GetBufferPointer();

	// マテリアルの総数の取得
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// メッシュの取得
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		//マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		if (m_pTextureIdx[nCntMat] != -1)
		{
			//テクスチャの設定
			pDevice->SetTexture(0, pTexture->GetAdress(m_pTextureIdx[nCntMat]));
		}
		else
		{
			//テクスチャの設定
			pDevice->SetTexture(0, NULL);
		}
		
		if (m_nTextureMTIdx != -1)
		{
			//テクスチャの設定
			pDevice->SetTexture(1, pTexture->GetAdress(m_nTextureMTIdx));
		}
		else
		{
			//テクスチャの設定
			pDevice->SetTexture(1, NULL);
		}

		//モデル(パーツ)の描画
		pMesh->DrawSubset(nCntMat);
	}

	//保存していたマテリアルを元に戻す
	pDevice->SetMaterial(&matDef);

	// もとに戻す
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

//===================================================
// 影の描画処理
//===================================================
void CModel::DrawShadow(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// ライトの方向
	D3DXVECTOR4 lightDir = D3DXVECTOR4(-0.707f, -0.707f, -0.707f, 0.0f);

	// 影を落とす地面
	D3DXPLANE plane = D3DXPLANE(0, -1.0f, 0, 0);

	// 影用のマトリックスを作成
	D3DXMATRIX mtxShadow, mtxShadowWorld;

	D3DMATERIAL9 matDef;//現在のマテリアル保存用

	// 影用のマトリックスの作成
	D3DXMatrixShadow(&mtxShadow, &lightDir, &plane);

	// 影のマトリックスと自分のマトリックスを掛け合わせる
	D3DXMatrixMultiply(&mtxShadowWorld, &m_mtxWorld, &mtxShadow);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadowWorld);

	//現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// 半透明の黒マテリアルをセット
	D3DMATERIAL9 matShadow = {};

	// 影の色の設定
	matShadow.Diffuse.r = 0.0f;
	matShadow.Diffuse.g = 0.0f;
	matShadow.Diffuse.b = 0.0f;
	matShadow.Diffuse.a = 1.0f;

	// モデルの取得
	CModelManager* pModel = CManager::GetModel();

	// メッシュの取得
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	// メッシュがnullptrだったら関数を抜ける
	if (pMesh == nullptr) return;

	// マテリアルの数の取得
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// 影の描画
	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		// 影のマテリアルの設定
		pDevice->SetMaterial(&matShadow);

		// テクスチャの設定
		pDevice->SetTexture(0, NULL);

		// 影の描画
		pMesh->DrawSubset(nCntMat);
	}

	// マテリアルをもとに戻す
	pDevice->SetMaterial(&matDef);
}

//===================================================
// テクスチャの名前
//===================================================
void CModel::SetTextureMT(const char* pTextureName)
{
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 取得できなかったら処理しない
	if (pTextureName == nullptr) return;

	// IDの取得
	m_nTextureMTIdx = pTexture->Register(pTextureName);
}

//===================================================
// 親の設定
//===================================================
void CModel::SetParent(CModel* pModel, const int nIdx)
{
	m_pParent = pModel;
	m_nParentIdx = nIdx;
}

//===================================================
// 情報のコピー
//===================================================
void CModel::Copy(CModel* pModel)
{
	pModel->m_offpos = m_offpos;	// 基準の位置
	pModel->m_offrot = m_offrot;	// 基準の向き
}

//===================================================
// 生成処理
//===================================================
CModel* CModel::Create(const char *pModelName)
{
	CModel* pModel = nullptr;

	// モデルの生成
	pModel = new CModel;

	if (pModel == nullptr) return nullptr;

	pModel->Init(pModelName);

	return pModel;
}
