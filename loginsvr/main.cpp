#include "loginModule.h"

int main(int argc, char const *argv[])
{
	GameServer* game = GameServer::Instance();
	game->RegisterModule(LOGINWAY_MODULE,new loginModule);
	game->Run();

	return 0;
}