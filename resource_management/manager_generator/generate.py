#!/usr/bin/python3

import argparse
import os
import sys
import string
import collections

Settings = collections.namedtuple('Settings',['project_name', 'class_name', 'message_types', 'message_dependencies', 'reactive_input_names','reactive_input_names_cs'])


def createCatkinFiles(args, settings):
    cmake_msg_deps = ' '.join(settings.message_dependencies)
    # CmakeLists.txt
    fcmake = open(os.path.join(args.package_name, "CMakeLists.txt"),"w")
    fcmake.write('cmake_minimum_required(VERSION 2.8.3)\n'
    'project({0})\n'
    'add_compile_options(-std=c++14 -Wall -pthread)\n'
    'find_package(catkin REQUIRED\n'
    'roscpp\n'
    '{0}_msgs\n'
    'resource_management\n'
    '{catkin_msgs_deps}\n'
    'pluginlib\n'
    ')\n'
    '\n'
    'catkin_package(\n'
    '#INCLUDE_DIRS include\n'
    '#LIBRARIES ${{PROJECT_NAME}}\n'
    'CATKIN_DEPENDS roscpp message_runtime\n'
    '# DEPENDS\n'
    ')\n'
    '\n'
    'include_directories(include ${{catkin_INCLUDE_DIRS}})\n\n'
    'add_executable(${{PROJECT_NAME}} src/main.cpp src/${{PROJECT_NAME}}.cpp src/ArtificialLife.cpp)\n'
    'add_dependencies(${{PROJECT_NAME}} ${{${{PROJECT_NAME}}_EXPORTED_TARGETS}} ${{catkin_EXPORTED_TARGETS}})\n'
    'target_link_libraries(${{PROJECT_NAME}} ${{catkin_LIBRARIES}})\n'
    '\n'
    'add_executable(${{PROJECT_NAME}}_synchronised src/main_synchronised.cpp src/${{PROJECT_NAME}}.cpp src/ArtificialLife.cpp)\n'
    'add_dependencies(${{PROJECT_NAME}}_synchronised ${{${{PROJECT_NAME}}_EXPORTED_TARGETS}} ${{catkin_EXPORTED_TARGETS}})\n'
    'target_link_libraries(${{PROJECT_NAME}}_synchronised ${{catkin_LIBRARIES}})\n'
    '\n'
    'if(CATKIN_ENABLE_TESTING)\n'
    '    find_package(rostest REQUIRED)\n'
    '    add_rostest_gtest(tests_${{PROJECT_NAME}} test/main.test test/test.cpp)\n'
    '    add_dependencies(tests_${{PROJECT_NAME}} ${{${{PROJECT_NAME}}_EXPORTED_TARGETS}} ${{catkin_EXPORTED_TARGETS}})\n'
    '    target_link_libraries(tests_${{PROJECT_NAME}} ${{catkin_LIBRARIES}})\n'
    'endif()\n'
    .format(settings.project_name,catkin_msgs_deps=cmake_msg_deps)
    )
    fcmake.close()

    pack_deps = ['<depend>'+x+'</depend>\n' for x in settings.message_dependencies]
    # package.xml
    fpackage = open(os.path.join(args.package_name, "package.xml"),"w")
    fpackage.write('<package format="2">\n'
    '<name>{0}</name>\n'
    '<version>0.0.0</version>\n'
    '<description>\n'
    '{0} package autogenerated from resource_manager generator.\n'
    '</description>\n'
    '<maintainer email="todo@todo.org">TBD</maintainer>\n'
    '<license>TBD</license>\n'
    '<depend>roscpp</depend>\n'
    '<depend>{0}_msgs</depend>\n'
    '<depend>resource_management</depend>\n'
    '{depends}'
    '<depend>pluginlib</depend>\n'
    '<buildtool_depend>catkin</buildtool_depend>\n'
    '<build_depend>message_generation</build_depend>\n'
    '<exec_depend>message_runtime</exec_depend>\n'
    '</package>\n'.format(settings.project_name,depends=''.join(pack_deps)))
    fpackage.close()

def underscore_to_CamelCase(word):
    return ''.join(x.capitalize() or '_' for x in word.split('_'))

def create_folders(settings):
    os.makedirs(os.path.join(settings.project_name,"src"),exist_ok=True)
    os.makedirs(os.path.join(settings.project_name,"test"),exist_ok=True)
    os.makedirs(os.path.join(settings.project_name,os.path.join("include",settings.project_name)),exist_ok=True)

def substitue_for_loop(tpl,for_var,the_list):
    res=""
    for x in the_list:
        res+=eval('tpl.format({}=x)'.format(for_var))
    return res

def substitue_fmt(tpl,unused0,the_var):
    return tpl.format(the_var)


def configure_template(template_path, output_path,settings):
    f_in = open(template_path,'r')
    tpl = ""
    tpl_inside=""
    for_var=""
    inside=False
    for line in f_in:
        if line.startswith('!!for '):
            inside='for_loop'
            command=line[2:-1]
            for_var=command[command.find(' ')+1:]
            for_var=for_var[0:for_var.find(' ')]
            for_list=command[command.find(' in ')+4:]
            tpl_inside=""
        elif line.startswith('!!fmt '):
            inside='fmt'
            command=line[2:-1]
            for_list=command[command.find(' ')+1:]
            tpl_inside=""
        elif line.startswith('!!end'):
            tpl+=eval('substitue_{}(tpl_inside,for_var,settings.{})'.format(inside,for_list))
            inside=None
        elif inside:
            tpl_inside+=line
        else:
            tpl+=line
    f_in.close()

    # write main cpp file

    fo = open(output_path,"w+")
    fo.write(string.Template(tpl).substitute(**settings._asdict()))
    fo.close()


def main():

    parser = argparse.ArgumentParser(description="generate a resource manager node")
    parser.add_argument("--package-name",metavar="PKG", type=str, required=True,
            help="name of the project/package")
    parser.add_argument("--target-types", type=str, metavar="name,ros_data_type,cpp_type", nargs='+', help="", required=True)
    parser.add_argument("--reactive-topics", type=str, metavar="topic_name", nargs='+', help="")

    args=parser.parse_args()
    generator_dir=os.path.dirname(sys.argv[0])
    in_msg_dir = os.path.join(generator_dir,"cmake","gen","msg")

    message_types = [x.split(',') for x in args.target_types]
    project_name=args.package_name
    class_name=underscore_to_CamelCase(project_name)

    reactive_input_names=args.reactive_topics
    reactive_input_names_cs = ', '.join(['"'+x+'"' for x in reactive_input_names])

    # try to infer dependencies for the message types from the input messages
    message_dependencies=set()
    for data_type in message_types :
        if '/' in data_type[1]:
            message_dependencies.add(data_type[1].split('/',1)[0])

    settings = Settings(project_name, class_name, message_types, message_dependencies, reactive_input_names,reactive_input_names_cs)

    #package architecture
    create_folders(settings)

    createCatkinFiles(args, settings)

    # led_manager.cpp
    configure_template(os.path.join(generator_dir,'template_manager.cpp'),os.path.join(project_name, "src", project_name +".cpp"),settings)
    # led_manager.h
    configure_template(os.path.join(generator_dir,'template_manager.h'),os.path.join(project_name, "include", project_name, project_name +".h"),settings)

    # ArtificialLife.cpp
    configure_template(os.path.join(generator_dir,'template_artificialLife.cpp'),os.path.join(project_name, "src", "ArtificialLife.cpp"),settings)
    # ArtificialLife.h
    configure_template(os.path.join(generator_dir,'template_artificialLife.h'),os.path.join(project_name, "include", project_name, "ArtificialLife.h"),settings)

    # test.cpp
    configure_template(os.path.join(generator_dir,'test','template_test.cpp'),os.path.join(project_name, "test", "test.cpp"),settings)
    # main.test
    configure_template(os.path.join(generator_dir,'test','template_main.test'),os.path.join(project_name, "test", "main.test"),settings)

    # main.cpp
    configure_template(os.path.join(generator_dir,'template_main.cpp'),os.path.join(project_name, "src", "main.cpp"),settings)
    # main_synchronised.cpp
    configure_template(os.path.join(generator_dir,'template_main_synchronised.cpp'),os.path.join(project_name, "src", "main_synchronised.cpp"),settings)

if __name__ == '__main__':
    main()