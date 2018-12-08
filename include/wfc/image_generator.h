#ifndef _WFC_IMAGE_GENERATOR_H_
#define _WFC_IMAGE_GENERATOR_H_

#include <functional>
#include <experimental/optional>

#include <wfc/imodel.h>

using ImageGenerator = std::function< std::experimental::optional<Image>(size_t) >;

#endif
