//===================================================
//
// プレイヤーの移動処理をもったクラス [PlayerMovement.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "PlayerMovement.h"
#include "transform.h"
#include "input.h"
#include "manager.h"
#include "GameCamera.h"
#include "game.h"
#include "tutorial.h"
#include "debugproc.h"
#include "velocity.h"

//***************************************************
// 定数宣言
//***************************************************
constexpr float FORWARD_ANGLE_MIN = -25.0f; // 前に進む角度
constexpr float FORWARD_ANGLE_MAX = 25.0f;  // 前に進む角度

constexpr float LEFT_ANGLE_MAX = -25.0f;    // 左に進む角度
constexpr float LEFT_ANGLE_MIN = -150.0f;   // 左に進む角度

constexpr float RIGHT_ANGLE_MAX = 150.0f;   // 右に進む角度
constexpr float RIGHT_ANGLE_MIN = -25.0f;   // 右に進む角度

constexpr float BACK_ANGLE_MAX = 150.0f;    // 後ろに進む角度
constexpr float BACK_ANGLE_MIN = -150.0f;   // 後ろに進む角度

//===================================================
// コンストラクタ
//===================================================
CPlayerMovement::CPlayerMovement()
{
	m_pMove = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CPlayerMovement::~CPlayerMovement()
{
}

//===================================================
// 初期化処理
//===================================================
void CPlayerMovement::Init(void)
{
	// 移動処理へのポインタ
	m_pMove = std::make_unique<CVelocity>();
}

//===================================================
// 終了処理
//===================================================
void CPlayerMovement::Uninit(void)
{
	m_pMove.reset();
}

//===================================================
// キーボードの処理
//===================================================
bool CPlayerMovement::MoveKeyboard(CInputKeyboard* pKeyboard, const float fSpeed, float* pRotDest)
{
	bool bMove = false;

	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	// カメラの取得
	CGameCamera* pCamera = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}

	// 取得できなかったら処理しない
	if (pCamera == nullptr) return false;

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// 移動量
	D3DXVECTOR3 move = m_pMove->Get();

	CInputJoypad* pJoyPad = CManager::GetInputJoypad();

	if (pJoyPad->GetJoyStickL())
	{
		return false;
	}

	if (pKeyboard->GetPress(DIK_A))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W) == true)
		{
			move.x += sinf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;
			move.z += cosf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;

			*pRotDest = cameraRot.y + D3DX_PI * 0.75f;

			bMove = true;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;

			*pRotDest = cameraRot.y + D3DX_PI * 0.25f;

			bMove = true;
		}
		// プレイヤーの移動(左)
		else
		{
			move.z += sinf(cameraRot.y) * fSpeed;
			move.x -= cosf(cameraRot.y) * fSpeed;

			*pRotDest = cameraRot.y + D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//プレイヤーの移動(右)
	else if (pKeyboard->GetPress(DIK_D))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W))
		{
			move.x += sinf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;
			move.z += cosf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;

			*pRotDest = cameraRot.y - D3DX_PI * 0.75f;

			bMove = true;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;

			*pRotDest = cameraRot.y - D3DX_PI * 0.25f;

			bMove = true;
		}
		// プレイヤーの移動(右)
		else
		{
			move.z -= sinf(cameraRot.y) * fSpeed;
			move.x += cosf(cameraRot.y) * fSpeed;

			*pRotDest = cameraRot.y - D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//プレイヤーの移動(上)
	else if (pKeyboard->GetPress(DIK_W) == true)
	{
		move.x += sinf(cameraRot.y) * fSpeed;
		move.z += cosf(cameraRot.y) * fSpeed;

		*pRotDest = cameraRot.y + D3DX_PI;

		bMove = true;
	}
	//プレイヤーの移動(下)
	else if (pKeyboard->GetPress(DIK_S) == true)
	{
		move.x -= sinf(cameraRot.y) * fSpeed;
		move.z -= cosf(cameraRot.y) * fSpeed;

		*pRotDest = cameraRot.y;

		bMove = true;
	}

	// 移動量の設定
	m_pMove->Set(move);

	return bMove;
}

//===================================================
// 移動方向の取得
//===================================================
CPlayerMovement::MOVE_DIRECTION CPlayerMovement::GetMoveKeyboardDir(CInputKeyboard* pKeyboard, const float fSpeed, float* pRotDest)
{
	// 移動方向
	MOVE_DIRECTION dir = MOVE_DIRECTION::NONE;

	// モードの取得
	CScene::MODE mode = CManager::GetMode();
	
	// カメラの取得
	CGameCamera* pCamera = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}

	// 取得できなかったら処理しない
	if (pCamera == nullptr) return MOVE_DIRECTION::NONE;

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// 移動量
	D3DXVECTOR3 move = m_pMove->Get();

	CInputJoypad* pJoyPad = CManager::GetInputJoypad();

	if (pJoyPad->GetJoyStickL())
	{
		return MOVE_DIRECTION::NONE;
	}

	if (pKeyboard->GetPress(DIK_A))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W) == true)
		{
			move.x += sinf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;
			move.z += cosf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;

			*pRotDest = cameraRot.y + D3DX_PI * 0.75f;

			// 前に移動
			dir = MOVE_DIRECTION::LEFT;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;

			*pRotDest = cameraRot.y + D3DX_PI * 0.25f;

			// 後ろ移動
			dir = MOVE_DIRECTION::LEFT;
		}
		// プレイヤーの移動(左)
		else
		{
			move.z += sinf(cameraRot.y) * fSpeed;
			move.x -= cosf(cameraRot.y) * fSpeed;

			*pRotDest = cameraRot.y + D3DX_PI * 0.5f;

			// 左移動
			dir = MOVE_DIRECTION::LEFT;
		}
	}
	//プレイヤーの移動(右)
	else if (pKeyboard->GetPress(DIK_D))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W))
		{
			move.x += sinf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;
			move.z += cosf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;

			*pRotDest = cameraRot.y - D3DX_PI * 0.75f;

			// 前に移動
			dir = MOVE_DIRECTION::RIGHT;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;

			*pRotDest = cameraRot.y - D3DX_PI * 0.25f;

			// 後ろ移動
			dir = MOVE_DIRECTION::RIGHT;
		}
		// プレイヤーの移動(右)
		else
		{
			move.z -= sinf(cameraRot.y) * fSpeed;
			move.x += cosf(cameraRot.y) * fSpeed;

			*pRotDest = cameraRot.y - D3DX_PI * 0.5f;

			// 右移動
			dir = MOVE_DIRECTION::RIGHT;
		}
	}
	//プレイヤーの移動(上)
	else if (pKeyboard->GetPress(DIK_W) == true)
	{
		move.x += sinf(cameraRot.y) * fSpeed;
		move.z += cosf(cameraRot.y) * fSpeed;

		*pRotDest = cameraRot.y + D3DX_PI;

		// 前に移動
		dir = MOVE_DIRECTION::FORWARD;
	}
	//プレイヤーの移動(下)
	else if (pKeyboard->GetPress(DIK_S) == true)
	{
		move.x -= sinf(cameraRot.y) * fSpeed;
		move.z -= cosf(cameraRot.y) * fSpeed;

		*pRotDest = cameraRot.y;

		// 後ろ移動
		dir = MOVE_DIRECTION::BACK;
	}

	// 移動量の設定
	m_pMove->Set(move);

	return dir;
}

//===================================================
// パッドの処理
//===================================================
bool CPlayerMovement::MoveJoypad(CInputJoypad* pJoypad, const float fSpeed, float* pRotDest)
{
	bool bMove = false;

	XINPUT_STATE* pStick;

	pStick = pJoypad->GetJoyStickAngle();

	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	// カメラの取得
	CGameCamera* pCamera = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}

	// 取得できなかったら処理しない
	if (pCamera == nullptr) return false;

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// Lスティックの角度
	float LStickAngleY = pStick->Gamepad.sThumbLY;
	float LStickAngleX = pStick->Gamepad.sThumbLX;

	// デッドゾーン
	float deadzone = 32767.0f * 0.25f;

	// スティックの傾けた角度を求める
	float magnitude = sqrtf((LStickAngleX * LStickAngleX) + (LStickAngleY * LStickAngleY));

	// 動かせる
	if (magnitude > deadzone)
	{
		bMove = true;

		// アングルを正規化
		float normalizeX = (LStickAngleX / magnitude);
		float normalizeY = (LStickAngleY / magnitude);

		// プレイヤーの移動量
		float moveX = normalizeX * cosf(-cameraRot.y) - normalizeY * sinf(-cameraRot.y);
		float moveZ = normalizeX * sinf(-cameraRot.y) + normalizeY * cosf(-cameraRot.y);

		// 移動量をスティックの角度によって変更
		float speedWk = fSpeed * ((magnitude - deadzone) / (32767.0f - deadzone));

		// 移動量
		D3DXVECTOR3 moveWk = m_pMove->Get();

		// プレイヤーの移動
		moveWk.x += moveX * speedWk;
		moveWk.z += moveZ * speedWk;

		// 移動量の設定
		m_pMove->Set(moveWk);

		// プレイヤーの角度を移動方向にする
		float fDestAngle = atan2f(-moveX, -moveZ);

		*pRotDest = fDestAngle;
	}

	return bMove;
}

//===================================================
// パッドの処理
//===================================================
CPlayerMovement::MOVE_DIRECTION CPlayerMovement::GetMoveJoypadDir(CInputJoypad* pJoypad, const float fSpeed, float* pRotDest)
{
	// 移動方向
	MOVE_DIRECTION dir = MOVE_DIRECTION::NONE;

	XINPUT_STATE* pStick;

	pStick = pJoypad->GetJoyStickAngle();

	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	// カメラの取得
	CGameCamera* pCamera = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}

	// 取得できなかったら処理しない
	if (pCamera == nullptr) return MOVE_DIRECTION::NONE;

	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// Lスティックの角度
	float LStickAngleY = pStick->Gamepad.sThumbLY;
	float LStickAngleX = pStick->Gamepad.sThumbLX;

	// デッドゾーン
	float deadzone = 32767.0f * 0.25f;

	// スティックの傾けた角度を求める
	float magnitude = sqrtf((LStickAngleX * LStickAngleX) + (LStickAngleY * LStickAngleY));

	// 動かせる
	if (magnitude > deadzone)
	{
		// アングルを正規化
		float normalizeX = (LStickAngleX / magnitude);
		float normalizeY = (LStickAngleY / magnitude);

		// プレイヤーの移動量
		float moveX = normalizeX * cosf(-cameraRot.y) - normalizeY * sinf(-cameraRot.y);
		float moveZ = normalizeX * sinf(-cameraRot.y) + normalizeY * cosf(-cameraRot.y);

		// 移動量をスティックの角度によって変更
		float speedWk = fSpeed * ((magnitude - deadzone) / (32767.0f - deadzone));

		// 移動量
		D3DXVECTOR3 moveWk = m_pMove->Get();

		// プレイヤーの移動
		moveWk.x += moveX * speedWk;
		moveWk.z += moveZ * speedWk;

		// 移動量の設定
		m_pMove->Set(moveWk);

		// プレイヤーの角度を移動方向にする
		float fDestAngle = atan2f(-moveX, -moveZ);

		*pRotDest = fDestAngle;

		// 角度の取得
		float radian = atan2f(LStickAngleX, LStickAngleY);

		// 角度を変換
		float degree = D3DXToDegree(radian);

		CDebugProc::Print("コントローラーの移動方向%.3f\n", degree);

		// 前方だったら
		if (degree >= FORWARD_ANGLE_MIN && degree <= FORWARD_ANGLE_MAX)
		{
			dir = MOVE_DIRECTION::FORWARD;
		}
		// 左方向だったら
		else if (degree >= LEFT_ANGLE_MIN && degree <= LEFT_ANGLE_MAX)
		{
			dir = MOVE_DIRECTION::LEFT;
		}
		// 右方向だったら
		else if (degree >= RIGHT_ANGLE_MIN && degree <= RIGHT_ANGLE_MAX)
		{
			dir = MOVE_DIRECTION::RIGHT;
		}
		// 後方だったら
		else if (degree <= BACK_ANGLE_MIN || degree >= BACK_ANGLE_MAX)
		{
			dir = MOVE_DIRECTION::BACK;
		}
	}

	return dir;
}

//===================================================
// 向いている方向に移動する処理
//===================================================
void CPlayerMovement::MoveForward(const float fSpeed, const float fAngle)
{
	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 移動方向の計算
	move.x = sinf(fAngle) * fSpeed;
	move.z = cosf(fAngle) * fSpeed;

	// 移動量の設定
	m_pMove->Set(move);
}

//===================================================
// 前に進む処理
//===================================================
void CPlayerMovement::MoveForward(const float fSpeed, const float fJump, const float fAngle)
{
	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 移動方向の計算
	move.x = sinf(fAngle) * fSpeed;
	move.y = fJump;
	move.z = cosf(fAngle) * fSpeed;

	// 移動量の設定
	m_pMove->Set(move);
}

//===================================================
// 移動量の取得
//===================================================
D3DXVECTOR3 CPlayerMovement::Get(void) const
{
	// 移動量が生成されていないなら処理しない
	if (m_pMove == nullptr) return Const::VEC3_NULL;

	return m_pMove->Get();
}

//===================================================
// 移動量の設定
//===================================================
void CPlayerMovement::Set(const D3DXVECTOR3 move)
{
	// 移動量の設定
	m_pMove->Set(move);
}

//===================================================
// 移動方向の設定処理
//===================================================
void CPlayerMovement::SetMoveDir(const float dir, const float fSpeed, const float fAngleBase)
{
	// nullだったら処理しない
	if (m_pMove == nullptr) return;

	// 移動量の取得
	D3DXVECTOR3 moveWk = m_pMove->Get();

	// 移動量の設定
	moveWk.x = sinf(fAngleBase + dir) * fSpeed;
	moveWk.z = cosf(fAngleBase + dir) * fSpeed;

	// 移動量の設定
	m_pMove->Set(moveWk);
}

//===================================================
// 位置の更新処理
//===================================================
void CPlayerMovement::UpdatePosition(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld)
{
	// 移動量が生成されていないなら処理しない
	if (m_pMove == nullptr) return;

	// 慣性の設定
	m_pMove->SetInertia3D(0.25f);

	if (*pPosOld != nullptr)
	{
		// 前回の位置の設定
		*pPosOld = *pPos;
	}
	if (*pPos != nullptr)
	{
		// 位置の更新処理
		*pPos += m_pMove->Get();
	}
}

//===================================================
// 重力の設定
//===================================================
void CPlayerMovement::Gravity(const float fGravity)
{
	// 移動量が生成されていないなら処理しない
	if (m_pMove == nullptr) return;

	// 重力の設定
	m_pMove->Gravity(fGravity);
}

//===================================================
// 吹き飛び処理
//===================================================
float CPlayerMovement::BlowOff(const D3DXVECTOR3 AttackerDir, const float blowOff, const float jump)
{
	// 角度を求める
	float fAngle = atan2f(AttackerDir.x, AttackerDir.z);

	// 移動量
	D3DXVECTOR3 moveWk;

	// 移動量の設定
	moveWk.x = sinf(fAngle) * blowOff;
	moveWk.y = jump;
	moveWk.z = cosf(fAngle) * blowOff;

	m_pMove->Set(moveWk);

	return fAngle;
}

//===================================================
// 前に吹き飛ぶ
//===================================================
void CPlayerMovement::BlowForward(const float fMove, const float fJump, const float fAngle)
{
	// 移動量
	D3DXVECTOR3 move;

	// 移動量の設定
	move.x = sinf(fAngle) * fMove;
	move.y = fJump;
	move.z = cosf(fAngle) * fMove;

	m_pMove->Set(move);
}