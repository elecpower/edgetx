
set(wizards_NAMES
  modelwizarddata
  modelwizarddialog
  sdcardwizarddialog
)

foreach(name ${wizards_NAMES})
  set(wizards_SRCS ${wizards_SRCS} ${name}.cpp)
  set(wizards_HDRS ${wizards_HDRS} ${name}.h)
endforeach()

qt5_wrap_cpp(wizards_SRCS ${wizards_HDRS})

add_library(wizards ${wizards_SRCS} ${wizards_HDRS})
target_link_libraries(wizards PRIVATE ${CPN_COMMON_LIB})
