//==============================================
//
// �e�N�X�`���}�l�[�W���[ [textureManager.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// ���d�C���N���[�h�h�~
//**********************************************
#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

//**********************************************
// �C���N���[�h�t�@�C��
//**********************************************
#include"main.h"
#include "object.h"

//**********************************************
// �}�N����`
//**********************************************
#define MAX_TEXTURE (128) // �e�N�X�`���̍ő吔

//**********************************************
// �e�N�X�`���̃}�l�[�W���[�N���X�̒�`
//**********************************************
class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	int Register(const char* pFilename);
	int RegisterX(const char* pFilename);
	LPDIRECT3DTEXTURE9 GetAdress(int nIdx);
	HRESULT Load(void);
	void UnLoad(void);
private:

	// �e�N�X�`���̏��̒�`
	struct TextureInfo
	{
		LPDIRECT3DTEXTURE9 pTexture;			 // �e�N�X�`���ւ̃|�C���^
		char filepath[MAX_WORD];				 // �t�@�C���p�X
	}; 

	TextureInfo m_apTextureInfo[MAX_TEXTURE];	 // �e�N�X�`���̏��
	static int m_nNumAll;						 // �e�N�X�`���̔ԍ�
};

#endif