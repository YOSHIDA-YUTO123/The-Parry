//===================================================
//
// �v���R���p�C�� [pch.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _PCH_H_
#define _PCH_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
//#define _CRT_SECURE_NO_WARNINGS		 // scanf�̌x���Ώ��}�N��
#include<windows.h>
#include"d3dx9.h"
#define DIRECTINPUT_VERSION (0x0800) // �r���h���̌x���Ώ��}�N��
#include "dinput.h"
#include"Xinput.h"
#include"xaudio2.h"

//***************************************************
// ���C�u�����̃����N
//***************************************************
#pragma comment(lib,"d3d9.lib")		// �`�揈���ɕK�v
#pragma comment(lib,"d3dx9.lib")	// [d3d9.lib]�̊g�����C�u����
#pragma comment(lib,"dxguid.lib")	// DirectX�R���|�l�[�V����(���i)�g�p�ɕK�v
#pragma comment(lib,"winmm.lib")	// �V�X�e�������擾�ɕK�v
#pragma comment(lib,"dinput8.lib")	// ���͏����ɕK�v
#pragma comment(lib,"xinput.lib")	// �W���C�p�b�h�����ɕK�v

//***************************************************
// �}�N����`
//***************************************************
#define CLASS_NAME "WindowClass"	// �E�C���h�E�N���X�̖��O
#define WINDOW_NAME "The Parry"		// �E�C���h�E�̖��O(�L���v�V�����ɕ\��)
#define SCREEN_WIDTH (1280)			// �E�C���h�E�̕�
#define SCREEN_HEIGHT (720)			// �E�C���h�E�̍���
#define FVF_VERTEX_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define FVF_VERTEX_3D (D3DFVF_XYZ | D3DFVF_NORMAL |  D3DFVF_DIFFUSE | D3DFVF_TEX1) // ���W,�@��,�J���[,�e�N�X�`��

//**********************
// ���_���[2D]�̍\����
//**********************
typedef struct
{
	D3DXVECTOR3 pos;				// ���_���W
	float rhw;						// ���W�ԊҌW��(1.0f�Ŏw��)
	D3DCOLOR col;					// ���_�J���[
	D3DXVECTOR2 tex;				// �e�N�X�`�����W
}VERTEX_2D;

//**********************
// ���_���[3D]�̍\����
//**********************
typedef struct
{
	D3DXVECTOR3 pos;			// ���_���W
	D3DXVECTOR3 nor;			// �@���x�N�g��
	D3DCOLOR col;				// ���_�J���[
	D3DXVECTOR2 tex;			// �e�N�X�`�����W
}VERTEX_3D;

typedef enum
{
	MODE_TITLE = 0,					// �^�C�g�����
	MODE_TUTORIAL,					// �`���[�g���A�����
	MODE_GAME,						// �Q�[�����
	MODE_RESULT,					// ���U���g���
	MODE_RANKING,					// �����L���O���
	MODE_MAX
}MODE;


#endif
