//===================================================
//
// プレイヤー [player.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "player.h"
#include "input.h"
#include "manager.h"
#include "effect.h"
#include "particle.h"
#include "renderer.h"
#include "impact.h"
#include "motion.h"
#include "score.h"
#include "shadow.h"
#include "explosion.h"
#include "dust.h"
#include "LoadManager.h"
#include "Wave.h"
#include "cylinder.h"
#include "Collider.h"
#include "obstaclemanager.h"
#include "Obstacle.h"
#include "statebase.h"
#include "playerstate.h"
#include "shadowS.h"
#include "meshfield.h"
#include "GameCamera.h"
#include "slow.h"
#include "game.h"
#include "Observer.h"
#include "Gage.h"
#include "EffectAnim.h"
#include "MoveSmoke.h"
#include "Orbit.h"
#include "overlay.h"
#include "math.h"
#include "ParticleSpark.h"
#include "light.h"
#include "tutorial.h"
#include "BlockManager.h"
#include "ParryEffect.h"
#include "bird.h"
#include "BirdManager.h"
#include "sound.h"
#include "RockOn.h"
#include "effect2D.h"
#include "particle2D.h"
#include "Collision.h"
#include "PlayerMovement.h"

//***************************************************
// 名前空間
//***************************************************
using namespace math;							// 名前空間mathを使用
using namespace std;							// 名前空間をstdを使用する
using namespace Const;							// 名前空間Constを使用する

using STATE = CCharacter3D::STATE;				// キャラクターの状態

constexpr float JUMP_HEIGHT = 25.0f;			// ジャンプ量
constexpr float SHADOW_SIZE = 50.0f;			// 影の大きさ
constexpr float SHADOW_MAX_HEIGHT = 500.0f;		// 影が見える最大の高さ
constexpr float SHADOW_A_LEVEL = 0.9f;			// 影のアルファ値のオフセット
constexpr float AVOID_STAMINA = 15.0f;			// 回避に使用するスタミナ
constexpr float SIDE_MOVE_VALUE = 1.0f;			// 横移動
constexpr float ADD_REVENGE_WEAK = 5.0f;		// 反撃ゲージの加算量(セーフ時)
constexpr float ADD_REVENGE_NORMAL = 15.0f;		// 反撃ゲージの加算量(ノーマル時)
constexpr float ADD_REVENGE_PERFECT = 25.0f;	// 反撃ゲージの加算量(パーフェクト時)

constexpr float DEC_STAMINA_WEAK = 30.0f;		// パリィ時のスタミナ消費(セーフ時)
constexpr float DEC_STAMINA_NORMAL = 20.0f;		// パリィ時のスタミナ消費(ノーマル時)
constexpr float DEC_STAMINA_PERFECT = 10.0f;	// パリィ時のスタミナ消費(パーフェクト時)
constexpr float STANCE_STAMINA = 20.0f;			// 反撃の使用スタミナ

constexpr int PARRY_TIME = 25;					// パリィの有効時間
constexpr int ATTACK_TIME = 120;				// 攻撃の有効時間

//===================================================
// コンストラクタ
//===================================================
CPlayer::CPlayer() : CCharacter3D(TYPE_PLAYER)
{
	m_ParryMotion = PARRYMOTION_KICK;
	m_fDestRevengeValue = NULL;
	m_fRevengeValue = NULL;
	m_pMachine = nullptr;
	m_pMovement = nullptr;
	m_pFOV = nullptr;
	m_pSphere = nullptr;
	m_posOld = VEC3_NULL;
	m_nParryTime = NULL;
	m_nParryCounter = NULL;
	m_bJump = false;
	m_bDash = false;
	m_pHpObserver = nullptr;
	m_pStaminaObserver = nullptr;
	m_pRevengeObserver = nullptr;
	m_pOrbit = nullptr;
	m_nAttackCounter = NULL;
	m_fStamina = NULL;
	m_bGravity = true;
	m_nPerfectCnt = NULL;
	m_nNormalCnt = NULL;
	m_nWeakCnt = NULL;
	m_bRevenge = false;
}

//===================================================
// デストラクタ
//===================================================
CPlayer::~CPlayer()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CPlayer::Init(void)
{
	// 大きさの取得
	D3DXVECTOR3 Size = CCharacter3D::GetSize();

	// 状態制御の生成
	m_pMachine = make_unique<CStateMachine>();

	// 初期状態を設定
	ChangeState(make_shared<CPlayerNormal>());

	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	// 視界判定の生成
	m_pFOV = CColliderFOV::Create(pos, 0.0f, D3DX_PI * 0.5f, -D3DX_PI * 0.5f, 1000.0f);

	// 円の当たり判定の生成
	m_pSphere = CColliderSphere::Create(pos, 50.0f);

	m_pMovement = make_unique<CPlayerMovement>();
	m_pMovement->Init();

	// コライダーの生成
	m_pAABB = CColliderAABB::Create(pos, m_posOld, Size);

	// カプセルコライダーの生成
	m_Capsule = CColliderCapsule::Create(pos, D3DXVECTOR3(pos.x, pos.y + Size.y, pos.z), 60.0f,0);

	// スタミナを設定
	m_fStamina = MAX_STAMINA;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CPlayer::Uninit(void)
{
	// ゲームのマネージャーの取得
	auto pGameManager = CGameManager::GetInstance();

	if (pGameManager != nullptr)
	{
		// プレイヤーの情報のセーブ
		pGameManager->SavePlayerInfo(m_nPerfectCnt, m_nNormalCnt, m_nWeakCnt);
	}
	// HPオブザーバーの破棄
	if (m_pHpObserver != nullptr)
	{
		delete m_pHpObserver;
		m_pHpObserver = nullptr;
	}

	// Staminaオブザーバーの破棄
	if (m_pStaminaObserver != nullptr)
	{
		delete m_pStaminaObserver;
		m_pStaminaObserver = nullptr;
	}
	// 反撃オブザーバーの破棄
	if (m_pRevengeObserver != nullptr)
	{
		delete m_pRevengeObserver;
		m_pRevengeObserver = nullptr;
	}

	m_pFOV = nullptr;
	m_pSphere = nullptr;
	m_pMovement = nullptr;
	m_pOrbit = nullptr;
	m_pAABB = nullptr;
	m_Capsule = nullptr;

	// キャラクターの破棄
	CCharacter3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CPlayer::Update(void)
{
	// モーションの取得
	auto pMotion = CCharacter3D::GetMotion();

	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	// 読み込めていなかったら
	if (pMotion->IsLoad() == false)
	{
		return;
	}
	
	// ヒットストップ状態だったら
	if (CCharacter3D::HitStop())
	{
		// 更新を止める
		return;
	}
	
	// 生きてるかどうか
	bool bAlive = CCharacter3D::GetAlive();

	// プレイヤーが死んだら
	if (bAlive == false)
	{
		// ゲームを終了
		CGame::SetState(CGame::STATE_END);
		CGame::SetResult(CGame::RESULTTYPE_LOSE);
	}

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// コントローラーの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// マウスの取得
	CInputMouse* pMouse = CManager::GetInputMouse();

	// メッシュフィールドの取得
	CMeshField* pMesh = nullptr;

	// カメラの取得
	CGameCamera* pCamera = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();

		// メッシュフィールドの取得
		pMesh = CGame::GetField();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();

		// メッシュフィールドの取得
		pMesh = CTutorial::GetField();
	}

	if (CCharacter3D::HitStop())
	{
		return;
	}

	// ロックオン状態じゃないかダッシュ中だったら
	if (pCamera->GetState() != CGameCamera::STATE_ROCKON || m_bDash)
	{
		// 移動の更新処理
		UpdateMove(pMotion, bAlive, pKeyboard, pJoypad, pCamera);
	}
	// ロックオン状態中でダッシュしていないなら
	else if(!m_bDash)
	{
		// ロックオン状態の移動処理
		UpdateRockOnMove(pMotion, bAlive, pKeyboard, pJoypad, pCamera);
	}

	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	// 位置の更新処理
	m_pMovement->UpdatePosition(&pos, &m_posOld);

	float fHeight = 0.0f;

	// メッシュフィールドの当たり判定
	if (pMesh != nullptr && pMesh->Collision(pos, &fHeight))
	{
		//// 位置の設定
		//SetPosition(pos);

		// 高さの設定
		pos.y = fHeight;

		// ジャンプできる
		m_bJump = true;

		// モーションがジャンプだったら
		if (pMotion->GetBlendType() == MOTIONTYPE_JUMP)
		{
			// 着地状態に派生
			ChangeState(make_shared<CPlayerLanding>());
		}
	}
	else
	{
		m_bJump = false;
	}

	// メッシュシリンダーの取得
	CMeshCylinder* pCylinder = CGame::GetCylinder();

	// シリンダーの判定
	if (pCylinder != nullptr && pCylinder->Collision(&pos))
	{
	}

	// ブロックの当たり判定
	CollisionBlock(&pos);

	// コライダーを更新する
	UpdateCollider(pos);

	// 障害物との当たり判定
	if (CollisionObstacle(&pos))
	{
		// コライダーを更新する
		UpdateCollider(pos);
	}

	// インパクトの当たり判定
	CollisionImpact(pMesh, &pos, pMotion);

	// 鳥のマネージャの取得
	auto pBirdManager = CBirdManager::GetInstance();

	if (pBirdManager != nullptr)
	{
		// 鳥との距離の判定
		pBirdManager->CheckDistance(pos, 300.0f);
	}

	if (m_bGravity)
	{
		// 重力を加算
		m_pMovement->Gravity(-MAX_GRABITY);
	}
	
	// 敵の死亡ムービーじゃないなら
	if (CGame::GetState() != CGame::STATE_ENEMY_DEATH)
	{
		// ロックオン
		if (pKeyboard->GetTrigger(DIK_R) || pJoypad->GetTrigger(pJoypad->JOYKEY_RIGHT_THUMB))
		{
			CGameCamera::STATE state; // カメラの状態

			// カメラの状態を判定
			const bool rockon = pCamera->GetState() == CGameCamera::STATE_ROCKON;

			// ロックオンじゃなかったらロックオン
			state = rockon ? CGameCamera::STATE_TRACKING : CGameCamera::STATE_ROCKON;

			pCamera->SetState(state);
		}
	}

	// 回避ボタンを押したかつ生きているなら
	if ((pKeyboard->GetTrigger(DIK_SPACE) || pJoypad->GetTrigger(pJoypad->JOYKEY_B)) && bAlive)
	{
		// 回避できるなら
		if (IsAvoid(pMotion))
		{
			// スタミナの設定
			SetStamina(-AVOID_STAMINA);

			// 向きを設定
			SetMoveAngle(pCamera, pKeyboard, pJoypad);

			// 回避
			ChangeState(make_shared<CPlayerAvoid>(35.0f));
		}
	}

	// カウンター状態
	if ((pMouse->OnMouseTriggerDown(0) || pJoypad->GetTriggerTrigger(pJoypad->JOYKEY_L2)))
	{
		// 構えが出せるなら
		if (IsStance(pMotion) && bAlive)
		{
			pMotion->SetMotion(MOTIONTYPE_STANCE, true, 5);

			// パリィの時間
			m_nParryTime = PARRY_TIME;
			m_nParryCounter = 0;

			CCharacter3D::SetState(STATE_ACTION, PARRY_TIME);
		}
	}

	// ズームインだったら解除
	if (pMotion->GetBlendType() != MOTIONTYPE_PARRY)
	{
		if (pCamera->GetState() == CGameCamera::STATE_ZOOMIN)
		{
			// カメラのズーム解除
			pCamera->ResetState();
		}
	}

	// 反撃ボタンを押したら&&ダメージ状態じゃないなら
	if ((pMouse->OnMouseTriggerDown(1) || pJoypad->GetTriggerTrigger(pJoypad->JOYKEY_R2)) && !IsDamage(pMotion))
	{
		// 反撃の受付時間いないだったら
		if (m_nAttackCounter >= 0)
		{
			// 反撃受付しない
			m_nAttackCounter = 0;

			switch (m_ParryMotion)
			{
			case PARRYMOTION_KICK:
				// 回し蹴り状態を設定
				ChangeState(make_shared<CPlayerRoundKick>());
				break;
			case PARRYMOTION_JUMP:
				// ジャンプ攻撃状態を設定
				ChangeState(make_shared<CPlayerJumpAttack>());
				break;
			default:
				break;
			}
		}
	}

	// 視界判定
	if (m_pFOV != nullptr)
	{
		// 位置の設定
		m_pFOV->SetPosition(pos);
	}

	// スタミナの更新処理
	UpdateStamina();

	// 目的の値に近づける
	m_fRevengeValue += (m_fDestRevengeValue - m_fRevengeValue) * 0.1f;

	// 反撃ゲージが最大を超えたら
	if (m_fRevengeValue >= MAX_REVENGE)
	{
		// 反撃ゲージ
		m_fRevengeValue = MAX_REVENGE;

		m_bRevenge = true;

		// エフェクトの生成
		auto pEffect = CEffect2D::Create(D3DXVECTOR3(50.0f, 50.0f, 0.0f), D3DXVECTOR2(80.0f, 80.0f), D3DXCOLOR(1.0f, 0.3f, 0.3f, 0.2f));
		pEffect->SetParam(60, VEC3_NULL);

		// パーティクルの生成
		auto pNormal = CParticle3DNormal::Create(CCharacter3D::GetModelPos(MODEL_CHEST), 10.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

		// パーティクルの設定処理
		pNormal->SetParticle(5.0f, 120, 5, 1, 60);
		pNormal->SetParam(CEffect3D::TYPE_NORAML, 100);
		
		// モーションの種類の取得
		int nMotiontype = pMotion->GetBlendType();

		if ((pKeyboard->GetTrigger(DIK_Q) || pJoypad->GetTrigger(pJoypad->JOYKEY_LEFT_THUMB)) &&
			!IsDamage(pMotion) && nMotiontype != MOTIONTYPE_REVENGEATTACK)
		{
			// 状態の遷移
			ChangeState(make_shared<CPlayerRevenge>());

			m_fDestRevengeValue = 0.0f;
			m_fRevengeValue = 0.0f;
		}
	}
	else
	{
		m_bRevenge = false;
	}

	// オブザーバーへの通知処理
	Notify();

	// パリィの更新処理
	UpdateParry();

	// モーションの更新処理
	CCharacter3D::UpdateMotion();
	
	// コライダーの更新
	UpdateCollider(pos);

	// 位置の設定
	CCharacter3D::SetPosition(pos);

	// 状態制御がnullじゃなかったら
	if (m_pMachine != nullptr)
	{
		// 状態の更新処理
		m_pMachine->Update();
	}

	// キャラクターの更新処理
	CCharacter3D::Update();

	D3DXVECTOR3 posRDest;

	D3DXVECTOR3 rot = CCharacter3D::GetRotation();

	D3DXVECTOR3 modelpos = GetModelPos(CPlayer::MODEL_CHEST);

	posRDest.x = modelpos.x + sinf(rot.y) * 1.0f;
	posRDest.y = (modelpos.y + 0.0f) + sinf(rot.y) * 1.0f;
	posRDest.z = modelpos.z + cosf(rot.y) * 1.0f;

	// 視点の設定
	D3DXVECTOR3 posVDest(modelpos);

	// カメラの追従処理
	pCamera->SetTracking(posVDest, posRDest, 0.1f, CGameCamera::TRACKOBJ_PLAYER);

	// チュートリアルだったら
	if (mode == CScene::MODE_TUTORIAL)
	{
		m_fDestRevengeValue += 0.5f;
	}

#ifdef _DEBUG
	// デバッグ情報
	DebugInfo();
#endif // _DEBUG
}

//===================================================
// 描画処理
//===================================================
void CPlayer::Draw(void)
{
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	//m_pMotion->Debug();
	// デバック
	CDebugProc::Print("プレイヤーの位置 X = %.2f Y = %.2f Z = %.2f\n", pos.x, pos.y,pos.z);

#if 0

	LPDIRECT3DSURFACE9 pRenderDef, pZBuffer;
	D3DVIEWPORT9 viepowtDef;
	D3DXMATRIX mtxViewDef, mtxProjectionDef;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 現在のレンダーターゲットの取得
	pDevice->GetRenderTarget(0, &pRenderDef);

	// 現在のZバッファの取得
	pDevice->GetDepthStencilSurface(&pZBuffer);

	// 現在のビューポートの取得
	pDevice->GetViewport(&viepowtDef);

	// 現在のビューマトリックスの取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxViewDef);

	// 現在のプロジェクションマトリックスの取得
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjectionDef);

	CCamera* pCamera = CManager::GetCamera();

	D3DXVECTOR3 posV, posR,vecU;

	posV = pCamera->GetPosV();

	posR = pCamera->GetPosR();

	vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// レンダリングターゲットの変更
	CManager::GetRenderer()->ChangeTarget(posV, posR, vecU);

	// レンダリングターゲット用テクスチャのクリア
	pDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 255, 255, 255), 1.0f, 0);
#endif

	//D3DXVECTOR3 headpos = GetModelPos(2);

	//CManager::GetLight()->SetPoint(headpos,500.0f,D3DXCOLOR(1.0f,0.39f,0.0f,1.0f), D3DXCOLOR(1.0f, 0.39f, 0.0f, 1.0f));

	// キャラクターの描画
	CCharacter3D::Draw();

	//CManager::GetLight()->DeleteLight();
#if 0

	// レンダーターゲットをもとに戻す
	pDevice->SetRenderTarget(0, pRenderDef);

	// Zバッファをもとに戻す
	pDevice->SetDepthStencilSurface(pZBuffer);

	// ビューポートをもとに戻す
	pDevice->SetViewport(&viepowtDef);

	pDevice->SetTransform(D3DTS_VIEW, &mtxViewDef);

	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjectionDef);

#endif // 0
}

//===================================================
// 状態の変更
//===================================================
void CPlayer::ChangeState(std::shared_ptr<CPlayerState> pNewState)
{
	if (pNewState != nullptr)
	{
		// オーナの設定
		pNewState->SetOwner(this);
	}

	if (m_pMachine != nullptr)
	{
		// 状態の変更
		m_pMachine->Change(pNewState);
	}
}

//===================================================
// 生成処理
//===================================================
CPlayer* CPlayer::Create(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = nullptr;

	// 3Dオブジェクトの生成
	pPlayer = new CPlayer;

	if (pPlayer == nullptr) return nullptr;

	// 設定処理
	pPlayer->Config(nLife,fSpeed,ShadowScal,Size);

	// 初期化処理
	if (FAILED(pPlayer->Init()))
	{
		pPlayer->Uninit();
		pPlayer = nullptr;
		return nullptr;
	}

	pPlayer->SetPosition(pos);
	pPlayer->SetRotDest(rot.y);
	pPlayer->SetRotation(rot);

	return pPlayer;
}


//===================================================
// コライダーの更新処理
//===================================================
void CPlayer::UpdateCollider(D3DXVECTOR3 pos)
{
	// 当たり判定の設定処理
	if (m_pSphere != nullptr)
	{
		// 位置の設定処理
		m_pSphere->SetPosition(pos);
	}

	if (m_pAABB != nullptr)
	{
		// データの取得
		auto dataAABB = m_pAABB->GetData();

		// 大きさの取得
		float fSizeY = dataAABB.Size.y * 0.5f;

		// コライダーの更新処理
		m_pAABB->UpdateData(D3DXVECTOR3(pos.x, pos.y + fSizeY, pos.z), D3DXVECTOR3(m_posOld.x, m_posOld.y + fSizeY, m_posOld.z));
	}

	if (m_Capsule != nullptr)
	{
		// 大きさの取得
		D3DXVECTOR3 Size = CCharacter3D::GetSize();

		// データの取得
		auto dataCapsule = m_Capsule->GetData();

		dataCapsule.EndPos = pos;
		dataCapsule.EndPos.y = pos.y + Size.y;

		// データの更新
		dataCapsule.StartPos = pos;

		// データの更新処理
		m_Capsule->UpdateData(dataCapsule);

	}

	// 位置の設定
	SetPosition(pos);
}

//===================================================
// スタミナの更新処理
//===================================================
void CPlayer::UpdateStamina(void)
{
	m_fStamina += 0.08f;

	// 範囲制限する
	m_fStamina = Clamp(m_fStamina, 0.0f, MAX_STAMINA);
}

//===================================================
// パリィの成功度の取得
//===================================================
int CPlayer::SuccessParry(void)
{		
	return m_ParryResult;
}

//====================================================
// 矩形の判定
//===================================================
bool CPlayer::CollisionAABB(CColliderAABB* pAABB)
{
	// 押し出し位置
	D3DXVECTOR3 pushPos = VEC3_NULL;

	if (CCollisionAABB::Collision(m_pAABB.get(), pAABB,&pushPos))
	{
		// 位置の取得
		CCharacter3D::SetPosition(pushPos);

		// コライダーの更新
		UpdateCollider(pushPos);

		return true;
	}
	return false;
}

//===================================================
// カプセルの当たり判定
//===================================================
bool CPlayer::CollisionCapsule(CColliderCapsule* pCapsule, const bool bPush)
{
	D3DXVECTOR3 nearPlayerPos,nearPos2; // 最近接点1,プレイヤー,最近接点2

	// カプセルとカプセルが当たったら
	if (CCollisionCapsule::Collision(m_Capsule.get(), pCapsule,&nearPlayerPos,&nearPos2))
	{
		if (bPush)
		{
			// 敵の方向を求める
			D3DXVECTOR3 dir = nearPos2 - nearPlayerPos;

			// 距離を求める
			float fDistance = D3DXVec3Length(&dir);

			// 方向ベクトルを正規化
			D3DXVec3Normalize(&dir, &dir);

			// 二つの半径を足す
			float fRadius = m_Capsule->GetData().fRadius + pCapsule->GetData().fRadius;

			// どのくらい埋まったか
			float fDepth = fRadius - fDistance;

			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = GetPosition();

			// コライダーの更新 + 埋まった分を戻す
			UpdateCollider(playerPos - dir * fDepth);
		}
		return true;
	}

	return false;
}

//===================================================
// パリィできるか判定
//===================================================
bool CPlayer::IsParry(const D3DXVECTOR3 pos)
{	
	// モーションの取得
	auto pMotion = CCharacter3D::GetMotion();

	// 向きの取得
	D3DXVECTOR3 rot = CCharacter3D::GetRotation();
	
	// 視界の更新処理
	m_pFOV->UpdateData(rot.y);
		
	// 視界内かつ状態が攻撃の時
	if (CCharacter3D::GetState() == CCharacter3D::STATE_ACTION &&
		CCollisionFOV::Collision(pos, m_pFOV.get()) &&
		pMotion->GetBlendType() != MOTIONTYPE_PARRY&&
		pMotion->GetBlendType() != MOTIONTYPE_DAMAGE)
	{
		return true;
	}
	
	return false;
}

//===================================================
// 向きの設定
//===================================================
void CPlayer::SetAngle(const float angleY)
{
	// 向きの設定
	CCharacter3D::SetRotDest(angleY);
}

//===================================================
// 障害物の当たり判定
//===================================================
bool CPlayer::CollisionObstacle(D3DXVECTOR3* pPos)
{
	//	障害物マネージャーのインスタンスの取得
	CObstacleManager* pObstacleManager = CObstacleManager::GetInstance();
	
	// マネージャーが無かったら
	if (pObstacleManager == nullptr) return false;
		
	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	bool bResult = false;

	// 障害物の総数分調べる
	for (auto itr = pObstacleManager->Begin(); itr != pObstacleManager->End(); ++itr)
	{
		// 取得できなかったら処理しない
		if ((*itr) == nullptr)
		{
			continue;
		}

		// 当たっているかどうか
		const bool bCollision = (*itr)->Collision(m_pAABB.get(), pPos);

		// 当たっていたら
		if (!bCollision)
		{
			continue;
		}

		// 種類の取得
		CObstacle::TYPE type = (*itr)->GetType();

		if (type != CObstacle::TYPE_TNT_BARREL)
		{
			if (mode == CScene::MODE_GAME)
			{			
				// ダメージ状態の生成
				auto pDamageState = make_shared<CPlayerDamage>(3);

				// 音の設定
				pDamageState->SetSound(CPlayerDamage::TYPE_SPIKE);

				// ダメージ状態にする
				ChangeState(pDamageState);
			}
			else if (mode == CScene::MODE_TUTORIAL)
			{
				// ダメージ状態の生成
				auto pDamageState = make_shared<CPlayerDamage>(0);

				// 音の設定
				pDamageState->SetSound(CPlayerDamage::TYPE_SPIKE);

				// ダメージ状態にする
				ChangeState(pDamageState);
			}
		}

		bResult = true;
	}

	// 爆発の位置
	D3DXVECTOR3 explosionPos;

	// モーションの取得
	CMotion* pMotion = CCharacter3D::GetMotion();
	
	// モーションの種類の取得
	int nMotionType = pMotion->GetBlendType();

	// 絶対反撃中は攻撃を受けない
	if (nMotionType != MOTIONTYPE_REVENGEATTACK)
	{
		// 爆発の当たり判定
		if (pObstacleManager->CollisionExplotion(m_Capsule.get(), &explosionPos))
		{
			// ダメージ状態に変更
			SetDamageMotion(explosionPos, 2);
		}
	}

	return bResult;
}

//===================================================
// 剣の軌跡
//===================================================
void CPlayer::Orbit(const int nSegH, const D3DXCOLOR col)
{
	// 武器の先端の位置
	D3DXVECTOR3 Top = GetModelPos(9);
	D3DXVECTOR3 Bottom = GetModelPos(11);

	// 軌跡の生成
	if (m_pOrbit == nullptr)
	{
		m_pOrbit = CMeshOrbit::Create(Top, Bottom, nSegH, col);
	}

	// 軌跡が使われていて寿命が無かったら
	if (m_pOrbit != nullptr)
	{
		m_pOrbit->SetPosition(Top, Bottom);
	}
}

//===================================================
// 絶対反撃のエフェクトの設定
//===================================================
void CPlayer::SetRevengeEffect(void)
{
	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	// メッシュフィールドの取得処理
	CMeshField* pMeshField = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// メッシュフィールドの取得
		pMeshField = CGame::GetField();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// メッシュフィールドの取得
		pMeshField = CTutorial::GetField();
	}

	// 左足の位置
	D3DXVECTOR3 FootL = GetModelPos(MODEL_FOOTL);

	// メッシュサークルの生成
	auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 0.5f, 0.5f, 1.0f), FootL, 0.0f, 35.0f);

	// サークルの設定処理
	pCircle->SetCircle(0.0f, 50.0f, 60, true);

	// チュートリアルだったら
	if (mode == CScene::MODE_TUTORIAL) return;

	// フィールドの波の設定
	CMeshFieldWave::Config config = { FootL,250.0f,380.0f,280.0f,12.0f,0.01f,120 };

	if (pMeshField != nullptr)
	{
		// 地面に波を発生させる
		pMeshField->SetWave(config);
	}

	// 瓦礫を出す
	SetRubble(FootL);
}

//===================================================
// ダメージモーションの選択
//===================================================
void CPlayer::SetDamageMotion(const D3DXVECTOR3 AttackerPos,const int nDamage,const float fBack,const float fForward)
{
	// モーションの取得
	CMotion* pMotion = CCharacter3D::GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 現在のモーションの取得
	int nNowMotion = pMotion->GetBlendType();

	// ダメージだったら処理しない
	if (nNowMotion == MOTIONTYPE_DAMAGE || nNowMotion == MOTIONTYPE_BACK_DAMAGE) return;

	// 視界の中だったら
	if (CCollisionFOV::Collision(AttackerPos, m_pFOV.get()))
	{
		// 位置の取得
		D3DXVECTOR3 pos = CCharacter3D::GetPosition();

		// 吹き飛び処理
		float fAngle = m_pMovement->BlowOff(pos - AttackerPos, fBack, 10.0f);

		// 向きの設定
		SetAngle(fAngle);

		// プレイヤーの状態の設定
		ChangeState(make_shared<CPlayerDamage>(nDamage));
	}
	else
	{
		// 向きの取得
		float fAngle = CCharacter3D::GetRotation().y;

		// 前に吹き飛ぶ
		m_pMovement->BlowForward(fForward, 15.0f, fAngle);

		// プレイヤーの状態の設定
		ChangeState(make_shared<CPlayerDamageBack>(nDamage));
	}
}

//===================================================
// 瓦礫の設定
//===================================================
void CPlayer::SetRubble(const D3DXVECTOR3 pos)
{
	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_IMPACT000);
	}
	// 瓦礫の数分出す
	for (int nCnt = 0; nCnt < NUM_RUBBLE; nCnt++)
	{
		// 分割に応じた方向を求める
		float fAngle = (D3DX_PI * 2.0f) / NUM_RUBBLE * nCnt;

		// 吹っ飛び量を選出
		float dir = rand() % 15 + 5.0f;
		float Jump = rand() % 15 + 25.0f;

		// 方向に応じた吹っ飛び量を計算
		float fMoveX = sinf(fAngle) * dir;
		float fMoveZ = cosf(fAngle) * dir;

		// 寿命を選出
		int nLife = rand() % 120 + 60;

		// 種類を選出
		int nType = rand() % CRubble::TYPE_FOUR;

		// 瓦礫を生成
		CRubble::Create(pos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife, nType);
	}
}

//===================================================
// パリィの成功度の設定
//===================================================
void CPlayer::SetParryResult(const int nParry)
{
	// 種類の遷移
	switch (nParry)
	{
	case PARRY_MISS:
		break;
	case PARRY_WEAK:
		m_nWeakCnt++;
		break;
	case PARRY_NORMAL:
		m_nNormalCnt++;
		break;
	case PARRY_PARFECT:
		m_nPerfectCnt++;
		break;
	default:
		break;
	}
}

//===================================================
// 剣の軌跡の削除
//===================================================
void CPlayer::DeleteOrbit(void)
{
	if (m_pOrbit != nullptr)
	{
		m_pOrbit->Uninit();
		m_pOrbit = nullptr;
	}
}

//===================================================
// 構えの設定
//===================================================
void CPlayer::SetStance(const D3DXVECTOR3 enemyPos, const PARRYMOTION parry)
{
	// どのモーションか設定
	m_ParryMotion = parry;

	// 向きの取得
	D3DXVECTOR3 rot = CCharacter3D::GetRotation();

	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	// カメラの取得
	CGameCamera* pCamera = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerPos = CCharacter3D::GetPosition();

	// 角度を求める
	float fAngle = GetTargetAngle(playerPos, enemyPos);

	// 角度を設定
	SetAngle(fAngle);

	pCamera->SetShake(20, 2);

	// ズームインの処理
	pCamera->SetZoomIn(60, rot.y + D3DX_PI);

	// モーションの取得
	auto pMotion = GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTIONTYPE_PARRY, false, 0);

		// モーションを更新してポーズを設定
		CPlayer::UpdateMotion();
	}

	// 左手の位置
	D3DXVECTOR3 playerHandL = CCharacter3D::GetModelPos(MODEL_HANDL);
	
	// パリィエフェクトの生成
	CParryEffect::Create(playerHandL, D3DXVECTOR3(150.0f, 150.0f, 0.0f), D3DXVECTOR3(0.0f, fAngle, 0.0f), 5, 2, 4, false,CParryEffect::TYPE_PARRY);

	// パーティクルの生成
	CParticleSpark* pSpark = CParticleSpark::Create(playerHandL, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(255, 127, 80,255));
	pSpark->SetParticle(15.0f, 60, 150, 1, -180);
	
	pSpark = CParticleSpark::Create(playerHandL, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(106, 90, 205, 255));
	pSpark->SetParticle(15.0f, 60, 150, 1, -180);

	// パーティクルの生成
	CParticle3DNormal *pNormal = CParticle3DNormal::Create(playerHandL, 35.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// パーティクルの設定処理
	pNormal->SetParticle(15.0f, 120, 60, 1, 314);
	pNormal->SetParam(CEffect3D::TYPE_HIT,0);

	// 状態がアクションじゃなかったら抜ける
	if (CCharacter3D::GetState() != STATE::STATE_ACTION) return;

	// 攻撃の有効時間を設定
	m_nAttackCounter = ATTACK_TIME;

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// パーフェクトだったら
	if (m_nParryCounter >= 0 && m_nParryCounter <= 3)
	{
		// パーティクルの生成
		auto pParticle = CParticle2D::Create(D3DXVECTOR3(50.0f, 80.0f, 0.0f), 10.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

		// パーティクルの生成
		pParticle->SetParam(20.0f, 60, 30, 10, D3DXVECTOR3(0.0f, -10.0f, 0.0f));
		pParticle->SetParam(7, 100);

		// レンダラーの取得
		auto pRenderer = CManager::GetRenderer();

		if (pRenderer != nullptr)
		{
			// ブラーの設定
			pRenderer->onEffect(0.8f);
		}

		if (pSound != nullptr)
		{
			// パリィ
			pSound->Play(CSound::SOUND_LABEL_PARRYPARFECT);
		}

		// 成功度の設定
		m_ParryResult = PARRY_PARFECT;

		// 反撃ゲージを加算
		m_fDestRevengeValue += ADD_REVENGE_PERFECT;

		// スタミナを減らす
		SetStamina(-DEC_STAMINA_PERFECT);

		return;
	}
	else if (m_nParryCounter > 3 && m_nParryCounter <= 10)
	{
		// パーティクルの生成
		auto pParticle = CParticle2D::Create(D3DXVECTOR3(50.0f, 80.0f, 0.0f), 10.0f, D3DXCOLOR(0.4f, 1.0f, 1.0f, 1.0f));

		// パーティクルの生成
		pParticle->SetParam(20.0f, 60, 30, 10, D3DXVECTOR3(0.0f, -10.0f, 0.0f));
		pParticle->SetParam(7, 100);

		if (pSound != nullptr)
		{
			// パリィ
			pSound->Play(CSound::SOUND_LABEL_PARRYNORMAL);
		}

		// 成功度の設定
		m_ParryResult = PARRY_NORMAL;

		// 反撃ゲージを加算
		m_fDestRevengeValue += ADD_REVENGE_NORMAL;

		// スタミナを減らす
		SetStamina(-DEC_STAMINA_NORMAL);

		return;
	}
	else if (m_nParryCounter > 10 && m_nParryCounter <= m_nParryTime)
	{
		// パーティクルの生成
		auto pParticle = CParticle2D::Create(D3DXVECTOR3(50.0f, 80.0f, 0.0f), 10.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// パーティクルの生成
		pParticle->SetParam(20.0f, 60, 30, 10, D3DXVECTOR3(0.0f, -10.0f, 0.0f));
		pParticle->SetParam(7, 100);

		if (pSound != nullptr)
		{
			// パリィ
			pSound->Play(CSound::SOUND_LABEL_PARRYWEAK);
		}

		// 成功度の設定
		m_ParryResult = PARRY_WEAK;

		// 反撃ゲージ
		m_fDestRevengeValue += ADD_REVENGE_WEAK;

		// スタミナを減らす
		SetStamina(-DEC_STAMINA_WEAK);

		return;
	}

	// 成功度の設定
	m_ParryResult = PARRY_MISS;
}

//===================================================
// スタミナの設定
//===================================================
void CPlayer::SetStamina(const float fStamina)
{
	m_fStamina += fStamina;
}

//===================================================
// スタミナの設定
//===================================================
void CPlayer::CollisionImpact(CMeshField* pMeshField, D3DXVECTOR3* pPos, CMotion* pMotion)
{
	// nullだったら処理しない
	if (pMeshField == nullptr) return;

	// 最初の位置
	D3DXVECTOR3 firstPos = VEC3_NULL;

	// 衝撃波の位置
	D3DXVECTOR3 ImpactPos = VEC3_NULL;

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// インパクトとの判定
	const bool bCollision = pMeshField->CollisionImpact(*pPos, 150.0f, CMeshFieldImpact::OBJ_PLAYER, &firstPos, &ImpactPos);

	if (bCollision && CCharacter3D::GetState() == STATE::STATE_ACTION)
	{
		// パーティクルの生成
		auto pParticle2D = CParticle2D::Create(D3DXVECTOR3(50.0f, 80.0f, 0.0f), 10.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

		// パーティクルの生成
		pParticle2D->SetParam(20.0f, 60, 30, 10, D3DXVECTOR3(0.0f, -10.0f, 0.0f));
		pParticle2D->SetParam(7, 100);

		// 方向の設定
		D3DXVECTOR3 dir = firstPos - *pPos;

		float fAngle = GetTargetAngle(firstPos, *pPos);

		// 向きの設定
		CCharacter3D::SetRotDest(fAngle + D3DX_PI);

		// 右手の位置
		D3DXVECTOR3 playerHandR = GetModelPos(5);

		// モーションをダメージにする
		pMotion->SetMotion(MOTIONTYPE_PUNCH, true, 2);

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(playerHandR, 20.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// パーティクルの設定処理
		pParticle->SetParticle(15.0f, 240, 25, 2,314);

		// インパクトを生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f), playerHandR, 0.0f, 50.0f, 32);

		// サークルの設定処理
		pCircle->SetCircle(50.0f, 10.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		// 再設定
		pMeshField->ResetImpact(dir, CMeshFieldImpact::OBJ_PLAYER, playerHandR, D3DXCOLOR(1.0f, 1.0f, 0.5f, 1.0f));

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_IMPACT001);
		}

		m_fDestRevengeValue += 30;

		m_nParryCounter++;
	}
	// インパクトの当たり判定
	else if (bCollision && pMotion->GetBlendType() != MOTIONTYPE_DAMAGE)
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_PERFECT);
		}

		// ダメージモーションの設定
		SetDamageMotion(ImpactPos, 7);
	}
}

//===================================================
// ブロックの当たり判定
//===================================================
bool CPlayer::CollisionBlock(D3DXVECTOR3 *pPos)
{
	// ブロックマネージャーの取得
	auto pBlockManager = CBlockManager::GetInstance();

	// 取得できなかったら処理しない
	if (pBlockManager == nullptr) return false;

	// nullだったら処理しない
	if (m_pAABB == nullptr) return false;

	// コライダーの更新
	UpdateCollider(*pPos);

	// ブロックの当たり判定
	if (pBlockManager->Collision(m_pAABB.get(), pPos))
	{
		// コライダーの更新
		UpdateCollider(*pPos);

		return true;
	}

	return false;
}

//===================================================
// 初期設定
//===================================================
void CPlayer::Config(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size)
{
	// モーションのロード処理
	LoadMotion("motionPlayer.txt", MOTIONTYPE_MAX);

	// キャラクターの初期化処理
	CCharacter3D::Init();

	// キャラクターの設定処理
	CCharacter3D::SetCharacter(nLife, fSpeed, ShadowScal, Size);
}

//===================================================
// 移動の更新処理
//===================================================
void CPlayer::UpdateMove(CMotion* pMotion, const bool bAlive, CInputKeyboard* pKeyboard, CInputJoypad* pJoyPad, CGameCamera* pGameCamera)
{
	// 速さ
	float fSpeed = m_bDash ? CCharacter3D::GetSpeed() : 1.5f;

	float fAngleDest = 0.0f;

	// 移動できるなら
	if (IsMove(pMotion) && bAlive)
	{
		// 移動を入力していたら
		const bool bKeyboardMove = m_pMovement->MoveKeyboard(pKeyboard, fSpeed, &fAngleDest);
		const bool bJoypadMove = m_pMovement->MoveJoypad(pJoyPad, fSpeed, &fAngleDest);

		// 移動ごとの処理ができるか判定
		const bool bPlayerMove = bKeyboardMove || bJoypadMove;

		// 移動ごとの処理ができるなら
		if (bPlayerMove)
		{
			CCharacter3D::SetRotDest(fAngleDest);

			// ダッシュモーションか歩きモーションかを判定
			int isDashMotion = (m_bDash ? MOTIONTYPE_DASH : MOTIONTYPE_MOVE);

			// 現在のモーションの取得
			int nNowMotionType = pMotion->GetBlendType();

			// ダウンニュートラルか判定
			const bool isDownNeutral = nNowMotionType == MOTIONTYPE_DOWN_NEUTRAL || nNowMotionType == MOTIONTYPE_DOWN_NEUTRA_BACK;

			// ダウンニュートラルか判定
			int motiontype = isDownNeutral ? MOTIONTYPE_AVOID : isDashMotion;

			// フレームを設定
			const int nFrame = m_bDash ? 5 : 10;

			// モーションの設定
			pMotion->SetMotion(motiontype, true, nFrame);

			// ダッシュ状態だったら
			if (motiontype == MOTIONTYPE_DASH)
			{
				// 状態の変更
				ChangeState(make_shared<CPlayerDash>());
			}
			else if (motiontype == MOTIONTYPE_MOVE)
			{
				// 状態の変更
				ChangeState(make_shared<CPlayerMove>());
			}
			else
			{
				// 移動方向に向ける処理
				SetMoveAngle(pGameCamera, pKeyboard, pJoyPad);

				// 状態の変更
				ChangeState(make_shared<CPlayerAvoid>(25.0f));
			}
		}
		else
		{
			// モーションの種類の取得
			int motiontype = pMotion->GetBlendType();

			// 移動状態だったら
			if (motiontype == MOTIONTYPE_MOVE || motiontype == MOTIONTYPE_DASH && pMotion != nullptr)
			{
				pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 15);

				// 状態の変更
				ChangeState(make_shared<CPlayerNormal>());
			}
		}
	}

	// ダッシュボタンを押したら
	if ((pKeyboard->GetPress(DIK_LSHIFT) || pJoyPad->GetPress(pJoyPad->JOYKEY_LEFT_SHOULDER)))
	{
		m_bDash = true;
	}
	else
	{
		m_bDash = false;
	}
}

//===================================================
// ロックオン状態時の移動処理
//===================================================
void CPlayer::UpdateRockOnMove(CMotion* pMotion, const bool bAlive, CInputKeyboard* pKeyboard, CInputJoypad* pJoyPad, CGameCamera* pGameCamera)
{
	// 速さ
	float fSpeed = SIDE_MOVE_VALUE;

	float fAngleDest = 0.0f;

	// ダッシュボタンを押したら
	if ((pKeyboard->GetPress(DIK_LSHIFT) || pJoyPad->GetPress(pJoyPad->JOYKEY_LEFT_SHOULDER)))
	{
		m_bDash = true;
		return;
	}
	else
	{
		m_bDash = false;
	}

	// 移動できるなら
	if (IsMove(pMotion) && bAlive)
	{
		// 移動方向
		auto MoveDir = CPlayerMovement::MOVE_DIRECTION::NONE;

		// 移動方向
		auto KeyboardmoveDir = m_pMovement->GetMoveKeyboardDir(pKeyboard, fSpeed, &fAngleDest);
		auto JoypadmoveDir = m_pMovement->GetMoveJoypadDir(pJoyPad, fSpeed, &fAngleDest);

		// 移動を入力していたら
		const bool bKeyboardMove = KeyboardmoveDir != CPlayerMovement::MOVE_DIRECTION::NONE;
		const bool bJoypadMove = JoypadmoveDir != CPlayerMovement::MOVE_DIRECTION::NONE;

		// 移動ごとの処理ができるか判定
		const bool bPlayerMove = bKeyboardMove || bJoypadMove;

		// キーボードで移動していたら
		if (bKeyboardMove)
		{
			// 移動方向の設定
			MoveDir = KeyboardmoveDir;
		}
		// パッドで移動していたら
		else if (bJoypadMove)
		{
			// 移動方向の設定
			MoveDir = JoypadmoveDir;
		}

		// 移動ごとの処理ができるなら
		if (bPlayerMove)
		{
			// ダッシュ状態だったら
			if (IsDamage(pMotion))
			{
				// 移動方向に向ける処理
				SetMoveAngle(pGameCamera, pKeyboard, pJoyPad);

				// 状態の変更
				ChangeState(make_shared<CPlayerAvoid>(25.0f));
				return;
			}

			// 移動方向の設定
			switch (MoveDir)
			{
			case CPlayerMovement::MOVE_DIRECTION::LEFT:
				pMotion->SetMotion(MOTIONTYPE_LEFT_MOVE, true, 10);
				break;
			case CPlayerMovement::MOVE_DIRECTION::RIGHT:
				pMotion->SetMotion(MOTIONTYPE_RIGHT_MOVE, true, 10);
				break;
			case CPlayerMovement::MOVE_DIRECTION::BACK:
				pMotion->SetMotion(MOTIONTYPE_BACK_MOVE, true, 10);
				break;
			case CPlayerMovement::MOVE_DIRECTION::FORWARD:
				pMotion->SetMotion(MOTIONTYPE_MOVE, true, 10);
				break;
			default:
				break;
			}
		}
		else
		{
			// 移動状態だったら
			if (GetMoveState(pMotion) && pMotion != nullptr)
			{
				pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 15);

				// 状態の変更
				ChangeState(make_shared<CPlayerNormal>());
			}
		}

		// 注視点の位置の取得
		D3DXVECTOR3 posR = pGameCamera->GetPosR();

		// 位置の取得
		D3DXVECTOR3 pos = CCharacter3D::GetPosition();

		// 向きの取得
		float fAngle = GetTargetAngle(pos, posR);

		// 向きの設定
		SetAngle(fAngle);
	}
}

//===================================================
// 移動状態の取得
//===================================================
bool CPlayer::GetMoveState(CMotion* pMotion)
{
	// モーションの種類
	int motiontypeBlend = pMotion->GetBlendType();

	// 移動状態だったら
	if (motiontypeBlend == MOTIONTYPE_MOVE) return true;

	// 移動状態だったら
	if (motiontypeBlend == MOTIONTYPE_LEFT_MOVE) return true;

	// 移動状態だったら
	if (motiontypeBlend == MOTIONTYPE_DASH) return true;

	// 移動状態だったら
	if (motiontypeBlend == MOTIONTYPE_RIGHT_MOVE) return true;

	// 移動状態だったら
	if (motiontypeBlend == MOTIONTYPE_BACK_MOVE) return true;

	return false;
}

//===================================================
// デバッグ情報
//===================================================
void CPlayer::DebugInfo(void)
{
#ifdef _DEBUG

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	// カメラの取得
	CGameCamera* pCamera = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}

	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	// デバッグ表示
	CDebugProc::Print("カメラの回転 X = %.2f Y = %.2f\n", pCamera->GetRotaition().x, pCamera->GetRotaition().y);

	if (pKeyboard->GetTrigger(DIK_Y))
	{
		// パーティクルの生成
		auto pParticle = CParticleSpark::Create(GetModelPos(2), D3DXVECTOR2(3.0f, 20.0f), D3DXCOLOR(1.0f, 0.2f, 0.2f, 1.0f));

		pParticle->SetParticle(10.0f, 60, 50, 1, 180);
	}

	if (pKeyboard->GetPress(DIK_9))
	{
		m_fDestRevengeValue += 1.0f;
	}

	if (pKeyboard->GetTrigger(DIK_O))
	{
		CTNTBarrel::Create(D3DXVECTOR3(-713.0f, 286.0f, 1613.0f), pos);
		CTNTBarrel::Create(D3DXVECTOR3(656.0f, 286.0f, 1574.0f), pos);
		CTNTBarrel::Create(D3DXVECTOR3(795.0f, 286.0f, -1412.0f), pos);
		CTNTBarrel::Create(D3DXVECTOR3(-685.0f, 286.0f, -1507.0f), pos);
	}

	if (pKeyboard->GetTrigger(DIK_B))
	{
		for (int nCnt = 0; nCnt < 5; nCnt++)
		{
			CBird::Create(D3DXVECTOR3(static_cast<float>(rand() % 2800 - 1400.0f), 0.0f, static_cast<float>(rand() % 2800 - 1400.0f)), CBird::TYPE_NORMAL);
			CBird::Create(D3DXVECTOR3(static_cast<float>(rand() % 2000 - 1000.0f), static_cast<float>(rand() % 200 + 600.0f), static_cast<float>(rand() % 2000 - 1000.0f)), CBird::TYPE_FLY_MOVE, false);
		}
	}

	if (pKeyboard->GetTrigger(DIK_J))
	{
		ChangeState(make_shared<CPlayerJumpAttack>());
	}
#endif // _DEBUG

}

//===================================================
// 移動できるかどうか
//===================================================
bool CPlayer::IsMove(CMotion *pMotion)
{
	// モーションの種類
	int motiontype = pMotion->GetBlendType();
	
	// ダメージ状態だったら移動できない
	if (motiontype == MOTIONTYPE_DAMAGE) return false;
	
	// ダメージ状態だったら移動できない
	if (motiontype == MOTIONTYPE_BACK_DAMAGE) return false;

	// 回避状態だったら移動できない
	if (motiontype == MOTIONTYPE_AVOID) return false;
	
	// カウンター状態だったら移動できない
	if (pMotion->IsEventFrame(0, 40, MOTIONTYPE_PARRY)) return false;
	
	// パリィだったら移動できない
	if (motiontype == MOTIONTYPE_PUNCH) return false;
	
	// 反撃状態だったら移動できない
	if (motiontype == MOTIONTYPE_ROUNDKICK) return false;
	
	// 反撃状態だったら移動できない
	if (motiontype == MOTIONTYPE_REVENGE) return false;

	// 反撃状態だったら移動できない
	if (motiontype == MOTIONTYPE_REVENGEATTACK) return false;

	// ジャンプ攻撃だったら移動できない
	if (motiontype == MOTIONTYPE_JUMP_ATTACK) return false;

	// 構え状態だったら移動できない
	if (pMotion->IsEventFrame(0,35, MOTIONTYPE_STANCE)) return false;
	
	// 移動できる
	return true;
}

//===================================================
// 構えできるかどうか
//===================================================
bool CPlayer::IsStance(CMotion* pMotion)
{
	// モーションの種類
	int motiontypeBlend = pMotion->GetBlendType();
	int motiontype = pMotion->GetType();

	// ダメージモーションだったら
	if (IsDamage(pMotion)) return false;

	// 構え状態だったら
	if (motiontypeBlend == MOTIONTYPE_STANCE || motiontype == MOTIONTYPE_STANCE) return false;
	
	// 回避状態だったら
	if (motiontypeBlend == MOTIONTYPE_AVOID) return false;
	
	// 反撃状態だったら構え出来ない
	if (motiontypeBlend == MOTIONTYPE_REVENGE) return false;

	// 反撃状態だったら構え出来ない
	if (motiontypeBlend == MOTIONTYPE_REVENGEATTACK) return false;

	// 反撃していたら構えできない
	if (motiontypeBlend == MOTIONTYPE_ROUNDKICK || motiontype == MOTIONTYPE_ROUNDKICK) return false;

	// 消費スタミナ以上無かったら構え出来ない
	if (m_fStamina < STANCE_STAMINA) return false;

	return true; 
}

//===================================================
// 回避できるかどうか
//===================================================
bool CPlayer::IsAvoid(CMotion* pMotion)
{
	// モーションの種類の取得
	int motiontype = pMotion->GetBlendType();
	
	// ダメージモーション中だったらできない
	if (pMotion->IsEventFrame(0,20,MOTIONTYPE_DAMAGE) || pMotion->IsEventFrame(0, 20, MOTIONTYPE_BACK_DAMAGE)) return false;

	// 反撃受付時間は回避できない
	if (pMotion->IsEventFrame(1, m_nParryTime, MOTIONTYPE_STANCE)) return false;
	
	// 反撃モーションの時一定時間回避できない
	if (pMotion->IsEventFrame(0, 15, MOTIONTYPE_PARRY)) return false;
	
	// 回避モーションの時回避できない
	if (motiontype == MOTIONTYPE_AVOID || pMotion->GetType() == MOTIONTYPE_AVOID) return false;
	
	// ジャンプ中は回避できない
	if (motiontype == MOTIONTYPE_JUMP) return false;
	
	// 反撃状態だったら移動できない
	if (motiontype == MOTIONTYPE_REVENGE) return false;

	// 反撃状態だったら移動できない
	if (motiontype == MOTIONTYPE_REVENGEATTACK) return false;

	// スタミナが消費分無かったら
	if (m_fStamina < AVOID_STAMINA) return false;
	
	return true;
}

//===================================================
// ダメージ状態かどうか
//===================================================
bool CPlayer::IsDamage(CMotion* pMotion)
{
	// モーションの種類の取得
	int nMotionType = pMotion->GetBlendType();

	// ダメージモーションだったら
	if (nMotionType == CPlayer::MOTIONTYPE_DAMAGE || nMotionType == CPlayer::MOTIONTYPE_BACK_DAMAGE) return true;

	// ダウン状態だったら
	if (nMotionType == CPlayer::MOTIONTYPE_DOWN_NEUTRAL || nMotionType == CPlayer::MOTIONTYPE_DOWN_NEUTRA_BACK) return true;

	return false;
}

//===================================================
// オブザーバーへの通知処理
//===================================================
void CPlayer::Notify(void)
{
	if (m_pHpObserver != nullptr)
	{
		// HPの取得
		int nLife = CCharacter3D::GetLife();

		// HPの変化を通知する
		m_pHpObserver->OnNotify(nLife);
	}

	if (m_pStaminaObserver != nullptr)
	{
		// スタミナの変化を通知する
		m_pStaminaObserver->OnNotify(m_fStamina);
	}

	if (m_pRevengeObserver != nullptr)
	{
		// 反撃量の変化を通知する
		m_pRevengeObserver->OnNotify(m_fRevengeValue);
	}
}

//===================================================
// パリィの更新処理
//===================================================
void CPlayer::UpdateParry(void)
{
	// パリィのカウンター
	if (m_nParryCounter >= m_nParryTime)
	{
		m_nParryCounter = 0;
	}
	else
	{
		m_nParryCounter++;
	}

	// 0以上だったら
	if (m_nAttackCounter >= 0)
	{
		// 攻撃カウンターを減らす
		m_nAttackCounter--;
	}

	// 攻撃状態だったら
	if (CCharacter3D::GetState() == STATE_ACTION)
	{
		// パーティクルの生成
		auto pNormal = CParticle3DNormal::Create(CCharacter3D::GetModelPos(MODEL_CHEST), 10.0f, D3DXCOLOR(0.4f, 1.0f, 1.0f, 1.0f));

		// パーティクルの設定処理
		pNormal->SetParticle(5.0f, 120, 5, 1, 60);
		pNormal->SetParam(CEffect3D::TYPE_NORAML, 100);
	}
}

//===================================================
// 移動時の向きの設定処理
//===================================================
void CPlayer::SetMoveAngle(CGameCamera* pCamera, CInputKeyboard* pKeyboard, CInputJoypad* pJoypad)
{
	// カメラの向き
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// 角度の取得
	D3DXVECTOR3 Angle = CCharacter3D::GetRotation();

	if (pJoypad->GetJoyStickL())
	{
		XINPUT_STATE* pStick;

		pStick = pJoypad->GetJoyStickAngle();

		// Lスティックの角度
		float LStickAngleY = pStick->Gamepad.sThumbLY;
		float LStickAngleX = pStick->Gamepad.sThumbLX;

		// デッドゾーン
		float deadzone = 32767.0f * 0.25f;

		// スティックの傾けた角度を求める
		float magnitude = sqrtf((LStickAngleX * LStickAngleX) + (LStickAngleY * LStickAngleY));

		// 動かせる
		if (magnitude > deadzone)
		{
			// アングルを正規化
			float normalizeX = (LStickAngleX / magnitude);
			float normalizeY = (LStickAngleY / magnitude);

			// プレイヤーの移動量
			float moveX = normalizeX * cosf(-cameraRot.y) - normalizeY * sinf(-cameraRot.y);
			float moveZ = normalizeX * sinf(-cameraRot.y) + normalizeY * cosf(-cameraRot.y);

			// プレイヤーの角度を移動方向にする
			Angle.y = atan2f(-moveX, -moveZ);
		}

		// 向きの設定
		CCharacter3D::SetRotation(Angle);
		CCharacter3D::SetRotDest(Angle.y);

		return;
	}

	if (pKeyboard->GetPress(DIK_A))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W) == true)
		{
			Angle.y = cameraRot.y + D3DX_PI * 0.75f;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			Angle.y = cameraRot.y + D3DX_PI * 0.25f;
		}
		// プレイヤーの移動(左)
		else
		{
			Angle.y = cameraRot.y + D3DX_PI * 0.5f;
		}
	}
	//プレイヤーの移動(右)
	else if (pKeyboard->GetPress(DIK_D))
	{
		//プレイヤーの移動(上)
		if (pKeyboard->GetPress(DIK_W))
		{
			Angle.y = cameraRot.y - D3DX_PI * 0.75f;
		}
		//プレイヤーの移動(下)
		else if (pKeyboard->GetPress(DIK_S))
		{
			Angle.y = cameraRot.y - D3DX_PI * 0.25f;
		}
		// プレイヤーの移動(右)
		else
		{
			Angle.y = cameraRot.y - D3DX_PI * 0.5f;
		}
	}
	//プレイヤーの移動(上)
	else if (pKeyboard->GetPress(DIK_W) == true)
	{
		Angle.y = cameraRot.y + D3DX_PI;
	}
	//プレイヤーの移動(下)
	else if (pKeyboard->GetPress(DIK_S) == true)
	{
		Angle.y = cameraRot.y;
	}

	// 向きの設定
	CCharacter3D::SetRotation(Angle);
	CCharacter3D::SetRotDest(Angle.y);
}