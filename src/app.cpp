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
			runModel(generalConfig, model);
		},
		.tileAction = [] (const GeneralConfig& generalConfig, const TileModelInternal& internal)
		{
			TileModel model(internal);
			runModel(generalConfig, model);
		}
	};

	run_config_file(fileName, actions);
}

void runModel(const GeneralConfig& generalConfig, const Model& model)
{
	auto imageFunc = [&] (size_t seed)
	{
		return createImage(model, seed, generalConfig.limit);
	};

	run_and_write(generalConfig.name, generalConfig.screenshots, imageFunc);
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

