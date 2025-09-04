//===================================================
//
// 目印 [mark.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "mark.h"

using namespace Const; // 名前空間Constを使用

//***************************************************
// 定数宣言
//***************************************************
constexpr float MAX_WIDTH = 25.0f;	 // 横幅
constexpr float MAX_HEIGHT = 25.0f;	 // 縦幅

//===================================================
// コンストラクタ
//===================================================
CMark::CMark()
{
}

//===================================================
// デストラクタ
//===================================================
CMark::~CMark()
{
}

//===================================================
// 生成処理
//===================================================
CMark* CMark::Create(const D3DXVECTOR3 pos)
{
	CMark* pMark = new CMark;

	if (pMark == nullptr) return nullptr;

	pMark->SetPosition(pos);
	pMark->SetSize(D3DXVECTOR2(MAX_WIDTH, MAX_HEIGHT));
	pMark->Init();

	return pMark;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CMark::Init(void)
{
	// 初期化処理
	if (FAILED(CObjectBillboard::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CObjectBillboard::SetTextureID("data/TEXTURE/UI/mark.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CMark::Uninit(void)
{
	// 終了処理
	CObjectBillboard::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CMark::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CObjectBillboard::GetPosition();

	pos += m_move;

	m_fCounter += 0.1f;

	m_move.y = sinf(m_fCounter) * 2.0f;

	// 位置の設定
	CObjectBillboard::SetPosition(pos);
}

//===================================================
// 描画処理
//===================================================
void CMark::Draw(void)
{
	// 描画処理
	CObjectBillboard::Draw();
}
