import argparse
import os
import sys
from typing import NamedTuple

ResourceManager = NamedTuple("ResourceManager", [("type", str), ("name", str)])

def type_and_name(double_points_str):
    sp = double_points_str.split(':')
    if len(sp) != 2:
        raise argparse.ArgumentTypeError("'{}' is not a valid resource manager format. Format should be 'resource_manager_type:resource_manager_name'")
    return ResourceManager(*sp)

def create_msg_and_srv_files(package_msg_dir, dependencies):
    msgs_files = []
    srvs_files = []
    fsm_srv_file_name = os.path.join(package_msg_dir, "srv", "MetaStateMachine.srv")
    fsm_srv_file = "resource_synchronizer_msgs/MetaStateMachineHeader header\n"
    for dep in dependencies:
        msg_file_name = "SubStateMachine_{type}".format(type=dep.type)
        subsm_msg_name = os.path.join(package_msg_dir, "msg", "{msg_file_name}.msg".format(msg_file_name=msg_file_name))
        with open(subsm_msg_name, "w") as f:
            f.write("resource_synchronizer_msgs/SubStateMachineHeader header\n"
                    "{type}/StateMachine state_machine_{name}".format(type=dep.type, name=dep.name))
        msgs_files.append("{msg_file_name}.msg".format(msg_file_name=msg_file_name))
        fsm_srv_file += ("{msg_file_name} state_machine_{name}\n".format(msg_file_name=msg_file_name, name=dep.name))
    fsm_srv_file += "---\nuint32 id\nstring error"
    with open(fsm_srv_file_name, "w") as f:
        f.write(fsm_srv_file)
    srvs_files.append("MetaStateMachine.srv")

    return msgs_files, srvs_files

def create_catkin_files_msgs(package_name, package_dir, dependencies, msgs_to_gen, srvs_to_gen):
    unique_dep_type = set([d.type for d in dependencies])
    # CmakeLists.txt for msgs package
    with open(os.path.join(package_dir, "CMakeLists.txt"),"w") as fcmake:
        fcmake.write("cmake_minimum_required(VERSION 2.8.3)\n"
                     "project({package_name})\n".format(package_name=package_name)+
                     "\n"
                     "find_package(catkin REQUIRED\n"
                     "roscpp\n"
                     "message_generation\n"
                     "resource_management_msgs\n"
                     "resource_synchronizer_msgs\n"
                     +"\n".join(unique_dep_type)+"\n"
                     ")\n"
                     "\n"
                     "## Generate messages in the \'msg\' folder\n"
                     "add_message_files(\n"
                     "FILES\n"
                     +"\n".join(msgs_to_gen)+"\n"
                     ")\n"
                     "   \n"
                     "## Generate services in the \'srv\' folder\n"
                     "add_service_files(\n"
                     "FILES\n"
                     +"\n".join(srvs_to_gen)+"\n"
                     ")\n"
                     "   \n"
                     "generate_messages(DEPENDENCIES resource_management_msgs resource_synchronizer_msgs {catkin_msgs_deps})\n"
                     "\n"
                     "catkin_package(CATKIN_DEPENDS message_runtime {catkin_msgs_deps})".format(catkin_msgs_deps=" ".join(unique_dep_type)))

    pack_deps = ['<depend>'+d+'</depend>\n' for d in unique_dep_type]
    # package.xml
    with open(os.path.join(package_dir, "package.xml"),"w") as fpackage:
        fpackage.write('<package format="2">\n'
                       '<name>{package_name}</name>\n'.format(package_name=package_name)+
                       '<version>0.0.0</version>\n'
                       '<description>\n'
                       '{0} package autogenerated from resource_synchronizer messages generator.\n'
                       '</description>\n'
                       '<maintainer email="todo@todo.org">TBD</maintainer>\n'
                       '<license>TBD</license>\n'
                       '<depend>roscpp</depend>\n'
                       '<depend>resource_management_msgs</depend>\n'
                       '<depend>resource_synchronizer_msgs</depend>\n'
                       +"".join(pack_deps)+
                       '<buildtool_depend>catkin</buildtool_depend>\n'
                       '<build_depend>message_generation</build_depend>\n'
                       '<exec_depend>message_runtime</exec_depend>\n'
                       '</package>\n')

def create_catkin_files_src(package_name, package_dir, package_msgs_name):
    # CmakeLists.txt for src package
    with open(os.path.join(package_dir, "CMakeLists.txt"),"w") as fcmake:
        fcmake.write("cmake_minimum_required(VERSION 2.8.3)\n"
                     "project({package_name})\n".format(package_name=package_name)+
                     "\n"
                     "find_package(catkin REQUIRED\n"
                     "roscpp\n"
                     "message_generation\n"
                     "resource_management_msgs\n"
                     "resource_synchronizer_msgs\n"
                     "{package_msgs_name}\n"
                     ")\n"
                     "\n"
                     "catkin_package(CATKIN_DEPENDS message_runtime {package_msgs_name})".format(package_msgs_name=package_msgs_name))

    # package.xml
    with open(os.path.join(package_dir, "package.xml"),"w") as fpackage:
        fpackage.write('<package format="2">\n'
                       '<name>{package_name}</name>\n'.format(package_name=package_name)+
                       '<version>0.0.0</version>\n'
                       '<description>\n'
                       '{0} package autogenerated from resource_synchronizer messages generator.\n'
                       '</description>\n'
                       '<maintainer email="todo@todo.org">TBD</maintainer>\n'
                       '<license>TBD</license>\n'
                       '<depend>roscpp</depend>\n'
                       '<depend>resource_management_msgs</depend>\n'
                       '<depend>resource_synchronizer_msgs</depend>\n'
                       '<depend>{package_msgs_name}</depend>\n'.format(package_name, package_msgs_name=package_msgs_name)+
                       '<buildtool_depend>catkin</buildtool_depend>\n'
                       '<build_depend>message_generation</build_depend>\n'
                       '<exec_depend>message_runtime</exec_depend>\n'
                       '</package>\n')


if __name__ == "__main__":
    argparser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    argparser.add_argument(dest='package_name', help="The name of the created synchronizer package")
    argparser.add_argument(dest='dependencies', nargs="+",
                           help="The resource managers package name to synchronize (must be in the same workspace)",
                           type=type_and_name)
    args = argparser.parse_args()

    package_msg_name = args.package_name+"_msgs"
    package_msg_dir = os.path.join(args.package_name, package_msg_name)
    package_src_dir = os.path.join(args.package_name, args.package_name)
    generator_dir=os.path.dirname(sys.argv[0])
    os.makedirs(os.path.join(package_src_dir, "src"))
    os.makedirs(os.path.join(package_msg_dir, "msg"))
    os.makedirs(os.path.join(package_msg_dir, "srv"))

    msgs_dep = [ResourceManager(d.type + "_msgs", d.name) for d in args.dependencies]

    msgs_to_gen, srvs_to_gen = create_msg_and_srv_files(package_msg_dir, msgs_dep)

    create_catkin_files_msgs(package_msg_name, package_msg_dir, msgs_dep, msgs_to_gen, srvs_to_gen)

    create_catkin_files_src(args.package_name, package_src_dir, package_msg_name)