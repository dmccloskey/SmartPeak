# --------------------------------------------------------------------------
#   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
# --------------------------------------------------------------------------
# Copyright The SmartPeak Team -- Novo Nordisk Foundation 
# Center for Biosustainability, Technical University of Denmark 2018-2022.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
# INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# --------------------------------------------------------------------------
# $Maintainer: Douglas McCloskey, Ahmed Khalil $
# $Authors: Douglas McCloskey, Ahmed Khalil $
# --------------------------------------------------------------------------

## Windows installer
## With VS2019 the architecture HAS TO BE specified with the "–A" option or CMAKE_GENERATOR_PLATFORM var.
## Therefore the legacy way of adding a suffix to the Generator is not valid anymore.
## Read value of CMAKE_VS_PLATFORM_NAME instead
if (CMAKE_VS_PLATFORM_NAME MATCHES ".*Win32.*" OR CMAKE_GENERATOR MATCHES ".*Win32.*")
  set(PLATFORM "32")
  set(ARCH "x86")
else()
  set(PLATFORM "64")
  set(ARCH "x64")
endif()
set(VC_REDIST_EXE "vcredist_${ARCH}.exe")


## Find redistributable to be installed by NSIS
if (NOT VC_REDIST_PATH)
	string(REGEX REPLACE ".*Visual Studio ([1-9][1-9]) .*" "\\1" SMARTPEAK_MSVC_VERSION_STRING "${CMAKE_GENERATOR}")
	if("${SMARTPEAK_MSVC_VERSION_STRING}" GREATER "14")  
	  ## We have to glob recurse in the parent folder because there is a version number in the end.
	  ## Unfortunately in my case the default version (latest) does not include the redist?!
	  ## TODO Not sure if this environment variable always exists. In the VS command line it should! Fallback vswhere or VCINSTALLDIR/Redist/MSVC?
	  get_filename_component(VC_ROOT_PATH "$ENV{VCToolsRedistDir}.." ABSOLUTE)
	  file(GLOB_RECURSE VC_REDIST_ABS_PATH "${VC_ROOT_PATH}/${VC_REDIST_EXE}")
	  ## TODO pick the latest of the found redists
	  get_filename_component(VC_REDIST_PATH "${VC_REDIST_ABS_PATH}" DIRECTORY)
	elseif(SMARTPEAK_MSVC_VERSION_STRING GREATER "10")
	  set(VC_REDIST_PATH "$ENV{VCINSTALLDIR}redist/1033")  
	else()
	  message(FATAL_ERROR "Variable VC_REDIST_PATH missing."
	  "Before Visual Studio 2012 you have to provide the path"
	  "to the redistributable package of the VS you are using on your own.")
	endif()
endif()

## System runtime libraries for users that do not have the needed VC libraries installed
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)
install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
       DESTINATION bin
       COMPONENT library)

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Win${PLATFORM}")
# set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/cmake/Windows/SmartPeak.ico")

## Create own target because you cannot "depend" on the internal target 'package'
add_custom_target(dist
  COMMAND cpack -G ${CPACK_GENERATOR}
  COMMENT "Building ${CPACK_GENERATOR} package"
)

## TODO maybe find signtool and maybe check existence of ID in the beginning.
## ID needs to be installed beforehand. Rightclick a p12 file that has a cert for codesigning.
if (DEFINED SIGNING_IDENTITY AND NOT "${SIGNING_IDENTITY}" STREQUAL "") 
	add_custom_target(signed_dist
	  COMMAND signtool sign /v /n "${SIGNING_IDENTITY}" /t http://timestamp.digicert.com ${CPACK_PACKAGE_FILE_NAME}.exe
	  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
	  COMMENT "Signing ${CPACK_PACKAGE_FILE_NAME}.exe with '${SIGNING_IDENTITY}'"
	  DEPENDS dist
	)
endif()

## For now we fully rely only on our NSIS template. Later we could use
## the following to let CMake generate snippets for the NSIS script
## Plus an additional entry in the nsis template (see CPack-NSIS docu)

set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "SmartPeakGUI.exe")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\SmartPeakGUI.exe")

set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/cmake/SmartPeak.ico")
# set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/cmake/SmartPeak.ico")

# set(CPACK_NSIS_HELP_LINK "https://www.SmartPeak.com/getting-started")
# set(CPACK_NSIS_URL_INFO_ABOUT "https://www.SmartPeak.com")
# set(CPACK_NSIS_CONTACT "smartpeak-general@lists.sourceforge.net")
# set(CPACK_NSIS_MENU_LINKS
#     "https://www.SmartPeak.com" "SmartPeak Web Site")

# Helper macro to set shortcuts for the application.
# Creates link with name linkName to the application both on desktop and start menu.
# Source: https://crascit.com/2015/08/07/cmake_cpack_nsis_shortcuts_with_parameters/
macro(prepare_nsis_link linkName appName params)
	#prepare start menu links
	LIST(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA "  CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${linkName}.lnk' '$INSTDIR\\\\bin\\\\${appName}.exe' '${params}'")
	LIST(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA "  Delete '$SMPROGRAMS\\\\$START_MENU\\\\${linkName}.lnk'")
	#prepare desktop links
	LIST(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA  "  CreateShortCut '$DESKTOP\\\\${linkName}.lnk' '$INSTDIR\\\\bin\\\\${appName}.exe' '${params}'")
	LIST(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA  "  Delete '$DESKTOP\\\\${linkName}.lnk'")
endmacro()

prepare_nsis_link(${CPACK_PACKAGE_FILE_NAME} "SmartPeakGUI" " ")

# Replace semicolons with new lines:
string (REPLACE ";" "\n" CPACK_NSIS_CREATE_ICONS_EXTRA "${CPACK_NSIS_CREATE_ICONS_EXTRA}")
string (REPLACE ";" "\n" CPACK_NSIS_DELETE_ICONS_EXTRA "${CPACK_NSIS_DELETE_ICONS_EXTRA}")
