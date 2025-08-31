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

//***************************************************
// 前方宣言
//***************************************************
class CGameCamera;
class CMeshField;

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
private:
	static CGameCamera* m_pCamera;		// カメラへのポインタ
	static CMeshField* m_pMeshField;	// メッシュフィールドへのポインタ
};

#endif