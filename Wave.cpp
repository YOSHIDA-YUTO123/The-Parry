#include "pch.h"
#include "Wave.h"

CMeshWave::CMeshWave()
{
}

CMeshWave::~CMeshWave()
{
}

CMeshWave* CMeshWave::Create(const D3DXVECTOR3 pos, const float fRadius, const float fHeight, const float speed, const int nLife, const int nSegX, const D3DXCOLOR col, const D3DXVECTOR3 rot)
{
	return nullptr;
}

HRESULT CMeshWave::Init(void)
{
	return E_NOTIMPL;
}

void CMeshWave::Uninit(void)
{
}

void CMeshWave::Update(void)
{
}

void CMeshWave::Draw(void)
{
}

void CMeshWave::SetWave(const int nSegX, const float fRadius, const float fHeight)
{
}
