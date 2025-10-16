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

//***************************************************
// 名前空間
//***************************************************
using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
const char* CBlock::m_pFilePath[CBlock::TYPE_MAX] = // ファイルパス
{
	"wall000.x",
	"wall001.x",
	"wall002.x",
	"pillar000.x",
	"gate000.x",
	"spear_rack.x",
	"sword_rack.x",
	"sword_rack001.x",
	"wepon_rack.x",
	"wooden_box.x",
	"wooden_box001.x",
	"torch001.x",
	"monument.x",
	"monument001.x",
};

//===================================================
// コンストラクタ
//===================================================
CBlock::CBlock()
{
	m_type = TYPE_WALL000;
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
CBlock* CBlock::Create(const D3DXVECTOR3 pos, const TYPE type, const D3DXVECTOR3 rot)
{
	CBlock* pBlock = new CBlock;

	// 位置の設定
	pBlock->SetPosition(pos);
	pBlock->m_type = type;

	// 初期化処理
	if (FAILED(pBlock->Init()))
	{// 初期化に失敗したら

		// 終了処理
		pBlock->Uninit();
		pBlock = nullptr;
		return nullptr;
	}

	// 向きの設定
	pBlock->SetRotation(rot);

	return pBlock;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CBlock::Init(void)
{
	// 省略用パス
	std::string pPath = "data/MODEL/obj/";

	// 文字列をつなげる
	pPath += m_pFilePath[m_type];

	// モデルのロード
	if (FAILED(CObjectX::LoadModel(pPath.c_str())))
	{
		return E_FAIL;
	}

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
	// nullだったら処理しない
	if (m_pAABB == nullptr) return false;

	// nullだったら処理しない
	if (pAABB == nullptr) return false;

	// 矩形の判定
	if (CCollisionAABB::Collision(pAABB, m_pAABB.get(), pPushPos))
	{
		return true;
	}
	return false;
}
