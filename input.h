//================================================
//
// ���͏��� [input.h]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _INPUT_H_
#define _INPUT_H_

//*************************************************
// �C���N���[�h�t�@�C��
//*************************************************
#include"main.h"

//*************************************************
// �}�N����`
//*************************************************
#define MAX_KEYBORAD (256) // �L�[�̍ő吔

//*************************************************
// ���̓N���X�̒�`
//*************************************************
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(HINSTANCE hInstance);
	virtual void Uninit(void);
	virtual void Update(void) = 0;
protected:
	static LPDIRECTINPUT8 m_pInput; // DIrectInput�I�u�W�F�N�g�ւ̃|�C���^
	LPDIRECTINPUTDEVICE8 m_pDevice;	// ���̓f�o�C�X�ւ̃|�C���^
};

//*************************************************
// �L�[�{�[�h�N���X�̒�`
//*************************************************
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nkey);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey, const int nMaxCnt);
	bool GetPressAnyKey(void);

private:
	BYTE m_aKeyState[MAX_KEYBORAD];		// �L�[�{�[�h�̏��
	BYTE m_OldKeyState[MAX_KEYBORAD];	// �O�̃L�[�{�[�h�̏��
	int m_nRepeatCount;					// �L�[�̒������J�E���^�[
	bool m_bFirstPress;					// �ŏ��ɉ���������p�ϐ�
};

//*************************************************
// �p�b�h�̒�`
//*************************************************
class CInputJoypad : public CInput
{
public:
	CInputJoypad();
	~CInputJoypad();

	// �{�^���̎��
	typedef enum
	{
		JOYKEY_UP = 0,					// �\���L�[��
		JOYKEY_DOWN,					// �\���L�[��
		JOYKEY_LEFT,					// �\���L�[��
		JOYKEY_RIGHT,					// �\���L�[�E
		JOYKEY_START,					// START
		JOYKEY_BAKC,					// BACK
		JOYKEY_LEFT_THUMB,
		JOYKEY_RIGHT_THUMB,
		JOYKEY_LEFT_SHOULDER,			// ���g���K�[
		JOYKEY_RIGHT_SHOULDER,			// �E�g���K�[
		JOYKEY_L2,
		JOYKEY_R2,
		JOYKEY_A,						// A�{�^��
		JOYKEY_B,						// B�{�^��
		JOYKEY_X,						// X�{�^��
		JOYKEY_Y,						// Y�{�^��
		JOYKEY_MAX
	}JOYKEY;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	bool GetPress(JOYKEY key);
	bool GetTrigger(JOYKEY key);
	bool GetRelease(JOYKEY key);
	bool GetRepeat(JOYKEY key, const int nMaxCnt);
	bool GetJoyStickL(void);
	bool GetJoyStickR(void);
	XINPUT_STATE* GetJoyStickAngle(void);
	bool GetPressAnyButton(void);
	bool GetTriggerTrigger(JOYKEY Trigger);
	bool GetTriggerPress(JOYKEY Trigger);
private:
	XINPUT_STATE m_joyKeyState;				// �W���C�p�b�h�̃v���X���
	XINPUT_STATE m_OldKeyState;				// �W���C�p�b�h�̃v���X���(����)
	XINPUT_STATE m_joyKeyStateTrigger;		// �W���C�p�b�h�̃g���K�[���
	XINPUT_STATE m_joykeyStateRelease;		// �W���C�p�b�h�̃����[�X���
	int m_nRepeatCount;						// �L�[�̒������J�E���^�[
	bool m_bFirstPress;						// �ŏ��ɉ���������p�ϐ�
};

//*************************************************
// �}�E�X�N���X�̒�`
//*************************************************
class CInputMouse : public CInput
{
public:
	CInputMouse();
	~CInputMouse();

	HRESULT Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool OnMousePress(int button_type);
	bool OnMouseTriggerDown(int button_type);
	bool OnMouseTriggerUp(int button_type);
	bool OnMouseRepet(int button_type, const int nMaxCnt);
	D3DXVECTOR2 GetVelocity(void);
	D3DXVECTOR2 GetOldVelocity(void);
	int GetMouseWeel(void);
private:
	DIMOUSESTATE m_PrevState;		// �}�E�X�̉ߋ��̏��
	DIMOUSESTATE m_CurrentState;	// �}�E�X�̌��݂̏��
	int m_nRepeatCount;				// �L�[�̒������J�E���^�[
	bool m_bFirstPress;				// �ŏ��ɉ���������p�ϐ�
};

#endif // !_INPUT_H_