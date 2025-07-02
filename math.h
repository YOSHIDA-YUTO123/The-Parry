//===================================================
//
// �v�Z [math.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _MATH_H_
#define _MATH_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"

// �͈͓��Ɏ��߂�
template <class T> T Clamp(const T value, const T min, const T max)
{
	if (value < min) return min;

	if (value > max) return max;

	return value;
};

// �͈͓��ɂ����[�v����
template <class T> T Wrap(const T value, const T min, const T max)
{
	if (value < min) return max;

	if (value > max) return min;

	return value;
};

// �p�x�𐳋K������
template <class T> T NormalizeRot(T* value)
{
	if ((*value) < -D3DX_PI)
	{
		(*value) += D3DX_PI * 2.0f;
	}
	else if ((*value) >D3DX_PI)
	{
		(*value) += -D3DX_PI * 2.0f;
	}

	return (*value);
};

// �ړI�̊p�x�܂ł̋��������ߐ��K������
template <class T> T NormalizeDiffRot(const T Diff, T* value)
{
	if (Diff <= -D3DX_PI)
	{
		(*value) += -D3DX_PI * 2.0f;
	}
	else if (Diff >= D3DX_PI)
	{
		(*value) += D3DX_PI * 2.0f;
	}

	return *value;
};

// �ړI�̒l�܂ł̕��
template <class T> T LerpDest(const T dest,const T value,const float coef)
{
	T out = value + (dest - value) * coef;

	return out;
};

// �ړI�̒l�܂ł̕��
template <class T> T LerpDiff(const T offset, const T Diff, const float fRate)
{
	T out = offset + (Diff * fRate);

	return out;
};

// 
template <class T> bool IsInRange(const T value,const T min,const T max)
{
	if (value >= min && value <= max)
	{
		return true;
	}

	return false;
};

//***************************************************
// �v�Z�pnamespace�̒�`
//***************************************************
namespace math
{
	float GetDistance(const D3DXVECTOR3 diff);
	D3DXVECTOR3 GetVector(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos);
	float GetTargetAngle(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos); // �ڕW�܂ł̊p�x�̎擾
	D3DXVECTOR3 GetPositionFromMatrix(const D3DXMATRIX mtxWorld);		 // MATRIX�̈ʒu�̏���D3DXVECTOR3�^�ɕϊ�����
	D3DXQUATERNION CreateQuaternion(D3DXQUATERNION* pOut, D3DXVECTOR3 axis, float fAngle); 
	D3DXMATRIX SetMatrixRotationQuaternion(D3DXMATRIX* pOut, D3DXQUATERNION quaternion);
	D3DXVECTOR3 MatrixToEulerXYZ(const D3DXMATRIX mtxWorld);			// ��]�s����I�C���[�p�ɕϊ�
}
#endif