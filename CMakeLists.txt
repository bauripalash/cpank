cmake_minimum_required(VERSION 3.0)
project(pankti
	VERSION 0.3.0
	DESCRIPTION "The Pankti Programming Language"
	LANGUAGES C)

set(CPANK_CORE_DIR "cpank")
set(CPANK_STDLIB_DIR "stdlib")
set(CPANK_EXT_DIR "ext")
set(CPANK_GUI_DIR "gui")

#"${CPANK_CORE_DIR}/cpank/main.c")
set(CPANK_CORE_SRC "${CPANK_CORE_DIR}/api.c"
					"${CPANK_CORE_DIR}/bn.c"
					"${CPANK_CORE_DIR}/builtins.c"
					"${CPANK_CORE_DIR}/compiler.c"
					"${CPANK_CORE_DIR}/debug.c"
					"${CPANK_CORE_DIR}/errmsg.c" 
					"${CPANK_CORE_DIR}/htable.c"
					"${CPANK_CORE_DIR}/instruction.c"
					"${CPANK_CORE_DIR}/lexer.c"
					"${CPANK_CORE_DIR}/mem.c"
					"${CPANK_CORE_DIR}/obj.c"
					"${CPANK_CORE_DIR}/openfile.c"
					"${CPANK_CORE_DIR}/runfile.c"
					"${CPANK_CORE_DIR}/utils.c"
					"${CPANK_CORE_DIR}/value.c"
					"${CPANK_CORE_DIR}/vm.c"
)

set(CPANK_STDLIB_SRC "${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/array.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/big.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/bnmath.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/bnos.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/common.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/file.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/map.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/math.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/os.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/stdlib.c" 
					"${CPANK_CORE_DIR}/${CPANK_STDLIB_DIR}/string.c" 
)

set(CPANK_BAURINUM_SRC 	"${CPANK_CORE_DIR}/${CPANK_EXT_DIR}/baurinum/absneg.c" 
						"${CPANK_CORE_DIR}/${CPANK_EXT_DIR}/baurinum/addsub.c" 
						"${CPANK_CORE_DIR}/${CPANK_EXT_DIR}/baurinum/baurinum.c" 
						"${CPANK_CORE_DIR}/${CPANK_EXT_DIR}/baurinum/compare.c" 
						"${CPANK_CORE_DIR}/${CPANK_EXT_DIR}/baurinum/getter.c" 
						"${CPANK_CORE_DIR}/${CPANK_EXT_DIR}/baurinum/setter.c" 
						"${CPANK_CORE_DIR}/${CPANK_EXT_DIR}/baurinum/utility.c"
)

set(CPANK_GUI_SRC "${CPANK_GUI_DIR}/gui.c")

add_executable(pankti "${CPANK_CORE_DIR}/main.c")
target_sources(pankti PRIVATE ${CPANK_CORE_SRC})
target_sources(pankti PRIVATE ${CPANK_STDLIB_SRC})
target_sources(pankti PRIVATE ${CPANK_BAURINUM_SRC})
target_link_libraries(pankti PUBLIC m)

add_executable(panktiw ${CPANK_GUI_SRC})
target_sources(panktiw PRIVATE ${CPANK_CORE_SRC})
target_sources(panktiw PRIVATE ${CPANK_STDLIB_SRC})
target_sources(panktiw PRIVATE ${CPANK_BAURINUM_SRC})
target_link_libraries(panktiw PUBLIC m)
add_subdirectory(cpank/ext/iup)
target_link_libraries(panktiw PRIVATE cpankiup)

# only use cmake, if you really "have to"
