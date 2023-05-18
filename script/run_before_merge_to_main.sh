python ./script/thirdparty/iwyu_tool.py -v -p ./build > ./build/iwyu.out -- -Xiwyu --cxx17ns -Xiwyu --no_fwd_decls
python ./script/thirdparty/fix_includes.py --nocomments --nosafe_headers < ./build/iwyu.out
python ./script/thirdparty/run-clang-format.py -ir include src