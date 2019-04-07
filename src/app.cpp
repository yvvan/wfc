#include <wfc/app.h>

#include <wfc/algorithm.h>
#include <wfc/configuru.h>

#include <stb_image_write.h>

#include <emilib/strprintf.hpp>

void runConfiguruFile(const std::string& fileName)
{

	ConfigActions actions =
	{
		.overlappingAction = [] (const GeneralConfig& generalConfig, const OverlappingModelConfig& overlappingModelConfig)
		{
			auto computedInfo = fromConfig(overlappingModelConfig);
			auto imageGenerator = overlappingGenerator(computedInfo, generalConfig.limit);
			seedLoop(generalConfig.name, generalConfig.numOutput, imageGenerator);
		},
		.tileAction = [] (const GeneralConfig& generalConfig, const TileModelConfig& tileModelConfig)
		{
			auto internal = fromConfig(tileModelConfig);
			auto imageGenerator = tileGenerator(internal, generalConfig.limit);
			seedLoop(generalConfig.name, generalConfig.numOutput, imageGenerator);
		}
	};

	run_config_file(fileName, actions);
}

void seedLoop(const std::string& name, int numOutput, const ImageGenerator& func)
{
	int numTries = 0;
	int numSuccess = 0;
	const int maxTries = 10 * numOutput;

	const int desiredSuccess = numOutput;
	
	int randSeed = 0;

	while (numTries < maxTries && numSuccess < desiredSuccess)
	{
		++numTries;

		// Generate an image based on the seed
		auto result = func(randSeed++);

		if (result) 
		{
			const auto& image = *result;
			const auto out_path = emilib::strprintf("output/%s_%d.png", name.c_str(), numSuccess);
			CHECK_F(stbi_write_png(out_path.c_str(), image.size().width, image.size().height, 4, image.data(), 0) != 0,
					"Failed to write image to %s", out_path.c_str());

			++numSuccess;
		}
	}
}

