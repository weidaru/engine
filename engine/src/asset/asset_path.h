#ifndef ASSET_PATH_H_
#define ASSET_PATH_H_

#include "utils/s2string.h"

namespace s2 {

static const char *kAssetRoot = "D:/github_repository/engine/engine/";
//"C:/Users/zhiwshen/Documents/GitHub/engine/engine/"
//"D:/github_repository/engine/engine/"

static inline s2string ResolveTestAssetPath(const s2string &input) {
	return s2string(kAssetRoot) + "test/" + input;
}

static inline s2string ResolveAssetPath(const s2string &input) {
	return s2string(kAssetRoot) + "content/" + input;
}

}

#endif