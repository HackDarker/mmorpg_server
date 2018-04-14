#include "../common/configMgr.h"

int main(int argc, char *argv[])
{
	char* config_file = "./config";
	if (argc > 1) {
		config_file = argv[1];
	}	
	ConfigMgr::Instance()->LoadConfig(config_file);
}
