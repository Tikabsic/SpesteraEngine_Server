#include "GameCharacterService.h"

GameCharacterService::GameCharacterService(std::string uri)
{
	repository_ = new GameCharacterRepository(uri);
}

GameCharacterService::~GameCharacterService()
{

	delete repository_;
	repository_ = nullptr;

}

DatabaseResponseWrapper GameCharacterService::get_character_list(DatabaseRequestWrapper& wrapper)
{
	DatabaseResponseWrapper response_wrapper;
	return response_wrapper;
}

