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
#include<memory>

//***************************************************
// �O���錾
//***************************************************
class CInputKeyboard;
class CInputJoypad;
class CMotion;
class CShadow;
class CScoreLerper;
class CCharacter3D;
class CCollisionFOV;
class CPlayerMotionController;

//***************************************************
// �v���C���[�N���X�̒�`
//***************************************************
class CPlayer : public CObject
{
public:

	CPlayer(int nPriority = 3);
	~CPlayer();

	static CPlayer* Create(const D3DXVECTOR3 pos = VEC3_NULL, const D3DXVECTOR3 rot = VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool MoveKeyboard(CInputKeyboard *pKeyboard);
	void MoveJoypad(CInputJoypad* pJoypad);
	CCollisionSphere* GetSphere(void) { return m_pSphere.get(); }
	void UpdateParry(void);
	D3DXVECTOR3 GetPos(void) const { return m_pCharacter3D->GetPosition(); }
	D3DXVECTOR3 GetModelPos(const int nIdx) { return math::GetPositionFromMatrix(m_apModel[nIdx]->GetMatrixWorld()); }
	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff,const float jump);
	bool IsParry(const D3DXVECTOR3 pos);
	CPlayerMotionController* GetMotionController(void) { return m_pMotion.get(); } // ���[�V�����R���g���[���[�̎擾

private:

	std::unique_ptr<CCharacter3D> m_pCharacter3D;		// �L�����N�^�[�N���X
	std::unique_ptr<CCollisionFOV> m_pFOV;				// ���E�̔���
	std::unique_ptr<CCollisionSphere> m_pSphere;		// �~�̓����蔻��
	std::unique_ptr<CPlayerMotionController> m_pMotion;	// �v���C���[�̃��[�V��������̃N���X�̃|�C���^
	CScoreLerper *m_pScore;							// �X�R�A�N���X�ւ̃|�C���^
	std::vector<CModel*> m_apModel;					// ���f���N���X�̃|�C���^

	std::unique_ptr<CShadow> m_pShadow;				// �e�N���X�ւ̃|�C���^
	CVelocity* m_pMove;								// �ړ���
	D3DXVECTOR3 m_posOld;							// �O��̈ʒu

	int m_nParryTime;								// �p���B�̗L������
	int m_nParryCounter;							// �p���B�\�̃J�E���^�[

	int m_nNumModel;								// ���f���̍ő吔
	bool m_bJump;									// �W�����v�ł��邩�ǂ���
	bool m_bDash;									// �����Ă邩�ǂ���
};

//***************************************************
// �v���C���[�̃��[�V�����̐���N���X�̒�`
//***************************************************
class CPlayerMotionController
{
public:

	// ���[�V�����̎��
	typedef enum
	{
		TYPE_NEUTRAL = 0,
		TYPE_MOVE,
		TYPE_ACTION,
		TYPE_JUMP,
		TYPE_LANDING,
		TYPE_DASH,
		TYPE_DAMAGE,
		TYPE_PARRY,
		TYPE_MAX
	}TYPE;

	CPlayerMotionController();
	~CPlayerMotionController();

	void Load(std::vector<CModel*> &pModel,int *pOutNumModel); // ���[�V�����̃��[�h
	void Uninit(void);
	void Update(CModel**ppModel,const int nNumModel);

	// ���[�h�ł������ǂ���
	bool IsLoad(void) const;
	void SetMotion(const int type, bool bBlend, const int nFrameBlend);

	bool IsParryEvent(const int start, const int end);
	int GetBlendType(void) const;
private:

	// ���[�V�����̑J��
	void TransitionMotion(void);
	std::unique_ptr<CMotion> m_pMotion; // ���[�V�����̃N���X�ւ̃|�C���^
};
#endif