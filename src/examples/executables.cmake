set(pipeline_executables_list
	FIAMS_FullScan_Unknown_test
	GCMS_FullScan_Unknown_test
	GCMS_SIM_Unknown_test
	HPLC_UV_Standards_test
	HPLC_UV_Unknown_test
	LCMS_MRM_QCs_test
	LCMS_MRM_Standards_test
	LCMS_MRM_Unknown_test
)

set(interactive_executables_list
  #CLI // TODO: refactor according to AUT-322
	SmartPeakCLI
	SmartPeakGUI
	SmartPeakServer
)

### collect example executables
set(EXAMPLE_executables
	${pipeline_executables_list}
	${interactive_executables_list}
)
