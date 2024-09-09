# ## This file should be placed in the root directory of your project.
# ## Then modify the CMakeLists.txt file in the root directory of your
# ## project to incorporate the testing dashboard.
# ## # The following are required to uses Dart and the Cdash dashboard
# ##   enable_testing()
# ##   include(CTest)
# set(CTEST_PROJECT_NAME "snclient")
# set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

# set(CTEST_DROP_METHOD "http")
# set(CTEST_DROP_SITE "open.cdash.org")
# set(CTEST_DROP_LOCATION "/submit.php?project=snclient")
# set(CTEST_DROP_SITE_CDASH TRUE)
# #set(CTEST_USE_LAUNCHERS 1)
# set(SCRIPT_MODE "Experimental") # "Experimental", "Continuous", "Nightly"
# set(CTEST_DASHBOARD_ROOT "$ENV{HOME}/Dashboards/${SCRIPT_MODE}")
