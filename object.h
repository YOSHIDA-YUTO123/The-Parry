//===================================================
//
// オブジェクトの描画処理 [object.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECT_H_
#define _OBJECT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include<vector>

class CObstacle;
class CObstacleOwner;

//***************************************************
// オブジェクトクラスの定義
//***************************************************
class CObject
{
public:

	// オブジェクトの種類
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_PLAYER,
		TYPE_ENEMY,
		TYPE_OBSTACLE,
		TYPE_PAUSE,
		TYPE_MAX
	}TYPE;

	CObject(int nPriority = 3);
	virtual ~CObject();

	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static int GetNumObject(const int nPriority) { return m_nNumAll[nPriority]; }
	static void DestroyAll(void);

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	int GetPriority(void) const { return m_nPriority; }
	void SetType(const TYPE type) { m_type = type; }
	TYPE GetType(void) const { return m_type; }
	bool GetDeath(void) const { return m_bDeath; }	// 死亡フラグの取得

protected:
	void Release(void);								// 死亡フラグを立てる
private:
	void Destroy(CObject* pObject);
	static int m_nNumAll[Const::NUM_PRIORITY];		// オブジェクトの総数
	static CObject* m_pTop[Const::NUM_PRIORITY];	// 先頭オブジェクトへのポインタ
	static CObject* m_pCur[Const::NUM_PRIORITY];	// 最後尾オブジェクトへのポインタ
	CObject* m_pPrev;								// 前のオブジェクトへのポインタ
	CObject* m_pNext;								// 次のオブジェクトへのポインタ
	TYPE m_type;									// オブジェクトの種類
	int m_nPriority;								// 優先順位
	bool m_bDeath;									// 死亡フラグ
};

#endif