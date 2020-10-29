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

#include "cyber/common/global_data.h"
#include "cyber/common/log.h"
#include "cyber/init.h"
#include "cyber/mainboard/module_argument.h"   // "mainboard.cc"引用
#include "cyber/mainboard/module_controller.h" // "mainboard.cc"引用
#include "cyber/state.h"

#include "gflags/gflags.h"

using apollo::cyber::mainboard::ModuleArgument;
using apollo::cyber::mainboard::ModuleController;

int main(int argc, char **argv)
{
  google::SetUsageMessage("we use this program to load dag and run user apps.");

  // parse the argument   // 解析模块参数
  ModuleArgument module_args;
  // 解析模块参数 解析模块参数在"module_argument.h"和"module_argument.cc"中的"ModuleArgument"类中
  module_args.ParseArgument(argc, argv);

  // initialize cyber   // 初始化cyber
  // 初始化cyber 初始化cyber就是cyber目录下的"init.h"和"init.cc"中
  apollo::cyber::Init(argv[0]);

  // start module    // 启动模块  // 加载模块
  ModuleController controller(module_args);
  if (!controller.Init())
  {
    controller.Clear();
    AERROR << "module start error.";
    return -1;
  }

  // 等待cyber关闭
  apollo::cyber::WaitForShutdown();
  // 卸载模块
  controller.Clear();
  AINFO << "exit mainboard.";

  return 0;
}
