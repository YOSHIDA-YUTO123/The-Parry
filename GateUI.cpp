//===================================================
//
// ゲートのUI [GateUI.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "GateUI.h"
#include"tutorial.h"
#include"player.h"
#include"math.h"

using namespace Const; // 名前空間Constの使用
using namespace math;  // 名前空間mathの使用

//***************************************************
// 定数宣言
//***************************************************
constexpr float USE_RANGE = 200.0f; // 表示する範囲

//===================================================
// コンストラクタ
//===================================================
CGateUI::CGateUI()
{
    m_UsePos = VEC3_NULL;
    m_col = WHITE;
    m_fALvCounter = NULL;
    m_bUse = false;
}

//===================================================
// デストラクタ
//===================================================
CGateUI::~CGateUI()
{
}

//===================================================
// 生成処理
//===================================================
CGateUI* CGateUI::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXVECTOR3 UsePos)
{
    CGateUI* pGateUI = new CGateUI;

    // 初期化処理
    if (FAILED(pGateUI->Init()))
    {
        pGateUI->Uninit();
        pGateUI = nullptr;
        return nullptr;
    }

    // 位置の設定
    pGateUI->SetPosition(pos);
    pGateUI->m_UsePos = UsePos;
    pGateUI->SetSize(Size.x, Size.y);
    pGateUI->SetVtx(WHITE);

    return pGateUI;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CGateUI::Init(void)
{
    // 初期化処理
    if (FAILED(CObject2D::Init()))
    {
        return E_FAIL;
    }

    // テクスチャのIDの設定
    CObject2D::SetTextureID("data/TEXTURE/UI/tutorial_gateUI.png");

    return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CGateUI::Uninit(void)
{
    // 終了処理
    CObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CGateUI::Update(void)
{
    // プレイヤーの取得
    CPlayer* pPlayer = CTutorial::GetPlayer();

    // 取得できなかったら処理しない
    if (pPlayer == nullptr) return;

    // プレイヤーの位置の取得
    D3DXVECTOR3 playerPos = pPlayer->GetPosition();

    // 距離の取得
    float fDistance = GetDistance(playerPos - m_UsePos);

    m_fALvCounter += 0.02f;

    // アルファ値の設定
    m_col.a = 1.0f - fabsf(sinf(m_fALvCounter));

    // 範囲内に入ったら
    if (fDistance <= USE_RANGE)
    {

        m_bUse = true;
    }
    else
    {
        // リセット
        m_fALvCounter = NULL;

        m_col.a = 1.0f;

        m_bUse = false;
    }

    // 色の設定
    CObject2D::SetColor(m_col);
}

//===================================================
// 描画処理
//===================================================
void CGateUI::Draw(void)
{
    // 使用状態だったら
    if (m_bUse)
    {
        // 描画処理
        CObject2D::Draw();
    }
}
