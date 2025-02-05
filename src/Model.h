//
// Copyright by Persival GmbH 2025
//

#ifndef MODEL_H
#define MODEL_H

#include "osi_sensordata.pb.h"

#include "json.hpp"

/** @brief The Model class contains the main model functionality with all necessary functions.
 *
 * The Init function initializes the model and loads the model_reference mapping file.
 * The step function is called in every cycle and updates the model_references of the stationary environment and the moving objects.
 * */
class Model {
public:
  /**
   * @brief Initializes the model.
   *
   * It has to be called in the first simulation time step.
   *
   * @param model_reference_map_path Absolute path to model reference mapping json file
   */
  void Init(const std::string& model_reference_map_path);

  /**
   * @brief This method gets called each iteration and updates the model_reference fields.
   *
   * @param sensor_view OSI SensorView containing the ground truth moving and stationary objects with the model_references that are to be changed
   *
   * @return bool Returns true if the data was updated.
   */
  bool Step(osi3::SensorView& sensor_view);

protected:
  nlohmann::json model_reference_map_;

};

#endif  // MODEL_H
