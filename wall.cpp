//===================================================
//
// 3Dポリゴンの壁を設置 [wall.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "wall.h"
#include "manager.h"
#include "renderer.h"
#include "textureManager.h"
#include "GameCamera.h"
#include "tutorial.h"
#include "math.h"

using namespace Const; // 名前空間Constを使用
using namespace math;  // 名前空間mathを使用

//===================================================
// コンストラクタ
//===================================================
CWall::CWall()
{
	m_pVtxBuffer = nullptr;
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
	m_Size = VEC3_NULL;
	m_nTextureIdx = -1;
	D3DXMatrixIdentity(&m_mtxWorld);
}

//===================================================
// デストラクタ
//===================================================
CWall::~CWall()
{
}

//===================================================
// 生成処理
//===================================================
CWall* CWall::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size, const D3DXVECTOR3 rot)
{
	CWall* pWall = new CWall;

	pWall->m_pos = pos;
	pWall->m_Size = Size;
	pWall->m_rot = rot;

	// 初期化処理
	if (FAILED(pWall->Init()))
	{
		pWall->Uninit();
		pWall = nullptr;
		return nullptr;
	}

	return pWall;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CWall::Init(void)
{
	// 初期化処理
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

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
	D3DXVECTOR3 Size = m_Size;

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, -Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, -Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, Size.z);

	// 法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// 頂点カラーの設定
	pVtx[0].col = WHITE;
	pVtx[1].col = WHITE;
	pVtx[2].col = WHITE;
	pVtx[3].col = WHITE;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CWall::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != NULL)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = NULL;
	}

	// 自分自身の破棄
	CObject::Release();
}

//===================================================
// 更新処理
//===================================================
void CWall::Update(void)
{
	// カメラの取得
	CGameCamera* pCamera = CTutorial::GetCamera();

	// カメラが取得できてなかったら
	if (pCamera == nullptr) return;

	// 位置の取得
	D3DXVECTOR3 cameraPos = pCamera->GetPosV();

	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 vtx0, vtx1, vtx2;

	vtx0.x = m_pos.x - cosf(m_rot.y) * m_Size.x;
	vtx0.y = m_pos.y + m_Size.y;
	vtx0.z = m_pos.z + sinf(m_rot.y) * m_Size.x;

	vtx1.x = m_pos.x + cosf(m_rot.y) * m_Size.x;
	vtx1.y = m_pos.y + m_Size.y;
	vtx1.z = m_pos.z - sinf(m_rot.y) * m_Size.x;

	vtx2.x = m_pos.x - sinf(m_rot.y) * m_Size.x;
	vtx2.y = m_pos.y - m_Size.y;
	vtx2.z = m_pos.z - cosf(m_rot.y) * m_Size.x;

	// 壁の辺のベクトル
	D3DXVECTOR3 VecWalledge = GetVector(vtx1, vtx0);

	// カメラまでのベクトル
	D3DXVECTOR3 VecCameraPos = GetVector(cameraPos, vtx0);

	D3DXVECTOR3 cross;

	// 二つのベクトルの外積
	D3DXVec3Cross(&cross, &VecWalledge, &VecCameraPos);

	// 壁の辺のベクトル2
	D3DXVECTOR3 VecWallEdge2 = GetVector(vtx2, vtx0);

	// 壁の法線
	D3DXVECTOR3 normal;

	// 壁の法線を求める
	D3DXVec3Cross(&normal, &VecWalledge, &VecWallEdge2);

	CDebugProc::Print("外積 = [ %.3f,%.3f,%.3f ]\n", cross.x, cross.y, cross.z);

	// カメラが壁の外に出た
	if (cross.y <= 0.0f)
	{
		// カメラの前回の位置
		D3DXVECTOR3 CameraPosOld = pCamera->GetPosOld();

		// カメラの進行ベクトル
		D3DXVECTOR3 VecCameraMove = GetVector(cameraPos, CameraPosOld);

		// 法線とカメラの進行ベクトルの内積
		float fDot = D3DXVec3Dot(&VecCameraMove, &normal);

		// 壁づり方向を設定
		D3DXVECTOR3 NewMove = VecCameraMove - fDot * normal;

		D3DXVECTOR3 diffCameraToEdge = cameraPos - vtx0;

		float fDepth = fabsf(D3DXVec3Dot(&diffCameraToEdge, &normal));

		cameraPos += -VecCameraPos * fDepth;

		D3DXVECTOR3 posR = pCamera->GetPosR();

		// 状態の取得
		CGameCamera::STATE state = pCamera->GetState();

		if (state == CGameCamera::STATE_ROCKON)
		{
			// カメラの位置の設定
			pCamera->LerpPos(posR, cameraPos, 0.5f);
		}
		else
		{
			// カメラの位置の設定
			pCamera->LerpPos(posR, cameraPos, 1.0f);
		}
		//pCamera->UpdatePositionV();
		//pCamera->UpdatePositionR();
	}

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

}

//===================================================
// 描画処理
//===================================================
void CWall::Draw(void)
{
#ifdef _DEBUG

	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// ライトの影響を受けない
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//	ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向き
	D3DXVECTOR3 rot = m_rot;

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置
	D3DXVECTOR3 pos = m_pos;

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
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

	// ライトの影響を受けない
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
#endif // _DEBUG
}
