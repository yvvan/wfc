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
			OverlappingModel model(overlappingModelConfig);

			auto imageFunc = [&] (size_t seed)
			{
				return createImage(model, seed, generalConfig.limit);
			};

			run_and_write(generalConfig.name, generalConfig.screenshots, imageFunc);
		},
		.tileAction = [] (const GeneralConfig& generalConfig, const TileModelInternal& internal)
		{
			TileModel model(internal);

			auto imageFunc = [&] (size_t seed)
			{
				return createImage(model, seed, generalConfig.limit);
			};

			run_and_write(generalConfig.name, generalConfig.screenshots, imageFunc);
		}
	};

	run_config_file(fileName, actions);
}

void run_and_write(const std::string& name, int screenshots, ImageFunction func)
{
	int numTries = 0;
	int numSuccess = 0;

	const int maxTries = 20;
	const int desiredSuccess = screenshots;
	
	int randSeed = 0;

	while (numTries < maxTries && numSuccess < desiredSuccess)
	{
		++numTries;
		int seed = randSeed++;

		auto result = func(seed);

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

