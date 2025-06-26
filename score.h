//================================================
//
// �X�R�A [score.h]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _SCORE_H_
#define _SCORE_H_

//*************************************************
// �C���N���[�h�t�@�C��
//*************************************************
#include"main.h"
#include"object2D.h"
#include "number.h"

//*************************************************
// �}�N����`
//*************************************************
#define MAX_DIGIT (9)			// �ő�̌���
#define MAX_SCORE (999999999)	// �ő�̃X�R�A
#define MIN_SCORE (0)			// �ŏ��̃X�R�A

//*************************************************
// �X�R�A�N���X�̒�`
//*************************************************
class CScore : public CObject
{
public:

	// �X�R�A�̎��
	typedef enum
	{
		TYPE_NORMAL = 0, // ����
		TYPE_LERPER,	 // ���
		TYPE_MAX
	}TYPE;

	CScore(int nPriority = NUM_PRIORITY - 1);
	~CScore();
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR2 GetSize(void) const { return D3DXVECTOR2(m_fWidth,m_fHeight); }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(const float fWidth, const float fHeight);

	static CScore* Create(const TYPE type,const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
	void AddScore(const int value) { m_nScore += value; }
	void SetScore(const int value) { m_nScore = value; }
	int GetScore(void) const { return m_nScore; }
private:
	static CNumber* m_apNumber[MAX_DIGIT];	// �i���o�[�I�u�W�F�N�g�ւ̃|�C���^
	int m_nScore;							// �X�R�A
	D3DXVECTOR3 m_pos;						// �ʒu	
	float m_fWidth, m_fHeight;				// ���A����
};

//*************************************************
// �X�R�A�N���X(���X�ɕ��)�̒�`
//*************************************************
class CScoreLerper : public CScore
{
public:
	CScoreLerper(int nPriority = NUM_PRIORITY - 1);
	~CScoreLerper();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDestScore(const int value,const int nTime);
private:
	int m_nDestScore; // �ړI�̃X�R�A
	int m_nTime;	  // �ړI�̃X�R�A�ɂȂ�܂ł̎���
	int m_nCounter;	  // �J�E���^�[
};
#endif