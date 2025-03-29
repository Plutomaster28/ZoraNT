::------------------------------------------------------------------------------
:: Copyright (C) Intel Corporation
::
:: SPDX-License-Identifier: MIT
::------------------------------------------------------------------------------
:: Configure environment variables
@echo off
FOR /D %%i IN ("%~dp0\..\..") DO (
  set VPL_PREFIX=%%~fi
)

IF DEFINED INCLUDE (
  set "INCLUDE=%VPL_PREFIX%\include;%INCLUDE%"
) ELSE (
  set "INCLUDE=%VPL_PREFIX%\include"
)

IF DEFINED LIB (
  set "LIB=%VPL_PREFIX%\lib;%LIB%"
) ELSE (
  set "LIB=%VPL_PREFIX%\lib"
)

IF DEFINED PATH (
  set "PATH=%VPL_PREFIX%\bin;%PATH%"
) ELSE (
  set "PATH=%VPL_PREFIX%\bin"
)

IF DEFINED CMAKE_PREFIX_PATH (
  set "CMAKE_PREFIX_PATH=%VPL_PREFIX%;%CMAKE_PREFIX_PATH%"
) ELSE (
  set "CMAKE_PREFIX_PATH=%VPL_PREFIX%"
)

IF DEFINED PKG_CONFIG_PATH (
  set "PKG_CONFIG_PATH=%VPL_PREFIX%\;%PKG_CONFIG_PATH%"
) ELSE (
  set "PKG_CONFIG_PATH=%VPL_PREFIX%\"
)
set "VPL_EXAMPLES_PATH=%VPL_PREFIX%\share\vpl\examples"
set VPL_PREFIX=
