//================================================
//
// 影 [shadow.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "shadow.h"
#include "manager.h"
#include"renderer.h"
#include"math.h"

using namespace math; // 名前空間mathを使用
using namespace std;  // 名前空間stdを使用

//================================================
// コンストラクタ
//================================================
CShadow::CShadow()
{
	m_col = WHITE;
	m_pos = VEC3_NULL;
	m_pVtxBuffer = nullptr;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_nTextureIdx = -1;
}

//================================================
// デストラクタ
//================================================
CShadow::~CShadow()
{
}

//================================================
// 生成処理
//================================================
unique_ptr<CShadow> CShadow::Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const D3DXCOLOR col)
{
	// 影の生成
	unique_ptr<CShadow> pShadow = make_unique<CShadow>();

	if (pShadow == nullptr) return nullptr;

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 影のテクスチャの設定
	pShadow->m_nTextureIdx = pTexture->Register("data/TEXTURE/effect000.jpg");

	// 位置、向き、大きさの設定
	pShadow->m_pRot = make_unique<CRotation>();
	pShadow->m_pSize = make_unique<CSize3D>();
	pShadow->m_pos = pos;
	pShadow->m_pSize->Set(D3DXVECTOR3(fWidth, 0.0f, fHeight));
	pShadow->m_col = col;
	pShadow->Init();
	
	return pShadow;
}

//================================================
// 初期化処理
//================================================
HRESULT CShadow::Init(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	//頂点バッファの生成・頂点情報の設定
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 大きさの取得
	D3DXVECTOR3 Size = m_pSize->Get();

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, -Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, -Size.z);

	// 法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CShadow::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}
}

//================================================
// 更新処理
//================================================
void CShadow::Update(const D3DXVECTOR3 player, const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const float fMaxHeight, const float fAlv)
{
	// 高さと最大の高さの割合を計算
	float fWidthWk = player.y / fMaxHeight;
	float fHeightWk = player.y / fMaxHeight;

	// 割合をもとに大きさを決める
	float SetWidth = fWidth + (fWidthWk * fWidth);
	float SetHeight = fHeight + (fHeightWk * fHeight);

	// 位置の設定
	m_pos = pos;

	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-SetWidth, 0.0f, SetHeight);
	pVtx[1].pos = D3DXVECTOR3(SetWidth, 0.0f, SetHeight);
	pVtx[2].pos = D3DXVECTOR3(-SetWidth, 0.0f, -SetHeight);
	pVtx[3].pos = D3DXVECTOR3(SetWidth, 0.0f, -SetHeight);

	float fAvalue = fabsf(player.y / fMaxHeight);

	m_col.a = fAlv - fAvalue;

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// 描画処理
//================================================
void CShadow::Draw(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// Zテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// 透明度オプション追加
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//	ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向き
	D3DXVECTOR3 rot = m_pRot->Get();

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Zテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//===================================================
// 地面に合わせた回転の取得処理
//===================================================
void CShadow::SetFieldAngle(D3DXVECTOR3 Nor, D3DXVECTOR3 up)
{
	D3DXVECTOR3 axis; // 回転軸

	D3DXVec3Normalize(&Nor, &Nor);

	// 軸を求めるて正規化する
	D3DXVec3Cross(&axis, &up, &Nor);
	D3DXVec3Normalize(&axis, &axis);

	// フィールドの法線と上方向ベクトルの内積して角度を求める
	float dot = D3DXVec3Dot(&up, &Nor);
	float fAngle = acosf(dot);

	D3DXMATRIX mtxRot; // 計算用回転行列

	// 任意の軸から回転行列を求める
	D3DXMatrixRotationAxis(&mtxRot, &axis, fAngle);

	// 回転行列をオイラー角に変換
	D3DXVECTOR3 rot = MatrixToEulerXYZ(mtxRot);

	// 設定
	m_pRot->Set(rot);
}
