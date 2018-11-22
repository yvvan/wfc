#include <wfc/algorithm.h>
#include <wfc/configuru.h>

const auto kUsage = R"(
wfc.bin [-h/--help] [job=samples.cfg, ...]
	-h/--help   Print this help
	file        Jobs to run
)";

int main(int argc, char* argv[])
{
	loguru::init(argc, argv);

	std::vector<std::string> files;

	for (int i = 1; i < argc; ++i) 
	{
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) 
		{
			printf(kUsage);
			exit(0);
		} 
		else 
		{
			files.push_back(argv[i]);
		}
	}

	if (files.empty()) 
	{
		files.push_back("samples.cfg");
	}

	ConfigActions actions =
	{
		.overlappingAction = [] (const GeneralConfig& generalConfig, const OverlappingModelConfig& overlappingModelConfig)
		{
			OverlappingModel model(overlappingModelConfig);
			run_and_write(generalConfig.name, generalConfig.limit, generalConfig.screenshots, model);
		},
		.tileAction = [] (const GeneralConfig& generalConfig, const TileModelInternal& internal)
		{
			TileModel model(internal);

			run_and_write(generalConfig.name, generalConfig.limit, generalConfig.screenshots, model);
		}
	};

	for (const auto& file : files) 
	{
		run_config_file(file, actions);
	}
}
