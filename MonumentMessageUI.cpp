//===================================================
//
// 石碑ブロックのメッセージ表示処理 [MonumentMessageUI.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "MonumentMessageUI.h"
#include "background.h"

//***************************************************
// 名前空間
//***************************************************
using namespace Const; // 名前空間Constの使用

//***************************************************
// 定数宣言
//***************************************************
constexpr float WIDTH = 640.0f;  // 横幅
constexpr float HEIGHT = 360.0f; // 縦幅
constexpr int FADE_TIME = 60;	 // フェード時間

//===================================================
// コンストラクタ
//===================================================
CMonumentMessageUI::CMonumentMessageUI()
{
	m_bShow = false;
	m_type = TYPE_ONE;
}

//===================================================
// デストラクタ
//===================================================
CMonumentMessageUI::~CMonumentMessageUI()
{
}

//===================================================
// 生成処理
//===================================================
CMonumentMessageUI* CMonumentMessageUI::Create(const D3DXVECTOR3 pos, const TYPE type)
{
	CMonumentMessageUI* pUI = new CMonumentMessageUI;

	pUI->SetPosition(pos);
	pUI->m_type = type;

	// 初期化処理
	if (FAILED(pUI->Init()))
	{
		pUI->Uninit();
		pUI = nullptr;
		return nullptr;
	}

	return pUI;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CMonumentMessageUI::Init(void)
{
	// 初期化処理
	if (FAILED(CFadeInObject2D::Init()))
	{
		return E_FAIL;
	}

	// 位置の取得
	D3DXVECTOR3 pos = CFadeInObject2D::GetPosition();

	// 種類の遷移
	switch (m_type)
	{
	case TYPE_ONE:
		CFadeInObject2D::SetTextureID("data/TEXTURE/UI/monumentMessage000.png");
		break;
	case TYPE_TWO:
		CFadeInObject2D::SetTextureID("data/TEXTURE/UI/monumentMessage001.png");
		break;
	default:
		break;
	}

	// 設定処理
	CFadeInObject2D::SetSize(WIDTH, HEIGHT);
	CFadeInObject2D::SetVtx(WHITE);
	CFadeInObject2D::SetAddAvl(FADE_TIME);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CMonumentMessageUI::Uninit(void)
{
	// 終了処理
	CFadeInObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CMonumentMessageUI::Update(void)
{
	// 更新処理
	CFadeInObject2D::Update();
}

//===================================================
// 描画処理
//===================================================
void CMonumentMessageUI::Draw(void)
{
	// 表示できるなら
	if (m_bShow)
	{
		// 描画処理
		CFadeInObject2D::Draw();
	}
}
