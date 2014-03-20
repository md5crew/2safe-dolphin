
set(qjson_MOC_HDRS
  parserrunnable.h
  serializerrunnable.h
)

qt4_wrap_cpp(qjson_MOC_SRCS ${qjson_MOC_HDRS})

set (qjson_SRCS parser.cpp qobjecthelper.cpp json_scanner.cpp json_parser.cc parserrunnable.cpp serializer.cpp serializerrunnable.cpp)

set(CMAKE_CXX_FLAGS ${CMAKE_CSS_FLAGS} "-fPIC") 

add_library (qjson STATIC ${qjson_SRCS} ${qjson_MOC_SRCS})
target_link_libraries( qjson ${QT_LIBRARIES})

