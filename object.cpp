//===================================================
//
// �I�u�W�F�N�g�̕`�揈�� [object.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "object.h"
#include "camera.h"
#include "manager.h"

//***************************************************
// �ÓI�����o�ϐ��̐錾
//***************************************************
CObject* CObject::m_apObject[MAX_OBJECT][NUM_PRIORITY] = {};	// �I�u�W�F�N�g�̃|�C���^
int CObject::m_nNumAll[NUM_PRIORITY] = { NULL };										// �I�u�W�F�N�g�̑���

//===================================================
// �R���X�g���N�^(�I�[�o�[���[�h)
//===================================================
CObject::CObject(int nPriority)
{
	m_nID = NULL;
	m_type = TYPE_NONE;
	m_nPriority = NULL;

	// �I�u�W�F�N�g�̍ő吔����
	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		// �I�u�W�F�N�g��NULL�Ȃ�
		if (m_apObject[nCntObject][nPriority] == nullptr)
		{
			m_apObject[nCntObject][nPriority] = this;	// �������g�̃|�C���^����
			m_nNumAll[nPriority]++;						// �I�u�W�F�N�g�̑������J�E���g�A�b�v
			m_nID = nCntObject;							// �I�u�W�F�N�g��ID��n��
			m_nPriority = nPriority;					// �D�揇��
			break;
		}
	}
}
//===================================================
// �f�X�g���N�^
//===================================================
CObject::~CObject()
{
}
//===================================================
// ���ׂẴI�u�W�F�N�g�̔j��
//===================================================
void CObject::ReleaseAll(void)
{
	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// �I�u�W�F�N�g�̍ő吔����
		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{
			// �I�u�W�F�N�g��NULL�Ȃ�
			if (m_apObject[nCntObject][nCntPriority] != nullptr)
			{
				// �I�u�W�F�N�g�̏I������
				m_apObject[nCntObject][nCntPriority]->Uninit();
			}
		}
	}
}
//===================================================
// ���ׂẴI�u�W�F�N�g�̍X�V����
//===================================================
void CObject::UpdateAll(void)
{
	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// �I�u�W�F�N�g�̐�����
		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{
			// �I�u�W�F�N�g��NULL�Ȃ�
			if (m_apObject[nCntObject][nCntPriority] != nullptr)
			{
				// ���ׂẴI�u�W�F�N�g�̍X�V
				m_apObject[nCntObject][nCntPriority]->Update();
			}
		}
	}
}
//===================================================
// ���ׂẴI�u�W�F�N�g�̕`�揈��
//===================================================
void CObject::DrawAll(void)
{
	// �J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	pCamera->SetCamera();

	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// �I�u�W�F�N�g�̐�����
		for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
		{
			// �I�u�W�F�N�g��NULL�Ȃ�
			if (m_apObject[nCntObject][nCntPriority] != nullptr)
			{
				// ���ׂẴI�u�W�F�N�g�̍X�V
				m_apObject[nCntObject][nCntPriority]->Draw();
			}
		}
	}
}
//===================================================
// �I�u�W�F�N�g�̎擾����
//===================================================
CObject* CObject::GetObject(const int nPriority, const int nIdx)
{
	// �I�u�W�F�N�g�̃C���f�b�N�X��Ԃ�
	return m_apObject[nIdx][nPriority];
}

//===================================================
// �I�u�W�F�N�g�̔j��
//===================================================
void CObject::Release(void)
{
	// �������g��ID���擾
	int nID = this->m_nID;

	// �����̗D�揇��
	int nPriority = this->m_nPriority;

	// �I�u�W�F�N�g�j��
	if (m_apObject[nID][nPriority] != nullptr)
	{
		// �������̉��
		delete m_apObject[nID][nPriority];

		m_apObject[nID][nPriority] = nullptr;

		// ���������炷
		m_nNumAll[nPriority]--;
	}
}
