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

//***************************************************
// ゲームクラスの定義
//***************************************************
class CGame : public CScene
{
public:

	CGame();
	~CGame();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CMeshField* GetField(void) { return m_pMeshField; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CMeshCylinder* GetCylinder(void){ return m_pCylinder; }
private:
	static CMeshField* m_pMeshField;	// メッシュフィールドへのポインタ
	static CPlayer* m_pPlayer;			// プレイヤークラスへのポインタ
	static CMeshCylinder* m_pCylinder;	// メッシュシリンダーへのポインタ
};

#endif

