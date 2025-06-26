//===================================================
//
// �J���� [camera.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _CAMERA_H_
#define _CAMERA_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"

//***************************************************
// �J�����N���X�̒�`
//***************************************************
class CCamera
{
public:

	// �Ǐ]����I�u�W�F�N�g�̎��
	typedef enum
	{
		STATE_NONE = 0,
		STATE_TRACKING,
		STATE_ROCKON,
		STATE_MAX
	}STATE;

	CCamera();
	~CCamera();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void SetCamera(void);
	void MouseWheel(void);
	void MouseView(void);
	void PadView(void);
	void SetState(const STATE state) { m_state = state; }
	void SetTracking(const D3DXVECTOR3 posRDest, const float fSpeed, const float fcoef);

	D3DXVECTOR3 GetRotaition(void) const { return m_rot; }
	D3DXVECTOR3 GetPosV(void) const { return m_posV; }
	D3DXVECTOR3 GetPosR(void) const { return m_posR; }
	void SetRot(const D3DXVECTOR3 rot) { m_rot = rot; }
	void Rockon(D3DXVECTOR3 playerPos, D3DXVECTOR3 enemyPos);
	STATE GetState(void) const { return m_state; }

private:
	STATE m_state;				// �J�����̏��
	D3DXMATRIX m_mtxView;		// �r���[�}�g���b�N�X
	D3DXMATRIX m_mtxProjection;	// �v���W�F�N�V�����}�g���b�N�X
	D3DXVECTOR3 m_posV;			// ���_
	D3DXVECTOR3 m_posR;			// �����_
	D3DXVECTOR3 m_vecU;			// ������x�N�g��
	D3DXVECTOR3 m_rot;			// ����
	D3DXVECTOR3 m_posRDest;		// �ړI�̒����_
	D3DXVECTOR3 m_posVDest;		// �ړI�̎��_
	float m_fDistance;			// ����
};

#endif