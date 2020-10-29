/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "cyber/mainboard/module_argument.h"

#include <getopt.h>
#include <libgen.h>

using apollo::cyber::common::GlobalData;

namespace apollo
{
  namespace cyber
  {
    namespace mainboard
    {

      ModuleArgument::ModuleArgument() {}

      ModuleArgument::~ModuleArgument() {}

      void ModuleArgument::DisplayUsage()
      {
        AINFO << "Usage: \n    " << binary_name_ << " [OPTION]...\n"
              << "Description: \n"
              << "    -h, --help : help infomation \n"
              << "    -d, --dag_conf=CONFIG_FILE : module dag config file\n"
              << "    -p, --process_group=process_group: the process "
                 "namespace for running this module, default in manager process\n"
              << "    -s, --sched_name=sched_name: sched policy "
                 "conf for hole process, sched_name should be conf in cyber.pb.conf\n"
              << "Example:\n"
              << "    " << binary_name_ << " -h\n"
              << "    " << binary_name_ << " -d dag_conf_file1 -d dag_conf_file2 "
              << "-p process_group -s sched_name\n";
      }


// 解析参数是在"ModuleArgument"类中实现的，主要是解析加载DAG文件时候带的参数
      void ModuleArgument::ParseArgument(const int argc, char *const argv[])
      {
        // 二进制模块名称
        binary_name_ = std::string(basename(argv[0]));
        // 解析输入参数
        GetOptions(argc, argv);

// 如果没有process_group_和sched_name_，则赋值为默认值
        if (process_group_.empty())
        {
          process_group_ = DEFAULT_process_group_;
        }

        if (sched_name_.empty())
        {
          sched_name_ = DEFAULT_sched_name_;
        }

// 如果有，则设置对应的参数
        // 设置执行组，类似linux的cgroups
        GlobalData::Instance()->SetProcessGroup(process_group_);
        // 设置调度器名称
        GlobalData::Instance()->SetSchedName(sched_name_);
        // 打印模块的信息：名称，组，DAG配置
        AINFO << "binary_name_ is " << binary_name_ << ", process_group_ is "
              << process_group_ << ", has " << dag_conf_list_.size() << " dag conf";
        // 打印所有模块的依赖关系
        for (std::string &dag : dag_conf_list_)
        {
          AINFO << "dag_conf: " << dag;
        }
      }

      void ModuleArgument::GetOptions(const int argc, char *const argv[])
      {
        opterr = 0; // extern int opterr
        int long_index = 0;
        const std::string short_opts = "hd:p:s:";
        static const struct option long_opts[] = {
            {"help", no_argument, nullptr, 'h'},
            {"dag_conf", required_argument, nullptr, 'd'},
            {"process_name", required_argument, nullptr, 'p'},
            {"sched_name", required_argument, nullptr, 's'},
            {NULL, no_argument, nullptr, 0}};

        // log command for info
        std::string cmd("");
        for (int i = 0; i < argc; ++i)
        {
          cmd += argv[i];
          cmd += " ";
        }
        AINFO << "command: " << cmd;

        do
        {
          int opt =
              getopt_long(argc, argv, short_opts.c_str(), long_opts, &long_index);
          if (opt == -1)
          {
            break;
          }
          switch (opt)
          {
          case 'd':
            dag_conf_list_.emplace_back(std::string(optarg));
            for (int i = optind; i < argc; i++)
            {
              if (*argv[i] != '-')
              {
                dag_conf_list_.emplace_back(std::string(argv[i]));
              }
              else
              {
                break;
              }
            }
            break;
          case 'p':
            process_group_ = std::string(optarg);
            break;
          case 's':
            sched_name_ = std::string(optarg);
            break;
          case 'h':
            DisplayUsage();
            exit(0);
          default:
            break;
          }
        } while (true);
      }

    } // namespace mainboard
  }   // namespace cyber
} // namespace apollo
