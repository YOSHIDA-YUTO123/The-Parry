//================================================
//
// ���b�V���V�����_�[ [cylinder.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _CYLINDER_H_
#define _CYLINDER_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"mesh.h"

//************************************************
// ���b�V���t�B�[���h�N���X�̒�`
//************************************************
class CMeshCylinder : public CMesh
{
public:
	CMeshCylinder();
	~CMeshCylinder();

	static CMeshCylinder* Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ,const float fRadius, const float fHeight, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetCylinder(const int nSegX, const int nSegZ, const float fRadius, const float fHeight);
private:
};
#endif