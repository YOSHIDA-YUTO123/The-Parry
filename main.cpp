//==================================================
//
// ���C�� [main.cpp]
// Author:YUTO YOSHIDA
//
//==================================================

//**************************************************
// �C���N���[�h�t�@�C��
//**************************************************
#include "main.h"
#include<stdlib.h>
//#include<crtdbg.h>
#include "manager.h"

//**************************************************
// �v���g�^�C�v�錾
//**************************************************
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//==================================================
// ���C���֐�
//==================================================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hInstancePrev, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
//#ifdef _DEBUG

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF); //�������[���[�N���m�p�t���O

//#endif // _DEBUG

	DWORD dwCurrentTime;					// ���ݎ���
	DWORD dwExecLastTime = timeGetTime();	// �Ō�ɏ�����������
	
	// ����\��ݒ�
	timeBeginPeriod(1);

	dwCurrentTime = 0;	// ����������
	dwExecLastTime = timeGetTime();	// ���ݎ����ۑ�

	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),				// WNDCLASSEX�̃������T�C�Y
		CS_CLASSDC,						// �E�C���h�E�̃X�^�C��
		WindowProc,						// �E�C���h�E�v���V�[�W��
		0,								// 0�ɂ���(�ʏ�͎g�p���Ȃ�)
		0,								// 0�ɂ���(�ʏ�͎g�p���Ȃ�)
		hInstance,						// �C���X�^���X�n���h��
		LoadIcon(NULL,IDI_APPLICATION),	// �^�X�N�o�[�̃A�C�R��
		LoadCursor(NULL,IDC_ARROW),		// �}�E�X�J�[�\��
		(HBRUSH)(COLOR_WINDOW + 1),		// �N���C�A���g�̈�̐F
		NULL,							// ���j���[�o�[
		CLASS_NAME,						// �E�C���h�E�N���X�̖��O
		LoadIcon(NULL,IDI_APPLICATION)  // �t�@�C���̃A�C�R��
	};

	HWND hWnd; // �E�C���h�E�̃n���h��

	MSG msg;

	// �E�C���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �N���C�A���g�̈���w��T�C�Y�ɒ���
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// �E�C���h�E�𐶐�
	hWnd = CreateWindowEx(0,		// �g���E�B���h�E�X�^�C��
		CLASS_NAME,					// �E�C���h�E�N���X�̖��O
		WINDOW_NAME,				// �E�C���h�E�̖��O
		WS_OVERLAPPEDWINDOW,		// �E�C���h�E�̃X�^�C��
		CW_USEDEFAULT,				// �E�C���h�E����X���W
		CW_USEDEFAULT,				// �E�C���h�E����Y���W
		(rect.right - rect.left),	// �E�C���h�E����x���W��
		(rect.bottom - rect.top),	// �E�C���h�E����Y���W����
		NULL,						// �e�E�C���h�E�̃n���h��
		NULL,						// ���j���[�n���h���܂��̓E�C���h�EID
		hInstance,					// �C���X�^���X�n���h��
		NULL);						// �E�C���h�E�쐬�f�[�^

	// �E�C���h�E�̕\��
	ShowWindow(hWnd, nCmdShow); // �E�C���h�E�̕\����Ԃ�ݒ�
	UpdateWindow(hWnd);         // �N���C�A���g�̈���X�V

	DWORD dwFrameCount;  // �t���[���J�E���g
	DWORD dwFPSLastTime; // �Ō��FPS���v����������

	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();

	CManager* pManager = NULL;

	// �����_���[��NULL�Ȃ�
	if (pManager == NULL)
	{
		// �����_���[�̐���
		pManager = new CManager;
	}

	// ����������
	if (FAILED(pManager->Init(hInstance,hWnd, TRUE)))
	{
		return -1;
	}

	// ���b�Z�[�W���[�v
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{ // windows�̏���
			if (msg.message == WM_QUIT)
			{ // WM_QUIT���b�Z�[�W���󂯎�����烋�[�v�𔲂���
				break;
			}
			else
			{
				TranslateMessage(&msg); // ���z�L�[���b�Z�[�W�𕶎����b�Z�[�W�֕ϊ�
				DispatchMessage(&msg);  // �E�C���h�E�v���V�[�W���փ��b�Z�[�W�𑗏o
			}
		}
		else
		{
			// DirectX�̏���
			// ���b�Z�[�W�̐ݒ�
			dwCurrentTime = timeGetTime(); // ���ݎ���

			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{ // 0.5�b�o��
				////FPS���v��

				// FPS���v�Z����
				int fps = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				// FPS�̐ݒ�
				pManager->SetFps(fps);

				dwFPSLastTime = dwCurrentTime; // FPS���v������������ۑ�
				dwFrameCount = 0; // �t���[���J�E���g���N���A

			}
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{ // 60����1�b�o��
				dwExecLastTime = dwCurrentTime; // �����J�n�̎���(���ݎ���)��ۑ�

				dwFrameCount++; //�t���[���J�E���g�����Z

				// �X�V����
				pManager->Update();

				// �`�揈��
				pManager->Draw();
			}	
		}	
	}

	// �����_���[�̔j��
	if (pManager != nullptr)
	{
		// �����_���[�̏I������
		pManager->Uninit();

		delete pManager;

		pManager = nullptr;
	}

#ifdef _DEBUG

	// �������[���[�N���m
	if (_CrtDumpMemoryLeaks())
	{
		MessageBox(NULL, "���������[�N�����o����܂���", "�G���[", MB_OK | MB_ICONERROR);
		exit(1);
	}

#endif

	// �E�C���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}
//==================================================
// �E�C���h�E�v���V�[�W��
//==================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const RECT rect = { 0,0, SCREEN_WIDTH,SCREEN_HEIGHT }; // �E�C���h�E�̗̈�

	switch (uMsg)
	{
	case WM_DESTROY: // �E�C���h�E�̔j�����b�Z�[�W
		// WM_QUIT�փ��b�Z�[�W�𑗂�
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN: // �L�[�����̃��b�Z�[�W
		switch (wParam)
		{
		case VK_ESCAPE:				// [ESC]�L�[�������ꂽ
			int nID = MessageBox(hWnd, "�I�����܂���?", "�I�����b�Z�[�W", MB_YESNO);

			if (nID == IDYES)
			{
				DestroyWindow(hWnd);// �E�C���h�E�j�����b�Z�[�W

			}
			else
			{
				return 0;			// �Ԃ�
			}
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);    // ����̏�����Ԃ�
}
