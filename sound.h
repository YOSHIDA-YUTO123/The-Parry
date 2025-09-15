//================================================
//
// サウンド処理 [sound.h]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _SOUND_H_
#define _SOUND_H_

//*************************************************
// インクルードファイル
//*************************************************
#include"main.h"

//*************************************************
// サウンドクラスの定義
//*************************************************
class CSound
{
public:

	// サウンドの種類
	typedef enum
	{
		SOUND_LABEL_PARRYPARFECT = 0,	// パリィパーフェクト
		SOUND_LABEL_PARRYNORMAL,		// パリィ普通
		SOUND_LABEL_PARRYWEAK,			// パリィ微妙
		SOUND_LABEL_ROAR,				// 咆哮
		SOUND_LABEL_EXPLOSION,			// 爆発
		SOUND_LABEL_MAX
	} SOUND_LABEL;

	CSound();
	~CSound();

	HRESULT Init(HWND hWnd);
	void Uninit(void);
	HRESULT PlaySound(SOUND_LABEL label);
	void StopSound(SOUND_LABEL label);
	void StopSound(void);
private:
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
	IXAudio2* m_pXAudio2;									// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice* m_pMasteringVoice;				// マスターボイス
	IXAudio2SourceVoice* m_apSourceVoice[SOUND_LABEL_MAX];	// ソースボイス
	BYTE* m_apDataAudio[SOUND_LABEL_MAX];					// オーディオデータ
	DWORD m_aSizeAudio[SOUND_LABEL_MAX];					// オーディオデータサイズ

	// サウンド情報構造体の定義
	struct SOUND_INFO
	{
		const char* pFilename;  // ファイル名
		int nCntLoop;			// ループカウント
	};

	// サウンドの情報
	SOUND_INFO m_aSoundInfo[SOUND_LABEL_MAX] = 
	{
		{"data/SE/parryParfect.wav", 0},  // パリィパーフェクト
		{"data/SE/parryNormal.wav", 0},   // 普通
		{"data/SE/parryWeak.wav", 0},     // 微妙
		{"data/SE/roar.wav", 0},		  // 咆哮
		{"data/SE/explosion.wav", 0},	  // 爆発
	};				
};
#endif
