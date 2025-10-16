//===================================================
//
// 空間情報の処理をもつクラス [transform.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "transform.h"
#include "manager.h"
#include "renderer.h"

//===================================================
// コンストラクタ
//===================================================
CTransform::CTransform()
{
	// 値のクリア
	ZeroMemory(&m_Info, sizeof(m_Info));
}

//===================================================
// デストラクタ
//===================================================
CTransform::~CTransform()
{
}

//===================================================
// 位置の更新処理
//===================================================
void CTransform::UpdatePosition(const D3DXVECTOR3 move)
{
	// 前回の位置の保存
	m_Info.posOld = m_Info.pos;

	// 位置の更新
	m_Info.pos += move;
}

//===================================================
// マトリックスの設定
//===================================================
void CTransform::SetMatrix(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_Info.mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_Info.rot.y, m_Info.rot.x, m_Info.rot.z);
	D3DXMatrixMultiply(&m_Info.mtxWorld, &m_Info.mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_Info.pos.x, m_Info.pos.y, m_Info.pos.z);
	D3DXMatrixMultiply(&m_Info.mtxWorld, &m_Info.mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_Info.mtxWorld);
}
