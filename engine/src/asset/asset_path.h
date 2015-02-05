#ifndef ASSET_PATH_H_
#define ASSET_PATH_H_

#include "utils/s2string.h"

namespace s2 {

static const char *kAssetRoot = "d:/github_repository/engine/engine/test/";

static inline s2string ResolveAssetPath(const s2string &input) {
	return kAssetRoot + input;
}

}

#endif