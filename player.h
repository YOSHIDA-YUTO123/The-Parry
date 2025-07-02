//===================================================
//
// �v���C���[ [player.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _PLAYER_H_
#define _PLAYER_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"character3D.h"
#include "Collision.h"
#include<vector>
#include"math.h"
#include"model.h"

//***************************************************
// �}�N����`
//***************************************************
#define NUM_PARTS (15) // ���f���̍ő吔

//***************************************************
// �O���錾
//***************************************************
class CInputKeyboard;
class CInputJoypad;
class CMotion;
class CShadow;
class CScoreLerper;

//***************************************************
// �v���C���[�N���X�̒�`
//***************************************************
class CPlayer : public CCharacter3D
{
public:

	// ���[�V�����̎��
	typedef enum
	{
		MOTIONTYPE_NEUTRAL = 0,
		MOTIONTYPE_MOVE,
		MOTIONTYPE_ACTION,
		MOTIONTYPE_JUMP,
		MOTIONTYPE_LANDING,
		MOTIONTYPE_DASH,
		MOTIONTYPE_DAMAGE,
		MOTIONTYPE_PARRY,
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	CPlayer(int nPriority = 3);
	~CPlayer();

	static CPlayer* Create(const D3DXVECTOR3 pos = VEC3_NULL, const D3DXVECTOR3 rot = VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool MoveKeyboard(CInputKeyboard *pKeyboard);
	void MoveJoypad(CInputJoypad* pJoypad);
	void Load(void);
	CCollisionSphere* GetSphere(void);
	void UpdateParry(void);
	D3DXVECTOR3 GetModelPos(const int nIdx) { return math::GetPositionFromMatrix(m_apModel[nIdx]->GetMatrixWorld()); }
	CCollisionFOV* GetFOV(void) { return m_pFOV; }
	void SetMotion(const int type, bool bBlend, const int nFrameBlend);
	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff,const float jump);

private:
	void TransitionMotion(void);					// ���[�V�����̑J��

	CCollisionFOV* m_pFOV;				// ���E�̔���
	CCollisionSphere* m_pCollision;		// �����蔻��
	CMotion *m_pMotion;					// ���[�V�����̃N���X�ւ̃|�C���^
	CScoreLerper *m_pScore;				// �X�R�A�N���X�ւ̃|�C���^
	CModel* m_apModel[NUM_PARTS];		// ���f���N���X�ւ̃|�C���^
	CShadow* m_pShadow;					// �e�N���X�ւ̃|�C���^
	CVelocity* m_pMove;					// �ړ���
	D3DXVECTOR3 m_posOld;				// �O��̈ʒu

	int m_nParryTime;					// �p���B�̗L������
	int m_nParryCounter;				// �p���B�\�̃J�E���^�[

	int m_nNumModel;					// ���f���̍ő吔
	bool m_bJump;						// �W�����v�ł��邩�ǂ���
	bool m_bDash;						// �����Ă邩�ǂ���
};

#endif