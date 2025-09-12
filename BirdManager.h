//===================================================
//
// 鳥のマネージャー [BirdManager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _BIRDMANAGER_H_
#define _BIRDMANAGER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object.h"
#include<list>

//***************************************************
// 前方宣言
//***************************************************
class CBird;

//***************************************************
// 鳥のマネージャーのクラスの定義
//***************************************************
class CBirdManager : public CObject
{
public:
	~CBirdManager();

	static void Create(void);
	static CBirdManager* GetInstance(void) { return m_pInstance; }
	void AddBirdList(CBird* pBird) { m_apBirdList.push_back(pBird); }
	void Erase(CBird* pBird);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	// 対象の位置からの距離を判定
	bool CheckDistance(const D3DXVECTOR3 otherPos,const float fRadius);

	void SetOpening(void);
private:
	void InAreaRenge(const D3DXVECTOR3 otherPos);

	static const int MAX_AREA = 4;	// 出現エリア
	static const int MAX_POINT = 2;	// 出現ポイント

	// 位置の情報
	struct AreaPoint
	{
		D3DXVECTOR3 pos; // 出現エリアの位置
		float fRadius;	 // エリアの半径
	};

	CBirdManager();
	static CBirdManager* m_pInstance; // 自分のインスタンス
	std::list<CBird*> m_apBirdList;	  // 鳥のリスト
	AreaPoint m_Arena[MAX_AREA];	  // 出現エリアの情報
	D3DXVECTOR3 m_Point[MAX_POINT];	  // 出現ポイントの情報
	int m_nAreaPopTime;				  // エリアに出現する時間
	int m_nPointPopTime;			  // 出現ポイントに出現する時間
};

#endif