//===================================================
//
// �v�Z [math.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "math.h"

//===================================================
// �����̎擾
//===================================================
float math::GetDistance(const D3DXVECTOR3 diff)
{
	// �������悷��
	float sqX = diff.x * diff.x;
	float sqY = diff.y * diff.y;
	float sqZ = diff.z * diff.z;
	
	// ���������߂�
	float Out = sqrtf(sqX + sqY + sqZ);
	
	return Out;
}

//===================================================
// �ړI�̕����x�N�g���̎擾
//===================================================
D3DXVECTOR3 math::GetVector(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos)
{
	// ���������߂�
	D3DXVECTOR3 diff = dest - pos;

	// ���K������
	D3DXVec3Normalize(&diff, &diff);

	return diff;
}

//===================================================
// �ړI�܂ł̊p�x�̎擾
//===================================================
float math::GetTargetAngle(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos)
{
	D3DXVECTOR3 diff = dest - pos;

	float fAngle = atan2f(diff.x, diff.z);

	return fAngle;
}

//===================================================
// �ʒu�̍s���D3DXVECTOR3�^�ɕϊ�
//===================================================
D3DXVECTOR3 math::GetPositionFromMatrix(const D3DXMATRIX mtxWorld)
{
	D3DXVECTOR3 pos(mtxWorld._41, mtxWorld._42, mtxWorld._43);

	return pos;
}

//===================================================
// �N�E�H�[�^��I���̎��̍쐬����
//===================================================
D3DXQUATERNION math::CreateQuaternion(D3DXQUATERNION* pOut, D3DXVECTOR3 axis, float fAngle)
{
	// �N�E�H�[�^��I��
	D3DXQUATERNION aQuaternion;

	// �p�x�𔼕��ɂ���(�N�E�H�[�^��I���͔����̊p�x�����ƂɌv�Z���邽��)
	float angle = fAngle / 2.0f;

	D3DXVECTOR3 NormalizeAxis;

	// ���𐳋K������
	D3DXVec3Normalize(&NormalizeAxis, &axis);

	// �N�E�H�[�^��I���̌v�Z
	aQuaternion.w = cosf(angle);
	aQuaternion.x = sinf(angle) * NormalizeAxis.x;
	aQuaternion.y = sinf(angle) * NormalizeAxis.y;
	aQuaternion.z = sinf(angle) * NormalizeAxis.z;

	// �v�Z���ʂ�����
	*pOut = aQuaternion;

	return aQuaternion;
}

//===================================================
// �N�E�H�[�^��I������]�s��ɕϊ�����
//===================================================
D3DXMATRIX math::SetMatrixRotationQuaternion(D3DXMATRIX* pOut, D3DXQUATERNION quaternion)
{
	// �v�Z�p���[���h�}�g���b�N�X
	D3DXMATRIX outMatrix;

	// �s�����������
	D3DXMatrixIdentity(&outMatrix);

	// �p�x��2��
	float sqX = quaternion.x * quaternion.x;
	float sqY = quaternion.y * quaternion.y;
	float sqZ = quaternion.z * quaternion.z;
	//float sqW = quaternion.w * quaternion.w;

	// �s��_11 _21 _31�̌v�Z
	outMatrix._11 = 1 - (2 * (sqY + sqZ));
	outMatrix._21 = (2 * ((quaternion.x * quaternion.y) + (quaternion.w * quaternion.z)));
	outMatrix._31 = (2 * ((quaternion.x * quaternion.z) + (quaternion.w * quaternion.y)));

	// �s��_12 _22 _32�̌v�Z
	outMatrix._12 = 2 * ((quaternion.x * quaternion.y) - (quaternion.w * quaternion.z));
	outMatrix._22 = 1 - (2 * (sqX + sqZ));
	outMatrix._32 = (2 * ((quaternion.y * quaternion.z) + (quaternion.w * quaternion.x)));

	// �s��_13 _23 _33�̌v�Z
	outMatrix._13 = 2 * ((quaternion.x * quaternion.z) - (quaternion.w * quaternion.y));
	outMatrix._23 = 2 * ((quaternion.y * quaternion.z) - (quaternion.w * quaternion.x));
	outMatrix._33 = 1 - (2 * (sqX + sqY));

	outMatrix._14 = 0.0f;
	outMatrix._24 = 0.0f;
	outMatrix._34 = 0.0f;

	outMatrix._41 = 0.0f;
	outMatrix._42 = 0.0f;
	outMatrix._43 = 0.0f;
	outMatrix._44 = 1.0f;

	// �v�Z���ʂ���
	*pOut = outMatrix;

	// �v�Z�����}�g���b�N�X��Ԃ�
	return outMatrix;
}

//===================================================
// ��]�s����I�C���[�p�ɕϊ�
//===================================================
D3DXVECTOR3 math::MatrixToEulerXYZ(const D3DXMATRIX mtxWorld)
{
	D3DXVECTOR3 rot;

	if (fabsf(mtxWorld._32) < 1.0f)
	{
		rot.x = asinf(-mtxWorld._32);
		rot.y = atan2f(mtxWorld._31,mtxWorld._33);
		rot.z = atan2f(mtxWorld._12, mtxWorld._22);
	}
	else
	{
		rot.x = (mtxWorld._32 <= -1.0f) ? D3DX_PI * 0.5f : -D3DX_PI * 0.5f;
		rot.y = atan2f(-mtxWorld._13, mtxWorld._11);
		rot.z = 0.0f;
	}

	return rot;
}
