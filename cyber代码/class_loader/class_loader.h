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
#ifndef CYBER_CLASS_LOADER_CLASS_LOADER_H_
#define CYBER_CLASS_LOADER_CLASS_LOADER_H_

#include <algorithm>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "cyber/class_loader/class_loader_register_macro.h"

namespace apollo
{
  namespace cyber
  {
    namespace class_loader
    {

      /**
 *  for library load,createclass object
 */
      class ClassLoader
      {
      public:
        explicit ClassLoader(const std::string &library_path);
        virtual ~ClassLoader();

        // 动态库是否已经加载
        bool IsLibraryLoaded();
        // 加载动态库
        bool LoadLibrary();
        // 卸载动态库
        int UnloadLibrary();
        // 获取动态库路径
        const std::string GetLibraryPath() const;
        template <typename Base>
        // 获取有效的类名称
        std::vector<std::string> GetValidClassNames();
        template <typename Base>
        // 创建对象
        std::shared_ptr<Base> CreateClassObj(const std::string &class_name);
        template <typename Base>
        // 类是否有效
        bool IsClassValid(const std::string &class_name);

      private:
        // 当类删除
        template <typename Base>
        void OnClassObjDeleter(Base *obj);

      private:
        // 类的路径
        std::string library_path_;
        // 类加载引用次数
        int loadlib_ref_count_;
        // 类加载引用次数锁
        std::mutex loadlib_ref_count_mutex_;
        // 类引用次数
        int classobj_ref_count_;
        // 类引用次数锁
        std::mutex classobj_ref_count_mutex_;
      };

      // 获取classloader中加载的类的集合
      template <typename Base>
      std::vector<std::string> ClassLoader::GetValidClassNames()
      {
        return (utility::GetValidClassNames<Base>(this));
      }

      // 查找类是否加载
      template <typename Base>
      bool ClassLoader::IsClassValid(const std::string &class_name)
      {
        std::vector<std::string> valid_classes = GetValidClassNames<Base>();
        return (std::find(valid_classes.begin(), valid_classes.end(), class_name) !=
                valid_classes.end());
      }

      //  根据类名称创建对象，并且返回对象指针，注意创建对象的过程中classobj_ref_count_加1，释放对象之后减1，
      // 通过计数器表明类加载器是否还存在引用关系，而不会释放掉
      template <typename Base>
      std::shared_ptr<Base> ClassLoader::CreateClassObj(
          const std::string &class_name)
      {
        if (!IsLibraryLoaded())
        {
          // 加载动态库
          LoadLibrary();
        }

        // 创建对象  // 根据类名称创建对象
        Base *class_object = utility::CreateClassObj<Base>(class_name, this);
        if (nullptr == class_object)
        {
          AWARN << "CreateClassObj failed, ensure class has been registered. "
                << "classname: " << class_name << ",lib: " << GetLibraryPath();
          return std::shared_ptr<Base>();
        }

        std::lock_guard<std::mutex> lck(classobj_ref_count_mutex_);
        // 类引用计数加1
        classobj_ref_count_ = classobj_ref_count_ + 1;
        // 构造智能指针，并且指定删除器  // 指定类的析构函数
        // 可以看到创建类的时候，类引用计数加1，并且绑定类的析构函数(OnClassObjDeleter)，删除对象的时候让类引用计数减1
        std::shared_ptr<Base> classObjSharePtr(
            class_object, std::bind(&ClassLoader::OnClassObjDeleter<Base>, this,
                                    std::placeholders::_1));
        return classObjSharePtr;
      }

      template <typename Base>
      void ClassLoader::OnClassObjDeleter(Base *obj)
      {
        if (nullptr == obj)
        {
          return;
        }

        std::lock_guard<std::mutex> lck(classobj_ref_count_mutex_);
        delete obj;
        classobj_ref_count_ = classobj_ref_count_ - 1;
      }

    } // namespace class_loader
  }   // namespace cyber
} // namespace apollo
#endif // CYBER_CLASS_LOADER_CLASS_LOADER_H_
