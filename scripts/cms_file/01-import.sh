#/bin/bash
set -e

# ulimit
ulimit -s unlimited

# for kotlin
export SDKMAN_DIR="$HOME/.sdkman"
[[ -s "$HOME/.sdkman/bin/sdkman-init.sh" ]] && source "$HOME/.sdkman/bin/sdkman-init.sh"

# compile special checker
{{shell_compile_checker}}

cmsImportContest --loader italy_yaml --import-task contest

# run generator and pack tests
{{shell_generate_tests}}

# update contest languages
{{shell_update_langs}}

# add test cases
{{shell_add_testcases}}

# update scores
{{shell_update_scores}}

# update two steps
{{shell_update_two_steps}}

# update formats for output only
{{shell_update_submission_format}}
