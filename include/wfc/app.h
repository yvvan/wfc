#ifndef _WFC_APP_H_
#define _WFC_APP_H_

#include <string>
#include <functional>
#include <experimental/optional>

#include <wfc/imodel.h>

class GeneralConfig;
class Model;

void runConfiguruFile(const std::string& fileName);

using ImageFunction = std::function<std::experimental::optional<Image>(size_t seed)>;

//! \brief Run an image generation function multiple times with different seeds.
void seedLoop(const std::string& name, int screenshots, int maxTries, ImageFunction func);

void runModel(const GeneralConfig& generalConfig, const Model& model);

#endif
