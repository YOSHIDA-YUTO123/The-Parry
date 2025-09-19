//===================================================
//
// チュートリアル [tutorial.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "scene.h"
#include<memory>

//***************************************************
// 前方宣言
//***************************************************
class CGameCamera;
class CMeshField;
class CPlayer;
class CColliderSphere;
class CTrainingEnemy;

//***************************************************
// チュートリアルクラスの定義
//***************************************************
class CTutorial : public CScene
{
public:
	CTutorial();
	~CTutorial();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	static CGameCamera* GetCamera(void) { return m_pCamera; }
	static CMeshField* GetField(void) { return m_pMeshField; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CTrainingEnemy* GetTrainingEnemy(void) { return m_pTrainingEnemy; }
private:
	static CGameCamera* m_pCamera;		// カメラへのポインタ
	static CMeshField* m_pMeshField;	// メッシュフィールドへのポインタ
	static CPlayer* m_pPlayer;			// プレイヤーへのポインタ
	static CTrainingEnemy* m_pTrainingEnemy; // 練習用の敵のクラスへのポインタ
	std::unique_ptr<CColliderSphere> m_pSphere; // 円のコライダー
};

#endif