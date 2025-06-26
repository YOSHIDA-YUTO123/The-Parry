//===================================================
//
// �L�����N�^�[ [character.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _CHARACTER2D_H_
#define _CHARACTER2D_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"object2D.h"

//***************************************************
// �L�����N�^�[�N���X�̒�`
//***************************************************
class CCharacter2D : public CObject2D
{
public:
	CCharacter2D(int nPriority = 5);
	virtual ~CCharacter2D();

	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetTexture(const char* pFilePath);
	void Flash(const int nCount, const float fSpeed);
	bool HitCharacter(const int nDamage);

private:
	D3DXCOLOR m_col;	// �F
	int m_nIdxTexture;	// �e�N�X�`���̃C���f�b�N�X
	int m_nLife;		// �̗�
};

//***************************************************
// �A�j���[�V��������L�����N�^�[�̃N���X�̒�`
//***************************************************
class CAnimCharacter2D : public CCharacter2D
{
public:
	CAnimCharacter2D();
	~CAnimCharacter2D();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetTexPos(const int nPosX, const int nPosY);
	void SetAnimationSpeed(const int nAnimSpeed = 1) { m_nAnimSpeed = nAnimSpeed; }
	void SetTexDir(const int nOffTexPosH) { m_nOffTexPosH = nOffTexPosH; }
	void StopAnim(const int nPattern) { m_nPatternAnim = nPattern; }

private:
	float m_fPosTexU;		// �e�N�X�`�����WUV
	float m_fPosTexH;		// �e�N�X�`�����WHV
	int m_nOffTexPosH;		// �e�N�X�`�����W�̃I�t�Z�b�gHV
	int m_nAnimSpeed;		// �A�j���[�V�����̑���
	int m_nPatternAnim;		// �A�j���[�V�����̃p�^�[��
	int m_nCounterAnim;		// �A�j���[�V�����̃J�E���^�[
	int m_nPosX;			// �e�N�X�`���̕�����X
	int m_nPosY;			// �e�N�X�`���̕�����Y
};

#endif