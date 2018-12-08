#ifndef _WFC_APP_H_
#define _WFC_APP_H_

#include <string>
#include <functional>
#include <experimental/optional>

#include <wfc/image_generator.h>

void runConfiguruFile(const std::string& fileName);

//! \brief Run an image generation function multiple times with different seeds.
void seedLoop(const std::string& name, int numOutput, const ImageGenerator& func);

#endif
