//==============================================
//
// 障害物 [obstacle.cpp]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// インクルードファイル
//**********************************************
#include "Obstacle.h"
#include "objectX.h"
#include "modelManager.h"
#include "manager.h"

using namespace Const;							// 名前空間Constを使用する
using namespace std;							// 名前空間stdを使用する

//==============================================
// コンストラクタ
//==============================================
CObstacle::CObstacle()
{
	m_pObjectX = nullptr;
}

//==============================================
// デストラクタ
//==============================================
CObstacle::~CObstacle()
{
}

//==============================================
// 生成処理
//==============================================
CObstacle* CObstacle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CObstacle* pObstacle = nullptr;

	// 障害物の生成
	pObstacle = new CObstacle;

	// 優先順位の取得
	int nPriority = pObstacle->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pObstacle != nullptr)
	{
		// 自分のポインタの解放
		pObstacle->Uninit();

		// nullにする
		pObstacle = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pObstacle == nullptr) return nullptr;

	// オブジェクトXの生成
	pObstacle->m_pObjectX = new CObjectX;

	// 初期化に失敗したら
	if (FAILED(pObstacle->Init()))
	{
		// 終了処理
		pObstacle->m_pObjectX->Uninit();
		pObstacle->Uninit();
		pObstacle = nullptr;

		return nullptr;
	}

	// オブジェクト
	pObstacle->m_pObjectX->SetPosition(D3DXVECTOR3(pos.x,pos.y,pos.z));
	
	// 向きの設定
	pObstacle->m_pObjectX->GetRotaition()->Set(rot);

	return pObstacle;
}

//==============================================
// 初期化処理
//==============================================
HRESULT CObstacle::Init(void)
{
	// 初期化処理
	if (m_pObjectX != nullptr && FAILED(m_pObjectX->Init()))
	{
		return E_FAIL;
	}

	// モデルの読み込み
	m_pObjectX->LoadModel("data/MODEL/obstacle/spiketrap.x");

	// 移動量の生成
	m_pMove = make_unique<CVelocity>();

    return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CObstacle::Uninit(void)
{
	// クリア
	m_pObjectX = nullptr;

	// 自分の破棄
	CObject::Release();
}

//==============================================
// 更新処理
//==============================================
void CObstacle::Update(void)
{
	// フィールドの取得
	CMeshField* pField = CManager::GetMeshField();

	// 位置の取得
	D3DXVECTOR3 pos = m_pObjectX->GetPosition();

	// 移動量
	pos += m_pMove->Get();

	float fHeight = 0.0f;

	if (pField->Collision(pos, &fHeight))
	{
		pos.y = fHeight;
	}

	m_pMove->Gravity(-MAX_GRABITY);

	m_pObjectX->SetPosition(pos);
}

//==============================================
// 描画処理
//==============================================
void CObstacle::Draw(void)
{
	if (m_pObjectX != nullptr)
	{
		// 描画処理
		m_pObjectX->Draw();
	}
}
