//================================================
//
// ���b�V���h�[�� [dome.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _DOME_H_
#define _DOME_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"mesh.h"

//************************************************
// ���b�V���h�[���N���X�̒�`
//************************************************
class CMeshDome : public CMesh
{
public:
	CMeshDome();
	~CMeshDome();

	static CMeshDome* Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ, const float fRadius, const float fHeight, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDome(const int nSegX, const int nSegZ, const float fRadius, const float fHeight);
private:
	int m_nOffsetIdx;
};
#endif