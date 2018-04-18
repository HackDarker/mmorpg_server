#include "../common/configMgr.h"
#include "gameServer.h"
#include "mapModule.h"

int main(int argc, char const *argv[])
{
	char* config_file = "./config";
	if (argc > 1) {
		config_file = argv[1];
	}	
	ConfigMgr::Instance()->LoadConfig(config_file);

	GameServer* game = GameServer::Instance();
	game->RegisterModule(MAP_MODULE,new MapModule);
	game->Run();

	return 0;
}
