//===================================================
//
// �I�u�W�F�N�g�̕`�揈�� [object.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _OBJECT_H_
#define _OBJECT_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"

//***************************************************
// �}�N����`
//***************************************************
#define VEC3_NULL (D3DXVECTOR3(0.0f,0.0f,0.0f))		// ������
#define VEC2_NULL (D3DXVECTOR2(0.0f,0.0f))			// ������
#define WHITE (D3DXCOLOR(1.0f,1.0f,1.0f,1.0f))		// �F�̏�����
#define MAX_OBJECT (256)							// �I�u�W�F�N�g�̍ő吔
#define FRAME (60)									// ��t���[��
#define NUM_PRIORITY (8)							// �D�揇�ʂ̐�
#define MAX_WORD (5024)								// �ő�̕�����
#define MAX_GRABITY (1.0f)							// �d�͂̑傫��

//***************************************************
// �I�u�W�F�N�g�N���X�̒�`
//***************************************************
class CObject
{
public:

	// �I�u�W�F�N�g�̎��
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_PLAYER,
		TYPE_ENEMY,
		TYPE_BULLET,
		TYPE_BLOCK,
		TYPE_MAX
	}TYPE;

	CObject(int nPriority = 3);
	virtual ~CObject();

	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static int GetNumObject(const int nPriority) { return m_nNumAll[nPriority]; }
	static CObject* GetObject(const int nPriority, const int nIdx);

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	int GetPriority(void) const { return m_nPriority; }
	void SetType(const TYPE type) { m_type = type; }
	TYPE GetType(void) const { return m_type; }
protected:
	void Release(void);
private:
	static CObject* m_apObject[MAX_OBJECT][NUM_PRIORITY];	// �I�u�W�F�N�g�̃|�C���^
	static int	m_nNumAll[NUM_PRIORITY];					// �I�u�W�F�N�g�̑���
	TYPE m_type;											// �I�u�W�F�N�g�̎��
	int	m_nID;												// �I�u�W�F�N�g��ID
	int m_nPriority;										// �D�揇��
};
#endif