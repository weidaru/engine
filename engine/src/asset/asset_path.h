#ifndef ASSET_PATH_H_
#define ASSET_PATH_H_

#include "utils/s2string.h"

namespace s2 {

static const char *kTestAssetRoot = "D:/github_repository/engine/engine/test/";
//"C:/Users/zhiwshen/Documents/GitHub/engine/engine/test/"
//"D:/github_repository/engine/engine/test/"


static const char *kAssetRoot = "D:/github_repository/engine/engine/content/";


static inline s2string ResolveTestAssetPath(const s2string &input) {
	return kTestAssetRoot + input;
}

static inline s2string ResolveAssetPath(const s2string &input) {
	return kAssetRoot + input;
}

}

#endif