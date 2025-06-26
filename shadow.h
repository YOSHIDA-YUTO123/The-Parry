//================================================
//
// �e [shadow.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _SHADOW_H_
#define _SHADOW_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"main.h"
#include"object.h"
#include"object3D.h"

//*************************************************
// �e�N���X�̒�`
//*************************************************
class CShadow : public CObject3D
{
public:
	CShadow();
	~CShadow();
	static CShadow* Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight,const D3DXCOLOR col);

	void Setting(const D3DXVECTOR3 player, const D3DXVECTOR3 pos,const float fWidth,const float fHeight,const float fMaxHeight,const float fAlv);
	D3DXVECTOR3 GetFieldAngle(D3DXVECTOR3 Nor, D3DXVECTOR3 up);
	void Uninit(void);
private:
	HRESULT Init(void);
	void Update(void);
	void Draw(void);
};
#endif