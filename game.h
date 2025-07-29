//===================================================
//
// ゲームシーン [game.h]
// Author:YUTO YOSHIDA
//
//===================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _GAME_H_
#define _GAME_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include "scene.h"

//***************************************************
// 前方宣言
//***************************************************
class CMeshField;
class CPlayer;
class CMeshCylinder;
class CPauseManager;
class CGameCamera;

//***************************************************
// ゲームクラスの定義
//***************************************************
class CGame : public CScene
{
public:

	enum STATE
	{
		STATE_NORMAL = 0,
		STATE_END,
		STATE_MAX
	};

	CGame();
	~CGame();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void SetState(const STATE state) { m_state = state; }
	static CMeshField* GetField(void) { return m_pMeshField; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CMeshCylinder* GetCylinder(void){ return m_pCylinder; }
	static CGameCamera* GetCamera(void) { return m_pCamera; }
private:
	static STATE m_state;				// ゲームの状態
	static CMeshField* m_pMeshField;	// メッシュフィールドへのポインタ
	static CPlayer* m_pPlayer;			// プレイヤークラスへのポインタ
	static CMeshCylinder* m_pCylinder;	// メッシュシリンダーへのポインタ
	static CGameCamera* m_pCamera;		// ゲームカメラクラスへのポインタ
	CPauseManager* m_pPauseManager;		// ポーズマネージャークラスへのポインタ
	int m_nCounterState;				// 状態のカウンター
};

#endif

