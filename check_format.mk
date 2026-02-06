CF_SOURCE_DIRS := $(shell find -L $(CF_SRC) $(foreach dir,$(CF_EXCLUDE_DIRS),-path '$(CF_SRC)/$(dir)' -prune -o) -type d -print)

# define the C header files
CF_HEADERS		:= $(wildcard $(patsubst %,%/*.h, $(CF_SOURCE_DIRS)))
# define the C source files
CF_SOURCES		:= $(wildcard $(patsubst %,%/*.c, $(CF_SOURCE_DIRS)))
# define the CPP source files
CF_SOURCES_CXX		:= $(wildcard $(patsubst %,%/*.cpp, $(CF_SOURCE_DIRS)))

apply_format:
	@clang-format --style=file:$(ROOT_DIR)/.clang-format $(CF_SOURCES) $(CF_SOURCES_CXX) $(CF_HEADERS) -i

check_format:
	@echo "----------------------- check_format"
	@clang-format --style=file:$(ROOT_DIR)/.clang-format $(CF_SOURCES) $(CF_SOURCES_CXX) $(CF_HEADERS) --Werror --dry-run

code_check:
	@echo "----------------------- Static analysis"
	@CodeChecker check --build "make CCACHE= -j$$(nproc) clean && make CCACHE= -j$$(nproc) build_tests" \
		--analyzers cppcheck clangsa clang-tidy --ctu --disable-all \
		--enable severity:CRITICAL --enable severity:HIGH --enable severity:MEDIUM --no-missing-checker-error \
		--no-missing-checker-error \
		--disable cppcheck-uninitMemberVar \
		--disable android-cloexec-accept \
		--disable clang-diagnostic-padded \
		--disable clang-diagnostic-c++98-compat-pedantic \
		--disable cppcheck-preprocessorErrorDirective \
		--skip $(ROOT_DIR)/CodeChecker.skipfile \
		--output $(ROOT_DIR)/build/CodeChecker/check_static/reports --clean	|| true
	CodeChecker parse --export html --output build/CodeChecker/reports/check_static build/CodeChecker/check_static/reports > /dev/null 2>&1 || true