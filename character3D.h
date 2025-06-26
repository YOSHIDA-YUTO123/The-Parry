//===================================================
//
// �L�����N�^�[ [character.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _CHARACTER3D_H_
#define _CHARACTER3D_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"objectX.h"
#include"transform.h"

//***************************************************
// �L�����N�^�[3D�N���X�̒�`
//***************************************************
class CCharacter3D : public CObject
{
public:

	// ��Ԃ̎��
	typedef enum
	{
		STATE_NORMAL = 0,
		STATE_MOVE,
		STATE_ACTION,
		STATE_DAMAGE,
		STATE_MAX
	}STATE;

	CCharacter3D(int nPriority = 2);
	~CCharacter3D();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	CPosition* GetPosition(void) const { return m_pPos; }
	CRotation* GetRotation(void) const { return m_pRot; }

	void SetCharacter(const int nLife) { m_nLife = nLife; }
	bool HitCharacter(int nDamage);

	void SetState(const STATE state,const int nTime);
	STATE GetState(void) { return m_state; }
private:
	CPosition *m_pPos;		// �ʒu�N���X�ւ̃|�C���^
	CRotation *m_pRot;		// �����N���X�ւ̃|�C���^
	STATE m_state;			// ���
	int m_nCounterState;	// ��Ԃ̃J�E���^�[

	D3DXMATRIX m_mtxWorld;	// ���[���h�}�g���b�N�X
	int m_nLife;			// ����
};

#endif