//===================================================
//
// パリィのエフェクト [ParryEffect.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "ParryEffect.h"
#include "manager.h"
#include "renderer.h"

//===================================================
// コンストラクタ
//===================================================
CParryEffect::CParryEffect()
{
    m_type = TYPE_PARRY;
}

//===================================================
// デストラクタ
//===================================================
CParryEffect::~CParryEffect()
{
}

//===================================================
// 生成処理
//===================================================
CParryEffect* CParryEffect::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size, const D3DXVECTOR3 rot, const int nSegX, const int nSegY, const int nSpeed, const bool bLoop, const TYPE type)
{
    CParryEffect* pEffect = new CParryEffect;

    // パラメータの設定
    pEffect->SetParam(nSegX, nSegY, nSpeed, bLoop);
    pEffect->SetPosition(pos);
    pEffect->SetSize(Size);
    pEffect->SetRotaition(rot);
    pEffect->m_type = type;

    // 初期化処理
    if (FAILED(pEffect->Init()))
    {
        pEffect->Uninit();
        pEffect = nullptr;
        return nullptr;
    }

    return pEffect;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CParryEffect::Init(void)
{
    // 初期化処理
    if (FAILED(CObject3DAnim::Init()))
    {
        return E_FAIL;
    }

    switch (m_type)
    {
    case TYPE_PARRY:
        // IDの設定
        CObject3DAnim::SetTextureID("effect/Hit000.png");
        break;
    case TYPE_ROUND_KICK:
        // IDの設定
        CObject3DAnim::SetTextureID("effect/Hit003.png");
        break;
    case TYPE_SPARK:
        // IDの設定
        CObject3DAnim::SetTextureID("effect/Hit002.png");
        break;
    default:
        break;
    }

    return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CParryEffect::Uninit(void)
{
    // 終了処理
    CObject3DAnim::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CParryEffect::Update(void)
{
    // 更新処理
    CObject3DAnim::Update();
}

//===================================================
// 描画処理
//===================================================
void CParryEffect::Draw(void)
{
    // デバイスの取得
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

    // カリングをオンにする
    pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

    // カリングをオンにする
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    // aブレンディング
    pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    //// Zテスト
    //pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    //pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    // 描画処理
    CObject3DAnim::Draw();

    // aブレンディングをもとに戻す
    pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    //// Zテスト
    //pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    //pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

        // カリングをオンにする
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

    // カリングをオフにする
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
