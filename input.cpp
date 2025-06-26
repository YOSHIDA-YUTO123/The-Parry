//================================================
//
// ���͏��� [input.cpp]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// �C���N���[�h�t�@�C��
//*************************************************
#include "input.h"

//*************************************************
// �}�N����`
//*************************************************
#define DEAD_ZONE (10000) // �f�b�h�]�[��

//*************************************************
// �ÓI�����o�ϐ��̐錾
//*************************************************
LPDIRECTINPUT8 CInput::m_pInput = NULL; // DIrectInput�I�u�W�F�N�g�ւ̃|�C���^

//===============================================
// ���͏����̃R���X�g���N�^
//===============================================
CInput::CInput()
{
	m_pDevice = NULL;
	m_pInput = NULL;
}
//===============================================
// ���͏����̃f�X�g���N�^
//===============================================
CInput::~CInput()
{

}
//===============================================
// ���͏����̏���������
//===============================================
HRESULT CInput::Init(HINSTANCE hInstance)
{
	// NULL��������
	if (m_pInput == NULL)
	{
		// DirectInput�I�u�W�F�N�g�̐���
		if (FAILED(DirectInput8Create(hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&m_pInput,
			NULL)))
		{
			// ���s
			return E_FAIL;
		}
	}

	return S_OK;
}
//===============================================
// ���͏����̏���������
//===============================================
void CInput::Uninit(void)
{
	// ���̓f�o�C�X(�L�[�{�[�h)�̔j��
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();	// �A�N�Z�X���̔j��
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	// DirectInput�I�u�W�F�N�g�̔j��
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}
//===============================================
// �L�[�{�[�h�̃R���X�g���N�^
//===============================================
CInputKeyboard::CInputKeyboard()
{
	// �L�[���̏�����
	memset(&m_aKeyState, NULL, sizeof(m_aKeyState));

	// �O�̃L�[���̏�����
	memset(&m_OldKeyState, NULL, sizeof(m_OldKeyState));

	// �L�[�̒������J�E���^�[
	m_nRepeatCount = 0;

	// �ŏ��ɉ��������ǂ���
	m_bFirstPress = false;
}
//===============================================
// �L�[�{�[�h�̃f�X�g���N�^
//===============================================
CInputKeyboard::~CInputKeyboard()
{
}
//===============================================
// �L�[�{�[�h�̏���������
//===============================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	// ���͂̏���������
	CInput::Init(hInstance);

	// ���̓f�o�C�X����
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// �������[�h��ݒ�
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// �L�[�{�[�h�̃A�N�Z�X�����l��
	m_pDevice->Acquire();

	return S_OK;
}
//===============================================
// �L�[�{�[�h�̏I������
//===============================================
void CInputKeyboard::Uninit(void)
{
	// �I������
	CInput::Uninit();
}
//===============================================
// �L�[�{�[�h�̍X�V����
//===============================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[MAX_KEYBORAD];		// �L�[�{�[�h�̓��͏��

	for (int nCntkey = 0; nCntkey < MAX_KEYBORAD; nCntkey++)
	{
		m_OldKeyState[nCntkey] = m_aKeyState[nCntkey];	// �L�[�{�[�h�̃v���X����ۑ�
	}

	//���̓f�o�C�X����f�[�^���擾
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (int nCntkey = 0; nCntkey < MAX_KEYBORAD; nCntkey++)
		{
			m_aKeyState[nCntkey] = aKeyState[nCntkey];	// �L�[�{�[�h�̃v���X����ۑ�
		}
	}
	else
	{
		m_pDevice->Acquire();			// �L�[�{�[�h�̃A�N�Z�X�����l��
	}
}
//===============================================
// �L�[�{�[�h�̃v���X����
//===============================================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true : false;
}
//===============================================
// �L�[�{�[�h�̃g���K�[����
//===============================================
bool CInputKeyboard::GetTrigger(int nkey)
{
	// ����p�t���O
	bool Trigger = false;

	if (m_aKeyState[nkey] & 0x80 && !(m_OldKeyState[nkey] & 0x80))
	{
		Trigger = true;
	}

	return Trigger;
}
//===============================================
// �L�[�{�[�h�̃����[�X����
//===============================================
bool CInputKeyboard::GetRelease(int nKey)
{
	// ����p�t���O
	bool Trigger = false;
	if (m_OldKeyState[nKey] & 0x80 && !(m_aKeyState[nKey] & 0x80))
	{
		Trigger = true;
	}

	return Trigger;
}

//===============================================
// �L�[�{�[�h�̌p������(�A�ˏ���)
//===============================================
bool CInputKeyboard::GetRepeat(int nKey,const int nMaxCnt)
{
	// �L�[�������Ă�����
	if (GetPress(nKey))
	{
		if (m_bFirstPress == false)
		{
			m_bFirstPress = true;
			return true;
		}

		// �L�[�̃J�E���g���ő�܂ōs������
		if (m_nRepeatCount >= nMaxCnt)
		{
			// �J�E���g�����Z�b�g
			m_nRepeatCount = 0;

			// ����J�n
			return true;
		}
		else
		{
			// �J�E���g��i�߂�
			m_nRepeatCount++;
		}
	}
	else
	{
		// �J�E���g�����Z�b�g
		m_nRepeatCount = 0;
		m_bFirstPress = false;
	}

	return false;
}

//===============================================
// �����L�[�������Ɣ�������
//===============================================
bool CInputKeyboard::GetPressAnyKey(void)
{
	// �L�[�̍ő吔����
	for (int nCnt = 0; nCnt < MAX_KEYBORAD; nCnt++)
	{
		if (m_aKeyState[nCnt] != 0) return true;
	}
	return false;
}
//===============================================
// �p�b�h�̃R���X�g���N�^
//===============================================
CInputJoypad::CInputJoypad()
{
	// �������̃N���A
	memset(&m_joyKeyState, 0, sizeof(XINPUT_STATE));

	// �������̃N���A
	memset(&m_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));

	// �ŏ��ɃL�[��������������
	m_bFirstPress = false;
	m_nRepeatCount = 0;
}
//===============================================
// �p�b�h�̃f�X�g���N�^
//===============================================
CInputJoypad::~CInputJoypad()
{
}
//===============================================
// �p�b�h�̏���������
//===============================================
HRESULT CInputJoypad::Init(void)
{	
	// �������̃N���A
	memset(&m_joyKeyState, 0, sizeof(XINPUT_STATE));

	// �������̃N���A
	memset(&m_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));

	// �������̃N���A
	memset(&m_joykeyStateRelease, 0, sizeof(XINPUT_STATE));

	// �������̃N���A
	memset(&m_OldKeyState, 0, sizeof(XINPUT_STATE));

	// Xinput�̃X�e�[�g��ݒ�(�L����)
	XInputEnable(true);

	return S_OK;
}
//===============================================
// �p�b�h�̏I������
//===============================================
void CInputJoypad::Uninit(void)
{
	// Xinput�̃X�e�[�g��ݒ�(������)
	XInputEnable(false);
}
//===============================================
// �p�b�h�̍X�V����
//===============================================
void CInputJoypad::Update(void)
{
	XINPUT_STATE joykeyState;			// ���͏����擾

	//�W���C�p�b�h�̏�Ԃ��擾.
	if (XInputGetState(0, &joykeyState) == ERROR_SUCCESS)
	{
		WORD Button = joykeyState.Gamepad.wButtons;				// �������Ƃ��̓��͏��
		WORD OldButton = m_joyKeyState.Gamepad.wButtons;		// 1F�O�̓��͏��

		m_joyKeyStateTrigger.Gamepad.wButtons = Button & ~OldButton;
		m_joykeyStateRelease.Gamepad.wButtons = OldButton & ~Button;

		m_joyKeyState = joykeyState;							// �W���C�p�b�h�̃v���X����ۑ�(�i�[)
	}
}
//===============================================
// �p�b�h�̃v���X�̎擾����
//===============================================
bool CInputJoypad::GetPress(JOYKEY key)
{
	return (m_joyKeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}
//===============================================
// �p�b�h�̃g���K�[�̎擾����
//===============================================
bool CInputJoypad::GetTrigger(JOYKEY key)
{
	return (m_joyKeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}
//===============================================
// �p�b�h�̃����[�X�̎擾����
//===============================================
bool CInputJoypad::GetRelease(JOYKEY key)
{
	return (m_joykeyStateRelease.Gamepad.wButtons & (0x01 << key)) ? true : false;
}
//===============================================
// �p�b�h�̃��s�[�g����
//===============================================
bool CInputJoypad::GetRepeat(JOYKEY key, const int nMaxCnt)
{
	// �L�[�������Ă�����
	if (GetPress(key))
	{
		if (m_bFirstPress == false)
		{
			m_bFirstPress = true;
			return true;
		}

		// �L�[�̃J�E���g���ő�܂ōs������
		if (m_nRepeatCount >= nMaxCnt)
		{
			// �J�E���g�����Z�b�g
			m_nRepeatCount = 0;

			// ����J�n
			return true;
		}
		else
		{
			// �J�E���g��i�߂�
			m_nRepeatCount++;
		}
	}
	else
	{
		// �J�E���g�����Z�b�g
		m_nRepeatCount = 0;
		m_bFirstPress = false;
	}

	return false;

}
//===============================================
// �p�b�h��L�X�e�B�b�N����
//===============================================
bool CInputJoypad::GetJoyStickL(void)
{
	//���[�J���ϐ��錾
	bool joykey = false;

	//���S����Ȃ�������
	if (m_joyKeyState.Gamepad.sThumbLX >= DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbLX <= -DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbLY >= DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbLY <= -DEAD_ZONE)
	{
		joykey = true;
	}

	return joykey;
}

//===============================================
// �p�b�h��R�X�e�B�b�N����
//===============================================
bool CInputJoypad::GetJoyStickR(void)
{
	//���[�J���ϐ��錾
	bool joykey = false;

	//���S����Ȃ�������
	if (m_joyKeyState.Gamepad.sThumbRX >= DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbRX <= -DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbRY >= DEAD_ZONE ||
		m_joyKeyState.Gamepad.sThumbRY <= -DEAD_ZONE)
	{
		joykey = true;
	}

	return joykey;
}

//===============================================
// �p�b�h�̃X�e�B�b�N���̎擾����
//===============================================
XINPUT_STATE* CInputJoypad::GetJoyStickAngle(void)
{
	return &m_joyKeyState;
}
//===============================================
// �����{�^���������Ɣ�������
//===============================================
bool CInputJoypad::GetPressAnyButton(void)
{
	if (m_joyKeyState.Gamepad.wButtons != 0) return true;

	return false;
}
//===============================================
// �p�b�h�̃g���K�[�̃g���K�[����
//===============================================
bool CInputJoypad::GetTriggerTrigger(JOYKEY Trigger)
{
	static bool bRightTriggerPressed = false;  // �E�g���K�[�������ꂽ���ǂ���
	static bool bLeftTriggerPressed = false;   // ���g���K�[�������ꂽ���ǂ���

	// �������l�i�Ⴆ�΁A150�ȏ�ŉ����ꂽ�Ƃ݂Ȃ��j
	const BYTE triggerThreshold = 150;

	// �W���C�p�b�h�̏�Ԃ��擾
	if (XInputGetState(0, &m_joyKeyStateTrigger) == ERROR_SUCCESS)
	{
		// ���݂̃g���K�[�̒l���擾
		BYTE rightTrigger = m_joyKeyStateTrigger.Gamepad.bRightTrigger;
		BYTE leftTrigger = m_joyKeyStateTrigger.Gamepad.bLeftTrigger;

		switch (Trigger)
		{
		case CInputJoypad::JOYKEY_R2:
			// �E�g���K�[��������Ă���A�܂��������Ă��Ȃ��ꍇ
			if (rightTrigger >= triggerThreshold && !bRightTriggerPressed)
			{
				// �E�g���K�[�������ꂽ���Ƃ��L�^
				bRightTriggerPressed = true;

				return true;
			}
			break;
		case CInputJoypad::JOYKEY_L2:
			// ���g���K�[��������Ă���A�܂��������Ă��Ȃ��ꍇ
			if (leftTrigger >= triggerThreshold && !bLeftTriggerPressed)
			{
				// ���g���K�[�������ꂽ�Ƃ��̏���
				// �����ɏ������L�q

				// ���g���K�[�������ꂽ���Ƃ��L�^
				bLeftTriggerPressed = true;

				return true; // ���g���K�[�������ꂽ�ꍇ��true��Ԃ�
			}
			break;
		default:
			break;
		}
	}

	// �g���K�[�������ꂽ�ꍇ�A�t���O�����Z�b�g
	if (m_joyKeyStateTrigger.Gamepad.bRightTrigger < triggerThreshold)
	{
		bRightTriggerPressed = false;
	}

	if (m_joyKeyStateTrigger.Gamepad.bLeftTrigger < triggerThreshold)
	{
		bLeftTriggerPressed = false;
	}	return false;
}
//===============================================
// �p�b�h�̃g���K�[�̃v���X����
//===============================================
bool CInputJoypad::GetTriggerPress(JOYKEY key)
{
	// �W���C�p�b�h�̏�Ԃ��擾
	XInputGetState(0, &m_joyKeyState);

	switch (key)
	{
	case CInputJoypad::JOYKEY_R2:
		if (m_joyKeyState.Gamepad.bRightTrigger) return true;
		break;
	case CInputJoypad::JOYKEY_L2:
		if (m_joyKeyState.Gamepad.bLeftTrigger) return true;
		break;
	default:
		break;
	}
	return false;
}
//===============================================
// �}�E�X�̃R���X�g���N�^
//===============================================
CInputMouse::CInputMouse()
{
	memset(&m_CurrentState, NULL, sizeof(m_CurrentState));
		
	memset(&m_PrevState, NULL, sizeof(m_PrevState));

	m_bFirstPress = false;
	m_nRepeatCount = 0;
}
//===============================================
// �}�E�X�̃f�X�g���N�^
//===============================================
CInputMouse::~CInputMouse()
{

}
//===============================================
// �}�E�X�̏���������
//===============================================
HRESULT CInputMouse::Init(HWND hWnd)
{
	//// ����������
	//CInput::Init(hInstance, hWnd);

	if (FAILED(m_pInput->CreateDevice
	(
		GUID_SysMouse,
		&m_pDevice,
		NULL)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse)))
	{
		return E_FAIL;
	}

	// �������[�h�̐ݒ�
	if (FAILED(m_pDevice->SetCooperativeLevel(
		hWnd,
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
	{
		return E_FAIL;
	}

	//�L�[�{�[�h�̃A�N�Z�X�����l��
	m_pDevice->Acquire();

	return S_OK;
}
//===============================================
// �}�E�X�̏I������
//===============================================
void CInputMouse::Uninit(void)
{
	// �I������
	CInput::Uninit();

	//���̓f�o�C�X(�}�E�X)�̔j��
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();//�L�[�{�[�h�ւ̃A�N�Z�X�������
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	//DirectInput�I�u�W�F�N�g�̍U��
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}
//===============================================
// �}�E�X�̍X�V����
//===============================================
void CInputMouse::Update(void)
{
	m_PrevState.lX = m_CurrentState.lX;
	m_PrevState.lY = m_CurrentState.lY;

	// �X�V�O�ɍŐV�}�E�X����ۑ�����
	m_PrevState = m_CurrentState;

	// �ŐV�̃}�E�X�̏�Ԃ��X�V
	HRESULT	hr = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_CurrentState);

	if (FAILED(hr))
	{
		m_pDevice->Acquire();
	}

	// �}�E�X���W���擾����
	POINT p;
	GetCursorPos(&p);

	// �X�N���[�����W���N���C�A���g���W�ɕϊ�����
	ScreenToClient(FindWindowA(CLASS_NAME, WINDOW_NAME), &p);

	m_CurrentState.lX = p.x;
	m_CurrentState.lY = p.y;
}
//===============================================
// �}�E�X�̃v���X����
//===============================================
bool CInputMouse::OnMousePress(int button_type)
{
	return (m_CurrentState.rgbButtons[button_type] & 0x80) ? true : false;
}
//===============================================
// �}�E�X�̃v���X����
//===============================================
bool CInputMouse::OnMouseTriggerDown(int button_type)
{
	if (!(m_PrevState.rgbButtons[button_type] & (0x80)) &&
		m_CurrentState.rgbButtons[button_type] & (0x80))
	{
		return true;
	}
	return false;
}
//===============================================
// �}�E�X�̃����[�X����
//===============================================
bool CInputMouse::OnMouseTriggerUp(int button_type)
{
	if ((m_PrevState.rgbButtons[button_type] & (0x80)) &&
		!(m_CurrentState.rgbButtons[button_type] & (0x80)))
	{
		return true;
	}
	return false;

}
//===============================================
// �}�E�X�̃��s�[�g����
//===============================================
bool CInputMouse::OnMouseRepet(int button_type, const int nMaxCnt)
{
	// �L�[�������Ă�����
	if (OnMousePress(button_type))
	{
		if (m_bFirstPress == false)
		{
			m_bFirstPress = true;
			return true;
		}

		// �L�[�̃J�E���g���ő�܂ōs������
		if (m_nRepeatCount >= nMaxCnt)
		{
			// �J�E���g�����Z�b�g
			m_nRepeatCount = 0;

			// ����J�n
			return true;
		}
		else
		{
			// �J�E���g��i�߂�
			m_nRepeatCount++;
		}
	}
	else
	{
		// �J�E���g�����Z�b�g
		m_nRepeatCount = 0;
		m_bFirstPress = false;
	}

	return false;
}
//===============================================
// �}�E�X�̈ړ��ʎ擾����
//===============================================
D3DXVECTOR2 CInputMouse::GetVelocity(void)
{
	return D3DXVECTOR2((float)m_CurrentState.lX, (float)m_CurrentState.lY);
}
//===============================================
// �}�E�X�̉ߋ��̈ړ��ʎ擾����
//===============================================
D3DXVECTOR2 CInputMouse::GetOldVelocity(void)
{
	return D3DXVECTOR2((float)m_PrevState.lX, (float)m_PrevState.lY);
}
//===============================================
// �}�E�X�̃z�C�[���̎擾
//===============================================
int CInputMouse::GetMouseWeel(void)
{
	return m_CurrentState.lZ;
}
