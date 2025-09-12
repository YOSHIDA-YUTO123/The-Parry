
//===================================================
//
// キャラクター [character.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _CHARACTERMANAGER_H_
#define _CHARACTERMANAGER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include<memory>
#include<vector>
#include <string>
#include "character3D.h"
#include "motion.h"

//***************************************************
// 前方宣言
//***************************************************
class CCharacter3D;
class CMotion;
class CModel;

//***************************************************
// キャラクターのマネージャークラスの定義
//***************************************************
class CCharacterManager
{
public:
	~CCharacterManager();
	static void Create(void);
	static CCharacterManager* GetInstance(void) { return m_pInstance.get(); }
	void SetMotion(CMotion* pMotion, const CCharacter3D::TYPE type);
	void SetModel(CModel* pModel, const CCharacter3D::TYPE type, const int nNumModel, const int nIdx);
	void GetMotion(CMotion* pMotion, const CCharacter3D::TYPE type);
	void GetModel(CModel** pModel, const CCharacter3D::TYPE type, const int nIdx, int* pParentIndx);
	int GetModelSize(const CCharacter3D::TYPE type) { return static_cast<int>(m_aConfig[type].aModelList.size()); }
private:

	struct Config
	{
		std::vector<CModel> aModelList;
		CMotion Motion;
		CCharacter3D::TYPE type;
	};

	CCharacterManager();
	static std::unique_ptr<CCharacterManager> m_pInstance;	// 自分のインスタンス
	Config m_aConfig[CCharacter3D::TYPE_MAX];				// キャラクターの構成情報

	//std::vector<CModel> m_aModelList;						// 保存用のモデル情報
};

#endif