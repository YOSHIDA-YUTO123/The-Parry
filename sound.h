//================================================
//
// �T�E���h���� [sound.h]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _SOUND_H_
#define _SOUND_H_

//*************************************************
// �C���N���[�h�t�@�C��
//*************************************************
#include"main.h"

//*************************************************
// �T�E���h�N���X�̒�`
//*************************************************
class CSound
{
public:

	// �T�E���h�̎��
	typedef enum
	{
		SOUND_LABEL_ONE = 0,	// 
		SOUND_LABEL_MAX
	} SOUND_LABEL;

	CSound();
	~CSound();
	HRESULT Init(HWND hWnd);
	void Uninit(void);
	HRESULT PlaySound(SOUND_LABEL label);
	void StopSound(SOUND_LABEL label);
	void StopSound(void);
private:
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
	IXAudio2* m_pXAudio2;									// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice* m_pMasteringVoice;				// �}�X�^�[�{�C�X
	IXAudio2SourceVoice* m_apSourceVoice[SOUND_LABEL_MAX];	// �\�[�X�{�C�X
	BYTE* m_apDataAudio[SOUND_LABEL_MAX];					// �I�[�f�B�I�f�[�^
	DWORD m_aSizeAudio[SOUND_LABEL_MAX];					// �I�[�f�B�I�f�[�^�T�C�Y

	// �T�E���h���\���̂̒�`
	struct SOUND_INFO
	{
		const char* pFilename;  // �t�@�C����
		int nCntLoop;			// ���[�v�J�E���g
	};

	// �T�E���h�̏��
	SOUND_INFO m_aSoundInfo[SOUND_LABEL_MAX] = 
	{
		{"data/BGM/titlebgm.wav", -1},    // �^�C�g��
	};				
};
#endif
