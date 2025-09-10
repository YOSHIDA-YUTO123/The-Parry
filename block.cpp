//===================================================
//
// ブロック [block.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "block.h"
#include<string>
#include"Collider.h"
#include"manager.h"
#include"scene.h"
#include "Collision.h"
#include"manager.h"
#include"renderer.h"

using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CBlock::CBlock()
{
	m_CenterPos = VEC3_NULL;
	m_pAABB = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CBlock::~CBlock()
{
}

//===================================================
// 生成処理
//===================================================
CBlock* CBlock::Create(const D3DXVECTOR3 pos, const char* pModelFileName, const D3DXVECTOR3 rot)
{
	CBlock* pBlock = new CBlock;

	// 位置の設定
	pBlock->SetPosition(pos);

	// 省略用パス
	std::string pPath = "data/MODEL/obj/";

	// 文字列をつなげる
	pPath += pModelFileName;

	// モデルのロード
	pBlock->LoadModel(pPath.c_str());

	// 初期化処理
	if (FAILED(pBlock->Init()))
	{// 初期化に失敗したら

		// 終了処理
		pBlock->Uninit();
		pBlock = nullptr;
		return nullptr;
	}

	// 向きの設定
	pBlock->GetRotation()->Set(rot);

	return pBlock;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CBlock::Init(void)
{
	// 初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	if (m_pAABB == nullptr)
	{
		// 位置の取得
		D3DXVECTOR3 pos = CObjectX::GetPosition();

		// 大きさの取得
		D3DXVECTOR3 Size = CObjectX::GetSize();

		// AABBの生成
		m_pAABB = CColliderAABB::Create(pos, pos, Size);
	}
	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CBlock::Uninit(void)
{
	m_pAABB = nullptr;

	// 終了処理
	CObjectX::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CBlock::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	// 大きさの取得
	D3DXVECTOR3 Size = CObjectX::GetSize();

	// 中心座標の設定
	m_CenterPos = pos;
	m_CenterPos.y = pos.y + Size.y * 0.5f;

	if (m_pAABB != nullptr)
	{
		// データの更新
		m_pAABB->UpdateData(m_CenterPos, m_CenterPos);
	}

}

//===================================================
// 描画処理
//===================================================
void CBlock::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);   // テクスチャ1
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);   // ステージ0の結果

		// テクスチャステージステートの設定
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

	// 描画処理
	CObjectX::Draw();

	pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	// もとに戻す
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

//===================================================
// 当たり判定
//===================================================
bool CBlock::Collision(CColliderAABB* pAABB,D3DXVECTOR3 *pPushPos)
{
	// AABBの取得
	auto pCollision = CCollisionAABB::GetInstance();

	// 取得できなかったら処理しない
	if (pCollision == nullptr) return false;

	// nullだったら処理しない
	if (m_pAABB == nullptr) return false;

	// nullだったら処理しない
	if (pAABB == nullptr) return false;

	// 矩形の判定
	if (pCollision->Collision(pAABB, m_pAABB.get(), pPushPos))
	{
		return true;
	}
	return false;
}
