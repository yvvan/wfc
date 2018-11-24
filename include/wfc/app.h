#ifndef _WFC_APP_H_
#define _WFC_APP_H_

#include <string>

class GeneralConfig;
class Model;

void runConfiguruFile(const std::string& fileName);

void run_and_write(const GeneralConfig& generalConfig, const Model& model);

#endif
