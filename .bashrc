
# Use gcc 7; latest gcc 8 has internal compiler error
export CC=/usr/bin/gcc-7
export CXX=/usr/bin/g++-7
export CBAG_PYTHON=$(which python)
export CBAG_PYTEST=$(which pytest)

export OA_LINK_DIR=/home/erichang/programs/oa_dist/lib/linux_rhel50_gcc44x_64/opt
export OA_INCLUDE_DIR=/home/erichang/programs/oa_dist/include
