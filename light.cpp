//===================================================
//
// ���C�g [light.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "light.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// �R���X�g���N�^
//===================================================
CLight::CLight()
{
	ZeroMemory(m_aLightInfo, sizeof(m_aLightInfo));
}

//===================================================
// �f�X�g���N�^
//===================================================
CLight::~CLight()
{
}

//===================================================
// ����������
//===================================================
HRESULT CLight::Init(void)
{
	// �l�̃N���A
	ZeroMemory(m_aLightInfo, sizeof(m_aLightInfo));

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CLight::Uninit(void)
{
}

//===================================================
// �X�V����
//===================================================
void CLight::Update(void)
{

}

//===================================================
// ���C�g�̐ݒ菈��
//===================================================
void CLight::SetLight(const D3DLIGHTTYPE type,const D3DXCOLOR Diffuse,const D3DXVECTOR3 dir,const D3DXVECTOR3 pos)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCnt = 0; nCnt < NUM_LIGHT; nCnt++)
	{
		if (m_aLightInfo[nCnt].bUse == false)
		{
			m_aLightInfo[nCnt].aLight.Type = type;
			m_aLightInfo[nCnt].aLight.Diffuse = Diffuse;

			// ���C�g�̕����x�N�g��
			D3DXVECTOR3 Direction = dir;

			// ���K������
			D3DXVec3Normalize(&Direction, &Direction);

			// �����x�N�g���̑��
			m_aLightInfo[nCnt].aLight.Direction = Direction;

			// �ʒu�̐ݒ�
			m_aLightInfo[nCnt].aLight.Position = pos;

			m_aLightInfo[nCnt].bUse = true;

			// ���C�g�̐ݒ�
			pDevice->SetLight(nCnt, &m_aLightInfo[nCnt].aLight);

			// ���C�g��L����
			pDevice->LightEnable(nCnt, TRUE);

			break;
		}
	}
}
