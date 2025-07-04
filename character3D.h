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
class CCharacter3D
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

	CCharacter3D();
	~CCharacter3D();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	bool Hit(int nDamage);

	// �Q�b�^�[
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	CRotation* GetRotation(void) const { return m_pRot; }
	STATE GetState(void) { return m_state; }
	float GetSpeed(void) { return m_fSpeed; }

	// �Z�b�^�[
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCharacter(const int nLife, const float fSpeed);
	void SetState(const STATE state,const int nTime);
private:
	D3DXVECTOR3 m_pos;		// �ʒu
	CRotation *m_pRot;		// �����N���X�ւ̃|�C���^
	STATE m_state;			// ���
	D3DXMATRIX m_mtxWorld;	// ���[���h�}�g���b�N�X
	float m_fSpeed;			// ���̑���
	int m_nCounterState;	// ��Ԃ̃J�E���^�[
	int m_nLife;			// ����
};

#endif