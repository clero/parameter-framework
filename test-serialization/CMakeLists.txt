
add_executable(testSeria Serializer.cpp)

include_directories(
    "${PROJECT_SOURCE_DIR}/xmlserializer"
    "${PROJECT_SOURCE_DIR}/utility"
    "${PROJECT_SOURCE_DIR}/test-serialization")

target_link_libraries(testSeria xmlserializer)
