#ifndef INCLUDED_VFM_JSON
#define INCLUDED_VFM_JSON

#include <nlohmann/json.hpp>

#include <string>

#include <vfm_types.h>

namespace vfm {

nlohmann::json matchResultsToJson(const MatchResults& results);

std::string matchResultsToJsonString(const MatchResults& results, int indent = -1);

bool writeMatchResultsToFile(const MatchResults& results,
                              const std::string& filename,
                              int indent = 4);

}

#endif
