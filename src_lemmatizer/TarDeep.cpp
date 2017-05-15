#include "TarDeep.h"
#include "defines.h"
#include "constants.h"

tagger::TarDeep::TarDeep() 
{
}

tagger::TarDeep::TarDeep(std::locale& inputLocale) :
	TarShallow(inputLocale)
{
}


tagger::TarDeep::~TarDeep()
{
}


std::string tagger::TarDeep::tweakTar(const std::string& wordform)
{
	std::string cleanWordform = TarShallow::tweakTar(wordform);
	// �� => ��
	cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("��([��])", bx::regex_constants::icase), "��$1");
	// ��� => ���
	cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("([��])���", bx::regex_constants::icase), "$1���");
	// nominal inflection
	cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("([��])��$", bx::regex_constants::icase), "$1��");
	// �' => �
	cleanWordform = replaceLj(cleanWordform);
	// Cons => Cons'
	cleanWordform = replaceCons(cleanWordform);

	return cleanWordform;
}


std::string& tagger::TarDeep::replaceLj(std::string& wordform)
{
	if (bx::regex_search(wordform, compiler.compile("�[����]"))){
		wordform = bx::regex_replace(wordform, compiler.compile("������([��])", bx::regex_constants::icase), "�����$1");
		wordform = bx::regex_replace(wordform, compiler.compile("����(��|��|��)", bx::regex_constants::icase), "����$1");
		wordform = bx::regex_replace(wordform, compiler.compile("����", bx::regex_constants::icase), "����");
		wordform = bx::regex_replace(wordform, compiler.compile("����([���])", bx::regex_constants::icase), "����$1");
		wordform = bx::regex_replace(wordform, compiler.compile("���([���])", bx::regex_constants::icase), "���$1");
		wordform = bx::regex_replace(wordform, compiler.compile("�(�|��)", bx::regex_constants::icase), "��$1");
		wordform = bx::regex_replace(wordform, compiler.compile("����", bx::regex_constants::icase), "����");
		wordform = bx::regex_replace(wordform, compiler.compile("����([��])", bx::regex_constants::icase), "����$1");	// BEWARE! "���������"
		wordform = bx::regex_replace(wordform, compiler.compile("([�])���", bx::regex_constants::icase), "$1���");
		wordform = bx::regex_replace(wordform, compiler.compile("���(��|��|���|��|��|��)", bx::regex_constants::icase), "���$1");
		wordform = bx::regex_replace(wordform, compiler.compile("������", bx::regex_constants::icase), "������");	// OUT OF DOMAIN �� => �� CHANGE
		wordform = bx::regex_replace(wordform, compiler.compile("�������", bx::regex_constants::icase), "�������");	// OUT OF DOMAIN � => � CHANGE
		if (bx::regex_search(wordform, compiler.compile("���"))){
			wordform = bx::regex_replace(wordform, compiler.compile("���(�([�����-])|�([��])|�([���])|�)", bx::regex_constants::icase), "���$1");
		}
	}
	return wordform;
}


std::string& tagger::TarDeep::replaceCons(std::string& wordform)
{
	if(bx::regex_search(wordform, compiler.compile("[�������][��]"))){
		wordform = bx::regex_replace(wordform, compiler.compile("([�������])�([��])([��])", bx::regex_constants::icase), "$1�$2$3");	// BEWARE! "�������", "�����������"
		wordform = bx::regex_replace(wordform, compiler.compile("([�����])��", bx::regex_constants::icase), "$1��");
	}
	// � => �'
	if (bx::regex_search(wordform, compiler.compile("�[��]"))){
		wordform = bx::regex_replace(wordform, compiler.compile("������", bx::regex_constants::icase), "�����");
		wordform = bx::regex_replace(wordform, compiler.compile("�����", bx::regex_constants::icase), "����");
		wordform = bx::regex_replace(wordform, compiler.compile("����([��])", bx::regex_constants::icase), "���$1");
		wordform = bx::regex_replace(wordform, compiler.compile("����", bx::regex_constants::icase), "����");
	}

	// � => �'
	if (bx::regex_search(wordform, compiler.compile("�[��]"))){
		wordform = bx::regex_replace(wordform, compiler.compile("����", bx::regex_constants::icase), "���");
		wordform = bx::regex_replace(wordform, compiler.compile("����", bx::regex_constants::icase), "����");	// BEWARE! "�����", "�������"
		wordform = bx::regex_replace(wordform, compiler.compile("���([��])", bx::regex_constants::icase), "��$1");
		wordform = bx::regex_replace(wordform, compiler.compile("����", bx::regex_constants::icase), "���");
		wordform = bx::regex_replace(wordform, compiler.compile("����", bx::regex_constants::icase), "���");
	}

	// � => �'
	if (bx::regex_search(wordform, compiler.compile("��"))){
		wordform = bx::regex_replace(wordform, compiler.compile("��(���|����|�([��]|��)|�(���|���|��[��]|[��])|���)", bx::regex_constants::icase), "��$1");
	}

	// � => �'
	if (bx::regex_search(wordform, compiler.compile("��"))){
		wordform = bx::regex_replace(wordform, compiler.compile("��(�|[��]�|��)", bx::regex_constants::icase), "��$1");
	}

	// � => �'
	if (bx::regex_search(wordform, compiler.compile("��"))){
		wordform = bx::regex_replace(wordform, compiler.compile("��(�([����])|���|[��]��)", bx::regex_constants::icase), "��$1");
	}

	// � => �'
	if (bx::regex_search(wordform, compiler.compile("��"))){
		wordform = bx::regex_replace(wordform, compiler.compile("��([������]|�)", bx::regex_constants::icase), "��$1");	// BEWARE! "�������", "������"
	}

	// � => �'
	if (bx::regex_search(wordform, compiler.compile("��"))){
		wordform = bx::regex_replace(wordform, compiler.compile("��([��]�|�[^��])", bx::regex_constants::icase), "��$1");
		wordform = bx::regex_replace(wordform, compiler.compile("����", bx::regex_constants::icase), "�����");	// OUT OF DOMAIN � => �� CHANGE
		wordform = bx::regex_replace(wordform, compiler.compile("������", bx::regex_constants::icase), "������");	// OUT OF DOMAIN �� => �� CHANGE
		wordform = bx::regex_replace(wordform, compiler.compile("(\\w)����", bx::regex_constants::icase), "$1����");
		wordform = bx::regex_replace(wordform, compiler.compile("�����", bx::regex_constants::icase), "�����");
		wordform = bx::regex_replace(wordform, compiler.compile("�������", bx::regex_constants::icase), "�������");
		wordform = bx::regex_replace(wordform, compiler.compile("�����", bx::regex_constants::icase), "�����");
		wordform = bx::regex_replace(wordform, compiler.compile("������", bx::regex_constants::icase), "������");
	}

	// � => �'
	if (bx::regex_search(wordform, compiler.compile("��"))){
		wordform = bx::regex_replace(wordform, compiler.compile("��([��][��]|��)", bx::regex_constants::icase), "��$1");
		wordform = bx::regex_replace(wordform, compiler.compile("([��])����", bx::regex_constants::icase), "$1����");
		wordform = bx::regex_replace(wordform, compiler.compile("�����", bx::regex_constants::icase), "�����");	// OUT OF DOMAIN � => � CHANGE
	}
	return wordform;
}