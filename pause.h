//================================================
//
// ポーズ [pause.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _PAUSE_H_
#define _PAUSE_H_

//************************************************
// インクルードファイル
//************************************************
#include "object2D.h"
#include <memory>

//************************************************
// ポーズクラスの定義
//************************************************
class CPause : public CObject2D
{
public:

	// ポーズのメニュー
	enum TYPE
	{
		TYPE_CONTINUE = 0, // コンテニュー
		TYPE_RETRY,        // リトライ
		TYPE_QUIT,		   // やめる
		TYPE_MAX
	};

	virtual ~CPause();

	static CPause* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size,const TYPE type);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override = 0;
	virtual void Draw(void) override;

protected:
	CPause(const TYPE type);

	D3DXVECTOR2 GetBaseSize(void) const { return m_BaseSize; }
	TYPE GetType(void) const { return m_Type; }

	void SetBaseSize(const D3DXVECTOR2 Size) { m_BaseSize = Size; }
private:
	TYPE m_Type;					// ポーズのメニュー
	D3DXVECTOR2 m_BaseSize;			// 基準の大きさ
};

//************************************************
// ポーズ(コンテニュー)クラスの定義
//************************************************
class CPauseContinue : public CPause
{
public:
	CPauseContinue();
	~CPauseContinue();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

//************************************************
// ポーズ(リトライ)クラスの定義
//************************************************
class CPauseRetry : public CPause
{
public:
	CPauseRetry();
	~CPauseRetry();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

//************************************************
// ポーズ(やめる)クラスの定義
//************************************************
class CPauseQuit : public CPause
{
public:
	CPauseQuit();
	~CPauseQuit();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

//************************************************
// ポーズのマネージャークラスの定義
//************************************************
class CPauseManager
{
public:
	~CPauseManager();
	static CPauseManager* GetInstance(void) { return m_pInstance.get(); }
	static void Create(void);

	void Uninit(void);
	void EnablePause(void);									   // 引数なし
	void EnablePause(const bool bPause) { m_bPause = bPause; } // 引数あり
	void SelectMenu(void);
	static bool GetPause(void) { return m_bPause; }
	CPause::TYPE GetSelectMenu(void) const { return m_SelectMenu; }
private:
	CPauseManager();
	CPause::TYPE m_SelectMenu;						   // ポーズのメニューの選択
	static std::unique_ptr<CPauseManager> m_pInstance; // 自分のインスタンス
	static bool m_bPause; // ポーズ状態かどうか
};

//************************************************
// ポーズ(ポーズ中)のクラスの定義
//************************************************
class CPauseNow : public CObject2D
{
public:
	CPauseNow();
	~CPauseNow();

	static CPauseNow* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size);
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	D3DXVECTOR2 m_BaseSize;	// 元の大きさ
	float m_fCounter;		// カウンター
};
#endif