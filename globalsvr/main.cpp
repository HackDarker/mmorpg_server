#include "gameServer.h"
#include "globalModule.h"

int main(int argc, char const *argv[])
{
	char* config_file = "./config";
	if (argc > 1) {
		config_file = argv[1];
	}	
	ConfigMgr::Instance()->LoadConfig(config_file);
	
	GameServer* game = GameServer::Instance();
	game->RegisterModule(GLOBAL_MODULE,new GlobalModule);
	game->Run();

	return 0;
}
