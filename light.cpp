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
	m_nNumAll = -1;
	//ZeroMemory(m_aLightInfo, sizeof(m_aLightInfo));
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
	// 要素のクリア
	m_aLightInfo.clear();

	m_nNumAll = 0;

	//// 値のクリア
	//ZeroMemory(m_aLightInfo, sizeof(m_aLightInfo));

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CLight::Uninit(void)
{

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
	pDevice->SetLight(m_nNumAll, &info.aLight);

	// ライトを有効化
	pDevice->LightEnable(m_nNumAll, TRUE);

	// ライトの情報の設定
	m_aLightInfo.push_back(info);

	// 総数を加算
	m_nNumAll++;
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

	info.aLight.Type = D3DLIGHT_POINT;

	// ライトの位置
	info.aLight.Position = pos;

	info.aLight.Diffuse = Diffuse;

	info.aLight.Ambient = Ambient;

	info.aLight.Specular = Diffuse;
	info.aLight.Attenuation0 = 0.0f;
	info.aLight.Attenuation1 = 0.002f;
	info.aLight.Attenuation2 = 0.00f;
	info.aLight.Range = fRange;

	// ライトの設定
	pDevice->SetLight(m_nNumAll, &info.aLight);

	// ライトを有効化
	pDevice->LightEnable(m_nNumAll, TRUE);

	// ライトの情報の設定
	m_aLightInfo.push_back(info);

	// 総数を加算
	m_nNumAll++;
}

//===================================================
// ライトの設定処理
//===================================================
void CLight::SetLight(void)
{
	for (int nCnt = 0; nCnt < m_nNumAll; nCnt++)
	{
		// デバイスの取得
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		// ライトの設定
		pDevice->SetLight(nCnt, &m_aLightInfo[nCnt].aLight);

		// ライトを有効化
		pDevice->LightEnable(nCnt, TRUE);
	}
}
