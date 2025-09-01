//===================================================
//
// ライト [light.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "light.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// コンストラクタ
//===================================================
CLight::CLight()
{
	ZeroMemory(m_aLightInfo, sizeof(m_aLightInfo));
}

//===================================================
// デストラクタ
//===================================================
CLight::~CLight()
{
	
}

//===================================================
// 初期化処理
//===================================================
HRESULT CLight::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCnt = 0; nCnt < MAX_LIGHT; nCnt++)
	{
		// ライトを無効化
		pDevice->LightEnable(nCnt, FALSE);
	}

	// 値のクリア
	ZeroMemory(m_aLightInfo, sizeof(m_aLightInfo));

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CLight::Uninit(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCnt = 0; nCnt < MAX_LIGHT; nCnt++)
	{
		// ライトを無効化
		pDevice->LightEnable(nCnt, FALSE);
	}

	// 値のクリア
	ZeroMemory(m_aLightInfo, sizeof(m_aLightInfo));
}

//===================================================
// 更新処理
//===================================================
void CLight::Update(void)
{

}

//===================================================
// ライトの設定処理
//===================================================
void CLight::SetDirectional(const D3DXCOLOR Diffuse,const D3DXVECTOR3 dir,const D3DXVECTOR3 pos)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライトの情報
	LightInfo info = {};

	for (int nCnt = 0; nCnt < MAX_LIGHT; nCnt++)
	{
		// 使われていないなら
		if (m_aLightInfo[nCnt].bUse == false)
		{
			info.aLight.Type = D3DLIGHT_DIRECTIONAL;
			info.aLight.Diffuse = Diffuse;

			// ライトの方向ベクトル
			D3DXVECTOR3 Direction = dir;

			// 正規化する
			D3DXVec3Normalize(&Direction, &Direction);

			// 方向ベクトルの代入
			info.aLight.Direction = Direction;

			// 位置の設定
			info.aLight.Position = pos;

			info.bUse = true;

			// ライトの設定
			pDevice->SetLight(nCnt, &info.aLight);

			// ライトを有効化
			pDevice->LightEnable(nCnt, TRUE);

			// 情報を設定
			m_aLightInfo[nCnt] = info;

			break;
		}
	}
}

//===================================================
// ポイントライトの設定処理
//===================================================
void CLight::SetPoint(const D3DXVECTOR3 pos, const float fRange, const D3DXCOLOR Diffuse, const D3DXCOLOR Ambient)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライトの情報
	LightInfo info = {};

	for (int nCnt = 0; nCnt < MAX_LIGHT; nCnt++)
	{
		// 使われていないなら
		if (m_aLightInfo[nCnt].bUse == false)
		{
			info.aLight.Type = D3DLIGHT_POINT;
			info.aLight.Diffuse = Diffuse;

			// ライトの位置
			info.aLight.Position = pos;

			info.aLight.Diffuse = Diffuse;

			info.aLight.Ambient = Ambient;

			info.aLight.Specular = Diffuse;
			info.aLight.Attenuation0 = 0.0f;
			info.aLight.Attenuation1 = 0.002f;
			info.aLight.Attenuation2 = 0.00f;
			info.aLight.Range = fRange;

			info.bUse = true;

			// ライトの設定
			pDevice->SetLight(nCnt, &info.aLight);

			// ライトを有効化
			pDevice->LightEnable(nCnt, TRUE);

			// ライトの情報の設定
			m_aLightInfo[nCnt] = info;

			break;
		}
	}


}

//===================================================
// ライトの設定処理
//===================================================
void CLight::SetLight(void)
{
	for (int nCnt = 0; nCnt < MAX_LIGHT; nCnt++)
	{
		// デバイスの取得
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		// ライトの設定
		pDevice->SetLight(nCnt, &m_aLightInfo[nCnt].aLight);

		// ライトを有効化
		pDevice->LightEnable(nCnt, TRUE);
	}
}

//===================================================
// ディレクションライト以外の消去
//===================================================
void CLight::DeleteLight(void)
{
	// ライトの最大数分調べる
	for (int nCnt = 0; nCnt < MAX_LIGHT; nCnt++)
	{
		// ライトが使われているかつディレクションライト以外だったら
		if (m_aLightInfo[nCnt].bUse == true && m_aLightInfo[nCnt].aLight.Type != D3DLIGHT_DIRECTIONAL)
		{
			// デバイスの取得
			LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

			// ライトを有効化
			pDevice->LightEnable(nCnt, FALSE);
		}
	}
}
