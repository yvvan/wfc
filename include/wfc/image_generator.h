#pragma once

#include <functional>

#include <wfc/imodel.h>

using ImageGenerator =
    std::function<std::unique_ptr<Image>(size_t)>;
