//===================================================
//
// リザルトカメラ [ResultCamera.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _RESULTCAMERA_H_
#define _RESULTCAMERA_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "camera.h"

//***************************************************
// リザルトのカメラ
//***************************************************
class CResultCamera : public CCamera
{
public:
	CResultCamera();
	~CResultCamera();
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
private:
};

#endif
