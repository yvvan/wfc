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
			run_and_write(generalConfig, model);
		},
		.tileAction = [] (const GeneralConfig& generalConfig, const TileModelInternal& internal)
		{
			TileModel model(internal);

			run_and_write(generalConfig, model);
		}
	};

	run_config_file(fileName, actions);
}

void run_and_write(const GeneralConfig& generalConfig, const Model& model)
{
	int numTries = 0;
	int numSuccess = 0;

	const int maxTries = 20;
	const int desiredSuccess = generalConfig.screenshots;
	
	int randSeed = 0;

	while (numTries < maxTries && numSuccess < desiredSuccess)
	{
		++numTries;
		int seed = randSeed++;

		Output output = create_output(model);

		const auto result = run(output, model, seed, generalConfig.limit);

		if (result == Result::kSuccess) 
		{
			const auto image = model.image(output);
			const auto out_path = emilib::strprintf("output/%s_%d.png", generalConfig.name.c_str(), numSuccess);
			CHECK_F(stbi_write_png(out_path.c_str(), image.width(), image.height(), 4, image.data(), 0) != 0,
					"Failed to write image to %s", out_path.c_str());

			++numSuccess;
		}
	}
}
