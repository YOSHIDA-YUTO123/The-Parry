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
#include "Collision.h"
#include<memory>
#include"character3D.h"
#include<vector>

//***************************************************
// �O���錾
//***************************************************
class CCharacter3D;
class CShadow;
class CMotion;
class CEnemyMotionController;
class CModel;

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
		ACTION_WAIT,	 // �ҋ@
		ACTION_MAX
	}ACTION;

	CEnemyAI();
	~CEnemyAI();
	void Init(std::shared_ptr<CEnemyMotionController> pMotion, const int nBlendFrame = 10, const int nFirstMotion = 0);
	void Update(void);
	int CheckDistance(const D3DXVECTOR3 dest,const D3DXVECTOR3 pos, const float fRadius);
	int IsAttack(void);
	bool Wait(void);
private:
	std::shared_ptr<CEnemyMotionController> m_pMotion;	// �G�̃��[�V�����̐���N���X�̃|�C���^
	ACTION m_Action;			// �G�̍s���p�^�[��
	int m_nNextCounter;			// ���̍s���̒��I�J�E���^�[
	int m_nCounterAction;		// �s���̃J�E���^�[
};

//***************************************************
// �G�N���X�̒�`
//***************************************************
class CEnemy : public CObject
{
public:

	CEnemy();
	~CEnemy();

	static CEnemy* Create(const D3DXVECTOR3 pos = VEC3_NULL, const D3DXVECTOR3 rot = VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump);
	bool CollisionWepon(void);
private:
	void SetParent(const int nCnt);

	std::unique_ptr<CCharacter3D> m_pCharactor;			// �L�����N�^�[�N���X
	std::unique_ptr<CCollisionSphere> m_pSphere;		// �~�̓����蔻��N���X
	std::unique_ptr<CShadow> m_pShadow;					// �e�̃N���X�ւ̃|�C���^
	std::unique_ptr<CEnemyAI> m_pAI;					// �G��AI
	std::shared_ptr<CEnemyMotionController> m_pMotion;	// �G�̃��[�V�����̐���N���X�̃|�C���^
	std::vector<CModel*> m_apModel;						// ���f���N���X�ւ̃|�C���^
	std::shared_ptr<CVelocity> m_pMove;					// �ړ��N���X�̐���
	D3DXMATRIX m_weponMatrix;							// ����̃��[���h�}�g���b�N�X
	int m_nNumModel;									// ���f���̍ő吔
};

//***************************************************
// �G�̃��[�V�����̐���N���X�̒�`
//***************************************************
class CEnemyMotionController
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
		TYPE_DAMAGE,
		TYPE_ACTION2,
		TYPE_MAX
	}TYPE;

	CEnemyMotionController();
	~CEnemyMotionController();
	void Load(std::vector<CModel*>& pModel, int* pOutNumModel); // ���[�V�����̃��[�h

	void Init(std::shared_ptr<CVelocity> pMove);
	void Uninit(void);
	void Update(CModel** ppModel, const int nNumModel);

	// ���[�h�ł������ǂ���
	bool IsLoad(void) const;
	void SetMotion(const int type, bool bBlend, const int nFrameBlend);
	int GetBlendType(void) const;
	int GetType(void) const;
	bool IsEventFrame(const int start, const int end, const TYPE type);
	bool IsFinishEndBlend(void);

	// ���[�V�����̑J��
	void TransitionMotion(CCharacter3D *pCharacter);
	void ConfigMove(CCharacter3D* pCharacter,const D3DXVECTOR3 dest);
private:

	std::unique_ptr<CMotion> m_pMotion;	// ���[�V�����̃N���X�ւ̃|�C���^
	std::shared_ptr<CVelocity> m_pMove;	// �ړ��N���X�̐���
};
#endif