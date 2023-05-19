python ./script/thirdparty/iwyu_tool.py -v -p ./build -- -Xiwyu --cxx17ns -Xiwyu --no_fwd_decls >./build/iwyu.out
python ./script/thirdparty/fix_includes.py --nocomments --nosafe_headers <./build/iwyu.out
python ./script/replace_quotes_in_thirdparty_includes.py
python ./script/thirdparty/run-clang-format.py -ir include src
