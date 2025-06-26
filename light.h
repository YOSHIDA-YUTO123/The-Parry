//===================================================
//
// ���C�g [light.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _LIGHT_H_
#define _LIGHT_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"

//***************************************************
// �}�N����`
//***************************************************
#define NUM_LIGHT (30) // ���C�g�̑���

//***************************************************
// ���C�g�̃N���X�̒�`
//***************************************************
class CLight
{
public:
	CLight();
	~CLight();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void SetLight(const D3DLIGHTTYPE type, const D3DXCOLOR Diffuse, const D3DXVECTOR3 dir, const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetDir(void) const { return m_aLightInfo[0].aLight.Direction; }
private:

	// ���C�g�̏��
	struct LightInfo
	{
		D3DLIGHT9 aLight;	// ���C�g�̍\����
		bool bUse;			// �g�p���
	};

	LightInfo m_aLightInfo[NUM_LIGHT]; // ���C�g�̍\����
};
#endif