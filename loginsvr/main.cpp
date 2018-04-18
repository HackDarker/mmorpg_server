#include "../common/configMgr.h"
#include "gameServer.h"
#include "loginModule.h"

int main(int argc, char const *argv[])
{
	GameServer* game = GameServer::Instance();
	game->RegisterModule(LOGINWAY_MODULE,new LoginModule);
	game->Run();

	return 0;
}
