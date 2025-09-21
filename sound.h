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
		SOUND_LABEL_WARK000,			// 歩き0
		SOUND_LABEL_WARK001,			// 歩き1
		SOUND_LABEL_BIRDSOUND,			// 鳥の鳴き声
		SOUND_LABEL_BIRDFLY,			// 鳥が飛び立つ
		SOUND_LABEL_WIND,				// 風
		SOUND_LABEL_PERFECT,			// パーフェクト
		SOUND_LABEL_NORMAL,				// 普通
		SOUND_LABEL_WEAK,				// 弱い
		SOUND_LABEL_GAME_BGM,			// ゲーム中のBGM
		SOUND_LABEL_WARK003,			// 敵の足音
		SOUND_LABEL_SPIKE,				// スパイクトラップにぶつかった音
		SOUND_LABEL_END_RUSH,			// 突進終了
		SOUND_LABEL_IMPACT000,			// 衝撃波0
		SOUND_LABEL_ENEMY_JUMP,			// 敵のジャンプ
		SOUND_LABEL_SWING,				// 風切り音
		SOUND_LABEL_SP,					// 必殺技
		SOUND_LABEL_MENU,				// メニュー選択
		SOUND_LABEL_ENTER,				// 決定
		SOUND_LABEL_IMPACT001,			// 衝撃波1
		SOUND_LABEL_TITLE_BGM,			// タイトルのBGM
		SOUND_LABEL_RESULT_LOSE,		// 敗北時のBGM
		SOUND_LABEL_AVOID,				// 回避
		SOUND_LABEL_ENEMY_DEATH,		// 敵死亡
		SOUND_LABEL_ENEMY_ATTACK,		// 敵の攻撃
		SOUND_LABEL_ENEMY_DAMAGE,		// 敵のダメージ
		SOUND_LABEL_BLOW,				// ダメージを受けて背中から着地
		SOUND_LABEL_BARRIEL,			// 樽
		SOUND_LABEL_RESULT000,			// リザルト
		SOUND_LABEL_RESULT001,			// リザルト
		SOUND_LABEL_BGM_RANKING,			// ランキングBGM
		SOUND_LABEL_MAX
	} SOUND_LABEL;

	CSound();
	~CSound();

	HRESULT Init(HWND hWnd);
	void Uninit(void);
	HRESULT Play(SOUND_LABEL label);
	HRESULT Play(SOUND_LABEL label,const float fVolume);
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
		{"data/SE/wark000.wav", 0},		  // 歩き0
		{"data/SE/wark001.wav", 0},		  // 歩き1
		{"data/SE/birdsound.wav", 0},	  // 鳥の鳴き声
		{"data/SE/BirdFly.wav", 0},		  // 鳥が羽ばたく
		{"data/SE/wind.wav", 0},		  // 風
		{"data/SE/perfect.wav", 0},		  // パーフェクト
		{"data/SE/normal.wav", 0},		  // 普通
		{"data/SE/weak.wav", 0},		  // 弱い
		{"data/BGM/game_bgm.wav", -1},	  // ゲーム中のBGM
		{"data/SE/wark003.wav", 0},		  // 敵の足音
		{"data/SE/spike.wav", 0},		  // スパイクトラップにぶつかった音
		{"data/SE/EndRush.wav", 0},		  // 突進終了
		{"data/SE/Impact000.wav", 0},	  // 衝撃波0
		{"data/SE/enemyJump.wav", 0},	  // 敵のジャンプ
		{"data/SE/swing.wav", 0},		  // 風切り音
		{"data/SE/SP.wav", 0},			  // 必殺技
		{"data/SE/menu000.wav", 0},		  // メニュー選択
		{"data/SE/Enter.wav", 0},		  // 決定
		{"data/SE/impact001.wav", 0},	  // 衝撃波1
		{"data/BGM/title_bgm.wav", -1},	  // タイトルのBGM
		{"data/BGM/result_lose.wav", -1}, // 敗北時BGM
		{"data/SE/avoid.wav", 0},		  // 回避
		{"data/SE/enemy_death.wav", 0},	  // 敵死亡
		{"data/SE/enemy_attack.wav", 0},  // 敵攻撃
		{"data/SE/enemy_damage.wav", 0},  // 敵ダメージ
		{"data/SE/blow.wav", 0},		  // 吹き飛び
		{"data/SE/barriel.wav", 0},		  // 樽
		{"data/SE/result000.wav", 0},	  // リザルト
		{"data/SE/result001.wav", 0},     // リザルト
		{"data/BGM/ranking.wav", -1},	  // ランキングBGM
	};				
};
#endif
