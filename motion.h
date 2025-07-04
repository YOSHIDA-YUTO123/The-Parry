//===================================================
//
// ���[�V���� [motion.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _MOTION_H_
#define _MOTION_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"model.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<memory>

//***************************************************
// �}�N����`
//***************************************************
#define MAX_PARTS (56) // �p�[�c�̍ő吔
#define MAX_KEY (20)   // �L�[�̍ő吔

//***************************************************
// �O���錾
//***************************************************
class CModel;
class CMotionLoader;

//***************************************************
// ���[�V�����N���X�̒�`
//***************************************************
class CMotion
{
public:

	// �ǂݍ��݌`��
	typedef enum
	{
		LOAD_TEXT = 0,
		LOAD_BIN,
		LOAD_JSON,
		LOAD_MAX
	}LOAD;

	// �L�[�̍\����
	struct Key
	{
		float fPosX; // �ʒuX
		float fPosY; // �ʒuY
		float fPosZ; // �ʒuZ

		float fRotX; // ����X
		float fRotY; // ����Y
		float fRotZ; // ����Z
	};

	// �L�[���̍\����
	struct Key_Info
	{
		int nFrame;						// �Đ��t���[��
		std::vector <Key> aKey;				// �e�L�[�p�[�c�̃L�[�v�f
	};

	// ���[�V�������̍\����
	struct Info
	{
		bool bLoop;						// ���[�v���邩�ǂ���
		int nNumkey;					// �L�[�̑���
		std::vector<Key_Info> aKeyInfo;		// �L�[���
	};

	CMotion();
	~CMotion();

	static std::unique_ptr<CMotion> Load(const char* pLoadFileName, std::vector<CModel*> &pModel, int* pOutModel, const int nNumMotion, LOAD type);
	void Uninit(void);
	void Update(CModel** pModel, const int nNumModel);
	void SetMotion(const int motiontype, bool bBlend, const int nBlendFrame);
	bool IsEndMotion(void);
	bool IsFinishEndBlend(void);
	int GetType(void) const { return m_nType; }
	int GetBlendType(void) const { return m_nTypeBlend; }
	bool IsEventFrame(const int nStartFrame, const int nEndFrame, const int nType);
	bool IsLoad(void);
	bool FinishLoopMotion(void);

	void Debug(void);
private:
	void UpdateCurrentMotion(CModel** pModel, int nIdx);
	void UpdateBlendMotion(CModel** pModel, int nIdx);
	void FinishFirstBlend(void);

	CMotionLoader* m_pLoader;   // ���[�V�����̃��[�_�[
	std::vector<Info> m_aInfo;		// ���[�V�������ւ̃|�C���^
	int m_nType;				// ���[�V�����̎��
	int m_nNumKey;				// �L�[�̑���
	int m_nKey;					// ���݂̃L�[No.
	int m_nCount;				// ���[�V�����̃J�E���^�[
	int m_nextKey;				// ���̃L�[
	int m_nAllFrame;			// �S�̂̃t���[����
	int m_nAllCounter;			// �S�̃t���[���̃J�E���^�[
	bool m_bLoopMotion;				// ���[�v���邩�ǂ���

	bool m_bBlend;						// �u�����h�����邩�ǂ���
	bool m_bLoopBlend;					// ���[�v���邩�ǂ���
	bool m_bFinish;						// ���[�V�������I��������ǂ���
	bool m_bFirst;						// ���[�V�������n�܂����t���O

	int m_nTypeBlend;					// �u�����h�̎��
	int m_nFrameBlend;					// �u�����h�̃t���[����
	int m_nCounterBlend;				// �u�����h�J�E���^�[
	int m_nNumKeyBlend;					// �u�����h���[�V�����̍ő�̃L�[
	int m_nKeyBlend;					// �u�����h���[�V�����̌��݂̃L�[
	int m_nNextKeyBlend;				// �u�����h���[�V�����̎��̃L�[
	int m_nCounterMotionBlend;			// �u�����h�̃J�E���^�[
};

//***************************************************
// ���[�V�����̃��[�h�N���X�̒�`
//***************************************************
class CMotionLoader
{
public:

	CMotionLoader();
	~CMotionLoader();

	void SetNumModel(const int nNumModel) { m_nNumModel = nNumModel; }
	void SetNumMotion(const int nNumMotion) { m_nNumMotion = nNumMotion; }

	int GetNumModel(void) const { return m_nNumModel; }
	int GetNumMotion(void) const { return m_nNumMotion; }

protected:
	std::vector<CMotion::Info> m_aInfo;  // ���[�V�����̏��
private:
	int m_nNumModel;				// ���f���̍ő吔
	int m_nNumMotion;				// ���[�V�����̑���
};

//***************************************************
// ���[�V�����̃��[�h�N���X(textFile)�̒�`
//***************************************************
class CLoderText : public CMotionLoader
{
public:
	static CLoderText* LoadTextFile(const char* pFileName, std::vector<CMotion::Info>& Info, std::vector<CModel*>& pModel, int* OutNumModel, const int nNumMotion);
private:
	bool LoadModel(std::vector<CModel*>& pModel, int nCnt, std::string input, std::string line);
	bool LoadCharacterSet(std::vector<CModel*>& pModel, std::string line, std::string input);
	void LoadMotionSet(CLoderText* pLoader, std::ifstream& File, std::string nowLine, const int nNumMotion);
	std::istringstream SetInputvalue(std::string input);
};
#endif