//
// Copyright by Persival GmbH 2025
//

#include "Model.h"

#include <fstream>

void Model::Init(const std::string& model_reference_map_path) {
  std::cout << "Loading map file: " << model_reference_map_path << std::endl;
  std::ifstream json_file(model_reference_map_path);
  if (!json_file.is_open()) {
    throw std::runtime_error("Failed to open model_reference map JSON file.");
  }
  model_reference_map_ = nlohmann::json::parse(json_file);
}

bool Model::Step(osi3::SensorView& sensor_view) {
  // static environment
  const auto current_static_environment = sensor_view.global_ground_truth().model_reference();
  if (model_reference_map_.contains("static_environment")) {
    sensor_view.mutable_global_ground_truth()->set_model_reference(model_reference_map_["static_environment"]);
    std::cout << "Changed model_reference of static environment" << std::endl;   //todo: implement FMI logging
  }
  else {
    std::cout << "model_reference of the static environment (keyword 'static_environment') not in the map: " << current_static_environment << std::endl;   //todo: implement FMI logging
  }

  // moving objects
  auto *moving_objects = sensor_view.mutable_global_ground_truth()->mutable_moving_object();
  for (auto& moving_object : *moving_objects) {
    const auto current_model_reference = moving_object.model_reference();
    if (model_reference_map_.contains(moving_object.model_reference())) {
      moving_object.set_model_reference(model_reference_map_[moving_object.model_reference()]);
      std::cout << "Changed model_reference of object " << moving_object.id().value() << std::endl;   //todo: implement FMI logging
    }
    else {
      std::cout << "model_reference of object " << moving_object.id().value() << " not in the map: " << current_model_reference << std::endl;   //todo: implement FMI logging
    }
  }

  return true; // NOLINT // for now this function returns always true
}
