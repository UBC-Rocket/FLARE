#include "sensor_collection.h"

SensorCollectionPtr getSensors() {
    std::unique_ptr<SensorCollection> tmp(new SensorCollection);
    return std::move(tmp);
}
