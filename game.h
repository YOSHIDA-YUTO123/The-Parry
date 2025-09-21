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
#include<memory>
#include<string>

//***************************************************
// 前方宣言
//***************************************************
class CMeshField;
class CPlayer;
class CMeshCylinder;
class CPauseManager;
class CGameCamera;
class CLoadManager;
class CEnemy;

//***************************************************
// ゲームクラスの定義
//***************************************************
class CGame : public CScene
{
public:

	// ゲームの状態
	enum STATE
	{
		STATE_NORMAL = 0,
		STATE_ENEMY_DEATH,
		STATE_END,
		STATE_MAX
	};

	// リザルトの種類
	enum RESULTTYPE
	{
		RESULTTYPE_WIN = 0,
		RESULTTYPE_LOSE,
		RESULTTYPE_MAX
	};

	CGame();
	~CGame();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void SetState(const STATE state) { m_state = state; }
	static void SetResult(const RESULTTYPE type) { m_ResultType = type; }
	static CMeshField* GetField(void) { return m_pMeshField; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CMeshCylinder* GetCylinder(void){ return m_pCylinder; }
	static CGameCamera* GetCamera(void) { return m_pCamera; }
	static STATE GetState(void) { return m_state; }
	static CEnemy* GetEnemy(void) { return m_pEnemy; }
private:
	void Load(void);					// ゲームのロード
	void LoadObstacle(std::fstream& file, std::string line, CLoadManager* pLoad);
	void LoadCharacter(std::fstream& file, std::string line, CLoadManager* pLoad);
	void PlayerConfig(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size, const D3DXVECTOR3 pos, const float fAngle);
	void EnemyConfig(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size, const D3DXVECTOR3 pos, const float fAngle);

	static STATE m_state;				// ゲームの状態
	static CMeshField* m_pMeshField;	// メッシュフィールドへのポインタ
	static CPlayer* m_pPlayer;			// プレイヤークラスへのポインタ
	static CMeshCylinder* m_pCylinder;	// メッシュシリンダーへのポインタ
	static CGameCamera* m_pCamera;		// ゲームカメラクラスへのポインタ
	static CEnemy* m_pEnemy;			// 敵クラスへのポインタ
	CPauseManager* m_pPauseManager;		// ポーズマネージャークラスへのポインタ
	static RESULTTYPE m_ResultType;		// リザルトの種類
	int m_nCounterState;				// 状態のカウンター
};

//***************************************************
// ゲームのマネージャークラスの定義
//***************************************************
class CGameManager
{
public:
	~CGameManager();

	void Init(void);
	void Uninit(void);
	void Update(void);

	static void Create(void);
	static CGameManager* GetInstance(void) { return m_pInstance.get(); }
	int GetTime(void) { return m_pInstance->m_nGameTime; }
	void SavePlayerInfo(const int nPerfect, const int nNormal, const int nWeak);
private:
	CGameManager();

	static std::unique_ptr<CGameManager> m_pInstance; // 自分のインスタンス
	int m_nCounter;					  // カウンター
	int m_nGameTime;			  // ゲームの経過時間
};

#endif

