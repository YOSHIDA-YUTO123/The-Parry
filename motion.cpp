//===================================================
//
// ���[�V���� [motion.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "motion.h"
#include<stdio.h>
#include"object.h"
#include"math.h"
#include "debugproc.h"
#include "manager.h"

//***************************************************
// �}�N����`
//***************************************************
#define NEUTRAL (0) // �j���[�g����

//===================================================
// �R���X�g���N�^
//===================================================
CMotion::CMotion()
{
	//m_aInfo = nullptr;
	m_bLoopMotion = false;
	m_nCount = NULL;
	m_nextKey = NULL;
	m_nKey = NULL;
	m_nNumKey = NULL;
	m_nType = NULL;
	m_nTypeBlend = NULL;
	m_bFinish = false;
	m_bFirst = false;
	m_bBlend = false;
	m_bLoopBlend = false;
	m_nCounterBlend = NULL;
	m_nCounterMotionBlend = NULL;
	m_nNextKeyBlend = NULL;
	m_nFrameBlend = NULL;
	m_nKeyBlend = NULL;
	m_nNumKeyBlend = NULL;
	m_nAllCounter = NULL;
	m_nAllFrame = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CMotion::~CMotion()
{
}

//===================================================
// ���[�V�����̃��[�h����
//===================================================
CMotion* CMotion::Load(const char* pLoadFileName, CModel** ppModel, const int nMaxSize, int* pOutModel,const int nNumMotion, LOAD type)
{
	// ���[�V�����𐶐�
	CMotion* pMotion = new CMotion;

	if (pMotion != nullptr)
	{
		switch (type)
		{
		case LOAD_TEXT:
			// ���[�V�����̃��[�h����(textFile)
			pMotion->m_pLoader = CLoderText::LoadTextFile(pLoadFileName, pMotion->m_aInfo, ppModel, pOutModel, nMaxSize, nNumMotion);
			break;
		default:
			MessageBox(NULL, "���̌`���͓ǂݍ��߂܂���", "�G���[", MB_OK | MB_ICONWARNING);
			break;
		}
	}

	return pMotion;
}

//===================================================
// �C�x���g�t���[���̔���
//===================================================
bool CMotion::IsIventFrame(const int nStartFrame, const int nEndFrame,const int nType)
{
	// �C�x���g�t���[���͈̔͂ɓ�������
	if (m_nAllCounter >= nStartFrame &&
		m_nAllCounter <= nEndFrame &&
		m_nType == nType)
	{
		return true;
	}

	return false;
}

//===================================================
// ���[�h�ł������ǂ���
//===================================================
bool CMotion::GetLoadResult(void)
{
	// �ǂݍ��� ? ����true : ���sfalse
	return m_pLoader ? true : false;
}

//===================================================
// ���[�v���[�V�����̏I������
//===================================================
bool CMotion::FinishLoopMotion(void)
{
	if (m_aInfo[m_nType].bLoop == true && m_aInfo[m_nType].nNumkey - 1 <= m_nKey)
	{
		return true;
	}
	return false;
}

//===================================================
// �f�o�b�O
//===================================================
void CMotion::Debug(void)
{
	// �f�o�b�O���̕`��
	CDebugProc::Print("FRAMEALL = %d / %d\n", m_nAllCounter, m_nAllFrame);
	//CDebugProc::Print(" = %d / %d");
}

//===================================================
// �I������
//===================================================
void CMotion::Uninit(void)
{
	// ���[�_�[�̔j��
	if (m_pLoader != nullptr)
	{
		delete m_pLoader;
		m_pLoader = nullptr;
	}
}

//===================================================
// �ŏ��̃��[�V�����̃u�����h���I�������
//===================================================
void CMotion::FinishFirstBlend(void)
{
	// ���[�V�����̏o�����̃u�����h���I������
	if (m_bFirst == true && m_nCounterBlend >= m_nFrameBlend && m_bFinish == false)
	{
		// ���Ƃɖ߂�
		m_bFirst = false;

		m_nKey = 0;

		// ���[�V�������u�����h�������[�V�����ɂ���
		m_nType = m_nTypeBlend;

		m_nCount = m_nCounterMotionBlend;
		m_nAllCounter = m_nCount;

		m_nCounterBlend = 0;
	}
}

//===================================================
// ���݂̃��[�V�����̍X�V����
//===================================================
void CMotion::UpdateCurrentMotion(CModel** pModel, int nIdx)
{
	// ���̃L�[�̃A�h���X�̎擾
	Key* pKeyNext = &m_aInfo[m_nType].aKeyInfo[m_nextKey].aKey[nIdx];

	// ���݂̃L�[�̃A�h���X�̎擾
	Key* pKey = &m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nIdx];

	// �L�[���̃A�h���X�̎擾
	Key_Info* pKeyInfo = &m_aInfo[m_nType].aKeyInfo[m_nKey];

	// ���̃L�[�܂ł̋��������߂�
	float fDiffPosX = pKeyNext->fPosX - pKey->fPosX;
	float fDiffPosY = pKeyNext->fPosY - pKey->fPosY;
	float fDiffPosZ = pKeyNext->fPosZ - pKey->fPosZ;

	float fDiffRotX = pKeyNext->fRotX - pKey->fRotX;
	float fDiffRotY = pKeyNext->fRotY - pKey->fRotY;
	float fDiffRotZ = pKeyNext->fRotZ - pKey->fRotZ;

	// �p�x�̐��K��
	NormalizeRot(&fDiffRotX);
	NormalizeRot(&fDiffRotY);
	NormalizeRot(&fDiffRotZ);

	// �t���[���ƍő�t���[���̊��������߂�
	float fRateFrame = (float)m_nCount / (float)pKeyInfo->nFrame;

	// ���̃L�[�̈ʒu�܂Ńt���[���ɉ������ʒu��ݒ肷��
	float fPosX = LerpDiff(pKey->fPosX, fDiffPosX, fRateFrame);
	float fPosY = LerpDiff(pKey->fPosY, fDiffPosY, fRateFrame);
	float fPosZ = LerpDiff(pKey->fPosZ, fDiffPosZ, fRateFrame);

	// ���̃L�[�̌����܂Ńt���[���ɉ�����������ݒ肷��
	float fRotX = LerpDiff(pKey->fRotX, fDiffRotX, fRateFrame);
	float fRotY = LerpDiff(pKey->fRotY, fDiffRotY, fRateFrame);
	float fRotZ = LerpDiff(pKey->fRotZ, fDiffRotZ, fRateFrame);

	pModel[nIdx]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));

	pModel[nIdx]->SetRotaition(D3DXVECTOR3(fRotX, fRotY, fRotZ));
}

//===================================================
// �u�����h�̃��[�V�����̍X�V����
//===================================================
void CMotion::UpdateBlendMotion(CModel** pModel, int nIdx)
{
	// ���݂̃L�[�̃A�h���X���擾
	Key* pCurrentKey = &m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nIdx];

	// ���̃L�[�̃A�h���X���擾
	Key* pNextKey = &m_aInfo[m_nType].aKeyInfo[m_nextKey].aKey[nIdx];

	// �L�[���̃A�h���X�̎擾
	Key_Info* pKeyInfo = &m_aInfo[m_nType].aKeyInfo[m_nKey];

	float fRateMotion = (float)m_nCount / (float)pKeyInfo->nFrame; // ���Βl
	float fRateMotionBlend = (float)m_nCounterMotionBlend / (float)m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame;

	float fRateBlend = (float)m_nCounterBlend / (float)m_nFrameBlend;

	// ���݂̃��[�V�����̊p�x�̍���
	float fDiffMotionRX = pNextKey->fRotX - pCurrentKey->fRotX;
	float fDiffMotionRY = pNextKey->fRotY - pCurrentKey->fRotY;
	float fDiffMotionRZ = pNextKey->fRotZ - pCurrentKey->fRotZ;

	// �p�x�̐��K��
	NormalizeRot(&fDiffMotionRX);
	NormalizeRot(&fDiffMotionRY);
	NormalizeRot(&fDiffMotionRZ);

	// ���݂̃��[�V�����̃t���[���ɉ������p�x��ݒ�
	float fRotXCurrent = LerpDiff(pCurrentKey->fRotX, fDiffMotionRX, fRateMotion);
	float fRotYCurrent = LerpDiff(pCurrentKey->fRotY, fDiffMotionRY, fRateMotion);
	float fRotZCurrent = LerpDiff(pCurrentKey->fRotZ, fDiffMotionRZ, fRateMotion);

	// �u�����h�̃L�[�̃A�h���X���擾
	Key* pCurrentBlendKey = &m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nIdx];

	// �u�����h�̎��̃L�[�̃A�h���X���擾
	Key* pNextBlendKey = &m_aInfo[m_nTypeBlend].aKeyInfo[m_nNextKeyBlend].aKey[nIdx];

	//�u�����h���[�V�����̊p�x�̍��������߂�
	float fDiffMotionBlendRX = pNextBlendKey->fRotX - pCurrentBlendKey->fRotX;
	float fDiffMotionBlendRY = pNextBlendKey->fRotY - pCurrentBlendKey->fRotY;
	float fDiffMotionBlendRZ = pNextBlendKey->fRotZ - pCurrentBlendKey->fRotZ;

	// �p�x�̐��K��
	NormalizeRot(&fDiffMotionBlendRX);
	NormalizeRot(&fDiffMotionBlendRY);
	NormalizeRot(&fDiffMotionBlendRZ);

	// �u�����h���[�V�����̃t���[���ɉ������p�x�̐ݒ�
	float fRotXBlend = LerpDiff(pCurrentBlendKey->fRotX, fDiffMotionBlendRX, fRateMotionBlend);
	float fRotYBlend = LerpDiff(pCurrentBlendKey->fRotY, fDiffMotionBlendRY, fRateMotionBlend);
	float fRotZBlend = LerpDiff(pCurrentBlendKey->fRotZ, fDiffMotionBlendRZ, fRateMotionBlend);

	//���݂̃��[�V�����̈ʒu�̍��������߂�
	float fDiffMotionPX = pNextKey->fPosX - pCurrentKey->fPosX;
	float fDiffMotionPY = pNextKey->fPosY - pCurrentKey->fPosY;
	float fDiffMotionPZ = pNextKey->fPosZ - pCurrentKey->fPosZ;

	// ���݂̃��[�V�����̃t���[���ɉ������ʒu�̐ݒ�
	float fPosXCurrent = LerpDiff(pCurrentKey->fPosX, fDiffMotionPX, fRateMotion);
	float fPosYCurrent = LerpDiff(pCurrentKey->fPosY, fDiffMotionPY, fRateMotion);
	float fPosZCurrent = LerpDiff(pCurrentKey->fPosZ, fDiffMotionPZ, fRateMotion);

	// �u�����h���[�V�����̈ʒu�̍��������߂�
	float fDiffMotionBlendPX = pNextBlendKey->fPosX - pCurrentBlendKey->fPosX;
	float fDiffMotionBlendPY = pNextBlendKey->fPosY - pCurrentBlendKey->fPosY;
	float fDiffMotionBlendPZ = pNextBlendKey->fPosZ - pCurrentBlendKey->fPosZ;

	// �u�����h���[�V�����̃t���[���ɉ������ʒu�̐ݒ�
	float fPosXBlend = LerpDiff(pCurrentBlendKey->fPosX, fDiffMotionBlendPX, fRateMotionBlend);
	float fPosYBlend = LerpDiff(pCurrentBlendKey->fPosY, fDiffMotionBlendPY, fRateMotionBlend);
	float fPosZBlend = LerpDiff(pCurrentBlendKey->fPosZ, fDiffMotionBlendPZ, fRateMotionBlend);

	// ���݂̊p�x�ƃu�����h����p�x�̍��������߂�
	float fDiffBlendRX = fRotXBlend - fRotXCurrent;
	float fDiffBlendRY = fRotYBlend - fRotYCurrent;
	float fDiffBlendRZ = fRotZBlend - fRotZCurrent;

	// �u�����h�̃t���[���ɉ����������̐ݒ�
	float fRotX = LerpDiff(fRotXCurrent, fDiffBlendRX, fRateBlend);
	float fRotY = LerpDiff(fRotYCurrent, fDiffBlendRY, fRateBlend);
	float fRotZ = LerpDiff(fRotZCurrent, fDiffBlendRZ, fRateBlend);

	// ���݂̈ʒu�ƃu�����h����ʒu�̍��������߂�
	float fDiffBlendPX = fPosXBlend - fPosXCurrent;
	float fDiffBlendPY = fPosYBlend - fPosYCurrent;
	float fDiffBlendPZ = fPosZBlend - fPosZCurrent;

	// �u�����h�̃t���[���ɉ������ʒu�̐ݒ�
	float fPosX = LerpDiff(fPosXCurrent, fDiffBlendPX, fRateBlend);
	float fPosY = LerpDiff(fPosYCurrent, fDiffBlendPY, fRateBlend);
	float fPosZ = LerpDiff(fPosZCurrent, fDiffBlendPZ, fRateBlend);

	// �ʒu��ݒ�
	pModel[nIdx]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));

	// ������ݒ�
	pModel[nIdx]->SetRotaition(D3DXVECTOR3(fRotX, fRotY, fRotZ));
}

//===================================================
// �X�V����
//===================================================
void CMotion::Update(CModel** pModel,const int nNumModel)
{
	if (GetLoadResult() == false) return;

	for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
	{
		// �L�[�̑���
		m_nNumKey = m_aInfo[m_nType].nNumkey;
		m_nNumKeyBlend = m_aInfo[m_nTypeBlend].nNumkey;

		// ���[�v���邩�ǂ���
		m_bLoopMotion = m_aInfo[m_nType].bLoop;

		//if (motiontype < 0 || motiontype >= TYPE::TYPE_MAX)
		//{
		//	return;
		//}
		
		// ���̃L�[�𑝂₷
		m_nextKey = (m_nKey + 1) % m_aInfo[m_nType].nNumkey;

		// ���̃u�����h�L�[�𑝂₷
		m_nNextKeyBlend = (m_nKeyBlend + 1) % m_nNumKeyBlend;

		if (m_bFinish == false && m_bFirst == false)
		{
			// ���݂̃��[�V�����̍X�V����
			UpdateCurrentMotion(pModel, nCntModel);
		}
		if ((m_bFinish == true || m_bFirst == true) && m_bBlend == true)
		{
			// �u�����h�̃��[�V�����̍X�V����
			UpdateBlendMotion(pModel, nCntModel);
		}
	}	
	
	// ���[�V�������I��������
	if (IsEndMotion())
	{
		m_nCounterMotionBlend = 0;
		m_nKeyBlend = 0;
		m_nAllCounter = 0;
		m_nCounterBlend = 0;
		m_bFinish = true;
		m_nFrameBlend = m_aInfo[m_nType].aKeyInfo[m_nNumKey - 1].nFrame;
		m_nTypeBlend = NEUTRAL;
	}

	// ���[�V�����̏o�����̃u�����h���I������
	FinishFirstBlend();

	// �L�[���ő傩�u�����h�̃J�E���g���ő�ɂȂ���
	if (IsFinishEndBlend() == true)
	{
		m_bFinish = false;				// ���Ƃɖ߂�
		m_bBlend = false;				// ���Ƃɖ߂�
		m_nCount = m_nCounterMotionBlend;	    // �t���[�����u�����h������̃t���[���ɍ��킹��
		m_nAllCounter = m_nCounterMotionBlend;
		m_nType = NEUTRAL;				// ���[�V�����^�C�v���j���[�g�����ɂ���
		m_nCounterBlend = 0;
	}

	// �t���[�����ő�t���[���𒴂�����
	if (m_nCount >= m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
	{
		// �L�[�𑝂₷
		m_nKey++;

		// �͈͓��Ƀ��b�v����
		m_nKey = Wrap(m_nKey, 0, m_aInfo[m_nType].nNumkey - 1);

		// �J�E���^�[�����Z�b�g
		m_nCount = 0;
	}

	// �u�����h�L�[��i�߂�
	if (m_nCounterMotionBlend >= m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame && (m_bFinish == true || m_bFirst == true))
	{
		// �L�[�𑝂₷
		m_nKeyBlend++;

		// �͈͓��Ƀ��b�v����
		m_nKeyBlend = Wrap(m_nKeyBlend, 0, m_aInfo[m_nTypeBlend].nNumkey - 1);

		m_nCounterMotionBlend = 0;
	}

	if (m_bFirst == false)
	{
		m_nAllCounter++;
		m_nCount++;
	}

	// �u�����h���n�܂�����
	if (m_bFinish == true || m_bFirst == true)
	{
		m_nCounterBlend++;
	}


	int nAllFrame = 0;

	for (int nCnt = 0; nCnt < m_aInfo[m_nType].nNumkey; nCnt++)
	{
		nAllFrame += m_aInfo[m_nType].aKeyInfo[nCnt].nFrame;
	}

	m_nAllFrame = nAllFrame;

	// �ő�𒴂�����
	if (m_nAllCounter >= m_nAllFrame)
	{
		m_nCounterMotionBlend++;
		m_nAllCounter = 0;
	}

	CDebugProc::Print("%d / %d\n", m_nAllCounter, m_nAllFrame);
}

//===================================================
// ���[�V�����̐ݒ菈��
//===================================================
void CMotion::SetMotion(const int motiontype,bool bBlend,const int nBlendFrame)
{
	// ������ނ̃��[�V������������
	if (m_nTypeBlend == motiontype || m_nType == motiontype) return;

	// ���[�V�����̑����𒴂��Ă�����
	if (m_pLoader->GetNumMotion() <= motiontype)
	{
		return;
	}

	// �u�����h������Ȃ�
	if (bBlend == true)
	{
		m_nKeyBlend = 0;

		if (m_bFirst == false)
		{
			m_nCounterBlend = 0;		 // �u�����h�J�E���^�[�����Z�b�g
			m_nFrameBlend = nBlendFrame; // �u�����h�t���[����ݒ肷��
			m_bFirst = true;			 // �ŏ��̃u�����h�J�n�t���O��true�ɂ���
		}

		m_bFinish = false;			 // ���[�V�������I����Ă��Ȃ�
		m_nTypeBlend = motiontype;   // �u�����h��̎�ނ�ݒ�
	}
	else
	{
		m_nKey = 0;						// �ŏ�����n�߂�
		m_nCount = 0;					// �ŏ�����n�߂�
		m_nType = motiontype;			// �u�����h���郂�[�V�����̃^�C�v����
		m_nTypeBlend = motiontype;		// �u�����h���郂�[�V�����̃^�C�v����
		m_bFinish = false;
	}

	m_bBlend = bBlend; // �u�����h���邩�ǂ�������
	m_nCounterMotionBlend = 0;
	m_nAllCounter = 0; // �S�̂̃t���[���̃J�E���^�[�����Z�b�g����
}

//===================================================
// ���[�V�������I��������ǂ���
//===================================================
bool CMotion::IsEndMotion(void)
{
	// ���[�V�������I��������
	if (m_bFinish == false &&
		m_nKey >= m_nNumKey - 1 &&
		m_bBlend == true &&
		m_bLoopMotion == false &&
		m_bFirst == false)
	{
		return true;
	}
	return false;
}

//===================================================
// �I���̃u�����h����������������
//===================================================
bool CMotion::IsFinishEndBlend(void)
{
	if (m_bFinish == true && m_nFrameBlend <= m_nCounterBlend && m_bFirst == false)
	{
		return true;
	}
	return false;
}

//===================================================
// �R���X�g���N�^
//===================================================
CMotionLoader::CMotionLoader()
{
	m_nNumModel = NULL;
	m_nNumMotion = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CMotionLoader::~CMotionLoader()
{
}

//===================================================
// �e�L�X�g�t�@�C���̃��[�h����
//===================================================
CLoderText* CLoderText::LoadTextFile(const char* pFileName, vector<CMotion::Info>& Info, CModel** ppModel, int* OutNumModel, const int nMaxSize,const int nNumMotion)
{
	// �t�@�C�������[�h����
	ifstream File(pFileName);
	string line;
	string input;

	int nNumModel = 0;

	D3DXVECTOR3 offset = VEC3_NULL;

	bool bCharacterSet = false;	// �L�����N�^�[�̐ݒ���������ǂ���

	if (File.is_open() == true)
	{
		// ���[�V�����𐶐�
		CLoderText* pLoder = new CLoderText;

		// ���[�V�������\���̂̃������̊m��
		pLoder->m_aInfo.resize(nNumMotion);

		// �Ō�̍s�ɂȂ�܂œǂݍ���
		while (getline(File, line))
		{
			size_t equal_pos = line.find("="); // =�̈ʒu

			// [=] ���������߂�
			input = line.substr(equal_pos + 1);

			// ���f���̃��[�h����
			if (pLoder->LoadModel(ppModel, nMaxSize, nNumModel, input, line))
			{
				if (nNumModel <= nMaxSize - 1)
				{
					nNumModel++;
				}
			}

			// �p�[�c�̐ݒ肪�I����Ė���������
			if (bCharacterSet == false)
			{
				bCharacterSet = pLoder->LoadCharacterSet(ppModel, line, input);
			}

			// ���[�V�����̐ݒ�̓ǂݍ���
			pLoder->LoadMotionSet(pLoder, File, line, nNumMotion);

			// ���[�V�����̐����ő�܂ōs������
			if (pLoder->GetNumMotion() >= nNumMotion)
			{
				break;
			}
		}

		// ���̎󂯓n��
		Info = pLoder->m_aInfo;

		// ���f���̏��̎󂯓n��
		*OutNumModel = nNumModel;

		// �t�@�C�������
		File.close();

		return pLoder;
	}
	else
	{
		// ���b�Z�[�W�{�b�N�X
		MessageBox(NULL, pFileName, "�t�@�C�����J���܂���ł���", MB_OK);

		return nullptr;
	}

}

//===================================================
// ���f���̃��[�h����
//===================================================
bool CLoderText::LoadModel(CModel** ppModel, const int nMaxSize, int nCnt, string input, string line)
{
	if (line.find("MODEL_FILENAME") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���f���̖��O�i�[�p�ϐ�
		string modelName;

		// ���l��������
		value_Input >> modelName;

		// ���f���̖��O����
		const char* MODEL_NAME = modelName.c_str();

		// �T�C�Y�ȏ�ɓǂݍ��ނƃG���[���o�邽�ߐ���
		if (nCnt <= nMaxSize - 1)
		{
			// ���f���̐���
			ppModel[nCnt] = CModel::Create(MODEL_NAME);

			return true;
		}
		else
		{
			MessageBox(NULL, MODEL_NAME, "����ȏ�ǂݍ��߂܂���", MB_OK);
		}
	}
	return false;
}

//===================================================
// �L�����N�^�[�̃��[�h����
//===================================================
bool CLoderText::LoadCharacterSet(CModel** ppModel, string line, string input)
{
	static int nIdx = 0;
	int nNumParts = 0;
	int nParent = 0;
	D3DXVECTOR3 offset = VEC3_NULL;

	if (line.find("NUM_PARTS") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> nNumParts;

		// �p�[�c�̍ő吔��ݒ�
		SetNumModel(nNumParts);
	}

	if (line.find("INDEX") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> nIdx;
	}

	if (line.find("PARENT") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> nParent;

		if (nParent != -1)
		{// �e�����݂��Ă�����
			// �e�̃��f���̐ݒ�
			ppModel[nIdx]->SetParent(ppModel[nParent]);
		}
		else
		{// �e�����݂��Ă��Ȃ�������
			ppModel[nIdx]->SetParent(nullptr);
		}
	}

	if (line.find("POS") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> offset.x;
		value_Input >> offset.y;
		value_Input >> offset.z;

		ppModel[nIdx]->SetOffPos(offset);
	}

	if (line.find("ROT") != string::npos)
	{
		// ���l��ǂݍ��ޏ���
		istringstream value_Input = SetInputvalue(input);

		// ���l��������
		value_Input >> offset.x;
		value_Input >> offset.y;
		value_Input >> offset.z;

		ppModel[nIdx]->SetOffRot(offset);
	}

	if (line.find("END_CHARACTERSET") != string::npos)
	{
		return true;
	}

	return false;
}

//===================================================
// ���[�V�����̃��[�h����
//===================================================
void CLoderText::LoadMotionSet(CLoderText* pLoader, ifstream& File, string nowLine, const int nNumMotion)
{
	string line, input;

	int loop = 0;
	int nKey = 0;
	int nCntModel = 0;

	if (nowLine.find("MOTIONSET") != string::npos)
	{
		while (getline(File, line))
		{
			int nNum = GetNumMotion();

			size_t equal_pos = line.find("="); // =�̈ʒu

			// [=] ���������߂�
			input = line.substr(equal_pos + 1);

			if (line.find("LOOP") != string::npos)
			{
				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >> loop;

				// ���[�v���邩�ǂ���
				m_aInfo[nNum].bLoop = (loop == 1) ? true : false;
			}

			if (line.find("NUM_KEY") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);
				
				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);
				
				// ���l��������
				value_Input >> m_aInfo[nNum].nNumkey;
				
				// �L�[�̑�������
				int nNumKey = m_aInfo[nNum].nNumkey;
				
				m_aInfo[nNum].aKeyInfo.resize(nNumKey);
				
				// �L�[�̃T�C�Y�̊m��
				for (int nCntKey = 0; nCntKey < nNumKey; nCntKey++)
				{
					m_aInfo[nNum].aKeyInfo[nCntKey].aKey.resize(GetNumModel());
				}
			}

			if (line.find("FRAME") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].nFrame;
			}

			if (line.find("POS") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosX;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosY;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosZ;
			}
			if (line.find("ROT") != string::npos)
			{
				// = ���������߂�
				input = line.substr(equal_pos + 1);

				// ���l��ǂݍ��ޏ���
				istringstream value_Input = SetInputvalue(input);

				// ���l��������
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fRotX;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fRotY;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fRotZ;
			}

			if (line.find("END_KEY") != string::npos)
			{
				nCntModel++;

				nCntModel = Clamp(nCntModel, 0, pLoader->GetNumModel() - 1);
			}

			if (line.find("END_KEYSET") != string::npos)
			{
				nKey++;
				nCntModel = NULL;
			}
			if (line.find("END_MOTIONSET") != string::npos)
			{
				nKey = NULL;

				if (nNum <= nNumMotion - 1)
				{
					nNum++;

					SetNumMotion(nNum);
				}
				else
				{
				}


				break;
			}
		}
	}
}

//===================================================
// input���琔�l�����o����
//===================================================
istringstream CLoderText::SetInputvalue(string input)
{
	// �擪�� = ������
	input.erase(0, input.find_first_not_of(" = "));

	// input���琔�l�����o������
	istringstream value_Input(input);

	return value_Input;
}
