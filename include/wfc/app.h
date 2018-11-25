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

void run_and_write(const std::string& name, int screenshots, ImageFunction func);

void runModel(const GeneralConfig& generalConfig, const Model& model);

#endif
