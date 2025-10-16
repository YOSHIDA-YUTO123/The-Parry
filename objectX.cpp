//===================================================
//
// オブジェクトXファイル [objectX.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "objectX.h"
#include"manager.h"
#include"renderer.h"
#include"modelManager.h"
#include"textureManager.h"

//===================================================
// コンストラクタ
//===================================================
CObjectX::CObjectX(int nPriority) : CObject(nPriority)
{
	memset(m_mtxWorld, NULL, sizeof(D3DXMATRIX));

	m_pos = Const::VEC3_NULL;
	m_rot = Const::VEC3_NULL;
	m_pTextureIdx = nullptr;
	m_nModelIdx = -1;
	m_nTextureMT = -1;
}

//===================================================
// デストラクタ
//===================================================
CObjectX::~CObjectX()
{
}

//===================================================
// 生成処理
//===================================================
CObjectX* CObjectX::Create(const D3DXVECTOR3 pos, const char* pModelName,const D3DXVECTOR3 rot)
{
	CObjectX* pObjectX = nullptr;

	// 3Dオブジェクトの生成
	pObjectX = new CObjectX;

	if (pObjectX == nullptr) return nullptr;

	pObjectX->Init();
	pObjectX->m_pos = pos;
	pObjectX->m_rot = rot;
	pObjectX->LoadModel(pModelName);

	return pObjectX;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CObjectX::Init(void)
{
	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CObjectX::Uninit(void)
{
	// マテリアルの破棄
	if (m_pTextureIdx != nullptr)
	{
		delete []m_pTextureIdx;
		m_pTextureIdx = nullptr;
	}
	
	// 自分自身の破棄
	Release();
}

//===================================================
// 更新処理
//===================================================
void CObjectX::Update(void)
{
}

//===================================================
// 描画処理
//===================================================
void CObjectX::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxScal;

	D3DMATERIAL9 matDef;//現在のマテリアル保存用

	D3DXMATERIAL* pMat;//マテリアルデータへのポインタ

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

		if (m_nTextureMT != -1)
		{
			//テクスチャの設定
			pDevice->SetTexture(1, pTexture->GetAdress(m_nTextureMT));
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
}

//===================================================
// 描画処理(オーバーロード)
//===================================================
void CObjectX::Draw(const float Diffuse)
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
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

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
		D3DXMATERIAL Mat = pMat[nCntMat];

		Mat.MatD3D.Diffuse.a = Diffuse;

		//マテリアルの設定
		pDevice->SetMaterial(&Mat.MatD3D);

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
// 影の描画処理
//===================================================
void CObjectX::DrawShadow(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライトの方向
	D3DXVECTOR4 lightDir = D3DXVECTOR4(0.707f, -0.707f, 0.707f, 0.0f);

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
// ワールドマトリックスの設定
//===================================================
void CObjectX::SetUpMatrix(const D3DXVECTOR3 Scal)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans,mtxScal;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 大きさを反映
	D3DXMatrixScaling(&mtxScal, Scal.x, Scal.y, Scal.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScal);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
}

//===================================================
// ワールドマトリックスの設定
//===================================================
void CObjectX::SetUpMatrix(const D3DXMATRIX mtxRot, const D3DXMATRIX mtxTrans, const D3DXVECTOR3 Scal)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 計算用のマトリックス
	D3DXMATRIX mtxScal;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 大きさを反映
	D3DXMatrixScaling(&mtxScal, Scal.x, Scal.y, Scal.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScal);

	// 向きを反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
}

//===================================================
// 描画までの設定処理
//===================================================
void CObjectX::SetUpDraw(const float Diffuse)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	D3DMATERIAL9 matDef;//現在のマテリアル保存用

	D3DXMATERIAL* pMat;//マテリアルデータへのポインタ

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
		D3DXMATERIAL Mat = pMat[nCntMat];

		Mat.MatD3D.Diffuse.a = Diffuse;

		//マテリアルの設定
		pDevice->SetMaterial(&Mat.MatD3D);

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
// マルチテクスチャのIDの設定
//===================================================
void CObjectX::SetTextureMT(const char* pTextureName)
{
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 取得できなかったら処理しない
	if (pTexture == nullptr) return;

	// IDの取得
	m_nTextureMT = pTexture->Register(pTextureName);
}

//===================================================
// モデルのロード処理
//===================================================
HRESULT CObjectX::LoadModel(const char* pXFileName)
{
	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// インデックスの登録
	m_nModelIdx = pModel->Register(pXFileName);

	if (m_nModelIdx == -1)
	{
		return E_FAIL;
	}

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

	return S_OK;
}

//===================================================
// 大きさの取得
//===================================================
D3DXVECTOR3 CObjectX::GetSize(void)
{
	// モデルクラスの取得
	CModelManager* pModel = CManager::GetModel();

	// 大きさの取得
	D3DXVECTOR3 Size = pModel->GetSize(m_nModelIdx);

	// 大きさを返す
	return Size;
}

