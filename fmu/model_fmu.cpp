
#include <fmu4cpp/fmu_base.hpp>

#include "../src/Model.h"
#include "osi_version.pb.h"

using namespace fmu4cpp;

namespace {

const auto& GetOsiVersion() {
  static const auto version = osi3::InterfaceVersion::descriptor()->file()->options().GetExtension(osi3::current_interface_version);
  return version;
}

const auto& GetOsiVersionString() {
  static const auto version_string =
      std::to_string(GetOsiVersion().version_major()) + "." + std::to_string(GetOsiVersion().version_minor()) + "." + std::to_string(GetOsiVersion().version_patch());
  return version_string;
}

void* DecodeIntegerToPointer(fmi2Integer hi, fmi2Integer lo) {
#if PTRDIFF_MAX == INT64_MAX
  union Addrconv {
    struct {
      int lo;
      int hi;
    } base;

    unsigned long long address;
  } myaddr{};

  myaddr.base.lo = lo;
  myaddr.base.hi = hi;
  return reinterpret_cast<void*>(myaddr.address);
#elif PTRDIFF_MAX == INT32_MAX
  return reinterpret_cast<void*>(lo);
#else
#error "Cannot determine 32bit or 64bit environment!"
#endif
}

void EncodePointerToInteger(const void* ptr, fmi2Integer& hi, fmi2Integer& lo) {
#if PTRDIFF_MAX == INT64_MAX
  union Addrconv {
    struct {
      int lo;
      int hi;
    } base;

    unsigned long long address;
  } myaddr{};

  myaddr.address = reinterpret_cast<unsigned long long>(ptr);
  hi = myaddr.base.hi;
  lo = myaddr.base.lo;
#elif PTRDIFF_MAX == INT32_MAX
  hi = 0;
  lo = reinterpret_cast<int>(ptr);
#else
#error "Cannot determine 32bit or 64bit environment!"
#endif
}

}  // namespace

class ModelFMU final : public fmu_base {

public:
  ModelFMU(const std::string& instance_name, const std::filesystem::path& resources) : fmu_base(instance_name, resources),
  current_output_buffer_(std::make_unique<std::string>()),
  last_output_buffer_(std::make_unique<std::string>()) {

    register_variable(integer("OSMPSensorViewIn.base.lo", &osmp_sensor_view_in_base_lo_)
                          .setCausality(causality_t::INPUT)
                          .setVariability(variability_t::DISCRETE)
                          .addAnnotation("<Tool name=\"net.pmsf.osmp\" xmlns:osmp=\"http://xsd.pmsf.net/OSISensorModelPackaging\"><osmp:osmp-binary-variable "
                                         "name=\"OSMPSensorViewIn\" role=\"base.lo\" mime-type=\"application/x-open-simulation-interface; type=SensorView; version=" +
                                         GetOsiVersionString() + "\"/></Tool>"));

    register_variable(integer("OSMPSensorViewIn.base.hi", &osmp_sensor_view_in_base_hi_)
                          .setCausality(causality_t::INPUT)
                          .setVariability(variability_t::DISCRETE)
                          .addAnnotation("<Tool name=\"net.pmsf.osmp\" xmlns:osmp=\"http://xsd.pmsf.net/OSISensorModelPackaging\"><osmp:osmp-binary-variable "
                                         "name=\"OSMPSensorViewIn\" role=\"base.hi\" mime-type=\"application/x-open-simulation-interface; type=SensorView; version=" +
                                         GetOsiVersionString() + "\"/></Tool>"));

    register_variable(integer("OSMPSensorViewIn.size", &osmp_sensor_view_in_size_)
                          .setCausality(causality_t::INPUT)
                          .setVariability(variability_t::DISCRETE)
                          .addAnnotation("<Tool name=\"net.pmsf.osmp\" xmlns:osmp=\"http://xsd.pmsf.net/OSISensorModelPackaging\"><osmp:osmp-binary-variable "
                                         "name=\"OSMPSensorViewIn\" role=\"size\" mime-type=\"application/x-open-simulation-interface; type=SensorView; version=" +
                                         GetOsiVersionString() + "\"/></Tool>"));

    register_variable(integer("OSMPSensorViewOut.base.lo", &osmp_sensor_view_out_base_lo_)
                          .setCausality(causality_t::OUTPUT)
                          .setVariability(variability_t::DISCRETE)
                          .setInitial(initial_t::EXACT)
                          .addAnnotation("<Tool name=\"net.pmsf.osmp\" xmlns:osmp=\"http://xsd.pmsf.net/OSISensorModelPackaging\"><osmp:osmp-binary-variable "
                                         "name=\"OSMPSensorViewOut\" role=\"base.lo\" mime-type=\"application/x-open-simulation-interface; type=SensorView; version=" +
                                         GetOsiVersionString() + "\"/></Tool>"));

    register_variable(integer("OSMPSensorViewOut.base.hi", &osmp_sensor_view_out_base_hi_)
                          .setCausality(causality_t::OUTPUT)
                          .setVariability(variability_t::DISCRETE)
                          .setInitial(initial_t::EXACT)
                          .addAnnotation("<Tool name=\"net.pmsf.osmp\" xmlns:osmp=\"http://xsd.pmsf.net/OSISensorModelPackaging\"><osmp:osmp-binary-variable "
                                         "name=\"OSMPSensorViewOut\" role=\"base.hi\" mime-type=\"application/x-open-simulation-interface; type=SensorView; version=" +
                                         GetOsiVersionString() + "\"/></Tool>"));

    register_variable(integer("OSMPSensorViewOut.size", &osmp_sensor_view_out_size_)
                          .setCausality(causality_t::OUTPUT)
                          .setVariability(variability_t::DISCRETE)
                          .setInitial(initial_t::EXACT)
                          .addAnnotation("<Tool name=\"net.pmsf.osmp\" xmlns:osmp=\"http://xsd.pmsf.net/OSISensorModelPackaging\"><osmp:osmp-binary-variable "
                                         "name=\"OSMPSensorViewOut\" role=\"size\" mime-type=\"application/x-open-simulation-interface; type=SensorView; version=" +
                                         GetOsiVersionString() + "\"/></Tool>"));

    // Model Parameter

    register_variable(string("model_reference_map_path", &model_reference_map_path_).setCausality(causality_t::PARAMETER).setVariability(variability_t::FIXED));

    ModelFMU::reset();
  }

  bool do_step(double current_time, double dt) override {
    auto status = false;
    osi3::SensorView sensor_view_in;
    if (!GetFmiSensorViewIn(sensor_view_in)) {
      /* We have no valid input, so no valid output */
      ResetFmiSensorViewOut();
      throw std::runtime_error("Unable to get a valid sensor_view input"); // will be caught and fmi2Error returned
    }

    status=model_.Step(sensor_view_in);
    if (status) {
      SetFmiSensorViewOut(sensor_view_in);
    }

    return status;
  }

  void exit_initialisation_mode() override {
   model_.Init(model_reference_map_path_);
  }

  void reset() override {
    model_reference_map_path_ = "";
  }

  bool GetFmiSensorViewIn(osi3::SensorView& sensor_view_in) const {
    if (osmp_sensor_view_in_size_ > 0) {
      void* buffer = DecodeIntegerToPointer(osmp_sensor_view_in_base_hi_, osmp_sensor_view_in_base_lo_);
      sensor_view_in.ParseFromArray(buffer, osmp_sensor_view_in_size_);
      return true;
    }
    return false;
  }

  void SetFmiSensorViewOut(const osi3::SensorView& sensor_view_out) {
    sensor_view_out.SerializeToString(current_output_buffer_.get());
    EncodePointerToInteger(current_output_buffer_->data(), osmp_sensor_view_out_base_hi_, osmp_sensor_view_out_base_lo_);
    osmp_sensor_view_out_size_ = static_cast<fmi2Integer>(current_output_buffer_->length());
    std::swap(current_output_buffer_, last_output_buffer_);
  }

  void ResetFmiSensorViewOut() {
    osmp_sensor_view_out_size_ = 0;
    osmp_sensor_view_out_base_hi_ = 0;
    osmp_sensor_view_out_base_lo_ = 0;
  }

private:
  Model model_;

  std::unique_ptr<std::string> current_output_buffer_;
  std::unique_ptr<std::string> last_output_buffer_;

  int osmp_sensor_view_in_base_lo_ = 0;                 // OSMP Sensor View In Base Lo
  int osmp_sensor_view_in_base_hi_ = 0;                 // OSMP Sensor View In Base Hi
  int osmp_sensor_view_in_size_ = 0;                    // OSMP Sensor View In Size
  int osmp_sensor_view_out_base_lo_ = 0;                // OSMP Sensor View Out Base Lo
  int osmp_sensor_view_out_base_hi_ = 0;                // OSMP Sensor View Out Base Hi
  int osmp_sensor_view_out_size_ = 0;                   // OSMP Sensor View Out Size

  std::string model_reference_map_path_;            // Absolute path to model reference mapping json file
};

model_info fmu4cpp::get_model_info() {
  model_info info;
  info.modelName = CMAKE_PROJECT_NAME;
  info.description = "FMU to set or change model_reference fields in an ASAM OSI SensorView message";
  info.modelIdentifier = CMAKE_PROJECT_NAME;
  info.version = MODEL_VERSION;
  info.author = "Persival GmbH";
  info.vendorAnnotations = {"<Tool name=\"net.pmsf.osmp\" xmlns:osmp=\"http://xsd.pmsf.net/OSISensorModelPackaging\"><osmp:osmp version=\"1.0.0\" osi-version=\"" + GetOsiVersionString() + // NOLINT
                            "\"/></Tool>"};
  return info;
}

std::unique_ptr<fmu_base> fmu4cpp::createInstance(const std::string& instance_name, const std::filesystem::path& fmu_resource_location) {
  return std::make_unique<ModelFMU>(instance_name, fmu_resource_location);
}
