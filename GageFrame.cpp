//================================================
//
// ゲージフレーム [GageFrame.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "GageFrame.h"

using namespace Const; // 名前空間Constを処理

//================================================
// コンストラクタ
//================================================
CGageFrame::CGageFrame(int nPriority) : CObject2D(nPriority)
{
	m_type = TYPE_HP_PLAYER;
}

//================================================
// デストラクタ
//================================================
CGageFrame::~CGageFrame()
{
}

//================================================
// フレームの生成処理
//================================================
CGageFrame* CGageFrame::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const TYPE type)
{
	// フレームの生成
	CGageFrame* pFrame = new CGageFrame;

	// 設定処理
	pFrame->m_type = type;
	pFrame->Init();
	pFrame->SetPosition(pos);
	pFrame->SetSize(Size.x, Size.y);
	pFrame->SetVtx(WHITE);

	return pFrame;
}

//================================================
// 初期化処理
//================================================
HRESULT CGageFrame::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	//switch (m_type)
	//{
	//case TYPE_HP_PLAYER:
	//	// テクスチャのID
	//	SetTextureID("data/TEXTURE/GageFrame/playerHpFrame.png");
	//	break;
	//case TYPE_HP_ENEMY:
	//	// テクスチャのID
	//	SetTextureID("data/TEXTURE/GageFrame/enemyHpgageFrame.png");
	//	break;
	//default:
	//	break;
	//}

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CGageFrame::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//================================================
// 更新処理
//================================================
void CGageFrame::Update(void)
{
}

//================================================
// 描画処理
//================================================
void CGageFrame::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}
