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
		SOUND_LABEL_ONE = 0,	// 
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
		{"data/BGM/titlebgm.wav", -1},    // タイトル
	};				
};
#endif
