//===================================================
//
// ���[�h�}�l�[�W���[ [LoadManager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _LOADMANAGER_H_
#define _LOADMANAGER_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>

using namespace std; // ���O���std���g�p

//***************************************************
// ���[�h�̃}�l�[�W���[�N���X�̒�`
//***************************************************
class CLoadManager
{
public:
	CLoadManager();
	~CLoadManager();
	istringstream SetInputvalue(string input); // = �����̐��l�����o������
	string GetString(string input);	   // std::string���當��������o������
private:
};

#endif
