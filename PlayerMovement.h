//===================================================
//
// プレイヤーの移動処理をもったクラス [PlayerMovement.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _PLAYERMOVEMENT_H_
#define _PLAYERMOVEMENT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "main.h"
#include <memory>

//***************************************************
// 前方宣言
//***************************************************
class CInputKeyboard;
class CInputJoypad;
class CVelocity;

//***************************************************
// プレイヤーの移動処理の定義
//***************************************************
class CPlayerMovement
{
public:

	// 移動方向の種類
	enum class MOVE_DIRECTION
	{
		NONE = 0,
		RIGHT,
		LEFT,
		FORWARD,
		BACK,
		MAX
	};

	CPlayerMovement();
	~CPlayerMovement();

	void Init(void);
	void Uninit(void);

	/// <summary>
	/// キーボードの移動処理
	/// </summary>
	/// <param name="pKeyboard"></param>
	/// <param name="fSpeed"></param>
	/// <param name="pRotDest"></param>
	/// <returns>移動しているかどうか</returns>
	bool MoveKeyboard(CInputKeyboard* pKeyboard, const float fSpeed, float* pRotDest);

	/// <summary>
	/// キーボードの移動&移動方向の取得
	/// </summary>
	/// <param name="pKeyboard"></param>
	/// <param name="fSpeed"></param>
	/// <param name="pRotDest"></param>
	/// <returns>移動方向</returns>
	MOVE_DIRECTION GetMoveKeyboardDir(CInputKeyboard* pKeyboard, const float fSpeed, float* pRotDest);

	/// <summary>
	/// パッドの移動処理
	/// </summary>
	/// <param name="pJoypad"></param>
	/// <param name="fSpeed"></param>
	/// <param name="pRotDest"></param>
	/// <returns>移動しているかどうか</returns>
	bool MoveJoypad(CInputJoypad* pJoypad, const float fSpeed, float* pRotDest);

	/// <summary>
	/// パッドの移動&移動方向の取得
	/// </summary>
	/// <param name="pKeyboard"></param>
	/// <param name="fSpeed"></param>
	/// <param name="pRotDest"></param>
	/// <returns>移動方向</returns>
	MOVE_DIRECTION GetMoveJoypadDir(CInputJoypad* pJoypad, const float fSpeed, float* pRotDest);

	void MoveForward(const float fSpeed, const float fAngle);
	void MoveForward(const float fSpeed, const float fJump, const float fAngle);

	/// <summary>
	/// 移動量による位置の更新処理
	/// </summary>
	/// <param name="pPos"></param>
	/// <param name="pPosOld"></param>
	void UpdatePosition(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld);
	void Gravity(const float fGravity);
	float BlowOff(const D3DXVECTOR3 AttackerDir, const float blowOff, const float jump);
	void BlowForward(const float fMove, const float fJump, const float fAngle);


	D3DXVECTOR3 Get(void) const;
	void Set(const D3DXVECTOR3 move);
	void SetMoveDir(const float dir, const float fSpeed, const float fAngleBase); // 移動方向の設定


private:
	std::unique_ptr<CVelocity> m_pMove; // 移動クラスへのポインタ
};

#endif