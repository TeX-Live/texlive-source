project(profile-graphite)

add_executable(profile-graphite
	GrUtfTextSrc.cpp
	ProfileHarness.cpp
	)

target_link_libraries(profile-graphite graphite)
