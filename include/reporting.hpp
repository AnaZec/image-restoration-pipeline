#ifndef REPORTING_HPP
#define REPORTING_HPP

#include "experiment_types.hpp"

#include <string>
#include <vector>

namespace irp
{

std::string buildExperimentReport(const ExperimentResult& result);
std::string buildGlobalReport(const std::vector<ExperimentResult>& results,
                              const std::string& inputName);
std::string buildCSVReport(const std::vector<ExperimentResult>& results);

} // namespace irp

#endif // REPORTING_HPP