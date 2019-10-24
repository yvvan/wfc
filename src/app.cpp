#include <wfc/app.h>

#include <wfc/algorithm.h>
#include <wfc/configuru.h>

#include <stb_image_write.h>

#include <iostream>
#include <sstream>

void runConfiguruFile(const std::string &fileName) {

  ConfigActions actions = {
      [](const GeneralConfig &generalConfig,
         const OverlappingModelConfig &overlappingModelConfig) {
        auto computedInfo = fromConfig(overlappingModelConfig);
        auto imageGenerator =
            overlappingGenerator(computedInfo, generalConfig.limit);
        seedLoop(generalConfig.name, generalConfig.numOutput, imageGenerator);
      },
      [](const GeneralConfig &generalConfig,
         const TileModelConfig &tileModelConfig) {
        auto internal = fromConfig(tileModelConfig);
        auto imageGenerator = tileGenerator(internal, generalConfig.limit);
        seedLoop(generalConfig.name, generalConfig.numOutput, imageGenerator);
      }};

  run_config_file(fileName, actions);
}

void seedLoop(const std::string &name, int numOutput,
              const ImageGenerator &func) {
  int numTries = 0;
  int numSuccess = 0;
  const int maxTries = 10 * numOutput;

  const int desiredSuccess = numOutput;

  int randSeed = 0;

  while (numTries < maxTries && numSuccess < desiredSuccess) {
    ++numTries;
    // Generate an image based on the seed
    auto result = func(randSeed++);

    if (result) {
      const auto &image = *result;
      std::stringstream out_path;
      out_path << "output/" << name << "_" << numSuccess << ".png";
      if (stbi_write_png(out_path.str().c_str(), image.size().width,
                         image.size().height, 4, image.data(), 0) == 0) {
        std::cout << "Failed to write image to " << out_path.str() << "\n";
      }

      ++numSuccess;
    }
  }
}
