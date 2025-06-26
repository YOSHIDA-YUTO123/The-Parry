//===================================================
//
// �G [enemy.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _ENEMY_H_
#define _ENEMY_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"character3D.h"
#include "motion.h"
#include "Collision.h"
class CShadow;

//***************************************************
// �}�N����`
//***************************************************
#define ENEMY_MAX_PARTS (16) // ���f���̍ő吔

//***************************************************
// �G��AI�N���X
//***************************************************
class CEnemyAI
{
public:

	// �G�̍s���̎��
	typedef enum
	{
		ACTION_IDLE = 0, // �������Ȃ�
		ACTION_MOVE,	 // �ړ�
		ACTION_ATTACK,	 // �U��
		ACTION_WAIT,
		ACTION_MAX
	}ACTION;

	CEnemyAI();
	~CEnemyAI();
	void Init(CMotion* pMotion, const int nBlendFrame = 10, const int nFirstMotion = 0);
	void Update(void);
	int CheckDistance(const D3DXVECTOR3 dest,const D3DXVECTOR3 pos, const float fRadius);
	int IsAttack(void);
	bool Wait(void);
private:
	CMotion* m_pMotion;			// ���[�V�����̃N���X�ւ̃|�C���^
	ACTION m_Action;			// �G�̍s���p�^�[��
	int m_nCounterAction;		// �s���̃J�E���^�[
};

//***************************************************
// �G�N���X�̒�`
//***************************************************
class CEnemy : public CCharacter3D
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
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	CEnemy(int nPriority = 3);
	~CEnemy();

	static CEnemy* Create(const D3DXVECTOR3 pos = VEC3_NULL, const D3DXVECTOR3 rot = VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void UpdateMoveMotion(void);
private:
	void TransitionMotion(void);
	void SetParent(const int nCnt);
	void Load(void);

	CCollisionSphere* m_pCollision;		// �����蔻��
	CShadow* m_pShadow;					// �e�̃N���X�ւ̃|�C���^
	CEnemyAI* m_pAI;					// �G��AI
	CMotion* m_pMotion;					// ���[�V�����̃N���X�ւ̃|�C���^
	CModel* m_apModel[ENEMY_MAX_PARTS];	// ���f���N���X�ւ̃|�C���^
	CVelocity* m_pMove;					// �ړ���
	D3DXMATRIX m_weponMatrix;			// ����̃��[���h�}�g���b�N�X
	float m_fSpeed;						// �ړ����x
	int m_nNumModel;					// ���f���̍ő吔
};

#endif