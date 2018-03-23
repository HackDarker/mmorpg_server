#include "../common/configMgr.h"
#include "gameServer.h"
#include "dbModule.h"

int main(int argc, char *argv[])
{
	char* config_file = "./config";
	if (argc > 1) {
		config_file = argv[1];
	}	
	ConfigMgr::Instance()->LoadConfig(config_file);

	GameServer* game = GameServer::Instance();
	game->RegisterModule(DB_MODULE,new DbModule);
	game->Run();

	return 0;
}
