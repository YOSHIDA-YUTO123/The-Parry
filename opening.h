//===================================================
//
// オープニングシーン [opening.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OPENING_H_
#define _OPENING_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"scene.h"

//***************************************************
// 前方宣言
//***************************************************
class COpeningCamera;
class CFadeEffect;
class CMeshField;

//***************************************************
// オープニングシーンクラスの定義
//***************************************************
class COpening : public CScene
{
public:

	// オープニングの状態
	typedef enum
	{
		STATE_NORMAL = 0,
		STATE_FADEOUT,
		STATE_FADEIN,
		STATE_MAX
	}STATE;

	COpening();
	~COpening();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	static void SetState(const STATE state) { m_state = state; }
	static CFadeEffect* GetFadeEffect(void) { return m_pFadeEffect; }
	static STATE GetState(void) { return m_state; }

	static COpeningCamera* GetCamera(void) { return m_pCamera; }
	static CMeshField* GetField(void) { return m_pMeshField; }
	void EnableTutorial(const bool bTutorial) { m_bTutorial = bTutorial; }
private:
	static CMeshField* m_pMeshField;	// メッシュフィールドへのポインタ
	static STATE m_state;				// 状態
	static CFadeEffect* m_pFadeEffect;	// フェードエフェクトへのポインタ
	static COpeningCamera* m_pCamera;   // カメラへのポインタ
	bool m_bTutorial;					// チュートリアルに移行するかどうか
};

#endif