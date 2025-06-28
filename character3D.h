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

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	bool Hit(int nDamage);

	// �Q�b�^�[
	CPosition* GetPosition(void) const { return m_pPos; }
	CRotation* GetRotation(void) const { return m_pRot; }
	STATE GetState(void) { return m_state; }
	float GetSpeed(void) { return m_fSpeed; }

	// �Z�b�^�[
	void SetCharacter(const int nLife, const float fSpeed);
	void SetState(const STATE state,const int nTime);
private:
	CPosition *m_pPos;		// �ʒu�N���X�ւ̃|�C���^
	CRotation *m_pRot;		// �����N���X�ւ̃|�C���^
	STATE m_state;			// ���
	D3DXMATRIX m_mtxWorld;	// ���[���h�}�g���b�N�X
	float m_fSpeed;			// ���̑���
	int m_nCounterState;	// ��Ԃ̃J�E���^�[
	int m_nLife;			// ����
};

#endif