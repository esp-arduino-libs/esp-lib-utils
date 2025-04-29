set(SRC_DIR ./src)
file(GLOB_RECURSE SRCS_C ${SRC_DIR}/*.c)
file(GLOB_RECURSE SRCS_CPP ${SRC_DIR}/*.cpp)

idf_component_register(
    SRCS ${SRCS_C} ${SRCS_CPP}
    INCLUDE_DIRS ${SRC_DIR}
)

target_compile_options(${COMPONENT_LIB}
    PUBLIC
        -Wno-missing-field-initializers
    PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++20>
)
