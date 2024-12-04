# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/bavendanom/esp/v5.3.1/esp-idf/components/bootloader/subproject"
  "/home/bavendanom/Documentos/9_Semestre/Sistemas_en_tiempo_real/sistemas_en_tiempo_real/toggle/build/bootloader"
  "/home/bavendanom/Documentos/9_Semestre/Sistemas_en_tiempo_real/sistemas_en_tiempo_real/toggle/build/bootloader-prefix"
  "/home/bavendanom/Documentos/9_Semestre/Sistemas_en_tiempo_real/sistemas_en_tiempo_real/toggle/build/bootloader-prefix/tmp"
  "/home/bavendanom/Documentos/9_Semestre/Sistemas_en_tiempo_real/sistemas_en_tiempo_real/toggle/build/bootloader-prefix/src/bootloader-stamp"
  "/home/bavendanom/Documentos/9_Semestre/Sistemas_en_tiempo_real/sistemas_en_tiempo_real/toggle/build/bootloader-prefix/src"
  "/home/bavendanom/Documentos/9_Semestre/Sistemas_en_tiempo_real/sistemas_en_tiempo_real/toggle/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/bavendanom/Documentos/9_Semestre/Sistemas_en_tiempo_real/sistemas_en_tiempo_real/toggle/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/bavendanom/Documentos/9_Semestre/Sistemas_en_tiempo_real/sistemas_en_tiempo_real/toggle/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
