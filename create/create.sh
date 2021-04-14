#!/bin/sh

if [ $# -ne "1" ]; then
    echo "Usage: Please input module name"
    exit
fi

module_name=$1
module_dir=../module/$module_name
typeset -u upper_name
upper_name=$module_name
class_name=
array=(${module_name//_/ })
for var in ${array[@]}
do
    typeset -u tmp
    tmp=${var:0:1}
    class_name=$class_name$tmp${var:1}
done

if [ -d $module_dir ]; then
    echo "$module_dir 已存在, 请更换模块名称或者删除该目录"
    exit
else
    mkdir $module_dir
fi

# 生成xxx_config.h文件
file=$module_dir/$module_name"_config.h"
sed 's/$1/'$module_name'/g' config.h > $file
sed -i 's/$2/'$upper_name'/g' $file

# 生成xxx_module.h文件
file=$module_dir/$module_name"_module.h"
sed 's/$1/'$module_name'/g' module.h > $file
sed -i 's/$2/'$upper_name'/g' $file
sed -i 's/$3/'$class_name'/g' $file

# 生成xxx_module.cpp文件
file=$module_dir/$module_name"_module.cpp"
sed 's/$1/'$module_name'/g' module.cpp > $file
sed -i 's/$2/'$upper_name'/g' $file
sed -i 's/$3/'$class_name'/g' $file

# 生成xxx_servlet.h文件
file=$module_dir/$module_name"_servlet.h"
sed 's/$1/'$module_name'/g' servlet.h > $file
sed -i 's/$2/'$upper_name'/g' $file
sed -i 's/$3/'$class_name'/g' $file

# 生成xxx_servlet.cpp文件
file=$module_dir/$module_name"_servlet.cpp"
sed 's/$1/'$module_name'/g' servlet.cpp > $file
sed -i 's/$2/'$upper_name'/g' $file
sed -i 's/$3/'$class_name'/g' $file

# 生成Makefile文件
file=$module_dir/Makefile
sed 's/$1/'$module_name'/g' Makefile.txt > $file

# 添加编译语句
sed -i '/default:/a \\t$(MAKE) -f module/'$module_name'/Makefile' ../module/Makefile
sed -i '/rm -rf $(MODULE_PATH)\//a \\t$(MAKE) clean -f module/'$module_name'/Makefile' ../module/Makefile
