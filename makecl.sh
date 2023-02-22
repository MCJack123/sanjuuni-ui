#!/bin/sh
printf '// Generated automatically; do not edit!\n#include <string>\nnamespace OpenCL {std::string get_opencl_c_code() { return ' > $2
sed -n -e '/#ifndef OPENCV/{:a; N; /#endif/!ba; d}; s/"/\\"/g; s/^/"/g; s/$/\\n"/g; p' $1 >> $2
echo ';}}' >> $2
